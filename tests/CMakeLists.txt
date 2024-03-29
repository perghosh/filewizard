﻿# CMakeList.txt : CMake project for filewizard, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)


project ("fw_test")

# print compiler options for visual studio
include( "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/CompilerOptions.cmake" )

# set(BOOST_ROOT "${vcpkg_repo}/vcpkg/installed/x64-windows") 


set( source_cpp 
   "../source/gd_utf8.cpp"
   "../source/gd_utf8_string.cpp"
   "../source/application_file.cpp"
   "../source/application.cpp"
)

#  ${CMAKE_CURRENT_SOURCE_DIR}/../libraries/catch2/catch_amalgamated.cpp


set( catch_test 
   "catch.h"
   "catch_text.cpp"
   "catch_file.cpp"
   "catch_application.cpp"
   "catch_lua.cpp"
)

message( ${CMAKE_PREFIX_PATH} )




# Add source to this project's executable.
add_executable ("fw_test" "fw_test.cpp" "fw_test.h" "${source_cpp}" "${catch_test}")

target_compile_definitions( "fw_test" PUBLIC DEBUG )

find_package(Catch2 CONFIG REQUIRED)

find_package(Boost 1.75.0 COMPONENTS regex REQUIRED)
if( Boost_FOUND )
   message(" [INFO] Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}")
   message(" [INFO] Boost_LIBRARY_DIRS = ${Boost_LIBRARY_DIRS}")
endif()
set(Boost_USE_MULTITHREADED ON)

message(STATUS "Boost version: ${Boost_VERSION}")

message(STATUS "folder: ${Boost_LIBRARY_DIRS}")
message(STATUS "folder debug: ${Boost_LIBRARY_DIRS_DEBUG}")


target_link_libraries("fw_test" PRIVATE Catch2::Catch2)

# target_compile_options( "fw_test" PRIVATE /Zc:hiddenFriend- )

# Add boost to project
target_include_directories( "fw_test" PRIVATE ${BOOST_ROOT} )
target_link_directories( "fw_test" INTERFACE "${Boost_LIBRARY_DIRS}" )
# target_link_libraries( "fw_test" PUBLIC "${Boost_LIBRARY_DIRS}" )
target_link_libraries("fw_test" PRIVATE ${Boost_LIBRARIES})

target_include_directories( "fw_test" PRIVATE ${Boost_INCLUDE_DIRS} )


# add general project folder to current project
target_include_directories("fw_test" PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories("fw_test" PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../source)




# TODO: Add tests and install targets if needed.
