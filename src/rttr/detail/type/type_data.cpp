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

#include "rttr/type.h"
#include "rttr/detail/type/type_data.h"

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

static std::unique_ptr<type_data> get_invalid_type_data_impl() RTTR_NOEXCEPT
{
    auto instance = std::make_unique<type_data>(
            nullptr, //-- type_data* m_raw_type_data;
            nullptr, //-- type_data* m_wrapped_type;
            nullptr, //-- type_data* m_array_raw_type;

            std::string(""),    //-- std::string m_name;
            std::string_view(), //-- std::string m_type_name;

            0, //-- std::size_t m_get_sizeof;
            0, //-- std::size_t m_get_pointer_dimension;

            &create_invalid_variant_policy::create_variant, //-- impl::create_variant_func m_create_variant;

            nullptr,                         //-- enumeration_wrapper_base* m_enum_wrapper;
            get_create_wrapper_func<void>(), //-- impl::create_wrapper_func m_create_wrapper;
            nullptr,                         //-- impl::visit_type_func     m_visit_type;

            type_trait_value{0},                                        //-- type_traits m_type_traits;
            class_data(nullptr, std::vector<template_argument_data>()), //-- class_data  m_class_data;

            nullptr, //-- type_data* m_cont_key;
            nullptr, //-- type_data* m_cont_value;

            base_classes<void>::get_types(), //-- info_container m_base_types;

            false //-- bool m_is_valid;
        );

    instance->m_raw_type_data  = instance.get();
    instance->m_wrapped_type   = instance.get();
    instance->m_array_raw_type = instance.get();

    return instance;
}

/////////////////////////////////////////////////////////////////////////////////////////

type_data& get_invalid_type_data() RTTR_NOEXCEPT
{
    static auto instance = get_invalid_type_data_impl();
    return *instance;
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr


