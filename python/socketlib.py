import asyncore
import socket
import asynchat
import sys

class Socket(asynchat.async_chat):

    def __init__(self, sock = False, *args):
        if sock != False:
            asynchat.async_chat.__init__(self, sock=sock)
        else:
            asynchat.async_chat.__init__(self)
        self.ibuffer = ""
        self.set_terminator("\n")
        self.handling = False
        self.open = False
        
    def write(self, data):
        self.push(data + self.get_terminator())
        
    def collect_incoming_data(self, data):
        """Buffer the data"""
        self.ibuffer += data   

    def found_terminator(self):
        
        read = self.ibuffer
        self.ibuffer = ""
        self.handle_request(read)  

    def handle_request(self, read):
        print read
        
    def handle_close(self):
        self.open = False
        print "Connection closed"
        self.close()
        
    def is_open(self):
        return self.open
        

class Server(asyncore.dispatcher):

    def __init__(self, host, port):
        asyncore.dispatcher.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.set_reuse_addr()
        self.bind((host, port))
        self.listen(5)
        self.handler = Socket
        self.handler_args = ()
        
    def set_handler(self, handler):
        self.handler = handler
        
    def set_handler_args(self, *args):
        self.handler_args = args
        

    def handle_accept(self):
        pair = self.accept()
        if pair is None:
            pass
        else:
            sock, addr = pair
            print 'Incoming connection from %s' % repr(addr)
            self.handler(sock, *self.handler_args)
