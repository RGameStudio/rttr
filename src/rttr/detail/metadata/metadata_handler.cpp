#include "rttr/detail/metadata/metadata_handler.h"
#include "rttr/variant.h"

namespace rttr::detail
{

const variant& get_empty_variant()
{
    static const variant dummy;
    return dummy;
}

} // end namespace rttr::detail
