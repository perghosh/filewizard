#pragma once
#include <cassert>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#ifndef _GD_BEGIN_VARIANT
#  define GD_BEGIN_VARIANT
#endif

#pragma warning( push )
#pragma warning( disable : 4267 26495 26812 )

#ifndef _GD_BEGIN
namespace gd {
#else
_GD_BEGIN
#endif


namespace variant_type
{
   enum enumGroup
   {
      eGroupBoolean     = 0x01000000,
      eGroupInteger     = 0x02000000,
      eGroupDecimal     = 0x04000000,
      eGroupString      = 0x08000000,
      eGroupBinary      = 0x10000000,
   };

   enum enumFlag
   {
      eFlagAllocate     = 0x00010000,                                            // variant object has allocated data on heap
      eFlagLengthPrefix = 0x00020000,                                            // pointer to object is prefixed with length
   };

   /*-----------------------------------------*/ /**
    * \brief variant type based on single number
    *
    *
    */
   enum enumTypeNumber
   {
      eTypeNumberUnknown = 0,
      eTypeNumberBit,
      eTypeNumberBool,
      eTypeNumberInt8,
      eTypeNumberInt16,
      eTypeNumberInt32,
      eTypeNumberInt64,
      eTypeNumberUInt8,
      eTypeNumberUInt16,
      eTypeNumberUInt32,
      eTypeNumberUInt64,
      eTypeNumberCFloat,
      eTypeNumberCDouble,
      eTypeNumberGuid,
      eTypeNumberBinary,
      eTypeNumberUtf8String,
      eTypeNumberUtf32String,
      eTypeNumberString,
      eTypeNumberWString,
      eTypeNumberJson,
      eTypeNumberXml,
      eTypeNumberVoid,
   };


   /*-----------------------------------------*/ /**
    * \brief One single type each supported value in variant
    *
    *
    */
   enum enumType
   {
      eTypeUnknown      = eTypeNumberUnknown,
      eTypeBit          = eTypeNumberBit      | eGroupBoolean,
      eTypeBool         = eTypeNumberBool     | eGroupBoolean,
      eTypeInt8         = eTypeNumberInt8     | eGroupInteger,
      eTypeInt16        = eTypeNumberInt16    | eGroupInteger,
      eTypeInt32        = eTypeNumberInt32    | eGroupInteger,
      eTypeInt64        = eTypeNumberInt64    | eGroupInteger,
      eTypeUInt8        = eTypeNumberUInt8    | eGroupInteger,
      eTypeUInt16       = eTypeNumberUInt16   | eGroupInteger,
      eTypeUInt32       = eTypeNumberUInt32   | eGroupInteger,
      eTypeUInt64       = eTypeNumberUInt64   | eGroupInteger,
      eTypeCFloat       = eTypeNumberCFloat   | eGroupDecimal,
      eTypeCDouble      = eTypeNumberCDouble  | eGroupDecimal,
      eTypeGuid         = eTypeNumberGuid     | eGroupBinary,
      eTypeBinary       = eTypeNumberBinary   | eGroupBinary,
      eTypeUtf8String   = eTypeNumberUtf8String | eGroupString,
      eTypeUtf32String  = eTypeNumberUtf32String | eGroupString,
      eTypeString       = eTypeNumberString   | eGroupString,
      eTypeWString      = eTypeNumberWString  | eGroupString,
      eTypeJson         = eTypeNumberJson     | eGroupString,
      eTypeXml          = eTypeNumberXml      | eGroupString,
      eTypeVoid         = eTypeNumberVoid,
   };

   enum enumFilter
   {
      eFilterTypeGroup  = 0xff000000,     ///< flags used to get group information from type
      eFilterTypeNumber = 0x000000ff,     ///< flags for filter out index for type
   };

}


/**
 * \brief variant holds type and value
 *
 *
 *
 \code
 \endcode
 */
class variant 
{
public:
   /*-----------------------------------------*/ /**
    * \brief wrapper used to handle utf8 text
    */
   struct utf8 
   {
      utf8( const char* pbsz ) : m_pbsz(pbsz), m_uLength( strlen( pbsz ) ) {}
      utf8( const char* pbsz, size_t uLength ) : m_pbsz(pbsz), m_uLength( uLength ) {}
      utf8( const utf8& o ) { m_pbsz = o.m_pbsz; m_uLength = o.m_uLength; }
      // attributes
      public:
      const char* m_pbsz;
      size_t m_uLength;
   };

private:
   enum enumMASK
   {
      eTYPE             = 0x000000ff,  ///< filter type value
      eLENGTH           = 0x00020000,  ///< item is prefixed with length
   };


// construction
public:
   variant()               : m_uType(variant_type::eTypeUnknown)    {}
   variant( bool b )       : m_uType(variant_type::eTypeBool)       { m_V.m_b = b; }
   variant( int8_t v )     : m_uType(variant_type::eTypeInt8)       { m_V.m_int8 = v; }
   variant( int16_t v )    : m_uType(variant_type::eTypeInt16)      { m_V.m_int16 = v; }
   variant( int32_t v )    : m_uType(variant_type::eTypeInt32)      { m_V.m_int32 = v; }
   variant( int64_t v )    : m_uType(variant_type::eTypeInt64)      { m_V.m_int64 = v; }
   variant( uint8_t v )    : m_uType(variant_type::eTypeUInt8)      { m_V.m_uint8 = v; }
   variant( uint16_t v )   : m_uType(variant_type::eTypeUInt16)     { m_V.m_uint16 = v; }
   variant( uint32_t v )   : m_uType(variant_type::eTypeUInt32)     { m_V.m_uint32 = v; }
   variant( uint64_t v )   : m_uType(variant_type::eTypeUInt64)     { m_V.m_uint64 = v; }
   variant( float v )      : m_uType(variant_type::eTypeCFloat)     { m_V.m_f = v; }
   variant( double v )     : m_uType(variant_type::eTypeCDouble)    { m_V.m_d = v; }
   variant( const char* v ): m_uType(variant_type::eTypeString|variant_type::eFlagAllocate), m_uSize(strlen(v)) { m_V.m_pbsz = (char*)allocate(m_uSize + 1u); memcpy( m_V.m_pbsz, v, m_uSize + 1u ); }
   variant( const wchar_t* v ): m_uType(variant_type::eTypeWString|variant_type::eFlagAllocate), m_uSize(wcslen(v)) { m_V.m_pwsz = (wchar_t*)allocate((m_uSize + 1u) * sizeof(wchar_t)); memcpy( m_V.m_pwsz, v, (m_uSize + 1u) * sizeof(wchar_t) ); }
   variant( const char* v, size_t uLength ): m_uType(variant_type::eTypeString|variant_type::eFlagAllocate), m_uSize(uLength) { m_V.m_pbsz = (char*)allocate(uLength + 1); memcpy( m_V.m_pbsz, v, uLength ); m_V.m_pbsz[uLength] = '\0';  }
   variant( const char* v, size_t uLength, bool ): m_uType(variant_type::eTypeString), m_uSize(uLength) { m_V.m_pbsz = const_cast<char*>(v); }
   variant( const wchar_t* v, size_t uLength ): m_uType(variant_type::eTypeWString|variant_type::eFlagAllocate), m_uSize(uLength) { m_V.m_pwsz = (wchar_t*)allocate((uLength + 1) * sizeof(wchar_t)); memcpy( m_V.m_pwsz, v, (uLength) * sizeof(wchar_t) ); m_V.m_pwsz[uLength] = L'\0'; }
   variant( const wchar_t* v, size_t uLength, bool ) : m_uType(variant_type::eTypeWString), m_uSize(uLength) { m_V.m_pwsz = const_cast<wchar_t*>(v); }
   variant( const unsigned char* v, size_t uLength ): m_uType(variant_type::eTypeBinary|variant_type::eFlagAllocate), m_uSize(uLength) { m_V.m_pb = (unsigned char*)allocate(uLength); memcpy( m_V.m_pb, v, uLength );  }
   variant( const unsigned char* v, size_t uLength, bool ): m_uType(variant_type::eTypeBinary), m_uSize(uLength) { m_V.m_pb = const_cast<unsigned char*>(v); }
   variant( const utf8& v ) : m_uType(variant_type::eTypeUtf8String|variant_type::eFlagAllocate), m_uSize(v.m_uLength) { m_V.m_pbsz = (char*)allocate( m_uSize + 1u ); memcpy( m_V.m_pbsz, v.m_pbsz, m_uSize + 1u ); }
   variant( const utf8& v, unsigned int uType ) : m_uType(uType|variant_type::eFlagAllocate), m_uSize(v.m_uLength) { m_V.m_pbsz = (char*)allocate( m_uSize + 1u ); memcpy( m_V.m_pbsz, v.m_pbsz, m_uSize + 1u ); }
   variant( const utf8& v, bool ) : m_uType(variant_type::eTypeUtf8String), m_uSize(v.m_uLength) { m_V.m_pbsz = const_cast<char*>(v.m_pbsz); }
   variant( unsigned int uType, void* v, size_t uLength, size_t uDataLength = 0 ) : m_uType(uType), m_uSize( uLength ) { if( uDataLength == 0 ) uDataLength = uLength; m_V.m_pb = (unsigned char*)allocate(uDataLength); memcpy( m_V.m_pb, v, uDataLength );  }

   variant(const char* v, bool) : m_uType(variant_type::eTypeString), m_uSize(strlen(v)) { m_V.m_pbsz_const = v; }
   variant(std::string_view v, bool) : m_uType(variant_type::eTypeString), m_uSize(v.length()) { m_V.m_pbsz_const = v.data(); }


   // explicit variant( const _variant& rv ) : m_uType(variant_type::eTypeUnknown) { _set_value( rv );  }

   //variant( uint32_t uType, int8_t v )     : m_uType(uType)       { m_V.m_int8 = v; }
   //variant( uint32_t uType, const char* v, size_t uLength ): m_uType(uType), m_uSize(uLength) { m_V.m_pbsz = v; }
   //variant( uint32_t uType, const unsigned char* v, size_t uLength ): m_uType(uType), m_uSize(uLength) { m_V.m_pb = v; }

   //variant( uint32_t uType, bool ) : m_uType(uType) {}
  // variant( uint32_t uType, uint32_t uExtra, bool ) : m_uType(uType) { m_uType |= uExtra; }

   variant( const variant& o ) { common_construct( o ); }                        // copy
   variant( variant&& o ) noexcept { move_construct( o ); }                      // move

   // assign
   variant& operator=( const variant& o ) { clear(); common_construct( o ); return *this; }
   variant& operator=( variant&& o ) noexcept { 
      clear();
      if( this != &o ) { ((uint64_t*)this)[0] = ((uint64_t*)&o)[0]; ((uint64_t*)this)[1] = ((uint64_t*)&o)[1]; o.m_uType = variant_type::eTypeUnknown; }
      return *this; }
   ~variant() { clear(); }

   void operator=( bool b )     { clear(); m_uType = variant_type::eTypeBool; m_V.m_b = b; }
   void operator=( int8_t v )   { clear(); m_uType = variant_type::eTypeInt8; m_V.m_int8 = v;  }
   void operator=( int16_t v )  { clear(); m_uType = variant_type::eTypeInt16; m_V.m_int16 = v; }
   void operator=( int32_t v )  { clear(); m_uType = variant_type::eTypeInt32; m_V.m_int32 = v; }
   void operator=( int64_t v )  { clear(); m_uType = variant_type::eTypeInt64; m_V.m_int64 = v; }
   void operator=( uint8_t v )  { clear(); m_uType = variant_type::eTypeUInt8; m_V.m_uint8 = v;  }
   void operator=( uint16_t v ) { clear(); m_uType = variant_type::eTypeUInt16; m_V.m_uint16 = v; }
   void operator=( uint32_t v ) { clear(); m_uType = variant_type::eTypeUInt32; m_V.m_uint32 = v; }
   void operator=( uint64_t v ) { clear(); m_uType = variant_type::eTypeUInt64; m_V.m_uint64 = v; }
   void operator=( float v )    { clear(); m_uType = variant_type::eTypeCFloat; m_V.m_f = v;  }
   void operator=( double v )   { clear(); m_uType = variant_type::eTypeCDouble; m_V.m_d = v;  }
   void operator=( const char* v ) { clear(); m_uType = variant_type::eTypeString|variant_type::eFlagAllocate; m_uSize = (unsigned int)strlen(v); m_V.m_pbsz = (char*)allocate(m_uSize + 1u); memcpy( m_V.m_pbsz, v, m_uSize + 1u ); }
   void operator=( const wchar_t* v ) { clear(); m_uType = variant_type::eTypeWString|variant_type::eFlagAllocate; m_uSize = (unsigned int)wcslen(v); m_V.m_pwsz = (wchar_t*)allocate((m_uSize + 1u) * sizeof(wchar_t)); memcpy( m_V.m_pwsz, v, (m_uSize + 1u) * sizeof(wchar_t) ); }
   void operator=( const utf8& v ) { clear(); m_uType = variant_type::eTypeUtf8String|variant_type::eFlagAllocate; m_uSize = v.m_uLength; m_V.m_pbsz = (char*)allocate( m_uSize + 1u ); memcpy( m_V.m_pbsz, v.m_pbsz, m_uSize + 1u ); }
   //void operator=( const std::pair<const char*,size_t>& rpairV ) { m_uType = variant_type::eTypeString; m_uSize = rpairV.second; m_V.m_pbsz = rpairV.first; }
   //void operator=( const std::pair<const wchar_t*,size_t>& rpairV ) { m_uType = variant_type::eTypeString; m_uSize = rpairV.second; m_V.m_pwsz = rpairV.first; }
   //void operator=( const std::pair<const unsigned char*,size_t>& rpairV ) { m_uType = variant_type::eTypeBinary; m_uSize = rpairV.second; m_V.m_pb = rpairV.first; }

   int32_t operator+(int32_t v) { return m_V.m_int32 + v; }
   uint32_t operator+(uint32_t v) { return m_V.m_uint32 + v; }
   int64_t operator+(int64_t v) { return m_V.m_int64 + v; }
   uint64_t operator+(uint64_t v) { return m_V.m_uint64 + v; }
   int32_t operator-(int32_t v) { return m_V.m_int32 - v; }
   uint32_t operator-(uint32_t v) { return m_V.m_uint32 - v; }
   int64_t operator-(int64_t v) { return m_V.m_int64 - v; }
   uint64_t operator-(uint64_t v) { return m_V.m_uint64 - v; }

   variant& operator+=(int32_t v) { m_V.m_int32 += v; return *this; }
   variant& operator+=(uint32_t v) { m_V.m_uint32 += v; return *this; }
   variant& operator+=(int64_t v) { m_V.m_int64 += v; return *this; }
   variant& operator+=(uint64_t v) { m_V.m_uint64 += v; return *this; }
   variant& operator-=(int32_t v) { m_V.m_int32 -= v; return *this; }
   variant& operator-=(uint32_t v) { m_V.m_uint32 -= v; return *this; }
   variant& operator-=(int64_t v) { m_V.m_int64 -= v; return *this; }
   variant& operator-=(uint64_t v) { m_V.m_uint64 -= v; return *this; }


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
   void assign( const utf8& v ) { _set_value( v ); }
   void assign( const utf8& v, unsigned int uType ) { _set_value( v, uType ); }
   //void assign_binary( const uint8_t* p, size_t uLength ) { _set_binary_value( p, uLength ); }


   operator bool() const      { assert(type_number() == variant_type::eTypeNumberBool); return m_V.m_b; }
   operator int8_t() const    { assert(type_number() == variant_type::eTypeNumberInt8); return m_V.m_int8; }
   operator int16_t() const   { assert(type_number() == variant_type::eTypeNumberInt16); return m_V.m_int16; }
   operator int32_t() const   { assert(type_number() == variant_type::eTypeNumberInt32); return m_V.m_int32; }
   operator int64_t() const   { assert(type_number() == variant_type::eTypeNumberInt64); return m_V.m_int64; }
   operator uint8_t() const   { assert(type_number() == variant_type::eTypeNumberUInt8); return m_V.m_uint8; }
   operator uint16_t() const  { assert(type_number() == variant_type::eTypeNumberUInt16); return m_V.m_uint16; }
   operator uint32_t() const  { assert(type_number() == variant_type::eTypeNumberUInt32); return m_V.m_uint32; }
   operator uint64_t() const  { assert(type_number() == variant_type::eTypeNumberUInt64); return m_V.m_uint64; }
   operator float()  const    { assert(type_number() == variant_type::eTypeNumberCFloat); return m_V.m_f; }
   operator double() const    { assert(type_number() == variant_type::eTypeNumberCDouble); return m_V.m_d; }
   operator const char*() const { assert(type_number() == variant_type::eTypeNumberString || type_number() == variant_type::eTypeNumberUtf8String || type_number() == variant_type::eTypeNumberJson || type_number() == variant_type::eTypeNumberXml ); return m_V.m_pbsz; }
   operator const wchar_t*() const { assert(type_number() == variant_type::eTypeNumberWString); return m_V.m_pwsz; }
   operator const unsigned char*() const { assert(type_number()== variant_type::eTypeNumberBinary); return m_V.m_pb; }

   //operator std::string() const { return get_string(); }
   //operator std::wstring() const { return get_wstring_1(); }

   bool operator==(int8_t v) const { if( type_number() == variant_type::eTypeNumberInt8 && m_V.m_int8 == v ) { return true; } return false; }
   bool operator==(uint8_t v) const { if( type_number() == variant_type::eTypeNumberUInt8 && m_V.m_uint8 == v ) { return true; } return false; }
   bool operator==(int16_t v) const { if( type_number() == variant_type::eTypeNumberInt16 && m_V.m_int16 == v ) { return true; } return false; }
   bool operator==(uint16_t v) const { if( type_number() == variant_type::eTypeNumberUInt16 && m_V.m_uint16 == v ) { return true; } return false; }
   bool operator==( int32_t v ) const { if( type_number() == variant_type::eTypeNumberInt32 && m_V.m_int32 == v ) { return true; } return false;  }
   bool operator==( uint32_t v ) const { if( type_number() == variant_type::eTypeNumberUInt32 && m_V.m_uint32 == v ) { return true; } return false;  }
   bool operator==( int64_t v ) const { if( type_number() == variant_type::eTypeNumberInt64 && m_V.m_int64 == v ) { return true; } return false;  }
   bool operator==( uint64_t v ) const { if( type_number() == variant_type::eTypeNumberUInt64 && m_V.m_uint64 == v ) { return true; } return false;  }


/** \name RAW
*///@{
   void _set_type( uint32_t uType ) { m_uType = uType; }

   void _set_value( bool v )        { clear(); m_uType = variant_type::eTypeBool;     m_V.m_b = v; }
   void _set_value( int8_t v )      { clear(); m_uType = variant_type::eTypeInt8;     m_V.m_int8 = v; }
   void _set_value( int16_t v )     { clear(); m_uType = variant_type::eTypeInt16;    m_V.m_int16 = v; }
   void _set_value( int32_t v )     { clear(); m_uType = variant_type::eTypeInt32;    m_V.m_int32 = v; }
   void _set_value( int64_t v )     { clear(); m_uType = variant_type::eTypeInt64;    m_V.m_int64 = v; }
   void _set_value( uint8_t v )     { clear(); m_uType = variant_type::eTypeUInt8;    m_V.m_uint8 = v; }
   void _set_value( uint16_t v )    { clear(); m_uType = variant_type::eTypeUInt16;   m_V.m_uint16 = v; }
   void _set_value( uint32_t v )    { clear(); m_uType = variant_type::eTypeUInt32;   m_V.m_uint32 = v; }
   void _set_value( uint64_t v )    { clear(); m_uType = variant_type::eTypeUInt64;   m_V.m_uint64 = v; }
   void _set_value( float v )       { clear(); m_uType = variant_type::eTypeCFloat;   m_V.m_f = v; }
   void _set_value( double v )      { clear(); m_uType = variant_type::eTypeCDouble;  m_V.m_d = v; }
   void _set_value( const char* v ) { clear(); m_uType = variant_type::eTypeString|variant_type::eFlagAllocate; m_uSize = (unsigned int)strlen(v); m_V.m_pbsz = (char*)allocate(m_uSize + 1u); memcpy( m_V.m_pbsz, v, m_uSize ); m_V.m_pbsz[m_uSize] = '\0'; }
   void _set_value( const char* v, unsigned int  uLength ) { clear(); m_uType = variant_type::eTypeString|variant_type::eFlagAllocate; m_uSize = (unsigned int)uLength; m_V.m_pbsz = (char*)allocate(m_uSize + 1u); memcpy( m_V.m_pbsz, v, m_uSize ); m_V.m_pbsz[m_uSize] = '\0'; }
   void _set_value( const wchar_t* v ) { clear(); m_uType = variant_type::eTypeWString|variant_type::eFlagAllocate; m_uSize = (unsigned int)wcslen(v); m_V.m_pwsz = (wchar_t*)allocate((m_uSize + 1u) * sizeof(wchar_t)); memcpy( m_V.m_pwsz, v, (m_uSize + 1u) * sizeof(wchar_t) ); }
   void _set_value( const unsigned char* v, unsigned int  uLength ) { clear(); m_uType = variant_type::eTypeBinary|variant_type::eFlagAllocate; m_uSize = (unsigned int)uLength; m_V.m_pb = (unsigned char*)allocate(uLength); memcpy( m_V.m_pb, v, uLength ); }
   void _set_value( const wchar_t* v, unsigned int uLength ) { clear(); m_uType = variant_type::eTypeWString|variant_type::eFlagAllocate; m_uSize = (unsigned int)uLength; m_V.m_pwsz = (wchar_t*)allocate((m_uSize + 1u) * sizeof(wchar_t)); memcpy( m_V.m_pwsz, v, (m_uSize) * sizeof(wchar_t) );  m_V.m_pwsz[m_uSize] = 0; }
   void _set_value( const utf8& v ) { clear(); m_uType = variant_type::eTypeUtf8String|variant_type::eFlagAllocate; m_uSize = v.m_uLength; m_V.m_pbsz = (char*)allocate( m_uSize + 1u ); memcpy( m_V.m_pbsz, v.m_pbsz, m_uSize + 1u ); }
   void _set_value( const utf8& v, unsigned int uType ) { clear(); m_uType = uType|variant_type::eFlagAllocate; m_uSize = v.m_uLength; m_V.m_pbsz = (char*)allocate( m_uSize + 1u ); memcpy( m_V.m_pbsz, v.m_pbsz, m_uSize + 1u ); }

   // void _set_value( _variant v );

   //void _set_binary_value( const uint8_t* v, unsigned int uLength ) { clear(); m_uType = variant_type::eTypeBinary|variant_type::eFlagAllocate; m_uSize = uLength; m_V.m_pb = (unsigned char*)allocate( m_uSize ); memcpy( m_V.m_pb, v, m_uSize ); }

//@}

private:
   // common copy
   void common_construct( const variant& o ) {
      m_uType = o.m_uType;
      m_uSize = o.m_uSize;
      if( (m_uType & variant_type::eFlagAllocate) == 0 )
      {
         m_V.m_int64 = o.m_V.m_int64;
      }
      else if( type_number() == variant_type::eTypeNumberWString )
      {
         m_V.m_pwsz = (wchar_t*)allocate((m_uSize + 1u) * sizeof(wchar_t)); memcpy( m_V.m_pwsz, o.m_V.m_pwsz, (m_uSize + 1u) * sizeof(wchar_t) );
      }
      else if( type_number() == variant_type::eTypeNumberString || type_number() == variant_type::eTypeNumberUtf8String )
      {
         m_V.m_pbsz = (char*)allocate(m_uSize + 1u); memcpy( m_V.m_pbsz, o.m_V.m_pbsz, m_uSize + 1u );
      }
      else
      {
         m_V.m_pb = (unsigned char*)allocate(m_uSize); memcpy( m_V.m_pb, o.m_V.m_pb, m_uSize );
      }
   }

   void move_construct( variant& o ) {
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
   int32_t get_int32() const { if(m_uType == variant_type::eTypeUInt32) return m_V.m_int32; else return (int32_t)get_int(); }
   unsigned int get_uint() const;   
   int64_t get_int64() const;
   double get_decimal() const;
   std::string get_string() const;
   std::wstring get_wstring() const;
   //gd_std::_variant get__variant() const { return _variant( this ); }


   void set_void( void* p ) { clear(); m_uType = variant_type::eTypeVoid; m_uSize = 0; m_V.m_p = p; }
   void* get_void() const { return m_V.m_p; }
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
   bool is_decimal() const { return (m_uType & variant_type::eGroupDecimal ? true : false); }
   bool is_string() const { return (m_uType & variant_type::eGroupString ? true : false); }
   bool is_binary() const { return (m_uType & variant_type::eGroupBinary ? true : false); }

   bool is_json() const { return (m_uType & variant_type::enumFilter::eFilterTypeGroup) == variant_type::eTypeNumberJson ? true : false; }
   bool is_xml() const { return (m_uType & variant_type::enumFilter::eFilterTypeGroup) == variant_type::eTypeNumberXml ? true : false; }

   bool is_true() const;

   bool is_void() const { return (m_uType & variant_type::enumFilter::eFilterTypeGroup) == variant_type::eTypeNumberVoid ? true : false; }

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
      return m_V.m_pbsz; 
   }
   /// return pointer to wide char buffer
   const wchar_t* c_wstr() const {                                               assert( is_string() );
      return m_V.m_pwsz; 
   }
   
   uint32_t length() const { return m_uSize; }

//@}

/** \name GROUP
*///@{
/*
   bool is_number() const throw()   { return (m_uType & eColumnTypeGroupNumber   ? true : false); }
   bool is_integer() const throw()  { return (m_uType & eColumnTypeGroupInteger  ? true : false); }
   bool is_decimal() const throw()  { return (m_uType & eColumnTypeGroupDecimal  ? true : false); }
   bool is_signed() const throw()   { return (m_uType & eColumnTypeGroupSigned   ? true : false); }
   bool is_string() const throw()   { return (m_uType & eColumnTypeGroupString   ? true : false); }
   bool is_date() const throw()     { return (m_uType & eColumnTypeGroupDate     ? true : false); }
   bool is_binary() const throw()   { return (m_uType & eColumnTypeGroupBinary   ? true : false); }

   bool is_08() const throw()       { return (m_uType & eColumnTypeGroup08       ? true : false); }
   bool is_16() const throw()       { return (m_uType & eColumnTypeGroup16       ? true : false); }
   bool is_32() const throw()       { return (m_uType & eColumnTypeGroup32       ? true : false); }
   bool is_64() const throw()       { return (m_uType & eColumnTypeGroup64       ? true : false); }

   bool is_ascii() const throw()    { return (m_uType & eColumnTypeGroupChar     ? true : false); }
   bool is_unicode() const throw()  { return (m_uType & eColumnTypeGroupWChar    ? true : false); }

   bool is_null() const throw()     { return (m_uType & eColumnTypeGroupNull     ? true : false); }
   */
//@}

/** \name LOGICAL
*///@{
   //bool is_true() const throw();
   void clear() { 
      if( (m_uType & variant_type::eFlagAllocate) == variant_type::eFlagAllocate ) { free_(); } 
      m_uType = variant_type::eTypeUnknown; 
   }
   bool empty() const { return m_uType == variant_type::eTypeUnknown; }
//@}

protected:
/** \name INTERNAL
*///@{
   void* allocate( size_t uSize ) { return  ::malloc( uSize ); }
   void free_() { 
      if( !(m_uType & variant_type::eFlagLengthPrefix) ) { ::free( m_V.m_p ); }
      else                                               { ::free( ((unsigned char*)m_V.m_p - sizeof(uint32_t)) ); } 
   }
   //@}

// attributes
public:
   uint32_t m_uType;
   uint32_t m_uSize;    ///< Holds size for data that varies in size, size should be compatible with type that variant is holding. 
                        ///< If string it could be 0 and they you need to calculate size
   union value
   {
      bool        m_b;
      int8_t      m_int8;
      int16_t     m_int16;
      int32_t     m_int32;
      int64_t     m_int64;
      uint8_t     m_uint8;
      uint16_t    m_uint16;
      uint32_t    m_uint32;
      uint64_t    m_uint64;
      char*       m_pbsz;
      const char* m_pbsz_const;
      char8_t     m_putf8;
      char32_t    m_putf32;
      wchar_t*    m_pwsz;
      wchar_t*    m_pwsz_const;
      unsigned char* m_pb;
      float       m_f;
      double      m_d;
      void*       m_p;   
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
      case variant_type::enumTypeNumber::eTypeNumberCFloat: return "float";
      case variant_type::enumTypeNumber::eTypeNumberCDouble: return "double";
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
static_assert( sizeof(variant) == 16, "variant size isn't 16 bytes" );




} // namespace gd


#pragma warning( pop )