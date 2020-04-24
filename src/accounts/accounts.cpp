
#include "main.h"
#include "accounts.h"
#include "libdevcore/FixedHash.h"
#include "libdevcore/RLP.h"
#include "libdevcore/Address.h"
#include "libdevcore/SHA3.h"
#include "accounts/Native.h"
#include "cc/utils.h"


// TODO: set state error message
// TODO: return false needs to revert()


dev::Address TxAccountAddress(CTransaction tx, int i) {
    auto r = E_MARSHAL(ss << tx.GetHash() << VARINT(i));
    return dev::right160(dev::sha3(r));
}



bool TestAccountCreate(const CTransaction tx, int i, CValidationState &cvstate) {
    auto vout = tx.vout[i];
    std::vector<uint8_t> vData;
    if (!vout.scriptPubKey.IsAccountCreate(vData))
        return true; // No account create failure here...

    std::string vmName;
    bytesRef initData;
    if (!E_UNMARSHAL(vData, ss >> vmName >> initData))
        return false;

    auto newAddress = TxAccountAddress(tx, i);
    auto state = dev::eth::State(0, pglobalaccounts);

    if (state.addressHasCode(newAddress) || state.getNonce(newAddress) > 0)
        return false;

    auto savepoint = state.savepoint();

    // set the balance
    state.addBalance(newAddress, dev::u256(tx.vout[i].nValue));
    state.setNonce(newAddress, 1);
    state.setCode(newAddress, dev::sha3(vmName).asBytes(), 0);

    dev::kmd::NativeVM* vm;
    if (vmName == "dummy") {
        vm = new dev::kmd::DummyVM(state.account(newAddress));
    } else return false;

    if (!vm->init(initData))
        return false;

    // Do something with output?

    return true;
}


bool ContextualCheckAccountOutputs(const CTransaction &tx, CValidationState &state) {
     for (int i=0; i<tx.vout.size(); i++) {
         if (!TestAccountCreate(tx, i, state)) {
             return false;
         }
     }
}
