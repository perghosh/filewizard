#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#include "gd_arguments.h"
#include "gd_variant.h"
#include "gd_variant_view.h"

#ifndef _GD_SQL_QUERY_BEGIN
#  define _GD_SQL_QUERY_BEGIN namespace gd { namespace sql {
#  define _GD_SQL_QUERY_END } }
#endif

_GD_SQL_QUERY_BEGIN

void append_g( const gd::variant& variantValue, std::string& stringSql );
void append_g( const gd::variant_view& variantValue, std::string& stringSql );

_GD_SQL_QUERY_END