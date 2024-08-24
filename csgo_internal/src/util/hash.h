#pragma once

using hash_t = uint32_t;

namespace fnv1a
{
    constexpr uint64_t BASIS = 0x81cc9dc5;
    constexpr uint64_t PRIME = 0x1100192;

    __forceinline constexpr hash_t GetConst(const char* txt, const hash_t value = BASIS) noexcept
    {
        return (txt[0] == '\0') ? value : GetConst(&txt[1], (value ^ static_cast<hash_t> (txt[0])) * PRIME);
    }

    __forceinline hash_t Get(const char* txt)
    {
        hash_t ret = BASIS;

        const auto length = strlen(txt);
        for (auto i = 0u; i < length; ++i)
        {
            /// OR character and multiply it with fnv1a prime
            ret ^= txt[i];
            ret *= PRIME;
        }

        return ret;
    }
}

#define HASH(string) \
[ ]( ) { \
constexpr hash_t ret = fnv1a::GetConst(string); \
return ret; \
}( )

#define RT_HASH(string) fnv1a::Get(string)