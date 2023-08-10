#include "gd_types.h"
#include "gd_utf8_2.h"

_GD_UTF8_BEGIN

/**
 * @brief 
 * 0x01 = integer
 * 0x02 = decimal
*/
enum enumNumberType : uint8_t { eString = 0x00, eInteger = 0x01, eDecimal = 0x02, };
static const uint8_t pIsNumber_s[0x100] =
{
 //0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x00-0x0F */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x10-0x1F */
   0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,0, /* 0x20-0x2F */
   1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0, /* 0x30-0x3F */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x40-0x4F */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x50-0x5F */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x60-0x6F */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x70-0x7F */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x80-0x8F */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x90-0x9F */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xA0-0xAF */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xB0-0xBF */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xC0-0xCF */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xD0-0xDF */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xE0-0xEF */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xF0-0xFF */
};


static uint8_t get_number_type_s( const std::string_view& stringCheckType )
{
   uint8_t uType = 0;
   for( auto it = std::begin( stringCheckType ), itEnd = std::end( stringCheckType ); it != itEnd; it++ )
   {
      uint8_t uCharType = pIsNumber_s[*it];
      if( uCharType == 0 ) return 0;                                           // found 0 number that means that this has to be a string
      uType |= uCharType;
   }
   return uType;
}


/** -------------------------------------------------------------------
 * @brief Split string into multiple strings and store them in vector, string is split where char is found
 * @param stringText text to split into multiple parts
 * @param stringSplit character sequence that marks where to split text
 * @param vectorPart vector where strings are stored
*/
void split( const std::string_view& stringText, const std::string_view& stringSplit, std::vector<gd::variant>& vectorPart, gd::variant_type::enumType eDefaultType )
{
   if( stringSplit.empty() == true ) return;

   gd::variant_type::enumType eType = eDefaultType;
   std::string stringPart;                // Store string parts added to vector
   auto uLength = stringSplit.length();   // Split string length
   const uint8_t* pubszSplitWith = reinterpret_cast<const uint8_t*>( stringSplit.data() ); // help compiler to optimize ?

   const uint8_t* pubszPosition = reinterpret_cast<const uint8_t*>( stringText.data() ); // start of text
   const uint8_t* pubszTextEnd = reinterpret_cast<const uint8_t*>( stringText.data() + stringText.length() ); // end of text

   while( pubszPosition != pubszTextEnd )
   {                                                                                               assert( pubszPosition < pubszTextEnd ); assert( *pubszPosition != 0 );
      // ## Compare if split text sequence is found, then add value to vector based on found type
      if( *pubszPosition == *pubszSplitWith && memcmp( ( void* )pubszPosition, ( void* )pubszSplitWith, uLength ) == 0 )
      {
         uint8_t uType = get_number_type_s( stringPart );
         if( uType == eString ) vectorPart.emplace_back( gd::variant( stringPart, eDefaultType ) );
         else
         {
            gd::variant value_;
            if( eDecimal & uType ) 
            {
               double dValue = std::stold( stringPart );
               value_ = dValue;
            }
            else
            {
               int64_t iValue = std::atoll( stringPart.c_str() );
               value_ = iValue;
            }

            vectorPart.emplace_back( value_ );
         }

         stringPart.clear();
         pubszPosition += uLength;                                             // Move past split sequence
         eType = eDefaultType;                                                 // Set to default type for new value
         continue;
      }

      stringPart += (char)*pubszPosition;
      pubszPosition++;                                                         // next character
   }

   // add final part
   if( stringPart.empty() == false )
   {
      uint8_t uType = get_number_type_s( stringPart );
      if( uType == eString ) vectorPart.emplace_back( gd::variant( stringPart, eDefaultType ) );
      else
      {
         if( eDecimal & uType ) vectorPart.emplace_back( gd::variant( (double)std::stold( stringPart ) ) );
         else                   vectorPart.emplace_back( gd::variant( (int64_t)std::atoll( stringPart.c_str() ) ) );
      }
   }
}


_GD_UTF8_END