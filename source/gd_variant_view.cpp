#include <cwchar>

#include "gd_utf8.hpp"
#include "gd_variant_view.h"


#if defined(_MSC_VER)
   #pragma warning( disable : 4996 26812 )
#else
   #pragma GCC diagnostic ignored "-Wswitch"
   #pragma GCC diagnostic ignored "-Wformat"
#endif



#ifndef _GD_BEGIN
namespace gd {
#else
_GD_BEGIN
#endif

using namespace variant_type;


bool variant_view::get_bool() const
{
   switch( type_number() )
   {
   case eTypeNumberUnknown     : return false;
   case eTypeNumberBit         : return m_V.int8 != 0 ? true : false;
   case eTypeNumberBool        : return m_V.b;
   case eTypeNumberInt8        : return m_V.int8 != 0 ? true : false;
   case eTypeNumberInt16       : return m_V.int16 != 0 ? true : false;
   case eTypeNumberInt32       : return m_V.int32 != 0 ? true : false;
   case eTypeNumberInt64       : return m_V.int64 != 0 ? true : false;
   case eTypeNumberUInt8       : return m_V.uint8 != 0 ? true : false;
   case eTypeNumberUInt16      : return m_V.uint16 != 0 ? true : false;
   case eTypeNumberUInt32      : return m_V.uint32 != 0 ? true : false;
   case eTypeNumberUInt64      : return m_V.uint64 != 0 ? true : false;
   case eTypeNumberFloat       : return m_V.f != 0.0 ? true : false;
   case eTypeNumberDouble      : return m_V.d != 0.0 ? true : false;
   case eTypeNumberGuid        : return false;
   case eTypeNumberBinary      : return false;
   case eTypeNumberUtf8String  : 
   case eTypeNumberString      : return *m_V.pbsz != '\0' ? true : false;
   case eTypeNumberWString     : return *m_V.pwsz != L'\0' ? true : false;
   default:                                                                      assert( false );
   }

   return false;
}


int variant_view::get_int() const
{
   switch( type_number() )
   {
   case eTypeNumberUnknown     : return 0;
   case eTypeNumberBit         : return (m_V.int8 & 0x01 ? 1 : 0);
   case eTypeNumberBool        : return (m_V.b == true ? 1 : 0);
   case eTypeNumberInt8        : return (int)m_V.int8;
   case eTypeNumberInt16       : return (int)m_V.int16;
   case eTypeNumberInt32       : return (int)m_V.int32;
   case eTypeNumberInt64       : return (int)m_V.int64;
   case eTypeNumberUInt8       : return (int)m_V.uint8;
   case eTypeNumberUInt16      : return (int)m_V.uint16;
   case eTypeNumberUInt32      : return (int)m_V.uint32;
   case eTypeNumberUInt64      : return (int)m_V.uint64;
   case eTypeNumberFloat       : return (int)m_V.f;
   case eTypeNumberDouble      : return (int)m_V.d;
   case eTypeNumberGuid        : return 0;
   case eTypeNumberBinary      : return 0;
   case eTypeNumberUtf8String  : 
   case eTypeNumberString      : return atoi( m_V.pbsz );
#if defined(_MSC_VER)
   case eTypeNumberWString     : return _wtoi( m_V.pwsz );
#else   
   case eTypeNumberWString     : return wcstol( m_V.pwsz, 0, 10 );
#endif   
   default:                                                                      assert( false );
   }

   return 0;
}

unsigned int variant_view::get_uint() const
{
   switch( type_number() )
   {
   case eTypeNumberUnknown     : return 0;
   case eTypeNumberBit         : return (m_V.int8 & 0x01 ? 1 : 0);
   case eTypeNumberBool        : return (m_V.b == true ? 1 : 0);
   case eTypeNumberInt8        : return (unsigned int)m_V.int8;
   case eTypeNumberInt16       : return (unsigned int)m_V.int16;
   case eTypeNumberInt32       : return (unsigned int)m_V.int32;
   case eTypeNumberInt64       : return (unsigned int)m_V.int64;
   case eTypeNumberUInt8       : return (unsigned int)m_V.uint8;
   case eTypeNumberUInt16      : return (unsigned int)m_V.uint16;
   case eTypeNumberUInt32      : return (unsigned int)m_V.uint32;
   case eTypeNumberUInt64      : return (unsigned int)m_V.uint64;
   case eTypeNumberFloat       : return (unsigned int)m_V.f;
   case eTypeNumberDouble      : return (unsigned int)m_V.d;
   case eTypeNumberGuid        : return 0;
   case eTypeNumberBinary      : return 0;
   case eTypeNumberUtf8String  : 
   case eTypeNumberString      : return (unsigned int)atoi( m_V.pbsz );
#if defined(_MSC_VER)
   case eTypeNumberWString     : return (unsigned int)_wtoi( m_V.pwsz );
#else   
   case eTypeNumberWString     : return (unsigned int)wcstol( m_V.pwsz, 0, 10 );
#endif   

   default:                                                                      assert( false );
   }

   return 0;
}


int64_t variant_view::get_int64() const
{
   switch( type_number() )
   {
   case eTypeNumberUnknown     : return 0;
   case eTypeNumberBit         : return (m_V.int8 & 0x01 ? 1 : 0);
   case eTypeNumberBool        : return (m_V.b == true ? 1 : 0);
   case eTypeNumberInt8        : return (int64_t)m_V.int8;
   case eTypeNumberInt16       : return (int64_t)m_V.int16;
   case eTypeNumberInt32       : return (int64_t)m_V.int32;
   case eTypeNumberInt64       : return (int64_t)m_V.int64;
   case eTypeNumberUInt8       : return (int64_t)m_V.uint8;
   case eTypeNumberUInt16      : return (int64_t)m_V.uint16;
   case eTypeNumberUInt32      : return (int64_t)m_V.uint32;
   case eTypeNumberUInt64      : return (int64_t)m_V.uint64;
   case eTypeNumberFloat       : return (int64_t)m_V.f;
   case eTypeNumberDouble      : return (int64_t)m_V.d;
   case eTypeNumberGuid        : return 0;
   case eTypeNumberBinary      : return 0;
   case eTypeNumberUtf8String  : 
   case eTypeNumberString      : return atoll( m_V.pbsz );// _atoi64( m_V.pbsz ); #GCC
   case eTypeNumberWString     : return std::wcstoll( m_V.pwsz, 0, 10 );
   //case eTypeNumberUtf32String : return _wtoi64( m_V.pwsz );                   // #TODO fix this
   default:                                                                                        assert( false );
   }

   return 0;
}


double variant_view::get_decimal() const
{
   switch( type_number() )
   {
   case eTypeNumberUnknown     : return 0.0;
   case eTypeNumberBit         : return (m_V.int8 & 0x01 ? 1.0 : 0.0);
   case eTypeNumberBool        : return (m_V.b == true ? 1.0 : 0.0);
   case eTypeNumberInt8        : return (double)m_V.int8;
   case eTypeNumberInt16       : return (double)m_V.int16;
   case eTypeNumberInt32       : return (double)m_V.int32;
   case eTypeNumberInt64       : return (double)m_V.int64;
   case eTypeNumberUInt8       : return (double)m_V.uint8;
   case eTypeNumberUInt16      : return (double)m_V.uint16;
   case eTypeNumberUInt32      : return (double)m_V.uint32;
   case eTypeNumberUInt64      : return (double)m_V.uint64;
   case eTypeNumberFloat       : return (double)m_V.f;
   case eTypeNumberDouble      : return m_V.d;
   case eTypeNumberGuid        : 
   case eTypeNumberBinary      :
   case eTypeNumberUtf8String  :
   case eTypeNumberString      :
   case eTypeNumberWString     :
      break;
   }

   return 0.0;
}

std::string variant_view::get_string() const
{
   switch( type_number() )
   {
   case eTypeNumberUnknown     : return std::string();
   case eTypeNumberBool        : return std::to_string( m_V.b );
   case eTypeNumberInt8        : return std::to_string( m_V.int8 );
   case eTypeNumberInt16       : return std::to_string( m_V.int16 );
   case eTypeNumberInt32       : return std::to_string( m_V.int32 );
   case eTypeNumberInt64       : return std::to_string( m_V.int64 );
   case eTypeNumberUInt8       : return std::to_string( m_V.uint8 );
   case eTypeNumberUInt16      : return std::to_string( m_V.uint16 );
   case eTypeNumberUInt32      : return std::to_string( m_V.uint32 );
   case eTypeNumberUInt64      : return std::to_string( m_V.uint64 );
   case eTypeNumberFloat       : return std::to_string(m_V.f);
   case eTypeNumberDouble      : return std::to_string(m_V.d);
      /*
   case eTypeNumberGuid        : 
      {  assert( false );
      gd_std::wstring s;
      s.assign_hex( m_V.pb, m_uSize );
      return s;
      }
   case eTypeNumberBinary      :
      {
         gd_std::wstring s;
         s.assign_hex( m_V.pb, m_uSize );
         return s;
      }
      */
   case eTypeNumberUtf8String  : 
   case eTypeNumberString      : return std::string( m_V.pbsz, m_uSize ); 
   case eTypeNumberWString     : 
   {
      std::string s;
      gd::utf8::convert_utf16_to_uft8( reinterpret_cast<const uint16_t*>(m_V.pwsz), s);
      return std::move(s);
   }
      /*
   case eTypeNumberJson        : return gd_std::wstring( gd_std::string::utf8( m_V.pbsz ), m_uSize ); 
   case eTypeNumberXml         : return gd_std::wstring( gd_std::string::utf8( m_V.pbsz ), m_uSize ); 
      break;
   */
   }

   return std::string();
}

/** ---------------------------------------------------------------------------
 * @brief return value as string_view
 * @return std::string_view value as string_view
*/
std::string_view variant_view::get_string_view() const
{
   if( is_char_string() == true ) return std::string_view(c_str(), length());
   return std::string_view();
}


/*
std::string variant_view::get_utf8string() const
{
   switch( type_number() )
   {
   case eTypeNumberUnknown     : return std::string();
   case eTypeNumberBit         : return std::string( gd_std::string_buffer::c_type( m_V.int8 ).c_str() );
   case eTypeNumberBool        : return std::string( string_buffer::c_type( m_V.b ).c_str() );
   case eTypeNumberInt8        : return std::string( string_buffer::c_type( m_V.int8 ).c_str() );
   case eTypeNumberInt16       : return std::string( string_buffer::c_type( m_V.int16 ).c_str() );
   case eTypeNumberInt32       : return std::string( string_buffer::c_type( m_V.int32 ).c_str() );
   case eTypeNumberInt64       : return std::string( string_buffer::c_type( m_V.int64 ).c_str() );
   case eTypeNumberUInt8       : return std::string( string_buffer::c_type( m_V.uint8 ).c_str() );
   case eTypeNumberUInt16      : return std::string( string_buffer::c_type( m_V.uint16 ).c_str() );
   case eTypeNumberUInt32      : return std::string( string_buffer::c_type( m_V.uint32 ).c_str() );
   case eTypeNumberUInt64      : return std::string( string_buffer::c_type( m_V.uint64 ).c_str() );
   case eTypeNumberCFloat      : return std::string( string_buffer::c_type( m_V.f,  gd_std::string::eConvertNoExponent ).c_str() );
   case eTypeNumberCDouble     : return std::string( string_buffer::c_type( m_V.d,  gd_std::string::eConvertNoExponent ).c_str() );
   case eTypeNumberUtf8String  : return std::string( m_V.pbsz, m_uSize ); 
   case eTypeNumberString      : 
      {
         auto _length = string::utf8_encode( m_V.pbsz, m_V.pbsz + m_uSize, nullptr );
         std::vector<unsigned char> vectorString;

         vectorString.reserve( _length + 1 );
         vectorString.resize( _length );
         string::utf8_encode( m_V.pbsz, m_V.pbsz + m_uSize, vectorString.data() );
         return std::string( vectorString.begin(), vectorString.end() ); 
      }
   case eTypeNumberWString : 
      {
         auto _length = string::utf8_encode( m_V.pwsz, m_V.pwsz + m_uSize, nullptr );
         std::vector<unsigned char> vectorString;

         vectorString.reserve( _length + 1 );
         vectorString.resize( _length );

         //string::utf8_encode( m_V.pwsz, m_V.pwsz + m_uSize, vectorString.data() );
         auto _size = string::utf8_encode_validate( m_V.pwsz, m_V.pwsz + m_uSize, vectorString.data() );
         if( _size < _length ) { vectorString.resize( _size ); }

         return std::string( vectorString.begin(), vectorString.end() ); 
      }
   case eTypeNumberJson        : return std::string( m_V.pbsz, m_uSize ); 
   case eTypeNumberXml         : return std::string( m_V.pbsz, m_uSize );
      break;
   }

   return std::string();

}
*/


std::wstring variant_view::get_wstring() const
{
   switch( type_number() )
   {
   case eTypeNumberUnknown: return std::wstring();
   case eTypeNumberBit: return std::to_wstring(m_V.int8);
   case eTypeNumberBool: return std::to_wstring(m_V.b);
   case eTypeNumberInt8: return std::to_wstring(m_V.int8);
   case eTypeNumberInt16: return std::to_wstring(m_V.int16);
   case eTypeNumberInt32: return std::to_wstring(m_V.int32);
   case eTypeNumberInt64: return std::to_wstring(m_V.int64);
   case eTypeNumberUInt8: return std::to_wstring(m_V.uint8);
   case eTypeNumberUInt16: return std::to_wstring(m_V.uint16);
   case eTypeNumberUInt32: return std::to_wstring(m_V.uint32);
   case eTypeNumberUInt64: return std::to_wstring(m_V.uint64);
   case eTypeNumberFloat: return std::to_wstring(m_V.f); 
   case eTypeNumberDouble: return std::to_wstring(m_V.d); 
   //case eTypeNumberUtf8String: return gd_std::wstring( gd_std::string::utf8( m_V.pbsz ) ).get_wstring();
   //case eTypeNumberString: return gd_std::wstring( m_V.pbsz ).get_wstring();
   case eTypeNumberWString: return std::wstring( m_V.pwsz, m_V.pwsz + m_uSize );
   //case eTypeNumberJson: return gd_std::wstring( gd_std::string::utf8( m_V.pbsz ), m_uSize ).get_wstring();
   //case eTypeNumberXml: return gd_std::wstring( gd_std::string::utf8( m_V.pbsz ), m_uSize ).get_wstring();
   default:                                                                      assert(false);

   }

   return std::wstring();
}

char* variant_view::get_string( char* pbszBuffer ) const
{
   char* pbszStart = pbszBuffer;
   switch( type_number() )
   {
   case eTypeNumberUnknown     : return pbszBuffer;
   case eTypeNumberBool        :
      *pbszBuffer = m_V.b == true ? '1' : '0'; 
      pbszBuffer++;
      break;
   case eTypeNumberInt8        :
      return (char*)gd::utf8::itoa( static_cast<int32_t>(m_V.int8), reinterpret_cast<uint8_t*>(pbszBuffer) );
   case eTypeNumberUInt8       : 
      return (char*)gd::utf8::utoa( static_cast<uint32_t>(m_V.uint8), reinterpret_cast<uint8_t*>(pbszBuffer) );
   case eTypeNumberInt16       : 
      return (char*)gd::utf8::itoa( static_cast<int32_t>(m_V.int16), reinterpret_cast<uint8_t*>(pbszBuffer) );
   case eTypeNumberUInt16      : 
      return (char*)gd::utf8::utoa( static_cast<uint32_t>(m_V.uint16), reinterpret_cast<uint8_t*>(pbszBuffer) );
   case eTypeNumberInt32       : 
      return (char*)gd::utf8::itoa( static_cast<int32_t>(m_V.int32), reinterpret_cast<uint8_t*>(pbszBuffer) );
   case eTypeNumberUInt32      : 
      return (char*)gd::utf8::utoa( static_cast<uint32_t>(m_V.uint32), reinterpret_cast<uint8_t*>(pbszBuffer) );
   case eTypeNumberInt64       : 
      return (char*)gd::utf8::itoa( static_cast<int64_t>(m_V.int64), reinterpret_cast<uint8_t*>(pbszBuffer) );
   case eTypeNumberUInt64      : 
      return (char*)gd::utf8::utoa( static_cast<uint64_t>(m_V.uint64), reinterpret_cast<uint8_t*>(pbszBuffer) );
   case eTypeNumberFloat       : 
      sprintf(pbszBuffer, "%f", static_cast<double>(m_V.f));
      pbszBuffer += strlen( pbszBuffer );
      break;
   case eTypeNumberDouble      : 
      sprintf(pbszBuffer, "%f", m_V.d);
      pbszBuffer += strlen( pbszBuffer );
      break;
      /*
   case eTypeNumberGuid        : 
      {  assert( false );
      gd_std::wstring s;
      s.assign_hex( m_V.pb, m_uSize );
      return s;
      }
   case eTypeNumberBinary      :
      {
         gd_std::wstring s;
         s.assign_hex( m_V.pb, m_uSize );
         return s;
      }
      */
   case eTypeNumberUtf8String  : 
   case eTypeNumberString      : 
      strncpy(pbszBuffer, m_V.pbsz, m_uSize);
      pbszBuffer += m_uSize;
      break;
   case eTypeNumberWString     : 
   {
      std::string s;
      gd::utf8::convert_unicode_to_ascii( m_V.pwsz, pbszBuffer, pbszBuffer + m_uSize);
      pbszBuffer += m_uSize;
   }
      /*
   case eTypeNumberJson        : return gd_std::wstring( gd_std::string::utf8( m_V.pbsz ), m_uSize ); 
   case eTypeNumberXml         : return gd_std::wstring( gd_std::string::utf8( m_V.pbsz ), m_uSize ); 
      break;
   */
   }
   
   *pbszBuffer = '\0';
   return pbszBuffer;
}



bool variant_view::is_true() const
{
   if( (unsigned int)m_uType & variant_type::eGroupInteger ) { if( m_V.uint64 != 0 ) return true; }
   else if( (unsigned int)m_uType & variant_type::eGroupBoolean ) { return m_V.b; }
   if( (unsigned int)m_uType & variant_type::eGroupDecimal ) { if( m_V.d != 0.0 ) return true; }
   else if( (unsigned int)m_uType & variant_type::eGroupString ) 
   {                                                                             assert( m_V.pwsz != nullptr );
      if( type_number() == eTypeNumberWString )
      {
         if(m_V.pwsz[0] != L'\0') return true; 
      }
      else if(m_V.pbsz[0] != '\0') return true; 
   }
   return false;
}


void variant_view::change( variant_type::enumType eType )
{
   if( (unsigned int)eType & variant_type::eGroupInteger )
   {
      if( (m_uType & variant_type::eGroupInteger) & variant_type::eGroupInteger ) return;
      auto _value = get_int64();
      switch( eType )
      {
      case eTypeInt8   : *this = (int8_t)_value; break;
      case eTypeInt16  : *this = (int16_t)_value; break;
      case eTypeInt32  : *this = (int32_t)_value; break;
      case eTypeInt64  : *this = (int64_t)_value; break;
      case eTypeUInt8  : *this = (uint8_t)_value; break;
      case eTypeUInt16 : *this = (uint16_t)_value; break;
      case eTypeUInt32 : *this = (uint32_t)_value; break;
      case eTypeUInt64 : *this = (uint64_t)_value; break;
      }
   }
   else if( (unsigned int)eType & variant_type::eGroupDecimal )
   {
      if( (m_uType & variant_type::eGroupDecimal) & variant_type::eGroupDecimal ) return;
      auto _value = get_decimal();
      switch( eType )
      {
      case eTypeCFloat : *this = (float)_value; break;
      case eTypeCDouble : *this = (double)_value; break;
      }
   }
   else if( (unsigned int)eType & variant_type::eGroupString )
   {                                                                             assert(false);
   /*
      if( (m_uType & variant_type::eGroupString) == variant_type::eGroupString ) return;
      auto _value = get_wstring();
      switch( eType )
      {
      case eTypeUtf8String : { auto _s = _value.get_utf8string(); this->_set_value( utf8( _s.c_str(), _s.length() ) ); } break;
      case eTypeString : { auto _s = _value.get_string(); this->_set_value( _s.c_str(), _s.length() ); } break;
      case eTypeWString : this->_set_value( _value.c_str(), _value.length() ); break;
      }
      */
   }
   else if( (unsigned int)eType & variant_type::eGroupBoolean )
   {
      if( (m_uType & variant_type::eGroupBoolean) & variant_type::eGroupBoolean ) return;
      *this = get_bool();
   }
   else                                                                          assert( false );
}

bool variant_view::compare( const variant_view& v )
{
   if( v.type_number() != type_number() ) return false;

   switch( type_number() )
   {
   case eTypeNumberUnknown: return true;
   case eTypeNumberBool: return m_V.b == v.m_V.b;
   case eTypeNumberInt8: return m_V.int8 == v.m_V.int8;
   case eTypeNumberInt16: return m_V.int16 == v.m_V.int16;
   case eTypeNumberInt32: return m_V.int32 == v.m_V.int32;
   case eTypeNumberInt64: return m_V.int64 == v.m_V.int64;
   case eTypeNumberUInt8: return m_V.uint8 == v.m_V.uint8;
   case eTypeNumberUInt16: return m_V.uint16 == v.m_V.uint16;
   case eTypeNumberUInt32: return m_V.uint32 == v.m_V.uint32;
   case eTypeNumberUInt64: return m_V.uint64 == v.m_V.uint64;
   case eTypeNumberFloat: return m_V.f == v.m_V.f; 
   case eTypeNumberDouble: return m_V.d == v.m_V.d; 
   case eTypeNumberPointer: return m_V.p == v.m_V.p; 
   case eTypeNumberGuid: 
      return (memcmp( m_V.pbsz, v.m_V.pbsz, 16 ) == 0);
   case eTypeNumberString:
   case eTypeNumberUtf8String:
      return (m_uSize == v.m_uSize && memcmp( m_V.pbsz, v.m_V.pbsz, m_uSize ) == 0);
   case eTypeNumberWString: 
      return (m_uSize == v.m_uSize && memcmp( m_V.pbsz, v.m_V.pbsz, m_uSize * sizeof( wchar_t ) ) == 0);
   case eTypeNumberUtf32String:
      return (m_uSize == v.m_uSize && memcmp( m_V.pbsz, v.m_V.pbsz, m_uSize * sizeof( int32_t ) ) == 0);
   case eTypeNumberBinary:
      return (m_uSize == v.m_uSize && memcmp( m_V.pbsz, v.m_V.pbsz, m_uSize ) == 0);
   case eTypeNumberBit: return m_V.int8 == v.m_V.int8;
   default:                                                                      assert(false);

   }

   return false;
}



} /* namespace gd */

