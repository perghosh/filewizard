#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#ifndef _GD_FILE_BEGIN

#  define _GD_FILE_BEGIN namespace gd { namespace file {
#  define _GD_FILE_END } }

#endif

_GD_FILE_BEGIN

// ## `closest` are used to find nearest folder in the parent hierarchy

extern std::pair<bool, std::string> closest_having_file_g(const std::string_view& stringPath, const std::string_view& stringFindFile);
// extern std::pair<bool, std::string> closest_having_text_g(const std::string_view& stringPath, const std::string_view& stringFindFile, const std::string_view& stringFindText);



_GD_FILE_END

