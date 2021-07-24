#include <iterator>
#include <filesystem>


#include "catch.hpp"

#include "gd_utf8.hpp"
#include "gd_utf8_string.hpp"

#include "application_file.hpp"

TEST_CASE("read file into application::CFile", "[file]") {
   using namespace application::file;

   CFile fileTest;

   std::filesystem::path pathCurrent = std::filesystem::current_path();

   std::string stringFile = pathCurrent.string();

   fileTest.FILE_Load( stringFile, "root");

}
