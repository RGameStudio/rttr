#pragma once

#include "rttr/detail/base/core_prerequisites.h"

#include <cstdint>
#include <string_view>

//-- FNV1a c++11 constexpr compile time hash functions, 32 and 64 bit str should be a null terminated string literal,
//-- value should be left out e.g hash_32_fnv1a_const("example")
//-- post: https://notes.underscorediscovery.com/constexpr-fnv1a/

namespace rttr
{
namespace detail
{

constexpr uint64_t val_64_const	= 0xcbf29ce484222325;
constexpr uint64_t prime_64_const = 0x100000001b3;

//----------------------------------------------------------------------------------------------------------------------
inline constexpr uint64_t hash_64_fnv1a_const(const char* const str, size_t length, const uint64_t value = val_64_const) noexcept
{
	return (length == 0) ? value : hash_64_fnv1a_const(&str[1], length - 1, (value ^ static_cast<uint64_t>(str[0])) * prime_64_const);
}

} // end namespace detail

inline constexpr uint64_t hash_string(std::string_view str) noexcept
{
	return detail::hash_64_fnv1a_const(str.data(), str.size());
}

} // end namespace rttr
