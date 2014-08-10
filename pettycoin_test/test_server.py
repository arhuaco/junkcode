#!/usr/bin/env python

from __future__ import absolute_import, division, print_function, unicode_literals
if sys.version_info  < (3,0):
    range = xrange

import json
import SocketServer
import random
import sys
import time

MAX_LEN = 1024 * 10

def make_random_json(max_depth=3, max_keys=3):
    ret = {}
    for idx in range(random.randint(1, max_keys)):
      assert max_depth - 1 >= 0
      next_depth = random.randint(0, max_depth - 1)
      if next_depth > 0:
        ret[str(idx)] = make_random_json(next_depth)
      else:
        ret[str(idx)] = idx
    return ret

class TestingServer(SocketServer.BaseRequestHandler):

    def handle(self):
        time.sleep(2)
        for i in range(10):
            to_send = json.dumps(make_random_json())
            print(to_send, file=sys.stderr)
            self.request.sendall(to_send)

if __name__ == "__main__":
    SocketServer.TCPServer.allow_reuse_address = True
    server = SocketServer.TCPServer(('localhost', 2020), TestingServer)
    server.serve_forever()
