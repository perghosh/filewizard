# CMakeList.txt : CMake project for filewizard, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)


project ("fw_test")

# print compiler options for visual studio
include( "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/CompilerOptions.cmake" )

set( source_cpp 
   "../source/gd_utf8.cpp"
   "../source/gd_utf8_string.cpp"
)

#  ${CMAKE_CURRENT_SOURCE_DIR}/../libraries/catch2/catch_amalgamated.cpp


set( catch_test 
   "catch.h"
   "catch_text.cpp"
)

message( ${CMAKE_PREFIX_PATH} )




# Add source to this project's executable.
add_executable ("fw_test" "fw_test.cpp" "fw_test.h" "${source_cpp}" "${catch_test}")
find_package(Catch2 CONFIG REQUIRED)


target_link_libraries("fw_test" PRIVATE Catch2::Catch2)

# target_compile_options( "fw_test" PRIVATE /Zc:hiddenFriend- )

# add general project folder to current project
target_include_directories("fw_test" PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories("fw_test" PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../source)


# TODO: Add tests and install targets if needed.
