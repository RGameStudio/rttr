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

#ifndef RTTR_RTTR_ENABLE_H_
#define RTTR_RTTR_ENABLE_H_

#include <type_traits>

#include "rttr/type.h"
#include "rttr/type_list.h"

#ifdef DOXYGEN

/*!
 * \brief This macro is necessary in order to retrieve type information about the
 *        inheritance graph of a class. When there is no inheritance graph, the macro is **not** needed, e.g. for POD types.
 *
 * Put the macro inside a base class, where you need the complete inheritance information about the class type.
 *
 * \code{.cpp}
 * struct Base
 * {
 *   RTTR_DECLARE_ROOT()
 * };
 * \endcode
 *
 * \remark Without this macro, it will not be possible to use meta information in the type class,
 *         like: \ref rttr::type::get_base_classes() "get_base_classes()" or
 *         \ref rttr::type::get_derived_classes() "get_derived_classes()".
 */
#define RTTR_DECLARE_ROOT()

/*!
 * \brief This macro is necessary in order to retrieve type information about the
 *        inheritance graph of a class. When there is no inheritance graph, the macro is **not** needed, e.g. for POD types.
 *
 * Put the macro inside every child class, where you need the complete inheritance information about the class type.
 *
 * \code{.cpp}
 * struct Base
 * {
 *   RTTR_DECLARE_ROOT()
 * };
 *
 * struct Derived : Base
 * {
 *   RTTR_DECLARE_ANCESTORS(Base)
 * };
 * \endcode
 *
 *  When you use multiple inheritance you simply separate every class with a comma.
 * \code{.cpp}
 *   struct MultipleDerived : Base, Other
 *   {
 *     RTTR_DECLARE_ANCESTORS(Base, Other)
 *   };
 * \endcode
 *
 * \remark Without this macro, it will not be possible to use meta information in the type class,
 *         like: \ref rttr::type::get_base_classes() "get_base_classes()" or
 *         \ref rttr::type::get_derived_classes() "get_derived_classes()".
 */
#define RTTR_DECLARE_ANCESTORS(...)

/*!
 * \brief This macro is necessary in order to retrieve concrete object type by reference to it's type or it's type base.
 *
 * Put the macro inside every polymorphic class where runtime type information is needed.
 *
 * \code{.cpp}
 * struct Base
 * {
 *   RTTR_DECLARE_ROOT()
 *   RTTR_ENABLE_OBJECT_INFO()
 * };
 *
 * struct Derived : Base
 * {
 *   RTTR_DECLARE_ANCESTORS(Base)
 *   RTTR_ENABLE_OBJECT_INFO()
 * };
 * 
 * Base* b = new Derived();
 * std::cout << "object type: " << b->get_type().get_name(); // will print "object type: Derived"
 * \endcode
 *
 * Place the macro \ref RTTR_ENABLE_OBJECT_INFO() somewhere in the class, it doesn't matter if its under the public,
 * protected or private class accessor section. The macro will close itself with a `private` visibility.
 * So when you not specify anything afterwords, everything will be `private`.
 *
 * If one of base classes contains macro \ref RTTR_ENABLE_OBJECT_INFO() all child classes must contain \ref RTTR_ENABLE_OBJECT_INFO() macro.
 * \code{.cpp}
 * struct Base
 * {
 *   RTTR_DECLARE_ROOT()
 *   RTTR_ENABLE_OBJECT_INFO()
 * };
 *
 * struct DerivedA: Base
 * {
 *   RTTR_DECLARE_ANCESTORS(Base)
 *   RTTR_ENABLE_OBJECT_INFO()
 * };
 *
 * struct DerivedB: Base
 * {
 *   RTTR_DECLARE_ANCESTORS(Base)
 *   RTTR_ENABLE_OBJECT_INFO()
 * };
 *
 * struct DerivedC: Base
 * {
 *   RTTR_DECLARE_ANCESTORS(Base)
 *   RTTR_ENABLE_OBJECT_INFO()
 * };
 * \endcode
 *
 * \remark Without this macro, it will not be possible to use \ref rttr::rttr_cast "rttr_cast".
 */
#define RTTR_ENABLE_OBJECT_INFO()

/*!
 * \brief This macro is necessary in order to retrieve concrete object type by reference to it's type or it's type base.
 *        This macro is useful for CRTP cases when you want skip CRTP wrapper in inheritance graph.
 * 
 * Put the macro inside every polymorphic class where runtime type information is needed.
 *
 * \code{.cpp}
 * struct Base
 * {
 *   RTTR_DECLARE_ROOT()
 *   RTTR_ENABLE_OBJECT_INFO()
 * };
 *
 * template<typename T>
 * struct DerivedBase : Base
 * {
 *   RTTR_DECLARE_ANCESTORS(Base)
 *   RTTR_ENABLE_OBJECT_INFO_AS(T)
 * };
 * 
 * struct DerivedA : DerivedBase<DerivedA> {};
 * struct DerivedB : DerivedBase<DerivedB> {};
 * \endcode
 *
 * Place the macro \ref RTTR_ENABLE_OBJECT_INFO_AS() somewhere in the class, it doesn't matter if its under the public,
 * protected or private class accessor section. The macro will close itself with a `private` visibility.
 * So when you not specify anything afterwords, everything will be `private`.
 *
 * \remark Without this macro, it will not be possible to use \ref rttr::rttr_cast "rttr_cast".
 */
#define RTTR_ENABLE_OBJECT_INFO_AS(Type)

#else

#define TYPE_LIST(...)      ::rttr::type_list<__VA_ARGS__>

#define RTTR_DECLARE_ROOT() \
public: \
    using base_class_list = TYPE_LIST(); \
private:

#define RTTR_DECLARE_ANCESTORS(...) \
public: \
    using base_class_list = TYPE_LIST(__VA_ARGS__); \
private:

#define RTTR_ENABLE_OBJECT_INFO() \
public: \
RTTR_BEGIN_DISABLE_OVERRIDE_WARNING \
    virtual RTTR_INLINE ::rttr::type get_type() const { return ::rttr::detail::get_type_from_instance(this); } \
    virtual RTTR_INLINE void* get_ptr() { return reinterpret_cast<void*>(this); } \
    virtual RTTR_INLINE ::rttr::detail::derived_info get_derived_info() { return { reinterpret_cast<void*>(this), ::rttr::detail::get_type_from_instance(this)}; } \
RTTR_END_DISABLE_OVERRIDE_WARNING \
private:

#define RTTR_ENABLE_OBJECT_INFO_AS(Type) \
public: \
RTTR_BEGIN_DISABLE_OVERRIDE_WARNING \
	virtual RTTR_INLINE ::rttr::type get_type() const { return ::rttr::detail::get_type_from_instance(static_cast<const Type*>(this)); } \
	virtual RTTR_INLINE void* get_ptr() { return reinterpret_cast<void*>(static_cast<Type*>(this)); } \
	virtual RTTR_INLINE ::rttr::detail::derived_info get_derived_info() { return { reinterpret_cast<void*>(static_cast<Type*>(this)), ::rttr::detail::get_type_from_instance(static_cast<const Type*>(this))}; } \
RTTR_END_DISABLE_OVERRIDE_WARNING \
private:

#endif // DOXYGEN

#endif // RTTR_RTTR_ENABLE_H_
