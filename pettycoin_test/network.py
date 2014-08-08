#!/usr/bin/env python

from __future__ import print_function

import socket
import sys

class Socket(object):

    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def connect(self, host, port):
        try:
            self.sock.connect((host, port))
        except socket.error as error:
            print('Socket.connect got exception: {}'.format(error),
                file=sys.stderr)
            return False
        return True

    def sendall(self, msg, timeout=None):
        # TODO(nel): Do error checking.
        self.sock.settimeout(timeout)
        self.sock.sendall(msg)

    def receive(self, max_len, timeout=None):
        self.sock.settimeout(timeout)
        received = ''
        bytes_recd = 0
        while bytes_recd < max_len:
            chunk = self.sock.recv(min(max_len - bytes_recd, max_len))
            if len(chunk) == 0:
                break
            received += chunk
            bytes_recd = bytes_recd + len(chunk)
        return len(received) > 0, received

def main():
    socket = Socket()
    if not socket.connect('127.0.0.1', 2020):
      print('Could not open connection.', file=sys.stderr)
      return 1
      
    socket.sendall('hola')
    status, received = socket.receive(max_len=1024, timeout=1)
    if status:
      print('received:', received)
    else:
      print('I did not receive anything')

    return 0

if __name__ == "__main__":
    sys.exit(main())
