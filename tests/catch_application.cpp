#include <iostream>
#include <iterator>
#include <array>
#include <iostream>
#include <fstream>

#include <windows.h>

#include "application.hpp"

#include "gd_utf8.hpp"

#include "catch.hpp"


std::string GetExePath() 
{
   wchar_t pwszbuffer[MAX_PATH] = { 0 };
   ::GetModuleFileNameW(NULL, pwszbuffer, MAX_PATH);

   char pbszBuffer[MAX_PATH * 2];

   std::string::size_type pos = std::wstring(pwszbuffer).find_last_of(L"\\/");
   std::wstring stringTemporary = std::wstring(pwszbuffer).substr(0, pos);

   pos = stringTemporary.find_last_of(L"\\/");
   stringTemporary = std::wstring(pwszbuffer).substr(0, pos);

   gd::utf8::convert_utf16_to_uft8(stringTemporary.c_str(), pbszBuffer);

   return std::string(pbszBuffer);
}


TEST_CASE("Test folder methods", "[folder]") {
   using namespace application;
   CApplication application;

   std::string stringFile = GetExePath();
   auto uCount = application.FOLDER_Append(file::CFolder("test", stringFile)); REQUIRE(uCount == 1);

   stringFile += "\\100.txt";

   std::ifstream fstreamText;
   fstreamText.open( stringFile, std::ios::in );
   if( fstreamText )
   {
      std::vector<uint8_t> vectorBuffer;
      char uValue;
      while( fstreamText.get( uValue ) ) { vectorBuffer.push_back( uValue ); }
      fstreamText.close();

      if( vectorBuffer.empty() == false )
      {
         auto _size = vectorBuffer.size();
         auto pbBOM = vectorBuffer.data();
         if( pbBOM[ 0 ] == 0xEF && pbBOM[ 1 ] == 0xBB && pbBOM[ 2 ] == 0xBF )
         {
            gd::utf8::string stringFile;
            stringFile.assign( reinterpret_cast<const uint8_t*>(pbBOM + 3), vectorBuffer.size() - 3 );

#ifdef DEBUG
            auto [ debug_p, debug_u ] = stringFile.dump(); REQUIRE(debug_u == stringFile.size()); REQUIRE((debug_p + debug_u - (const char*)stringFile.c_end()) == 0);
#endif 
            _size = stringFile.c_end() - stringFile.c_str();
            const char* p = (const char*)stringFile.c_str() + _size - 1000; 

            auto itPosition = stringFile.find( (const uint8_t*)"--" );
            while( itPosition != stringFile.cend() )
            {
               auto itSpace = itPosition;
               if( (itSpace - 1).value32() == ' ' )
               {
                  itSpace--;
                  while( itSpace.value32() == ' ' && itSpace != stringFile.cbegin() ) itSpace--;
                  if( itSpace.value32() == '\n' && itSpace != itPosition )
                  {
                     itSpace++;
                     itPosition = stringFile.erase( itSpace, itPosition, true );
                  }
               }
               itPosition++;
               itPosition = stringFile.find( itPosition, (const uint8_t*)"--" );
            }
            /*
            gd::utf8::string::iterator it = itPosition;
            auto _size1 = stringFile.size();
            stringFile.erase( it, it + 3, true );
            auto _size2 = stringFile.size();
            */
         }
      }
   }

   stringFile = GetExePath();
   stringFile += "\\test_remove_space.lua";
   fstreamText.open( stringFile, std::ios::in );
   if( fstreamText )
   {
      std::vector<uint8_t> vectorBuffer;
      char uValue;
      while( fstreamText.get( uValue ) ) { vectorBuffer.push_back( uValue ); }
      fstreamText.close();
      if( vectorBuffer.empty() == false )
      {
         auto _size = vectorBuffer.size();
         auto pbBOM = vectorBuffer.data();
         if( pbBOM[ 0 ] == 0xEF && pbBOM[ 1 ] == 0xBB && pbBOM[ 2 ] == 0xBF )
         {
            gd::utf8::string stringFile;
            stringFile.assign( reinterpret_cast<const uint8_t*>(pbBOM + 3), vectorBuffer.size() - 3 );
            auto itPosition = stringFile.find( (const uint8_t*)"application = nil;" );

            gd::utf8::string::iterator it = itPosition;
            stringFile.erase( it, it + strlen("application = nil;"), true );
         }
      }
   }



   // remove spaces from - "test_remove_space.lua"

   
}


/*
TEST_CASE("test", "[vanderbilt]") {
   uint8_t puArray[] = { 0x0A, 0x51, 0xB1A, 0x0A, 0x0A, 0x0A, 0x0A, 0x00, 0x0E };

   std::string stringText;

   std::cout << "test" << std::endl;



   for( auto i = 0; i < 9; i++ )
   {
      std::cout << std::hex << (int)puArray[i];
   }

   std::cout << std::endl;



   for( auto i = 0; i < 9; i++ )
   {

      auto ch = puArray[i];
      stringText += (char)((ch >> 4) + '0');
      stringText += (char)((ch & 0x07) + '0');
   }

}
*/
