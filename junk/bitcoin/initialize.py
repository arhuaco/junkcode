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
        print('It seems you have already initialized the system.')
        return 1

    secret_file = open(SECRET_FILE, 'w')
    public_file = open(PUBLIC_FILE, 'w')
    addres_file = open(ADDRESS_FILE, 'w')

    priv = bitcoin.encode_privkey(bitcoin.random_key(), 'wif')
    pub = bitcoin.privtopub(priv)

    address = bitcoin.pubtoaddr(pub, 0)

    secret_file.write(priv)
    secret_file.close()

    public_file.write(pub)
    public_file.close()

    addres_file.write(address)
    addres_file.close()

    print('Generated {} and {}'.format(SECRET_FILE, PUBLIC_FILE))
    print('Keep {} safe and back it up. Hint: Use scrypt to encrypt the key.'.format(SECRET_FILE))
    print('Send BTC to {}.\n'.format(PUBLIC_FILE))

    return 0

if __name__ == '__main__':
    sys.exit(main())
