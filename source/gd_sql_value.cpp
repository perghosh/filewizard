#include <stdlib.h>

#include "gd_utf8.hpp"

#include "gd_sql_value.h"

#if defined(_MSC_VER)
#  pragma warning( disable : 4996 6001 6255 )
#else
#endif


_GD_SQL_QUERY_BEGIN

/** ---------------------------------------------------------------------------
 * @brief Append ascii text to string as utf8
 * @param pbszAscii ascii text appended to string as utf8
 * @param stringSql string that gets ascii text formated as utf8
*/
void append_ascii( const uint8_t* pbszAscii, std::string& stringSql  )
{
   uint8_t pbUtf8[2];

   while( *pbszAscii )
   {
      if( *pbszAscii < 0x80 )
      {
         if( *pbszAscii != '\'' ) stringSql += (char)*pbszAscii;
         else                    stringSql += std::string_view{"''"};
      }
      else
      {
          gd::utf8::convert( *pbszAscii, pbUtf8 );
          stringSql.append( (const char*)pbUtf8, 2 );
      }
      pbszAscii++;
   }
}

/** ---------------------------------------------------------------------------
 * @brief Append utf8 (that is the default) text to string object
 * @param pbszUft8 utf8 formated text appended to text
 * @param stringSql utf8 text is appended to string
*/
void append_utf8( const uint8_t* pbszUft8, std::string& stringSql  )
{
   while( *pbszUft8 )
   {
      if( *pbszUft8 != '\'' ) stringSql += (char)*pbszUft8;
      else                    stringSql += std::string_view{"''"};

      pbszUft8++;
   }
}

static const uint8_t binary_pszHEX_s[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
                                  

//inline void append_binary( uint8_t* puPosition, uint8_t* puBytes, unsigned uLength )
inline void append_binary( const uint8_t* puBinary, unsigned uLength, std::string& stringSql )
{
   const unsigned STACK_SIZE = 1024;

   uint8_t* puBuffer; // temporary storage for hexadecimal binary text
   const uint8_t* puBytes = puBinary + uLength;// set to end
   const uint8_t* puBytesStop = puBinary; 

   unsigned uSize = uLength * 2;                                               // two characters for each byte !!!

   if( uLength > STACK_SIZE ) puBuffer = new uint8_t[uSize];
   else                       puBuffer = (uint8_t*)alloca( uSize );

   uint8_t* puPosition = puBuffer + uSize;                                     // set to buffer end

   // optimize, align for 4 byte sections
   for( unsigned int u = uLength % 4; u > 0; u--  )
   {
      puBytes--;
      puPosition--;
      *puPosition = binary_pszHEX_s[*puBytes & 0x0F];
      puPosition--;
      *puPosition = binary_pszHEX_s[(*puBytes & 0xF0) >> 4];
   }

   // ## copy four bytes for each loop iteration to speed up and simplify for
   //    compiler and cpu to do it's optimization.
   while( puBytes != puBytesStop )
   {
      puBytes--;
      puPosition--;
      *puPosition = binary_pszHEX_s[*puBytes & 0x0F];
      puPosition--;
      *puPosition = binary_pszHEX_s[(*puBytes & 0xF0) >> 4];

      puBytes--;
      puPosition--;
      *puPosition = binary_pszHEX_s[*puBytes & 0x0F];
      puPosition--;
      *puPosition = binary_pszHEX_s[(*puBytes & 0xF0) >> 4];

      puBytes--;
      puPosition--;
      *puPosition = binary_pszHEX_s[*puBytes & 0x0F];
      puPosition--;
      *puPosition = binary_pszHEX_s[(*puBytes & 0xF0) >> 4];

      puBytes--;
      puPosition--;
      *puPosition = binary_pszHEX_s[*puBytes & 0x0F];
      puPosition--;
      *puPosition = binary_pszHEX_s[(*puBytes & 0xF0) >> 4];
   }

   stringSql.append( (const char*)puBuffer, uSize );

   if( uLength > STACK_SIZE ) delete [] puBuffer;                              // delete heap memory if more than allowed stack size
}


/** ---------------------------------------------------------------------------
 * @brief Append variant value to string in a format that works for sql
 * @param variantValue value added to string as string
 * @param stringSql string value is appended to with sql rules
*/
void append_g( const gd::variant& variantValue, std::string& stringSql )
{
   using namespace gd::variant_type;

   size_t uSize = 0; // if value is added from buffer then size has number of bytes to copy
   char pbszBuffer[sizeof "-9223372036854775808_"]; // buffer used to convert numbers

   const gd::variant::value& value = variantValue.m_V; // get union from variant value

   auto uType = variantValue.type_number();
   switch( uType )
   {
   case eTypeNumberUnknown: 
      memcpy( pbszBuffer, "NULL", sizeof "NULL");
      uSize = 4;
      break;
   case eTypeNumberBit:
   case eTypeNumberBool:
      *pbszBuffer = value.b != 0 ? '1' : '0';
      uSize = 1;
      break;
   case eTypeNumberInt8: 
   {
      auto p_ = gd::utf8::itoa( (int32_t)value.int8, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;                                       // calculate filled buffer size
   }
   break;
   case eTypeNumberUInt8: 
   {
      auto p_ = gd::utf8::utoa( (uint32_t)value.uint8, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;                                       // calculate filled buffer size
   }
   break;
   case eTypeNumberInt16: 
   {
      auto p_ = gd::utf8::itoa( (int32_t)value.int16, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;                                       // calculate filled buffer size
   }
   break;
   case eTypeNumberUInt16: 
   {
      auto p_ = gd::utf8::utoa( (uint32_t)value.uint16, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;                                       // calculate filled buffer size
   }
   break;
   case eTypeNumberInt32: 
   {
      auto p_ = gd::utf8::itoa( (int32_t)value.int32, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;                                       // calculate filled buffer size
   }
   break;
   case eTypeNumberUInt32: 
   {
      auto p_ = gd::utf8::utoa( (uint32_t)value.int32, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;                                       // calculate filled buffer size
   }
   break;
   case eTypeNumberInt64: 
   {
      auto p_ = gd::utf8::itoa( (int64_t)value.int64, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;                                       // calculate filled buffer size
   }
   break;
   case eTypeNumberUInt64: 
   {
      auto p_ = gd::utf8::utoa( (uint64_t)value.uint64, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;
   }
   break;
   case eTypeNumberFloat: 
   {
      uSize = ::sprintf( pbszBuffer, "%f", value.f );
   }
   break;
   case eTypeNumberDouble: 
   {
      uSize = ::sprintf( pbszBuffer, "%g", value.d );
   }
   break;
   case eTypeNumberGuid: 
   {
      append_binary( (uint8_t*)value.pb, 16, stringSql );
   }
   break;
   case eTypeNumberString: 
   {
      append_ascii( (uint8_t*)value.pb, stringSql );
   }
   break;
   case eTypeNumberUtf8String: 
   {
      append_utf8( (uint8_t*)value.pb, stringSql );
   }
   break;
   case eTypeNumberBinary: 
   {
      append_binary( (uint8_t*)value.pb, variantValue.length(), stringSql );
   }
   break;

      
/*
   case eTypeNumberWString     : return *m_V.pwsz != L'\0' ? true : false;
*/
   default:                                                                      assert( false );
   }

   if( uSize != 0 ) { stringSql.append( pbszBuffer, uSize ); };

}


/** ---------------------------------------------------------------------------
 * @brief Append variant_view value to string in a format that works for sql
 * @param variantValue value added to string as string
 * @param stringSql string value is appended to with sql rules
*/
void append_g( const gd::variant_view& variantValue, std::string& stringSql )
{
   using namespace gd::variant_type;

   size_t uSize = 0; // if value is added from buffer then size has number of bytes to copy
   char pbszBuffer[sizeof "-9223372036854775808_"]; // buffer used to convert numbers

   const gd::variant_view::value& value = variantValue.m_V; // get union from variant value

   auto uType = variantValue.type_number();
   switch( uType )
   {
   case eTypeNumberUnknown: 
      memcpy( pbszBuffer, "NULL", sizeof "NULL");
      uSize = 4;
      break;
   case eTypeNumberBit:
   case eTypeNumberBool:
      *pbszBuffer = value.b != 0 ? '1' : '0';
      uSize = 1;
      break;
   case eTypeNumberInt8: 
   {
      auto p_ = gd::utf8::itoa( (int32_t)value.int8, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;                                       // calculate filled buffer size
   }
   break;
   case eTypeNumberUInt8: 
   {
      auto p_ = gd::utf8::utoa( (uint32_t)value.uint8, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;                                       // calculate filled buffer size
   }
   break;
   case eTypeNumberInt16: 
   {
      auto p_ = gd::utf8::itoa( (int32_t)value.int16, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;                                       // calculate filled buffer size
   }
   break;
   case eTypeNumberUInt16: 
   {
      auto p_ = gd::utf8::utoa( (uint32_t)value.uint16, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;                                       // calculate filled buffer size
   }
   break;
   case eTypeNumberInt32: 
   {
      auto p_ = gd::utf8::itoa( (int32_t)value.int32, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;                                       // calculate filled buffer size
   }
   break;
   case eTypeNumberUInt32: 
   {
      auto p_ = gd::utf8::utoa( (uint32_t)value.int32, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;                                       // calculate filled buffer size
   }
   break;
   case eTypeNumberInt64: 
   {
      auto p_ = gd::utf8::itoa( (int64_t)value.int64, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;                                       // calculate filled buffer size
   }
   break;
   case eTypeNumberUInt64: 
   {
      auto p_ = gd::utf8::utoa( (uint64_t)value.uint64, (uint8_t*)pbszBuffer );
      uSize = p_ - (uint8_t*)pbszBuffer;
   }
   break;
   case eTypeNumberFloat: 
   {
      uSize = ::sprintf( pbszBuffer, "%f", value.f );
   }
   break;
   case eTypeNumberDouble: 
   {
      uSize = ::sprintf( pbszBuffer, "%g", value.d );
   }
   break;
   case eTypeNumberGuid: 
   {
      append_binary( (uint8_t*)value.pb, 16, stringSql );
   }
   break;
   case eTypeNumberString: 
   {
      append_ascii( (uint8_t*)value.pb, stringSql );
   }
   break;
   case eTypeNumberUtf8String: 
   {
      append_utf8( (uint8_t*)value.pb, stringSql );
   }
   break;
   case eTypeNumberBinary: 
   {
      append_binary( (uint8_t*)value.pb, variantValue.length(), stringSql );
   }
   break;
      
/*
   case eTypeNumberWString     : return *m_V.pwsz != L'\0' ? true : false;
*/
   default:                                                                    assert( false );
   }

   if( uSize != 0 ) { stringSql.append( pbszBuffer, uSize ); };

}

_GD_SQL_QUERY_END