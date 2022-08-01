#include <filesystem>


#include "gd_file.h"

_GD_FILE_BEGIN



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