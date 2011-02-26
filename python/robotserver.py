#!/usr/bin/python


import asyncore
import socket
import asynchat
import struct
import sys
import re
import pickle
import mysettings
import time

import socketlib
import crc16
ROBOTSERVER_PORT = 8080
ROBOT_PORT = 5331

robot_sock = []
cmds = {
'error':            "\xFF", 
'pwm_set':          "\x10", 
'pwm_setscalar':    "\x11", 
'rpm':              "\x20",
'rpmset':           "\x21", 
'get_rpm':          "\x21", 
'invalid_cmd':      "\x03", 
'debug':            "\x30",
'echo':             "\x31",
'status':           "\x32"}

robot_status = {}

cmds_invert = dict()
for k,v in cmds.iteritems():
    cmds_invert[v] = k


class RobotConnection(socketlib.Socket):
    def __init__(self, user_sock):
        socketlib.Socket.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connect( ('localhost', ROBOT_PORT) )
        self.address = "\x01"
        self.set_terminator("\x00\xFF\xFF\xFF\xFF")
        self.user_sock = user_sock
        self.open = True
        
    def write(self, data):
        data = translate_to_robot(data)
        if data == False:
            self.user_sock.write("Invalid command or parameters")
            return
        checksum = crc16.crc16(self.address + data)
        checksum = struct.pack(">H", checksum)
        data = self.address + data + checksum
        #print "send", repr(data)
        socketlib.Socket.write(self, data)
        
                
    def handle_request(self, req):
        
        #print "req", repr(req)
        if len(req) < 3: return
        data = req[0:-2]
        if req[1] == cmds.get('debug') :
            debug_handler(data)
            return        
        translated = translate_from_robot(data)        
        if translated == False: return
        checksum = struct.unpack(">H", req[-2:])[0]
        if crc16.crc16(data) != checksum:
            translated = "CHECKSUM MISMATCH: " + repr(req)
            return

        if self.user_sock.is_open():
            self.user_sock.write(translated)

            
    def handle_close(self):
        if self.user_sock.is_open():
            self.user_sock.write("Robot microcontroller unresponsive, disconnecting...")
            self.user_sock.close()
        socketlib.Socket.handle_close(self)
        
        
    #def handle_error(self):
    #    err = "Exception in RobotConnection: " + str(sys.exc_info()[:2])
    #    self.user_sock.write(err)
    #    print err
    
class UserConnection(socketlib.Socket):
    
    def __init__(self, sock, *args):
        socketlib.Socket.__init__(self, sock)
        self.open = True
        self.robot_sock = args[0]
        self.set_terminator("\r\n")
        
    def handle_request(self, req):
        if req == '':
            return
        self.refresh_robot_sock()
        if (req == 'get_status'): return self.write("robot_status" + pickle.dumps(robot_status))                   
        self.robot_sock[0].write(req)       
        
        
    def refresh_robot_sock(self):
        if len(self.robot_sock) < 1 or self.robot_sock[0].is_open() == False:
            self.robot_sock = [RobotConnection(self)]
    def handle_error(self):
        err = "Exception in UserConnection: " + str(sys.exc_info()[:2])
        self.write(err)
        print err
    
    def handle_close(self):
        if len(self.robot_sock) >  0 and self.robot_sock[0].is_open() == True:
            self.robot_sock[0].close()
        socketlib.Socket.handle_close(self)
        

def translate_from_robot(packet):
    res = str(ord(packet[0])) + ','
    cmd = str(cmds_invert.get(packet[1]))
    if handlers.get(cmd, False) != False:
        is_res = handlers.get(cmd)(packet[2:])
        if is_res == False:
            return False
        else: 
            res += is_res
    else:
        res += str(cmds_invert.get(packet[1]))
        for element in packet[2:]:
            res += "," + str(ord(element))
    return res
        
    
def translate_to_robot(packet):
    
    tokens = packet.split(',')
    human_cmd = tokens[0].strip()
    cmd = cmds.get(human_cmd, "None")
    if cmd == "None": return False
    args = tokens[1:]
    out = str(cmd)
    if handlers.get(human_cmd, False) != False:
        return out + handlers.get(human_cmd)(args)
    
    for arg in args:
        arg = arg.split(':')
        if len(arg) == 1:
            bytes = "1"
            val = arg[0].strip()
        else:
            bytes = arg[0].strip()
            val = arg[1].strip()       
        
        if is_valid_arg(bytes) == False: return False
        if is_valid_arg(val) == False: return False
        
        out += pack(int(bytes), long(val))        
    return out

def is_valid_arg(arg):
    return not bool(re.search('[^0-9]', arg))


def pack(bytes, number):
    res = struct.pack(">L", number)
    return res[4-bytes:]


def rpm_handler(data):
    return "RPM: " + str( struct.unpack(">H", data[:2])[0]) + str( struct.unpack(">H", data[2:])[0])

def rpmset_handler(args):
    return struct.pack('>H', int(args[0].strip()) + 10000) + struct.pack('>H', int(args[1].strip()) + 10000)
    
def debug_handler(data):
    print "DEBUG " + str(data[2]),
    value = data[3:]
    if len(value) == 2:
        print str(struct.unpack(">H", data[3:])[0])
    elif len(value) == 4:
        print str(struct.unpack(">L", data[3:])[0])
    else:
        print "Unknown Data Type"

def status_handler(data):
    if len(data) != 7: return False
    params = []
    params.append(str(struct.unpack(">H", data[1:3])[0]))
    params.append(str(struct.unpack(">H", data[3:5])[0]))
    params.append(str(struct.unpack(">H", data[5:7])[0]))
    robot_status[data[0:1]] = params
    return False

        
    
handlers = {
'rpm': rpm_handler,
'rpmset': rpmset_handler,
'status': status_handler}

class RobotServer(socketlib.Server):

    def __init__(self, host, port):
        connected = False
	while connected == False:
            try:
                socketlib.Server.__init__(self, host, port)
		connected = True
            except:
                print "Unable to bind to",host,port
                time.sleep(1)
                pass
        
            

    def handle_error(self):
        err = "Exception in UserConnection: " + str(sys.exc_info()[:2])
        self.write(err)
        print err


    
robot_server = RobotServer(mysettings.ROBOTSERVER_HOST, ROBOTSERVER_PORT)
robot_server.set_handler(UserConnection)
robot_server.set_handler_args(robot_sock)

asyncore.loop()
