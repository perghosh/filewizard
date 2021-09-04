#include <iostream>
#include <iterator>
#include <array>
#include <iostream>
#include <fstream>
#include <regex>

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


TEST_CASE("Read and write files after conversion", "[folder]") {
   using namespace application;
   CApplication application;

   std::string stringFile = GetExePath();
   auto uCount = application.FOLDER_Append(file::CFolder("test", stringFile)); REQUIRE(uCount == 1);

   stringFile += "\\100.txt";

   std::ifstream ifstreamText;
   ifstreamText.open( stringFile, std::ios::in );
   if( ifstreamText )
   {
      std::vector<uint8_t> vectorBuffer;
      char uValue;
      while( ifstreamText.get( uValue ) ) { vectorBuffer.push_back( uValue ); }
      ifstreamText.close();

      if( vectorBuffer.empty() == false )
      {
         auto _size = vectorBuffer.size();
         auto pbBOM = vectorBuffer.data();
         if( pbBOM[ 0 ] == 0xEF && pbBOM[ 1 ] == 0xBB && pbBOM[ 2 ] == 0xBF )
         {
            gd::utf8::string stringData;
            stringData.assign( reinterpret_cast<const uint8_t*>(pbBOM + 3), vectorBuffer.size() - 3 );

#ifdef DEBUG
            auto [ debug_p, debug_u ] = stringData.dump(); REQUIRE(debug_u == stringData.size()); REQUIRE((debug_p + debug_u - (const char*)stringData.c_end()) == 0);
#endif 
            _size = stringData.c_end() - stringData.c_buffer();
            const char* p = (const char*)stringData.c_buffer() + _size - 1000; 

            auto itPosition = stringData.find( (const uint8_t*)"--" );
            while( itPosition != stringData.cend() )
            {
               auto itSpace = itPosition;
               if( (itSpace - 1).value32() == ' ' )
               {
                  itSpace--;
                  while( itSpace.value32() == ' ' && itSpace != stringData.cbegin() ) itSpace--;
                  if( itSpace.value32() == '\n' && itSpace != itPosition )
                  {
                     itSpace++;
                     itPosition = stringData.erase( gd::utf8::string::iterator( itSpace ), gd::utf8::string::iterator( itPosition ), true );
                  }
               }
               itPosition++;
               itPosition = stringData.find( itPosition, (const uint8_t*)"--" );
            }

            stringFile = GetExePath();
            stringFile += "\\100_new.txt";
            std::ofstream ofstreamNewFile( stringFile );
            ofstreamNewFile.put( (char)0xEF );
            ofstreamNewFile.put( (char)0xBB );
            ofstreamNewFile.put( (char)0xBF );
            ofstreamNewFile.write( stringData.c_str(), stringData.size() );
         }
      }
   }
}

TEST_CASE( "Split string", "[split]" ) {

   /*
   _CrtMemState MS1, MS2, MS3;
   _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
   _CrtMemCheckpoint( &MS1 ); // memory check

   using namespace application::file;
   
   {
      gd::utf8::string stringText( "11111111112222222222" );
      //auto s = stringText;
      //auto s2 = s;
      std::vector<gd::utf8::string> v{ stringText , stringText, stringText };

      int i = 0;
   _CrtMemCheckpoint( &MS2 ); // memory check
   _CrtMemDifference( &MS3, &MS1, &MS2 );
   std::cout << "Blocks allocated = " << MS3.lCounts[ _NORMAL_BLOCK ] << "\n";

   }

   _CrtMemCheckpoint( &MS2 ); // memory check
   _CrtMemDifference( &MS3, &MS1, &MS2 );

   std::cout << "Blocks allocated = " << MS3.lCounts[ _NORMAL_BLOCK ] << "\n";
   */
   using namespace application::file;
   gd::utf8::string stringText( "11111111112222222222" );
   CFile fileTest;
   fileTest.SECTION_Append( stringText );

   auto section = fileTest.SECTION_At( 0 );
   section.Split( 10 );                                                        REQUIRE( section.SECTION_Size() == 2 );
                                                                               REQUIRE( stringText == section.Join() );
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


// *
// ## regular expression to remove comments from SQL script
// tabs, line breaks: [\t\r\n]
// line comments: --.*\n (if . do not match newline) or --[^\r\n]*
// multi line comments: \/\*[\w\W]*?(?=\*\/)\*\/
// combine: (--[^\r\n]*)|(\/\*[\w\W]*?(?=\*\/)\*\/)
// 
// PRINT command:  PRINT\([^\)]*\);
//
TEST_CASE( "test boost regex", "[file]" ) {
   using namespace application;
   CApplication application;
   application.DOCUMENT_Append();
   CDocument* pDocument = application;

   std::string stringFile = GetExePath();
   stringFile += "\\changelog.sql";
   pDocument->FILE_Load( stringFile, "changelog" );

   auto pFile = pDocument->FILE_Get( "changelog" );                            REQUIRE( pFile != nullptr );

   //boost::regex regexMatch( R"(--.*\n)" );
   boost::regex regexMatch( R"((--[^\r\n]*)|(\/\*[\w\W]*?(?=\*\/)\*\/))" ); // regular expression to remove sql comments
   // (--[^\r\n]*) = find line comments
   // (\/\*[\w\W]*?(?=\*\/)\*\/) = find multi line comments
   pFile->SECTION_Erase( regexMatch );

   regexMatch = R"(PRINT\([^\)]*\);)"; pFile->SECTION_Erase( regexMatch );

   regexMatch = R"( NONCLUSTERED)"; pFile->SECTION_Erase( regexMatch );

   regexMatch = R"( DATETIME)"; pFile->SECTION_Replace( regexMatch, " TIMESTAMP" );

   regexMatch = R"( NVARCHAR\()"; pFile->SECTION_Replace( regexMatch, " VARCHAR(" ); 
   regexMatch = R"( VARCHAR\(\s*MAX[^\)]*\))"; pFile->SECTION_Replace( regexMatch, " TEXT" );

   regexMatch = R"( BIGINT\s+IDENTITY\s*\([^\)]*\))";  pFile->SECTION_Replace( regexMatch, " BIGSERIAL" );
   regexMatch = R"(CREATE\s+CLUSTERED\s+)";  pFile->SECTION_Replace( regexMatch, "CREATE " );


   stringFile = GetExePath();
   stringFile += "\\changelog_01.sql";
   pDocument->FILE_Save( stringFile, "changelog" );
}


TEST_CASE("convert lua file", "[folder]") {
   using namespace application;
   CApplication application;
   application.DOCUMENT_Append();
   CDocument* pDocument = application;

   std::string stringFile = GetExePath();
   stringFile += "\\test_remove_space.lua";

   pDocument->FILE_Load( stringFile, "test_remove_space" );

   auto pFile = pDocument->FILE_Get( "test_remove_space" );                    REQUIRE( pFile != nullptr );
   std::regex regexMatch( R"(^\s\s+)" );
   pFile->SECTION_Erase( regexMatch );

   stringFile = GetExePath();
   stringFile += "\\test_remove_space_01.lua";
   pDocument->FILE_Save( stringFile, "test_remove_space" );



   std::ifstream ifstreamText;
   ifstreamText.open( stringFile, std::ios::in );
   if( ifstreamText )
   {
      std::vector<uint8_t> vectorBuffer;
      char uValue;
      while( ifstreamText.get( uValue ) ) 
      { 
         vectorBuffer.push_back( uValue ); 
      }
      ifstreamText.close();

      if( vectorBuffer.empty() == false )
      {
         auto _size = vectorBuffer.size();
         auto pbBOM = vectorBuffer.data();
         if( pbBOM[ 0 ] == 0xEF && pbBOM[ 1 ] == 0xBB && pbBOM[ 2 ] == 0xBF )
         {
            gd::utf8::string stringLua;
            stringLua.assign( reinterpret_cast<const uint8_t*>(pbBOM + 3), vectorBuffer.size() - 3 );

            std::cmatch cmatchResult;
            std::regex regexFind( R"(^\s\s+)" );
            auto pbszPosition = stringLua.c_str();
            while( std::regex_search( pbszPosition, cmatchResult, regexFind ) )
            {
               // std::cout << cmatchResult.str() << "\n";
               auto pbszBegin = cmatchResult.prefix().second;
               auto pbszEnd = cmatchResult.suffix().first;
               auto _count = pbszEnd - pbszBegin;
               uint8_t* p = stringLua.c_buffer() + ((uint8_t*)pbszBegin - (uint8_t*)stringLua.c_buffer());

               stringLua.insert( p, p + _count, _count, 0 );                  // set character that are remove to 0
               pbszPosition = pbszEnd;
            }

            stringLua.squeeze();

            std::string stringFile = GetExePath();
            stringFile += "\\test_removed_spaces.lua";
            std::ofstream ofstreamText(stringFile,std::ofstream::binary);

            uint8_t pbBOM[] = { 0xEF, 0xBB, 0xBF };
            ofstreamText.write( (const char*)pbBOM, 3 );
            ofstreamText.write( stringLua.c_str(), stringLua.size() );
         }
      }
   }
}

TEST_CASE("query file", "[sql]") {

   //changelog.sql
}
