
#include <memory>
#include <json/json.h>
#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Hash.h>
#include <libdevcrypto/Common.h>
#include <libevm/ExtVMFace.h>
#include <libwingcore/Common.h>
#include "Asset.h"
#include "Native.h"

using namespace dev;
using namespace dev::eth;
using namespace dev::wing;

namespace dev
{

namespace wing
{


// TODO: Rename to hasActiveVM
bool NativeManager::isNativeContract(Address const& _addr) const
{
    NativeStorage store(RootContractAddress, m_s);
    std::string vmName;
    return store.getData(keyOn(PartitionNativeContracts, _addr), vmName);
}
 
owning_bytes_ref NativeManager::callVM(Address const& _addr, u256& io_gas, ExtVM& _ext, OnOpFunc const& _onOp)
{
    //if (_addr == RootContractAddress)
    //    return std::unique_ptr<VMFace>(new RootContractVM());
    
    std::string vmName;
    if (m_storage.getData(keyOn(1, _addr), vmName)) {
        if (vmName == "asset")
            return AssetVM(io_gas, _ext, _onOp).exec();
    }

    auto ec = std::make_error_code(std::errc::invalid_seek);
    std::string what = "Native VM not found: " + vmName;
    BOOST_THROW_EXCEPTION(std::system_error(ec, what));
}


}
}
