'''

Fetcher for HTTP and HTTPS servers.

I coded this a while ago and I've been reusing this code.

Let's make this the central location to gather improvements.

'''

import json
import logging
import requests
import sys

from requests.auth import HTTPBasicAuth

def basic_auth(user, password):
    ''' Basic auth so that other modules do not require requests.  '''
    return HTTPBasicAuth(user, password)

class Fetcher:
    ''' Fetcher implementation. '''

    def __init__(self, server, auth=None, method='get'):
        self.server = server
        self.ret = None
        self.status = 0
        assert method in ['get', 'post']
        self.method = method
        self.auth = auth

    def fetch(self, headers=None, params=None, data=None, timeout=5, #  pylint: disable=too-many-arguments
              parse_json=False, verbose=False):
        ''' Perform the HTTP/HTTPS fetch. Using a timeout. '''

        self.ret = None

        try:
            if verbose:
                print('Requesting:{} with params {}'.format(self.server,
                                                            params),
                      file=sys.stderr)
            method = requests.post if self.method == 'post' else requests.get
            response = method(self.server, params=params, data=data,
                              timeout=timeout, headers=headers, auth=self.auth)
            self.status = response.status_code
            self.ret = response.content
            if verbose:
                print('Fetcher got HTTP status:{}. Response:{}'.
                      format(self.status, self.ret), file=sys.stderr)
            if response.status_code == 200:
                if parse_json:
                    self.ret = json.loads(response.content.decode('utf-8'))
                return True
            return False
        except Exception as ex: # pylint: disable=broad-except
            self.ret = ''
            self.status = 0
            logging.error('Fetcher got exception:%s, url:%s',
                          repr(ex),
                          self.server)
        return False

    def last_fetch(self):
        ''' Return the most recent fetched result. Might be string or json. '''
        return self.ret

    def last_status(self):
        ''' Return the most recent HTTP status. '''
        return self.status

def main():
    ''' Main function. Used to demonstrate usage. '''
    fetcher = Fetcher(server='https://blockchain.info/tx-index/61447157')
    if fetcher.fetch(params={'format' : 'json'}):
        print(fetcher.last_fetch(), file=sys.stderr)
        return 0
    else:
        print('Fetch failed', file=sys.stderr)
        return 1

if __name__ == '__main__':
    sys.exit(main())
