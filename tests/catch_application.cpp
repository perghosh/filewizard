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

   stringFile += "\\test_remove_space.lua";

   std::ifstream fstreamText;
   fstreamText.open( stringFile, std::ios::in );
   if( !fstreamText ) {}
   else
   {
      std::vector<uint8_t> vectorBuffer;
      char uValue;
      while( fstreamText.get( uValue ) )
      {
         vectorBuffer.push_back( uValue );
      }

      if( vectorBuffer.empty() == false )
      {
         auto pbBOM = vectorBuffer.data();
         if( pbBOM[ 0 ] == 0xEF && pbBOM[ 1 ] == 0xBB && pbBOM[ 2 ] == 0xBF )
         {
            gd::utf8::string stringFile;
            stringFile.assign( reinterpret_cast<const uint8_t*>(pbBOM + 3), vectorBuffer.size() - 3 );

            auto itPosition = stringFile.find( (const uint8_t*)"application = nil;" );
         }


      }
   }



   // remove spaces from - "test_remove_space.lua"

   
}