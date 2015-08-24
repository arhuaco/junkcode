''' Debug multisig signingg issue. Defensive programming... Speed is not in mind! '''

import bitcoin
import re
import pprint

def get_raw_tx():
    ''' The raw TX we need to sign. Verify here: https://blockchain.info/decode-tx '''
    #return '01000000010550a87771eeaa9b0a3dd5f3cf9cdb3bb0fb3b3efd9b210c1c96053c543a1b4800000000c952410427db4059d24bab05df3f6bcc768fb01bd976b973f93e72cce2dfbfbed5a32056c9040a2c2ea4c10c812a54fed7ff2e6a917dbc843362d398f6ace4000fafa5c641043e12a6cb1c7c156f789110abf8397b714047414b5a32c742f17ccf93ff23bdf3128f946207086bcef012558240cd16182c741123e93ed18327c4cd6ebac668a94104e4168c172283c7dfaa85d2004f763a28bf6d0f1602fc1452ccec62a7c8a66e422af1410fbf24a47355ddc43dfe3491cb1b806574ccd1c434680466dcff926f0153aeffffffff0336150000000000001976a91451bb89eb8fcb138174114315fc8eaf054395c0cc88ac781e000000000000895121031c9456b7dfef9e2bfcec93f5efd3799bd925a536803a6218f05150f8848d71b921035f799fdbb42b47989107d627b5a56b4654afae218d9767c4a7310159a79a1ddc410427db4059d24bab05df3f6bcc768fb01bd976b973f93e72cce2dfbfbed5a32056c9040a2c2ea4c10c812a54fed7ff2e6a917dbc843362d398f6ace4000fafa5c653ae721a000000000000c952410427db4059d24bab05df3f6bcc768fb01bd976b973f93e72cce2dfbfbed5a32056c9040a2c2ea4c10c812a54fed7ff2e6a917dbc843362d398f6ace4000fafa5c641043e12a6cb1c7c156f789110abf8397b714047414b5a32c742f17ccf93ff23bdf3128f946207086bcef012558240cd16182c741123e93ed18327c4cd6ebac668a94104e4168c172283c7dfaa85d2004f763a28bf6d0f1602fc1452ccec62a7c8a66e422af1410fbf24a47355ddc43dfe3491cb1b806574ccd1c434680466dcff926f0153ae00000000' # pylint: disable=line-too-long
    return '0100000001caa1cd766de01294451f1610a0a0136bc9176070f1c22494eb2e28443624aa8e00000000c952410427db4059d24bab05df3f6bcc768fb01bd976b973f93e72cce2dfbfbed5a32056c9040a2c2ea4c10c812a54fed7ff2e6a917dbc843362d398f6ace4000fafa5c641043e12a6cb1c7c156f789110abf8397b714047414b5a32c742f17ccf93ff23bdf3128f946207086bcef012558240cd16182c741123e93ed18327c4cd6ebac668a94104e4168c172283c7dfaa85d2004f763a28bf6d0f1602fc1452ccec62a7c8a66e422af1410fbf24a47355ddc43dfe3491cb1b806574ccd1c434680466dcff926f0153aeffffffff0336150000000000001976a91451bb89eb8fcb138174114315fc8eaf054395c0cc88ac781e00000000000089512103ff58dcadeb5a447162ab629d309c2a28043a3437884b7b280fcfff9baa3119c021024131478633f18da1caec8ee455108b72feb19d70baae9ca86638661e47562bfa410427db4059d24bab05df3f6bcc768fb01bd976b973f93e72cce2dfbfbed5a32056c9040a2c2ea4c10c812a54fed7ff2e6a917dbc843362d398f6ace4000fafa5c653ae9268000000000000c952410427db4059d24bab05df3f6bcc768fb01bd976b973f93e72cce2dfbfbed5a32056c9040a2c2ea4c10c812a54fed7ff2e6a917dbc843362d398f6ace4000fafa5c641043e12a6cb1c7c156f789110abf8397b714047414b5a32c742f17ccf93ff23bdf3128f946207086bcef012558240cd16182c741123e93ed18327c4cd6ebac668a94104e4168c172283c7dfaa85d2004f763a28bf6d0f1602fc1452ccec62a7c8a66e422af1410fbf24a47355ddc43dfe3491cb1b806574ccd1c434680466dcff926f0153ae00000000'

def deserialize_tx(txn):
    ''' deserialize the TX. '''
    assert looks_like_hex(txn)
    return bitcoin.deserialize(txn)

def looks_like_hex(token):
    ''' Sanity check. '''
    try:
        return re.match('^[0-9a-fA-F]*$', token) and \
               isinstance(int(token, 16), int)
    except: # pylint: disable=bare-except
        return False

def check_priv_key_looks_good(hex_priv):
    ''' Check that we don't screw up in a trivial way. '''
    # get_privkey_format returns 'hex' if length == 64.
    assert len(hex_priv) == 51
    # Check that the string converts to hex.
    #assert looks_like_hex(hex_priv)

def priv2pub(hex_priv):
    ''' get the public key. '''
    check_priv_key_looks_good(hex_priv)
    return bitcoin.privtopub(hex_priv)

def fix_priv(priv):
    ''' Use this code from prod. '''
    return bitcoin.encode_privkey(priv, 'wif', 0)

def main():
    ''' Our main function. '''

    # Two of the three keys. The missing one:
    # 3347122612f83ad80517dfef41c8b6a6400eef1949df732ce60a4e7c9d00ccb0
    priv_keys = [
        fix_priv('93d716a849c4c215b3049670ef7c1eba9b19c0452caf37b8f3383dcdd5577440'),
        fix_priv('7b389eb21fb17ef293be410009d436a495e0942005b1e6845f64429a5ff13420'),
    ]

    # Same order that Counterparty uses.  Order should not matter.
    pub_0 = priv2pub(fix_priv('3347122612f83ad80517dfef41c8b6a6400eef1949df732ce60a4e7c9d00ccb0'))
    pub_1 = priv2pub(priv_keys[0])
    pub_2 = priv2pub(priv_keys[1])

    script_not_real = bitcoin.mk_multisig_script(pub_0, pub_1, pub_2, 2, 3)

    for priv in priv_keys:
        check_priv_key_looks_good(priv)

    deserialized_txn = deserialize_tx(get_raw_tx())

    print('The raw transaction: {}\n'.format(get_raw_tx()))

    print('The decoded transaction:\n{}\n'.format(pprint.pformat(deserialized_txn)))


    bitcoind_script = '52410427db4059d24bab05df3f6bcc768fb01bd976b973f93e72cce2dfbfbed5a32056c9040a2c2ea4c10c812a54fed7ff2e6a917dbc843362d398f6ace4000fafa5c641043e12a6cb1c7c156f789110abf8397b714047414b5a32c742f17ccf93ff23bdf3128f946207086bcef012558240cd16182c741123e93ed18327c4cd6ebac668a94104e4168c172283c7dfaa85d2004f763a28bf6d0f1602fc1452ccec62a7c8a66e422af1410fbf24a47355ddc43dfe3491cb1b806574ccd1c434680466dcff926f0153ae'

    # You might want to use _ for something.
    signatures_A = []
    signatures_B = []
    for tx_index, tx_input in enumerate(deserialized_txn['ins']):
        assert tx_index == 0 # Only once!
        script_real = tx_input['script']
        assert script_real == bitcoind_script
        assert len(script_real) == len(script_not_real)
        signatures_A.append(bitcoin.multisign(get_raw_tx(), tx_index, script_real, priv_keys[0]))
        signatures_B.append(bitcoin.multisign(get_raw_tx(), tx_index, script_real, priv_keys[1]))
    fully_signed_tx = get_raw_tx()
    fully_signed_tx_before = fully_signed_tx
    for tx_index, tx_input in enumerate(deserialized_txn['ins']):
        assert tx_index == 0 # Only once!
        fully_signed_tx = bitcoin.apply_multisignatures( \
            fully_signed_tx,
            tx_index,
            script_real,
            signatures_A[tx_index],
            signatures_B[tx_index])

    assert fully_signed_tx_before != fully_signed_tx

    # may the bytes be with you
    print('Fully signed: {}'.format(fully_signed_tx))

if __name__ == '__main__':
    main()
