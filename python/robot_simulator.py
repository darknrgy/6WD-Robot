#robot simulator
import asyncore
import socket
import asynchat
import socketlib
import struct


class RobotSimulatorConnection(socketlib.Socket):

    def __init__(self, sock, *args):
        socketlib.Socket.__init__(self, sock)
        self.open = True
        self.set_terminator("\x00\xFF\xFF\xFF\xFF")
        
    def handle_request(self, req):
        print "recv:", repr(req)
        self.write('\xfa2M0.\x00\x00\x00\x00k\x82')
        self.write('\xfa2L\x00\x00\x00\x08\x00\x00\x9f\xfe')
        self.write('\xfa2R\x00\x00\x00\x13\x00\x00\x99p')

        
        
    def write(self, data):
        print "send:", repr(data)
        socketlib.Socket.write(self, data)

robot_server = socketlib.Server('localhost', 5331)
robot_server.set_handler(RobotSimulatorConnection)
asyncore.loop()