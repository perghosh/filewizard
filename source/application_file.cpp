#include <format> 

#include <fcntl.h>
#include <io.h> 
#include <sys/types.h>

#include "application_file.hpp"

#pragma warning( push )
#pragma warning( disable : 4244 4267 4996 )

namespace application { namespace file {

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

