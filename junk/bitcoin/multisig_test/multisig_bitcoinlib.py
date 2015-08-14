''' Trying to correctly spend a transaction that needs 2 of 3 sigs. '''

from bitcoin import SelectParams
from bitcoin.core import (CMutableTransaction, CMutableTxIn, CMutableTxOut,
                          lx, COutPoint, COIN, b2x)
from bitcoin.core.script import SignatureHash, SIGHASH_ALL, CScript, CScriptOp
from bitcoin.core.scripteval import VerifyScript, SCRIPT_VERIFY_P2SH
from bitcoin.wallet import CBitcoinSecret

PRIV_HEX_1 = '93d716a849c4c215b3049670ef7c1eba9b19c0452caf37b8f3383dcdd5577440'
PRIV_HEX_2 = '7b389eb21fb17ef293be410009d436a495e0942005b1e6845f64429a5ff13420'

def main():
    ''' Our main function. '''

    SelectParams('mainnet')

    decoded_transaction = eval(open('transaction_to_sign.txt').read()) # pylint: disable=eval-used

    txin_txid = lx(decoded_transaction['vin'][0]['txid'])
    txin_vout = 0
    tx_in = CMutableTxIn(COutPoint(txin_txid, txin_vout))

    tx_out = []
    for idx in range(len(decoded_transaction['vout'])):
        satoshis = int(COIN * decoded_transaction['vout'][idx]['value'])
        script_pub_key = CScript(bytes.fromhex(decoded_transaction['vout'][idx]['scriptPubKey']['hex']))
        tx_out.append(CMutableTxOut(satoshis, script_pub_key))

    tx_to_spend = CMutableTransaction([tx_in], tx_out)

    priv_1 = CBitcoinSecret.from_secret_bytes(bytes.fromhex(PRIV_HEX_1))
    priv_2 = CBitcoinSecret.from_secret_bytes(bytes.fromhex(PRIV_HEX_2))

    txin_redeem_script = CScript(bytes.fromhex(decoded_transaction['vin'][0]['scriptSig']['hex']))
    # Input 0 is fixed.
    sighash = SignatureHash(txin_redeem_script, tx_to_spend, 0, SIGHASH_ALL)
    signatures = []
    for priv in [priv_1, priv_2]:
        signatures.append(priv.sign(sighash) + bytes([SIGHASH_ALL]))

    tx_in.scriptSig = CScript([CScriptOp(0x00), signatures[0], signatures[1], txin_redeem_script])

    # script_pub_key Defined in cycle.
    VerifyScript(tx_in.scriptSig, txin_redeem_script, tx_to_spend, 0, (SCRIPT_VERIFY_P2SH,))

    print(b2x(tx_to_spend.serialize()))

if __name__ == '__main__':
    main()
