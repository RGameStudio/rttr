#pragma once

/************************************************************************************
*                                                                                   *
*   Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>                           *
*                                                                                   *
*   This file is part of RTTR (Run Time Type Reflection)                            *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining           *
*   a copy of this software and associated documentation files (the "Software"),    *
*   to deal in the Software without restriction, including without limitation       *
*   the rights to use, copy, modify, merge, publish, distribute, sublicense,        *
*   and/or sell copies of the Software, and to permit persons to whom the           *
*   Software is furnished to do so, subject to the following conditions:            *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#include "rttr/detail/base/core_prerequisites.h"

#include <array>
#include <string_view>
#include <utility>

namespace rttr::detail
{

/////////////////////////////////////////////////////////////////////////////////

#if RTTR_COMPILER == RTTR_COMPILER_MSVC
#   define RTTR_FUNCTION __FUNCSIG__
#   define RTTR_TYPE_NAME_FUNC_NAME_HEAD "auto __cdecl rttr::detail::get_type_name_array<"
#   define RTTR_TYPE_NAME_FUNC_NAME_TAIL ">(void)"
#elif RTTR_COMPILER == RTTR_COMPILER_GNUC
#   define RTTR_FUNCTION __PRETTY_FUNCTION__
#   define RTTR_TYPE_NAME_FUNC_NAME_HEAD "constexpr auto rttr::detail::get_type_name_array() [with T = "
#   define RTTR_TYPE_NAME_FUNC_NAME_TAIL "]"
#elif RTTR_COMPILER == RTTR_COMPILER_CLANG || RTTR_COMPILER == RTTR_COMPILER_APPLECLANG
#   define RTTR_FUNCTION __PRETTY_FUNCTION__
#   define RTTR_TYPE_NAME_FUNC_NAME_HEAD "auto rttr::detail::get_type_name_array() [T = "
#   define RTTR_TYPE_NAME_FUNC_NAME_TAIL "]"
#else
#   error This compiler does not support extracting a function signature via preprocessor!
#endif

/////////////////////////////////////////////////////////////////////////////////

RTTR_STATIC_CONSTEXPR std::size_t skip_size_at_begin = sizeof(RTTR_TYPE_NAME_FUNC_NAME_HEAD) - 1;
RTTR_STATIC_CONSTEXPR std::size_t skip_size_at_end   = sizeof(RTTR_TYPE_NAME_FUNC_NAME_TAIL) - 1;

/////////////////////////////////////////////////////////////////////////////////

template <size_t N, std::size_t...Idxs>
RTTR_STATIC_CONSTEXPR auto cut_array(std::array<char, N> arr, std::index_sequence<Idxs...>)
{
    return std::array{arr[Idxs]...};
}

/////////////////////////////////////////////////////////////////////////////////

template <size_t N>
RTTR_STATIC_CONSTEXPR auto format_string(std::string_view type_name) {
    constexpr std::array<std::string_view, 3>prefixes{"class ", "struct ", "enum "};
    size_t prefix_size = 0;
    for (const auto prefix: prefixes)
    {
        if (type_name.find(prefix) == 0)
        {
            prefix_size = prefix.size();
            break;
        }
    }

    std::array<char, N> res = {};
    size_t wr_pos = 0;
    for (size_t i = prefix_size; i < type_name.size(); ++i)
    {
        if (type_name[i] == ' ' && i > prefix_size && type_name[i - 1] == ',')
        {
            continue;
        }
        if (type_name[i] == '>' && i > prefix_size && type_name[i - 1] == ' ')
        {
            res[wr_pos - 1] = type_name[i];
            continue;
        }
        res[wr_pos++] = type_name[i];
    }
    return std::pair(res, wr_pos);
}

/////////////////////////////////////////////////////////////////////////////////

template<typename T>
RTTR_STATIC_CONSTEXPR auto get_type_name_array()
{
    constexpr auto sig = std::string_view(RTTR_FUNCTION);
    constexpr auto name = sig.substr(skip_size_at_begin, sig.size() - skip_size_at_begin - skip_size_at_end);
    constexpr auto name_arr = format_string<name.size()>(name);

    return cut_array(name_arr.first, std::make_index_sequence<name_arr.second>{});
}

/////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct type_name_holder
{
    static inline constexpr auto value = get_type_name_array<T>();
};

/////////////////////////////////////////////////////////////////////////////////

template <typename T>
RTTR_STATIC_CONSTEXPR std::string_view get_type_name() RTTR_NOEXCEPT
{
    constexpr auto & value = type_name_holder<T>::value;
    return std::string_view{value.data(), value.size()};
}

/////////////////////////////////////////////////////////////////////////////////

} // end namespace rttr::detail
