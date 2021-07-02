
#include "gd_utf8_string.hpp"
#include "..\include\gd_utf8_string.hpp"

namespace gd { namespace utf8 { 




void string::push_back( uint8_t ch )
{
	allocate( 1 );

	auto pbszEnd = c_end();
	auto uLength = convert( ch, pbszEnd );

	pbszEnd[uLength] = '\0';

	m_uSize += uLength;
	m_uCount++;
}


void string::push_back( uint16_t ch )
{
	allocate( 2 );

	auto pbszEnd = c_end();
	auto uLength = convert( ch, pbszEnd );

	pbszEnd[uLength] = '\0';

	m_uSize += uLength;
	m_uCount++;
}


/**
 * @brief append character store in 32 bit value
 * @param ch character added to string
 */
void string::push_back( uint32_t ch )
{
	allocate( 4 );

	auto pbszEnd = c_end();
	auto uLength = convert( ch, pbszEnd );

	pbszEnd[uLength] = '\0';

	m_uSize += uLength;
	m_uCount++;
}


string& string::append( const char* pbszText, uint32_t uLength )
{
	uint32_t uSize = gd::utf8::size( pbszText, uLength );
	allocate( uSize );

	auto pbszEnd = c_end();
	convert_ascii( pbszText, pbszEnd );

	m_uSize += uLength;
	m_uCount += uSize;

	return *this;
}

/**
 * @brief allocate new buffer for string if needed
 * @param uSize added size string needs to have
*/
void string::allocate(uint32_t uSize)
{
	if( uSize + m_uSize >= m_uSizeBuffer )
	{
		auto _size = uSize + m_uSize;

		if( _size < 64 ) _size = 64;
		else if( _size < 1024 ) _size = 1024;
		else
		{
			uint32_t uAdd = _size % 4096;
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
