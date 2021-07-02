#pragma once

#include <cassert>
#include <iterator>
#include <string_view>
#include "gd_utf8.hpp"

namespace gd { namespace utf8 { 



class string
{
public:
	struct iterator
	{
		typedef iterator           self;
		typedef uint8_t*				pointer;
		typedef uint8_t&				reference;
		typedef std::random_access_iterator_tag  iterator_category;

		iterator( pointer p ): m_pPosition(p) { }

		reference operator*() const { return *m_pPosition; }
		pointer operator->() { return m_pPosition; }

		self operator++() { m_pPosition = gd::utf8::move::next( m_pPosition ); return *this; }  
		self operator++( int ) { self it = *this; ++(*this); return it; }  

		self operator--() { m_pPosition = gd::utf8::move::previous( m_pPosition ); return *this; }  
		self operator--( int ) { self it = *this; --(*this); return it; }  

		bool operator==(const self& r) { return m_pPosition == r.m_pPosition; }
		bool operator!=(const self& r) { return m_pPosition != r.m_pPosition; }

		pointer m_pPosition;		/// position in string	
	};

	struct const_iterator
	{
		typedef const_iterator     self;
		typedef uint8_t*				pointer;
		typedef uint8_t&				reference;
		typedef std::random_access_iterator_tag  iterator_category;

		const_iterator( pointer p ): m_pPosition(p) { }

		const reference operator*() const { return *m_pPosition; }
		const pointer operator->() { return m_pPosition; }

		self operator++() { m_pPosition = gd::utf8::move::next( m_pPosition ); return *this; }  
		self operator++( int ) { self it = *this; ++(*this); return it; }  

		self operator--() { m_pPosition = gd::utf8::move::previous( m_pPosition ); return *this; }  
		self operator--( int ) { self it = *this; --(*this); return it; }  

		bool operator==(const self& r) { return m_pPosition == r.m_pPosition; }
		bool operator!=(const self& r) { return m_pPosition != r.m_pPosition; }

		pointer m_pPosition;		/// position in string	
	};


public:
	string() {}
	~string()
	{
		delete [] m_puString;
	}

	uint32_t size() const { return m_uSize; }
	uint32_t length() const { return m_uSize; }
	uint32_t count() const { return m_uCount; }
	uint32_t capacity() const { return m_uSizeBuffer; }
	bool empty() const { return m_uSize == 0; }
	const uint8_t* c_str() const { assert(m_puString != nullptr); m_puString; }
	

public:
	void push_back( uint8_t ch );
	void push_back( uint16_t ch );
	void push_back( uint32_t ch );

	string& append( uint8_t ch ) { push_back( ch ); return *this; };
	string& append( uint16_t ch ) { push_back( ch ); return *this; };
	string& append( uint32_t ch ) { push_back( ch ); return *this; };

	string& append( const char* pbszText );
	string& append( std::string_view stringText ) { return append( stringText.data(), static_cast<uint32_t>(stringText.length()) ); }
	string& append( const char* pbszText, uint32_t uLength );

	iterator begin() { return iterator( m_puString ); }
	iterator end() { return iterator( m_puString + m_uSize ); }
	const_iterator begin() const { return const_iterator( m_puString ); }
	const_iterator end() const { return const_iterator( m_puString + m_uSize ); }

	/// Get last position in buffer
	uint8_t* c_end() const { return m_puString + m_uSize; }

	void allocate( uint32_t uSize );

public:
	uint8_t* m_puString = nullptr;		/// pointer to string
	uint32_t m_uSize = 0;					/// string length in bytes
	uint32_t m_uSizeBuffer = 0;   		/// string length in bytes
	uint32_t m_uCount = 0;					/// Number of utf8 characters in text

};




} }
