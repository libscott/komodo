import sys
import time
import json
import logging
import binascii
import struct
import base64
from testsupport import *


SCRIPT_FALSE = 'Script evaluated without error but finished with a false/empty top stack element'


@fanout_input(0)
def test_basic_spend(inp):
    spend = {'inputs': [inp], "outputs": [nospend]}
    spend_txid = submit(sign(spend))
    assert rpc.getrawtransaction(spend_txid)


@fanout_input(1)
def test_fulfillment_wrong_signature(inp):
    # Set other pubkey and sign
    inp['script']['fulfillment']['publicKey'] = bob_pk
    spend = {'inputs': [inp], 'outputs': [nospend]}
    signed = sign(spend)

    # Set the correct pubkey, signature is bob's
    signed['inputs'][0]['script']['fulfillment']['publicKey'] = alice_pk

    try:
        assert not submit(signed), 'should raise an error'
    except RPCError as e:
        assert SCRIPT_FALSE in str(e), str(e)


@fanout_input(2)
def test_fulfillment_wrong_pubkey(inp):
    spend = {'inputs': [inp], 'outputs': [nospend]}
    signed = sign(spend)

    # Set the wrong pubkey, signature is correct
    signed['inputs'][0]['script']['fulfillment']['publicKey'] = bob_pk

    try:
        assert not submit(signed), 'should raise an error'
    except RPCError as e:
        assert SCRIPT_FALSE in str(e), str(e)


@fanout_input(3)
def test_invalid_fulfillment_binary(inp):
    # Create a valid script with an invalid fulfillment payload
    inp['script'] = binascii.hexlify(b"\007invalid").decode('utf-8')
    spend = {'inputs': [inp], 'outputs': [nospend]}

    try:
        assert not submit(spend), 'should raise an error'
    except RPCError as e:
        assert 'Crypto-Condition payload is invalid' in str(e), str(e)


@fanout_input(4)
def test_invalid_condition(inp):
    # Create a valid output script with an invalid cryptocondition binary
    outputscript = to_hex(b"\007invalid\xcc")
    spend = {'inputs': [inp], 'outputs': [{'amount': 1000, 'script': outputscript}]}
    spend_txid = submit(sign(spend))

    spend1 = {
        'inputs': [{'txid': spend_txid, 'idx': 0, 'script': {'fulfillment': cond_alice}}],
        'outputs': [nospend],
    }

    try:
        assert not submit(sign(spend1)), 'should raise an error'
    except RPCError as e:
        assert SCRIPT_FALSE in str(e), str(e)


@fanout_input(5)
def test_oversize_fulfillment(inp):
    # Create oversize fulfillment script where the total length is <2000
    binscript = b'\x4d%s%s' % (struct.pack('h', 2000), b'a' * 2000)
    inp['script'] = to_hex(binscript)
    spend = {'inputs': [inp], 'outputs': [nospend]}

    try:
        assert not submit(spend), 'should raise an error'
    except RPCError as e:
        assert 'scriptsig-size' in str(e), str(e)


@fanout_input(6)
def test_eval_basic(inp):
    eval_cond = {
        'type': 'eval-sha-256',
        'method': 'testEval',
        'params': encode_base64('testEval')
    }

    # Setup some eval outputs
    spend0 = {
        'inputs': [inp],
        'outputs': [
            {'amount': 500, 'script': {'condition': eval_cond}},
            {'amount': 500, 'script': {'condition': eval_cond}}
        ]
    }
    spend0_txid = submit(sign(spend0))
    assert rpc.getrawtransaction(spend0_txid)

    # Test a good fulfillment
    spend1 = {
        'inputs': [{'txid': spend0_txid, 'idx': 0, 'script': {'fulfillment': eval_cond}}],
        'outputs': [{'amount': 500, 'script': {'condition': eval_cond}}]
    }
    spend1_txid = submit(sign(spend1))
    assert rpc.getrawtransaction(spend1_txid)

    # Test a bad fulfillment
    eval_cond['params'] = ''
    spend2 = {
        'inputs': [{'txid': spend0_txid, 'idx': 1, 'script': {'fulfillment': eval_cond}}],
        'outputs': [{'amount': 500, 'script': {'condition': eval_cond}}]
    }
    try:
        assert not submit(sign(spend2)), 'should raise an error'
    except RPCError as e:
        assert SCRIPT_FALSE in str(e), str(e)


@fanout_input(7)
def test_secp256k1_condition(inp):
    ec_cond = {
        'type': 'secp256k1-sha-256',
        'publicKey': notary_pk
    }

    # Create some secp256k1 outputs
    spend0 = {
        'inputs': [inp],
        'outputs': [
            {'amount': 500, 'script': {'condition': ec_cond}},
            {'amount': 500, 'script': {'condition': ec_cond}}
        ]
    }
    spend0_txid = submit(sign(spend0))
    assert rpc.getrawtransaction(spend0_txid)

    # Test a good fulfillment
    spend1 = {
        'inputs': [{'txid': spend0_txid, 'idx': 0, 'script': {'fulfillment': ec_cond}}],
        'outputs': [{'amount': 500, 'script': {'condition': ec_cond}}]
    }
    spend1_txid = submit(sign(spend1))
    assert rpc.getrawtransaction(spend1_txid)

    # Test a bad fulfillment
    spend2 = {
        'inputs': [{'txid': spend0_txid, 'idx': 1, 'script': {'fulfillment': ec_cond}}],
        'outputs': [{'amount': 500, 'script': {'condition': ec_cond}}]
    }
    signed = sign(spend2)
    signed['inputs'][0]['script']['fulfillment']['publicKey'] = \
            '0275cef12fc5c49be64f5aab3d1fbba08cd7b0d02908b5112fbd8504218d14bc7d'
    try:
        assert not submit(signed), 'should raise an error'
    except RPCError as e:
        assert SCRIPT_FALSE in str(e), str(e)


@fanout_input(20)
def test_eval_replacement(inp):
    eval_cond = {
        'type': 'eval-sha-256',
        'method': 'testReplace',
        'params': '',
    }

    # Setup replaceable output
    spend0 = {
        'inputs': [inp],
        'outputs': [
            {'amount': 1000, 'script': {'condition': eval_cond}},
        ]
    }
    spend0_txid = submit(sign(spend0))
    assert rpc.getrawtransaction(spend0_txid)

    b64_1 = 'AQ=='
    spend1 = {
        'inputs': [{'txid': spend0_txid, 'idx': 0, 'script': {'fulfillment': eval_cond}}],
        'outputs': [{'amount': 1000, 'script': {'op_return': b64_1}}]
    }
    
    b64_2 = 'Ag=='
    spend2 = {
        'inputs': [{'txid': spend0_txid, 'idx': 0, 'script': {'fulfillment': eval_cond}}],
        'outputs': [{'amount': 1000, 'script': {'op_return': b64_2}}]
    }

    # If spend2 is already registered, return true, as this test has already been performed
    spend2_txid = hoek.encodeTx(sign(spend2))['txid']
    try:
        rpc.getrawtransaction(spend2_txid)
        return
    except RPCError:
        pass

    # Send replaceable
    spend1_txid = submit(sign(spend1))
    assert rpc.getrawtransaction(spend1_txid)


    # Send replacement (higher OP_RETURN data)
    spend2_txid = submit(sign(spend2))
    assert rpc.getrawtransaction(spend2_txid)
    
    # Now the first transaction has gone
    try:
        assert not rpc.getrawtransaction(spend1_txid), "should raise an error"
    except RPCError as e:
        assert 'No information available about transaction' in str(e), str(e)


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    for name, f in globals().items():
        if name.startswith('test_'):
            logging.info("Running test: %s" % name)
            f()
            logging.info("Test OK: %s" % name)