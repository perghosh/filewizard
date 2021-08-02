#include <fstream>
#include <format>


#include "application.hpp"

namespace application {

   /**
    * @brief Load file into utf8 string
    * @param stringFile 
    * @param stringName 
    * @return 
   */
   std::pair<bool, std::string> FILE_Load( std::string_view stringFile, gd::utf8::string& stringLoadText )
   {
      std::ifstream ifstreamText;
      ifstreamText.open( std::string( stringFile ), std::ios::in );
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
            gd::utf8::string stringText;
            auto _size = vectorBuffer.size();
            auto pbBOM = vectorBuffer.data();
            if( pbBOM[ 0 ] == 0xEF && pbBOM[ 1 ] == 0xBB && pbBOM[ 2 ] == 0xBF )
            {
               stringText.assign( reinterpret_cast<const uint8_t*>(pbBOM + 3), vectorBuffer.size() - 3 );
            }
            else
            {
               stringText.assign( vectorBuffer.data(), vectorBuffer.size() );
            }

            stringLoadText = stringText;
         }
      }
      else
      {
         return { false, std::format("Failed to load file: {} [FILE_Load]", stringFile)};
      }

      return { true, std::string() };
   }

   std::pair<bool, std::string> FILE_Save( std::string_view stringFile, gd::utf8::string& stringSaveText )
   {
      std::ofstream ofstreamText( stringFile.data(), std::ofstream::binary );
      if( ofstreamText )
      {
         ofstreamText.write( stringSaveText.c_str(), stringSaveText.size() );
         ofstreamText.close();
      }

      return { true, std::string() };
   }

   /**
    * @brief Load file and create section that store file data
    * @param stringFile file name to load
    * @param stringName name for file
    * @return 
   */
   std::pair<bool, std::string> CDocument::FILE_Load( std::string_view stringFile, std::string_view stringName )
   {
      gd::utf8::string stringText;  // store file data
      auto [bOk, stringError] = application::FILE_Load( stringFile, stringText );
      if( bOk == true )
      {
         auto pFile = std::make_unique<application::file::CFile>( stringFile );
         pFile->SECTION_Append( stringName, stringText );                      // add section with file data
         m_vectorFile.push_back( std::move( pFile ) );
      }
      else return { bOk, stringError };

      return { true, std::string() };
   }

   /**
    * @brief Save utf8 sections  with specified file name
    * @param stringFile name of file sections are save to
    * @param stringName file name in document
    * @return true if ok, otherwise false and string message with error information
   */
   std::pair<bool, std::string> CDocument::FILE_Save( std::string_view stringFile, std::string_view stringName )
   {
      // collect information from attached sections
      const file::CFile* pFile = FILE_Get( stringName );
      if( pFile != nullptr )
      {
         gd::utf8::string stringCode;
         for( auto it = pFile->SECTION_Begin(); it != pFile->SECTION_End(); it++ )
         {
            stringCode += it->code();
         }

         return application::FILE_Save( stringFile, stringCode );
      }

      return { true, std::string() };
   }



   /**
    * @brief return copy of folder object
    * @param variantIndex alias or index to folder
    * @return file::CFolder or empty folder if not found
   */
   application::file::CFolder CApplication::FOLDER_Get(std::variant<std::string, uint32_t> variantIndex)
   {
      if(variantIndex.index() == 0)
      {
         for(auto it = std::begin(m_vectorFolder); it != std::end(m_vectorFolder); it++)
         {
            if(it->alias() == std::get<std::string>(variantIndex)) return *it;
         }
      }
      else
      {
         return m_vectorFolder[std::get<uint32_t>(variantIndex)];
      }

      return application::file::CFolder();
   }


}