#include <format> 

#include <fcntl.h>
#include <io.h> 
#include <sys/types.h>

#include "application_file.hpp"

#pragma warning( push )
#pragma warning( disable : 4244 4267 4996 )

namespace application { namespace file {


/**
 * ## Free functions ==========================================================
 */

std::pair<bool, std::string> Erase( gd::utf8::string& stringText, const std::regex& regexMatch )
{
   auto pbszPosition = stringText.c_str();
   std::cmatch cmatchResult;
   while( std::regex_search( pbszPosition, cmatchResult, regexMatch ) )
   {
      auto pbszBegin = cmatchResult.prefix().second;
      auto pbszEnd = cmatchResult.suffix().first;
      auto _count = pbszEnd - pbszBegin;
      uint8_t* p = stringText.c_buffer() + ((uint8_t*)pbszBegin - (uint8_t*)stringText.c_buffer());

      stringText.insert( p, p + _count, _count, 0 );                  // set character that are used to remove characters to 0
      pbszPosition = pbszEnd;
   }

   stringText.squeeze();

   return std::pair<bool, std::string>();
}

/**
 * @brief check if tag is found
 * @param stringTag tag name, if empty then true is returned
 * @return if group is found return true, otherwise false
*/
bool CSection::HasGroup( std::string_view stringTag ) const noexcept 
{ 
   if( stringTag.empty() == true ) return true;                               // empty group name will always return true

   if( m_stringTag == stringTag ) return true;

   if( m_stringTag.find( std::format( "[{}]", stringTag ) ) != m_stringTag.cend() ) return true;

   return false;
}

/**
 * @brief Split section code into one or more sections
 * @param vectorPosition positions where string is split into subsections and added as sections 
*/
void CSection::Split( std::vector<std::size_t> vectorPosition )
{
   auto itFrom = m_stringCode.cbegin();
   for( auto itCount = std::begin( vectorPosition ); itCount != std::end( vectorPosition ) && *itCount < m_stringCode.count(); itCount++ )
   {
      auto itTo = itFrom + *itCount;
      auto stringSection = m_stringCode.substr( itFrom, itTo );
      itFrom = itTo;
      m_vectorSection.push_back( CSection( m_pFile, m_stringTag, stringSection ) );
   }

   if( itFrom < m_stringCode.cend() )
   {
      auto stringSection = code().substr( itFrom, m_stringCode.cend() );
      m_vectorSection.push_back( CSection( m_pFile, m_stringTag, stringSection ) );
   }
}


/**
 * @brief Join sections into one utf8 string and return
 * @param stringTag tags if
 * @return 
*/
gd::utf8::string CSection::Join( std::string_view stringGroup )
{
   gd::utf8::string stringResult;
   for( auto it = std::begin( m_vectorSection ); it != std::end( m_vectorSection ); it++ )
   {
      if( stringGroup.empty() == false || it->HasGroup( stringGroup ) )
      {
         stringResult += it->code();
      }
   }

   return stringResult;
}


/**
 * ## CFile ===================================================================
 */

std::pair<bool, std::string> CFile::SECTION_Erase( const std::regex& regexMatch, std::string_view stringGroup )
{
   for( auto it = std::begin( m_vectorSection ); it != std::end( m_vectorSection ); it++ )
   {
      if( stringGroup.length() && it->HasGroup( stringGroup ) == false ) continue;

      auto [bOk, stringError] = it->Erase( regexMatch );
      if( bOk == false ) return { bOk, stringError };
   }

   return { true, std::string() };
}

/**
 * @brief Set name for file from the full path
 * Tries to extract file name from full path and set it as the name
*/
void CFile::SetNameFromPath()
{
   auto iPosition = m_stringPath.rfind( '\\' );
   if( iPosition == std::string::npos ) iPosition = m_stringPath.rfind( '/' );

   if( iPosition == std::string::npos ) m_stringName = m_stringPath;
   else
   {
      m_stringName = m_stringPath.substr( iPosition + 1 );
      iPosition = m_stringName.rfind( '.' );
      if( iPosition != std::string::npos ) m_stringName = m_stringName.substr( 0, iPosition );
   }
}


std::pair<bool, std::string> CFile::FILE_Load( std::string stringFileName, std::string_view stringName )
{
   constexpr int BUFFER_SIZE = 4*1024;
   int iFileHandle = _open( stringFileName.c_str(), _O_RDONLY );
   if( iFileHandle == -1 ) return { false, std::format("failed to open {}", stringFileName.c_str()) };

   uint8_t pBuffer[BUFFER_SIZE + 1];
   std::vector<uint8_t> vectorBuffer;
   std::size_t uCount;
   while( (uCount = read( iFileHandle, pBuffer, BUFFER_SIZE )) != 0 )
   {
      vectorBuffer.insert( vectorBuffer.end(), pBuffer, pBuffer + uCount );
   }
 

   return { true, std::string() };
}



} }

#pragma warning( pop )

