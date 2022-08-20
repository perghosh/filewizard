#pragma once
#include <cassert>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#include "gd_variant.h"

#ifndef _GD_BEGIN_VARIANT_VIEW
#  define _GD_BEGIN_VARIANT_VIEW
#endif

#pragma warning( push )
#pragma warning( disable : 4267 26495 26812 )

#ifndef _GD_BEGIN
namespace gd {
#else
_GD_BEGIN
#endif

/**
 * \brief variant_view holds type and value
 *
 *
 *
 \code
 \endcode
 */
class variant_view 
{
public:

private:
   enum enumMASK
   {
      eTYPE             = 0x000000ff,  ///< filter type value
      eLENGTH           = 0x00020000,  ///< item is prefixed with length
   };


// construction
public:
   variant_view()               : m_uType(variant_type::eTypeUnknown)    {}
   variant_view( bool b )       : m_uType(variant_type::eTypeBool)       { m_V.b = b; }
   variant_view( int8_t v )     : m_uType(variant_type::eTypeInt8)       { m_V.int8 = v; }
   variant_view( int16_t v )    : m_uType(variant_type::eTypeInt16)      { m_V.int16 = v; }
   variant_view( int32_t v )    : m_uType(variant_type::eTypeInt32)      { m_V.int32 = v; }
   variant_view( int64_t v )    : m_uType(variant_type::eTypeInt64)      { m_V.int64 = v; }
   variant_view( uint8_t v )    : m_uType(variant_type::eTypeUInt8)      { m_V.uint8 = v; }
   variant_view( uint16_t v )   : m_uType(variant_type::eTypeUInt16)     { m_V.uint16 = v; }
   variant_view( uint32_t v )   : m_uType(variant_type::eTypeUInt32)     { m_V.uint32 = v; }
   variant_view( uint64_t v )   : m_uType(variant_type::eTypeUInt64)     { m_V.uint64 = v; }
   variant_view( float v )      : m_uType(variant_type::eTypeCFloat)     { m_V.f = v; }
   variant_view( double v )     : m_uType(variant_type::eTypeCDouble)    { m_V.d = v; }
   variant_view(const char* v) : m_uType(variant_type::eTypeString), m_uSize(strlen(v)) { m_V.pbsz_const = v;  }
   variant_view(const wchar_t* v) : m_uType(variant_type::eTypeWString), m_uSize(wcslen(v)) { m_V.pwsz_const = v; }
   variant_view(const char* v, size_t uLength) : m_uType(variant_type::eTypeString), m_uSize(uLength) { m_V.pbsz_const = v; }
   variant_view(const std::string& v) : m_uType(variant_type::eTypeString), m_uSize(v.length()) { m_V.pbsz_const = v.c_str(); }
   variant_view(const std::string_view& v) : m_uType(variant_type::eTypeString), m_uSize(v.length()) { m_V.pbsz_const = v.data(); }
   variant_view( const char* v, size_t uLength, bool ): m_uType(variant_type::eTypeString), m_uSize(uLength) { m_V.pbsz = const_cast<char*>(v); }
   variant_view(const wchar_t* v, size_t uLength) : m_uType(variant_type::eTypeWString), m_uSize(uLength) { m_V.pwsz_const = v; }
   variant_view(const unsigned char* v, size_t uLength) : m_uType(variant_type::eTypeBinary), m_uSize(uLength) { m_V.pb_const = v; }
   variant_view(const variant_type::utf8& v) : m_uType(variant_type::eTypeUtf8String), m_uSize(v.m_uLength) { m_V.pbsz_const = v.m_pbsz; }
   variant_view(const variant_type::utf8& v, unsigned int uType) : m_uType(uType), m_uSize(v.m_uLength) { m_V.pbsz_const = v.m_pbsz; }
   variant_view(const variant_type::uuid& v) : m_uType(variant_type::eTypeGuid), m_uSize(16) { m_V.pb_const = v.m_pbUuid; }
   variant_view(unsigned int uType, void* v, size_t uLength) : m_uType(uType), m_uSize(uLength) { m_V.b = v; }

   variant_view(const char* v, bool) : m_uType(variant_type::eTypeString), m_uSize(strlen(v)) { m_V.pbsz_const = v; }
   variant_view(std::string_view v, bool) : m_uType(variant_type::eTypeString), m_uSize(v.length()) { m_V.pbsz_const = v.data(); }

   variant_view( const variant_view& o ) { common_construct( o ); }                        // copy
   variant_view( variant_view&& o ) noexcept { move_construct( o ); }                      // move

   // assign
   variant_view& operator=( const variant_view& o ) { clear(); common_construct( o ); return *this; }
   variant_view& operator=( variant_view&& o ) noexcept { 
      clear();
      if( this != &o ) { ((uint64_t*)this)[0] = ((uint64_t*)&o)[0]; ((uint64_t*)this)[1] = ((uint64_t*)&o)[1]; o.m_uType = variant_type::eTypeUnknown; }
      return *this; }
   ~variant_view() { clear(); }

   void operator=( bool b )     { clear(); m_uType = variant_type::eTypeBool; m_V.b = b; }
   void operator=( int8_t v )   { clear(); m_uType = variant_type::eTypeInt8; m_V.int8 = v;  }
   void operator=( int16_t v )  { clear(); m_uType = variant_type::eTypeInt16; m_V.int16 = v; }
   void operator=( int32_t v )  { clear(); m_uType = variant_type::eTypeInt32; m_V.int32 = v; }
   void operator=( int64_t v )  { clear(); m_uType = variant_type::eTypeInt64; m_V.int64 = v; }
   void operator=( uint8_t v )  { clear(); m_uType = variant_type::eTypeUInt8; m_V.uint8 = v;  }
   void operator=( uint16_t v ) { clear(); m_uType = variant_type::eTypeUInt16; m_V.uint16 = v; }
   void operator=( uint32_t v ) { clear(); m_uType = variant_type::eTypeUInt32; m_V.uint32 = v; }
   void operator=( uint64_t v ) { clear(); m_uType = variant_type::eTypeUInt64; m_V.uint64 = v; }
   void operator=( float v )    { clear(); m_uType = variant_type::eTypeCFloat; m_V.f = v;  }
   void operator=( double v )   { clear(); m_uType = variant_type::eTypeCDouble; m_V.d = v;  }
   void operator=(const char* v) { clear(); m_uType = variant_type::eTypeString; m_uSize = (unsigned int)strlen(v); m_V.pbsz_const = v; }
   void operator=(const wchar_t* v) { clear(); m_uType = variant_type::eTypeWString; m_uSize = (unsigned int)wcslen(v); m_V.pwsz_const = v; }
   void operator=(const variant_type::utf8& v) { clear(); m_uType = variant_type::eTypeUtf8String; m_uSize = v.m_uLength; m_V.pbsz_const = v.m_pbsz; }
   void operator=(const variant_type::uuid& v) { clear(); m_uType = variant_type::eTypeGuid; m_uSize = 16; m_V.pb_const = v.m_pbUuid; }

   int32_t operator+(int32_t v) { return m_V.int32 + v; }
   uint32_t operator+(uint32_t v) { return m_V.uint32 + v; }
   int64_t operator+(int64_t v) { return m_V.int64 + v; }
   uint64_t operator+(uint64_t v) { return m_V.uint64 + v; }
   int32_t operator-(int32_t v) { return m_V.int32 - v; }
   uint32_t operator-(uint32_t v) { return m_V.uint32 - v; }
   int64_t operator-(int64_t v) { return m_V.int64 - v; }
   uint64_t operator-(uint64_t v) { return m_V.uint64 - v; }

   variant_view& operator+=(int32_t v) { m_V.int32 += v; return *this; }
   variant_view& operator+=(uint32_t v) { m_V.uint32 += v; return *this; }
   variant_view& operator+=(int64_t v) { m_V.int64 += v; return *this; }
   variant_view& operator+=(uint64_t v) { m_V.uint64 += v; return *this; }
   variant_view& operator-=(int32_t v) { m_V.int32 -= v; return *this; }
   variant_view& operator-=(uint32_t v) { m_V.uint32 -= v; return *this; }
   variant_view& operator-=(int64_t v) { m_V.int64 -= v; return *this; }
   variant_view& operator-=(uint64_t v) { m_V.uint64 -= v; return *this; }


   void assign( bool v )      { _set_value( v ); }
   void assign( int8_t v )    { _set_value( v ); }
   void assign( int16_t v )   { _set_value( v ); }
   void assign( int32_t v )   { _set_value( v ); }
   void assign( int64_t v )   { _set_value( v ); }
   void assign( uint8_t v )   { _set_value( v ); }
   void assign( uint16_t v )  { _set_value( v ); }
   void assign( uint32_t v )  { _set_value( v ); }
   void assign( uint64_t v )  { _set_value( v ); }
   void assign( float v  )    { _set_value( v ); }
   void assign( double v )    { _set_value( v ); }
   void assign( const char* v ) { _set_value( v ); }
   void assign( const wchar_t* v ) { _set_value( v ); }
   void assign( const unsigned char* v, size_t uLength ) { _set_value( v, uLength ); }
   void assign( const wchar_t* v, unsigned int uLength ) { _set_value( v, uLength ); }
   void assign( const variant_type::utf8& v ) { _set_value( v ); }
   void assign( const variant_type::utf8& v, unsigned int uType ) { _set_value( v, uType ); }
   void assign(const variant_type::uuid& v) { _set_value(v); }


   operator bool() const      { assert(type_number() == variant_type::eTypeNumberBool); return m_V.b; }
   operator int8_t() const    { assert(type_number() == variant_type::eTypeNumberInt8); return m_V.int8; }
   operator int16_t() const   { assert(type_number() == variant_type::eTypeNumberInt16); return m_V.int16; }
   operator int32_t() const   { assert(type_number() == variant_type::eTypeNumberInt32); return m_V.int32; }
   operator int64_t() const   { assert(type_number() == variant_type::eTypeNumberInt64); return m_V.int64; }
   operator uint8_t() const   { assert(type_number() == variant_type::eTypeNumberUInt8); return m_V.uint8; }
   operator uint16_t() const  { assert(type_number() == variant_type::eTypeNumberUInt16); return m_V.uint16; }
   operator uint32_t() const  { assert(type_number() == variant_type::eTypeNumberUInt32); return m_V.uint32; }
   operator uint64_t() const  { assert(type_number() == variant_type::eTypeNumberUInt64); return m_V.uint64; }
   operator float()  const    { assert(type_number() == variant_type::eTypeNumberFloat); return m_V.f; }
   operator double() const    { assert(type_number() == variant_type::eTypeNumberDouble); return m_V.d; }
   operator const char*() const { assert(type_number() == variant_type::eTypeNumberString || type_number() == variant_type::eTypeNumberUtf8String || type_number() == variant_type::eTypeNumberJson || type_number() == variant_type::eTypeNumberXml ); return m_V.pbsz; }
   operator const wchar_t*() const { assert(type_number() == variant_type::eTypeNumberWString); return m_V.pwsz; }
   operator const unsigned char*() const { assert(type_number()== variant_type::eTypeNumberBinary); return m_V.pb; }

   //operator std::string() const { return get_string(); }
   //operator std::wstring() const { return get_wstring_1(); }

   bool operator==(int8_t v) const { if( type_number() == variant_type::eTypeNumberInt8 && m_V.int8 == v ) { return true; } return false; }
   bool operator==(uint8_t v) const { if( type_number() == variant_type::eTypeNumberUInt8 && m_V.uint8 == v ) { return true; } return false; }
   bool operator==(int16_t v) const { if( type_number() == variant_type::eTypeNumberInt16 && m_V.int16 == v ) { return true; } return false; }
   bool operator==(uint16_t v) const { if( type_number() == variant_type::eTypeNumberUInt16 && m_V.uint16 == v ) { return true; } return false; }
   bool operator==( int32_t v ) const { if( type_number() == variant_type::eTypeNumberInt32 && m_V.int32 == v ) { return true; } return false;  }
   bool operator==( uint32_t v ) const { if( type_number() == variant_type::eTypeNumberUInt32 && m_V.uint32 == v ) { return true; } return false;  }
   bool operator==( int64_t v ) const { if( type_number() == variant_type::eTypeNumberInt64 && m_V.int64 == v ) { return true; } return false;  }
   bool operator==( uint64_t v ) const { if( type_number() == variant_type::eTypeNumberUInt64 && m_V.uint64 == v ) { return true; } return false;  }


/** \name RAW
*///@{
   void _set_type( uint32_t uType ) { m_uType = uType; }

   void _set_value( bool v )        { clear(); m_uType = variant_type::eTypeBool;     m_V.b = v; }
   void _set_value( int8_t v )      { clear(); m_uType = variant_type::eTypeInt8;     m_V.int8 = v; }
   void _set_value( int16_t v )     { clear(); m_uType = variant_type::eTypeInt16;    m_V.int16 = v; }
   void _set_value( int32_t v )     { clear(); m_uType = variant_type::eTypeInt32;    m_V.int32 = v; }
   void _set_value( int64_t v )     { clear(); m_uType = variant_type::eTypeInt64;    m_V.int64 = v; }
   void _set_value( uint8_t v )     { clear(); m_uType = variant_type::eTypeUInt8;    m_V.uint8 = v; }
   void _set_value( uint16_t v )    { clear(); m_uType = variant_type::eTypeUInt16;   m_V.uint16 = v; }
   void _set_value( uint32_t v )    { clear(); m_uType = variant_type::eTypeUInt32;   m_V.uint32 = v; }
   void _set_value( uint64_t v )    { clear(); m_uType = variant_type::eTypeUInt64;   m_V.uint64 = v; }
   void _set_value( float v )       { clear(); m_uType = variant_type::eTypeCFloat;   m_V.f = v; }
   void _set_value( double v )      { clear(); m_uType = variant_type::eTypeCDouble;  m_V.d = v; }
   void _set_value( const char* v ) { clear(); m_uType = variant_type::eTypeString; m_uSize = (unsigned int)strlen(v); m_V.pbsz_const = v; }
   void _set_value( const char* v, unsigned int  uLength ) { clear(); m_uType = variant_type::eTypeString; m_uSize = (unsigned int)uLength; m_V.pbsz_const = v; }
   void _set_value( const wchar_t* v ) { clear(); m_uType = variant_type::eTypeWString; m_uSize = (unsigned int)wcslen(v); m_V.pwsz_const = v; }
   void _set_value( const unsigned char* v, unsigned int  uLength ) { clear(); m_uType = variant_type::eTypeBinary; m_uSize = (unsigned int)uLength; m_V.pb = (unsigned char*)allocate(uLength); memcpy( m_V.pb, v, uLength ); }
   void _set_value( const wchar_t* v, unsigned int uLength ) { clear(); m_uType = variant_type::eTypeWString; m_uSize = (unsigned int)uLength; m_V.pwsz_const = v; }
   void _set_value( const variant_type::utf8& v ) { clear(); m_uType = variant_type::eTypeUtf8String; m_uSize = v.m_uLength; m_V.pbsz_const = v.m_pbsz; }
   void _set_value(const variant_type::utf8& v, unsigned int uType) { clear(); m_uType = uType; m_uSize = v.m_uLength; m_V.pbsz_const = v.m_pbsz; }
   void _set_value(const variant_type::uuid& v) { clear(); m_uType = variant_type::eTypeGuid; m_uSize = 16; m_V.pb_const = v.m_pbUuid; }

   // void _set_value( _variant v );

   //void _set_binary_value( const uint8_t* v, unsigned int uLength ) { clear(); m_uType = variant_type::eTypeBinary; m_uSize = uLength; m_V.pb = (unsigned char*)allocate( m_uSize ); memcpy( m_V.pb, v, m_uSize ); }

//@}

private:
   // common copy
   void common_construct( const variant_view& o ) {
      ((uint64_t*)this)[0] = ((uint64_t*)&o)[0];
      ((uint64_t*)this)[1] = ((uint64_t*)&o)[1];
   }

   void move_construct( variant_view& o ) {
      ((uint64_t*)this)[0] = ((uint64_t*)&o)[0];
      ((uint64_t*)this)[1] = ((uint64_t*)&o)[1];
      o.m_uType = variant_type::eTypeUnknown;
   }


// operator
public:
   

public:

/** \name GET/SET
*///@{
   bool get_bool() const; 
   int get_int() const;   
   int32_t get_int32() const { if(m_uType == variant_type::eTypeUInt32) return m_V.int32; else return (int32_t)get_int(); }
   unsigned int get_uint() const;   
   int64_t get_int64() const;
   double get_decimal() const;
   double get_double() const { return get_decimal(); };
   std::string get_string() const;
   std::wstring get_wstring() const;


   // ## buffer methods, use this if you need speed, no heap allocations

   char* get_string( char* pbszBuffer ) const;


   void set_void( void* p ) { clear(); m_uType = variant_type::eTypeVoid; m_uSize = 0; m_V.p = p; }
   void* get_void() const { return m_V.p; }
//@}

/** \name OPERATION
*///@{
   /// Get type variant holds, last two bytes holds static type information
   uint32_t type() const throw() { return m_uType; }
   uint32_t type_number() const throw() { return (m_uType & eTYPE); }
   std::string_view type_name() { return variant::get_type_name_s(m_uType); }
   constexpr std::string_view type_name() const { return variant::get_type_name_s(m_uType); }
   variant_type::enumTypeNumber get_type_number() const throw() { return variant_type::enumTypeNumber(m_uType & variant_type::enumFilter::eFilterTypeNumber); }
   variant_type::enumGroup get_type_group() const throw() { return variant_type::enumGroup(m_uType & variant_type::enumFilter::eFilterTypeGroup); }


   bool is_null() const { return (m_uType == variant_type::eTypeUnknown); }
   bool is_bool() const { return (m_uType & variant_type::eGroupBoolean ? true : false); }
   bool is_int() const { return (m_uType & variant_type::eGroupInteger ? true : false); }
   bool is_integer() const { return (m_uType & variant_type::eGroupInteger ? true : false); }
   bool is_decimal() const { return (m_uType & variant_type::eGroupDecimal ? true : false); }
   bool is_number() const { return m_uType & (variant_type::eGroupInteger | variant_type::eGroupDecimal) ? true : false; }
   bool is_string() const { return (m_uType & variant_type::eGroupString ? true : false); }
   bool is_binary() const { return (m_uType & variant_type::eGroupBinary ? true : false); }

   bool is_json() const { return (m_uType & variant_type::enumFilter::eFilterTypeGroup) == variant_type::eTypeNumberJson ? true : false; }
   bool is_xml() const { return (m_uType & variant_type::enumFilter::eFilterTypeGroup) == variant_type::eTypeNumberXml ? true : false; }
   bool is_void() const { return (m_uType & variant_type::enumFilter::eFilterTypeGroup) == variant_type::eTypeNumberVoid ? true : false; }

   bool is_true() const;

   bool is_char_string() const { return (m_uType == variant_type::eTypeUtf8String || m_uType == variant_type::eTypeString ? true : false); }


   void change( variant_type::enumType eType );

   /*
   /// get value as 64 bit integer
   int32_t get_int32() const throw();
   /// get value as 64 bit integer
   int64_t get_int64() const throw();
   /// get value as double
   double get_double() const throw();
   /// get value as string
   void get_string( gd_std::wstring& rstring ) const;
   gd_std::wstring get_wstring() const;
   */
   /// return pointer to char buffer
   const char* c_str() const {                                                   assert( is_string() );
      return m_V.pbsz; 
   }
   /// return pointer to wide char buffer
   const wchar_t* c_wstr() const {                                               assert( is_string() );
      return m_V.pwsz; 
   }
   
   uint32_t length() const { return m_uSize; }

//@}

/** \name LOGICAL
*///@{
   //bool is_true() const throw();
   void clear() { 
      //if( (m_uType & variant_type::eFlagAllocate) == variant_type::eFlagAllocate ) { free_(); } 
      m_uType = variant_type::eTypeUnknown; 
   }
   bool empty() const { return m_uType == variant_type::eTypeUnknown; }
//@}

protected:
/** \name INTERNAL
*///@{
   void* allocate( size_t uSize ) { return  ::malloc( uSize ); }
   void free_() { 
      //if( !(m_uType & variant_type::eFlagLengthPrefix) ) { ::free( m_V.p ); }
      //else                                               { ::free( ((unsigned char*)m_V.p - sizeof(uint32_t)) ); } 
   }
   //@}

// attributes
public:
   uint32_t m_uType;
   uint32_t m_uSize;    ///< Holds size for data that varies in size, size should be compatible with type that variant is holding. 
                        ///< If string it could be 0 and they you need to calculate size
   union value
   {
      bool        b;
      int8_t      int8;
      int16_t     int16;
      int32_t     int32;
      int64_t     int64;
      uint8_t     uint8;
      uint16_t    uint16;
      uint32_t    uint32;
      uint64_t    uint64;
      char*       pbsz;
      const char* pbsz_const;
      char8_t     putf8;
      char32_t    putf32;
      wchar_t*    pwsz;
      const wchar_t* pwsz_const;
      unsigned char* pb;
      const unsigned char* pb_const;
      float       f;
      double      d;
      void*       p;   
   } m_V;

/** \name INTERNAL FREE FUNCTIONS
*///@{
   static constexpr std::string_view get_type_name_s(uint32_t uType)
   {
      switch( uType & variant::enumMASK::eTYPE )
      {
      case variant_type::enumTypeNumber::eTypeNumberUnknown: return "unknown";
      case variant_type::enumTypeNumber::eTypeNumberBit: return "bit";
      case variant_type::enumTypeNumber::eTypeNumberBool: return "boolean";
      case variant_type::enumTypeNumber::eTypeNumberInt8: return "int8";
      case variant_type::enumTypeNumber::eTypeNumberInt16: return "int16";
      case variant_type::enumTypeNumber::eTypeNumberInt32: return "int32";
      case variant_type::enumTypeNumber::eTypeNumberInt64: return "int64";
      case variant_type::enumTypeNumber::eTypeNumberUInt8: return "uint8";
      case variant_type::enumTypeNumber::eTypeNumberUInt16: return "uint16";
      case variant_type::enumTypeNumber::eTypeNumberUInt32: return "uint32";
      case variant_type::enumTypeNumber::eTypeNumberUInt64: return "uint64";
      case variant_type::enumTypeNumber::eTypeNumberFloat: return "float";
      case variant_type::enumTypeNumber::eTypeNumberDouble: return "double";
      case variant_type::enumTypeNumber::eTypeNumberPointer: return "pointer";
      case variant_type::enumTypeNumber::eTypeNumberGuid:  return "guid";
      case variant_type::enumTypeNumber::eTypeNumberBinary:  return "binary";
      case variant_type::enumTypeNumber::eTypeNumberUtf8String: return "utf8";
      case variant_type::enumTypeNumber::eTypeNumberUtf32String: return "utf32";
      case variant_type::enumTypeNumber::eTypeNumberString: "string";
      case variant_type::enumTypeNumber::eTypeNumberWString: "wstring";
      case variant_type::enumTypeNumber::eTypeNumberJson: return "json";
      case variant_type::enumTypeNumber::eTypeNumberXml: return "xml";
      case variant_type::enumTypeNumber::eTypeNumberVoid: return "void";
         break;
      }

      return std::string_view();
   }
//@}
};

//inline _variant::_variant( variant v ) { ((uint64_t*)this)[0] = ((uint64_t*)&v)[0]; ((uint64_t*)this)[1] = ((uint64_t*)&v)[1];  }

// ä'static_assert( sizeof(_variant) == 16, "_variant size isn't 16 bytes" );
static_assert( sizeof(variant_view) == 16, "variant size isn't 16 bytes" );




} // namespace gd


#pragma warning( pop )