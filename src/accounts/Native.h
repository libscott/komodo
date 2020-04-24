
#pragma once

#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
// #include <libdevcrypto/Hash.h>
// #include <libdevcrypto/Common.h>
// #include <libwingcore/Common.h>
#include <accounts/State.h>

using namespace std;
using namespace dev;
using namespace dev::eth;


namespace dev
{

namespace kmd
{

class NativeVM
{
public:
    virtual bool init(bytesConstRef data) = 0;

protected:
    Account m_account;
    bytes m_output;
};

class DummyVM : public NativeVM
{
public:
    bool init(bytesConstRef data) {
        return true;
    }
};


// Address const RootContractAddress = Address("0300000000000000000000000000000000000000");
// Address const AssetContractAddress = Address("0301000000000000000000000000000000000000");
// 
// int const PartitionNativeContracts = 1;
// 
// /*
//  * Storage
//  */
// 
// //ETH_SIMPLE_EXCEPTION_VM(InvalidConversion);
// 
// 
// u256 as256(int _i);
// u256 as256(std::string _s);
// 
// 
// std::string from256(u256 _h);
// 
// 
// template <typename K>
// u256 keyOn(int _partition, K _key)
// {
//     RLPStream rlp;
//     rlp.appendList(2);
//     rlp << _partition << _key;
//     auto h = sha3(rlp.out());
//     return u256(h);
// }
// 
// class NativeStorage
// {
// public:
//     NativeStorage(Address _addr, State& _state): m_addr(_addr), m_s(_state) {}
//     //template <typename T, typename F>
//     //T get(int _p, F _k) const {
//     //    u256s s = getWords(keyOn(_p, _k));
//     //    return from256s(words); }
// 
//     //template <typename K, typename V>
//     //void put(int _p, K _k, V _v) { return m_s.setStorage(m_addr, keyOn(_p, _k), as256(_v)); }
// 
//     u256 getWord(u256 const& _key) const { return m_s.storage(m_addr, _key); }
//     void putWord(u256 const& _key, u256 const& _val) { m_s.setStorage(m_addr, _key, _val); }
// 
//     template <typename V> size_t getData(u256 _key, V& out) const;
//     template <typename V> void putData(u256 _key, V const& in);
// 
//     template <typename V> std::string get(int _p, V const* _k)
//     {
//         std::string out;
//         getData(keyOn(_p, _k), out);
//         return out;
//     }
// 
//     std::string toJsonMap() const;  /// WARNING: This only returns information in the account storage overlay, so it won't include committed data from the state
// private:
//     Address m_addr;
//     State& m_s;
// };
// 
// 
// class NativeManager
// {
// public:
//     NativeManager(State& _s): m_s(_s), m_storage(RootContractAddress, m_s) {};
//     bool isNativeContract(Address const& _addr) const;
// 
//     owning_bytes_ref callVM(Address const& _addr, u256& io_gas, ExtVM& _ext, OnOpFunc const& _onOp);
//     std::unique_ptr<VMFace> vmFor(Address const& _addr) const;
// private:
//     enum Partitions
//     {
//         VMs = 0
//     };
//     State& m_s;
//     NativeStorage m_storage;
// };





}
}
