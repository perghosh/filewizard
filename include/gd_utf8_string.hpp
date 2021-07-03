#pragma once

#include <cassert>
#include <iterator>
#include <string_view>
#include "gd_utf8.hpp"

namespace gd { namespace utf8 { 

	/**
	 * @brief character taken from utf8 string
	 */
	struct value32
	{
		value32( const uint8_t* pubszValue ) {
			m_uValue = character( pubszValue );
		}

		value32( const value32& o ) { m_uValue = o.m_uValue; }
		value32& operator=( const value32& o ) { m_uValue = o.m_uValue; return *this; }


/*
		value32( const value32& o ) { *reinterpret_cast<uint32_t*>( m_pValue ) = *reinterpret_cast<uint32_t*>( const_cast<value32*>(&o)->m_pValue); }
		value32& operator=( const value32& o ) { *reinterpret_cast<uint32_t*>( m_pValue ) = *reinterpret_cast<uint32_t*>( const_cast<value32*>(&o)->m_pValue); return *this; }
*/

		operator char() const {                                                  assert( m_uValue < 0x100 );
			return static_cast<char>(m_uValue);
		}

		operator wchar_t() const {                                               assert( m_uValue < 0x10000 );
			return static_cast<wchar_t>(m_uValue);
		}

		uint32_t m_uValue;
	};



class string
{
public:
	typedef string						self;
   typedef uint8_t					value_type;
	typedef value_type*           pointer;
	typedef const value_type*     const_pointer;
   typedef value_type&           reference;
   typedef const value_type&     const_reference;
	typedef std::size_t				size_type;


public:
	struct iterator
	{
		typedef iterator           self;
		typedef uint8_t 				value_type;
		typedef uint8_t*				pointer;
		typedef uint8_t&				reference;
		typedef std::size_t        size_type;
		typedef std::ptrdiff_t     difference_type;
		typedef std::bidirectional_iterator_tag  iterator_category;

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
		typedef uint8_t 				value_type;
		typedef uint8_t*				pointer;
		typedef const pointer	   const_pointer;
		typedef uint8_t&				reference;
		typedef std::size_t        size_type;
		typedef std::ptrdiff_t     difference_type;
		typedef std::bidirectional_iterator_tag  iterator_category;

		const_iterator( pointer p ): m_pPosition(p) { }

		const reference operator*() const { return *m_pPosition; }
		const pointer operator->() { return m_pPosition; }

		operator const_pointer() { return m_pPosition; }

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
		//delete [] m_puString;
		if( string::m_pbuffer_empty != m_pbuffer) m_pbuffer->release();
	}

	gd::utf8::value32 operator [](size_type uIndex ) const { return at( uIndex ); }

	[[nodiscard]] uint32_t size() const { return m_pbuffer->size(); }
	[[nodiscard]] uint32_t length() const { return m_pbuffer->size(); }
	[[nodiscard]] uint32_t count() const { return m_pbuffer->count(); }
	[[nodiscard]] uint32_t capacity() const { return m_pbuffer->capacity(); }
	[[nodiscard]] bool empty() const { return m_pbuffer->empty(); }
	[[nodiscard]] const uint8_t* c_str() const { assert(m_pbuffer != string::m_pbuffer_empty); m_pbuffer->c_str(); }
	

public:
	void push_back( uint8_t ch );
	void push_back( uint16_t ch );
	void push_back( uint32_t ch );

	string& append( uint8_t ch ) { push_back( ch ); return *this; };
	string& append( char8_t ch ) { push_back( static_cast<uint8_t>( ch ) ); return *this; };
	string& append( uint16_t ch ) { push_back( ch ); return *this; };
	string& append( char16_t ch ) { push_back( static_cast<uint16_t>( ch ) ); return *this; };
	string& append( uint32_t ch ) { push_back( ch ); return *this; };
	string& append( char32_t ch ) { push_back( static_cast<uint32_t>( ch ) ); return *this; };

	string& append( const char* pbszText ) { return append( pbszText, static_cast<uint32_t>( std::strlen( pbszText ) ) ); }
	string& append( std::string_view stringText ) { return append( stringText.data(), static_cast<uint32_t>(stringText.length()) ); }
	string& append( const char* pbszText, uint32_t uLength );

	[[nodiscard]] gd::utf8::value32 at( size_type uIndex ) const { 
		auto it = begin();
		std::advance( it, uIndex );
		return value32( it ); 
	}
	[[nodiscard]] gd::utf8::value32 at( const_iterator it ) const { return value32( it ); }

	[[nodiscard]] iterator begin() { return iterator( m_pbuffer->c_buffer() ); }
	[[nodiscard]] iterator end() { return iterator( m_pbuffer->c_buffer_end() ); }
	[[nodiscard]] const_iterator begin() const { return const_iterator( m_pbuffer->c_buffer() ); }
	[[nodiscard]] const_iterator end() const { return const_iterator( m_pbuffer->c_buffer_end() ); }

	//character at( std::size_t uPosition );

	/// Get last position in buffer
	[[nodiscard]] uint8_t* c_end() const { return m_pbuffer->c_buffer_end(); }

	void allocate( uint32_t uSize );

public:
/*
	uint8_t* m_puString = nullptr;		/// pointer to string
	uint32_t m_uSize = 0;					/// string length in bytes
	uint32_t m_uSizeBuffer = 0;   		/// string length in bytes
	uint32_t m_uCount = 0;					/// Number of utf8 characters in text
	*/

	struct buffer
	{
		uint32_t m_uSize;					/// string length in bytes
		uint32_t m_uSizeBuffer;   		/// string length in bytes
		uint32_t m_uCount;				/// Number of utf8 characters in text
		uint32_t m_uFlags;			   /// Internal flags how string logic works
		int32_t  m_iReferenceCount;

		uint32_t size() const { return m_uSize; }
		void size( uint32_t uSize ) { m_uSize = uSize; }
		uint32_t length() const { return m_uSize; }
		void length( uint32_t uLength ) { m_uSize = uLength; }
		uint32_t count() const { return m_uCount; }
		void count( uint32_t uCount ) { m_uCount = uCount; }
		uint32_t capacity() const { return m_uSizeBuffer; }
		void capacity( uint32_t uCapacity ) { m_uSizeBuffer = uCapacity; }
		bool empty() const { return m_uSize == 0; }


		string::pointer c_buffer() { return reinterpret_cast<string::pointer>(this + 1); }
		string::pointer c_buffer_end() { return reinterpret_cast<string::pointer>(this + 1) + size(); }
		string::const_pointer c_buffer() const { return reinterpret_cast<string::const_pointer>(this + 1); }
		string::const_pointer c_buffer_end() const { return reinterpret_cast<string::const_pointer>(this + 1) + size(); }
		string::const_pointer c_str() const { return reinterpret_cast<string::const_pointer>(this + 1); }

		void set_reference( int32_t iCount ) { m_iReferenceCount = iCount; }
		void add_reference() { m_iReferenceCount++; }

		void release()
		{																								assert( m_iReferenceCount > 0 );
			m_iReferenceCount--;
			if( m_iReferenceCount == 0 ) delete [] reinterpret_cast<buffer*>( this );
		}
	};

	buffer* m_pbuffer = string::m_pbuffer_empty;


	static buffer m_pbuffer_empty[];
};




} }
