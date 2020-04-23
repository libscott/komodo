
#include "main.h"
#include "accounts.h"



bool TestAccountCreate(const CTxOut vout, CValidationState &cvstate) {
    std::vector<uint8_t> vData;
    if (!vout.scriptPubKey.IsAccountCreate(vData))
        return true;

    auto state = dev::eth::State(0, pglobalaccounts);
    auto addr = dev::ZeroAddress;

    return !state.addressInUse(addr);
}


bool ContextualCheckAccountOutputs(const CTransaction &tx, CValidationState &state) {
     for (int i=0; i<tx.vout.size(); i++) {
         if (!TestAccountCreate(tx.vout[i], state)) {
             return false;
         }
     }
}
