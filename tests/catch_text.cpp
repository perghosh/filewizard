#include "catch.hpp"

#include "gd_utf.hpp"

TEST_CASE("utf8 move operations", "[utf8]") {
   char pBuffer[100] = { 0 };
   const char* p1 = "01234567890123456789012345678901234567890123456789";

   p1 = gd::utf8::move::next(p1); REQUIRE(*p1 == '1');
   p1 = gd::utf8::move::next(p1, 9); REQUIRE(*p1 == '0');
   p1 = gd::utf8::move::previous(p1, 1); REQUIRE(*p1 == '9');
   p1 = gd::utf8::move::previous(p1); REQUIRE(*p1 == '8');
   

   const char* pAscii = (const char*)u8"AAA. Xåäö.XÅÄÖ.X ©¢¥¹ X";

   p1 = pAscii; 
   p1 = gd::utf8::move::next(p1, 5); REQUIRE(*p1 == 'X');
   p1 = gd::utf8::move::next(p1, 5); REQUIRE(*p1 == 'X');
   p1 = gd::utf8::move::next(p1, 5); REQUIRE(*p1 == 'X');
   p1 = gd::utf8::move::end(p1);     REQUIRE(*p1 == '\0');
   p1 = gd::utf8::move::previous(p1);REQUIRE(*p1 == 'X');
   p1 = gd::utf8::move::previous(p1,7); REQUIRE(*p1 == 'X');
   p1 = gd::utf8::move::previous(p1, 1); REQUIRE(*p1 == '.');
   p1 = gd::utf8::move::previous(p1, 1);
   auto result = memcmp(p1, u8"Ö", 2); REQUIRE(memcmp(p1, u8"Ö", 2) == 0);

   wchar_t wch = gd::utf8::character(p1);
   char ch = gd::utf8::character(p1);


   //const char* p2 = "";
   //p2 = gd::utf8::move::next(p2,5); REQUIRE(*p2 == 'O');

}

