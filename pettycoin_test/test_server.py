''' Server used for testing.  '''

import json
import socketserver
import sys
import time

import json_generator

MAX_LEN = 1024 * 10
HOST = 'localhost'
PORT = 2020

class TestingRequestHandler(socketserver.BaseRequestHandler):
    '''' The request handler. '''
    def handle(self):
        time.sleep(0.3)
        for _ in range(20):
            to_send = json.dumps(json_generator.make_random_json_like_hash())
            print(to_send, file=sys.stderr)
            self.request.sendall(bytes(to_send, 'utf-8'))

def main():
    ''' Our main function. '''
    socketserver.TCPServer.allow_reuse_address = True
    server = socketserver.TCPServer((HOST, PORT), TestingRequestHandler)
    server.serve_forever()
    return 0

if __name__ == "__main__":
    sys.exit(main())
