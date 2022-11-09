
/**
 * \file gd_file.h
 * 
 * \brief Miscellaneous file operations 
 * 
 */


#pragma once

#include <cassert>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#include <fcntl.h>

#include "gd_arguments.h"

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

// ## files in folder

std::vector<std::string> list_files_g(const std::string_view& stringFolder, const gd::argument::arguments& argumentsFilter);
inline std::vector<std::string> list_files_g(const std::string_view& stringFolder) { return list_files_g( stringFolder, gd::argument::arguments() ); }

// ## `file` operations

// ### 

std::pair<int, std::string> file_add_reference_g(const std::string_view& stringFindName);

//std::pair<int, std::string> file_release_reference_g(const std::string_view& stringFindName);



_GD_FILE_END

