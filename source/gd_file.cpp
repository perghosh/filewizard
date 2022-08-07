#include <filesystem>

#ifdef WIN32

#include <windows.h>
#include <ShlObj_core.h>

#endif



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



_GD_FILE_END