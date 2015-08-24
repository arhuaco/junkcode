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
import math

def mMod(a, b):
    return int(a  - math.floor(a/b)*b)

def makeSeed(phrase, words=None):
    if words is None:
        words = mnemonic_word_list
    n = len(words)
    s = []
    for i in range(4):
        w1 = words.index(phrase[3*i])
        w2 = words.index(phrase[3*i + 1])
        w3 = words.index(phrase[3*i + 2])
        s.append(w1 + n*mMod(w2-w1, n) + n*n*mMod(w3 - w2, n))
    return s

def GetKeys(pass_phrase, depth):
    step1 = makeSeed(pass_phrase)
    step2 = toHex(step1)
    step4 = unhexlify(step2)
    master = bitcoin.bip32_master_key(step4)
    external = bitcoin.bip32_ckd(master, 2**31 + 0) # hardened
    external = bitcoin.bip32_ckd(external, 0)
    ret = []
    for idx in range(depth):
        priv_32 = bitcoin.bip32_ckd(external, idx)
        priv = bitcoin.bip32_extract_key(priv_32)
        pub = bitcoin.privtopub(priv)
        ret.append(bitcoin.pubtoaddr(pub))
    return ret

print(' '.join(mnemonic_word_list[:12]))
print(GetKeys(mnemonic_word_list[:12], 2))
