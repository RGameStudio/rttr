#ifndef RTTR_TEMPLATE_ARGUMENT_DATA_H_
#define RTTR_TEMPLATE_ARGUMENT_DATA_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/type.h"

namespace rttr
{
struct RTTR_API template_argument_data
{
	type m_type;
	bool m_templateParam;
};

} // end namespace rttr

#endif // RTTR_TEMPLATE_ARGUMENT_DATA_H_