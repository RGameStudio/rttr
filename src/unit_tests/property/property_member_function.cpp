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
#include <rttr/registration_friend>
#include <iostream>
#include <memory>
#include <functional>

#include <catch/catch.hpp>

using namespace rttr;

static int g_invalid_instance = 0;

using func_ptr = void(*)(int);
struct property_member_func_test
{
    property_member_func_test() : m_int_value(12)
    {
    }

    const std::string& get_text() const { return m_text; }
    std::string get_text_2() const { return m_text; }
    void set_text(const std::string& text) { m_text = text; }
    void set_text_2(std::string text) { m_text = std::move(text); }
    void set_text_3(std::string&& text) { m_text = std::move(text); }

    int get_int_value() { return m_int_value; }

    int& get_int_ref() { return m_int_value; }

    void set_function_cb(func_ptr cb) { m_funcPtr = cb; }
    func_ptr get_function_cb() const { return m_funcPtr; }

    std::string     m_text;
    int             m_int_value;
    func_ptr        m_funcPtr;

    RTTR_REGISTRATION_FRIEND;
};

static void my_callback(int)
{
}

inline static constexpr uint64_t c_description_meta_key = rttr::hash_string("Description");

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<property_member_func_test>("property_member_func_test")
        .property("p1", &property_member_func_test::get_text, &property_member_func_test::set_text)
        (
            metadata(c_description_meta_key, "Some Text")
        )
        .property("p1-copy-g", &property_member_func_test::get_text_2, &property_member_func_test::set_text)
        .property("p1-copy-s", &property_member_func_test::get_text, &property_member_func_test::set_text_2)
        .property("p1-move-g", &property_member_func_test::get_text_2, &property_member_func_test::set_text_3)
        .property("p1-move-s", &property_member_func_test::get_text, &property_member_func_test::set_text_3)
        .property("p1-lambda",
            [](const property_member_func_test& obj) -> std::string { return obj.m_text; },
            [](property_member_func_test& obj, const std::string& value) { obj.m_text = value; })
        .property("p1-lambda-move",
            [](const property_member_func_test& obj) -> std::string { return obj.m_text; },
            [](property_member_func_test& obj, std::string&& value) { obj.m_text = std::move(value); })
        .property_readonly("p2", &property_member_func_test::get_int_value)
        (
            metadata(c_description_meta_key, "Some Text")
        )
        .property("p3", &property_member_func_test::get_text, &property_member_func_test::set_text)
        (
            metadata(c_description_meta_key, "Some Text"),
            policy::prop::bind_as_ptr
        )
        .property("p3-move", &property_member_func_test::get_text, &property_member_func_test::set_text_3)(policy::prop::bind_as_ptr)
        .property_readonly("p4", &property_member_func_test::get_int_ref)
        (
            metadata(c_description_meta_key, "Some Text"),
            policy::prop::bind_as_ptr
        )
        .property("callback", &property_member_func_test::get_function_cb, &property_member_func_test::set_function_cb)
        .property("p5", &property_member_func_test::get_text, &property_member_func_test::set_text)
        (
            metadata(c_description_meta_key, "Some Text"),
            policy::prop::as_reference_wrapper
        )
        .property("p5-move", &property_member_func_test::get_text, &property_member_func_test::set_text_3)
        (
            policy::prop::as_reference_wrapper
         )
        .property("p5-lambda-move",
            [](const property_member_func_test& obj) -> const std::string& { return obj.m_text; },
            [](property_member_func_test& obj, std::string&& value) { obj.m_text = std::move(value); })
        (
            policy::prop::as_reference_wrapper
         )
        .property_readonly("p6", &property_member_func_test::get_int_ref)
        (
            metadata(c_description_meta_key, "Some Text"),
            policy::prop::as_reference_wrapper
        )
        ;
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - class function", "[property]")
{
    property_member_func_test obj;
    type prop_type = type::get<decltype(obj)>();
    REQUIRE(prop_type.is_valid() == true);

    property prop = prop_type.get_property("p1");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == false);
    CHECK(prop.get_type() == type::get<std::reference_wrapper<const std::string>>());
    CHECK(prop.get_declaring_type() == type::get<property_member_func_test>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata(c_description_meta_key) == "Some Text");

    // valid invoke
    CHECK(prop.set_value(obj, std::string("New Text")) == true);
    CHECK(prop.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
    CHECK(prop.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "New Text");

    // copy & move
    std::string v = "one";
    CHECK(prop.set_value(obj, v) == true);
    CHECK(prop.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
    CHECK(prop.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "one");
    CHECK(v == "one");
    v = "two";
    CHECK(prop.set_value(obj, std::move(v)) == true);
    CHECK(prop.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
    CHECK(prop.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "two");
    CHECK(v == "two");
    v = "three";
    CHECK(prop.set_value_move(obj, v) == true);
    CHECK(prop.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
    CHECK(prop.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "three");
    CHECK(v == "three");
    v = "four";
    CHECK(prop.set_value_move(obj, std::move(v)) == true);
    CHECK(prop.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
    CHECK(prop.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "four");
    CHECK(v == "four");

    // invalid invoke
    CHECK(prop.set_value(obj, 42) == false);
    CHECK(prop.set_value(instance(), 42) == false);
    CHECK(prop.get_value(g_invalid_instance).is_valid() == false);

    // different cv-refs in getter and setter
    {
        property prop2 = prop_type.get_property("p1-copy-g");
        REQUIRE(prop2.is_valid() == true);
        CHECK(prop2.set_value(obj, std::string("String 2")) == true);
        CHECK(prop2.get_value(obj).is_type<std::string>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::string>() == "String 2");

        v = "five";
        CHECK(prop2.set_value(obj, v) == true);
        CHECK(prop2.get_value(obj).is_type<std::string>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::string>() == "five");
        CHECK(v == "five");
        v = "six";
        CHECK(prop2.set_value_move(obj, v) == true);
        CHECK(prop2.get_value(obj).is_type<std::string>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::string>() == "six");
        CHECK(v == "six");
    }

    {
        property prop2 = prop_type.get_property("p1-copy-s");
        REQUIRE(prop2.is_valid() == true);
        CHECK(prop2.set_value(obj, std::string("String 3")) == true);
        CHECK(prop2.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "String 3");

        v = "seven";
        CHECK(prop2.set_value(obj, v) == true);
        CHECK(prop2.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "seven");
        CHECK(v == "seven");
        v = "eight";
        CHECK(prop2.set_value_move(obj, v) == true);
        CHECK(prop2.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "eight");
        CHECK(v.empty());
    }

    {
        property prop2 = prop_type.get_property("p1-move-g");
        REQUIRE(prop2.is_valid() == true);
        CHECK(prop2.set_value(obj, std::string("String 4")) == true);
        CHECK(prop2.get_value(obj).is_type<std::string>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::string>() == "String 4");

        v = "nine";
        CHECK(prop2.set_value(obj, v) == true);
        CHECK(prop2.get_value(obj).is_type<std::string>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::string>() == "nine");
        CHECK(v == "nine");
        v = "ten";
        CHECK(prop2.set_value_move(obj, v) == true);
        CHECK(prop2.get_value(obj).is_type<std::string>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::string>() == "ten");
        CHECK(v.empty());
    }

    {
        property prop2 = prop_type.get_property("p1-move-s");
        REQUIRE(prop2.is_valid() == true);
        CHECK(prop2.set_value(obj, std::string("String 5")) == true);
        CHECK(prop2.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "String 5");

        v = "eleven";
        CHECK(prop2.set_value(obj, v) == true);
        CHECK(prop2.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "eleven");
        CHECK(v == "eleven");
        v = "twelve";
        CHECK(prop2.set_value_move(obj, v) == true);
        CHECK(prop2.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "twelve");
        CHECK(v.empty());
    }

    // lambdas that accept [const] T&
    {
        property prop2 = prop_type.get_property("p1-lambda");
        REQUIRE(prop2.is_valid() == true);
        CHECK(prop2.is_static() == false);
        CHECK(prop2.set_value(obj, std::string("String 6")) == true);
        CHECK(prop2.get_value(obj).is_type<std::string>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::string>() == "String 6");

        v = "thirteen";
        CHECK(prop2.set_value(obj, v) == true);
        CHECK(prop2.get_value(obj).is_type<std::string>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::string>() == "thirteen");
        CHECK(v == "thirteen");
        v = "fourteen";
        CHECK(prop2.set_value_move(obj, v) == true);
        CHECK(prop2.get_value(obj).is_type<std::string>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::string>() == "fourteen");
        CHECK(v == "fourteen");
    }

    {
        property prop2 = prop_type.get_property("p1-lambda-move");
        REQUIRE(prop2.is_valid() == true);
        CHECK(prop2.is_static() == false);
        CHECK(prop2.set_value(obj, std::string("String 7")) == true);
        CHECK(prop2.get_value(obj).is_type<std::string>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::string>() == "String 7");

        v = "fifteen";
        CHECK(prop2.set_value(obj, v) == true);
        CHECK(prop2.get_value(obj).is_type<std::string>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::string>() == "fifteen");
        CHECK(v == "fifteen");
        v = "sixteen";
        CHECK(prop2.set_value_move(obj, v) == true);
        CHECK(prop2.get_value(obj).is_type<std::string>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::string>() == "sixteen");
        CHECK(v.empty());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - class function - read only", "[property]")
{
    property_member_func_test obj;
    type prop_type = type::get<decltype(obj)>();

    property prop = prop_type.get_property("p2");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == false);
    CHECK(prop.get_type() == type::get<int>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata(c_description_meta_key) == "Some Text");

    // invoke
    CHECK(prop.get_value(obj).is_type<int>() == true);
    CHECK(prop.get_value(obj).get_value_unsafe<int>() == 12);

    // invalid invoke
    CHECK(prop.set_value(obj, 23) == false);
    CHECK(prop.get_value(g_invalid_instance).is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - class function - bind as ptr", "[property]")
{
    property_member_func_test obj;
    type prop_type = type::get<decltype(obj)>();

    property prop = prop_type.get_property("p3");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == false);
    CHECK(prop.get_type() == type::get<const std::string*>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata(c_description_meta_key) == "Some Text");

    // valid invoke
    const std::string text("Hello World");
    CHECK(prop.set_value(obj, &text) == true);
    CHECK(prop.get_value(obj).is_type<const std::string*>() == true);
    CHECK(*prop.get_value(obj).get_value_unsafe<const std::string*>() == "Hello World");
    CHECK(text == "Hello World");

    // move test
    const std::string other("Hi");
    CHECK(prop.set_value_move(obj, &other) == true);
    CHECK(prop.get_value(obj).is_type<const std::string*>() == true);
    CHECK(*prop.get_value(obj).get_value_unsafe<const std::string*>() == "Hi");
    CHECK(other == "Hi");

    // invalid invoke
    CHECK(prop.set_value(obj, 42) == false);
    CHECK(prop.set_value(instance(), 42) == false);
    CHECK(prop.get_value(g_invalid_instance).is_valid() == false);

    // movable variation
    {
        property prop2 = prop_type.get_property("p3-move");
        REQUIRE(prop2.is_valid() == true);

        std::string str = "foo";
        CHECK(prop2.set_value(obj, &str) == true);
        CHECK(prop2.get_value(obj).is_type<const std::string*>() == true);
        CHECK(*prop2.get_value(obj).get_value_unsafe<const std::string*>() == "foo");
        CHECK(str == "foo");

        str = "bar";
        CHECK(prop2.set_value_move(obj, &str) == true);
        CHECK(prop2.get_value(obj).is_type<const std::string*>() == true);
        CHECK(*prop2.get_value(obj).get_value_unsafe<const std::string*>() == "bar");
        CHECK(str.empty());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - class function - read only - bind as ptr", "[property]")
{
    property_member_func_test obj;
    type prop_type = type::get<decltype(obj)>();

    property prop = prop_type.get_property("p4");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == false);

    CHECK(prop.get_type() == type::get<const int*>());
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata(c_description_meta_key) == "Some Text");

    // invoke
    REQUIRE(prop.get_value(obj).is_type<const int*>() == true);
    CHECK(*prop.get_value(obj).get_value_unsafe<const int*>() == 12);

    // invalid invoke
    CHECK(prop.set_value(obj, 23) == false);
    CHECK(prop.set_value("wrong instance", 23) == false);
    CHECK(prop.get_value(g_invalid_instance).is_valid() == false);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - class function - function pointer", "[property]")
{
    property_member_func_test obj;
    type prop_type = type::get<decltype(obj)>();

    property prop = prop_type.get_property("callback");

    auto cb = &my_callback;
    bool ret = prop.set_value(obj, cb);
    CHECK(ret == true);

    variant var = prop.get_value(obj);
    REQUIRE(var.is_type<func_ptr>() == true);
    CHECK(var.get_value_unsafe<func_ptr>() == cb);
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - class function - as_reference_wrapper", "[property]")
{
    property_member_func_test obj;
    type prop_type = type::get<decltype(obj)>();

    property prop = prop_type.get_property("p5");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == false);
    CHECK(prop.get_type() == type::get<std::reference_wrapper<const std::string>>());
    CHECK(prop.get_type().is_wrapper() == true);
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata(c_description_meta_key) == "Some Text");

    // set value using ref
    const std::string text("Hello World");
    CHECK(prop.set_value(obj, std::cref(text)) == true);
    CHECK(prop.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
    CHECK(prop.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "Hello World");
    CHECK(text == "Hello World");

    // move value using ref
    const std::string other("Hi");
    CHECK(prop.set_value_move(obj, std::ref(other)) == true);
    CHECK(prop.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
    CHECK(prop.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "Hi");
    CHECK(other == "Hi");

    // set value using copy
    const std::string next_text("Hello World, again!");
    CHECK(prop.set_value(obj, next_text) == true);
    CHECK(prop.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
    CHECK(prop.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "Hello World, again!");
    CHECK(next_text == "Hello World, again!");

    // move value using copy
    const std::string other_text = "Hello!";
    CHECK(prop.set_value_move(obj, other_text) == true);
    CHECK(prop.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
    CHECK(prop.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "Hello!");
    CHECK(other_text == "Hello!");

    // invalid invoke
    CHECK(prop.set_value(obj, 42) == false);
    CHECK(prop.set_value(instance(), 42) == false);
    CHECK(prop.get_value(g_invalid_instance).is_valid() == false);

    // movable variation
    {
        property prop2 = prop_type.get_property("p5-move");
        REQUIRE(prop2.is_valid() == true);

        // set value using ref
        std::string str = "foo";
        CHECK(prop2.set_value(obj, std::ref(str)) == true);
        CHECK(prop2.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "foo");
        CHECK(str == "foo");

        // move value using ref
        str = "bar";
        CHECK(prop2.set_value_move(obj, std::ref(str)) == true);
        CHECK(prop2.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "bar");
        CHECK(str.empty());

        // set value using copy
        str = "foo/bar";
        CHECK(prop2.set_value(obj, str) == true);
        CHECK(prop2.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "foo/bar");
        CHECK(str == "foo/bar");

        // move value using copy
        str = "bar/foo";
        CHECK(prop2.set_value_move(obj, str) == true);
        CHECK(prop2.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "bar/foo");
        CHECK(str.empty());
    }

    // lambda movable variation
    {
        property prop2 = prop_type.get_property("p5-lambda-move");
        REQUIRE(prop2.is_valid() == true);

        // set value using ref
        std::string str = "foo";
        CHECK(prop2.set_value(obj, std::ref(str)) == true);
        CHECK(prop2.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "foo");
        CHECK(str == "foo");

        // move value using ref
        str = "bar";
        CHECK(prop2.set_value_move(obj, std::ref(str)) == true);
        CHECK(prop2.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "bar");
        CHECK(str.empty());

        // set value using copy
        str = "foo/bar";
        CHECK(prop2.set_value(obj, str) == true);
        CHECK(prop2.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "foo/bar");
        CHECK(str == "foo/bar");

        // move value using copy
        str = "bar/foo";
        CHECK(prop2.set_value_move(obj, str) == true);
        CHECK(prop2.get_value(obj).is_type<std::reference_wrapper<const std::string>>() == true);
        CHECK(prop2.get_value(obj).get_value_unsafe<std::reference_wrapper<const std::string>>().get() == "bar/foo");
        CHECK(str.empty());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("property - class function - read only - as_reference_wrapper", "[property]")
{
    property_member_func_test obj;
    type prop_type = type::get<decltype(obj)>();

    property prop = prop_type.get_property("p6");
    REQUIRE(prop.is_valid() == true);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == false);
    CHECK(prop.get_type() == type::get<std::reference_wrapper<const int>>());
    CHECK(prop.get_type().is_wrapper() == true);
    CHECK(prop.get_access_level() == rttr::access_levels::public_access);
    CHECK(prop.get_metadata(c_description_meta_key) == "Some Text");

    // invoke
    REQUIRE(prop.get_value(obj).is_type<std::reference_wrapper<const int>>() == true);
    CHECK(prop.get_value(obj).get_value_unsafe<std::reference_wrapper<const int>>().get() == 12);

    // invalid invoke
    CHECK(prop.set_value(obj, 23) == false);
    CHECK(prop.set_value("wrong instance", 23) == false);
    CHECK(prop.get_value(g_invalid_instance).is_valid() == false);
}


/////////////////////////////////////////////////////////////////////////////////////////
