#include <iostream>
#include <iterator>
#include <array>
#include <regex>

#include "catch.hpp"

#include "gd_utf8.hpp"
#include "gd_utf8_string.hpp"

TEST_CASE("utf8 count, move and find operations", "[utf8]") {
   char pBuffer[100] = { 0 };
   const char* p1 = "01234567890123456789012345678901234567890123456789";

   auto uCount = gd::utf8::count( p1 ).first;REQUIRE(uCount == 50);

   p1 = gd::utf8::move::next(p1);         REQUIRE(*p1 == '1');
   p1 = gd::utf8::move::next(p1, 9);      REQUIRE(*p1 == '0');
   p1 = gd::utf8::move::previous(p1, 1);  REQUIRE(*p1 == '9');
   p1 = gd::utf8::move::previous(p1);     REQUIRE(*p1 == '8');
   

   const char* pAscii = (const char*)u8"AAA. Xåäö.XÅÄÖ.X ©¢¥¹ X";
   uCount = gd::utf8::count( pAscii ).first;REQUIRE(uCount == 23);
   uCount = static_cast<uint32_t>( reinterpret_cast<const char*>(gd::utf8::count( pAscii ).second) - pAscii ); REQUIRE(uCount == 33);
   uCount = gd::utf8::size("AAA. Xåäö.XÅÄÖ.X ©¢¥¹ X");REQUIRE(uCount == 33);
   uCount = gd::utf8::size({'A','A','A','.',' ','X','å','ä','ö','.','X','Å','Ä','Ö','.','X',' ','©','¢','¥','¹',' ','X'});REQUIRE(uCount == 33);
   std::vector<char> v({'A','A','A','.',' ','X','å','ä','ö','.','X','Å','Ä','Ö','.','X',' ','©','¢','¥','¹',' ','X'});

   p1 = pAscii; 
   p1 = gd::utf8::move::next(p1, 5);      REQUIRE(*p1 == 'X');
   p1 = gd::utf8::move::next(p1, 5);      REQUIRE(*p1 == 'X');
   p1 = gd::utf8::move::next(p1, 5);      REQUIRE(*p1 == 'X');
   p1 = gd::utf8::move::end(p1);          REQUIRE(*p1 == '\0');
   p1 = gd::utf8::move::previous(p1);     REQUIRE(*p1 == 'X');
   p1 = gd::utf8::move::previous(p1,7);   REQUIRE(*p1 == 'X');
   p1 = gd::utf8::move::previous(p1, 1);  REQUIRE(*p1 == '.');
   p1 = gd::utf8::move::previous(p1, 1);  REQUIRE(memcmp(p1, u8"Ö", 2) == 0);

   p1 = pAscii; 
   p1 = gd::utf8::move::find( p1, '.');  REQUIRE(*p1 == '.');
   p1 = pAscii; 
   p1 = gd::utf8::move::find( p1, 'Ö');  REQUIRE(memcmp(p1, u8"Ö", 2) == 0);
   p1 = pAscii; 
   p1 = gd::utf8::move::find( p1, 214.01);  REQUIRE(memcmp(p1, u8"Ö", 2) == 0);

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


TEST_CASE( "String operation to check stack, single or referenced", "[utf8]" ) {
   uint8_t pBuffer[100];
   using namespace gd::utf8;

   string stringText( gd::utf8::buffer{ pBuffer, 100 } );
   stringText = "0123456789";
   auto s1 = stringText;
   //string s1
}

TEST_CASE("Test stack based string, no heap allocation", "[utf8]") {
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

TEST_CASE("insert and remove text in string", "[utf8]") {

   {
      auto uCount = 0u;

      gd::utf8::string s1("000111222");
      s1.insert( s1.begin() + 3, s1.begin() + 6, 3, '9');
      std::for_each( std::begin(s1), std::end(s1), [&uCount](auto it) { 
         if( it == 'A' ) uCount++; 
      });                                                                      REQUIRE( uCount == 0 );

      s1.insert( s1.begin() + 3, s1.begin() + 6, 3, '\0');
      s1.squeeze();
      s1.insert( s1.begin() + 6, s1.begin() + 9, 3, '3');
      

      s1.insert( s1.begin() + 3, s1.begin() + 6, 20, 'A');

      std::for_each( std::begin(s1), std::end(s1), [&uCount](auto it) { 
         if( it == 'A' ) uCount++; 
      });                                                                      REQUIRE( uCount == 20 );
   }

   {
      auto uCount = 0u;

      gd::utf8::string s1("000111222");
      s1.insert( s1.begin() + 3, s1.begin() + 6, 3, '9');

      s1.insert( s1.begin() + 3, s1.begin() + 6, 30, 'Ö');
      for( auto it = std::begin( s1 ); it != std::end( s1 ); it++ )
      {
         if( it.value32() == U'Ö' ) uCount++; 
      }                                                                        REQUIRE( uCount == 30) ;
   }

}

TEST_CASE("find text using regex", "[utf8]") {

   {
      const char* pbszText = "0123456789 XXX 0123456789";
      gd::utf8::string stringText(pbszText);

      const std::regex regexSearch("([X]+)");
      std::cmatch smatchFound;

      auto itTest = stringText.cbegin();
      std::advance( itTest, 10 );

      auto itBegin = std::regex_iterator( stringText.cbegin_char(), stringText.cend_char(), regexSearch);
      auto itEnd = std::regex_iterator<gd::utf8::string::char_const_iterator>();
      auto itEnd01 = std::sregex_iterator();

      for( std::regex_iterator<gd::utf8::string::char_const_iterator> it = itBegin; it != itEnd; ++it )
      {
         auto pairPrefix = it->prefix();
         auto pairSuffix = it->suffix();

         auto uPrefixOffset = pairPrefix.second - stringText.c_str();
         auto uSuffixOffset = pairSuffix.first - stringText.c_str();

         std::cout << "Match: " << it->str() << '\n';
         std::cout << "Distance to start of match: " << uPrefixOffset << '\n';
         std::cout << "Distance to end of match: " << uSuffixOffset << '\n';
      }
   }


   /*
   {
      const char* pbszText = "0123456789 XXX 0123456789";
      std::regex_iterator<const char*>::value_type match("X");
      std::regex_iterator<const char*> move( pbszText, pbszText + strlen(pbszText), match );
      std::regex_iterator<const char*> end;

      for(; match != end; ++match)
      {
         std::cout << "match == " << match->str() << std::endl;
      }
   }
   */

/*
   https://devblogs.microsoft.com/cppblog/the-filterator/
   https://stackoverflow.com/questions/17069327/issues-with-using-custom-iterators-for-regex-iterator


   const char* pbszText = "0123456789 XXX 0123456789";
   gd::utf8::string stringText("0123456789 XXX 0123456789");
   const std::regex regexSearch("([X]+)");
   std::cmatch smatchFound;

   //bool bMatch1 = std::regex_match( stringText.cbegin(), stringText.cend(), regexSearch );

   auto itBegin = std::regex_iterator(stringText.cbegin(), stringText.cend(), regexSearch);
   auto itEnd = std::sregex_iterator();

   bool bMatch2 = std::regex_match( pbszText, pbszText + strlen(pbszText), smatchFound, regexSearch );
   */
}


// regex: https://www.geeksforgeeks.org/check-three-or-more-consecutive-identical-characters-or-numbers/