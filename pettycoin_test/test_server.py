#!/usr/bin/env python

from __future__ import print_function

import SocketServer
import sys
import time

MAX_LEN = 1024 * 10

class TestingServer(SocketServer.BaseRequestHandler):
    """ Only for testing. Not very tested :-) """

    def handle(self):
        time.sleep(2)
        assert(self.request.sendall('{}') == None)

if __name__ == "__main__":
    SocketServer.TCPServer.allow_reuse_address = True
    server = SocketServer.TCPServer(('localhost', 2020), TestingServer)
    server.serve_forever()
