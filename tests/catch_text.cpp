#include <iterator>

#include "catch.hpp"

#include "gd_utf8.hpp"
#include "gd_utf8_string.hpp"

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

   //wchar_t wch = gd::utf8::character(p1);
   //char ch = gd::utf8::character(p1);
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

TEST_CASE("utf8 add to string", "[utf8]") {
   gd::utf8::string stringText;

   stringText.append(U'Ö');         REQUIRE(stringText.length() == 2);  REQUIRE(stringText.count() == 1);
   stringText.append("0123456789"); REQUIRE(stringText.length() == 12); REQUIRE(stringText.count() == 11);

   unsigned uCount = 0;
   for( auto it : stringText ) {
      auto ch = it;
      char chCharacter = ch;
      uCount++;
   }                                REQUIRE(uCount == 11);

   stringText.append(u'Ö');

   gd::utf8::string stringText2( stringText );

   
   //auto l = std::distance( it, stringText.end() );
   auto it = stringText.begin();

   static_assert(std::is_same_v<gd::utf8::string::iterator::iterator_category, std::bidirectional_iterator_tag>);

   std::advance( it, 5 );           REQUIRE( *it == '4' );
   auto uDistance = std::distance( stringText.begin(), stringText.end() );     REQUIRE( uDistance == 12 );

   REQUIRE( 'Ö' == (char)stringText[0]);
   REQUIRE( L'Ö' == (wchar_t)stringText[0]);
   REQUIRE( stringText[0] == stringText[11]);
   if( stringText[0] == stringText[11] ) stringText = "ok";
   if( stringText[11] == 'Ö' ) stringText = "ok";

   auto test = stringText.at(20);
}

TEST_CASE("Test stack based string", "[utf8]") {
   uint8_t pBuffer[100];

   gd::utf8::string stringText( gd::utf8::buffer{ pBuffer, 100 } );
   stringText.append("0123456789");
   stringText.append("0123456789");          REQUIRE(stringText.size() == 20);                             
   stringText.append("åäö");                 REQUIRE(stringText.size() == 26); 
                                             REQUIRE(stringText.count() == 23); 

   auto s1 = stringText;
   auto s2 = s1;
   auto s3 = s2;

/*
   
   auto v = stringText[20];
   auto v1 = v;
   if(v == gd::utf8::value32(u8"å"))
   {
      REQUIRE(v == gd::utf8::value32(u8"å"));
   }
   wchar_t wch = v;
   if((char8_t)'å' == v)
   {
      REQUIRE(v == (uint8_t)'å');
      v = stringText[21];
      REQUIRE(v == (uint8_t)'ä');
      v = stringText[22];
      REQUIRE(v == (uint8_t)'ö');
   }

   /*
   char8_t ch;
   gd::utf8::normalize( v, ch );
   
   if(ch == (char8_t)'ä') {
      REQUIRE(stringText.size() == 26);  
   };
   */
}


// regex: https://www.geeksforgeeks.org/check-three-or-more-consecutive-identical-characters-or-numbers/