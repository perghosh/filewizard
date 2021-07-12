#include <iostream>
#include <iterator>
#include <array>

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

   gd::utf8::convert_utf16_to_uft8(stringTemporary.c_str(), pbszBuffer);

   return std::string(pbszBuffer);
}


TEST_CASE("Test folder methods", "[folder]") {
   using namespace application;
   CApplication application;

   std::string stringPath = GetExePath();

   auto uCount = application.FOLDER_Append(file::CFolder("test", stringPath)); REQUIRE(uCount == 1);
}