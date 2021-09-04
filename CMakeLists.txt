# CMakeList.txt : CMake project for filewizard, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project("root")

set(CMAKE_CXX_STANDARD 20)

set(Boost_INCLUDE_DIR ${_VCPKG_INSTALLED_DIR}/x64-windows/include)     # _VCPKG_INSTALLED_DIR is set when vcpkg is installed


add_subdirectory("application/console")
add_subdirectory("tests")



# TODO: Add tests and install targets if needed.
