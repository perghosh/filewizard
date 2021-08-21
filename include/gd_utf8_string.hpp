#pragma once

#include <cassert>
#include <iterator>
#include <string_view>
#include <initializer_list>
#include <iostream>
		
#include "gd_utf8.hpp"

// LOOK: https://github.com/Maultasche/UtfString/blob/master/src/Utf8String.cpp

#ifdef DEBUG
#define DEBUG_ONLY(x) x
#else
   DEBUG_ONLY(x) (void)0
#endif

namespace gd::utf8 { 

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

/*
      operator char() const { assert( m_uValue < 0x100 ); return static_cast<char>(m_uValue); }
      operator char8_t() const { assert( m_uValue < 0x100 ); 
         const uint8_t* p = reinterpret_cast<const uint8_t*>(&m_uValue) + (m_uValue >= 0x80 ? 2 : 3);
         auto ch = gd::utf8::character( p );
         return static_cast<char8_t>(ch); 
      }
      operator wchar_t() const { assert( m_uValue < 0x10000 ); return static_cast<wchar_t>(m_uValue); }
      */
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
      eBufferStorageSingle           = 0x02,   // string data is not reference counted
   };

public:
   typedef string                self;
   typedef uint8_t               value_type;
   typedef value_type*           pointer;
   typedef const value_type*     const_pointer;
   typedef value_type&           reference;
   typedef const value_type&     const_reference;
   typedef std::size_t            size_type;


public:
   struct const_iterator;

   /**
    * @brief iterate utf8 string
    * @note Can be used to any utf8 formated uint8_t* string
    */
   struct iterator
   {
      typedef iterator           self;
      typedef uint8_t            value_type;
      typedef uint8_t*           pointer;
      typedef uint8_t&           reference;
      typedef std::size_t        size_type;
      typedef std::ptrdiff_t     difference_type;
      typedef std::bidirectional_iterator_tag  iterator_category;

      iterator(): m_pPosition(nullptr) { }
      //iterator( iterator& it ): m_pPosition(it.m_pPosition) { }
      iterator( pointer p ): m_pPosition(p) { }
      iterator( const_iterator& o ): m_pPosition(const_cast<pointer>( o.m_pPosition )) { }

      self& operator=( const_iterator& o ) { m_pPosition = const_cast<pointer>( o.m_pPosition ); return *this; }

      reference operator*() const { return *m_pPosition; }
      pointer operator->() { return m_pPosition; }

      operator const value_type*() { return m_pPosition; }

      self& operator++() { m_pPosition = gd::utf8::move::next( m_pPosition ); return *this; }  
      self operator++( int ) { self it = *this; ++(*this); return it; }  

      self& operator--() { m_pPosition = gd::utf8::move::previous( m_pPosition ); return *this; }  
      self operator--( int ) { self it = *this; --(*this); return it; }  

      bool operator==(const self& r) { return m_pPosition == r.m_pPosition; }
      bool operator!=(const self& r) { return !(*this == r); }

      /*
      bool operator==(const const_iterator& r) const { return m_pPosition == r.m_pPosition; }
      bool operator!=(const const_iterator& r) const { return !(*this == r); }
      */

      iterator operator+(std::size_t uMove) { auto p = m_pPosition; p = gd::utf8::move::next(p, static_cast<uint32_t>(uMove)); return self( p ); }
      iterator operator-(std::size_t uMove) { auto p = m_pPosition; p = gd::utf8::move::previous(p, static_cast<uint32_t>(uMove)); return self( p ); }


      pointer get() { return m_pPosition; }
      self& next() { 
         auto p = gd::utf8::move::next( m_pPosition );
         if( p != m_pPosition ) m_pPosition = p;
         else m_pPosition++;
         return *this; 
      }
      gd::utf8::value32 value32() const { return gd::utf8::value32( m_pPosition ); }


      pointer m_pPosition;      /// position in string   
   };

   /**
    * @brief iterate utf8 string
    * @note Can be used to any utf8 formated const uint8_t* string
    */
   struct const_iterator
   {
      typedef const_iterator     self;
      typedef uint8_t            value_type;
      typedef value_type*        pointer;
      typedef const value_type*  const_pointer;
      typedef value_type&        reference;
      typedef const value_type&  const_reference;
      typedef std::size_t        size_type;
      typedef std::ptrdiff_t     difference_type;
      typedef std::bidirectional_iterator_tag  iterator_category;

      const_iterator(): m_pPosition(nullptr) { }
      const_iterator( const_pointer p ): m_pPosition(p) { }
      const_iterator( iterator it ): m_pPosition(it.get()) { }

      const_iterator& operator=( iterator& o ) { m_pPosition = o.m_pPosition; return *this; }

      const_reference operator*() const { return *m_pPosition; }
      const_pointer operator->() const { return m_pPosition; }

      operator const value_type*() { return m_pPosition; }
      //operator char() const { return static_cast<char>( *m_pPosition ); }

      self& operator++() { m_pPosition = gd::utf8::move::next(m_pPosition); return *this; }
      self operator++( int ) { const_iterator it = *this; ++(*this); return it; }  

      self& operator--() { m_pPosition = gd::utf8::move::previous( m_pPosition ); return *this; }  
      self operator--( int ) { const_iterator it = *this; --(*this); return *this; }  

      bool operator==(const const_iterator& r) const { return m_pPosition == r.m_pPosition; }
      bool operator!=(const const_iterator& r) const { return !(*this == r); }

      bool operator==(const iterator& r) const { return m_pPosition == r.m_pPosition; }
      bool operator!=(const iterator& r) const { return !(*this == r); }

      const_iterator operator+(std::size_t uMove) { auto p = m_pPosition; p = gd::utf8::move::next(p, static_cast<uint32_t>(uMove)); return self( p ); }
      const_iterator operator-(std::size_t uMove) { auto p = m_pPosition; p = gd::utf8::move::previous(p, static_cast<uint32_t>(uMove)); return self( p ); }

      const_pointer get() { return m_pPosition; }
      gd::utf8::value32 value32() const { return gd::utf8::value32( m_pPosition ); }

      const_pointer m_pPosition;      /// position in string   
   };

   /**
    * @brief iterate utf8 string
    * @note This is special for use with std regular expression objects
    */
   struct char_const_iterator
   {
      typedef char_const_iterator self;
      typedef char               value_type;
      typedef value_type*        pointer;
      typedef const value_type*  const_pointer;
      typedef value_type&        reference;
      typedef const value_type&  const_reference;
      typedef std::size_t        size_type;
      typedef std::ptrdiff_t     difference_type;
      typedef std::bidirectional_iterator_tag  iterator_category;

      char_const_iterator(): m_pPosition(nullptr) { }
      explicit char_const_iterator( const_pointer p ): m_pPosition(p) { }
      explicit char_const_iterator( iterator it ): m_pPosition(reinterpret_cast<const_pointer>(it.get())) { }

      self& operator=( iterator& o ) { m_pPosition = reinterpret_cast<const_pointer>(o.m_pPosition); return *this; }

      const_reference operator*() const { return *m_pPosition; }
      const_pointer operator->() const { return m_pPosition; }

      operator const value_type*() { return m_pPosition; }

      self& operator++() { m_pPosition = gd::utf8::move::next(m_pPosition); return *this; }
      self operator++( int ) { self it = *this; ++(*this); return it; }  

      self& operator--() { m_pPosition = gd::utf8::move::previous( m_pPosition ); return *this; }  
      self operator--( int ) { self it = *this; --(*this); return *this; }  

      bool operator==(const self& r) const { return m_pPosition == r.m_pPosition; }
      bool operator!=(const self& r) const { return !(*this == r); }

      self operator+(std::size_t uMove) { auto p = m_pPosition; p = gd::utf8::move::next(p, static_cast<uint32_t>(uMove)); return self( p ); }
      self operator-(std::size_t uMove) { auto p = m_pPosition; p = gd::utf8::move::previous(p, static_cast<uint32_t>(uMove)); return self( p ); }

      const_pointer get() { return m_pPosition; }
      gd::utf8::value32 value32() const { return gd::utf8::value32( m_pPosition ); }

      const_pointer m_pPosition;      /// position in string   
   };



public:
   /** @name Construct
   *///@{
   string() {}
   explicit string( gd::utf8::buffer bufferStack );
   explicit string( const char* pbszText ) { assign( pbszText ); }
   string( std::string_view stringText );
   explicit string( const char* pbszText, uint32_t uLength ) { assign( pbszText, uLength ); }
   explicit string( const_iterator itFrom, const_iterator itTo ) { assign( itFrom.get(), itTo.get() - itFrom.get() ); }
   template<typename CHAR>
   string( std::initializer_list<CHAR> listString ) { assign( listString ); }

   string(string&& o) noexcept : m_pbuffer( o.m_pbuffer ) {
      o.m_pbuffer = string::m_pbuffer_empty;
      //m_pbuffer->add_reference();
      DEBUG_ONLY( m_psz = m_pbuffer->c_str() );
   }

   string(const string& o) { copy(o); }
   //string(const string& o): m_pbuffer( string::m_pbuffer_empty ) { clone(o); }
   ~string() {                                                                assert( m_pbuffer->get_reference() != 0 );
      string::release( m_pbuffer ); 
   }
   //@}

   string& operator=(const string& o) { copy(o); return *this; }
   //string& operator=(const string& o) { clone(o); return *this; }
   string& operator=(string&& o) noexcept { 
      m_pbuffer = o.m_pbuffer; o.m_pbuffer = string::m_pbuffer_empty;
      return *this;
   }
   string& operator=( const char* pbszText ) { return assign( pbszText ); }
   string& operator+=( const string& o ) { return append( o ); }
   string& operator+=( std::string_view stringText ) { return append( stringText ); }


   gd::utf8::value32 operator [](size_type uIndex ) const { return at( uIndex ); }

   bool operator==( const string& o ) const { return compare( o ); }
   friend bool operator==( const string& stringEqualWith, std::string_view stringEqualTo );
   friend std::ostream& operator<<( std::ostream& ostreamTo, const string& s );

   [[nodiscard]] uint32_t size() const { return m_pbuffer->size(); }
   [[nodiscard]] uint32_t length() const { return m_pbuffer->size(); }
   [[nodiscard]] uint32_t count() const { return m_pbuffer->count(); }
   [[nodiscard]] uint32_t capacity() const { return m_pbuffer->capacity(); }
   [[nodiscard]] bool empty() const { return m_pbuffer->empty(); }
   //[[nodiscard]] const value_type* c_buffer() const { assert(m_pbuffer != string::m_pbuffer_empty); return m_pbuffer->c_buffer(); }
   [[nodiscard]] const char* c_str() const { assert(m_pbuffer != string::m_pbuffer_empty); return m_pbuffer->c_str(); }
   

public:
   void copy(const string& o);
   void clone(const string& o) { string::release(m_pbuffer); _clone(o); }

   /** @name COMPARE
    *///@{
   bool compare( const value_type* pbszText ) const noexcept { return strcmp( c_str(), decltype(c_str())(pbszText) ) == 0; }
   bool compare( const string& o ) const noexcept { return count() == o.count() ? memcmp( c_str(), o.c_str(), size() ) == 0 : false; }
   //@}


   string& assign( const char* pbszText ) { return assign( pbszText, static_cast<uint32_t>( std::strlen( pbszText ) ) ); }
   string& assign( std::string_view stringText ) { return assign( stringText.data(), static_cast<uint32_t>(stringText.length()) ); }
   string& assign( const char* pbszText, uint32_t uLength );
   string& assign( const value_type* pbszText, uint32_t uSize, uint32_t uCount );
   //string& assign( const value_type* pbszText, uint32_t uSize ) { return assign( pbszText, uSize, gd::utf8::count( pbszText, pbszText + uSize ).first ); }
   string& assign( const value_type* pbszText, std::size_t uSize ) { return assign( pbszText, static_cast<uint32_t>( uSize ), gd::utf8::count( pbszText, pbszText + uSize ).first ); }
   string& assign( const string& stringFrom );
   template<typename CHAR>
   string& assign( std::initializer_list<CHAR> listString ) { // _Ilist.begin(), _Convert_size<size_type>(_Ilist.size()));
      for( auto it : listString ) push_back( static_cast<value_type>(it) );
      return *this;
   }


   /** @name APPEND
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
   string& append( const value_type* puText, uint32_t uLength );
   string& append( const value_type* puText, uint32_t uLength, uint32_t uCount );
   string& append( const string& o ) { return append( o.c_buffer(), o.size(), o.count() ); }
   //@}


   /** @name REPLACE
    *///@{
   string& insert( iterator itFrom, iterator itTo, uint32_t uSize, uint32_t uCharacter );
   template<typename CHARACTER>
   string& insert( iterator itFrom, iterator itTo, uint32_t uSize, CHARACTER Character ) {
      uint32_t uCharacter;
      if constexpr( sizeof(Character) == sizeof(uint8_t) ) uCharacter = static_cast<uint8_t>(Character);       // 1 byte
      else if constexpr( sizeof(Character) == sizeof(uint16_t) ) uCharacter = static_cast<uint16_t>(Character);// 2 byte value
      else uCharacter = static_cast<uint32_t>(Character);                                                      // 4 byte or over

      return insert( itFrom, itTo, uSize, static_cast<uint32_t>( uCharacter )); 
   }
   //@}

   /** @name REPLACE
    *///@{
   std::size_t squeeze() { return squeeze( begin(), end(), 0 ); }
   std::size_t squeeze( iterator itFrom, iterator itTo, uint32_t ch );
   //@}


   [[nodiscard]] gd::utf8::value32 at( size_type uIndex ) const { 
      auto it = begin();
      std::advance( it, uIndex );
      return value32( it.m_pPosition );
   }
   [[nodiscard]] gd::utf8::value32 at( const_iterator it ) const { return value32( it.m_pPosition ); }

   [[nodiscard]] const_iterator find( value_type ch ) const;
   [[nodiscard]] const_iterator find( value_type ch, const_iterator itFrom ) const;

   [[nodiscard]] const_iterator find( const_pointer pbszText, uint32_t uLength ) const;
   [[nodiscard]] const_iterator find( const_pointer pbszFind, std::size_t uLength ) const { return find( pbszFind, static_cast<uint32_t>( uLength ) ); }
   [[nodiscard]] const_iterator find( const_pointer pbszFind ) const { return find( pbszFind, std::strlen( reinterpret_cast<const char *>(pbszFind) ) );  }

   [[nodiscard]] const_iterator find( std::string_view stringFind ) const;

   [[nodiscard]] const_iterator find( const_iterator itFrom, const_pointer pbszFind ) const { return find( itFrom, pbszFind, static_cast<uint32_t>( std::strlen( reinterpret_cast<const char *>(pbszFind) ) ) ); }
   [[nodiscard]] const_iterator find( const_iterator itFrom, const_pointer pbszFind, uint32_t uLength ) const;


   [[nodiscard]] string substr( const_iterator itFrom ) { return substr( itFrom, cend() ); };
   // Returns a newly constructed string object with its value initialized to a copy of a substring of this object
   [[nodiscard]] string substr( const_iterator itFrom, const_iterator itTo );

   iterator erase( iterator itFirst, iterator itLast, bool bCount );


   [[nodiscard]] iterator begin() { return iterator( m_pbuffer->c_buffer() ); }
   [[nodiscard]] iterator end() { return iterator( m_pbuffer->c_buffer_end() ); }
   [[nodiscard]] const_iterator begin() const { return const_iterator( m_pbuffer->c_buffer() ); }
   [[nodiscard]] const_iterator end() const { return const_iterator( m_pbuffer->c_buffer_end() ); }
   [[nodiscard]] const_iterator cbegin() const { return const_iterator( m_pbuffer->c_buffer() ); }
   [[nodiscard]] const_iterator cend() const { return const_iterator( m_pbuffer->c_buffer_end() ); }
   [[nodiscard]] char_const_iterator cbegin_char() const { return char_const_iterator( m_pbuffer->c_str() ); }
   [[nodiscard]] char_const_iterator cend_char() const { return char_const_iterator( m_pbuffer->c_str_end() ); }

   /// Get last position in buffer
   [[nodiscard]] const value_type* c_end() const { return m_pbuffer->c_buffer_end(); }
   [[nodiscard]] value_type* c_buffer() const { return m_pbuffer->c_buffer(); }
   [[nodiscard]] value_type* c_buffer_end() const { return m_pbuffer->c_buffer_end(); }

   void allocate( uint32_t uSize );
   void allocate_exact( uint32_t uSize );

   /** @name SUPPORT methods
    *///@{
   pointer expand( value_type* pvPosition, uint32_t uSize );
   pointer contract( value_type* pvPosition, uint32_t uSize );
   static bool verify_iterator( const string& stringObject, const_pointer p );
   //@}

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

      void add_size( uint32_t uSize ) { m_uSize += uSize; }

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
      const char* c_str() const { return reinterpret_cast<const char*>(this + 1); }
      const char* c_str_end() const { return reinterpret_cast<const char*>(this + 1) + size(); }

      void set_reference( int32_t iCount ) { m_iReferenceCount = iCount; }
      void add_reference() { if( is_single() == false ) m_iReferenceCount++;  }
      int32_t get_reference() const { return m_iReferenceCount; }

      void release()
      {                                                                        assert( m_iReferenceCount > 0 );
         if( is_stack() == false )
         {
            m_iReferenceCount--;
            if(m_iReferenceCount == 0) delete[] reinterpret_cast<buffer*>(this);
         }
      }
   };

#  ifdef DEBUG
   const char* m_psz = nullptr;
   std::pair<const char*, uint32_t> dump() {
      uint32_t uSize = 0;
      if( m_psz != nullptr ) uSize = (uint32_t)std::strlen( m_psz );
      return { m_psz, uSize };
   }
#  endif

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


}
