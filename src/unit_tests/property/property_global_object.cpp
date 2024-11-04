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

#include <iostream>
#include <memory>
#include <functional>

#include <catch/catch.hpp>

using namespace rttr;
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////

static std::string g_name;
static const int g_int_value = 23;
static std::vector<int> g_my_array(1000, 42);
static std::string g_strarr[2];

inline static constexpr uint64_t c_description_meta_key = rttr::hash_string("Description");

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::property("global_obj_1", &g_name) ( metadata(c_description_meta_key, "Some Text") )
        .property_readonly("global_obj_2", &g_int_value) ( metadata(c_description_meta_key, "Some Text") )
        .property("global_obj_3", &g_my_array)
        (
            metadata(c_description_meta_key, "Some Text"),
            policy::prop::bind_as_ptr
        )
        .property_readonly("global_obj_4", &g_int_value)
        (
            metadata(c_description_meta_key, "Some Text"),
            policy::prop::bind_as_ptr
        )
        .property("global_obj_5", &g_my_array)
        (
            metadata(c_description_meta_key, "Some Text"),
            policy::prop::as_reference_wrapper
        )
        .property_readonly("global_obj_6", &g_int_value)
        (
            metadata(c_description_meta_key, "Some Text"),
            policy::prop::as_reference_wrapper
        )
        .property("global_obj_7", &g_strarr)
        .property("global_obj_8", &g_strarr)(policy::prop::bind_as_ptr)
        .property("global_obj_9", &g_strarr)(policy::prop::as_reference_wrapper)
        ;

    registration::property("global_obj_1", &g_name); // cannot register the same object twice
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global object", "[property]")
{
    property prop = type::get_global_property("global_obj_1");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type() == type::get<std::reference_wrapper<std::string>>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata(c_description_meta_key) == "Some Text");

    // valid invoke
    CHECK(prop.set_value(instance(), std::string("New Text")) == true);
    CHECK(prop.get_value(instance()).is_type<std::reference_wrapper<std::string>>() == true);
    CHECK(prop.get_value(instance()).get_value_unsafe<std::reference_wrapper<std::string>>().get() == "New Text");

    // copy & move
    std::string val = "one";
    CHECK(prop.set_value(instance(), val) == true);
    CHECK(prop.get_value(instance()).is_type<std::reference_wrapper<std::string>>() == true);
    CHECK(prop.get_value(instance()).get_value_unsafe<std::reference_wrapper<std::string>>().get() == "one");
    CHECK(val == "one");
    val = "two";
    CHECK(prop.set_value(instance(), std::move(val)) == true);
    CHECK(prop.get_value(instance()).is_type<std::reference_wrapper<std::string>>() == true);
    CHECK(prop.get_value(instance()).get_value_unsafe<std::reference_wrapper<std::string>>().get() == "two");
    CHECK(val == "two");
    val = "three";
    CHECK(prop.set_value_move(instance(), val) == true);
    CHECK(prop.get_value(instance()).is_type<std::reference_wrapper<std::string>>() == true);
    CHECK(prop.get_value(instance()).get_value_unsafe<std::reference_wrapper<std::string>>().get() == "three");
    CHECK(val.empty());
    val = "four";
    CHECK(prop.set_value_move(instance(), std::move(val)) == true);
    CHECK(prop.get_value(instance()).is_type<std::reference_wrapper<std::string>>() == true);
    CHECK(prop.get_value(instance()).get_value_unsafe<std::reference_wrapper<std::string>>().get() == "four");
    CHECK(val.empty());

    // invalid invoke
    CHECK(prop.set_value(instance(), 42) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global object - read only", "[property]")
{
    property prop = type::get_global_property("global_obj_2");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type() == type::get<std::reference_wrapper<const int>>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata(c_description_meta_key) == "Some Text");

    // valid invoke
    CHECK(prop.get_value(instance()).is_type<std::reference_wrapper<const int>>() == true);
    CHECK(prop.get_value(instance()).get_value_unsafe<std::reference_wrapper<const int>>().get() == 23);

    // invalid invoke
    CHECK(prop.set_value(instance(), 42) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global object - bind as ptr", "[property]")
{
    property prop = type::get_global_property("global_obj_3");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type().get_raw_type().is_sequential_container() == true);
    CHECK(prop.get_type() == type::get<std::vector<int>*>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata(c_description_meta_key) == "Some Text");

    // invoke
    REQUIRE(prop.get_value(instance()).is_type<std::vector<int>*>() == true);
    auto ptr = prop.get_value(instance()).get_value_unsafe<std::vector<int>*>();
    CHECK(ptr == &g_my_array);
    CHECK(prop.set_value(instance(), ptr) == true);

    std::vector<int> some_vec(1, 12);
    CHECK(prop.set_value(instance(), &some_vec) == true);
    CHECK(some_vec == g_my_array);
    CHECK(!some_vec.empty());

    // move
    some_vec = { 5, 6, 7 };
    CHECK(prop.set_value_move(instance(), &some_vec) == true);
    CHECK(g_my_array.size() == 3);
    CHECK(some_vec.empty());

    // negative invoke
    CHECK(prop.set_value(instance(), "test") == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global object - read only - bind as ptr", "[property]")
{
    property prop = type::get_global_property("global_obj_4");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type() == type::get<const int*>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata(c_description_meta_key) == "Some Text");

    // valid invoke
    CHECK(prop.get_value(instance()).is_type<const int*>() == true);
    CHECK(*prop.get_value(instance()).get_value_unsafe<const int*>() == 23);

    // invalid invoke
    CHECK(prop.set_value(instance(), 42) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global object - as_reference_wrapper", "[property]")
{
    property prop = type::get_global_property("global_obj_5");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type().get_wrapped_type().is_sequential_container() == true);
    CHECK(prop.get_type() == type::get< std::reference_wrapper<std::vector<int>> >());
    CHECK(prop.get_type().is_wrapper() == true);
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata(c_description_meta_key) == "Some Text");

    // set value using ref
    REQUIRE(prop.get_value(instance()).is_type< std::reference_wrapper<std::vector<int>> >() == true);
    auto value = prop.get_value(instance()).get_value_unsafe< std::reference_wrapper<std::vector<int>> >();
    CHECK(value.get() == g_my_array);
    CHECK(prop.set_value(instance(), value) == true);

    std::vector<int> some_vec(1, 12);
    CHECK(prop.set_value(instance(), std::ref(some_vec)) == true);
    CHECK(some_vec == g_my_array);
    CHECK(!some_vec.empty());

    // move value using ref
    some_vec = { 5, 6, 7 };
    CHECK(prop.set_value_move(instance(), std::ref(some_vec)) == true);
    CHECK(g_my_array.size() == 3);
    CHECK(some_vec.empty());

    // set value using copy
    some_vec = {8, 9, 10};
    CHECK(prop.set_value(instance(), some_vec) == true);

    // move value using copy
    some_vec = {11, 12, 13};
    CHECK(prop.set_value_move(instance(), some_vec) == true);

    // negative invoke
    CHECK(prop.set_value(instance(), "test") == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global object - read only - as_reference_wrapper", "[property]")
{
    property prop = type::get_global_property("global_obj_6");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type() == type::get< std::reference_wrapper<const int> >());
    CHECK(prop.get_type().is_wrapper() == true);
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata(c_description_meta_key) == "Some Text");

    // valid invoke
    CHECK(prop.get_value(instance()).is_type< std::reference_wrapper<const int> >() == true);
    CHECK(prop.get_value(instance()).get_value_unsafe< std::reference_wrapper<const int> >().get() == 23);

    // invalid invoke
    int value = 42;
    CHECK(prop.set_value(instance(), std::cref(value)) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global object - array", "[property]")
{
    property prop = type::get_global_property("global_obj_7");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type() == type::get<std::reference_wrapper<std::string[2]>>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);

    // copy & move
    std::string val[] = { "one", "foo" };
    CHECK(prop.set_value(instance(), val) == true);
    CHECK(prop.get_value(instance()).is_type<std::reference_wrapper<std::string[2]>>() == true);
    CHECK(prop.get_value(instance()).get_value_unsafe<std::reference_wrapper<std::string[2]>>()[0] == "one");
    CHECK(prop.get_value(instance()).get_value_unsafe<std::reference_wrapper<std::string[2]>>()[1] == "foo");
    CHECK(val[0] == "one");
    val[0] = "two";
    CHECK(prop.set_value(instance(), std::move(val)) == true);
    CHECK(prop.get_value(instance()).is_type<std::reference_wrapper<std::string[2]>>() == true);
    CHECK(prop.get_value(instance()).get_value_unsafe<std::reference_wrapper<std::string[2]>>()[0] == "two");
    CHECK(prop.get_value(instance()).get_value_unsafe<std::reference_wrapper<std::string[2]>>()[1] == "foo");
    CHECK(val[0] == "two");
    val[0] = "three";
    CHECK(prop.set_value_move(instance(), val) == true);
    CHECK(prop.get_value(instance()).is_type<std::reference_wrapper<std::string[2]>>() == true);
    CHECK(prop.get_value(instance()).get_value_unsafe<std::reference_wrapper<std::string[2]>>()[0] == "three");
    CHECK(prop.get_value(instance()).get_value_unsafe<std::reference_wrapper<std::string[2]>>()[1] == "foo");
    CHECK(val[0].empty());
    val[0] = "four";
    CHECK(prop.set_value_move(instance(), std::move(val)) == true);
    CHECK(prop.get_value(instance()).is_type<std::reference_wrapper<std::string[2]>>() == true);
    CHECK(prop.get_value(instance()).get_value_unsafe<std::reference_wrapper<std::string[2]>>()[0] == "four");
    CHECK(prop.get_value(instance()).get_value_unsafe<std::reference_wrapper<std::string[2]>>()[1].empty());
    CHECK(val[0].empty());

    // invalid invoke
    CHECK(prop.set_value(instance(), 42) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global object - array - bind as ptr", "[property]")
{
    property prop = type::get_global_property("global_obj_8");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type() == type::get<std::string(*)[2]>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);

    // copy & move
    std::string val[] = { "one", "foo" };
    CHECK(prop.set_value(instance(), &val) == true);
    CHECK(prop.get_value(instance()).is_type<std::string(*)[2]>() == true);
    CHECK((*prop.get_value(instance()).get_value_unsafe<std::string(*)[2]>())[0] == "one");
    CHECK((*prop.get_value(instance()).get_value_unsafe<std::string(*)[2]>())[1] == "foo");
    CHECK(val[0] == "one");
    val[0] = "three";
    CHECK(prop.set_value_move(instance(), &val) == true);
    CHECK(prop.get_value(instance()).is_type<std::string(*)[2]>() == true);
    CHECK((*prop.get_value(instance()).get_value_unsafe<std::string(*)[2]>())[0] == "three");
    CHECK((*prop.get_value(instance()).get_value_unsafe<std::string(*)[2]>())[1] == "foo");
    CHECK(val[0].empty());

    // invalid invoke
    CHECK(prop.set_value(instance(), 42) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - global object - array - as_reference_wrapper", "[property]")
{
    property prop = type::get_global_property("global_obj_9");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == true);
    CHECK(prop.get_type() == type::get< std::reference_wrapper<std::string[2]> >());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);

    // copy & move
    std::string val[] = { "one", "foo" };
    CHECK(prop.set_value(instance(), std::ref(val)) == true);
    CHECK(prop.get_value(instance()).is_type< std::reference_wrapper<std::string[2]> >() == true);
    CHECK(prop.get_value(instance()).get_value_unsafe< std::reference_wrapper<std::string[2]> >().get()[0] == "one");
    CHECK(prop.get_value(instance()).get_value_unsafe< std::reference_wrapper<std::string[2]> >().get()[1] == "foo");
    CHECK(val[0] == "one");
    val[0] = "three";
    CHECK(prop.set_value_move(instance(), std::ref(val)) == true);
    CHECK(prop.get_value(instance()).is_type< std::reference_wrapper<std::string[2]> >() == true);
    CHECK(prop.get_value(instance()).get_value_unsafe< std::reference_wrapper<std::string[2]> >().get()[0] == "three");
    CHECK(prop.get_value(instance()).get_value_unsafe< std::reference_wrapper<std::string[2]> >().get()[1] == "foo");
    CHECK(val[0].empty());

    // invalid invoke
    CHECK(prop.set_value(instance(), 42) == false);
}

/////////////////////////////////////////////////////////////////////////////////////////
