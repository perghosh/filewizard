#include <stdint.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <variant>

#include "app_file.hpp"

namespace application {

   class CApplication
   {
   public:
      CApplication() {}
      ~CApplication() {}

   public:

      /**
       * file operations
       */
       ///@{
      /// Load file
      // std::pair<bool, std::string> FILE_Load(std::string_view stringAlias, std::string_view stringName);
      ///@}


      /**
       * folder operations
       */
      ///@{
      std::size_t FOLDER_Append(const file::CFolder& folder) { m_vectorFolder.push_back(folder); return m_vectorFolder.size(); }
      file::CFolder FOLDER_Get(std::variant< std::string, uint32_t> variantAlias);
      ///@}


   public:
      std::vector<file::CFile> m_vectorFile;  // vector holding active folders
      std::vector<file::CFolder> m_vectorFolder;  // vector holding active folders

   };

}