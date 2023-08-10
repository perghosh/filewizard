#include <charconv>
#include <cwchar>


#include "gd_types.h"
#include "gd_utf8.hpp"
#include "gd_variant_view.h"
#include "gd_variant.h"


#if defined( __clang__ )
   #pragma clang diagnostic ignored "-Wdeprecated-declarations"
   #pragma clang diagnostic ignored "-Wswitch"
   #pragma clang diagnostic ignored "-Wformat"
   #pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#elif defined( __GNUC__ )
   #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
   #pragma GCC diagnostic ignored "-Wswitch"
   #pragma GCC diagnostic ignored "-Wformat"
   #pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#elif defined( _MSC_VER )
   #pragma warning( disable : 4996 26812 )
#endif




#ifndef _GD_BEGIN
namespace gd {
#else
_GD_BEGIN
#endif

using namespace variant_type;


bool variant::get_bool() const
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
   default:                                                                                        assert( false );
   }

   return false;
}


int variant::get_int() const
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
   default:                                                                                        assert( false );
   }

   return 0;
}

unsigned int variant::get_uint() const
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
   default:                                                                                        assert( false );
   }

   return 0;
}


int64_t variant::get_int64() const
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
   case eTypeNumberString      : return atoll( m_V.pbsz );
   case eTypeNumberWString     : return std::wcstoll( m_V.pwsz, 0, 10 );
   default:                                                                                        assert( false );
   }

   return 0;
}

uint64_t variant::get_uint64() const
{
   switch( type_number() )
   {
   case eTypeNumberUnknown     : return 0;
   case eTypeNumberBit         : return (m_V.int8 & 0x01 ? 1 : 0);
   case eTypeNumberBool        : return (m_V.b == true ? 1 : 0);
   case eTypeNumberInt8        : return (uint64_t)m_V.int8;
   case eTypeNumberInt16       : return (uint64_t)m_V.int16;
   case eTypeNumberInt32       : return (uint64_t)m_V.int32;
   case eTypeNumberInt64       : return (uint64_t)m_V.int64;
   case eTypeNumberUInt8       : return (uint64_t)m_V.uint8;
   case eTypeNumberUInt16      : return (uint64_t)m_V.uint16;
   case eTypeNumberUInt32      : return (uint64_t)m_V.uint32;
   case eTypeNumberUInt64      : return (uint64_t)m_V.uint64;
   case eTypeNumberFloat       : return (uint64_t)m_V.f;
   case eTypeNumberDouble      : return (uint64_t)m_V.d;
   case eTypeNumberGuid        : return 0;
   case eTypeNumberBinary      : return 0;
   case eTypeNumberUtf8String  : 
   case eTypeNumberString      :
   {
      uint64_t v_;      
      std::from_chars( m_V.pbsz, m_V.pbsz + length(), v_ );
      return v_;
   }
   case eTypeNumberWString     : return (uint64_t)std::wcstoll( m_V.pwsz, 0, 10 );
   default:                                                                                        assert( false );
   }

   return 0;
}


double variant::get_decimal() const
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
   case eTypeNumberGuid        : return 0.0;
   case eTypeNumberBinary      : return 0.0;
   case eTypeNumberUtf8String  :
   case eTypeNumberString: {
      double dValue = std::strtod( c_str(), nullptr );
      return dValue;
      }
      break;
   case eTypeNumberWString     :
      break;
   }

   return 0.0;
}

/** ---------------------------------------------------------------------------
 * @brief return variant value as string
 * @return converted string value
*/
std::string variant::get_string() const
{
   switch( type_number() )
   {
   case eTypeNumberUnknown     : return std::string();
   //case eTypeNumberBit         : return std::string( gd_std::string_buffer::c_type( m_V.int8 ).c_str() );
   case eTypeNumberBool        : return std::to_string( m_V.b );
   case eTypeNumberInt8        : return std::to_string( m_V.int8 );
   case eTypeNumberInt16       : return std::to_string( m_V.int16 );
   case eTypeNumberInt32       : return std::to_string( m_V.int32 );
   case eTypeNumberInt64       : return std::to_string( m_V.int64 );
   case eTypeNumberUInt8       : return std::to_string( m_V.uint8 );
   case eTypeNumberUInt16      : return std::to_string( m_V.uint16 );
   case eTypeNumberUInt32      : return std::to_string( m_V.uint32 );
   case eTypeNumberUInt64      : return std::to_string( m_V.uint64 );
   case eTypeNumberFloat       : return std::to_string( m_V.f );
   case eTypeNumberDouble      : return std::to_string( m_V.d );
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
   case eTypeNumberString: {                                                                       assert( strlen( m_V.pbsz ) == m_uSize );
      return std::string( m_V.pbsz, m_uSize );
   }
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
 * @param tag dispatcher
 * @return std::string_view value as string_view
*/
std::string_view variant::get_string(variant_type::tag_no_allocate) const
{                                                                                                  assert( is_char_string() == true );
   if( is_char_string() == true ) return std::string_view(c_str(), length());
   return std::string_view();
}

std::wstring variant::get_wstring() const
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
   default:                                                                                        assert(false);

   }

   return std::wstring();
}

/// Return value in variant as variant_view
variant_view variant::as_variant_view() const
{
   variant_view variantviewReturn;
   variant_view::copy_s( *this, variantviewReturn );
   return variantviewReturn;
}


/** ---------------------------------------------------------------------------
 * @brief checks if we have a "true" value
 * A true value is a valid type with content, If variant is a number then 0 is
 * false, all other numbers are true. If string then empty string is false, string
 * with characters is true. If binary then empty binary is false, binary with content
 * is true.
 * 
*Sample code to show how `is_true` behaves.*
@code
gd::variant variantTest;

variantTest = 0;     assert( variantTest.is_true() == false );
variantTest = 1;     assert( variantTest.is_true() == true );
variantTest = -123;  assert( variantTest.is_true() == true );
variantTest = 321;   assert( variantTest.is_true() == true );
variantTest = "";    assert( variantTest.is_true() == false );
variantTest = "7";   assert( variantTest.is_true() == true );
variantTest = (void*)nullptr;   assert( variantTest.is_true() == false );
@endcode

 * @return true if variant has content, false if not
*/
bool variant::is_true() const
{
   if( (unsigned int)m_uType & variant_type::eGroupInteger ) 
   { 
      if( m_uType & variant_type::eGroupSize32 ) return m_V.int32 != 0;
      else if(m_uType & variant_type::eGroupSize64 ) return m_V.uint64 != 0; 
      else if(m_uType & variant_type::eGroupSize16 ) return m_V.uint16 != 0; 
      return m_V.uint8 != 0;
   }
   else if( m_uType & variant_type::eGroupBoolean ) { return m_V.b; }
   else if( m_uType & variant_type::eGroupDecimal ) 
   { 
      if( m_uType & variant_type::eGroupSize64 ) return m_V.d != 0;
      return m_V.f != 0.0;
   }
   else if( m_uType & variant_type::eGroupString ) 
   {                                                                                               assert( m_V.pwsz != nullptr );
      if( type_number() == eTypeNumberWString ) 
      {
         if(m_V.pwsz[0] != L'\0') { return true; }
      }
      else if(m_V.pbsz[0] != '\0') { return true; }
   }
   else if( ( unsigned int )m_uType & variant_type::eGroupBinary )
   {
      return m_uSize > 0;                                                      // has binary content ?
   }
   else if( m_uType == eTypePointer )
   {
      return m_V.p != nullptr;
   }
   return false;
}


/** ---------------------------------------------------------------------------
 * @brief change type and this may also change internal data
 * @param eType type to change to
*/
void variant::convert( variant_type::enumType eType )
{
   unsigned uTypeNumber = type_number_s( eType );
   if( type_number() == uTypeNumber ) return;

   variant variantConvertTo;
   auto bOk = convert_to_s( this, &variantConvertTo, eType );                                      assert( bOk );

   *this = std::move( variantConvertTo );
}

/** ---------------------------------------------------------------------------
 * @brief change internal type in variant

*Sample code to convert big integer number to string*
@code
std::string stringType;
gd::variant variantTest;

variantTest = int64_t(1'000'000'000'000);
variantTest.convert( "string" );
stringType = gd::types::type_name_g( variantTest.type() ); std::cout << stringType << "\n"; // "string"
@endcode

*Sample code to convert from string to utf8 and back to string*
@code
   std::string stringType;
   gd::variant variantTest;

   variantTest = "���";
   variantTest.convert( "utf8" );
   stringType = gd::types::type_name_g( variantTest.type() ); std::cout << stringType << "\n"; // "utf8"

   std::cout << variantTest.c_str() << "\n";                                   // writes 6 strange characters
#ifdef _WIN32
   unsigned uCodePage = ::GetConsoleCP();                                      // get current code page
   ::SetConsoleOutputCP(CP_UTF8);                                              // set console to utf8
   std::cout << variantTest.c_str() << "\n";                                   // looks better "���"
   ::SetConsoleOutputCP(uCodePage);                                            // restore code page
#else
   std::locale::global(std::locale(""));
   std::wcout.imbue(std::locale());
   std::wstring_convert<std::codecvt_utf8<wchar_t>> wstringconvert;
   std::wstring wstringConvert = wstringconvert.from_bytes(variantTest.c_str());
   std::wcout << wstringConvert << std::endl;
#endif
   variantTest.convert( "string" );                                            // revert back to ascii string
   std::cout << variantTest.c_str() << "\n";                                   // prints "���"
@endcode

 * @param stringType type to change to
*/
void variant::convert( const std::string_view& stringType )
{
   auto eType = gd::types::type_g( stringType );
   convert( (variant_type::enumType)eType );
}

/** ---------------------------------------------------------------------------
 * @brief compare variant value
 * @param v value that is compared if they are equal
 * @return true if equal, false if not
*/
bool variant::compare( const variant& v ) const
{
#ifdef _DEBUG
   auto t1_d = v.type_number();
   auto t2_d = type_number();
#endif // DEBUG

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
   default:                                                                    assert(false);

   }

   return false;
}

/** ---------------------------------------------------------------------------
 * @brief compares if this is less than passed value
 * @param v value to check if this is less than
 * @return true if this is less than passed value
*/
bool variant::less( const variant& v ) const
{
   if( v.type_number() != type_number() ) return false;

   switch( type_number() )
   {
   case eTypeNumberUnknown: return true;
   case eTypeNumberBool: return m_V.b < v.m_V.b;
   case eTypeNumberInt8: return m_V.int8 < v.m_V.int8;
   case eTypeNumberInt16: return m_V.int16 < v.m_V.int16;
   case eTypeNumberInt32: return m_V.int32 < v.m_V.int32;
   case eTypeNumberInt64: return m_V.int64 < v.m_V.int64;
   case eTypeNumberUInt8: return m_V.uint8 < v.m_V.uint8;
   case eTypeNumberUInt16: return m_V.uint16 < v.m_V.uint16;
   case eTypeNumberUInt32: return m_V.uint32 < v.m_V.uint32;
   case eTypeNumberUInt64: return m_V.uint64 < v.m_V.uint64;
   case eTypeNumberFloat: return m_V.f < v.m_V.f; 
   case eTypeNumberDouble: return m_V.d < v.m_V.d; 
   case eTypeNumberPointer: return m_V.p < v.m_V.p; 
   case eTypeNumberGuid: 
      return (memcmp( m_V.pbsz, v.m_V.pbsz, 16 ) < 0);
   case eTypeNumberString:
   case eTypeNumberUtf8String:
      return memcmp( m_V.pbsz, v.m_V.pbsz, m_uSize ) < 0;
   case eTypeNumberWString: 
      return memcmp( m_V.pbsz, v.m_V.pbsz, m_uSize * sizeof( wchar_t ) ) < 0;
   case eTypeNumberUtf32String:
      return memcmp( m_V.pbsz, v.m_V.pbsz, m_uSize * sizeof( int32_t ) ) < 0;
   case eTypeNumberBinary:
      return memcmp( m_V.pbsz, v.m_V.pbsz, m_uSize ) < 0;
   case eTypeNumberBit: return m_V.int8 < v.m_V.int8;
   default:                                                                      assert(false);

   }

   return false;
}


/** ---------------------------------------------------------------------------
 * @brief Return index (iterator) for variant value within vector of variants
 * 
 * @param vectorFindIn vector of variants to search for "find" variant
 * @param variantFind variant value to match within vector of variants
 * @return index (iterator) to variant in vector if found otherwise `end` iterator
*/
std::vector<variant>::const_iterator variant::find_s( const std::vector<variant>& vectorFindIn, const variant& variantFind )
{
   for( auto it = std::begin( vectorFindIn ), itEnd = std::end( vectorFindIn ); it != itEnd; it++ )
   {
      if( it->compare( variantFind ) == true ) return it;
   }

   return std::end( vectorFindIn );
}

std::vector<std::pair<variant, variant>>::const_iterator variant::find_s( const std::vector< std::pair<variant, variant> >& vectorFindIn, std::pair<const variant&, const variant&> pairFind )
{
   for( auto it = std::begin( vectorFindIn ), itEnd = std::end( vectorFindIn ); it != itEnd; it++ )
   {
      if( it->first.compare( pairFind.first ) == true && it->second.compare( pairFind.second ) == true ) return it;
   }

   return std::end( vectorFindIn );
}

bool variant::convert_to_s( const gd::variant* pvariantFrom, gd::variant* pvariantTo, unsigned uType )
{                                                                                                  assert( pvariantFrom != nullptr ); assert( pvariantTo != nullptr );
   bool bOk = true;
   unsigned uTypeNumber = type_number_s( uType );
   unsigned uToTypeNumber = pvariantFrom->type_number();
   switch( uToTypeNumber )
   {
   case eTypeNumberInt8 :
      switch( uTypeNumber )
      {
      case eTypeNumberUInt8:
         pvariantTo->assign( (uint8_t)(int8_t)*pvariantFrom );
         break;
      case eTypeNumberInt16:
         pvariantTo->assign( (int16_t)(int8_t)*pvariantFrom );
         break;
      case eTypeNumberUInt16:
         pvariantTo->assign( (uint16_t)(int8_t)*pvariantFrom );
         break;
      case eTypeNumberInt32:
         pvariantTo->assign( (int32_t)(int8_t)*pvariantFrom );
         break;
      case eTypeNumberUInt32:
         pvariantTo->assign( (uint32_t)(int8_t)*pvariantFrom );
         break;
      case eTypeNumberInt64:
         pvariantTo->assign( (int64_t)(int8_t)*pvariantFrom );
         break;
      case eTypeNumberUInt64:
         pvariantTo->assign( (uint64_t)(int8_t)*pvariantFrom );
         break;
      case eTypeNumberDouble:
         pvariantTo->assign( (double)(int8_t)*pvariantFrom );
         break;
      case eTypeNumberString :
      case eTypeNumberUtf8String :
         pvariantTo->assign( std::to_string( (int8_t)*pvariantFrom ) );
         break;
      case eTypeNumberWString :
         pvariantTo->assign( std::to_wstring( (int8_t)*pvariantFrom ) );
         break;
      default:
         bOk = false;
      }
      break;

   case eTypeNumberUInt8 :
      switch( uTypeNumber )
      {
      case eTypeNumberInt8:
         pvariantTo->assign( (int8_t)(uint8_t)*pvariantFrom );
         break;
      case eTypeNumberInt16:
         pvariantTo->assign( (int16_t)(uint8_t)*pvariantFrom );
         break;
      case eTypeNumberUInt16:
         pvariantTo->assign( (uint16_t)(uint8_t)*pvariantFrom );
         break;
      case eTypeNumberInt32:
         pvariantTo->assign( (int32_t)(uint8_t)*pvariantFrom );
         break;
      case eTypeNumberUInt32:
         pvariantTo->assign( (uint32_t)(uint8_t)*pvariantFrom );
         break;
      case eTypeNumberInt64:
         pvariantTo->assign( (int64_t)(uint8_t)*pvariantFrom );
         break;
      case eTypeNumberUInt64:
         pvariantTo->assign( (uint64_t)(uint8_t)*pvariantFrom );
         break;
      case eTypeNumberDouble:
         pvariantTo->assign( (double)(uint8_t)*pvariantFrom );
         break;
      case eTypeNumberString :
      case eTypeNumberUtf8String :
         pvariantTo->assign( std::to_string( (uint8_t)*pvariantFrom ) );
         break;
      case eTypeNumberWString :
         pvariantTo->assign( std::to_wstring( (uint8_t)*pvariantFrom ) );
         break;
      default:
         bOk = false;
      }
      break;



   case eTypeNumberInt16 :
      switch( uTypeNumber )
      {
      case eTypeNumberInt8:
         pvariantTo->assign( (int8_t)(int16_t)*pvariantFrom );
         break;
      case eTypeNumberUInt8:
         pvariantTo->assign( (uint8_t)(int16_t)*pvariantFrom );
         break;
      case eTypeNumberUInt16:
         pvariantTo->assign( (uint16_t)(int16_t)*pvariantFrom );
         break;
      case eTypeNumberInt32:
         pvariantTo->assign( (int32_t)(int16_t)*pvariantFrom );
         break;
      case eTypeNumberUInt32:
         pvariantTo->assign( (uint32_t)(int16_t)*pvariantFrom );
         break;
      case eTypeNumberInt64:
         pvariantTo->assign( (int64_t)(int16_t)*pvariantFrom );
         break;
      case eTypeNumberUInt64:
         pvariantTo->assign( (uint64_t)(int16_t)*pvariantFrom );
         break;
      case eTypeNumberDouble:
         pvariantTo->assign( (double)(int16_t)*pvariantFrom );
         break;
      case eTypeNumberString :
      case eTypeNumberUtf8String :
         pvariantTo->assign( std::to_string( (int16_t)*pvariantFrom ) );
         break;
      case eTypeNumberWString :
         pvariantTo->assign( std::to_wstring( (int16_t)*pvariantFrom ) );
         break;
      default:
         bOk = false;
      }
      break;

   case eTypeNumberUInt16 :
      switch( uTypeNumber )
      {
      case eTypeNumberInt8:
         pvariantTo->assign( (int8_t)(uint16_t)*pvariantFrom );
         break;
      case eTypeNumberUInt8:
         pvariantTo->assign( (uint8_t)(uint16_t)*pvariantFrom );
         break;
      case eTypeNumberInt16:
         pvariantTo->assign( (int16_t)(uint16_t)*pvariantFrom );
         break;
      case eTypeNumberInt32:
         pvariantTo->assign( (int32_t)(uint16_t)*pvariantFrom );
         break;
      case eTypeNumberUInt32:
         pvariantTo->assign( (uint32_t)(uint16_t)*pvariantFrom );
         break;
      case eTypeNumberInt64:
         pvariantTo->assign( (int64_t)(uint16_t)*pvariantFrom );
         break;
      case eTypeNumberUInt64:
         pvariantTo->assign( (uint64_t)(uint16_t)*pvariantFrom );
         break;
      case eTypeNumberDouble:
         pvariantTo->assign( (double)(uint16_t)*pvariantFrom );
         break;
      case eTypeNumberString :
      case eTypeNumberUtf8String :
         pvariantTo->assign( std::to_string( (uint16_t)*pvariantFrom ) );
         break;
      case eTypeNumberWString :
         pvariantTo->assign( std::to_wstring( (uint16_t)*pvariantFrom ) );
         break;
      default:
         bOk = false;
      }
      break;


   case eTypeNumberInt32 :
      switch( uTypeNumber )
      {
      case eTypeNumberInt8:
         pvariantTo->assign( (int8_t)(int32_t)*pvariantFrom );
         break;
      case eTypeNumberUInt8:
         pvariantTo->assign( (uint8_t)(int32_t)*pvariantFrom );
         break;
      case eTypeNumberInt16:
         pvariantTo->assign( (int16_t)(int32_t)*pvariantFrom );
         break;
      case eTypeNumberUInt16:
         pvariantTo->assign( (uint16_t)(int32_t)*pvariantFrom );
         break;
      case eTypeNumberUInt32:
         pvariantTo->assign( (uint32_t)(int32_t)*pvariantFrom );
         break;
      case eTypeNumberInt64:
         pvariantTo->assign( (int64_t)(int32_t)*pvariantFrom );
         break;
      case eTypeNumberUInt64:
         pvariantTo->assign( (uint64_t)(int32_t)*pvariantFrom );
         break;
      case eTypeNumberDouble:
         pvariantTo->assign( (double)(int32_t)*pvariantFrom );
         break;
      case eTypeNumberString :
      case eTypeNumberUtf8String :
         pvariantTo->assign( std::to_string( (int32_t)*pvariantFrom ) );
         break;
      case eTypeNumberWString :
         pvariantTo->assign( std::to_wstring( (int32_t)*pvariantFrom ) );
         break;
      default:
         bOk = false;
      }
      break;

   case eTypeNumberUInt32 :
      switch( uTypeNumber )
      {
      case eTypeNumberInt8:
         pvariantTo->assign( (int8_t)(uint32_t)*pvariantFrom );
         break;
      case eTypeNumberUInt8:
         pvariantTo->assign( (uint8_t)(uint32_t)*pvariantFrom );
         break;
      case eTypeNumberInt16:
         pvariantTo->assign( (int16_t)(uint32_t)*pvariantFrom );
         break;
      case eTypeNumberUInt16:
         pvariantTo->assign( (uint16_t)(uint32_t)*pvariantFrom );
         break;
      case eTypeNumberInt32:
         pvariantTo->assign( (int32_t)(uint32_t)*pvariantFrom );
         break;
      case eTypeNumberInt64:
         pvariantTo->assign( (int64_t)(uint32_t)*pvariantFrom );
         break;
      case eTypeNumberUInt64:
         pvariantTo->assign( (uint64_t)(uint32_t)*pvariantFrom );
         break;
      case eTypeNumberDouble:
         pvariantTo->assign( (double)(uint32_t)*pvariantFrom );
         break;
      case eTypeNumberString :
      case eTypeNumberUtf8String :
         pvariantTo->assign( std::to_string( (uint32_t)*pvariantFrom ) );
         break;
      case eTypeNumberWString :
         pvariantTo->assign( std::to_wstring( (uint32_t)*pvariantFrom ) );
         break;
      default:
         bOk = false;
      }
      break;

   case eTypeNumberInt64 :
      switch( uTypeNumber )
      {
      case eTypeNumberInt8:
         pvariantTo->assign( (int8_t)(int64_t)*pvariantFrom );
         break;
      case eTypeNumberUInt8:
         pvariantTo->assign( (uint8_t)(int64_t)*pvariantFrom );
         break;
      case eTypeNumberInt16:
         pvariantTo->assign( (int16_t)(int64_t)*pvariantFrom );
         break;
      case eTypeNumberUInt16:
         pvariantTo->assign( (uint16_t)(int64_t)*pvariantFrom );
         break;
      case eTypeNumberInt32:
         pvariantTo->assign( (int32_t)(int64_t)*pvariantFrom );
         break;
      case eTypeNumberUInt32:
         pvariantTo->assign( (uint32_t)(int64_t)*pvariantFrom );
         break;
      case eTypeNumberInt64:
         pvariantTo->assign( (int64_t)(int64_t)*pvariantFrom );
         break;
      case eTypeNumberUInt64:
         pvariantTo->assign( (uint64_t)(int64_t)*pvariantFrom );
         break;
      case eTypeNumberDouble:
         pvariantTo->assign( (double)(int64_t)*pvariantFrom );
         break;
      case eTypeNumberString :
      case eTypeNumberUtf8String :
         pvariantTo->assign( std::to_string( (int64_t)*pvariantFrom ) );
         break;
      case eTypeNumberWString :
         pvariantTo->assign( std::to_wstring( (int64_t)*pvariantFrom ) );
         break;
      default:
         bOk = false;
      }
      break;

   case eTypeNumberUInt64 :
      switch( uTypeNumber )
      {
      case eTypeNumberUnknown:
         pvariantTo->clear();
         break;        
      case eTypeNumberInt8:
         pvariantTo->assign( (int8_t)(uint64_t)*pvariantFrom );
         break;
      case eTypeNumberUInt8:
         pvariantTo->assign( (uint8_t)(uint64_t)*pvariantFrom );
         break;
      case eTypeNumberInt16:
         pvariantTo->assign( (int16_t)(uint64_t)*pvariantFrom );
         break;
      case eTypeNumberUInt16:
         pvariantTo->assign( (uint16_t)(uint64_t)*pvariantFrom );
         break;
      case eTypeNumberInt32:
         pvariantTo->assign( (int32_t)(uint64_t)*pvariantFrom );
         break;
      case eTypeNumberUInt32:
         pvariantTo->assign( (uint32_t)(uint64_t)*pvariantFrom );
         break;
      case eTypeNumberInt64:
         pvariantTo->assign( (int64_t)(uint64_t)*pvariantFrom );
         break;
      case eTypeNumberDouble:
         pvariantTo->assign( (double)(uint64_t)*pvariantFrom );
         break;
      case eTypeNumberString :
      case eTypeNumberUtf8String :
         pvariantTo->assign( std::to_string( (uint64_t)*pvariantFrom ) );
         break;
      case eTypeNumberWString :
         pvariantTo->assign( std::to_wstring( (uint64_t)*pvariantFrom ) );
         break;
      default:
         bOk = false;
      }
      break;


   case eTypeNumberDouble :
      switch( uTypeNumber )
      {
      case eTypeNumberUnknown:
         pvariantTo->clear();
         break;        
      case eTypeNumberInt8:
         pvariantTo->assign( (int8_t)(double)*pvariantFrom );
         break;
      case eTypeNumberUInt8:
         pvariantTo->assign( (uint8_t)(double)*pvariantFrom );
         break;
      case eTypeNumberInt16:
         pvariantTo->assign( (int16_t)(double)*pvariantFrom );
         break;
      case eTypeNumberUInt16:
         pvariantTo->assign( (uint16_t)(double)*pvariantFrom );
         break;
      case eTypeNumberInt32:
         pvariantTo->assign( (int32_t)(double)*pvariantFrom );
         break;
      case eTypeNumberUInt32:
         pvariantTo->assign( (uint32_t)(double)*pvariantFrom );
         break;
      case eTypeNumberInt64:
         pvariantTo->assign( (int64_t)(double)*pvariantFrom );
         break;
      case eTypeNumberUInt64:
         pvariantTo->assign( (uint64_t)(double)*pvariantFrom );
         break;
      case eTypeNumberString :
      case eTypeNumberUtf8String :
         pvariantTo->assign( std::to_string( (double)*pvariantFrom ) );
         break;
      case eTypeNumberWString :
         pvariantTo->assign( std::to_wstring( (double)*pvariantFrom ) );
         break;
      default:
         bOk = false;
      }
      break;

   case eTypeNumberString :
   case eTypeNumberUtf8String :
      switch( uTypeNumber )
      {
      case eTypeNumberUnknown:
         pvariantTo->clear();
         break;        
      case eTypeNumberInt8: {
         int8_t v_;
         std::from_chars( pvariantFrom->c_str(), pvariantFrom->c_str() + pvariantFrom->length(), v_ );
         pvariantTo->assign( v_ );
         }
         break;
      case eTypeNumberUInt8: {
         uint8_t v_;
         std::from_chars( pvariantFrom->c_str(), pvariantFrom->c_str() + pvariantFrom->length(), v_ );
         pvariantTo->assign( v_ );
         }
         break;
      case eTypeNumberInt16: {
         int16_t v_;
         std::from_chars( pvariantFrom->c_str(), pvariantFrom->c_str() + pvariantFrom->length(), v_ );
         pvariantTo->assign( v_ );
         }
         break;
      case eTypeNumberUInt16: {
         uint16_t v_;
         std::from_chars( pvariantFrom->c_str(), pvariantFrom->c_str() + pvariantFrom->length(), v_ );
         pvariantTo->assign( v_ );
         }
         break;
      case eTypeNumberInt32: {
         int32_t v_;
         std::from_chars( pvariantFrom->c_str(), pvariantFrom->c_str() + pvariantFrom->length(), v_ );
         pvariantTo->assign( v_ );
         }
         break;
      case eTypeNumberUInt32: {
         uint32_t v_;
         std::from_chars( pvariantFrom->c_str(), pvariantFrom->c_str() + pvariantFrom->length(), v_ );
         pvariantTo->assign( v_ );
         }
         break;
      case eTypeNumberInt64: {
         int64_t v_;
         std::from_chars( pvariantFrom->c_str(), pvariantFrom->c_str() + pvariantFrom->length(), v_ );
         pvariantTo->assign( v_ );
         }
         break;
      case eTypeNumberUInt64: {
         uint64_t v_;
         std::from_chars( pvariantFrom->c_str(), pvariantFrom->c_str() + pvariantFrom->length(), v_ );
         pvariantTo->assign( v_ );
         }
         break;
      case eTypeNumberFloat: {
         float v_ = std::strtof( pvariantFrom->c_str(), nullptr );
         pvariantTo->assign( v_ );
         }
         break;
      case eTypeNumberDouble: {
         double v_ = std::strtod( pvariantFrom->c_str(), nullptr );
         pvariantTo->assign( v_ );
         }
         break;
      case eTypeNumberUtf8String: {                                                                assert( pvariantFrom->type_number() == eTypeNumberString );
         pvariantTo->clear();
         uint32_t uSize = pvariantFrom->length();
         if( uSize > 0 )
         {
            uSize = gd::utf8::size( pvariantFrom->c_str(), pvariantFrom->length() );
         }
         uint8_t* puUtf8Text = (uint8_t*)pvariantTo->allocate( uSize + 1 );
         // convert ascii text to utf8 text
         gd::utf8::convert_ascii( pvariantFrom->c_str(), pvariantFrom->c_str() + pvariantFrom->length(), puUtf8Text, puUtf8Text + (uSize + 1) );
         puUtf8Text[uSize] = '\0';                                             // set end
         pvariantTo->m_V.p = puUtf8Text;
         pvariantTo->m_uSize = uSize;
         pvariantTo->m_uType = (variant_type::eTypeUtf8String|variant_type::eFlagAllocate);// set internal value type
         }
         break;
      case eTypeNumberString: {                                                                    assert( pvariantFrom->type_number() == eTypeNumberUtf8String );
         pvariantTo->clear();
         uint32_t uCount = (uint32_t)pvariantFrom->length();
         if( uCount > 0 )
         {
            auto result_ = gd::utf8::count( pvariantFrom->c_str_uft8(), pvariantFrom->c_str_uft8() + uCount );// count number of characters in utf8 buffer
         }
         char* pbszText = (char*)pvariantTo->allocate( uCount + 1 );           // allocate buffer
         // convert utf8 text to ascii text
         gd::utf8::convert_utf8_to_ascii( pvariantFrom->c_str_uft8(),pvariantFrom->c_str_uft8() + pvariantFrom->length(), pbszText, pbszText + (uCount + 1) );// copy ascii to buffer as utf8
         pbszText[uCount] = '\0';                                              // set end
         pvariantTo->m_V.p = pbszText;
         pvariantTo->m_uSize = uCount;                                         // set memory size needed to store ascii text
         pvariantTo->m_uType = (variant_type::eTypeString|variant_type::eFlagAllocate);// set internal value type
         }
         break;
      default:
         bOk = false;
      }
      break;

   default:
      bOk = false;
   }

   return bOk;
}

inline unsigned variant::compute_digit_count_s( uint32_t uNumber )
{
   if( uNumber < 10 )            return 1;
   if( uNumber < 100 )           return 2;
   if( uNumber < 1000 )          return 3;
   if( uNumber < 10000 )         return 4;
   if( uNumber < 100000 )        return 5;
   if( uNumber < 1000000 )       return 6;
   if( uNumber < 10000000 )      return 7;
   if( uNumber < 100000000 )     return 8;
   if( uNumber < 1000000000 )    return 9;

   return 10;
}

unsigned variant::compute_digit_count_s( int32_t iNumber )
{
   unsigned uLength = 0;
   if( iNumber < 0 ) { uLength++; iNumber = -iNumber; }
   uLength += compute_digit_count_s( (uint32_t)iNumber );
   return uLength;
}

unsigned variant::compute_digit_count_s( uint64_t uNumber )
{
   if( uNumber < 10000000000 ) return compute_digit_count_s( (uint32_t)uNumber );
   return 10 + compute_digit_count_s( (uint32_t)(uNumber >> 32) );
}

unsigned variant::compute_digit_count_s( int64_t iNumber )
{
   unsigned uLength = 0;
   if( iNumber < 0 ) { uLength++; iNumber = -iNumber; }
   uLength += compute_digit_count_s( (uint64_t)iNumber );
   return uLength;
}


unsigned variant::compute_digit_count_s( const variant* pvariantValue )
{
   if( pvariantValue->is_integer() == true )
   {
      if( pvariantValue->is_32or64() == true )
      {
         if( pvariantValue->is_signed() == true )
         {
            if( pvariantValue->is_32() == true ) { return compute_digit_count_s( pvariantValue->cast_as_int32() ); }
            else                                 { return compute_digit_count_s( pvariantValue->cast_as_int64() ); }
         }
         else
         {
            if( pvariantValue->is_32() == true ) { return compute_digit_count_s( pvariantValue->cast_as_uint32() ); }
            else                                 { return compute_digit_count_s( pvariantValue->cast_as_uint64() ); }
         }
      }
      else
      {
         int64_t iValue = pvariantValue->as_int64();
         return compute_digit_count_s( iValue );

      }
   }
   else { assert( false ); }
   return 0;
}


unsigned variant::compute_ascii_size_s( const variant* pvariantValue )
{
   switch( pvariantValue->type_number() )
   {
   case eTypeNumberUnknown: return 0; break;
   case eTypeNumberBool: return 1; break;

   case eTypeNumberInt8: 
   case eTypeNumberInt16: 
   case eTypeNumberInt32: 
   case eTypeNumberInt64: 
   case eTypeNumberUInt8: 
   case eTypeNumberUInt16: 
   case eTypeNumberUInt32: 
   case eTypeNumberUInt64: 
   case eTypeNumberFloat: 
   {
       return compute_digit_count_s( pvariantValue );
   }
   //case eTypeNumberGuid: return 16;
   // case eTypeNumberDouble: return std::to_wstring(m_V.d); 

   case eTypeNumberString: return pvariantValue->length(); break;
   case eTypeNumberUtf8String: return pvariantValue->length(); break;

   //case eTypeNumberWString: return m_uSize * 2;
   //case eTypeNumberJson: return gd_std::wstring( gd_std::string::utf8( m_V.pbsz ), m_uSize ).get_wstring();
   //case eTypeNumberXml: return gd_std::wstring( gd_std::string::utf8( m_V.pbsz ), m_uSize ).get_wstring();
   case eTypeNumberBit: return 1;
   default:                                                                      assert(false);

   }

   return 0;
}



} /* namespace gd */

