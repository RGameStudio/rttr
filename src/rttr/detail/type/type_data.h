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

#ifndef RTTR_TYPE_DATA_H_
#define RTTR_TYPE_DATA_H_

#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/detail/misc/function_traits.h"
#include "rttr/detail/misc/template_type_trait.h"
#include "rttr/detail/type/base_classes.h"
#include "rttr/detail/type/get_derived_info_func.h"
#include "rttr/detail/type/get_create_variant_func.h"
#include "rttr/detail/type/type_register.h"
#include "rttr/detail/type/type_name.h"
#include "rttr/detail/misc/utility.h"
#include "rttr/destructor.h"
#include "rttr/method.h"
#include "rttr/property.h"
#include "rttr/constructor.h"
#include "rttr/destructor.h"
#include "rttr/detail/metadata/metadata.h"
#include "rttr/template_argument_data.h"

#include <type_traits>
#include <bitset>
#include <utility>


namespace rttr
{
namespace detail
{
struct type_data;

RTTR_LOCAL type_data& get_invalid_type_data() RTTR_NOEXCEPT;

static type get_invalid_type() RTTR_NOEXCEPT;

using rttr_cast_func        = void*(*)(void*);
using get_derived_info_func = derived_info(*)(void*);

class enumeration_wrapper_base;

/////////////////////////////////////////////////////////////////////////////////////////

struct RTTR_LOCAL class_data
{
    class_data(get_derived_info_func func, std::vector<template_argument_data> nested_types)
    :   m_derived_info_func(func),
        m_nested_types(std::move(nested_types)),
        m_dtor(create_invalid_item<destructor>())
    {}

    get_derived_info_func       m_derived_info_func;
    std::vector<type>           m_base_types;
    std::vector<type>           m_derived_types;
    std::vector<rttr_cast_func> m_conversion_list;
    std::vector<property>       m_properties;
    std::vector<method>         m_methods;
    std::vector<constructor>    m_ctors;
    std::vector<template_argument_data> m_nested_types;
    destructor                  m_dtor;
};

enum class type_trait_infos : std::size_t
{
    is_class = 0,
    is_enum,
    is_array,
    is_pointer,
    is_arithmetic,
    is_function_pointer,
    is_member_object_pointer,
    is_member_function_pointer,
    is_associative_container,
    is_sequential_container,
    is_template_instantiation,

    TYPE_TRAIT_COUNT
};

enum class type_of_visit : bool
{
    begin_visit_type,
    end_visit_type
};

 using type_traits = std::bitset<static_cast<std::size_t>(type_trait_infos::TYPE_TRAIT_COUNT)>;

/////////////////////////////////////////////////////////////////////////////////////////

namespace impl
{

using create_variant_func  = decltype(&create_invalid_variant_policy::create_variant);
using create_wrapper_func  = void(*)(const argument& arg, variant& var);
using get_class_data_func  = class_data&(*)(void);
using visit_type_func      = void(*)(type_of_visit, visitor&, const type&);

} // end namespace impl

/////////////////////////////////////////////////////////////////////////////////////////

struct RTTR_LOCAL type_data
{
    type_data* m_raw_type_data;
    type_data* m_wrapped_type;
    type_data* m_array_raw_type;

    std::string m_name;
    std::string m_type_name;

    std::size_t m_get_sizeof;
    std::size_t m_get_pointer_dimension;

    impl::create_variant_func m_create_variant;

    enumeration_wrapper_base* m_enum_wrapper;
    impl::create_wrapper_func m_create_wrapper;
    impl::visit_type_func     m_visit_type;

    type_traits m_type_traits;
    class_data  m_class_data;

    type_data* m_cont_key; // type of the key for associative containers, invalid otherwise
    type_data* m_cont_value; // type of the value for sequential or non-key-only associative containers, invalid otherwise

    info_container          m_base_types;
    std::vector<metadata>   m_metadata;
    std::vector<type_data*> m_dependent_types; // types whose name depend on this type name (e.g. pointer, array, etc.)

    bool m_is_valid;

    type_data(
            type_data* raw_type_data,
            type_data* wrapped_type,
            type_data* array_raw_type,

            std::string&& name,
            std::string_view type_name,

            std::size_t get_sizeof,
            std::size_t get_pointer_dimension,

            impl::create_variant_func create_variant,

            enumeration_wrapper_base*  enum_wrapper,
            impl::create_wrapper_func  create_wrapper,
            impl::visit_type_func      visit_type,

            type_traits&& type_traits,
            class_data&&  class_data,

            type_data* cont_key,
            type_data* cont_value,

            info_container&& base_types,

            bool is_valid
        )
        : m_raw_type_data(raw_type_data)
        , m_wrapped_type(wrapped_type)
        , m_array_raw_type(array_raw_type)

        , m_name(std::move(name))
        , m_type_name(type_name)

        , m_get_sizeof(get_sizeof)
        , m_get_pointer_dimension(get_pointer_dimension)

        , m_create_variant(create_variant)

        , m_enum_wrapper(enum_wrapper)
        , m_create_wrapper(create_wrapper)
        , m_visit_type(visit_type)

        , m_type_traits(std::move(type_traits))
        , m_class_data(std::move(class_data))

        , m_cont_key(cont_key)
        , m_cont_value(cont_value)

        , m_base_types(std::move(base_types))

        , m_is_valid(is_valid)
    {}

    RTTR_FORCE_INLINE bool type_trait_value(type_trait_infos type_trait) const RTTR_NOEXCEPT { return m_type_traits.test(static_cast<std::size_t>(type_trait)); }
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Enable = void>
struct RTTR_LOCAL get_size_of
{
    RTTR_INLINE RTTR_CONSTEXPR static std::size_t value()
    {
        return sizeof(T);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct RTTR_LOCAL get_size_of<T, enable_if_t<std::is_same<T, void>::value || std::is_function<T>::value>>
{
    RTTR_INLINE RTTR_CONSTEXPR static std::size_t value()
    {
        return 0;
    }
};

/////////////////////////////////////////////////////////////////////////////////

template<typename T, bool = std::is_same<T, typename raw_type<T>::type >::value>
struct RTTR_LOCAL raw_type_info
{
    static RTTR_INLINE type get_type() RTTR_NOEXCEPT { return get_invalid_type(); } // we have to return an empty type, so we can stop the recursion
};

/////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct RTTR_LOCAL raw_type_info<T, false>
{
    static RTTR_INLINE type get_type() RTTR_NOEXCEPT { return type::get<typename raw_type<T>::type>(); }
};

/////////////////////////////////////////////////////////////////////////////////

template<typename T, bool = std::is_array<T>::value>
struct RTTR_LOCAL array_raw_type
{
    static RTTR_INLINE type get_type() RTTR_NOEXCEPT { return type::get<raw_array_type_t<T>>(); }
};

/////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct RTTR_LOCAL array_raw_type<T, false>
{
    static RTTR_INLINE type get_type() RTTR_NOEXCEPT { return get_invalid_type(); } // we have to return an empty type, so we can stop the recursion
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, bool = is_wrapper<T>::value>
struct RTTR_LOCAL wrapper_type_info
{
    static RTTR_INLINE type get_type() RTTR_NOEXCEPT { return type::get<wrapper_mapper_t<T>>(); }
};

/////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct RTTR_LOCAL wrapper_type_info<T, false>
{
    static RTTR_INLINE type get_type() RTTR_NOEXCEPT { return get_invalid_type(); }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Wrapper, typename Wrapped_Type>
RTTR_LOCAL RTTR_INLINE void create_wrapper(const argument& arg, variant& var)
{
    if (arg.get_type() != type::get<Wrapped_Type>())
        return;

    auto& wrapped_type = arg.get_value<Wrapped_Type>();
    var = wrapper_mapper<Wrapper>::create(wrapped_type);
}

template<typename Wrapper, typename Tp = wrapper_mapper_t<Wrapper>>
RTTR_LOCAL RTTR_INLINE
enable_if_t<is_wrapper<Wrapper>::value &&
            rttr::detail::is_copy_constructible<Wrapper>::value &&
            std::is_default_constructible<Wrapper>::value &&
            has_create_wrapper_func<Wrapper>::value, impl::create_wrapper_func>
get_create_wrapper_func()
{
    return &create_wrapper<Wrapper, Tp>;
}


template<typename Wrapper, typename Tp = wrapper_mapper_t<Wrapper>>
RTTR_LOCAL RTTR_INLINE
enable_if_t<!is_wrapper<Wrapper>::value ||
            !rttr::detail::is_copy_constructible<Wrapper>::value ||
            !std::is_default_constructible<Wrapper>::value ||
            !has_create_wrapper_func<Wrapper>::value, impl::create_wrapper_func>
get_create_wrapper_func()
{
    return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

using type_trait_value = uint64_t;
#define TYPE_TRAIT_TO_BITSET_VALUE(trait) (static_cast<std::uint64_t>(std::trait<T>::value) << static_cast<std::size_t>(type_trait_infos::trait))
#define TYPE_TRAIT_TO_BITSET_VALUE_2(trait, enum_key) (static_cast<std::uint64_t>(trait<T>::value) << static_cast<std::size_t>(type_trait_infos::enum_key))

/////////////////////////////////////////////////////////////////////////////////////////
template<typename T, bool = is_sequential_container<T>::value, bool = is_associative_container<T>::value>
struct RTTR_LOCAL cont_type_info
{
    static RTTR_INLINE type get_key_type() RTTR_NOEXCEPT { return get_invalid_type(); }
    static RTTR_INLINE type get_value_type() RTTR_NOEXCEPT { return get_invalid_type(); }
};
template<typename T>
struct RTTR_LOCAL cont_type_info<T, true, false>
{
    static RTTR_INLINE type get_key_type() RTTR_NOEXCEPT { return get_invalid_type(); }
    static RTTR_INLINE type get_value_type() RTTR_NOEXCEPT { return type::get<typename sequential_container_mapper<T>::value_t>(); }
};
template<typename T>
struct RTTR_LOCAL cont_type_info<T, false, true>
{
    static RTTR_INLINE type get_key_type() RTTR_NOEXCEPT { return type::get<typename associative_container_mapper<T>::key_t>(); }
    static RTTR_INLINE type get_value_type() RTTR_NOEXCEPT
    {
        if constexpr (std::is_same_v<typename associative_container_mapper<T>::value_t, void>)
            return get_invalid_type();
        else
            return type::get<typename associative_container_mapper<T>::value_t>();
    }
};


} // end namespace detail
} // end namespace rttr


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

namespace rttr
{
namespace detail
{

template<typename T>
RTTR_LOCAL type_data make_type_data()
{
    class_data data(get_most_derived_info_func<T>(), template_type_trait<T>::get_template_arguments());

    type_traits traits(
        type_trait_value{
                TYPE_TRAIT_TO_BITSET_VALUE(is_class) |
                TYPE_TRAIT_TO_BITSET_VALUE(is_enum) |
                TYPE_TRAIT_TO_BITSET_VALUE(is_array) |
                TYPE_TRAIT_TO_BITSET_VALUE(is_pointer) |
                TYPE_TRAIT_TO_BITSET_VALUE(is_arithmetic) |
                TYPE_TRAIT_TO_BITSET_VALUE_2(is_function_ptr, is_function_pointer) |
                TYPE_TRAIT_TO_BITSET_VALUE(is_member_object_pointer) |
                TYPE_TRAIT_TO_BITSET_VALUE(is_member_function_pointer) |
                TYPE_TRAIT_TO_BITSET_VALUE_2(rttr::detail::is_associative_container, is_associative_container) |
                TYPE_TRAIT_TO_BITSET_VALUE_2(rttr::detail::is_sequential_container, is_sequential_container) |
                TYPE_TRAIT_TO_BITSET_VALUE_2(rttr::detail::template_type_trait, is_template_instantiation)
            }
        );

    return type_data(
            raw_type_info<T>::get_type().m_type_data,     //-- type_data* m_raw_type_data;
            wrapper_type_info<T>::get_type().m_type_data, //-- type_data* m_wrapped_type;
            array_raw_type<T>::get_type().m_type_data,    //-- type_data* m_array_raw_type;

            std::string{},                    //-- std::string m_name;
            rttr::detail::get_type_name<T>(), //-- std::string m_type_name;

            get_size_of<T>::value(), //-- std::size_t m_get_sizeof;
            pointer_count<T>::value, //-- std::size_t m_get_pointer_dimension;

            &create_variant_func<T>::create_variant, //-- impl::create_variant_func m_create_variant;

            nullptr,                      //-- enumeration_wrapper_base* m_enum_wrapper;
            get_create_wrapper_func<T>(), //-- impl::create_wrapper_func m_create_wrapper;
            nullptr,                      //-- impl::visit_type_func     m_visit_type;

            std::move(traits), //-- type_traits m_type_traits;
            std::move(data),   //-- class_data  m_class_data;

            cont_type_info<T>::get_key_type().m_type_data,   //-- type_data* m_cont_key;
            cont_type_info<T>::get_value_type().m_type_data, //-- type_data* m_cont_value;

            base_classes<T>::get_types(), //-- info_container m_base_types;

            true //-- bool m_is_valid;
        );
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_TYPE_DATA_H_
