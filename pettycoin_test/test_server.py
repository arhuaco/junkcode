#!/usr/bin/env python

import SocketServer

class TestingServer(SocketServer.BaseRequestHandler):
    """ Only for testing. """

    def handle(self):
        self.data = self.request.recv(2048).strip()
        print "{} wrote:".format(self.client_address[0])
        print self.data
        self.request.sendall(self.data.upper())

if __name__ == "__main__":
    server = SocketServer.TCPServer(('localhost', 2020), TestingServer)
    server.serve_forever()
