
#include <memory>
#include <json/json.h>
#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Hash.h>
#include <libdevcrypto/Common.h>
#include <libevm/ExtVMFace.h>
#include <libwingcore/Common.h>
#include "Native.h"

using namespace dev;
using namespace dev::eth;
using namespace dev::wing;

namespace dev
{

namespace wing
{

    
/*
 * NativeStorage
 */

u256 as256(std::string _s)
{
    if (_s.size() > 32)
        BOOST_THROW_EXCEPTION(ValueTooLarge() << errinfo_comment("NativeStorage string > 32b"));
    h256 out;
    memcpy(out.data(), _s.data(), _s.size());
    return u256(out);
}

std::string from256(u256 _h) {
    std::string s = toBigEndianString(_h);
    size_t c = s.find('\0');
    if (c != std::string::npos) s.resize(c);
    return s;
}


std::string NativeStorage::toJsonMap() const
{
    Json::Value r(Json::objectValue);

    auto a = m_s.account(m_addr);

    if (a)
    {
        for (auto const& i : a->storageOverlay())
            r[toCompactHexPrefixed(i.first, 1)] = toCompactHexPrefixed(i.second, 1);
    }
    std::ostringstream ss;
    ss << r;
    return ss.str();
}


static size_t memcpyr(void* _to, void const* _from, size_t _maxSize)
{
    memcpy(_to, _from, _maxSize);
    return _maxSize;
}

/*
 * Read a buffer from a sequential array of words.
 * The first 2 bytes of the first word specify the buffer length.
 */
template <typename V>
size_t NativeStorage::getData(u256 _key, V& _out) const
{
    u256 first = getWord(_key);
    size_t len = static_cast<size_t>(first >> 240);
    if (!len) return 0;
    _out.resize(len);

    h256 h(first);
    size_t read = memcpyr(&_out[0], h.data()+2, std::min(len, (size_t)30));
    
    while (read < len)
    {
        h = h256(getWord(++_key));
        read += memcpyr(&_out[read], h.data(),
                        std::min(len-read, (size_t)32));
    }
    return len;
}

/*
 * Write up to 8190b (256 words - 2 length bytes)
 * bytes to an sequence of storage locations
 *
 * XXX: The reason this is hardcoded is that it's indended to become part of
 * standard VM config in the future. VM config is held in root address.
 */
template <typename V>
void NativeStorage::putData(u256 _key, V const& _in)
{
    size_t len = _in.size();
    if (len > 8190)
        BOOST_THROW_EXCEPTION(ValueTooLarge() << errinfo_comment("NativeStorage putData max size is 8190"));

    h256 h(static_cast<u256>(len) << 240);

    size_t written = memcpyr(&h[2], _in.data(), std::min(len, (size_t)30));
    putWord(_key, u256(h));

    while (written < len)
    {
        h.clear();
        written += memcpyr(&h[0], _in.data()+written,
                           std::min(len-written, (size_t)32));
        putWord(++_key, u256(h));
    }
}

/* Instantiations */
template size_t NativeStorage::getData<std::string>(u256, std::string&) const;
template size_t NativeStorage::getData<bytes>(u256, bytes&) const;
template void NativeStorage::putData<std::string>(u256, std::string const&);
template void NativeStorage::putData<bytes>(u256, bytes const&);

}
}
