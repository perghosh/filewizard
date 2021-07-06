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
      value32(): m_uValue(0) {}
      explicit value32(char ch): m_uValue( static_cast<uint32_t>(ch) ) {}
      explicit value32(const char* pbszValue) {
         m_uValue = character(reinterpret_cast<const uint8_t*>(pbszValue));
      }
      explicit value32(const char8_t* pbszValue) {
         m_uValue = character(reinterpret_cast<const uint8_t*>(pbszValue));
      }
      value32( const uint8_t* pubszValue ) {
         m_uValue = character( pubszValue );
      }

      value32( const value32& o ) { m_uValue = o.m_uValue; }
      value32& operator=( const value32& o ) { m_uValue = o.m_uValue; return *this; }
      //constexpr auto operator<=>( const value32& _1 ) const { return m_uValue <=> _1.m_uValue; }


/*
      value32( const value32& o ) { *reinterpret_cast<uint32_t*>( m_pValue ) = *reinterpret_cast<uint32_t*>( const_cast<value32*>(&o)->m_pValue); }
      value32& operator=( const value32& o ) { *reinterpret_cast<uint32_t*>( m_pValue ) = *reinterpret_cast<uint32_t*>( const_cast<value32*>(&o)->m_pValue); return *this; }
*/

      operator char() const { assert( m_uValue < 0x100 ); return static_cast<char>(m_uValue); }
      operator char8_t() const { assert( m_uValue < 0x100 ); 
         const uint8_t* p = reinterpret_cast<const uint8_t*>(&m_uValue) + (m_uValue >= 0x80 ? 2 : 3);
         auto ch = gd::utf8::character( p );
         return static_cast<char8_t>(ch); 
      }
      operator wchar_t() const { assert( m_uValue < 0x10000 ); return static_cast<wchar_t>(m_uValue); }
      operator uint32_t() const { return m_uValue; }

      uint32_t m_uValue;
   };


   struct buffer;

class string
{
public:
   enum enumBufferStorage : std::uint32_t {
      eBufferStorageReferenceCount   = 0x00,
      eBufferStorageStack            = 0x01,   // string data is on stack, do not delete
      eBufferStorageSingle            = 0x02,   // string data is not reference counted
   };

public:
   typedef string                  self;
   typedef uint8_t               value_type;
   typedef value_type*           pointer;
   typedef const value_type*     const_pointer;
   typedef value_type&           reference;
   typedef const value_type&     const_reference;
   typedef std::size_t            size_type;


public:
   struct iterator
   {
      typedef iterator           self;
      typedef uint8_t             value_type;
      typedef uint8_t*            pointer;
      typedef uint8_t&            reference;
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

      pointer m_pPosition;      /// position in string   
   };

   struct const_iterator
   {
      typedef const_iterator     self;
      typedef uint8_t             value_type;
      typedef uint8_t*            pointer;
      typedef const pointer      const_pointer;
      typedef uint8_t&            reference;
      typedef std::size_t        size_type;
      typedef std::ptrdiff_t     difference_type;
      typedef std::bidirectional_iterator_tag  iterator_category;

      const_iterator( pointer p ): m_pPosition(p) { }

      const reference operator*() const { return *m_pPosition; }
      const pointer operator->() { return m_pPosition; }

      operator const_pointer() const { return m_pPosition; }
      operator char() const { return value32(m_pPosition); }
      operator wchar_t() const { return value32(m_pPosition); }
      operator uint32_t() const { return value32(m_pPosition); }

      self operator++() { m_pPosition = gd::utf8::move::next( m_pPosition ); return *this; }  
      self operator++( int ) { self it = *this; ++(*this); return it; }  

      self operator--() { m_pPosition = gd::utf8::move::previous( m_pPosition ); return *this; }  
      self operator--( int ) { self it = *this; --(*this); return it; }  

      bool operator==(const self& r) { return m_pPosition == r.m_pPosition; }
      bool operator!=(const self& r) { return m_pPosition != r.m_pPosition; }

      pointer m_pPosition;      /// position in string   
   };


public:
   string() {}
   explicit string( gd::utf8::buffer bufferStack );
   explicit string( const char* pbszText ) { assign( pbszText ); }
   explicit string( std::string_view stringText ) { assign( stringText.data(), static_cast<uint32_t>(stringText.length()) ); }
   explicit string( const char* pbszText, uint32_t uLength ) { assign( pbszText, uLength ); }

   string(string&& o) noexcept {
      m_pbuffer = o.m_pbuffer; o.m_pbuffer = string::m_pbuffer_empty;
   }

   string(string& o) { copy(o); }
   string(const string& o) { clone(o); }
   ~string() {string::release( m_pbuffer ); }

   string& operator=(string& o) { copy(o); return *this; }
   string& operator=(const string& o) { clone(o); return *this; }
   string& operator=(string&& o) noexcept { 
      m_pbuffer = o.m_pbuffer; o.m_pbuffer = string::m_pbuffer_empty;
      return *this;
   }
   string& operator=( const char* pbszText ) { return assign( pbszText ); }

   gd::utf8::value32 operator [](size_type uIndex ) const { return at( uIndex ); }

   [[nodiscard]] uint32_t size() const { return m_pbuffer->size(); }
   [[nodiscard]] uint32_t length() const { return m_pbuffer->size(); }
   [[nodiscard]] uint32_t count() const { return m_pbuffer->count(); }
   [[nodiscard]] uint32_t capacity() const { return m_pbuffer->capacity(); }
   [[nodiscard]] bool empty() const { return m_pbuffer->empty(); }
   [[nodiscard]] const value_type* c_str() const { assert(m_pbuffer != string::m_pbuffer_empty); return m_pbuffer->c_str(); }
   

public:
   void copy(string& o);
   void clone(const string& o) { string::release(m_pbuffer); _clone(o); }

   string& assign( const char* pbszText ) { return assign( pbszText, static_cast<uint32_t>( std::strlen( pbszText ) ) ); }
   string& assign( std::string_view stringText ) { return assign( stringText.data(), static_cast<uint32_t>(stringText.length()) ); }
   string& assign( const char* pbszText, uint32_t uLength );
   string& assign( const value_type* pbszText, uint32_t uSize, uint32_t uCount );
   string& assign( const value_type* pbszText, uint32_t uSize ) { return assign( pbszText, uSize, gd::utf8::count( pbszText ).first ); }
   string& assign( const string& stringFrom );


/** @name APPEND
 *  Append text 
*///@{
   void push_back( uint8_t ch );
   void push_back( uint16_t ch );
   void push_back( uint32_t ch );

   string& append( uint8_t ch ) { push_back( static_cast<value_type>( ch ) ); return *this; };
   string& append( char8_t ch ) { push_back( static_cast<value_type>( ch ) ); return *this; };
   string& append( uint16_t ch ) { push_back( ch ); return *this; };
   string& append( char16_t ch ) { push_back( static_cast<uint16_t>( ch ) ); return *this; };
   string& append( uint32_t ch ) { push_back( ch ); return *this; };
   string& append( char32_t ch ) { push_back( static_cast<uint32_t>( ch ) ); return *this; };

   string& append( const char* pbszText ) { return append( pbszText, static_cast<uint32_t>( std::strlen( pbszText ) ) ); }
   string& append( std::string_view stringText ) { return append( stringText.data(), static_cast<uint32_t>(stringText.length()) ); }
   string& append( const char* pbszText, uint32_t uLength );
//@}

   [[nodiscard]] gd::utf8::value32 at( size_type uIndex ) const { 
      auto it = begin();
      std::advance( it, uIndex );
      return value32(static_cast<const_iterator::const_pointer>(it) );
   }
   [[nodiscard]] gd::utf8::value32 at( const_iterator it ) const { return value32( static_cast<const_iterator::const_pointer>(it) ); }

   [[nodiscard]] iterator begin() { return iterator( m_pbuffer->c_buffer() ); }
   [[nodiscard]] iterator end() { return iterator( m_pbuffer->c_buffer_end() ); }
   [[nodiscard]] const_iterator begin() const { return const_iterator( m_pbuffer->c_buffer() ); }
   [[nodiscard]] const_iterator end() const { return const_iterator( m_pbuffer->c_buffer_end() ); }

   /// Get last position in buffer
   [[nodiscard]] const value_type* c_end() const { return m_pbuffer->c_buffer_end(); }
   [[nodiscard]] value_type* c_buffer() const { return m_pbuffer->c_buffer(); }
   [[nodiscard]] value_type* c_buffer_end() const { return m_pbuffer->c_buffer_end(); }

   void allocate( uint32_t uSize );
   void allocate_exact( uint32_t uSize );

public:

   struct buffer
   {
      uint32_t m_uSize;               /// string length in bytes
      uint32_t m_uSizeBuffer;         /// string length in bytes
      uint32_t m_uCount;            /// Number of utf8 characters in text
      uint32_t m_uFlags;            /// Internal flags how string logic works
      int32_t  m_iReferenceCount;

      uint32_t flags() const { return m_uFlags; }
      void flags( uint32_t uFlags ) { m_uFlags = uFlags; }
      uint32_t length() const { return m_uSize; }
      void length( uint32_t uLength ) { m_uSize = uLength; }
      uint32_t size() const { return m_uSize; }
      void size( uint32_t uSize ) { m_uSize = uSize; }
      uint32_t count() const { return m_uCount; }
      void count( uint32_t uCount ) { m_uCount = uCount; }
      uint32_t capacity() const { return m_uSizeBuffer; }
      void capacity( uint32_t uCapacity ) { m_uSizeBuffer = uCapacity; }
      bool empty() const { return m_uSize == 0; }

      bool is_refcount() const { return (m_uFlags == 0); }
      bool is_stack() const { return (m_uFlags & eBufferStorageStack) ? true : false; }
      bool is_single() const { return (m_uFlags & eBufferStorageSingle) ? true : false; }
      
      void reset( uint32_t uBufferSize ) { 
         m_uSize = 0; 
         m_uSizeBuffer = uBufferSize;
         m_uCount = 0;
         m_uFlags = 0;
         m_iReferenceCount = 1;
         if( m_uSizeBuffer > 0 ) *c_buffer() = '\0';
      }


      string::pointer c_buffer() { return reinterpret_cast<string::pointer>(this + 1); }
      string::pointer c_buffer_end() { return reinterpret_cast<string::pointer>(this + 1) + size(); }
      string::const_pointer c_buffer() const { return reinterpret_cast<string::const_pointer>(this + 1); }
      string::const_pointer c_buffer_end() const { return reinterpret_cast<string::const_pointer>(this + 1) + size(); }
      string::const_pointer c_str() const { return reinterpret_cast<string::const_pointer>(this + 1); }

      void set_reference( int32_t iCount ) { m_iReferenceCount = iCount; }
      void add_reference() { if( is_single() == false ) m_iReferenceCount++;  }

      void release()
      {                                                                        assert( m_iReferenceCount > 0 );
         if( is_stack() == false )
         {
            m_iReferenceCount--;
            if(m_iReferenceCount == 0) delete[] reinterpret_cast<buffer*>(this);
         }
      }
   };

   buffer* m_pbuffer = string::m_pbuffer_empty;

private:
   void _clone(const string& o);

   static bool is_empty( const buffer* p ) { return p == string::m_pbuffer_empty; }
   static void release( buffer* p ) {
      if( p != string::m_pbuffer_empty ) p->release(); 
   }
   static buffer* clone( buffer* p ) { 
      if( p != string::m_pbuffer_empty ) {
         if( p->is_single() == false ) { p->add_reference(); return p; }
      }
      return string::m_pbuffer_empty;
   }

   static buffer m_pbuffer_empty[];
};

/**
 * @brief buffer for use in string when string is storing data on stack
*/
struct buffer
{
   string::value_type* m_pBuffer;
   uint32_t m_uSize;
};


} }
