#pragma once

#include "gd_utf8.hpp"

namespace gd { namespace utf8 { 

class string
{
public:
	string();
	~string();


public:
	string& assign( char32_t ch );

	void allocate( std::size_t uSize );

public:
	uint8_t* m_puString = nullptr;		/// pointer to string
	std::size_t m_uSize = 0;				/// string length in bytes
	std::size_t m_uSizeBuffer = 0;   	/// string length in bytes
	std::size_t m_uCount = 0;				/// Number of utf8 characters in text

};

string::string()
{
}


} }