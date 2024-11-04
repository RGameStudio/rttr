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


#include <catch/catch.hpp>
#include <rttr/type>
#include <rttr/registration>
#include <vector>
#include <map>
#include <array>

using namespace rttr;

enum TestEnum {};
struct TestStructWithDefaultName {};
struct TestStructWithCustomName {};

template<typename T>
struct TestTemplate {};

class TestClass
{
public:
    class NestedClass {};
};

struct TestStruct
{
    struct NestedStruct {};
};

RTTR_REGISTRATION
{
    registration::class_<TestStructWithCustomName>("MyTestStruct");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("type get_name", "[type]")
{
   CHECK(type::get<int*>().get_name() == "int32*");
   CHECK(type::get<const int*>().get_name() == "const int32*");
   CHECK(type::get<const int&>().get_name() == "int32");
   CHECK(type::get<const int**&>().get_name() == "const int32**");

   auto t = type::get<std::array<float, 3>>();
   CHECK(t.get_name() == "std::array<float,3>");
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test rttr::type - type::get_by_name", "[type]")
{
    CHECK(type::get_by_name("std::string").is_valid()   == true);
    CHECK(type::get_by_name("std::string*").is_valid()  == true);

    CHECK(type::get_by_name("std::string[100]").is_valid()   == false);
    type::get<std::string[100]>(); // register it first, now it is available also by name lookup too
    CHECK(type::get_by_name("std::string[100]").is_valid()   == true);
}

/////////////////////////////////////////////////////////////////////////////////////////
TEST_CASE("Test get_type_name - detail::get_type_name", "[type]")
{
    static_assert(rttr::get_type_name<int>() == "int");
    static_assert(rttr::get_type_name<float>() == "float");
    static_assert(rttr::get_type_name<double>() == "double");

    static_assert(rttr::get_type_name<TestClass>() == "TestClass");
    static_assert(rttr::get_type_name<TestClass::NestedClass>() == "TestClass::NestedClass");

    static_assert(rttr::get_type_name<TestStruct>() == "TestStruct");
    static_assert(rttr::get_type_name<TestStruct::NestedStruct>() == "TestStruct::NestedStruct");

    static_assert(rttr::get_type_name<TestEnum>() == "TestEnum");
}

namespace
{

template<typename T>
void checkPortableName(std::string_view name)
{
    auto t = type::get<T>();
    CHECK(t.is_valid());
    CHECK(type::get_by_name(name) == t);
}

}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("portable type name", "[type]")
{
    checkPortableName<TestEnum>("TestEnum");
    checkPortableName<TestStructWithDefaultName>("TestStructWithDefaultName");
    checkPortableName<std::vector<TestStructWithCustomName>>("std::vector<MyTestStruct,std::allocator<MyTestStruct>>");
    checkPortableName<std::vector<TestStructWithDefaultName>>("std::vector<TestStructWithDefaultName,std::allocator<TestStructWithDefaultName>>");
    checkPortableName<std::map<std::string, TestStructWithCustomName>>("std::map<std::string,MyTestStruct,std::less<std::string>,std::allocator<std::pair<std::string,MyTestStruct>>>");
    checkPortableName<TestTemplate<TestTemplate<TestTemplate<std::string>>>>("TestTemplate<TestTemplate<TestTemplate<std::string>>>");
    checkPortableName<TestTemplate<TestTemplate<TestTemplate<int>>>>("TestTemplate<TestTemplate<TestTemplate<int32>>>");
    checkPortableName<TestTemplate<TestTemplate<TestTemplate<TestEnum>>>>("TestTemplate<TestTemplate<TestTemplate<TestEnum>>>");
#if RTTR_ARCH_TYPE == RTTR_ARCH_64
    checkPortableName<size_t>("uint64");
#else
    checkPortableName<size_t>("uint32");
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////

