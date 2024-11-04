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

#ifndef RTTR_PROPERTY_WRAPPER_MEMBER_OBJECT_H_
#define RTTR_PROPERTY_WRAPPER_MEMBER_OBJECT_H_

#include "rttr/detail/policies/prop_policies.h"
#include "rttr/detail/type/accessor_type.h"

#include "rttr/detail/property/property_wrapper.h"

using namespace rttr::detail;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// pointer to member - read write

template<typename Declaring_Typ, typename C, typename A, access_levels Acc_Level, typename Visitor_List>
class property_wrapper<member_object_ptr, Declaring_Typ, A(C::*), void, Acc_Level, return_as_copy, set_value, Visitor_List>
    : public property_wrapper_base, public metadata_handler
{
    using accessor   = A (C::*);

    public:
        property_wrapper(std::string_view name,
                         accessor acc, std::vector<metadata>&& metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, type::get<Declaring_Typ>()),
            metadata_handler(std::move(metadata_list)),
            m_acc(acc)
        {
            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT override    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT override                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT override                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT override                 { return false; }
        type get_type()     const RTTR_NOEXCEPT override                 { return type::get<A>(); }

        const variant& get_metadata(uint64_t key) const override { return metadata_handler::get_metadata(key); }

        template<bool Move>
        bool set_value_impl(instance& object, argument& arg) const
        {
            if (C* ptr = object.try_convert<C>())
            {
                if (arg.is_type<std::reference_wrapper<const A>>())
                {
                    return property_accessor<A>::set_value_copy((ptr->*m_acc), arg.get_value<std::reference_wrapper<const A>>().get());
                }

                if (arg.is_type<std::reference_wrapper<A>>())
                {
                    if constexpr (Move)
                    {
                        return property_accessor<A>::set_value_move((ptr->*m_acc), std::move(arg.get_value<std::reference_wrapper<A>>().get()));
                    }
                    else
                    {
                        return property_accessor<A>::set_value_copy((ptr->*m_acc), arg.get_value<std::reference_wrapper<A>>().get());
                    }
                }

                if (arg.is_type<A>())
                {
                    if constexpr (Move)
                    {
                        return property_accessor<A>::set_value_move((ptr->*m_acc), arg.get_value<A&&>());
                    }
                    else
                    {
                        return property_accessor<A>::set_value_copy((ptr->*m_acc), arg.get_value<A>());
                    }
                }
            }
            return false;
        }

        bool set_value_copy(instance& object, argument& arg) const override { return set_value_impl<false>(object, arg); }
        bool set_value_move(instance& object, argument& arg) const override { return set_value_impl<true>(object, arg); }

        variant get_value(instance& object) const override
        {
            if (C* ptr = object.try_convert<C>())
            {
                return variant((ptr->*m_acc));
            }
            return variant();
        }

        void visit(visitor& visitor, property prop) const RTTR_NOEXCEPT override
        {
            auto obj = make_property_info<Declaring_Typ, return_as_copy, accessor>(prop, m_acc);
            visitor_iterator<Visitor_List>::visit(visitor, make_property_visitor_invoker(obj));
        }

    private:
        accessor m_acc;
};


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// pointer to member

template<typename Declaring_Typ, typename C, typename A, access_levels Acc_Level, typename Visitor_List>
class property_wrapper<member_object_ptr, Declaring_Typ, A(C::*), void, Acc_Level, return_as_copy, read_only, Visitor_List>
    : public property_wrapper_base, public metadata_handler
{
    using accessor = A (C::*);
    public:
        property_wrapper(std::string_view name,
                         accessor acc, std::vector<metadata>&& metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, type::get<Declaring_Typ>()),
            metadata_handler(std::move(metadata_list)),
            m_acc(acc)
        {
            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT override    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT override                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT override                 { return true; }
        bool is_static()    const RTTR_NOEXCEPT override                 { return false; }
        type get_type()     const RTTR_NOEXCEPT override                 { return type::get<A>(); }

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
            if (C* ptr = object.try_convert<C>())
                return variant(ptr->*m_acc);
            else
                return variant();
        }

        void visit(visitor& visitor, property prop) const RTTR_NOEXCEPT override
        {
            auto obj = make_property_info<Declaring_Typ, return_as_copy, accessor>(prop, m_acc);
            visitor_iterator<Visitor_List>::visit(visitor, make_property_visitor_invoker<read_only>(obj));
        }

    private:
        accessor m_acc;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// pointer to member - read write

template<typename Declaring_Typ, typename C, typename A, access_levels Acc_Level, typename Visitor_List>
class property_wrapper<member_object_ptr, Declaring_Typ, A(C::*), void, Acc_Level, return_as_ptr, set_as_ptr, Visitor_List>
    : public property_wrapper_base, public metadata_handler
{
    using accessor = A (C::*);
    public:
        property_wrapper(std::string_view name,
                         accessor acc, std::vector<metadata>&& metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, type::get<Declaring_Typ>()),
            metadata_handler(std::move(metadata_list)),
            m_acc(acc)
        {
            static_assert(!std::is_pointer<A>::value, "The data type of the property is already a pointer type! The given policy cannot be used for this property.");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT override    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT override                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT override                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT override                 { return false; }
        type get_type()     const RTTR_NOEXCEPT override                 { return type::get<A*>(); }

        const variant& get_metadata(uint64_t key) const override { return metadata_handler::get_metadata(key); }

        template<bool Move>
        bool set_value_impl(instance& object, argument& arg) const
        {
            if (C* ptr = object.try_convert<C>())
            {
                if (arg.is_type<const A*>())
                {
                    return property_accessor<A>::set_value_copy((ptr->*m_acc), *arg.get_value<const A*>());
                }

                if (arg.is_type<A*>())
                {
                    if constexpr (Move)
                    {
                        return property_accessor<A>::set_value_move((ptr->*m_acc), std::move(*arg.get_value<A*>()));
                    }
                    else
                    {
                        return property_accessor<A>::set_value_copy((ptr->*m_acc), *arg.get_value<A*>());
                    }
                }
            }
            return false;
        }

        bool set_value_copy(instance& object, argument& arg) const override { return set_value_impl<false>(object, arg); }
        bool set_value_move(instance& object, argument& arg) const override { return set_value_impl<true>(object, arg); }

        variant get_value(instance& object) const override
        {
            if (C* ptr = object.try_convert<C>())
            {
                return variant(&(ptr->*m_acc));
            }
            return variant();
        }

        void visit(visitor& visitor, property prop) const RTTR_NOEXCEPT override
        {
            auto obj = make_property_info<Declaring_Typ, return_as_ptr, accessor>(prop, m_acc);
            visitor_iterator<Visitor_List>::visit(visitor, make_property_visitor_invoker(obj));
        }

    private:
        accessor m_acc;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// pointer to member - read only

template<typename Declaring_Typ, typename C, typename A, access_levels Acc_Level, typename Visitor_List>
class property_wrapper<member_object_ptr, Declaring_Typ, A(C::*), void, Acc_Level, return_as_ptr, read_only, Visitor_List>
    : public property_wrapper_base, public metadata_handler
{
    using accessor = A (C::*);
    public:
        property_wrapper(std::string_view name,
                         accessor acc, std::vector<metadata>&& metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, type::get<Declaring_Typ>()),
            metadata_handler(std::move(metadata_list)), m_acc(acc)
        {
            static_assert(!std::is_pointer<A>::value, "The data type of the property is already a pointer type! The given policy cannot be used for this property.");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT override    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT override                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT override                 { return true; }
        bool is_static()    const RTTR_NOEXCEPT override                 { return false; }
        type get_type()     const RTTR_NOEXCEPT override                 { return type::get<typename std::add_const<A>::type*>(); }

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
            if (C* ptr = object.try_convert<C>())
                return variant(const_cast<const A*>(&(ptr->*m_acc)));
            else
                return variant();
        }

        void visit(visitor& visitor, property prop) const RTTR_NOEXCEPT override
        {
            auto obj = make_property_info<Declaring_Typ, return_as_ptr, accessor>(prop, m_acc);
            visitor_iterator<Visitor_List>::visit(visitor, make_property_visitor_invoker<read_only>(obj));
        }

    private:
        accessor m_acc;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// pointer to member - read write

template<typename Declaring_Typ, typename C, typename A, access_levels Acc_Level, typename Visitor_List>
class property_wrapper<member_object_ptr, Declaring_Typ, A(C::*), void, Acc_Level, get_as_ref_wrapper, set_as_ref_wrapper, Visitor_List>
    : public property_wrapper_base, public metadata_handler
{
    using accessor = A (C::*);

    public:
        property_wrapper(std::string_view name,
                         accessor acc, std::vector<metadata>&& metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, type::get<Declaring_Typ>()),
            metadata_handler(std::move(metadata_list)),
            m_acc(acc)
        {
            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT override    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT override                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT override                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT override                 { return false; }
        type get_type()     const RTTR_NOEXCEPT override                 { return type::get<std::reference_wrapper<A>>(); }

        const variant& get_metadata(uint64_t key) const override { return metadata_handler::get_metadata(key); }

        template<bool Move>
        bool set_value_impl(instance& object, argument& arg) const
        {
            if (C* ptr = object.try_convert<C>())
            {
                if (arg.is_type<std::reference_wrapper<const A>>())
                {
                    return property_accessor<A>::set_value_copy((ptr->*m_acc), arg.get_value<std::reference_wrapper<const A>>().get());
                }

                if (arg.is_type<std::reference_wrapper<A>>())
                {
                    if constexpr (Move)
                    {
                        return property_accessor<A>::set_value_move((ptr->*m_acc), std::move(arg.get_value<std::reference_wrapper<A>>().get()));
                    }
                    else
                    {
                        return property_accessor<A>::set_value_copy((ptr->*m_acc), arg.get_value<std::reference_wrapper<A>>().get());
                    }
                }

                if (arg.is_type<A>())
                {
                    if constexpr (Move)
                    {
                        return property_accessor<A>::set_value_move((ptr->*m_acc), arg.get_value<A&&>());
                    }
                    else
                    {
                        return property_accessor<A>::set_value_copy((ptr->*m_acc), arg.get_value<A>());
                    }
                }
            }

            return false;
        }

        bool set_value_copy(instance& object, argument& arg) const override { return set_value_impl<false>(object, arg); }
        bool set_value_move(instance& object, argument& arg) const override { return set_value_impl<true>(object, arg); }

        variant get_value(instance& object) const override
        {
            if (C* ptr = object.try_convert<C>())
            {
                return variant(std::ref(ptr->*m_acc));
            }
            return variant();
        }

        void visit(visitor& visitor, property prop) const RTTR_NOEXCEPT override
        {
            auto obj = make_property_info<Declaring_Typ, get_as_ref_wrapper, accessor>(prop, m_acc);
            visitor_iterator<Visitor_List>::visit(visitor, make_property_visitor_invoker(obj));
        }

    private:
        accessor m_acc;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// pointer to member - read only

template<typename Declaring_Typ, typename C, typename A, access_levels Acc_Level, typename Visitor_List>
class property_wrapper<member_object_ptr, Declaring_Typ, A(C::*), void, Acc_Level, get_as_ref_wrapper, read_only, Visitor_List>
    : public property_wrapper_base, public metadata_handler
{
    using accessor = A (C::*);
    public:
        property_wrapper(std::string_view name,
                         accessor acc, std::vector<metadata>&& metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, type::get<Declaring_Typ>()),
            metadata_handler(std::move(metadata_list)), m_acc(acc)
        {
            init();
        }
        using policy_type = std::reference_wrapper<add_const_t<A>>;
        access_levels get_access_level() const RTTR_NOEXCEPT override    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT override                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT override                 { return true; }
        bool is_static()    const RTTR_NOEXCEPT override                 { return false; }
        type get_type()     const RTTR_NOEXCEPT override                 { return type::get< std::reference_wrapper<add_const_t<A>> >(); }

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
            if (C* ptr = object.try_convert<C>())
                return variant(std::cref((ptr->*m_acc)));
            else
                return variant();
        }

        void visit(visitor& visitor, property prop) const RTTR_NOEXCEPT override
        {
            auto obj = make_property_info<Declaring_Typ, get_as_ref_wrapper, accessor>(prop, m_acc);
            visitor_iterator<Visitor_List>::visit(visitor, make_property_visitor_invoker<read_only>(obj));
        }

    private:
        accessor m_acc;
};

#endif // RTTR_PROPERTY_WRAPPER_MEMBER_OBJECT_H_
