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

#ifndef RTTR_PROPERTY_WRAPPER_FUNC_H_
#define RTTR_PROPERTY_WRAPPER_FUNC_H_

#include "rttr/detail/policies/prop_policies.h"
#include "rttr/detail/type/accessor_type.h"

#include "rttr/detail/property/property_wrapper.h"


using namespace rttr::detail;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// global function getter/setter - function pointer

template<typename Declaring_Typ, typename Getter, typename Setter, access_levels Acc_Level, typename Visitor_List>
class property_wrapper<function_ptr, Declaring_Typ, Getter, Setter, Acc_Level, return_as_copy, set_value, Visitor_List>
    : public property_wrapper_base, public metadata_handler
{
    using return_type   = std::remove_cv_t<std::remove_reference_t<typename function_traits<Getter>::return_type>>;
    using true_arg_type = typename param_types<Setter, function_traits<Setter>::arg_count - 1>::type;
    using arg_type      = std::remove_cv_t<std::remove_reference_t<true_arg_type>>;
    static constexpr bool is_static_prop = function_traits<Getter>::arg_count == 0;

    public:
        property_wrapper(std::string_view name,
                         Getter get, Setter set,
                         std::vector<metadata>&& metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, type::get<Declaring_Typ>()),
            metadata_handler(std::move(metadata_list)),
            m_getter(get), m_setter(set)
        {
            static_assert(function_traits<Getter>::arg_count < 2, "Invalid number of argument, please provide a getter-function with 0 or 1 arguments.");
            static_assert(function_traits<Setter>::arg_count < 3, "Invalid number of argument, please provide a setter-function with 1 or 2 argument.");
            static_assert(std::is_same<return_type, arg_type>::value, "Please provide the same signature for getter and setter!");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT override    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT override                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT override                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT override                 { return is_static_prop; }
        type get_type()     const RTTR_NOEXCEPT override                 { return type::get<return_type>(); }

        const variant& get_metadata(uint64_t key) const override { return metadata_handler::get_metadata(key); }

        template<bool Move>
        bool set_value_impl(instance& object, argument& arg) const
        {
            if constexpr (!is_static_prop)
            {
                using class_type = std::remove_cv_t<std::remove_reference_t<typename param_types<Setter, 0>::type>>;
                if (class_type* ptr = object.try_convert<class_type>())
                {
                    if (arg.is_type<std::reference_wrapper<const arg_type>>())
                    {
                        if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                        {
                            m_setter(*ptr, arg.get_value<std::reference_wrapper<const arg_type>>().get());
                        }
                        else
                        {
                            m_setter(*ptr, arg_type(arg.get_value<std::reference_wrapper<const arg_type>>().get()));
                        }
                        return true;
                    }

                    if (arg.is_type<std::reference_wrapper<arg_type>>())
                    {
                        if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                        {
                            m_setter(*ptr, arg.get_value<std::reference_wrapper<arg_type>>().get());
                        }
                        else if constexpr (Move)
                        {
                            m_setter(*ptr, std::move(arg.get_value<std::reference_wrapper<arg_type>>().get()));
                        }
                        else
                        {
                            m_setter(*ptr, arg_type(arg.get_value<std::reference_wrapper<arg_type>>().get()));
                        }
                        return true;
                    }

                    if (arg.is_type<arg_type>())
                    {
                        if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                        {
                            m_setter(*ptr, arg.get_value<arg_type>());
                        }
                        else if constexpr (Move)
                        {
                            m_setter(*ptr, arg.get_value<arg_type&&>());
                        }
                        else
                        {
                            m_setter(*ptr, arg_type(arg.get_value<arg_type>()));
                        }
                        return true;
                    }
                }
            }
            else
            {
                if (arg.is_type<std::reference_wrapper<const arg_type>>())
                {
                    if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                    {
                        m_setter(arg.get_value<std::reference_wrapper<const arg_type>>().get());
                    }
                    else
                    {
                        m_setter(arg_type(arg.get_value<std::reference_wrapper<const arg_type>>().get()));
                    }
                    return true;
                }

                if (arg.is_type<std::reference_wrapper<arg_type>>())
                {
                    if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                    {
                        m_setter(arg.get_value<std::reference_wrapper<arg_type>>().get());
                    }
                    else if constexpr (Move)
                    {
                        m_setter(std::move(arg.get_value<std::reference_wrapper<arg_type>>().get()));
                    }
                    else
                    {
                        m_setter(arg_type(arg.get_value<std::reference_wrapper<arg_type>>().get()));
                    }
                    return true;
                }

                if (arg.is_type<arg_type>())
                {
                    if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                    {
                        m_setter(arg.get_value<arg_type>());
                    }
                    else if constexpr (Move)
                    {
                        m_setter(arg.get_value<arg_type&&>());
                    }
                    else
                    {
                        m_setter(arg_type(arg.get_value<arg_type>()));
                    }
                    return true;
                }
            }
            return false;
        }

        bool set_value_copy(instance& object, argument& arg) const override { return set_value_impl<false>(object, arg); }
        bool set_value_move(instance& object, argument& arg) const override { return set_value_impl<true>(object, arg); }

        variant get_value(instance& object) const override
        {
            if constexpr (!is_static_prop)
            {
                using class_type = std::remove_cv_t<std::remove_reference_t<typename param_types<Getter, 0>::type>>;
                if (class_type* ptr = object.try_convert<class_type>())
                {
                    return variant(m_getter(*ptr));
                }
                else
                {
                    return variant();
                }
            }
            else
            {
                return variant(m_getter());
            }
        }

        void visit(visitor& visitor, property prop) const RTTR_NOEXCEPT override
        {
            auto obj = make_property_getter_setter_info<Declaring_Typ, return_as_copy, Getter, Setter>(prop, m_getter, m_setter);
            visitor_iterator<Visitor_List>::visit(visitor, make_property_getter_setter_visitor_invoker(obj));
        }

    private:
        Getter m_getter;
        Setter m_setter;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// global function getter

template<typename Declaring_Typ, typename Getter, access_levels Acc_Level, typename Visitor_List>
class property_wrapper<function_ptr, Declaring_Typ, Getter, void, Acc_Level, return_as_copy, read_only, Visitor_List>
    : public property_wrapper_base, public metadata_handler
{
    using return_type = std::remove_cv_t<std::remove_reference_t<typename function_traits<Getter>::return_type>>;
    static constexpr bool is_static_prop = function_traits<Getter>::arg_count == 0;

    public:
        property_wrapper(std::string_view name,
                         Getter get, std::vector<metadata>&& metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, type::get<Declaring_Typ>()),
            metadata_handler(std::move(metadata_list)),
            m_accessor(get)
        {
            static_assert(function_traits<Getter>::arg_count < 2, "Invalid number of argument, please provide a getter-function with 0 or 1 arguments.");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT override    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT override                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT override                 { return true; }
        bool is_static()    const RTTR_NOEXCEPT override                 { return is_static_prop; }
        type get_type()     const RTTR_NOEXCEPT override                 { return type::get<return_type>(); }

        const variant& get_metadata(uint64_t key) const override { return metadata_handler::get_metadata(key); }

        bool set_value_copy(instance& object, argument& arg) const override
        {
            return false;
        }

        bool set_value_move(instance& object, argument& arg) const override
        {
            return false;
        }

        variant get_value(instance& object) const override
        {
            if constexpr (!is_static_prop)
            {
                using class_type = std::remove_cv_t<std::remove_reference_t<typename param_types<Getter, 0>::type>>;
                if (class_type* ptr = object.try_convert<class_type>())
                    return variant(m_accessor(*ptr));
                else
                    return variant();
            }
            else
            {
                return variant(m_accessor());
            }
        }

        void visit(visitor& visitor, property prop) const RTTR_NOEXCEPT override
        {
            auto obj = make_property_info<Declaring_Typ, return_as_copy, Getter>(prop, m_accessor);
            visitor_iterator<Visitor_List>::visit(visitor, make_property_visitor_invoker<read_only>(obj));
        }

    private:
        Getter m_accessor;
};



/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// global function getter/setter

template<typename Declaring_Typ, typename Getter, typename Setter, access_levels Acc_Level, typename Visitor_List>
class property_wrapper<function_ptr, Declaring_Typ, Getter, Setter, Acc_Level, return_as_ptr, set_as_ptr, Visitor_List>
    : public property_wrapper_base, public metadata_handler
{
    using return_type   = typename function_traits<Getter>::return_type;
    using true_arg_type = typename param_types<Setter, function_traits<Setter>::arg_count - 1>::type;
    using arg_type      = std::remove_cv_t<std::remove_reference_t<true_arg_type>>;
    static constexpr bool is_static_prop = function_traits<Getter>::arg_count == 0;

    public:
        property_wrapper(std::string_view name,
                         Getter get, Setter set,
                         std::vector<metadata>&& metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, type::get<Declaring_Typ>()),
            metadata_handler(std::move(metadata_list)),
            m_getter(get), m_setter(set)
        {
            static_assert(std::is_reference<return_type>::value, "Please provide a getter-function with a reference as return value!");
            static_assert(std::is_reference<true_arg_type>::value, "Please provide a setter-function with a reference as argument!");

            static_assert(function_traits<Getter>::arg_count < 2, "Invalid number of argument, please provide a getter-function with 0 or 1 arguments.");
            static_assert(function_traits<Setter>::arg_count < 3, "Invalid number of argument, please provide a setter-function with 1 or 2 argument.");

            using raw_return_type = std::remove_cv_t<std::remove_reference_t<return_type>>;
            static_assert(std::is_same<raw_return_type, arg_type>::value, "Please provide the same signature for getter and setter!");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT override    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT override                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT override                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT override                 { return is_static_prop; }
        type get_type()     const RTTR_NOEXCEPT override                 { return type::get<typename std::remove_reference<return_type>::type*>(); }

        const variant& get_metadata(uint64_t key) const override { return metadata_handler::get_metadata(key); }

        template<bool Move>
        bool set_value_impl(instance& object, argument& arg) const
        {
            if constexpr (!is_static_prop)
            {
                using class_type = std::remove_cv_t<std::remove_reference_t<typename param_types<Setter, 0>::type>>;
                if (class_type* ptr = object.try_convert<class_type>())
                {
                    const auto& orig = m_getter(*ptr);

                    if (arg.is_type<const arg_type*>() && is_different(orig, *arg.get_value<const arg_type*>()))
                    {
                        if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                        {
                            m_setter(*ptr, *arg.get_value<const arg_type*>());
                        }
                        else
                        {
                            m_setter(*ptr, arg_type(*arg.get_value<const arg_type*>()));
                        }
                        return true;
                    }

                    if (arg.is_type<arg_type*>() && is_different(orig, *arg.get_value<arg_type*>()))
                    {
                        if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                        {
                            m_setter(*ptr, *arg.get_value<arg_type*>());
                        }
                        else if constexpr (Move)
                        {
                            m_setter(*ptr, std::move(*arg.get_value<arg_type*>()));
                        }
                        else
                        {
                            m_setter(*ptr, arg_type(*arg.get_value<arg_type*>()));
                        }
                        return true;
                    }
                }
            }
            else
            {
                const auto& orig = m_getter();

                if (arg.is_type<const arg_type*>() && is_different(orig, *arg.get_value<const arg_type*>()))
                {
                    if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                    {
                        m_setter(*arg.get_value<const arg_type*>());
                    }
                    else
                    {
                        m_setter(arg_type(*arg.get_value<const arg_type*>()));
                    }
                    return true;
                }

                if (arg.is_type<arg_type*>() && is_different(orig, *arg.get_value<arg_type*>()))
                {
                    if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                    {
                        m_setter(*arg.get_value<arg_type*>());
                    }
                    else if constexpr (Move)
                    {
                        m_setter(std::move(*arg.get_value<arg_type*>()));
                    }
                    else
                    {
                        m_setter(arg_type(*arg.get_value<arg_type*>()));
                    }
                    return true;
                }
            }
            return false;
        }

        bool set_value_copy(instance& object, argument& arg) const override { return set_value_impl<false>(object, arg); }
        bool set_value_move(instance& object, argument& arg) const override { return set_value_impl<true>(object, arg); }

        variant get_value(instance& object) const override
        {
            if constexpr (!is_static_prop)
            {
                using class_type = std::remove_cv_t<std::remove_reference_t<typename param_types<Getter, 0>::type>>;
                if (class_type* ptr = object.try_convert<class_type>())
                {
                    return variant(&(m_getter(*ptr)));
                }
                else
                {
                    return variant();
                }
            }
            else
            {
                return variant(&(m_getter()));
            }
        }

        void visit(visitor& visitor, property prop) const RTTR_NOEXCEPT override
        {
            auto obj = make_property_getter_setter_info<Declaring_Typ, return_as_ptr, Getter, Setter>(prop, m_getter, m_setter);
            visitor_iterator<Visitor_List>::visit(visitor, make_property_getter_setter_visitor_invoker(obj));
        }

    private:
        Getter  m_getter;
        Setter  m_setter;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// global function getter

template<typename Declaring_Typ, typename Getter, access_levels Acc_Level, typename Visitor_List>
class property_wrapper<function_ptr, Declaring_Typ, Getter, void, Acc_Level, return_as_ptr, read_only, Visitor_List>
    : public property_wrapper_base, public metadata_handler
{
    using return_type = typename function_traits<Getter>::return_type;
    static constexpr bool is_static_prop = function_traits<Getter>::arg_count == 0;

    public:
        property_wrapper(std::string_view name,
                         Getter get, std::vector<metadata>&& metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, type::get<Declaring_Typ>()),
            metadata_handler(std::move(metadata_list)),
            m_accessor(get)
        {
            static_assert(std::is_reference<return_type>::value, "Please provide a function with a reference as return value!");

            static_assert(function_traits<Getter>::arg_count < 2, "Invalid number of argument, please provide a getter-function with 0 or 1 arguments.");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT override { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT override { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT override { return true; }
        bool is_static()    const RTTR_NOEXCEPT override { return is_static_prop; }
        type get_type()     const RTTR_NOEXCEPT override { return type::get<typename std::add_const<typename std::remove_reference<return_type>::type>::type*>(); }

        const variant& get_metadata(uint64_t key) const override { return metadata_handler::get_metadata(key); }

        bool set_value_copy(instance& object, argument& arg) const override
        {
            return false;
        }

        bool set_value_move(instance& object, argument& arg) const override
        {
            return false;
        }

        variant get_value(instance& object) const override
        {
            if constexpr (!is_static_prop)
            {
                using class_type = std::remove_cv_t<std::remove_reference_t<typename param_types<Getter, 0>::type>>;
                if (class_type* ptr = object.try_convert<class_type>())
                {
                    return (variant(const_cast<const typename std::remove_reference<return_type>::type*>(&(m_accessor(*ptr)))));
                }
                else
                {
                    return variant();
                }
            }
            else
            {
                return variant(const_cast<const typename std::remove_reference<return_type>::type*>(&(m_accessor())));
            }
        }

        void visit(visitor& visitor, property prop) const RTTR_NOEXCEPT override
        {
            auto obj = make_property_info<Declaring_Typ, return_as_copy, Getter>(prop, m_accessor);
            visitor_iterator<Visitor_List>::visit(visitor, make_property_visitor_invoker<read_only>(obj));
        }

    private:
        Getter  m_accessor;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// global function getter/setter

template<typename Declaring_Typ, typename Getter, typename Setter, access_levels Acc_Level, typename Visitor_List>
class property_wrapper<function_ptr, Declaring_Typ, Getter, Setter, Acc_Level, get_as_ref_wrapper, set_as_ref_wrapper, Visitor_List>
    : public property_wrapper_base, public metadata_handler
{
    using return_type   = typename function_traits<Getter>::return_type;
    using true_arg_type = typename param_types<Setter, function_traits<Setter>::arg_count - 1>::type;
    using arg_type      = std::remove_const_t<std::remove_reference_t<true_arg_type>>;
    static constexpr bool is_static_prop = function_traits<Getter>::arg_count == 0;

    public:
        property_wrapper(std::string_view name,
                         Getter get, Setter set,
                         std::vector<metadata>&& metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, type::get<Declaring_Typ>()),
            metadata_handler(std::move(metadata_list)),
            m_getter(get), m_setter(set)
        {
            static_assert(std::is_reference<return_type>::value, "Please provide a getter-function with a reference as return value!");
            static_assert(std::is_reference<true_arg_type>::value, "Please provide a setter-function with a reference as argument!");

            static_assert(function_traits<Getter>::arg_count < 2, "Invalid number of argument, please provide a getter-function with 0 or 1 arguments.");
            static_assert(function_traits<Setter>::arg_count < 3, "Invalid number of argument, please provide a setter-function with 1 or 2 argument.");

            using raw_return_type = std::remove_cv_t<std::remove_reference_t<return_type>>;
            using raw_arg_type = std::remove_cv_t<std::remove_reference_t<true_arg_type>>;
            static_assert(std::is_same<raw_return_type, raw_arg_type>::value, "Please provide the same signature for getter and setter!");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT override    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT override                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT override                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT override                 { return is_static_prop; }
        type get_type()     const RTTR_NOEXCEPT override                 { return type::get< std::reference_wrapper<remove_reference_t<return_type>> >(); }

        const variant& get_metadata(uint64_t key) const override { return metadata_handler::get_metadata(key); }

        template<bool Move>
        bool set_value_impl(instance& object, argument& arg) const
        {
            if constexpr (!is_static_prop)
            {
                using class_type = std::remove_cv_t<std::remove_reference_t<typename param_types<Setter, 0>::type>>;
                if (class_type* ptr = object.try_convert<class_type>())
                {
                    const auto& orig = m_getter(*ptr);

                    if (arg.is_type<std::reference_wrapper<const arg_type>>() && is_different(orig, arg.get_value<std::reference_wrapper<const arg_type>>().get()))
                    {
                        if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                        {
                            m_setter(*ptr, arg.get_value<std::reference_wrapper<const arg_type>>().get());
                        }
                        else
                        {
                            m_setter(*ptr, arg_type(arg.get_value<std::reference_wrapper<const arg_type>>().get()));
                        }
                        return true;
                    }

                    if (arg.is_type<std::reference_wrapper<arg_type>>() && is_different(orig, arg.get_value<std::reference_wrapper<arg_type>>().get()))
                    {
                        if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                        {
                            m_setter(*ptr, arg.get_value<std::reference_wrapper<arg_type>>().get());
                        }
                        else if constexpr (Move)
                        {
                            m_setter(*ptr, std::move(arg.get_value<std::reference_wrapper<arg_type>>().get()));
                        }
                        else
                        {
                            m_setter(*ptr, arg_type(arg.get_value<std::reference_wrapper<arg_type>>().get()));
                        }
                        return true;
                    }

                    if (arg.is_type<arg_type>() && is_different(orig, arg.get_value<arg_type>()))
                    {
                        if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                        {
                            m_setter(*ptr, arg.get_value<arg_type>());
                        }
                        else if constexpr (Move)
                        {
                            m_setter(*ptr, arg.get_value<arg_type&&>());
                        }
                        else
                        {
                            m_setter(*ptr, arg_type(arg.get_value<arg_type>()));
                        }
                        return true;
                    }
                }
            }
            else
            {
                const auto& orig = m_getter();

                if (arg.is_type<std::reference_wrapper<const arg_type>>() && is_different(orig, arg.get_value<std::reference_wrapper<const arg_type>>().get()))
                {
                    if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                    {
                        m_setter(arg.get_value<std::reference_wrapper<const arg_type>>().get());
                    }
                    else
                    {
                        m_setter(arg_type(arg.get_value<std::reference_wrapper<const arg_type>>().get()));
                    }
                    return true;
                }

                if (arg.is_type<std::reference_wrapper<arg_type>>() && is_different(orig, arg.get_value<std::reference_wrapper<arg_type>>().get()))
                {
                    if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                    {
                        m_setter(arg.get_value<std::reference_wrapper<arg_type>>().get());
                    }
                    else if constexpr (Move)
                    {
                        m_setter(std::move(arg.get_value<std::reference_wrapper<arg_type>>().get()));
                    }
                    else
                    {
                        m_setter(arg_type(arg.get_value<std::reference_wrapper<arg_type>>().get()));
                    }
                    return true;
                }

                if (arg.is_type<arg_type>() && is_different(orig, arg.get_value<arg_type>()))
                {
                    if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                    {
                        m_setter(arg.get_value<arg_type>());
                    }
                    else if constexpr (Move)
                    {
                        m_setter(arg.get_value<arg_type&&>());
                    }
                    else
                    {
                        m_setter(arg_type(arg.get_value<arg_type>()));
                    }
                    return true;
                }
            }
            return false;
        }

        bool set_value_copy(instance& object, argument& arg) const override { return set_value_impl<false>(object, arg); }
        bool set_value_move(instance& object, argument& arg) const override { return set_value_impl<true>(object, arg); }

        variant get_value(instance& object) const override
        {
            if constexpr (!is_static_prop)
            {
                using class_type = std::remove_cv_t<std::remove_reference_t<typename param_types<Getter, 0>::type>>;
                if (class_type* ptr = object.try_convert<class_type>())
                {
                    return variant(std::ref(m_getter(*ptr)));
                }
                return variant();
            }
            else
            {
                return variant(std::ref(m_getter()));
            }
        }

        void visit(visitor& visitor, property prop) const RTTR_NOEXCEPT override
        {
            auto obj = make_property_getter_setter_info<Declaring_Typ, get_as_ref_wrapper, Getter, Setter>(prop, m_getter, m_setter);
            visitor_iterator<Visitor_List>::visit(visitor, make_property_getter_setter_visitor_invoker(obj));
        }

    private:
        Getter  m_getter;
        Setter  m_setter;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// global function getter/setter

template<typename Declaring_Typ, typename Getter, typename Setter, access_levels Acc_Level, typename Visitor_List>
class property_wrapper<function_ptr, Declaring_Typ, Getter, Setter, Acc_Level, get_as_ref_wrapper, set_value, Visitor_List>
    : public property_wrapper_base, public metadata_handler
{
    using return_type   = typename function_traits<Getter>::return_type;
    using true_arg_type = typename param_types<Setter, function_traits<Setter>::arg_count - 1>::type;
    using arg_type      = std::remove_cv_t<std::remove_reference_t<true_arg_type>>;
    static constexpr bool is_static_prop = function_traits<Getter>::arg_count == 0;

    public:
        property_wrapper(std::string_view name,
                         Getter get, Setter set,
                         std::vector<metadata>&& metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, type::get<Declaring_Typ>()),
            metadata_handler(std::move(metadata_list)),
            m_getter(get), m_setter(set)
        {
            static_assert(std::is_reference<return_type>::value, "Please provide a getter-function with a reference as return value!");

            static_assert(function_traits<Getter>::arg_count < 2, "Invalid number of argument, please provide a getter-function with 0 or 1 arguments.");
            static_assert(function_traits<Setter>::arg_count < 3, "Invalid number of argument, please provide a setter-function with 1 or 2 argument.");

            using raw_return_type = std::remove_cv_t<std::remove_reference_t<return_type>>;
            static_assert(std::is_same<raw_return_type, arg_type>::value, "Please provide the same signature for getter and setter!");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT override    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT override                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT override                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT override                 { return is_static_prop; }
        type get_type()     const RTTR_NOEXCEPT override                 { return type::get< std::reference_wrapper<const remove_reference_t<return_type>> >(); }

        const variant& get_metadata(uint64_t key) const override { return metadata_handler::get_metadata(key); }

        template<bool Move>
        bool set_value_impl(instance& object, argument& arg) const
        {
            if constexpr (!is_static_prop)
            {
                using class_type = std::remove_cv_t<std::remove_reference_t<typename param_types<Setter, 0>::type>>;
                if (class_type* ptr = object.try_convert<class_type>())
                {
                    const auto& orig = m_getter(*ptr);

                    if (arg.is_type<std::reference_wrapper<const arg_type>>())
                    {
                        if (is_different(orig, arg.get_value<std::reference_wrapper<const arg_type>>().get()))
                        {
                            if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                            {
                                m_setter(*ptr, arg.get_value<std::reference_wrapper<const arg_type>>().get());
                            }
                            else
                            {
                                m_setter(*ptr, arg_type(arg.get_value<std::reference_wrapper<const arg_type>>().get()));
                            }
                        }
                        return true;
                    }

                    if (arg.is_type<std::reference_wrapper<arg_type>>())
                    {
                        if (is_different(orig, arg.get_value<std::reference_wrapper<arg_type>>().get()))
                        {
                            if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                            {
                                m_setter(*ptr, arg.get_value<std::reference_wrapper<arg_type>>().get());
                            }
                            else if constexpr (Move)
                            {
                                m_setter(*ptr, std::move(arg.get_value<std::reference_wrapper<arg_type>>().get()));
                            }
                            else
                            {
                                m_setter(*ptr, arg_type(arg.get_value<std::reference_wrapper<arg_type>>().get()));
                            }
                        }
                        return true;
                    }

                    if (arg.is_type<arg_type>())
                    {
                        if (is_different(orig, arg.get_value<arg_type>()))
                        {
                            if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                            {
                                m_setter(*ptr, arg.get_value<arg_type>());
                            }
                            else if constexpr (Move)
                            {
                                m_setter(*ptr, arg.get_value<arg_type&&>());
                            }
                            else
                            {
                                m_setter(*ptr, arg_type(arg.get_value<arg_type>()));
                            }
                        }
                        return true;
                    }
                }
            }
            else
            {
                const auto& orig = m_getter();

                if (arg.is_type<std::reference_wrapper<const arg_type>>())
                {
                    if (is_different(orig, arg.get_value<std::reference_wrapper<const arg_type>>().get()))
                    {
                        if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                        {
                            m_setter(arg.get_value<std::reference_wrapper<const arg_type>>().get());
                        }
                        else
                        {
                            m_setter(arg_type(arg.get_value<std::reference_wrapper<const arg_type>>().get()));
                        }
                    }
                    return true;
                }

                if (arg.is_type<std::reference_wrapper<arg_type>>())
                {
                    if (is_different(orig, arg.get_value<std::reference_wrapper<arg_type>>().get()))
                    {
                        if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                        {
                            m_setter(arg.get_value<std::reference_wrapper<arg_type>>().get());
                        }
                        else if constexpr (Move)
                        {
                            m_setter(std::move(arg.get_value<std::reference_wrapper<arg_type>>().get()));
                        }
                        else
                        {
                            m_setter(arg_type(arg.get_value<std::reference_wrapper<arg_type>>().get()));
                        }
                    }
                    return true;
                }

                if (arg.is_type<arg_type>())
                {
                    if (is_different(orig, arg.get_value<arg_type>()))
                    {
                        if constexpr (std::is_lvalue_reference_v<true_arg_type>)
                        {
                            m_setter(arg.get_value<arg_type>());
                        }
                        else if constexpr (Move)
                        {
                            m_setter(arg.get_value<arg_type&&>());
                        }
                        else
                        {
                            m_setter(arg_type(arg.get_value<arg_type>()));
                        }
                    }
                    return true;
                }
            }
            return false;
        }

        bool set_value_copy(instance& object, argument& arg) const override { return set_value_impl<false>(object, arg); }
        bool set_value_move(instance& object, argument& arg) const override { return set_value_impl<true>(object, arg); }

        variant get_value(instance& object) const override
        {
            if constexpr (!is_static_prop)
            {
                using class_type = std::remove_cv_t<std::remove_reference_t<typename param_types<Getter, 0>::type>>;
                if (class_type* ptr = object.try_convert<class_type>())
                {
                    return variant(std::cref(m_getter(*ptr)));
                }
                return variant();
            }
            else
            {
                return variant(std::cref(m_getter()));
            }
        }

        void visit(visitor& visitor, property prop) const RTTR_NOEXCEPT override
        {
            auto obj = make_property_getter_setter_info<Declaring_Typ, get_as_ref_wrapper, Getter, Setter>(prop, m_getter, m_setter);
            visitor_iterator<Visitor_List>::visit(visitor, make_property_getter_setter_visitor_invoker(obj));
        }

    private:
        Getter  m_getter;
        Setter  m_setter;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// global function getter

template<typename Declaring_Typ, typename Getter, access_levels Acc_Level, typename Visitor_List>
class property_wrapper<function_ptr, Declaring_Typ, Getter, void, Acc_Level, get_as_ref_wrapper, read_only, Visitor_List>
    : public property_wrapper_base, public metadata_handler
{
    using return_type = typename function_traits<Getter>::return_type;
    using policy_type = std::reference_wrapper<add_const_t<remove_reference_t<return_type>>>;
    static constexpr bool is_static_prop = function_traits<Getter>::arg_count == 0;

    public:
        property_wrapper(std::string_view name,
                         Getter get, std::vector<metadata>&& metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, type::get<Declaring_Typ>()),
            metadata_handler(std::move(metadata_list)),
            m_accessor(get)
        {
            static_assert(std::is_reference<return_type>::value, "Please provide a function with a reference as return value!");

            static_assert(function_traits<Getter>::arg_count < 2, "Invalid number of argument, please provide a getter-function with 0 or 1 arguments.");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT override { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT override { return true; }
        bool is_readonly()  const RTTR_NOEXCEPT override { return true; }
        bool is_static()    const RTTR_NOEXCEPT override { return is_static_prop; }
        type get_type()     const RTTR_NOEXCEPT override { return type::get<policy_type>(); }

        const variant& get_metadata(uint64_t key) const override { return metadata_handler::get_metadata(key); }

        bool set_value_copy(instance& object, argument& arg) const override
        {
            return false;
        }

        bool set_value_move(instance& object, argument& arg) const override
        {
            return false;
        }

        variant get_value(instance& object) const override
        {
            if constexpr (!is_static_prop)
            {
                using class_type = std::remove_cv_t<std::remove_reference_t<typename param_types<Getter, 0>::type>>;
                if (class_type* ptr = object.try_convert<class_type>())
                    return variant(std::cref(m_accessor(*ptr)));
                else
                    return variant();
            }
            else
            {
                return variant(std::cref(m_accessor()));
            }
        }

        void visit(visitor& visitor, property prop) const RTTR_NOEXCEPT override
        {
            auto obj = make_property_info<Declaring_Typ, return_as_copy, Getter>(prop, m_accessor);
            visitor_iterator<Visitor_List>::visit(visitor, make_property_visitor_invoker<read_only>(obj));
        }

    private:
        Getter  m_accessor;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// global function getter

template<typename Declaring_Typ, typename Accessor, access_levels Acc_Level, typename Visitor_List>
class property_wrapper<function_ptr, Declaring_Typ, Accessor, void, Acc_Level, get_as_ref_wrapper, set_as_ref_wrapper, Visitor_List>
    : public property_wrapper_base, public metadata_handler
{
    using return_type   = typename function_traits<Accessor>::return_type;
    using arg_type      = std::remove_cv_t<std::remove_reference_t<return_type>>;
    using policy_type   = std::reference_wrapper<remove_reference_t<return_type>>;
    static constexpr bool is_static_prop = function_traits<Accessor>::arg_count == 0;

    public:
        property_wrapper(std::string_view name,
                         Accessor acc,
                         std::vector<metadata>&& metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, type::get<Declaring_Typ>()),
            metadata_handler(std::move(metadata_list)),
            m_accessor(acc)
        {
            static_assert(function_traits<Accessor>::arg_count < 2, "Invalid number of argument, please provide a getter-function with 0 or 1 arguments.");
            static_assert(std::is_lvalue_reference<return_type>::value, "Please provide a getter function with a reference as return value!");
            static_assert(!std::is_const<std::remove_reference_t<return_type>>::value, "Please provide a getter function with a reference as return value!");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT override    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT override                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT override                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT override                 { return is_static_prop; }
        type get_type()     const RTTR_NOEXCEPT override                 { return type::get<policy_type>(); }

        const variant& get_metadata(uint64_t key) const override { return metadata_handler::get_metadata(key); }

        template<bool Move>
        bool set_value_impl(instance& object, argument& arg) const
        {
            if constexpr (!is_static_prop)
            {
                using class_type = std::remove_cv_t<std::remove_reference_t<typename param_types<Accessor, 0>::type>>;
                if (class_type* ptr = object.try_convert<class_type>())
                {
                    if (arg.is_type<std::reference_wrapper<const arg_type>>())
                    {
                        property_accessor<arg_type>::set_value_copy(m_accessor(*ptr), arg.get_value<std::reference_wrapper<const arg_type>>().get());
                        return true;
                    }

                    if (arg.is_type<std::reference_wrapper<arg_type>>())
                    {
                        if constexpr (Move)
                        {
                            property_accessor<arg_type>::set_value_move(m_accessor(*ptr), std::move(arg.get_value<std::reference_wrapper<arg_type>>().get()));
                        }
                        else
                        {
                            property_accessor<arg_type>::set_value_copy(m_accessor(*ptr), arg.get_value<std::reference_wrapper<arg_type>>().get());
                        }
                        return true;
                    }

                    if (arg.is_type<arg_type>())
                    {
                        if constexpr (Move)
                        {
                            property_accessor<arg_type>::set_value_move(m_accessor(*ptr), arg.get_value<arg_type&&>());
                        }
                        else
                        {
                            property_accessor<arg_type>::set_value_copy(m_accessor(*ptr), arg.get_value<arg_type>());
                        }
                        return true;
                    }
                }
            }
            else
            {
                if (arg.is_type<std::reference_wrapper<const arg_type>>())
                {
                    property_accessor<arg_type>::set_value_copy(m_accessor(), arg.get_value<std::reference_wrapper<const arg_type>>().get());
                    return true;
                }

                if (arg.is_type<std::reference_wrapper<arg_type>>())
                {
                    if constexpr (Move)
                    {
                        property_accessor<arg_type>::set_value_move(m_accessor(), std::move(arg.get_value<std::reference_wrapper<arg_type>>().get()));
                    }
                    else
                    {
                        property_accessor<arg_type>::set_value_copy(m_accessor(), arg.get_value<std::reference_wrapper<arg_type>>().get());
                    }
                    return true;
                }

                if (arg.is_type<arg_type>())
                {
                    if constexpr (Move)
                    {
                        property_accessor<arg_type>::set_value_move(m_accessor(), arg.get_value<arg_type&&>());
                    }
                    else
                    {
                        property_accessor<arg_type>::set_value_copy(m_accessor(), arg.get_value<arg_type>());
                    }
                    return true;
                }
            }
            return false;
        }

        bool set_value_copy(instance& object, argument& arg) const override { return set_value_impl<false>(object, arg); }
        bool set_value_move(instance& object, argument& arg) const override { return set_value_impl<true>(object, arg); }

        variant get_value(instance& object) const override
        {
            if constexpr (!is_static_prop)
            {
                using class_type = std::remove_cv_t<std::remove_reference_t<typename param_types<Accessor, 0>::type>>;
                if (class_type* ptr = object.try_convert<class_type>())
                {
                    return variant(std::ref(m_accessor(*ptr)));
                }
                return variant();
            }
            else
            {
                return variant(std::ref(m_accessor()));
            }
        }

        void visit(visitor& visitor, property prop) const RTTR_NOEXCEPT override
        {
            auto obj = make_property_info<Declaring_Typ, get_as_ref_wrapper, Accessor>(prop, m_accessor);
            visitor_iterator<Visitor_List>::visit(visitor, make_property_visitor_invoker(obj));
        }

    private:
        Accessor m_accessor;
};

#endif // RTTR_PROPERTY_WRAPPER_FUNC_H_
