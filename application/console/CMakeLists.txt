# CMakeList.txt : CMake project for filewizard, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)

project ("fw_console")

set( source_cpp "../../source/gd_utf8.cpp"
)

# Add source to this project's executable.
add_executable ("fw_console" "filewizard.cpp" "filewizard.h" "${source_cpp}")

# add general project folder to current project
target_include_directories("fw_console" PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
target_include_directories("fw_console" PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../source)


# TODO: Add tests and install targets if needed.
