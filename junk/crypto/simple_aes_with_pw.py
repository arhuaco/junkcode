# Code based on this S.O. question:
#
# http://stackoverflow.com/questions/14179784/python-encrypting-with-pycrypto-aes
#
# I need to store a key in disk and I want something better than clear text.
#
# TODO(nel): Migrate from sha256 to scrypt hash when it is available in
# Debian repositories.
#
# Python 2.X.

from base64 import b64encode, b64decode
from Crypto.Cipher import AES
import hashlib
import os

def _h256(k):
    return hashlib.sha256(k).digest()

def _add_padding(data):
    length = 16 - (len(data) % 16)
    return data + chr(length) * length

def _remove_padding(data):
    return data[:-ord(data[-1])]

def encrypt(data, key):
    data = _add_padding(data) # Padding so we can use AES with CBC.
    iv = os.urandom(16) # urandom. I know.
    return b64encode(iv + AES.new(_h256(_h256(key)), AES.MODE_CBC, iv).encrypt(data))

def decrypt(data, key):
    iv = b64decode(data)[:16]
    clear = AES.new(_h256(_h256(key)), AES.MODE_CBC, iv).decrypt(b64decode(data)[16:])
    return _remove_padding(clear)

def main():
    # Just an example.
    import getpass
    data = encrypt('hola', getpass.getpass())
    print decrypt(data, getpass.getpass())

if __name__ == "__main__":
    main()
