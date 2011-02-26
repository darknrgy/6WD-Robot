import asyncore
import socket
import asynchat
import struct
import sys

import socketlib

ROBOT_PORT = 5331

class RobotConnection(socketlib.Socket):
    def __init__(self):
        socketlib.Socket.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connect( ('localhost', ROBOT_PORT) )
        self.set_terminator("\x00\xFF\xFF\xFF\xFF")
        self.open = True
        
    def write(self, data):
        print "send", repr(data)
        socketlib.Socket.write(self, data)
        
                
    def handle_request(self, req):
        print "rcv", len(req), repr(req)


robot_server = RobotConnection()
asyncore.loop()
