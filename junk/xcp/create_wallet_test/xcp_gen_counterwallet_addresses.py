# Generate Counterwallet keys from passphrase. Python 3.
#
# Adapted from:
#
# https://github.com/rippler/CounterWalletHelper/blob/master/CounterWalletHelper.py
# by Nelson Castillo
# with help from Andres Velasquez (<velasquez.af@gmail.com>).
#
# Please go to https://github.com/rippler/CounterWalletHelper for the original Python 2 code.

from word_list import mnemonic_word_list

from binascii import unhexlify
import bitcoin
from Crypto import Random
import math
import struct

def _mMod(a, b):
    return int(a  - math.floor(a/b)*b)

def _toHex(s):
    seed = ''
    for i in s:
        seed += ('0'*8 + format(i, 'x'))[-8:]
    return seed

def _makeSeed(phrase, words=None):
    if words is None:
        words = mnemonic_word_list
    n = len(words)
    s = []
    for i in range(4):
        w1 = words.index(phrase[3*i])
        w2 = words.index(phrase[3*i + 1])
        w3 = words.index(phrase[3*i + 2])
        s.append(w1 + n*_mMod(w2-w1, n) + n*n*_mMod(w3 - w2, n))
    return s

def get_keys(pass_phrase, depth, testnet):
    step1 = _makeSeed(pass_phrase)
    step2 = _toHex(step1)
    step4 = unhexlify(step2)
    vbytes = vbytes = bitcoin.TESTNET_PRIVATE if testnet else bitcoin.MAINNET_PRIVATE
    master = bitcoin.bip32_master_key(step4, vbytes)
    external = bitcoin.bip32_ckd(master, 2**31 + 0) # hardened
    external = bitcoin.bip32_ckd(external, 0)
    ret = []
    for idx in range(depth):
        priv_32 = bitcoin.bip32_ckd(external, idx)
        priv = bitcoin.bip32_extract_key(priv_32)
        pub = bitcoin.privtopub(priv)
        ret.append(bitcoin.pubtoaddr(pub, 111 if testnet else 0))
    return ret

def random_passphrase():
    words = []
    for _ in range(12):
        idx = struct.unpack("<L", Random.get_random_bytes(4))[0] % len(mnemonic_word_list)
        words.append(mnemonic_word_list[idx])
    return words

def demo_generate(testnet):
    words = random_passphrase()
    network = 'testnet' if testnet else 'mainnet'
    print('Generating wallet for {}. passphrase: {}'.format(network, ' '.join(words)))
    print(get_keys(words, 1, testnet))

def main():
    demo_generate(testnet=False)
    demo_generate(testnet=True)

if __name__ == '__main__':
    main()
