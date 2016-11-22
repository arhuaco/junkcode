''' Initialize bitcoin keys. Intended for offline storage. '''

import os.path
import sys

import bitcoin

SECRET_FILE = 'secret.txt'
PUBLIC_FILE = 'public.txt'
ADDRESS_FILE = 'address.txt'

def main():
    ''' Our main function. '''

    if os.path.isfile(SECRET_FILE):
        print('It seems you have already created the keys.')
        return 1

    priv = bitcoin.encode_privkey(bitcoin.random_key(), 'wif')
    with open(SECRET_FILE, 'w') as secret_file:
        secret_file.write(priv)

    pub = bitcoin.privtopub(priv)
    with open(PUBLIC_FILE, 'w') as public_file:
        public_file.write(pub)

    address = bitcoin.pubtoaddr(pub, 0)
    with open(ADDRESS_FILE, 'w') as addres_file:
        addres_file.write(address)

    print('Generated {} and {}'.format(SECRET_FILE, PUBLIC_FILE))
    print('Keep {} safe and back it up. Hint: Use scrypt to encrypt the key.'.format(SECRET_FILE))
    print('Send BTC to {}'.format(address))

    return 0

if __name__ == '__main__':
    sys.exit(main())
