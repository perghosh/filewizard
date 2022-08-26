#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#include <fcntl.h>

#ifndef _GD_FILE_BEGIN

#  define _GD_FILE_BEGIN namespace gd { namespace file {
#  define _GD_FILE_END } }

#endif

_GD_FILE_BEGIN

// ## folder operations

/// gets known folder path for folder name
std::pair<bool, std::wstring> get_known_folder_wpath_g(const std::string_view& stringFolderId);

// ## `closest` are used to find nearest folder in the parent hierarchy

std::pair<bool, std::string> closest_having_file_g(const std::string_view& stringPath, const std::string_view& stringFindFile);

// ## `file` operations

// ### 

std::pair<int, std::string> file_add_reference_g(const std::string_view& stringFindName);
std::pair<int, std::string> file_release_reference_g(const std::string_view& stringFindName);



_GD_FILE_END

