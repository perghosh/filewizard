
#include "gd_utf8_string.hpp"
#include "..\include\gd_utf8_string.h"

namespace gd { namespace utf8 { 


string::~string()
{
	delete [] m_puString;
}

string& string::assign( char32_t ch )
{
	return *this;
}

/**
 * @brief allocate new buffer for string if needed
 * @param uSize added size string needs to have
*/
void string::allocate(std::size_t uSize)
{
	if( uSize + m_uSize >= m_uSizeBuffer )
	{
		auto _size = uSize + m_uSize;

		if( _size < 64 ) _size = 64;
		else if( _size < 1024 ) _size = 1024;
		else
		{
			std::size_t uAdd = _size % 4096;
			_size += uAdd;

			if( uAdd < 64 ) _size += 4096;
		}


		uint8_t* puNew = new uint8_t[_size];
		memcpy( puNew, m_puString, m_uSize );

		delete [] m_puString;
		m_puString = puNew;
		m_uSizeBuffer = _size;
	}
}


} }
