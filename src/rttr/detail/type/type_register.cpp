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

#include "rttr/detail/type/type_register.h"

#include "rttr/detail/type/type_register_p.h"

#include "rttr/detail/constructor/constructor_wrapper_base.h"
#include "rttr/detail/destructor/destructor_wrapper_base.h"
#include "rttr/detail/enumeration/enumeration_wrapper_base.h"
#include "rttr/detail/parameter_info/parameter_infos_compare.h"
#include "rttr/detail/method/method_wrapper_base.h"
#include "rttr/hash_string_constexpr.h"
#include "rttr/detail/property/property_wrapper.h"
#include "rttr/detail/metadata/metadata.h"
#include "rttr/constructor.h"
#include "rttr/destructor.h"
#include "rttr/property.h"
#include "rttr/method.h"
#include "rttr/detail/type/type_data.h"

#include "rttr/detail/filter/filter_item_funcs.h"
#include "rttr/detail/type/type_string_utils.h"
#include "rttr/detail/registration/registration_manager.h"

#include <set>

#include <iostream>

using namespace std;

namespace rttr
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////

void type_register::register_reg_manager(registration_manager* manager)
{
    type_register_private::get_instance().register_reg_manager(manager);
}

/////////////////////////////////////////////////////////////////////////////////////////

void type_register::unregister_reg_manager(registration_manager* manager)
{
     type_register_private::get_instance().unregister_reg_manager(manager);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::register_property(const property_wrapper_base* prop)
{
    return type_register_private::get_instance().register_property(prop);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::register_global_property(const property_wrapper_base* prop)
{
     return type_register_private::get_instance().register_global_property(prop);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::unregister_global_property(const property_wrapper_base* prop)
{
     return type_register_private::get_instance().unregister_global_property(prop);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::register_method(method_wrapper_base* meth)
{
     return type_register_private::get_instance().register_method(meth);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::register_global_method(method_wrapper_base* meth)
{
     return type_register_private::get_instance().register_global_method(meth);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::unregister_global_method(method_wrapper_base* meth)
{
     return type_register_private::get_instance().unregister_global_method(meth);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::register_constructor(constructor_wrapper_base* ctor)
{
     return type_register_private::get_instance().register_constructor(ctor);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::register_destructor(destructor_wrapper_base* dtor)
{
     return type_register_private::get_instance().register_destructor(dtor);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::register_enumeration(enumeration_wrapper_base* enum_data)
{
    const auto t = enum_data->get_type();
    t.m_type_data->m_enum_wrapper = enum_data;
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::unregister_enumeration(enumeration_wrapper_base* enum_data)
{
    const auto t = enum_data->get_type();
    t.m_type_data->m_enum_wrapper = nullptr; // FIXME: possible unsafe: m_type_data can be invalid
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

void type_register::custom_name(type& t, std::string_view custom_name)
{
     type_register_private::get_instance().register_custom_name(t, custom_name);
}

/////////////////////////////////////////////////////////////////////////////////////////

void type_register::metadata(const type& t, std::vector<::rttr::detail::metadata>&& data)
{
    auto& vec_to_insert = t.m_type_data->m_metadata;

    // when we insert new items, we want to check first whether a item with same key exist => ignore this data
    for (auto& new_item : data)
    {
        if (std::find_if(vec_to_insert.begin(), vec_to_insert.end(), [&new_item](const detail::metadata& m) { return m.get_key() == new_item.get_key(); }) == vec_to_insert.end())
        {
            vec_to_insert.emplace_back(std::move(new_item));
        }
    }

    std::sort(vec_to_insert.begin(), vec_to_insert.end(), [](const detail::metadata& left, const detail::metadata& right) { return left.get_key() < right.get_key(); });
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::register_converter(const type_converter_base* converter)
{
     return type_register_private::get_instance().register_converter(converter);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::unregister_converter(const type_converter_base* converter)
{
     return type_register_private::get_instance().unregister_converter(converter);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::register_equal_comparator(type_comparator_base* comparator)
{
     return type_register_private::get_instance().register_equal_comparator(comparator);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::unregister_equal_comparator(const type_comparator_base* converter)
{
     return type_register_private::get_instance().unregister_equal_comparator(converter);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::register_less_than_comparator(type_comparator_base* comparator)
{
     return type_register_private::get_instance().register_less_than_comparator(comparator);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::unregister_less_than_comparator(const type_comparator_base* converter)
{
     return type_register_private::get_instance().unregister_less_than_comparator(converter);
}

/////////////////////////////////////////////////////////////////////////////////////////

void type_register::register_base_class(const type& derived_type, const base_class_info& base_info)
{
    auto& class_data = derived_type.m_type_data->m_class_data;
    auto itr = std::find_if(class_data.m_base_types.begin(), class_data.m_base_types.end(),
    [base_info](const type& t)
    {
        return (t == base_info.m_base_type);
    });

    if (itr != class_data.m_base_types.end()) // already registered as base class => quit
        return;

    std::vector<std::pair<type, rttr_cast_func>> tmp_sort_vec;
    using sorted_pair = decltype(tmp_sort_vec)::value_type;
    if (class_data.m_base_types.size() != class_data.m_conversion_list.size())
        return; // error!!!

    int index = 0;
    for (const auto& item : class_data.m_base_types)
    {
        tmp_sort_vec.emplace_back(item, class_data.m_conversion_list[index]);
        ++index;
    }

    tmp_sort_vec.emplace_back(base_info.m_base_type, base_info.m_rttr_cast_func);
    std::sort(tmp_sort_vec.begin(), tmp_sort_vec.end(),
    [](const sorted_pair& left, const sorted_pair& right)
    { return left.first.get_id() < right.first.get_id(); });

    class_data.m_base_types.clear();
    class_data.m_conversion_list.clear();

    std::transform(tmp_sort_vec.begin(),
                   tmp_sort_vec.end(),
                   std::back_inserter(class_data.m_base_types),
                   [](const sorted_pair& item)-> type { return item.first; });

    std::transform(tmp_sort_vec.begin(),
                   tmp_sort_vec.end(),
                   std::back_inserter(class_data.m_conversion_list),
                   [](const sorted_pair& item)-> rttr_cast_func { return item.second; });

    auto r_type = base_info.m_base_type.get_raw_type();
    r_type.m_type_data->m_class_data.m_derived_types.push_back(type(derived_type.m_type_data));
}

/////////////////////////////////////////////////////////////////////////////////////////

type_register_private::type_register_private()
:   m_type_list({ type(&get_invalid_type_data()) }),
    m_type_data_storage({ get_invalid_type_data() })
{
}

/////////////////////////////////////////////////////////////////////////////////////////

type_register_private::~type_register_private()
{
    // When this dtor is running, it means, that RTTR library will be unloaded
    // In order to avoid that the registration_manager instance's
    // are trying to deregister its content, although the RTTR library is already unloaded
    // every registration manager class holds a flag whether it should deregister itself or not
    // and we are settings this flag here
    for (auto& manager : m_registration_manager_list)
        manager->set_disable_unregister();
}

/////////////////////////////////////////////////////////////////////////////////////////

type_register_private& type_register_private::get_instance()
{
    static type_register_private obj;
    return obj;
}

/////////////////////////////////////////////////////////////////////////////////////////

void type_register_private::register_reg_manager(registration_manager* manager)
{
    m_registration_manager_list.insert(manager);
}

/////////////////////////////////////////////////////////////////////////////////////////

void type_register_private::unregister_reg_manager(registration_manager* manager)
{
    m_registration_manager_list.erase(manager);
}

/////////////////////////////////////////////////////////////////////////////////////////

std::pair<type_data*, bool> type_register::register_type(type_data&& info) RTTR_NOEXCEPT
{
    return type_register_private::get_instance().register_type(std::move(info));
}

/////////////////////////////////////////////////////////////////////////////////////////

void type_register::unregister_type(type_data* info) RTTR_NOEXCEPT
{
    type_register_private::get_instance().unregister_type(info);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register::register_visit_type_func(type& t, visit_type_func func) RTTR_NOEXCEPT
{
    t.m_type_data->m_visit_type = func;
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// free static functions

static bool rotate_char_when_whitespace_before(std::string& text, std::string::size_type& pos, char c)
{
    auto result = text.find(c, pos);
    if (result != std::string::npos && result > 0)
    {
        if (::isspace(text[result - 1]))
        {
            text[result - 1] = c;
            text[result] = ' ';
        }
        pos = result + 1;
        return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

static void move_pointer_and_ref_to_type(std::string& type_name)
{
    std::string::size_type pos = 0;
    while(pos < type_name.length())
    {
        if (!rotate_char_when_whitespace_before(type_name, pos, '*') &&
            !rotate_char_when_whitespace_before(type_name, pos, '&') &&
            !rotate_char_when_whitespace_before(type_name, pos, ')'))
        {
            pos = std::string::npos;
        }
    }

    const auto non_whitespace = type_name.find_last_not_of(' ');
    type_name.resize(non_whitespace + 1);
}

/////////////////////////////////////////////////////////////////////////////////////////

static std::string normalize_orig_name(std::string_view name)
{
    std::string normalized_name = std::string(name);

    move_pointer_and_ref_to_type(normalized_name);
    return normalized_name;
}

/////////////////////////////////////////////////////////////////////////////////////////

static std::string derive_name_impl(const std::string& src_name, const std::string& raw_name,
                                    const std::string& custom_name)
{
    auto tmp_src_name = src_name;
    auto tmp_raw_name = raw_name;

    // We replace a custom registered name for a type for all derived types, e.g.
    // "std::basic_string<char>" => "std::string"
    // we want to use this also automatically for derived types like pointers, e.g.
    // "const std::basic_string<char>*" => "const std::string*"
    // therefore we have to replace the "raw_type" string
    remove_whitespaces(tmp_raw_name);
    remove_whitespaces(tmp_src_name);

    const auto start_pos = tmp_src_name.find(tmp_raw_name);
    const auto end_pos = start_pos + tmp_raw_name.length();
    if (start_pos == std::string::npos)
        return src_name; // nothing was found...

    // remember the two parts before and after the found "raw_name"
    const auto start_part = tmp_src_name.substr(0, start_pos);
    const auto end_part = tmp_src_name.substr(end_pos, tmp_src_name.length());

    tmp_src_name.replace(start_pos, tmp_raw_name.length(), custom_name);

    if (is_space_after(src_name, start_part))
        insert_space_after(tmp_src_name, start_part);

    if (is_space_before(src_name, end_part))
        insert_space_before(tmp_src_name, end_part);

    return tmp_src_name;
}

/////////////////////////////////////////////////////////////////////////////////////////

static std::vector<type> convert_param_list(const array_range<parameter_info>& param_list)
{
    std::vector<type> result;
    result.reserve(param_list.size());

    for (const auto& item : param_list)
        result.emplace_back(item.get_type());

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
static array_range<T> get_items_for_type(const type& t,
                                         const std::vector<T>& vec)
{
    return array_range<T>(vec.data(), vec.size(),
                          detail::default_predicate<T>([t](const T& item) { return (item.get_declaring_type() == t); }) );
}

/////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename I>
static bool remove_container_item(T& container, const I& item)
{
    bool result = false;
    container.erase(std::remove_if(container.begin(), container.end(),
                                   [&item, &result](I& item_)
                                   { return (item_== item) ? (result = true) : false; }),
                    container.end());

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//
// Here comes the implementation of the registration class 'type_register_private'
//
/////////////////////////////////////////////////////////////////////////////////////////

void type_register_private::register_base_class_info(type_data* info)
{
    auto base_classes = info->m_base_types;

    // remove double entries; can only be happen for virtual inheritance case
    set<type> double_entries;
    for (auto itr = base_classes.rbegin(); itr != base_classes.rend();)
    {
        if (double_entries.find(itr->m_base_type) == double_entries.end())
        {
            double_entries.insert(itr->m_base_type);
            ++itr;
        }
        else
        {
            itr = vector<base_class_info>::reverse_iterator(base_classes.erase((++itr).base()));
        }
    }

    // sort the base classes after it registration index, that means the root class is always the first in the list,
    // followed by its derived classes, here it depends on the order of RTTR_DECLARE_ANCESTORS(CLASS)
    std::sort(base_classes.begin(), base_classes.end(), [](const base_class_info& left, const base_class_info& right)
                                                         { return left.m_base_type.is_base_of(right.m_base_type); });

    if (!base_classes.empty())
    {
        auto& class_data = info->m_class_data;
        for (const auto& t : base_classes)
        {
            class_data.m_base_types.push_back(t.m_base_type);
            class_data.m_conversion_list.push_back(t.m_rttr_cast_func);

            auto r_type = t.m_base_type.get_raw_type();
            r_type.m_type_data->m_class_data.m_derived_types.push_back(type(info));
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

std::pair<type_data*, bool> type_register_private::register_type(type_data&& info) RTTR_NOEXCEPT
{
    using namespace detail;

    // this will register the base types
    //info.m_get_base_types();

    type_data* stored = nullptr;

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        // check if the type is registered
        const auto ret = m_orig_name_to_id.find(rttr::hash_string(info.m_type_name));
        if (ret != m_orig_name_to_id.cend())
        {
            if (ret->second.m_type_data->m_is_valid)
            {
                // type is registered and valid, return existing type_data
                return std::make_pair(ret->second.m_type_data, false);
            }
            else
            {
                // type is registered but not valid, maybe it was unregistered
                stored = ret->second.m_type_data;
            }
        }

        if (!stored)
        {
            // store new type_data
            stored = &m_type_data_storage.emplace_back(std::move(info));
        }
        else
        {
            // store new type_data into existing storage
            *stored = std::move(info);
        }

        stored->m_raw_type_data = !stored->m_raw_type_data->m_is_valid ? stored : stored->m_raw_type_data;

        m_orig_name_to_id.try_emplace(rttr::hash_string(stored->m_type_name), type(stored));
        stored->m_name = derive_name(type(stored));
        m_custom_name_to_id.try_emplace(rttr::hash_string(stored->m_name), type(stored));

        m_type_list.emplace_back(type(stored));
    }

    // has to be done as last step
    register_base_class_info(stored);

    register_dependent_type(stored);

    // when a base class type has class items, but the derived one not,
    // we update the derived class item list
    const auto t = type(stored);
    update_class_list(t, &class_data::m_properties);
    update_class_list(t, &class_data::m_methods);

    return std::make_pair(stored, true);
}

/////////////////////////////////////////////////////////////////////////////////////////

void type_register_private::remove_derived_types_from_base_classes(type& t, const std::vector<type>& base_types)
{
    // here we get from all base types, the derived types list and remove the given type "t"
    for (auto data : base_types)
    {
        auto& class_data = data.m_type_data->m_class_data;
        auto& derived_types = class_data.m_derived_types;
        derived_types.erase(std::remove_if(derived_types.begin(), derived_types.end(), [t](type derived_t) { return derived_t == t; }));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

void type_register_private::remove_base_types_from_derived_classes(type& t, const std::vector<type>& derived_types)
{
    // here we get from all base types, the derived types list and remove the given type "t"
    for (auto data : derived_types)
    {
        auto& class_data = data.m_type_data->m_class_data;
        auto& base_types = class_data.m_base_types;
        base_types.erase(std::remove_if(base_types.begin(), base_types.end(), [t](type base_t) { return base_t == t; }));
    }
}

/////////////////////////////////////////////////////////////////////////////////////

template<typename Fn>
void type_register_private::enumerate_dependencies(type_data* info, const Fn& fn)
{
    if (info->m_array_raw_type->m_is_valid)
    {
        fn(info->m_array_raw_type);
    }

    if (info->m_raw_type_data != info)
    {
        fn(info->m_raw_type_data);
    }

    for (auto nested: info->m_class_data.m_nested_types)
    {
        fn(nested.m_type.m_type_data);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

void type_register_private::register_dependent_type(type_data* info)
{
    enumerate_dependencies(info, [&](type_data* dependency)
    {
        dependency->m_dependent_types.push_back(info);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

void type_register_private::unregister_dependent_type(type_data* info)
{
    enumerate_dependencies(info, [&](type_data* dependency)
    {
        remove_container_item(dependency->m_dependent_types, info);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////

void type_register_private::unregister_type(type_data* info) RTTR_NOEXCEPT
{
    {
        // REMARK: the base_types has to be provided as argument explicitely and cannot be retrieve via the type_data itself,
        // because the `class_data` which holds the base_types information cannot be retrieve via the function `get_class_data`
        // anymore because the containing std::shared_ptr is already destroyed
        std::lock_guard<std::mutex> lock(m_mutex);

        // we want to remove the name info, only when we found the correct type_data
        // it can be, that a duplicate type_data object will try to unregister itself
        auto it = std::find_if(m_type_data_storage.begin(), m_type_data_storage.end(), [info](const auto& v) { return info == &v; });
        if (it != m_type_data_storage.cend())
        {
            type_data& ref = *it;

            type obj_t(&ref);
            remove_container_item(m_type_list, obj_t);
            unregister_dependent_type(&ref);
            remove_derived_types_from_base_classes(obj_t, ref.m_class_data.m_base_types);
            remove_base_types_from_derived_classes(obj_t, ref.m_class_data.m_derived_types);

            std::string custom_name = std::move(ref.m_name);
            std::string type_name = std::move(ref.m_type_name);

            ref = get_invalid_type_data();
            ref.m_name = std::move(custom_name);
            ref.m_type_name = std::move(type_name);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

void type_register_private::update_custom_name(const type& t, std::string new_name)
{
    auto& type_name = t.m_type_data->m_name;
    if (new_name == type_name)
    {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_custom_name_to_id.erase(rttr::hash_string(type_name));

        type_name = std::move(new_name);
        m_custom_name_to_id.emplace(rttr::hash_string(type_name), t);
    }

    for (auto dependent: t.m_type_data->m_dependent_types)
    {
        auto dt = type(dependent);
        update_custom_name(dt, derive_name(dt));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string type_register_private::derive_name(const type& t)
{
    std::string name;

    if (t.is_array() && t.get_raw_array_type().is_valid())
    {
        auto src_name_orig  = normalize_orig_name(t.get_full_name());
        auto custom_name    = t.get_raw_array_type().get_name();
        auto raw_name_orig  = normalize_orig_name( t.get_raw_array_type().get_full_name());

        name = derive_name_impl(src_name_orig, raw_name_orig, string(custom_name));
    }
    else if (t != t.get_raw_type())
    {
        auto src_name_orig  = normalize_orig_name(t.get_full_name());
        auto custom_name    = t.get_raw_type().get_name();
        auto raw_name_orig  = normalize_orig_name( t.get_raw_type().get_full_name());

        name = derive_name_impl(src_name_orig, raw_name_orig, string(custom_name));
    }
    else
    {
        name = normalize_orig_name(t.get_full_name());
    }

    if (t.is_template_instantiation())
    {
        auto nested_types = t.get_template_arguments();
        if (nested_types.empty()) // no template type
        {
            return name;
        }

        const auto has_custom_name = (name != t.get_full_name());
        if (has_custom_name)
        {
            return name;
        }

        const auto start_pos = name.find("<");
        const auto end_pos = name.rfind(">");

        if (start_pos == std::string::npos || end_pos == std::string::npos)
        {
            return name;
        }

        auto param_pos = start_pos + 1;

        auto new_name = name.substr(0, start_pos + 1);
        const auto end_part = name.substr(end_pos);
        bool mid = false;
        for (const auto& item : nested_types)
        {
            auto next_param_pos = name.find(',', param_pos);
            if (next_param_pos == std::string::npos)
            {
                next_param_pos = end_pos;
            }
            else
            {
                next_param_pos += 1;
            }

            if (mid)
            {
                new_name += ",";
            }
            else
            {
                mid = true;
            }

            if (item.m_templateParam)
            {
                new_name += item.m_type.m_type_data->m_name;
            }
            else
            {
                new_name += name.substr(param_pos, next_param_pos - param_pos);
            }

            param_pos = next_param_pos;
        }

        new_name += end_part;
        name = std::move(new_name);
    }

    return name;
}

/////////////////////////////////////////////////////////////////////////////////////////

void type_register_private::register_custom_name(type& t, std::string_view custom_name)
{
    if (!t.is_valid())
        return;

    update_custom_name(t, std::string(custom_name));
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register_private::register_constructor(const constructor_wrapper_base* ctor)
{
    const auto t = ctor->get_declaring_type();
    auto& class_data = t.m_type_data->m_class_data;
    class_data.m_ctors.emplace_back(create_item<::rttr::constructor>(ctor));
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

bool type_register_private::register_destructor(const destructor_wrapper_base* dtor)
{
    const auto t = dtor->get_declaring_type();
    auto& class_data = t.m_type_data->m_class_data;

    auto& dtor_type = class_data.m_dtor;
    if (!dtor_type) // when no dtor is set at the moment
    {
        auto d = create_item<::rttr::destructor>(dtor);
        dtor_type = d;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

bool type_register_private::register_property(const property_wrapper_base* prop)
{
    const auto t    = prop->get_declaring_type();
    const auto name = prop->get_name();

    auto& property_list = t.m_type_data->m_class_data.m_properties;

    if (get_type_property(t, name))
        return false;

    auto p = detail::create_item<::rttr::property>(prop);
    property_list.emplace_back(p);
    update_class_list(t, &class_data::m_properties);
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register_private::register_global_property(const property_wrapper_base* prop)
{
    const auto t    = prop->get_declaring_type();
    const auto name = prop->get_name();

     if (t.get_global_property(name))
         return false;

     auto p = detail::create_item<::rttr::property>(prop);
     m_global_properties.emplace_back(p);
     m_global_property_storage[rttr::hash_string(name)].push_back(std::move(p));
     return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register_private::unregister_global_property(const property_wrapper_base* prop)
{
    auto p = detail::create_item<::rttr::property>(prop);

    auto result = false;
    if (auto it = m_global_property_storage.find(rttr::hash_string(prop->get_name())); it != m_global_property_storage.end())
    {
        result = remove_container_item(it->second, p);
        if (it->second.empty())
        {
            m_global_property_storage.erase(it);
        }
    }

    auto result2 = remove_container_item(m_global_properties, p);
    return result && result2;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

bool type_register_private::register_method(const method_wrapper_base* meth)
{
    const auto t    = meth->get_declaring_type();
    const auto name = meth->get_name();
    auto m          = create_item<::rttr::method>(meth);

    if (get_type_method(t, name, convert_param_list(meth->get_parameter_infos())))
        return false;

    auto& method_list = t.m_type_data->m_class_data.m_methods;
    method_list.emplace_back(m);
    update_class_list(t, &class_data::m_methods);
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register_private::register_global_method(const method_wrapper_base* meth)
{
    const auto t    = meth->get_declaring_type();
    const auto name = meth->get_name();
    auto m          = create_item<::rttr::method>(meth);

    if (t.get_global_method(name, convert_param_list(meth->get_parameter_infos())))
        return false;

    m_global_methods.push_back(m);
    m_global_method_storage[rttr::hash_string(name)].push_back(std::move(m));
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register_private::unregister_global_method(const method_wrapper_base* meth)
{
    auto m          = create_item<::rttr::method>(meth);

    auto result = false;
    if (auto it = m_global_method_storage.find(rttr::hash_string(meth->get_name())); it != m_global_method_storage.end())
    {
        result = remove_container_item(it->second, m);
        if (it->second.empty())
        {
            m_global_method_storage.erase(it);
        }
    }

    auto result2 = remove_container_item(m_global_methods, m);
    return result && result2;
}

/////////////////////////////////////////////////////////////////////////////////////////

property type_register_private::get_type_property(const type& t, std::string_view name)
{
    for (const auto& prop : get_items_for_type(t, t.m_type_data->m_class_data.m_properties))
    {
        if (prop.get_name() == name)
            return prop;
    }

    return create_invalid_item<::rttr::property>();
}

/////////////////////////////////////////////////////////////////////////////////////////

method type_register_private::get_type_method(const type& t, std::string_view name,
                                              const std::vector<type>& type_list)
{
    for (const auto& meth : get_items_for_type(t, t.m_type_data->m_class_data.m_methods))
    {
        if (meth.get_name() == name &&
            compare_with_type_list::compare(meth.get_parameter_infos(), type_list))
        {
            return meth;
        }
    }

    return detail::create_invalid_item<::rttr::method>();
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void type_register_private::update_class_list(const type& t, T item_ptr)
{
    auto& all_class_items = (t.m_type_data->m_class_data.*item_ptr);

    // update type "t" with all items from the base classes
    auto item_range = get_items_for_type(t, all_class_items);
    detail::remove_cv_ref_t<decltype(all_class_items)> item_vec(item_range.begin(), item_range.end());
    all_class_items.reserve(all_class_items.size() + 1);
    all_class_items.clear(); // this will not reduce the capacity, i.e. new memory allocation may not necessary
    for (const auto& base_type : t.get_base_classes())
    {
        auto base_properties = get_items_for_type(base_type, base_type.m_type_data->m_class_data.*item_ptr);
        if (base_properties.empty())
            continue;

        all_class_items.reserve(all_class_items.size() + base_properties.size());
        all_class_items.insert(all_class_items.end(), base_properties.begin(), base_properties.end());
    }

    // insert own class items
    all_class_items.reserve(all_class_items.size() + item_vec.size());
    all_class_items.insert(all_class_items.end(), item_vec.begin(), item_vec.end());

    // update derived types
    for (const auto& derived_type : t.get_derived_classes())
        update_class_list<T>(derived_type, item_ptr);
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

bool type_register_private::register_converter(const type_converter_base* converter)
{
    const auto t = converter->get_source_type();

    if (!t.is_valid())
        return false;

    if (get_converter(t, converter->m_target_type))
        return false;

    using vec_value_type = data_container<const type_converter_base*>;
    m_type_converter_list.push_back({t.get_id(), converter});
    std::stable_sort(m_type_converter_list.begin(), m_type_converter_list.end(),
                     vec_value_type::order_by_id());
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename Container, typename Item>
static bool remove_item(Container& container, Item& item)
{
    using order = typename Container::value_type::order_by_id;
    auto itr = std::lower_bound(container.begin(), container.end(),
                                item, order());
    if (itr != container.end())
    {
        if ((*itr).m_data == item)
        {
            container.erase(itr);
            return true;
        }
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool type_register_private::unregister_converter(const type_converter_base* converter)
{
    return remove_item(m_type_converter_list, converter);
}

/////////////////////////////////////////////////////////////////////////////////////

const type_converter_base* type_register_private::get_converter(const type& source_type, const type& target_type)
{
    const auto src_id = source_type.get_id();
    const auto target_id = target_type.get_id();
    using vec_value_type = data_container<const type_converter_base*>;
    auto itr = std::lower_bound(m_type_converter_list.cbegin(), m_type_converter_list.cend(),
                                src_id, vec_value_type::order_by_id());
    for (; itr != m_type_converter_list.cend(); ++itr)
    {
        auto& item = *itr;
        if (item.m_id != src_id)
            break; // type not found

        if (item.m_data->m_target_type.get_id() == target_id)
            return item.m_data;
    }

    return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////

const type_comparator_base* type_register_private::get_equal_comparator(const type& t)
{
    return get_type_comparator_impl(t, m_type_equal_cmp_list);
}

/////////////////////////////////////////////////////////////////////////////////////

const type_comparator_base* type_register_private::get_less_than_comparator(const type& t)
{
    return get_type_comparator_impl(t, m_type_less_than_cmp_list);
}

/////////////////////////////////////////////////////////////////////////////////////

const type_comparator_base*
type_register_private::get_type_comparator_impl(const type& t,
                                                const std::vector<type_register_private::data_container<const type_comparator_base*>>& comparator_list)
 {
    using vec_value_type = data_container<const type_comparator_base*>;
    const auto id = t.get_id();
    auto itr = std::lower_bound(comparator_list.cbegin(), comparator_list.cend(), id,
                                vec_value_type::order_by_id());
    if (itr != comparator_list.cend() && itr->m_id == id)
        return itr->m_data;
    else
        return nullptr;
 }

/////////////////////////////////////////////////////////////////////////////////////

bool type_register_private::register_equal_comparator(const type_comparator_base* comparator)
{
    return register_comparator_impl(comparator->cmp_type, comparator, m_type_equal_cmp_list);
}

/////////////////////////////////////////////////////////////////////////////////////

bool type_register_private::unregister_equal_comparator(const type_comparator_base* converter)
{
    return remove_item(m_type_equal_cmp_list, converter);
}

/////////////////////////////////////////////////////////////////////////////////////

bool type_register_private::register_less_than_comparator(const type_comparator_base* comparator)
{
    return register_comparator_impl(comparator->cmp_type, comparator, m_type_less_than_cmp_list);
}

/////////////////////////////////////////////////////////////////////////////////////

bool type_register_private::unregister_less_than_comparator(const type_comparator_base* converter)
{
    return remove_item(m_type_less_than_cmp_list, converter);
}

/////////////////////////////////////////////////////////////////////////////////////

bool type_register_private::register_comparator_impl(const type& t, const type_comparator_base* comparator,
                                                     std::vector<data_container<const type_comparator_base*>>& comparator_list)
{
    if (!t.is_valid())
        return false;

    if (get_type_comparator_impl(t, comparator_list)) // already registered an comparator ?
        return false;

    using data_type = data_container<const type_comparator_base*>;
    comparator_list.push_back({t.get_id(), comparator});
    std::stable_sort(comparator_list.begin(), comparator_list.end(),
                     data_type::order_by_id());
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////

const variant& type_register_private::get_metadata(const type& t, uint64_t key)
{
    auto& meta_vec = t.m_type_data->m_metadata;
    return get_metadata(key, meta_vec);
}

/////////////////////////////////////////////////////////////////////////////////////////

const variant& type_register_private::get_metadata(uint64_t key, const std::vector<metadata>& data)
{
    const auto it = std::lower_bound(data.begin(), data.end(), key, [](const detail::metadata& meta, uint64_t key) { return meta.get_key() < key; });
    if (it != data.end() && key == it->get_key())
    {
        return it->get_value();
    }

    return detail::get_empty_variant();
}

/////////////////////////////////////////////////////////////////////////////////////

std::deque<type_data>& type_register_private::get_type_data_storage()
{
    return m_type_data_storage;
}

/////////////////////////////////////////////////////////////////////////////////////

std::vector<type>& type_register_private::get_type_storage()
{
    return m_type_list;
}

/////////////////////////////////////////////////////////////////////////////////////

//-- TODO (d_dezhko): thread-safe type lookup by name
//--				  Make a bug report to the author
type type_register_private::get_by_custom_name(std::string_view custom_name)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto ret = m_custom_name_to_id.find(rttr::hash_string(custom_name));
    if (ret != m_custom_name_to_id.end())
    {
        return ret->second;
    }

    return get_invalid_type();
}

/////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end
