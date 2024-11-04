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

#ifndef RTTR_METADATA_HANDLER_H_
#define RTTR_METADATA_HANDLER_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/metadata/metadata.h"
#include "rttr/variant.h"

#include <vector>

namespace rttr
{
namespace detail
{

RTTR_API const variant& get_empty_variant();

/*!
 * This class holds an index to possible meta data.
 * This can be also a name or the declaring type of a property or method.
 */
class metadata_handler
{
    public:
        RTTR_FORCE_INLINE metadata_handler(std::vector<metadata>&& new_data)
            : m_metadata_list(std::move(new_data))
        {
            std::sort(m_metadata_list.begin(), m_metadata_list.end(), [](const detail::metadata& left, const detail::metadata& right) { return left.get_key() < right.get_key(); });
            auto last = std::unique(m_metadata_list.begin(), m_metadata_list.end(), [](const detail::metadata& left, const detail::metadata& right) { return left.get_key() == right.get_key(); });
            m_metadata_list.erase(last, m_metadata_list.end());
        }

        RTTR_INLINE const variant& get_metadata(uint64_t key) const
        {
            const auto it = std::lower_bound(m_metadata_list.begin(), m_metadata_list.end(), key, [](const detail::metadata& meta, uint64_t key) { return meta.get_key() < key; });
            if (it != m_metadata_list.end() && key == it->get_key())
            {
                return it->get_value();
            }

            return get_empty_variant();
        }

    private:
        std::vector<metadata> m_metadata_list;
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_METADATA_HANDLER_H_
