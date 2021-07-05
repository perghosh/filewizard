#include <format> 

#include <fcntl.h>
#include <io.h> 
#include <sys/types.h>

#include "app_file.hpp"

#pragma warning( push )
#pragma warning( disable : 4996 )

namespace application { namespace file {

std::pair<bool, std::string> CFile::FILE_Load( std::string stringFileName, std::string_view stringName )
{
   constexpr int BUFFER_SIZE = 16*1024;
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

