#include <iostream>
#include <filesystem>
#include <regex>
#include <chrono>

#include <sys/stat.h>

#ifdef WIN32

#include <windows.h>
#include <ShlObj_core.h>

#endif

#include <io.h>

#include "gd_file.h"


_GD_FILE_BEGIN


   // Why adding line before comment
   // > Developers scroll a lot of code to find a function(mostly).If 
   // > you have a line before function developers gets a hint and know that 
   // > just below is the method name.It is a pattern that simplifies to scroll.
   // > It should be thin otherwise it will attract attention when it isn't
   // > needed (you are at the right place).


/*----------------------------------------------------------------------------- get_known_folder_wpath_g */ /**
 * Get full path for known folder. *known* means a folder that has some sort
 * of special functionality in the OS. Only a few folders are supported.
 * \param stringFolderId simple name for folder path is asked for
 * \return std::pair<bool, std::wstring> true and path to folder if success, false and error information if error
 */
std::pair<bool, std::wstring> get_known_folder_wpath_g(const std::string_view& stringFolderId)
{                                                                                assert( stringFolderId.length() > 0 );
   // ## copy first four bytes if requested id holds that many characters, copy is designed to be fast
   char pbFolderId[] = { '\0', '\0', '\0', '\0' }; // buffer used to match the requested folder path     // > variables declared on their own line should be commented
   if( stringFolderId.length() > 3 )
   {
      *(uint32_t*)pbFolderId = *(uint32_t*)stringFolderId.data();                // let compiler optimize
      for( auto it = pbFolderId, itLast = pbFolderId + sizeof(pbFolderId); it != itLast; it++ ) // convert to uppercase
      {
         if( *it >= 'a' ) *it -= ('a' - 'A');                                    // change to uppercase if lowercase letter
      }
   }
   else { pbFolderId[0] = *stringFolderId.data(); }
                                                                                                                          
   // ## Find out what type of folder path to return, checks characters in                               // > comments in with code is comments that work as 
   //    folder id to get the right one.                                                                 // > a divider between separate sections in method.
                                                                                                         // > also explaining what the section does.
#  ifdef WIN32                                                                                           // > here markdown `##` is used to mark that it is for next section
   const GUID* pguidFolderId = nullptr; // pointer to folder guid that path is returned for
   switch( pbFolderId[0] )                                                       // match what folder path to get
   {
   case 'd':
   case 'D' :
      pguidFolderId = &FOLDERID_Documents;                                       // "DOCUMENTS"
      if( pbFolderId[2] == 'W' ) pguidFolderId = &FOLDERID_Downloads;            // "DOWNLOADS"
      else if( pbFolderId[2] == 'V' ) pguidFolderId = &FOLDERID_Device;          // "DEVICE"
      break;
   case 'r':
   case 'R':
      pguidFolderId = &FOLDERID_Recent;                                          // "RECENT"
      if( pbFolderId[3] == 'Y' ) pguidFolderId = &FOLDERID_RecycleBinFolder;     // "RECYCLEBINFOLDER"
      break;
   default:                                                                      assert(false); // don't know this folder
      std::wstring stringError = L"Unknown folder id: ";
      for( auto it: stringFolderId ) stringError += it;
      return { false, stringError };
   }                                                                                                      // function name and comments at a margin?
                                                                                                          // > In this sample comments is not mixed
   std::wstring stringFolderPath; // gets path to requested folder                                        // > with code. Comments are separate from
                                                                                                          // > code to enable brieaf read without reading
   // ## Try to get path to folder                                                                        // > code to know what the code does.
   if( pguidFolderId != nullptr )                                                                         // > Puting comments at a margin will not clutter code.
   {
      wchar_t* pwszPath; // gets pointer to allocated buffer with folder path             
      if( FAILED(::SHGetKnownFolderPath(*pguidFolderId, 0, nullptr, &pwszPath)) ) // win32 api to get folder path
      {
         std::wstring stringError{ L"Failed to get known folder name, error is: " };
         stringError += std::to_wstring(GetLastError());
         return { false, stringError };
      }
      else
      {
         stringFolderPath = pwszPath;
         ::CoTaskMemFree(pwszPath);                                              // deallocate buffer with path
      }
   }
#  endif

   return { true, stringFolderPath };
}


/*----------------------------------------------------------------------------- closest_having_file_g */ /**
 * Try to find first parent folder containing specified file
 * Walks up in the folder hierarchy and tries to find specified file in folder, if found then return
 * folder, if not found go to parent folder and try to find file there.
 * This is done until no parent folders exists
 * \param stringPath start folder to begin search
 * \param stringFindFile file to search for 
 * \return std::pair<bool, std::string> true and folder name if found, false and empty string if not found
 */
std::pair<bool, std::string> closest_having_file_g(const std::string_view& stringPath, const std::string_view& stringFindFile)
{                                                                                assert(stringPath.empty() == false); assert(stringFindFile.empty() == false);   
   std::filesystem::path pathMatch(stringPath);
   std::string_view stringfolderSeparator{ "\\/" };

   pathMatch = pathMatch.parent_path();
   auto uRootMarkerLength = stringFindFile.length();

   while( pathMatch.root_name().string().length() + 1 < pathMatch.string().length() ) // check length for active folder, if it is longer than root than try to find root file
   {
      for( const auto& it : std::filesystem::directory_iterator(pathMatch) )
      {
         if( it.is_regular_file() == true )
         {
            std::string stringFileName = it.path().string();
            auto uFileNameLength = stringFileName.length();
            if( stringFileName.find(stringFindFile) != std::string::npos && 
                  stringfolderSeparator.find( stringFileName[uFileNameLength - uRootMarkerLength - 1] ) != std::string::npos 
               )
            {
               return  { true, stringFileName.substr(0, stringFileName.length() - stringFindFile.length()) };
            }
         }
      }
      pathMatch = pathMatch.parent_path();
   }

   return {false, std::string()};

}

/*----------------------------------------------------------------------------- dir */ /**
 * List files in specified folder
 * 
~~~{.cpp}
// list files with the pattern log(xxxx).txt that is at least one day old
auto vectorFile = gd::file::list_files(stringPath, { {"filter", R"(^log[\.\d].*\.txt)"}, {"to_days", -1} });
~~~
 * 
 * \param stringFolder folder where files is listed from
 * \param argumentsFilter different filters to select those files that you want to list
 * \return std::vector<std::string> files found in folder that match filters if any is sent
 */
std::vector<std::string> list_files(const std::string_view& stringFolder, const gd::argument::arguments& argumentsFilter )
{
   std::vector<std::string> vectorFile;

   // ## filter method is used when filter is found in arguments, file name is matched against wildcard or regular expression
   auto filter_ = [](const std::string stringFileName, auto& argumentFilter) -> bool {
      if( argumentFilter.is_text() )
      {
         std::string stringFilterOrRegex = argumentFilter.get_string();

         std::smatch smatchFirst;
         std::regex regexFind(stringFilterOrRegex);
         if( std::regex_search(stringFileName, smatchFirst, regexFind) == false ) return false; // not matched
      }
      return true;
   };

   auto day_count_ = [](const std::filesystem::path& pathFile, auto& argumentToDays) -> bool {
      if( argumentToDays.is_number() )
      {
         using namespace std::literals::chrono_literals;
         auto dToDays = argumentToDays.get_double();

         time_t timeNow = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
         time_t timeDifference = static_cast<time_t>(dToDays * (60.0 * 60 * 24));

         struct stat statFile;
         ::stat(pathFile.string().c_str(), &statFile);

         if( statFile.st_mtime >= (timeNow + timeDifference) ) return false;
      }
      return true;
   };

   for( const auto& itFile : std::filesystem::directory_iterator(stringFolder) )
   {
      if( itFile.is_regular_file() == false ) continue;

      const std::string stringFile = itFile.path().filename().string();

      if( filter_(stringFile, argumentsFilter["filter"]) == false ) continue;    // filter using regex or wildcard

      if( day_count_(itFile.path(), argumentsFilter["to_days"]) == false ) continue;// filter on time (days ?)

      vectorFile.push_back( itFile.path().string() );
   }

   return vectorFile;
}


std::pair<int, std::string> file_add_reference_g(const std::string_view& stringFileName)
{
   int iReference = 0;

   /*
   int iReference = 0;  // reference counter found in file
   int iFileHandle = 0; // handle to file
   char pbszBuffer[10]

   if( std::filesystem::exists(stringFileName) == false )
   {
      //bOk = std::filesystem::create_directory(stringPath);
#  if defined(_WIN32)
      errno_t iError = ::_sopen_s(&iFileHandle, stringFileName.data(), _O_CREAT|_O_WRONLY|_O_BINARY|_O_NOINHERIT, _SH_DENYWR, _S_IREAD|_S_IWRITE);
#     else
      iFileHandle = ::open(stringFileName->data(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#  endif
   }
   else
   {
#  if defined(_WIN32)
      errno_t iError = ::_sopen_s(&iFileHandle, stringFileName.data(), _O_WRONLY|_O_BINARY|_O_NOINHERIT, _SH_DENYWR, _S_IREAD|_S_IWRITE);
#     else
      iFileHandle = ::open(stringFileName->data(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#  endif

      int iCount = ::_read(iFileHandle, pbszBuffer, 10);
      auto pPosition = pbszBuffer;
      auto pEnd = pbszBuffer + sizeof(pbszBuffer);
      pPosition = gd::utf8::move::next_non_space(pPosition, pEnd);
      iReference = atoi( pbszBuffer );
   }

   iReference++;

   return { iReference, "" };
   */
   return { iReference, "" };
}



_GD_FILE_END