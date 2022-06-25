#include <malloc.h>
#include <iterator>

#include "gd_utf8.hpp"  

#include "gd_arguments.h"  

#pragma warning( disable : 4996 6054 6387 26812 33010 ) // disable warnings for buffer that might not be zero terminated and 

#ifndef _GD_PARAM_BEGIN
namespace gd::argument {
#else
_GD_PARAM_BEGIN
#endif

arguments::pointer arguments::move_to_value(pointer pPosition)
{                                                                                assert( pPosition != nullptr );
   enumCType eType = (enumCType)*pPosition;                                      // get section type
   if( eType == eType_ParameterName )
   {
      pPosition++;
      uint8_t uNameLength = *pPosition + 1;                                      // get name length and add one for byte that holds how many characters name has (it is not zero based)
      pPosition += uNameLength;
   }
   return pPosition;
}


arguments::const_pointer arguments::move_to_value(const_pointer pPosition)
{
   enumCType eType = (enumCType)*pPosition;                                      // get section type
   if( eType == eType_ParameterName )
   {
      pPosition++;
      uint8_t uNameLength = *pPosition + 1;                                      // get name length and add one for byte that holds how many characters name has (it is not zero based)
      pPosition += uNameLength;
   }
   return pPosition;
}


/*----------------------------------------------------------------------------- compare_name_s */ /**
 * compare name at argument position in arguments buffer
 * \param pPosition position in arguments buffer
 * \param stringName name to compare with
 * \return bool true if name matches, false if not
 */
bool arguments::compare_name_s(const_pointer pPosition, std::string_view stringName)
{
   if( is_name_s(pPosition) && get_name_s(pPosition) == stringName ) return true;
   return false;
}

static uint8_t ctype_size[arguments::CType_MAX] = {
   0,       // eTypeNumberUnknown = 0,
   1,       // eTypeNumberBool = 1,
   1,       // eCTypeNumberInt8 = 2,
   1,       // eCTypeNumberUInt8,
   2,       // eCTypeNumberInt16,
   2,       // eCTypeNumberUInt16,
   4,       // eCTypeNumberInt32,
   4,       // eCTypeNumberUInt32,
   8,       // eCTypeNumberInt64,
   8,       // eCTypeNumberUInt64,

   sizeof(float), // eTypeNumberFloat,
   sizeof(double),// eTypeNumberDouble,

   sizeof(void*), //eTypeNumberPointer,
   16,      // eTypeNumberGuid   
};


const char _binary_pszHEX[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };


template <typename CHAR>
inline void _binary_to_hex(CHAR* pTPosition, const uint8_t* pBytes, unsigned uLength)
{
   *pTPosition = (CHAR)0;                                                        // add char 0 ending to string
   const uint8_t* pBytesStop = pBytes - uLength;

   // optimize, align for 4 byte sections
   for( unsigned int u = uLength % 4; u > 0; u-- )
   {
      pBytes--;
      pTPosition--;
      *pTPosition = (CHAR)_binary_pszHEX[*pBytes & 0x0F];
      pTPosition--;
      *pTPosition = (CHAR)_binary_pszHEX[(*pBytes & 0xF0) >> 4];

   }

   while( pBytes != pBytesStop )
   {
      pBytes--;
      pTPosition--;
      *pTPosition = (CHAR)_binary_pszHEX[*pBytes & 0x0F];
      pTPosition--;
      *pTPosition = (CHAR)_binary_pszHEX[(*pBytes & 0xF0) >> 4];

      pBytes--;
      pTPosition--;
      *pTPosition = (CHAR)_binary_pszHEX[*pBytes & 0x0F];
      pTPosition--;
      *pTPosition = (CHAR)_binary_pszHEX[(*pBytes & 0xF0) >> 4];

      pBytes--;
      pTPosition--;
      *pTPosition = (CHAR)_binary_pszHEX[*pBytes & 0x0F];
      pTPosition--;
      *pTPosition = (CHAR)_binary_pszHEX[(*pBytes & 0xF0) >> 4];

      pBytes--;
      pTPosition--;
      *pTPosition = (CHAR)_binary_pszHEX[*pBytes & 0x0F];
      pTPosition--;
      *pTPosition = (CHAR)_binary_pszHEX[(*pBytes & 0xF0) >> 4];
   }
}

/*----------------------------------------------------------------------------- length */ /**
 * return length for param in bytes
 \note if param is holding a string, then ending zero isn't counted
 * \return unsigned int number of bytes param occupies 
 */
unsigned int arguments::argument::length() const
{
   auto eTypeNumber = type_number();
   if( eTypeNumber < arguments::eTypeNumberString ) { return ctype_size[(unsigned)eTypeNumber]; }
   else
   {
      if( ctype() & eValueLength )
      {
         auto uSize = *(uint32_t*)(m_unionValue.puch - 4);                       assert(uSize < 0x00A00000); // realistic
         return uSize;
      }
      else if( eTypeNumber == eTypeNumberWString ) return (unsigned int)wcslen(m_unionValue.pwsz) * 2 + 2;

      return (unsigned int)strlen(m_unionValue.pbsz) + 1;
   }
                                                                                 assert( false );
   return 0;
}

unsigned int arguments::argument::get_binary_as_hex(char* pbsz, unsigned int uLength) const
{                                                                                assert(uLength > 0);
   // each byte needs two characters
   uLength--;                                               // zero character position
   uLength >>= 1;                                           // number of bytes that string can hold

   auto uParamLength = length();
   if( uLength > uParamLength ) { uLength = uParamLength; }

   char* pbszPosition = &pbsz[uLength << 1];
   const uint8_t* pBytes = m_unionValue.puch + uLength;
   _binary_to_hex( pbszPosition, pBytes, uLength);

   return uLength;
}

#pragma warning( disable : 6255 )

/*----------------------------------------------------------------------------- get_binary_as_hex */ /**
 * Get binary value as hexadecimal formated string
 * \param stringHex
 * \return void
 */
void arguments::argument::get_binary_as_hex(std::string& stringHex) const
{
   char* pbsz;
   unsigned int uLength = length() * 2;

   if( uLength < 0x1000 )
   {
      pbsz = (char*)_alloca((size_t)uLength + 1);
   }
   else
   {
      pbsz = new char[(size_t)uLength + 1];
   }

   get_binary_as_hex(pbsz, uLength + 1);
   stringHex = std::string_view( pbsz, uLength );

   if( uLength >= 0x1000 ) delete[] pbsz;
}

/*----------------------------------------------------------------------------- get_int */ /**
 * Tries to convert value to int and returns that int value, if it failles to convert to int then 0 is returned
 * \return int converted value
 */
int arguments::argument::get_int() const
{
   int iValue = 0;

   switch( type_number_s( m_eType) )
   {
   case arguments::eTypeNumberUnknown:
      iValue = 0;
      break;
   case arguments::eTypeNumberBool :
      iValue = ( m_unionValue.b == true ? 1 : 0 );
      break;
   case arguments::eTypeNumberInt8:
      iValue = (int)m_unionValue.v_int8;
      break;
   case arguments::eTypeNumberUInt8:
      iValue = (int)m_unionValue.v_uint8;
      break;
   case arguments::eTypeNumberInt16 :
      iValue = (int)m_unionValue.v_int16;
      break;
   case arguments::eTypeNumberUInt16:
      iValue = (int)m_unionValue.v_uint16;
      break;
   case arguments::eTypeNumberInt32 :
      iValue = (int)m_unionValue.v_int32;
      break;
   case arguments::eTypeNumberUInt32 :
      iValue = (int)m_unionValue.v_uint32;
      break;
   case arguments::eTypeNumberInt64 :
      iValue = (int)m_unionValue.v_int64;
      break;
   case arguments::eTypeNumberUInt64 :
      iValue = (int)m_unionValue.v_uint64;
      break;
   case arguments::eTypeNumberFloat :
      iValue = (int)m_unionValue.f;
      break;
   case arguments::eTypeNumberDouble :
      iValue = (int)m_unionValue.d;
      break;
   case arguments::eTypeNumberString :
   case arguments::eTypeNumberUtf8String :
      iValue = atoi( m_unionValue.pbsz );
      break;
   case arguments::eTypeNumberWString :
      iValue = _wtoi( m_unionValue.pwsz );
      break;
   default:
                                                                                 assert( false );
   }


   return iValue;
}

/*----------------------------------------------------------------------------- get_uint */ /**
 * Try to get param value as unsigned int
 * \return unsigned int converted value
 */
unsigned int arguments::argument::get_uint() const
{
   unsigned int uValue = 0;

   switch( type_number_s( m_eType) )
   {
   case arguments::eTypeNumberUnknown:
      uValue = 0;
      break;
   case arguments::eTypeNumberBool:
      uValue = (m_unionValue.b == true ? 1 : 0);
      break;
   case arguments::eTypeNumberInt8:
      uValue = (unsigned int)m_unionValue.v_int8;
      break;
   case arguments::eTypeNumberUInt8:
      uValue = (unsigned int)m_unionValue.v_uint8;
      break;
   case arguments::eTypeNumberInt16:
      uValue = (unsigned int)m_unionValue.v_int16;
      break;
   case arguments::eTypeNumberUInt16:
      uValue = (unsigned int)m_unionValue.v_uint16;
      break;
   case arguments::eTypeNumberInt32:
      uValue = (unsigned int)m_unionValue.v_int32;
      break;
   case arguments::eTypeNumberUInt32:
      uValue = (unsigned int)m_unionValue.v_uint32;
      break;
   case arguments::eTypeNumberInt64:
      uValue = (unsigned int)m_unionValue.v_int64;
      break;
   case arguments::eTypeNumberUInt64:
      uValue = (unsigned int)m_unionValue.v_uint64;
      break;
   case arguments::eTypeNumberFloat:
      uValue = (unsigned int)m_unionValue.f;
      break;
   case arguments::eTypeNumberDouble:
      uValue = (unsigned int)m_unionValue.d;
      break;
   case arguments::eTypeNumberString:
   case arguments::eTypeNumberUtf8String:
      uValue = strtoul(m_unionValue.pbsz, nullptr, 10);
      break;
   case arguments::eTypeNumberWString:
      uValue = wcstoul(m_unionValue.pwsz, nullptr, 10);
      break;
   default:
      assert(false);
   }


   return uValue;
}

/*----------------------------------------------------------------------------- get_int */ /**
 * Tries to convert value to int and returns that int value, if it failles to convert to int then 0 is returned
 * \return int converted value
 */
int64_t arguments::argument::get_int64() const
{
   int64_t iValue = 0;

   switch( type_number_s( m_eType) )
   {
   case arguments::eTypeNumberUnknown:
      iValue = 0;
      break;
   case arguments::eTypeNumberBool :
      iValue = ( m_unionValue.b == true ? 1 : 0 );
      break;
   case arguments::eTypeNumberInt8:
      iValue = (int64_t)m_unionValue.v_int8;
      break;
   case arguments::eTypeNumberUInt8:
      iValue = (int64_t)m_unionValue.v_uint8;
      break;
   case arguments::eTypeNumberInt16 :
      iValue = (int64_t)m_unionValue.v_int16;
      break;
   case arguments::eTypeNumberUInt16:
      iValue = (int64_t)m_unionValue.v_uint16;
      break;
   case arguments::eTypeNumberInt32 :
      iValue = (int64_t)m_unionValue.v_int32;
      break;
   case arguments::eTypeNumberUInt32 :
      iValue = (int64_t)m_unionValue.v_uint32;
      break;
   case arguments::eTypeNumberInt64 :
      iValue = (int64_t)m_unionValue.v_int64;
      break;
   case arguments::eTypeNumberUInt64 :
      iValue = (int64_t)m_unionValue.v_uint64;
      break;
   case arguments::eTypeNumberFloat :
      iValue = (int64_t)m_unionValue.f;
      break;
   case arguments::eTypeNumberDouble :
      iValue = (int64_t)m_unionValue.d;
      break;
   case arguments::eTypeNumberString :
   case arguments::eTypeNumberUtf8String :
      iValue = _atoi64( m_unionValue.pbsz );
      break;
   case arguments::eTypeNumberWString :
      iValue = _wtoll( m_unionValue.pwsz );
      break;
   default:
                                                                                 assert( false );
   }


   return iValue;
}




double arguments::argument::get_double() const
{
   double dValue = 0;

   switch( type_number_s(m_eType) )
   {
   case arguments::eTypeNumberUnknown:
      dValue = 0;
      break;
   case arguments::eTypeNumberBool:
      dValue = (m_unionValue.b == true ? 1 : 0);
      break;
   case arguments::eTypeNumberInt8:
      dValue = (double)m_unionValue.v_int8;
      break;
   case arguments::eTypeNumberUInt8:
      dValue = (double)m_unionValue.v_uint8;
      break;
   case arguments::eTypeNumberInt16:
      dValue = (double)m_unionValue.v_int16;
      break;
   case arguments::eTypeNumberUInt16:
      dValue = (double)m_unionValue.v_uint16;
      break;
   case arguments::eTypeNumberInt32:
      dValue = (double)m_unionValue.v_int32;
      break;
   case arguments::eTypeNumberUInt32:
      dValue = (double)m_unionValue.v_uint32;
      break;
   case arguments::eTypeNumberInt64:
      dValue = (double)m_unionValue.v_int64;
      break;
   case arguments::eTypeNumberUInt64:
      dValue = (double)m_unionValue.v_uint64;
      break;
   case arguments::eTypeNumberFloat:
      dValue = (double)m_unionValue.f;
      break;
   case arguments::eTypeNumberDouble:
      dValue = m_unionValue.d;
      break;
   case arguments::eTypeNumberString:
   case arguments::eTypeNumberUtf8String:
      dValue = std::strtod(m_unionValue.pbsz, nullptr);
      break;
   case arguments::eTypeNumberWString:
      dValue = wcstod(m_unionValue.pwsz, nullptr);
      break;
   default:
      assert(false);
   }


   return dValue;
}

/*----------------------------------------------------------------------------- get_string */ /**
 * Return value as string object
 * \return std::string string object with value for param
 */
std::string arguments::argument::get_string() const
{
   std::string s;
   if( ctype_s( m_eType ) == (arguments::eTypeNumberString | eValueLength) || ctype_s( m_eType ) == (arguments::eTypeNumberUtf8String | eValueLength) )
   {
      s = std::string_view(m_unionValue.pbsz, length() - 1); // try for string before converting other possible values (remember to not include last zero ending as text)
   }
   else
   {
      char pbsz[40];
#ifdef _DEBUG
      memset(pbsz, 'x', 40);
      pbsz[39] = L'\0';
#else
      pbsz[0] = L'\0';
#endif
      enumCType eCType = static_cast<enumCType>( type_number_s(m_eType) );
      switch( eCType )
      {
      case arguments::eTypeNumberBool:
         pbsz[0] = '0';
         pbsz[1] = '\0';
         if( m_unionValue.b == true ) pbsz[0] = '1';
         break;
      case arguments::eTypeNumberInt8:
         pbsz[0] = m_unionValue.ch;
         pbsz[1] = '\0';
         break;
      case arguments::eTypeNumberUInt8:
         sprintf(pbsz, "%u", (unsigned int)m_unionValue.uch);
         break;
      case arguments::eTypeNumberInt16:
         sprintf(pbsz, "%d", (short)m_unionValue.s);
         break;
      case arguments::eTypeNumberUInt16:
         pbsz[0] = (char)m_unionValue.wch;
         pbsz[1] = '\0';
         break;
      case arguments::eTypeNumberInt32:
         sprintf(pbsz, "%d", (int)m_unionValue.v_int32);
         break;
      case arguments::eTypeNumberUInt32:
         sprintf(pbsz, "%u", (unsigned int)m_unionValue.v_uint32);
         break;
      case arguments::eTypeNumberInt64:
         sprintf(pbsz, "%lld", m_unionValue.v_int64);
         break;
      case arguments::eTypeNumberUInt64:
         sprintf(pbsz, "%llu", m_unionValue.v_uint64);
         break;
      case arguments::eTypeNumberFloat:
         sprintf(pbsz, "%g", (double)m_unionValue.f);
         break;
      case arguments::eTypeNumberDouble:
         sprintf(pbsz, "%g", m_unionValue.d);
         break;
      case arguments::eTypeNumberPointer:
         // When storing pointer and get_string is called, it thinks that the pointer
         // is to char string. If you haven't set this to char pointer and runs get_string
         // there will be an error.
                                                                                 assert(strlen(m_unionValue.pbsz) < 0x000F0000); // realistic
         s = m_unionValue.pbsz;
         break;
      case arguments::eTypeNumberString:
      case arguments::eTypeNumberUtf8String:
      {
         s = std::string_view(m_unionValue.pbsz);
      }
      break;
      case arguments::eTypeNumberWString:
      {
         gd::utf8::convert_utf16_to_uft8( reinterpret_cast<const uint16_t*>(m_unionValue.pwsz), s);
      }
      break;
      case arguments::eTypeNumberBinary:
      {
         get_binary_as_hex(s);
         return std::move(s);
      }
      case arguments::eTypeNumberGuid:
      {
         constexpr unsigned int uUuidSize = 16;
         //pbsz[uUuidSize] = '\0';
         _binary_to_hex(pbsz + uUuidSize * 2, (uint8_t*)m_unionValue.p + uUuidSize, uUuidSize);
      }
      break;
      default:
         assert(false);
      }

      if( s.empty() == true ) s = pbsz;
   }

   return std::move(s);
}


/*----------------------------------------------------------------------------- is_true */ /**
 * Checks to see if argument is a `true` value. When argument is a number then 0 = false, 
 * everything else ia true. Same with decimal values. For strings empty strings are false, 
 * and strings with text are true.
 * \return bool
 */
bool arguments::argument::is_true() const
{
   bool bTrue = false;

   switch( type_number_s(m_eType) )
   {
   case arguments::eTypeNumberUnknown:
      bTrue = false;
      break;
   case arguments::eTypeNumberBool:
      bTrue = m_unionValue.b;
      break;
   case arguments::eTypeNumberInt8:
      bTrue = (m_unionValue.v_int8 != 0 ? true : false);
      break;
   case arguments::eTypeNumberUInt8:
      bTrue = (m_unionValue.v_uint8 != 0 ? true : false);
      break;
   case arguments::eTypeNumberInt16:
      bTrue = (m_unionValue.v_int16 != 0 ? true : false);
      break;
   case arguments::eTypeNumberUInt16:
      bTrue = (m_unionValue.v_uint16 != 0 ? true : false);
      break;
   case arguments::eTypeNumberInt32:
      bTrue = (m_unionValue.v_int32 != 0 ? true : false);
      break;
   case arguments::eTypeNumberUInt32:
      bTrue = (m_unionValue.v_uint32 != 0 ? true : false);
      break;
   case arguments::eTypeNumberInt64:
      bTrue = (m_unionValue.v_int64 != 0 ? true : false);
      break;
   case arguments::eTypeNumberUInt64:
      bTrue = (m_unionValue.v_uint64 != 0 ? true : false);
      break;
   case arguments::eTypeNumberFloat:
      bTrue = (m_unionValue.f != 0.0 ? true : false);
      break;
   case arguments::eTypeNumberDouble:
      bTrue = (m_unionValue.d != 0.0 ? true : false);
      break;
   case arguments::eTypeNumberString:
   case arguments::eTypeNumberUtf8String:
      bTrue = (m_unionValue.pbsz != nullptr && *m_unionValue.pbsz != '\0' ? true : false);
      break;
   case arguments::eTypeNumberWString:
      bTrue = (m_unionValue.pwsz != nullptr && *m_unionValue.pbsz != L'\0' ? true : false);
      break;
   default:
      assert(false);                                                             // Why are you here ????
   }

   return bTrue;
}

void arguments::argument_edit::set(argument argumentSet)
{
   auto eType = argumentSet.type_number();
   if( is_type_fixed_size_s(eType) == true && eType == type_number() )
   {
      auto pValueData = m_pValue + 1;                                            // move past type to data
      unsigned uSize = ctype_size[eType];
      memcpy(pValueData, argumentSet.get_value_buffer(), uSize);
   }
   else
   {
      m_pArguments->set((pointer)m_pPosition, argumentSet.type(), (const_pointer)argumentSet.get_value_buffer(), argumentSet.length());
   }
}


/*----------------------------------------------------------------------------- arguments */ /**
 * construct arguments with pair object having name and value
 * *sample*
~~~{.cpp}
   arguments test({ "test", 1010101 });
   auto result = test.get_argument("test");
   if( result == arguments::argument( 1010101 ) )
   {
      std::cout << "equal !!" << std::endl;
   }
~~~ 
*/
arguments::arguments(std::pair<std::string_view, gd::variant> pairArgument)
{
   zero();
   auto _argument = get_argument_s( pairArgument.second );
   append_argument(pairArgument.first, _argument);
}



arguments::arguments(std::initializer_list<std::pair<std::string_view, gd::variant>> listPair)
{
   zero();
   for( auto it : listPair ) append_argument(it);
}

arguments& arguments::operator=(std::initializer_list<std::pair<std::string_view, gd::variant>> listPair)
{
   for( auto it : listPair ) append_argument(it);
   return *this;
}

/*----------------------------------------------------------------------------- append */ /**
 * Add typed parameter to binary stream of bytes
 * \param uType type of parameter
 * \param pBuffer pointer to binary data
 * \param uLength number of bytes added to stream
 * \return binary& reference to binary object
 */
arguments& arguments::append( param_type uType, const_pointer pBuffer, unsigned int uLength)
{
   reserve(m_uLength + uLength + sizeof(param_type) + sizeof(uint32_t));
   m_pBuffer[m_uLength] = uType;
   m_uLength++;
   if( (uType & eValueLength) == 0 )
   {
      memcpy(&m_pBuffer[m_uLength], pBuffer, uLength);
      m_uLength += uLength;                                                      assert(m_uLength < m_uBufferLength);

      return *this;
   }

   *(uint32_t*)&m_pBuffer[m_uLength] = (unsigned long)uLength;
   m_uLength += sizeof(uint32_t);
   memcpy(&m_pBuffer[m_uLength], pBuffer, uLength);
   m_uLength += uLength;                                                         assert(m_uLength < m_uBufferLength);

   return *this;
}


/*----------------------------------------------------------------------------- append */ /**
 * Add typed value to binary
 * typed value is a name in ascii format with beginning marker that it is a name, then length for name
 * and the name. After name type of value is added, then the value or if value needs length the length is added.
 * Lastly data for value is added
 * [name type][name length][name][value type][value length?][value]
 * \param pbszName name for value
 * \param uNameLength character count for name
 * \param uType type of value
 * \param pBuffer data, pointer to bytes holding data
 * \param uLength length for data
 * \return gd_std::binary& binary object for nested calls
 */
arguments& arguments::append(const char* pbszName, uint32_t uNameLength, param_type uType, const_pointer pBuffer, unsigned int uLength)
{                                                                                assert(strlen(pbszName) < 255); assert(m_uLength < 0xffffff); assert(uLength < 0xffffff);
#ifdef _DEBUG
   enumCType debug_eCType = (enumCType)(uType & ~eTypeNumber_MASK);              // get absolute variable type
#endif // _DEBUG

   // reserve data for name, length for name, name type and data type
   // [current length][data length][name length][type + length + zero ending (=param_type*3)][length value if data isn't primitive type]
   reserve(m_uLength + uLength + uNameLength + sizeof(param_type) * 3 + sizeof(uint32_t));

   m_pBuffer[m_uLength] = eType_ParameterName;                                   // set name type
   m_uLength++;
   m_pBuffer[m_uLength] = (uint8_t)uNameLength;                                  // set length for name
   m_uLength++;
   memcpy(&m_pBuffer[m_uLength], pbszName, uNameLength);                         // copy name
   m_uLength += uNameLength;                                                     // add length for name

   m_pBuffer[m_uLength] = uType;                                                 // set value type
   m_uLength++;

   if( (uType & eValueLength) == 0 )                                             // if type doesn't have specified length flag then just copy data into buffer
   {
      memcpy(&m_pBuffer[m_uLength], pBuffer, uLength);
      m_uLength += uLength;
                                                                                 assert(m_uLength <= m_uBufferLength);
      return *this;
   }

   *(uint32_t*)&m_pBuffer[m_uLength] = (uint32_t)uLength;                        // set length for data
   m_uLength += sizeof(uint32_t);
   memcpy(&m_pBuffer[m_uLength], pBuffer, uLength);                              // copy data
   m_uLength += uLength;                                                         assert(m_uLength <= m_uBufferLength);

   return *this;
}

arguments& arguments::set(const char* pbszName, uint32_t uNameLength, param_type uType, const_pointer pBuffer, unsigned int uLength)
{
   pointer pPosition = find(std::string_view(pbszName, uNameLength));
   if( pPosition == nullptr )
   {  // value was not found, just add it
      return append( pbszName, uNameLength, uType, pBuffer, uLength );
   }

   // ## Found value - replace
   // get current argument
   argument argumentOld = arguments::get_argument_s(pPosition);
   if( arguments::compare_type_s(argumentOld, uType) == true && (uType & (eValueLength|eValueArray)) == 0 )
   {
      pPosition += size_t(2) + size_t(uNameLength);
      memcpy(pPosition, pBuffer, uLength);
   }
   else
   {
      auto uOldSize = arguments::sizeof_name_s( uNameLength ) +  arguments::sizeof_s( argumentOld );
      auto uNewSize = arguments::sizeof_s( uNameLength, uType, uLength );
      if( uOldSize != uNewSize ) { resize(pPosition, uOldSize, uNewSize); }

      pPosition += arguments::sizeof_name_s(uNameLength);
      m_uLength += int(uNewSize - uOldSize);

      *pPosition = uType;
      pPosition++;

      if( (uType & eValueLength) == 0 )                                          // if type doesn't have specified length flag then just copy data into buffer
      {
         memcpy(pPosition, pBuffer, uLength);
      }
      else
      {
         *(uint32_t*)pPosition = (uint32_t)uLength;                              // set length for data
         pPosition += sizeof(uint32_t);
         memcpy(pPosition, pBuffer, uLength);                                    // copy data
      }
   }



   return *this;
}

arguments& arguments::set(pointer pPosition, param_type uType, const_pointer pBuffer, unsigned int uLength)
{
   // get current argument
   argument argumentOld = arguments::get_argument_s(pPosition);
   auto pPositionValue = move_to_value(pPosition);
   if( arguments::compare_type_s(argumentOld, uType) == true && (uType & (eValueLength | eValueArray)) == 0 )
   {
      memcpy(pPositionValue, pBuffer, uLength);
   }
   else
   {                                                                             assert( pPositionValue - pPosition < 255 ); // realistic
      auto uOldSize = get_total_param_length_s(pPosition);
      unsigned uNameLength = arguments::sizeof_name_s( pPosition );
      //auto uOldSize = uNameLength + arguments::sizeof_s(argumentOld);
      auto uNewSize = uNameLength + uLength + sizeof_value_prefix( uType );
      if( uOldSize != uNewSize ) 
      { 
         if( uOldSize < uNewSize ) reserve(m_uLength + (uNewSize - uOldSize));   // increase buffer if needed
         resize(pPosition, uOldSize, uNewSize); 
      }

      m_uLength += int(uNewSize - uOldSize);

      pPosition += uNameLength;
      *pPosition = uType;
      pPosition++;

      if( (uType & eValueLength) == 0 )                                          // if type doesn't have specified length flag then just copy data into buffer
      {
         memcpy(pPosition, pBuffer, uLength);
      }
      else
      {
         *(uint32_t*)pPosition = (uint32_t)uLength;                              // set length for data
         pPosition += sizeof(uint32_t);
         memcpy(pPosition, pBuffer, uLength);                                    // copy data
      }
   }



   return *this;
}

/*----------------------------------------------------------------------------- count */ /**
 * Count param values for name
 * \param stringName name that is counted
 * \return unsigned int number of param values found for name
 */
unsigned int arguments::count(std::string_view stringName) const
{
   unsigned int uCount = 0;
   for( auto pPosition = next(); pPosition != nullptr; pPosition = next(pPosition) )
   {
      if( arguments::is_name_s(pPosition) == true )
      {
         if( arguments::get_name_s(pPosition) == stringName ) uCount++;
      }
   }

   return uCount;
}

/*----------------------------------------------------------------------------- find */ /**
 * Get position to parameter for index, if there are more than one parameter
 * with the name the first is taken
 * \param stringName
 * \return gd::argument::arguments::const_pointer position to name if found, otherwise null
 */
arguments::pointer arguments::find( unsigned int uIndex )
{
   pointer pPosition = m_pBuffer;
   while( uIndex > 0 && (pPosition = next(pPosition)) != nullptr )
   {
      uIndex--;
   }

   return pPosition;
}


/*----------------------------------------------------------------------------- find */ /**
 * Try to find pointer in arguments that match name, if there are more than one parameter
 * with the name the first is taken
 * \param stringName
 * \return gd::argument::arguments::const_pointer position to name if found, otherwise null
 */
arguments::pointer arguments::find(std::string_view stringName)
{
   for( auto pPosition = next(); pPosition != nullptr; pPosition = next(pPosition) )
   {
      if( arguments::is_name_s(pPosition) == true )
      {
         if( arguments::get_name(pPosition) == stringName ) return pPosition;
      }
   }

   return nullptr;
}


/*----------------------------------------------------------------------------- find */ /**
 * Try to find parameter in params that match name, if there are more than one parameter
 * with the name the first is taken
 * \param stringName
 * \return gd::argument::arguments::const_pointer position to name if found, otherwise null
 */
arguments::const_pointer arguments::find(std::string_view stringName) const
{
   for( auto pPosition = next(); pPosition != nullptr; pPosition = next(pPosition) )
   {
      if( arguments::is_name_s(pPosition) == true )
      {
         if( arguments::get_name_s(pPosition) == stringName ) return pPosition;
      }
   }

   return nullptr;
}


/*----------------------------------------------------------------------------- find */ /**
 * Try to find parameter in params that match name, if there are more than one parameter
 * with the name the first is taken
 * \param stringName name to find
 * \param pPosition position where to start from
 * \return gd::argument::arguments::const_pointer position to name if found, otherwise null
 */
arguments::const_pointer arguments::find(std::string_view stringName, const_pointer pPosition) const
{
   for( ; pPosition != nullptr; pPosition = next(pPosition) )
   {
      if( arguments::is_name_s(pPosition) == true )
      {
         if( arguments::get_name_s(pPosition) == stringName ) return pPosition;
      }
   }

   return nullptr;
}

/*----------------------------------------------------------------------------- find_all */ /**
 * Find parameters for name, place them in vector and return
 * \param stringName name for parameter
 * \return std::vector<gd::argument::arguments::const_pointer> vector where parameters are placed
 */
std::vector<arguments::const_pointer> gd::argument::arguments::find_all(std::string_view stringName) const
{
   std::vector<arguments::const_pointer> vectorP;
   for( auto pPosition = next(); pPosition != nullptr; pPosition = next(pPosition) )
   {
      if( arguments::is_name_s(pPosition) == true )
      {
         if( arguments::get_name_s(pPosition) == stringName ) vectorP.push_back( pPosition );
      }
   }

   return vectorP;
}

/*----------------------------------------------------------------------------- print */ /**
 * Print all values into string
 * *sample*
~~~{.cpp}
   gd::argument::arguments argumentsValue;

   argumentsValue.append("AAA", int32_t(1111));
   argumentsValue.append("BBB", int32_t(2222));

   auto stringAll = argumentsValue.print();
~~~
 * \return std::string all values as text
 */
std::string arguments::print() const
{
   std::string stringPrint;

   for( auto pPosition = next(); pPosition != nullptr; pPosition = next(pPosition) )
   {
      if( stringPrint.empty() == false ) stringPrint += std::string_view(", ");

      stringPrint += arguments::print_s(pPosition);
   }

   return stringPrint;
}

std::string arguments::print(std::string_view stringFormat) const
{
   char pbszBuffer[256];
   std::string stringPrint;
   auto uLength = stringFormat.length();

   auto next = [](auto it, auto itEnd) -> auto {
      while( it != itEnd )
      {
         it++;
         if( *it == '{' )
         {
            it++;
            return it;
         }
      }
      return it;
   };

   auto itPosition = std::begin(stringFormat);
   auto itEnd = std::end(stringFormat);
   while( itPosition != itEnd )
   {
      auto itTo = next( itPosition, itEnd );
      std::copy(itPosition, itTo, std::back_inserter(stringPrint));
      if( *itTo != '{' )
      {
         *pbszBuffer = '\0';
         auto pbszPosition = pbszBuffer;
         while( *itTo != '}' && itTo != itEnd )
         {
            *pbszPosition += *itTo;
            pbszPosition++;
            itTo++;
         }

         *pbszPosition += '\0';

         stringPrint += get_argument(pbszBuffer).get_string();

      }

      itPosition = itTo;
   }

   /*
   for( auto itPosition = std::begin( stringFormat ); itPosition != std::end( stringFormat ); itPosition++ )
   {
      if( *itPosition == '{' )
      {
         itPosition++;
         if( itPosition == std::end(stringFormat) || *(itPosition + 1) == '{' )
         {
            stringPrint += '{';
            if( itPosition != std::end(stringFormat) ) itPosition++;
            continue;
         }
      }
   }
   */

   return stringPrint;
}

/*----------------------------------------------------------------------------- print_json */ /**
 * print in a json format
 * \return std::string
 */
std::string arguments::print_json() const
{
   std::string stringPrint;

   for( auto pPosition = next(); pPosition != nullptr; pPosition = next(pPosition) )
   {
      if( stringPrint.empty() == false ) stringPrint += std::string_view(", ");

      argument argumentValue = get_argument_s(pPosition);

      if( argumentValue.is_text() )
      {
         stringPrint += "\"";
         stringPrint += arguments::print_s(pPosition);
         stringPrint += "\"";
      }
      else
      {
         stringPrint += arguments::print_s(pPosition);
      }
   }

   return stringPrint;
}


/*----------------------------------------------------------------------------- print */ /**
 * Print selected values into string
 * *sample*
~~~{.cpp}
   gd::argument::arguments argumentsValue;

   argumentsValue.append("AAA", int32_t(1111));
   argumentsValue.append("BBB", int32_t(2222));
   argumentsValue.append("CCC", int32_t(3333));
   argumentsValue.append("DDD", int32_t(4444));

   auto stringAll = argumentsValue.print( std::begin( argumentsValue ) );
   auto it =  std::begin( argumentsValue );
   it++;
   it++;
   auto stringThirdAndFourth = argumentsValue.print( it );  // out: "CCC": 3333, "DDD": 4444
~~~
 * \param itBegin start printing values
 * \param itEnd quit printing values at this position
 * \param stringSplit text to split values for better format
 * \return std::string selected values as text
 */
std::string arguments::print( const_iterator itBegin, const_iterator itEnd, std::string_view stringSplit ) const
{
   std::string stringPrint;

   for( ; itBegin != itEnd; itBegin++ )
   {
      if( stringPrint.empty() == false ) stringPrint += stringSplit;

      stringPrint += arguments::print_s(itBegin);
   }

   return stringPrint;
}



/*----------------------------------------------------------------------------- reserve */ /**
 * Make sure internal buffer can hold number of bytes requested
 \note if buffer is increased it is increased with 1.5 * count
 * \param uCount number of bytes to reserve
 */
void arguments::reserve(unsigned int uCount)
{
   if( uCount > m_uBufferLength )
   {
      uCount += (uCount >> 1);
      if( uCount > 32 ) uCount += (64 - (uCount % 64));                          // align to 64 byte blocks
      unsigned char* pBuffer = new unsigned char[uCount];

      if( m_uLength > 0 ) memcpy(pBuffer, m_pBuffer, m_uLength);
      if( m_bOwner ) delete m_pBuffer;

      m_bOwner = true;
      m_pBuffer = pBuffer;
      m_uBufferLength = uCount;
   }
}


/*----------------------------------------------------------------------------- remove */ /**
 * Remove param from params by removing section in internal buffer holding param values
 * \param pPosition position where to remove
 */
void gd::argument::arguments::remove(const_pointer pPosition)
{                                                                                assert( verify_d(pPosition) );
   auto uSize = arguments::get_total_param_length_s( pPosition );

   // removes section by moving memory
   // [..........xxxxxxxxxx..........]
   // [....................] ( removed "xxxxxxxxxx" )
   memmove( (void*)pPosition, (void*)(pPosition + uSize), get_buffer_end() - (pPosition + uSize) ); // move memory

   m_uLength -= uSize;                                                           assert((int)m_uLength >= 0);
}

arguments::pointer arguments::_reserve_no_copy(unsigned int uCount)
{
   unsigned char* pBuffer = new unsigned char[uCount];

   if( m_bOwner ) delete m_pBuffer;

   m_uLength = 0;
   m_bOwner = true;
   m_pBuffer = pBuffer;
   m_uBufferLength = uCount;

   return pBuffer;
}

// TODO: Implement resize in order to be able to modify a value in arguments object
/*----------------------------------------------------------------------------- resize */ /**
 * Resize buffer, this is used to make space for new value at specific position (replace value)
 * \param pPosition
 * \param iOffset
 * \param iNewOffset
 * \return int
 */
int gd::argument::arguments::resize(pointer pPosition, int iOffset, int iNewOffset)
{
   int iSizeChange = iNewOffset - iOffset;

   if( iSizeChange != 0 )                                                         // need more  space for value compared to current value
   {
      // ## shrink section by moving memory
      // [..........xxxxxxxxxX..........] X = pPosition + iOffset
      // [..........xxxxxxxX..........] ( removed "xx" ) X = pPosition + iNewOffset
      // get_buffer_end() - (pPosition + iOffset) = number of bytes to be moved
      // ## expand section by moving memory
      // [..........xxxxxxxxxX..........] X = pPosition + iOffset
      // [..........ooooxxxxxxxxxX..........] ( added "oooo" ) X = pPosition + iNewOffset
      // get_buffer_end() - (pPosition + iOffset) = number of bytes to be moved
      memmove( (void*)(pPosition + iNewOffset), (void*)(pPosition + iOffset), get_buffer_end() - (pPosition + iOffset) ); // move memory
   }

   return iSizeChange;
}

/*----------------------------------------------------------------------------- size */ /**
 * Count number of param values in params object and return count
 * \return size_t number of values in params object
 */
size_t arguments::size() const
{
   if( empty() == true ) return 0;

   size_t uCount = 1;
   const_pointer pPosition = m_pBuffer;
   while( (pPosition = next(pPosition)) != nullptr )
   {
      uCount++;
   }
   return uCount;
}


   
/*----------------------------------------------------------------------------- get_param */ /**
 * Get param at specified position
 * \param uIndex position in buffer where param is found
 * \return params::param param value returned
 */
arguments::argument arguments::get_argument(unsigned int uIndex) const
{
   const_pointer pPosition = m_pBuffer;
   while( uIndex > 0 && (pPosition = next(pPosition)) != nullptr )
   {
      uIndex--;
   }

   if( pPosition != nullptr ) return arguments::get_argument_s(pPosition);
   return argument();
}

/*----------------------------------------------------------------------------- compare_argument_s */ /**
 * compare to argument values if equal
 * \param v1 first argument that is compared
 * \param argument2 second argument to compare
 * \return bool true if equal, false if not
 */
bool arguments::compare_argument_s(const argument& argument1, const argument& argument2)
{
   auto eType = argument1.type_number();
   if( eType != argument2.type_number() ) return false;
   switch( eType )
   {
   case arguments::eTypeNumberUnknown: return true;
   case arguments::eTypeNumberBool: return argument1.m_unionValue.b == argument2.m_unionValue.b;
   case arguments::eTypeNumberInt8: return argument1.m_unionValue.v_int8 == argument2.m_unionValue.v_int8;
   case arguments::eTypeNumberUInt8: return argument1.m_unionValue.v_uint8 == argument2.m_unionValue.v_uint8;
   case arguments::eTypeNumberInt16: return argument1.m_unionValue.v_int16 == argument2.m_unionValue.v_int16;
   case arguments::eTypeNumberUInt16: return argument1.m_unionValue.v_uint16 == argument2.m_unionValue.v_uint16;
   case arguments::eTypeNumberInt32: return argument1.m_unionValue.v_int32 == argument2.m_unionValue.v_int32;
   case arguments::eTypeNumberUInt32: return argument1.m_unionValue.v_uint32 == argument2.m_unionValue.v_uint32;
   case arguments::eTypeNumberInt64: return argument1.m_unionValue.v_int64 == argument2.m_unionValue.v_int64;
   case arguments::eTypeNumberUInt64: return argument1.m_unionValue.v_uint64 == argument2.m_unionValue.v_uint64;
   case arguments::eTypeNumberPointer: return argument1.m_unionValue.v_uint64 == argument2.m_unionValue.v_uint64;
      return argument1.m_unionValue.p == argument2.m_unionValue.p;
      break;
   case arguments::eTypeNumberGuid: return argument1.m_unionValue.v_uint64 == argument2.m_unionValue.v_uint64;
      return memcmp(argument1.m_unionValue.p, argument2.m_unionValue.p, 16) == 0;
      break;
   case arguments::eTypeNumberFloat: return argument1.m_unionValue.f == argument2.m_unionValue.f;
   case arguments::eTypeNumberDouble: return argument1.m_unionValue.d == argument2.m_unionValue.d;
   case arguments::eTypeNumberString: 
   case arguments::eTypeNumberUtf8String:
      return strcmp(argument1.m_unionValue.pbsz, argument2.m_unionValue.pbsz) == 0;
      break;
   case arguments::eTypeNumberWString:
      return wcscmp(argument1.m_unionValue.pwsz, argument2.m_unionValue.pwsz) == 0;
      break;
   }

   return false;
}

/*----------------------------------------------------------------------------- compare_argument_group_s */ /**
 * compare arguments based on type, it is a broader comparison that tries to match value
 * even if it isn't same type
 * \param argument1 first argument that is compared
 * \param argument2 second argument to compare
 * \return bool true if equal, false if not
 */
bool arguments::compare_argument_group_s(const argument& argument1, const argument& argument2)
{
   if( argument1.is_number() == true )
   {
      if( argument1.is_decimal() ) return argument1.get_double() == argument2.get_double();
      else return argument1.get_int64() == argument2.get_int64();
   }

   return compare_argument_s( argument1, argument2 );
}




bool arguments::compare_argument_group_s(const argument& argument1, const gd::variant_view& VV2)
{
   if( argument1.is_number() == VV2.is_number() )
   {
      if( argument1.is_decimal() ) return argument1.get_double() == VV2.get_double();
      else return argument1.get_int64() == VV2.get_int64();
   }

   return compare_s( argument1, VV2 );
}

bool arguments::compare_s(const argument& v1, const gd::variant_view v2)
{
   if( v1.type_number() != v2.type_number() ) return false;                      // type numbers matches between argument variant_view

   switch( v1.type_number() )
   {
   case arguments::eTypeNumberUnknown: return true;
   case arguments::eTypeNumberBool: return v1.m_unionValue.b == v2.m_V.b;
   case arguments::eTypeNumberInt8: return v1.m_unionValue.v_int8 == v2.m_V.int8;
   case arguments::eTypeNumberUInt8: return v1.m_unionValue.v_uint8 == v2.m_V.uint8;
   case arguments::eTypeNumberInt16: return v1.m_unionValue.v_int16 == v2.m_V.int16;
   case arguments::eTypeNumberUInt16: return v1.m_unionValue.v_uint16 == v2.m_V.uint16;
   case arguments::eTypeNumberInt32: return v1.m_unionValue.v_int32 == v2.m_V.int32;
   case arguments::eTypeNumberUInt32: return v1.m_unionValue.v_uint32 == v2.m_V.uint32;
   case arguments::eTypeNumberInt64: return v1.m_unionValue.v_int64 == v2.m_V.int64;
   case arguments::eTypeNumberUInt64: return v1.m_unionValue.v_uint64 == v2.m_V.uint64;
   case arguments::eTypeNumberPointer: return v1.m_unionValue.v_uint64 == v2.m_V.uint64;
      return v1.m_unionValue.p == v2.m_V.p;
      break;
   case arguments::eTypeNumberGuid: 
      return memcmp(v1.m_unionValue.p, v2.m_V.p, 16) == 0;
      break;
   case arguments::eTypeNumberFloat: return v1.m_unionValue.f == v2.m_V.f;
   case arguments::eTypeNumberDouble: return v1.m_unionValue.d == v2.m_V.d;
   case arguments::eTypeNumberString:
   case arguments::eTypeNumberUtf8String:
      return strcmp(v1.m_unionValue.pbsz, v2.m_V.pbsz) == 0;
      break;
   case arguments::eTypeNumberWString:
      return wcscmp(v1.m_unionValue.pwsz, v2.m_V.pwsz) == 0;
      break;
   }

   return false;
}

/*----------------------------------------------------------------------------- get_param_s */ /**
 * return param for position iterator is pointing at
 * \param it position for returned param
 * \return param  holding value at iterator position
 */
arguments::argument arguments::get_argument_s(arguments::const_pointer pPosition)
{
   arguments::enumCType eCType = (arguments::enumCType)*pPosition;
   pPosition++;
   switch( eCType )
   {
   case arguments::eTypeNumberUnknown: return arguments::argument();
   case arguments::eTypeNumberBool: return arguments::argument(*(bool*)pPosition);
   case arguments::eTypeNumberInt8: return arguments::argument(*(int8_t*)pPosition);
   case arguments::eTypeNumberUInt8: return arguments::argument(*(uint8_t*)pPosition);
   case arguments::eTypeNumberInt16: return arguments::argument(*(int16_t*)pPosition);
   case arguments::eTypeNumberUInt16: return arguments::argument(*(uint16_t*)pPosition);
   case arguments::eTypeNumberInt32: return arguments::argument(*(int32_t*)pPosition);
   case arguments::eTypeNumberUInt32: return arguments::argument(*(uint32_t*)pPosition);
   case arguments::eTypeNumberInt64: return arguments::argument(*(int64_t*)pPosition);
   case arguments::eTypeNumberUInt64: return arguments::argument(*(uint64_t*)pPosition);
   case arguments::eTypeNumberFloat: return arguments::argument(*(float*)pPosition);
   case arguments::eTypeNumberDouble: return arguments::argument(*(double*)pPosition);

   case arguments::eTypeNumberPointer: return arguments::argument((void*)(*(size_t*)pPosition));

   case arguments::eTypeNumberGuid: return arguments::argument((const uint8_t*)pPosition, eTypeGuid);

   // ## values that has value length stored in front of value, this is a four byte value


   case arguments::eTypeNumberString: return arguments::argument(eTypeString, (const uint8_t*)(const char*)(pPosition));
   case arguments::eTypeNumberUtf8String: return arguments::argument(eTypeUtf8String, (const uint8_t*)(const char*)(pPosition));
   case arguments::eTypeNumberWString: return arguments::argument(eTypeWString, (const uint8_t*)(const wchar_t*)(pPosition));
   case arguments::eTypeNumberBinary: return arguments::argument(eTypeGuid, (const uint8_t*)pPosition);

   case (arguments::eTypeNumberString | arguments::eValueLength): return arguments::argument(eTypeString | arguments::eValueLength, (const uint8_t*)(const char*)(pPosition + sizeof(uint32_t)));
   case (arguments::eTypeNumberUtf8String | arguments::eValueLength): return arguments::argument(eTypeUtf8String | arguments::eValueLength, (const uint8_t*)(const char*)(pPosition + sizeof(uint32_t)));
   case (arguments::eTypeNumberWString | arguments::eValueLength): return arguments::argument(eTypeWString | arguments::eValueLength, (const uint8_t*)(const wchar_t*)(pPosition + sizeof(uint32_t)));
   case (arguments::eTypeNumberBinary | arguments::eValueLength): return arguments::argument(eTypeBinary | arguments::eValueLength, (const uint8_t*)pPosition + sizeof(uint32_t));

   case arguments::eType_ParameterName:
   {
      pPosition += *pPosition;
      pPosition++;
      return get_argument_s(pPosition);
   }
   break;

   default:
      assert(false);
   }
   return arguments::argument();
}

arguments::argument_edit arguments::get_edit_param_s(arguments* parguments, arguments::const_pointer pPosition)
{
   arguments::argument argumentValue = arguments::get_argument_s( pPosition );
   return arguments::argument_edit( parguments, pPosition, argumentValue );
}

/*----------------------------------------------------------------------------- get_total_param_length_s */ /**
 * Calculate number of bytes param value is using in internal buffer where param values
 * is stored
 * \param pPosition start position for param
 * \return unsigned int number of bytes param value use in buffer
 */
unsigned int arguments::get_total_param_length_s(const_pointer pPosition)
{
   const_pointer pEnd = next_s( pPosition );
   return static_cast<unsigned int>(pEnd - pPosition);
}

std::vector<arguments::argument> arguments::get_argument_all_s(const_pointer pBegin, const_pointer pEnd, std::string_view stringName)
{
   std::vector<argument> vectorArgument;
   do 
   {
      if( compare_name_s(pBegin, stringName) == true ) vectorArgument.push_back( get_argument_s( pBegin ) );
   } while( (pBegin = next_s(pBegin)) < pEnd );
   return vectorArgument;
}

/*----------------------------------------------------------------------------- get_total_param_length_s */ /**
 * Calculate number of bytes needed to store named value in internal buffer used by arguments object
 * \param stringName name for value
 * \param argumentValue value store for name
 * \return unsigned int number of bytes needed to store value in internal buffer
 */
unsigned int gd::argument::arguments::get_total_param_length_s(std::string_view stringName, const argument argumentValue)
{
   return 1 + (unsigned int)stringName.length() + sizeof_s(argumentValue);
}


/*----------------------------------------------------------------------------- next */ /**
 * Move to next element in binary stream. Make sure that next value is compatible type
 * \param riPosition current position in stream
 * \return arguments::pointer position to next value
 */
arguments::pointer arguments::next_s(pointer pPosition) 
{
   pPosition = arguments::move_to_value(pPosition);
   uint8_t uType = *pPosition;                                                   // get value type
   pPosition++;                                                                  // move past type
   if( (uType & eType_MASK) == 0 )
   {
      pPosition += ctype_size[uType];
   }
   else
   {
      if( uType & eValueLength )
      {
         uint32_t uLength = *(uint32_t*)pPosition;
         pPosition += sizeof(uint32_t);
         pPosition += uLength;
      }
   }

   return pPosition;
}

/*----------------------------------------------------------------------------- next */ /**
 * Move to next element in binary stream. Make sure that next value is compatible type
 * \param riPosition current position in stream
 * \return arguments::const_pointer position to next value
 */
arguments::const_pointer arguments::next_s(const_pointer pPosition) 
{
   pPosition = arguments::move_to_value(pPosition);
   uint8_t uType = *pPosition;                                                   // get value type
   pPosition++;                                                                  // move past type
   if( (uType & eType_MASK) == 0 )
   {
      if( uType < eTypeNumberString ) { pPosition += ctype_size[uType]; }
      else
      {
         if( uType == eTypeNumberString ||
            uType == eTypeNumberUtf8String ) pPosition += strlen((const char*)pPosition) + 1;
         else if( uType == eTypeNumberString ) pPosition += wcslen((const wchar_t*)pPosition) * 2 + 2;
         else if( uType == eTypeNumberUtf32String )
         {
            unsigned uLength = 0;
            while( *((const char32_t*)pPosition + uLength) ) uLength++;
            pPosition += (uLength + 1) * sizeof(char32_t);
         }
         else assert(false);
      }
   }
   else
   {
      if( uType & eValueLength )
      {
         uint32_t uLength = *(uint32_t*)pPosition;
         pPosition += sizeof(uint32_t);
         pPosition += uLength;
      }
   }

   return pPosition;
}



/*----------------------------------------------------------------------------- sizeof_s */ /**
 * Return size for argument in bytes
 * \param argumentValue argument value size is returned for
 * \return unsigned int number of bytes for value
 */
unsigned int arguments::sizeof_s(const argument& argumentValue)
{
   unsigned int uSize = 1;                                                       // start with size for type prefix (1 byte value in front);
   if( argumentValue.ctype() & eValueLength )  uSize += sizeof(uint32_t);        // add size value in front of value

   return uSize + argumentValue.length();
}

/*----------------------------------------------------------------------------- print_s */ /**
 * Print values into text and return string with values
 * \param pPosition position in arguments object where value is located
 * \param uPairType pair type to print, key and/or value can be printed
 * \return std::string text containing key/value 
 */
std::string arguments::print_s(const_pointer pPosition, uint32_t uPairType)
{
   std::string stringArgument;
   if( *pPosition == eType_ParameterName )
   {
      if( uPairType & ePairTypeKey )
      {
         stringArgument += "\"";
         stringArgument.append( reinterpret_cast<const char*>(pPosition+ 2), *(pPosition + 1) );
         stringArgument += "\": ";
      }
      pPosition += size_t(pPosition[1]) + size_t(2);                             // move to value
   }

   if( uPairType & ePairTypeValue )
   {
      arguments::argument argumentValue( get_argument_s( pPosition ) );
      stringArgument += argumentValue.get_string();
   }

   return stringArgument;
}

/*----------------------------------------------------------------------------- sizeof_s */ /**
 * Calculate length for name value where each part have been counted
 * \param uNameLength name length for argument stored in arguments
 * \param uType type of value
 * \param uValueLength length needed to store value
 * \return unsigned int calculated length needed to store value in arguments buffer
 */
unsigned int gd::argument::arguments::sizeof_s(uint32_t uNameLength, param_type uType, unsigned int uValueLength)
{
   unsigned int uSize = 3;                                                       // 3 - one byte for name type, one byte for name length, and one byte for value type
   uSize += uNameLength;                                                         // add name length
   if( uType & eValueLength ) uSize += sizeof(uint32_t); 
   uSize += uValueLength;

   return uSize;
}

/*----------------------------------------------------------------------------- get_variant_s */ /**
 * Return argument value as variant
 * \param argumentValue value that is returned as variant
 * \return gd::variant value with argument value
 */
gd::variant arguments::get_variant_s(const arguments::argument& argumentValue)
{
   const auto& value = argumentValue.get_value();

   switch( type_number_s(argumentValue.m_eType) )
   {
   case arguments::eTypeNumberBool:
      return gd::variant(value.b);
      break;
   case arguments::eTypeNumberInt8:
      return gd::variant(value.v_int8);
      break;
   case arguments::eTypeNumberUInt8:
      return gd::variant(value.v_uint8);
      break;
   case arguments::eTypeNumberInt16:
      return gd::variant(value.v_int16);
      break;
   case arguments::eTypeNumberUInt16:
      return gd::variant(value.v_uint16);
      break;
   case arguments::eTypeNumberInt32:
      return gd::variant(value.v_int32);
      break;
   case arguments::eTypeNumberUInt32:
      return gd::variant(value.v_uint32);
      break;
   case arguments::eTypeNumberInt64:
      return gd::variant(value.v_int64);
      break;
   case arguments::eTypeNumberUInt64:
      return gd::variant(value.v_uint64);
      break;
   case arguments::eTypeNumberFloat:
      return gd::variant(value.f);
      break;
   case arguments::eTypeNumberDouble:
      return gd::variant(value.d);
      break;
   case arguments::eTypeNumberString:
   case arguments::eTypeNumberUtf8String:
      return gd::variant(value.pbsz, (size_t)argumentValue.length());
      break;
   case arguments::eTypeNumberWString:
      return gd::variant(value.pwsz, (size_t)argumentValue.length());
      break;
   default:
      assert(false);
   }

   return variant();
}

/*----------------------------------------------------------------------------- get_variant_s */ /**
 * Return argument value as variant, variant value returned do not own the memory so
 * make sure that the value isn't deleted as long as the variant value is in use
 * \param argumentValue value that is returned as variant
 * \return gd::variant value with argument value
 */
gd::variant arguments::get_variant_s(const arguments::argument& argumentValue, bool)
{
   const auto& value = argumentValue.get_value();

   switch( type_number_s(argumentValue.m_eType) )
   {
   case arguments::eTypeNumberBool:
      return gd::variant(value.b);
      break;
   case arguments::eTypeNumberInt8:
      return gd::variant(value.v_int8);
      break;
   case arguments::eTypeNumberUInt8:
      return gd::variant(value.v_uint8);
      break;
   case arguments::eTypeNumberInt16:
      return gd::variant(value.v_int16);
      break;
   case arguments::eTypeNumberUInt16:
      return gd::variant(value.v_uint16);
      break;
   case arguments::eTypeNumberInt32:
      return gd::variant(value.v_int32);
      break;
   case arguments::eTypeNumberUInt32:
      return gd::variant(value.v_uint32);
      break;
   case arguments::eTypeNumberInt64:
      return gd::variant(value.v_int64);
      break;
   case arguments::eTypeNumberUInt64:
      return gd::variant(value.v_uint64);
      break;
   case arguments::eTypeNumberFloat:
      return gd::variant(value.f);
      break;
   case arguments::eTypeNumberDouble:
      return gd::variant(value.d);
      break;
   case arguments::eTypeNumberString:
      return gd::variant(value.pbsz, (size_t)argumentValue.length(), false);
      break;
   case arguments::eTypeNumberUtf8String:
      return gd::variant( variant::utf8( value.pbsz, (size_t)argumentValue.length() ) );
      break;
   case arguments::eTypeNumberWString:
      return gd::variant(value.pwsz, (size_t)argumentValue.length(), false);
      break;
   default:
      assert(false);
   }

   return variant();
}

/*----------------------------------------------------------------------------- get_variant_s */ /**
 * Return argument value as variant_view
 * \param argumentValue value that is returned as variant_view
 * \return gd::variant_view value with argument value
 */
gd::variant_view arguments::get_variant_view_s(const arguments::argument& argumentValue)
{
   const auto& value = argumentValue.get_value();

   switch( type_number_s(argumentValue.m_eType) )
   {
   case arguments::eTypeNumberBool:
      return gd::variant_view(value.b);
      break;
   case arguments::eTypeNumberInt8:
      return gd::variant_view(value.v_int8);
      break;
   case arguments::eTypeNumberUInt8:
      return gd::variant_view(value.v_uint8);
      break;
   case arguments::eTypeNumberInt16:
      return gd::variant_view(value.v_int16);
      break;
   case arguments::eTypeNumberUInt16:
      return gd::variant_view(value.v_uint16);
      break;
   case arguments::eTypeNumberInt32:
      return gd::variant_view(value.v_int32);
      break;
   case arguments::eTypeNumberUInt32:
      return gd::variant_view(value.v_uint32);
      break;
   case arguments::eTypeNumberInt64:
      return gd::variant_view(value.v_int64);
      break;
   case arguments::eTypeNumberUInt64:
      return gd::variant_view(value.v_uint64);
      break;
   case arguments::eTypeNumberFloat:
      return gd::variant_view(value.f);
      break;
   case arguments::eTypeNumberDouble:
      return gd::variant_view(value.d);
      break;
   case arguments::eTypeNumberGuid:
      return gd::variant_view(value.pbsz, (size_t)argumentValue.length());
      break;
   case arguments::eTypeNumberString:
      return gd::variant_view(value.pbsz, (size_t)argumentValue.length());
      break;
   case arguments::eTypeNumberUtf8String:
      return gd::variant_view( variant_type::utf8( value.pbsz, (size_t)argumentValue.length() ) );
      break;
   case arguments::eTypeNumberWString:
      return gd::variant_view(value.pwsz, (size_t)argumentValue.length());
      break;
   default:
      assert(false);
   }

   return variant_view();
}


arguments::argument arguments::get_argument_s(const gd::variant& variantValue)
{
   switch( variantValue.type_number() )
   {
   case variant_type::eTypeNumberBool:
      return arguments::argument( (bool)variantValue );
      break;
   case variant_type::eTypeNumberInt8:
      return arguments::argument((int8_t)variantValue);
      break;
   case variant_type::eTypeNumberInt16:
      return arguments::argument((int16_t)variantValue);
      break;
   case variant_type::eTypeNumberInt32:
      return arguments::argument((int32_t)variantValue);
      break;
   case variant_type::eTypeNumberInt64:
      return arguments::argument((int64_t)variantValue);
      break;
   case variant_type::eTypeNumberUInt8:
      return arguments::argument((uint8_t)variantValue);
      break;
   case variant_type::eTypeNumberUInt16:
      return arguments::argument((uint16_t)variantValue);
      break;
   case variant_type::eTypeNumberUInt32:
      return arguments::argument((uint32_t)variantValue);
      break;
   case variant_type::eTypeNumberUInt64:
      return arguments::argument((uint64_t)variantValue);
      break;
   case variant_type::eTypeNumberFloat:
      return arguments::argument((float)variantValue);
      break;
   case variant_type::eTypeNumberDouble:
      return arguments::argument((double)variantValue);
      break;
   case variant_type::eTypeNumberString:
   case variant_type::eTypeNumberUtf8String:
      return arguments::argument((const char*)variantValue);
      break;
   case variant_type::eTypeNumberWString:
      return arguments::argument((const wchar_t*)variantValue);
      break;
   }

   return arguments::argument();
}

arguments::argument arguments::get_argument_s(const gd::variant_view& variantValue)
{
   switch( variantValue.type_number() )
   {
   case variant_type::eTypeNumberBool:
      return arguments::argument((bool)variantValue);
      break;
   case variant_type::eTypeNumberInt8:
      return arguments::argument((int8_t)variantValue);
      break;
   case variant_type::eTypeNumberInt16:
      return arguments::argument((int16_t)variantValue);
      break;
   case variant_type::eTypeNumberInt32:
      return arguments::argument((int32_t)variantValue);
      break;
   case variant_type::eTypeNumberInt64:
      return arguments::argument((int64_t)variantValue);
      break;
   case variant_type::eTypeNumberUInt8:
      return arguments::argument((uint8_t)variantValue);
      break;
   case variant_type::eTypeNumberUInt16:
      return arguments::argument((uint16_t)variantValue);
      break;
   case variant_type::eTypeNumberUInt32:
      return arguments::argument((uint32_t)variantValue);
      break;
   case variant_type::eTypeNumberUInt64:
      return arguments::argument((uint64_t)variantValue);
      break;
   case variant_type::eTypeNumberFloat:
      return arguments::argument((float)variantValue);
      break;
   case variant_type::eTypeNumberDouble:
      return arguments::argument((double)variantValue);
      break;
   case variant_type::eTypeNumberString:
      return arguments::argument((const char*)variantValue);
      break;
   case variant_type::eTypeNumberUtf8String:
      return arguments::argument(eTypeNumberUtf8String, (const char*)variantValue);
      break;
   case variant_type::eTypeNumberWString:
      return arguments::argument((const wchar_t*)variantValue);
      break;
   }

   return arguments::argument();
}



} // namespace gd::argument

