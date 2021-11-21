#include "catch.hpp"

#include "sol.hpp"

#include "gd_utf8.hpp"
#include "gd_utf8_string.hpp"



TEST_CASE("Initialize lua", "[lua]") {
   sol::state lua;
   int x = 0;
   lua.set_function("beep", [&x] { ++x; });
   lua.script("beep()");
   assert(x == 1);
}
