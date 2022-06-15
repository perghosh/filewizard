#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#include "gd_variant.h"
#include "gd_utf8.hpp"
#include "gd_calculate_parse.h"


#ifndef _GD_CALCULATE_BEGIN
namespace gd::calculate {
#else
_GD_CALCULATE_BEGIN
#endif


} // namespace _GD_CALCULATE_PARSE_BEGIN

std::pair<unsigned, const char*> gd::calculate::calculate_parse::get_token_s(const char* pbszPosition)
{

   return { 0, "" };
}
