#include <stdint.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <variant>


#include "application_file.hpp"

namespace application {

   extern std::pair<bool, std::string> FILE_Load( std::string_view stringFile, gd::utf8::string& stringLoadText );


   class CDocument
   {
   public:
      CDocument() {}
      CDocument( std::string_view stringName ) : m_stringName(stringName) {}
      ~CDocument() {}


      std::pair<bool, std::string> FILE_Load( std::string_view stringFile, std::string_view stringName );

   public:
      std::string m_stringName;     // document name
      std::vector<file::CFile> m_vectorFile;  // vector holding active files

   };


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
       * document operations
       */
      ///@{
      std::size_t DOCUMENT_Append(std::string_view stringName) { m_vectorDocument.push_back( std::make_unique<CDocument>(stringName) ); return m_vectorDocument.size(); }
      ///@}


      /**
       * folder operations
       */
      ///@{
      std::size_t FOLDER_Append(const file::CFolder& folder) { m_vectorFolder.push_back(folder); return m_vectorFolder.size(); }
      file::CFolder FOLDER_Get(std::variant< std::string, uint32_t> variantAlias);
      ///@}


   public:
      std::vector<std::unique_ptr<CDocument>> m_vectorDocument;

      std::vector<file::CFolder> m_vectorFolder;  // vector holding active folders

   };

}