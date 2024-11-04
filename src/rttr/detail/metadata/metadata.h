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

#ifndef RTTR_METADATA_H_
#define RTTR_METADATA_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/variant.h"

namespace rttr
{
namespace detail
{

/*!
 * This class holds meta data.
 *
 */
class RTTR_API metadata
{
    public:
        metadata() { }
        metadata(uint64_t key, variant value) : m_key(key), m_value(std::move(value)) { }
        metadata(const metadata& other) : m_key(other.m_key), m_value(other.m_value) {}
        metadata(metadata&& other) : m_key(other.m_key), m_value(std::move(other.m_value)) {}
        metadata& operator=(const metadata& other) { m_key = other.m_key; m_value = other.m_value; return *this; }

        uint64_t       get_key() const      { return m_key; }
        const variant& get_value() const    { return m_value; }

    private:
        uint64_t m_key;
        variant  m_value;
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_METADATA_H_
