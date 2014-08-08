#!/usr/bin/env python

from __future__ import absolute_import, division, print_function, unicode_literals

import select
import socket
import sys

MAX_RECV_LEN = 4096

class Socket(object):

    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.is_ok = False

    def make_nonblocking(self):
        self.sock.setblocking(0)

    def connect(self, host, port):
        try:
            self.sock.connect((host, port))
        except socket.error as error:
            print('Socket.connect got exception: {}'.format(error),
                file=sys.stderr)
            return False
        self.is_ok = True
        return True

    def fileno(self):
        return self.sock.fileno()

    def is_ok(self):
        """ Retuns true if the socket is open and ready. """
        return self.is_ok

    def close(self):
        self.is_ok = False
        try:
            self.sock.shutdown()
            self.sock.close()
        except socket.error as error:
            print('Socket.shutdown/close got exception: {}'.format(error),
                file=sys.stderr)
            return False
        return True

    def sendall(self, msg, timeout=None):
        self.sock.settimeout(timeout)
        try:
            return self.sock.sendall(msg) == None
        except socket.error as error:
            print('Socket.sendall got exception: {}'.format(error),
                file=sys.stderr)
            self.is_ok = False
        return False


    def receive(self, max_len, timeout=None):
        self.sock.settimeout(timeout)
        received = ''
        while len(received) < max_len:
            len_before = len(received)
            try:
                received += self.sock.recv(min(max_len - len(received), MAX_RECV_LEN))
            except socket.error as error:
                print('Socket.recv got exception: {}'.format(error),
                    file=sys.stderr)
                assert self.is_ok
                self.is_ok = error.args[0] != errno.EWOULDBLOCK # TODO: Test.
            if len(received) == len_before:
                # When the socket has an error condtion the first time we
                # might return true but the next call should fail.
                self.is_ok = False
                break
        return len(received) > 0, received

class JsonSocketReader(object):
    def __init__(self, sock, max_json_size=1024*1024):
        self.sock = sock
        self.last_chunk = ''
        self.json_buff = ''
        self.n_open = 0 # Number of '{' chars open.
        self.max_json_reply_size = max_json_size

    def read_more(self):
        if len(self.last_chunk) == 0:
            allowed_read_size  = self. max_json_reply_size - len(self.json_buff)
            assert allowed_read_size >= 0
            status, self.last_chunk = self.sock.receive(allowed_read_size)
            if not status:
                return False
        else:
            self.json_buff = ''
        
        for idx in range(len(self.last_chunk)):
            if self.last_chunk[idx] == '{':
                self.n_open += 1
            elif self.last_chunk[idx] == '}':
                self.n_open -= 1
                assert self.n_open >= 0
            self.json_buff += self.last_chunk[idx]
            if self.n_open == 0 and max(len(self.json_buff), idx) > 0: # {...}
                 self.last_chunk = self.last_chunk[idx + 1:]
                 return True
        self.last_chunk = ''
        return False

    @staticmethod
    def wait_for_json_reply(readers,  timeout = 0):
        readers_map = {}
        # If we kept this dict we would not have to build it everytime.
        # It is not an issue if there are just a few readers.
        readers_sock = []
        for reader in readers:
            readers_sock.append(reader.sock.fileno())
            readers_map[reader.sock.fileno()] = reader
        try:
            readers_ready, _, _ = select.select(readers_sock, [], [], timeout)
            for reader in [readers_map[fd] for fd in readers_ready]:
                if reader.read_more():
                    print('Got json:', reader.json_buff, file=sys.stderr)
                    sys.exit(0)

            #return True, [readers_map[reader] for reader in readers_ready]
        except socket.error as error:
            print('Socket.select got exception: {}'.format(error),
                file=sys.stderr)
            return False
        return True, []


def main():
    sock = Socket()
    if not sock.connect('localhost', 2020):
        return 1
    sock.make_nonblocking()
    reader = JsonSocketReader(sock)
    while True:
      status, ready_json = JsonSocketReader.wait_for_json_reply([reader], timeout=1)
      if status:
         if len(ready_json):
           for ready in [reader. ready_json]:
             print('Got json:'.format(ready.json_buff), file=sys.stderr)
      print('Waiting', file=sys.stderr)
    return 0

sys.exit(main())
