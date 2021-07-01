#include "catch.hpp"

#include "gd_utf8.hpp"

TEST_CASE("utf8 move operations", "[utf8]") {
   char pBuffer[100] = { 0 };
   const char* p1 = "01234567890123456789012345678901234567890123456789";

   p1 = gd::utf8::move::next(p1);         REQUIRE(*p1 == '1');
   p1 = gd::utf8::move::next(p1, 9);      REQUIRE(*p1 == '0');
   p1 = gd::utf8::move::previous(p1, 1);  REQUIRE(*p1 == '9');
   p1 = gd::utf8::move::previous(p1);     REQUIRE(*p1 == '8');
   

   const char* pAscii = (const char*)u8"AAA. Xåäö.XÅÄÖ.X ©¢¥¹ X";

   p1 = pAscii; 
   p1 = gd::utf8::move::next(p1, 5);      REQUIRE(*p1 == 'X');
   p1 = gd::utf8::move::next(p1, 5);      REQUIRE(*p1 == 'X');
   p1 = gd::utf8::move::next(p1, 5);      REQUIRE(*p1 == 'X');
   p1 = gd::utf8::move::end(p1);          REQUIRE(*p1 == '\0');
   p1 = gd::utf8::move::previous(p1);     REQUIRE(*p1 == 'X');
   p1 = gd::utf8::move::previous(p1,7);   REQUIRE(*p1 == 'X');
   p1 = gd::utf8::move::previous(p1, 1);  REQUIRE(*p1 == '.');
   p1 = gd::utf8::move::previous(p1, 1);  REQUIRE(memcmp(p1, u8"Ö", 2) == 0);

   wchar_t wch = gd::utf8::character(p1);
   char ch = gd::utf8::character(p1);
}


TEST_CASE("utf8 convert operations", "[utf8]") {

   {
      uint8_t pBuffer[100] = { 0 };
      const char* pAscii = "Ä";
      auto _result = gd::utf8::convert_ascii( pAscii, pBuffer, pBuffer + 100 );
      auto l = strlen( pAscii );
      const char* pUtf8 = (const char*)u8"Ä";
      REQUIRE( std::memcmp( reinterpret_cast<const char*>(pBuffer), pUtf8, l ) == 0 );
   }

   {
      uint8_t pBuffer[100] = { 0 };
      const char* pAscii = "AAA. Xåäö.XÅÄÖ.X ©¢¥¹ X";
      auto _result = gd::utf8::convert_ascii( pAscii, pBuffer, pBuffer + 100 );
      auto l = strlen( pAscii );
      const char* pUtf8 = (const char*)u8"AAA. Xåäö.XÅÄÖ.X ©¢¥¹ X";

   
      REQUIRE( std::strlen( reinterpret_cast<const char*>(pBuffer) ) == std::strlen( pUtf8 ) );
      REQUIRE( std::memcmp( reinterpret_cast<const char*>(pBuffer), pUtf8, l ) == 0 );

      auto p1 = pBuffer; 
      p1 = gd::utf8::move::next(p1, 5);      REQUIRE(*p1 == 'X');
      p1 = gd::utf8::move::next(p1, 5);      REQUIRE(*p1 == 'X');
      p1 = gd::utf8::move::next(p1, 5);      REQUIRE(*p1 == 'X');
      p1 = gd::utf8::move::end(p1);          REQUIRE(*p1 == '\0');
      p1 = gd::utf8::move::previous(p1);     REQUIRE(*p1 == 'X');
      p1 = gd::utf8::move::previous(p1,7);   REQUIRE(*p1 == 'X');
      p1 = gd::utf8::move::previous(p1, 1);  REQUIRE(*p1 == '.');
      p1 = gd::utf8::move::previous(p1, 1);  REQUIRE(memcmp(p1, u8"Ö", 2) == 0);
   }
   

}

