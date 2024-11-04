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

#include <rttr/registration>
#include <rttr/variant.h>

#include <vector>
#include <map>
#include <list>
#include <set>
#include <string>
#include <type_traits>
#include <cstdint>

namespace
{

template<typename T, bool Signed = std::is_signed_v<T>, size_t Size = sizeof(T)>
struct IntRegister;

#define DECLARE_INT_REGISTER(Signed, Size, Name) \
template<typename T> \
struct IntRegister<T, Signed, Size> \
{ \
    IntRegister() \
    { \
        RTTR_REGISTRATION_STANDARD_TYPE_VARIANTS(T) \
        rttr::registration::class_<T>(Name); \
    } \
};

DECLARE_INT_REGISTER(true,  2, "int16")
DECLARE_INT_REGISTER(false, 2, "uint16")
DECLARE_INT_REGISTER(true,  4, "int32")
DECLARE_INT_REGISTER(false, 4, "uint32")
DECLARE_INT_REGISTER(true,  8, "int64")
DECLARE_INT_REGISTER(false, 8, "uint64")

}

// explicit instantiation of std::string needed, otherwise we get a linker error with clang on osx
// thats a bug in libc++, because of interaction with __attribute__ ((__visibility__("hidden"), __always_inline__)) in std::string
template class std::basic_string<char>;

RTTR_REGISTRATION
{
    using namespace rttr;

    type::get<std::nullptr_t>();

    RTTR_REGISTRATION_STANDARD_TYPE_VARIANTS(void)
    RTTR_REGISTRATION_STANDARD_TYPE_VARIANTS(rttr::type)
    RTTR_REGISTRATION_STANDARD_TYPE_VARIANTS(bool)
    RTTR_REGISTRATION_STANDARD_TYPE_VARIANTS(signed char)
    RTTR_REGISTRATION_STANDARD_TYPE_VARIANTS(unsigned char)
    RTTR_REGISTRATION_STANDARD_TYPE_VARIANTS(char)
    RTTR_REGISTRATION_STANDARD_TYPE_VARIANTS(wchar_t)
    RTTR_REGISTRATION_STANDARD_TYPE_VARIANTS(float)
    RTTR_REGISTRATION_STANDARD_TYPE_VARIANTS(double)
    RTTR_REGISTRATION_STANDARD_TYPE_VARIANTS(long double)
    RTTR_REGISTRATION_STANDARD_TYPE_VARIANTS(std::string)

    IntRegister<std::int16_t>();
    IntRegister<std::uint16_t>();
    IntRegister<std::int32_t>();
    IntRegister<std::uint32_t>();
    IntRegister<std::int64_t>();
    IntRegister<std::uint64_t>();

    IntRegister<short int>();
    IntRegister<unsigned short int>();
    IntRegister<int>();
    IntRegister<unsigned int>();
    IntRegister<long int>();
    IntRegister<unsigned long int>();
    IntRegister<long long int>();
    IntRegister<unsigned long long int>();

    registration::class_<std::vector<bool>>("std::vector<bool>");
    registration::class_<std::vector<int>>("std::vector<int>");
    registration::class_<std::vector<float>>("std::vector<float>");
    registration::class_<std::vector<double>>("std::vector<double>");


    registration::class_<std::string>("std::string")
                .constructor<>()(policy::ctor::as_object)
                .constructor<const std::string&>()(policy::ctor::as_object)
                .constructor<const std::string&, unsigned int, unsigned int>()(policy::ctor::as_object)
                .constructor<const char*>()(policy::ctor::as_object)
                .constructor<const char*, unsigned int>()(policy::ctor::as_object)
                .constructor<unsigned int, char>()(policy::ctor::as_object)
                ;

    // required to enforce proper order of registration/unregistration of rttr::variant and reference_wrapper to it
    registration::class_<rttr::variant>("rttr::variant");
    registration::class_<std::reference_wrapper<rttr::variant>>("std::reference_wrapper<rttr::variant>");
}
