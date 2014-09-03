'''
  Read a JSON from a socket stream.

  TODO:

    - Does not work when the JSON has a '{' in a string. Fix.

'''

import errno
import select
import socket
import sys
import time

MAX_RECV_LEN = 4096

class Socket:
    ''' Just a socket wrapper. Is there anything better I should be using? '''

    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.is_ok = False

    def make_nonblocking(self):
        ''' Make the socket nonblocking. '''
        self.sock.setblocking(0)

    def connect(self, host, port):
        ''' Connect the socket. '''
        try:
            self.sock.connect((host, port))
        except socket.error as error:
            print('Socket.connect got exception: {}'.format(error),
                  file=sys.stderr)
            return False
        self.is_ok = True
        return True

    def fileno(self):
        '''' Return the file number of this socket. '''
        return self.sock.fileno()

    def is_healthy(self):
        ''' Retuns true if the socket is open and ready. '''
        return self.is_ok

    def close(self):
        ''' Close the socket. '''
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
        ''' Send with timeout. If timeout is None, write can block. '''
        self.sock.settimeout(timeout)
        try:
            return self.sock.sendall(msg) == None
        except socket.error as error:
            print('Socket.sendall got exception: {}'.format(error),
                  file=sys.stderr)
            self.is_ok = error.args[0] != errno.EWOULDBLOCK
        return False

    def receive(self, max_len, timeout=None):
        ''' Receive from socket. If timeout is None, read can block. '''
        self.sock.settimeout(timeout)
        received = bytes()
        while len(received) < max_len:
            len_before = len(received)
            try:
                received += self.sock.recv(min(max_len -
                                               len(received), MAX_RECV_LEN))
            except socket.error as error:
                print('Socket.recv got exception: {}'.format(error),
                      file=sys.stderr)
                assert self.is_ok
                self.is_ok = error.args[0] != errno.EWOULDBLOCK
            if len(received) == len_before:
                # When the socket has an error condtion the first time we
                # might return true but the next call should fail.
                if len(received) == 0:
                    self.is_ok = False
                break
        return len(received) > 0, received

class JsonSocketReader:
    ''' Read JSON strings from a noblocking socket.
        The length of the JSON string is not known in advance. '''
    def __init__(self, sock, max_json_size=1024*1024):
        self.sock = sock
        self.last_chunk = bytes()
        self.json_buff = ''
        self.json_ready = ''
        self.n_open = 0 # Number of '{' chars open.
        self.max_json_reply_size = max_json_size

    def should_read_more(self):
        ''' Is the socket healthy? Can we read more? '''
        return self.sock.is_healthy()

    def match_next(self):
        ''' Try to match a JSON string. '''
        if len(self.last_chunk) == 0:
            # Mixing bytes and unicode concepts here. Not an issue, we hope.
            #len(bytes('á', 'utf-8')) == 2, len('á') == 1.
            allowed_read_size = self. max_json_reply_size - len(self.json_buff)
            assert allowed_read_size >= 0
            status, self.last_chunk = self.sock.receive(allowed_read_size)
            if not status:
                return False

        try:
            self.last_chunk = bytes.decode(self.last_chunk)
        except UnicodeDecodeError:
            print('Unicode decode error. Recovering...', file=sys.stderr)
            # We probably read an incomplete character. Let's keep reading.
            return False

        for idx in range(len(self.last_chunk)):
            if self.last_chunk[idx] == '{':
                self.n_open += 1
            elif self.last_chunk[idx] == '}':
                self.n_open -= 1
                assert self.n_open >= 0 # malformed JSON will break this assert.
            self.json_buff += self.last_chunk[idx]
            if self.n_open == 0 and max(len(self.json_buff), idx) > 0: # {...}
                # Inneficient if we expect multiple JSON answers in a reply.
                self.last_chunk = bytes(self.last_chunk[idx + 1:], 'utf-8')
                self.json_ready = self.json_buff
                self.json_buff = ''
                return True
        self.last_chunk = bytes()
        return False

    def get_json(self):
        ''' Get the matched json. '''
        return self.json_ready

    def wait_for_json(self, timeout=0):
        ''' Try to match a JSON sting with the given timeout. '''
        try:
            while True:
                time_before = time.time()
                readers_ready, _, _ = select.select([self.sock.fileno()],
                                                    [], [], timeout)
                if len(readers_ready) > 0:
                    if self.match_next():
                        return True
                timeout -= time.time() - time_before
                if timeout <= 0.0:
                    print('Timeout in wait_for_json.', file=sys.stderr)
                    return False
        except socket.error as error:
            print('Socket exception: {}'.format(error), file=sys.stderr)
            return False

def main():
    ''' Our main function. '''
    sock = Socket()
    if not sock.connect('localhost', 2020):
        return 1
    sock.make_nonblocking()
    reader = JsonSocketReader(sock)
    while reader.should_read_more():
        if reader.wait_for_json(timeout=0.1):
            print('Got json:{}'.format(reader.get_json()), file=sys.stderr)
    return 0

sys.exit(main())
