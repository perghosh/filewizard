#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#include "gd_variant.h"
#include "gd_variant_view.h"


#ifndef _GD_CALCULATE_CONSTANT_BEGIN
namespace gd::calculate {
#else
_GD_PARAM_BEGIN
#endif

namespace type {
   enum enumGroup
   {
      eGroupNumber   = 0x0100'0000,
      eGroupString   = 0x0200'0000,
      eGroupArray    = 0x0300'0000,
      eGroupObject   = 0x0400'0000,
      eGroupOperator = 0x0500'0000,
      eGroupVariable = 0x0600'0000,
      eGroupCommand  = 0x0700'0000,
      eGroupFlow     = 0x0800'0000,
   };
}



} // namespace _GD_CALCULATE_PARSE_BEGIN