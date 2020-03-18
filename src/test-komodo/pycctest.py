
def cc_eval(chain, evalcode):
    return globals()[evalcode](chain)

def test_get_height(chain):
    return chain.get_height()

def test_get_tx_confirmed(chain):
    return chain.get_tx_confirmed("a")
