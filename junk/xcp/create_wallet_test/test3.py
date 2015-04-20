from word_list import mnemonic_word_list

from binascii import unhexlify
import bitcoin
import hashlib
import hmac
import math

def mMod(a, b): 
    return int(a  - math.floor(a/b)*b) 

def toHex(s):
    seed = '' 
    for i in s: 
        seed += ('0'*8 + format(i, 'x'))[-8:]
    return seed 

def makeSeed(phrase, words=mnemonic_word_list):
    n = len(mnemonic_word_list) 
    s = [] 
    for i in range(4):  # len(phrase) / 3 
        w1 = words.index(phrase[3*i])
        w2 = words.index(phrase[3*i + 1])
        w3 = words.index(phrase[3*i + 2])
        s.append(w1 + n*mMod(w2-w1, n) + n*n*mMod(w3 - w2, n)) 
    return s

def createWallet(entropy):
    I = hmac.new("Bitcoin seed", entropy, hashlib.sha512).digest()
    Il, Ir = I[:32], I[32:]
    key = BIP32Key(secret=Il, chain=Ir, depth=0, index=0, fpr='\0\0\0\0', public=False)
    return key

'''
def getAddressAccount(key, n): 
    AccountZero = key.ChildKey(BIP32_HARDEN)
    ExternalAccount = AccountZero.ChildKey(0)
    address_list = [] 
    for i in range(n):
        AddressAccount = ExternalAccount.ChildKey(i)
        x = {'address': AddressAccount.Address()} 
        #x['private key'] = AddressAccount.PrivateKey().encode('hex')
        address_list.append(x) 
    return address_list 
'''

def openWallet(pass_phrase, depth):
    step1 = makeSeed(pass_phrase)
    step2 = toHex(step1)
    step4 = unhexlify(step2) 
    #wallet = createWallet(step4) 
    
    master = bitcoin.bip32_master_key(step4)
    external = bitcoin.bip32_ckd(master, 2**31 + 0)
    ret = []
    for idx in range(depth):
      priv_32_0 = bitcoin.bip32_ckd(external, idx)
      priv_0 = bitcoin.bip32_extract_key(priv_32_0)
      pub = bitcoin.privtopub(priv_0)
      ret.append(bitcoin.pubtoaddr(pub))
    return ret
    
    #address_list = getAddressAccount(wallet, depth)
    #return address_list

print(' '.join(mnemonic_word_list[:12]))
print(openWallet(mnemonic_word_list[:12], 2))
