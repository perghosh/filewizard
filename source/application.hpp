#include <stdint.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <variant>


#include "application_file.hpp"

namespace application {

   extern std::pair<bool, std::string> FILE_Load( std::string_view stringFile, gd::utf8::string& stringLoadText );

/**
 * ## CDocument ===============================================================
 */


   class CDocument
   {
   public:
      CDocument() {}
      CDocument( std::string_view stringName ) : m_stringName( stringName ) {}
      ~CDocument() {}

      const std::string& name() const { return m_stringName; }
      std::string name() { return m_stringName; }
      void name( std::string_view stringName ) { m_stringName = stringName; }


      file::CFile* FILE_Get() const { return m_vectorFile.empty() == false ? m_vectorFile[ 0 ].get() : nullptr; }
      file::CFile* FILE_Get( std::size_t uIndex ) const { assert( uIndex < m_vectorFile.size() ); return m_vectorFile[ uIndex ].get(); }
      file::CFile* FILE_Get( std::string_view stringName ) const {
         for( auto it = std::begin( m_vectorFile ); it != std::end( m_vectorFile ); it++ ) { 
            if( (*it)->name() == stringName ) return it->get(); 
         }
         return nullptr;
      }
      std::pair<bool, std::string> FILE_Load( std::string_view stringFile, std::string_view stringName );
      std::pair<bool, std::string> FILE_Save( std::string_view stringFile, std::string_view stringName );

   public:
      std::string m_stringName;     ///< document name
      std::vector<std::unique_ptr<file::CFile>> m_vectorFile;///< vector holding active files

   };


/**
 * ## CApplication ============================================================
 */

   class CApplication
   {
   public:
      CApplication() {}
      ~CApplication() {}

      operator CDocument* () const { return m_vectorDocument.empty() == false ? m_vectorDocument[ 0 ].get() : nullptr; }

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
      CDocument* DOCUMENT_Get() const { return m_vectorDocument.empty() == false ? m_vectorDocument[ 0 ].get() : nullptr; }
      CDocument* DOCUMENT_Get( std::string_view stringName  ) const {
         for( auto it = std::begin( m_vectorDocument ); it != std::end( m_vectorDocument ); it++ ) { if( (*it)->name() == stringName ) return it->get(); }
         return nullptr;
      }
      std::size_t DOCUMENT_Append() { m_vectorDocument.push_back( std::make_unique<CDocument>() ); return m_vectorDocument.size(); }
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