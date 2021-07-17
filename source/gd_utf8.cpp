#include <cassert>
#include <stdint.h>
#include <stdexcept>
#include <initializer_list>
#include "gd_utf8.hpp"

namespace gd { 
   namespace utf8 {

      const uint8_t pNeededByteCount[0x100] =
      {
          1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x00-0x0F */
          1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x10-0x1F */
          1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x20-0x2F */
          1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x30-0x3F */
          1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x40-0x4F */
          1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x50-0x5F */
          1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x60-0x6F */
          1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x70-0x7F */
          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x80-0x8F */
          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x90-0x9F */
          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xA0-0xAF */
          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xB0-0xBF */
          0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* 0xC0-0xCF */
          2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* 0xD0-0xDF */
          3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, /* 0xE0-0xEF */
          4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0, /* 0xF0-0xFF */
      };

      void normalize( uint32_t uCharacter, char8_t& value )
      {
         if( uCharacter < 0x80 ) value = static_cast<uint8_t>(uCharacter);
         else if( ((uCharacter >> 8) & 0xc0) == 0xc0 )
         {
            value = static_cast<uint8_t>(uCharacter >> 8) & 0x1f;
            value |= static_cast<uint8_t>(uCharacter) & 0x3f;
            
         }
         throw std::runtime_error("invalid convert  (operation = normalize)");
      }


      uint32_t character(const uint8_t* pubszCharacter)
      {
         switch( pNeededByteCount[ *pubszCharacter ] )
         {
         case 0:
            throw std::runtime_error("invalid UTF-8  (operation = character)");
         case 1:
            return static_cast<uint32_t>(*pubszCharacter);
         case 2:
            return static_cast<uint32_t>( ((0x1f & pubszCharacter[0]) << 6) | (0x3f & pubszCharacter[1]) );
         case 3:
            return static_cast<uint32_t>( ((0x0f & pubszCharacter[0]) << 12) | ((0x3f & pubszCharacter[1]) << 6) | (0x3f & pubszCharacter[2]) );
         case 4:
            return static_cast<uint32_t>( ((0x07 & pubszCharacter[0]) << 18) | ((0x3f & pubszCharacter[1]) << 12) | ((0x3f & pubszCharacter[2]) << 6) | (0x3f & pubszCharacter[3]) );
         }
         return 0;
      }

      /**
       * @brief count utf8 characters in buffer
       * @param pubszText pointer to buffer with text where characters are counted
       * @return number of utf8 characters in buffer
      */
      std::pair<uint32_t, const uint8_t*> count( const uint8_t* pubszText )
      {
         uint32_t uCount = 0; // counted characters in buffer
         const uint8_t* pubszPosition = pubszText;
         while( *pubszPosition != '\0' )
         {                                                                    assert( pNeededByteCount[*pubszPosition] != 0 );
            pubszPosition += pNeededByteCount[*pubszPosition];
            uCount++;

         }

         return std::pair<uint32_t, const uint8_t*>(uCount, pubszPosition);
      }

      /**
       * @brief count utf8 characters in buffer
       * @param pubszText pointer to buffer with text where characters are counted
       * @param pubszEnd pointer to end of buffer
       * @return number of utf8 characters in buffer
      */
      std::pair<uint32_t, const uint8_t*> count( const uint8_t* pubszText, const uint8_t* pubszEnd )
      {                                                                        assert( pubszText < pubszEnd ); assert( pubszEnd - pubszText < 0x00100000 );
         uint32_t uCount = 0; // counted characters in buffer
         const uint8_t* pubszPosition = pubszText;
         while( pubszPosition < pubszEnd )
         {
            pubszPosition += pNeededByteCount[*pubszPosition];
            uCount++;

         }

         return std::pair<uint32_t, const uint8_t*>(uCount, pubszPosition);
      }


      /**
       * @brief get number of bytes needed in buffer to store utf8 for character
       * @param uCharacter character  size is calculated for
       * @return 
      */
      uint32_t size( uint8_t uCharacter ) 
      {
         if( uCharacter < 0x80 ) return 1;  
         return 2;
      }

      uint32_t size(uint16_t uCharacter)
      {
         if(uCharacter < 0x80) return 1;
         else if(uCharacter < 0x800) return 2;
         else if(uCharacter < 0x10000) return 3;
         return 4;
      }


      uint32_t size( const char* pbszText, uint32_t uLength )
      {
         uint32_t uSize = 0;
         const char* pbszEnd = pbszText + uLength;
         while( pbszText < pbszEnd )
         {
            uSize += static_cast<uint8_t>(*pbszText) < 0x80 ? 1 : 2;
            pbszText++;
         }
         return uSize;
      }

      uint32_t convert( uint8_t uCharacter, uint8_t* pbszTo )
      {
         if( uCharacter < 0x80 )
         {
            *pbszTo = static_cast<uint8_t>(uCharacter);
            return 1;
         }
         else
         {
            pbszTo[0] = static_cast<uint8_t>( 0xc0 | ((uCharacter >> 6) & 0x1f) );
            pbszTo[1] = static_cast<uint8_t>( 0x80 | (uCharacter & 0x3f) );
            return 2;
         }
         return 0;
      }



      /**
       * @brief Convert character to utf8
       * @param uCharacter character that is converted
       * @param pbszTo pointer to buffer that gets character information formated as utf8
       * @return buffer size used to store character in utf8
       */
      uint32_t convert( uint16_t uCharacter, uint8_t* pbszTo )
      {
         if( uCharacter < 0x80 )
         {
            *pbszTo = static_cast<uint8_t>(uCharacter);
            return 1;
         }
         else if( uCharacter < 0x800 )
         {
            pbszTo[0] = static_cast<uint8_t>( 0xc0 | ((uCharacter >> 6) & 0x1f) );
            pbszTo[1] = static_cast<uint8_t>( 0x80 | (uCharacter & 0x3f) );
            return 2;
         }
         return 0;
      }


      /**
       * @brief Convert character to utf8
       * @param uCharacter character that is converted
       * @param pbszTo pointer to buffer that gets character information formated as utf8
       * @return buffer size used to store character in utf8
       */
      uint32_t convert( uint32_t uCharacter, uint8_t* pbszTo )
      {
         if( uCharacter < 0x80 )
         {
            *pbszTo = static_cast<uint8_t>(uCharacter);
            return 1;
         }
         else if( uCharacter < 0x800 )
         {
            pbszTo[0] = static_cast<uint8_t>( 0xc0 | ((uCharacter >> 6) & 0x1f) );
            pbszTo[1] = static_cast<uint8_t>( 0x80 | (uCharacter & 0x3f) );
            return 2;
         }
         else if( uCharacter < 0x10000 )
         {
            pbszTo[0] = static_cast<uint8_t>( 0xe0 | ((uCharacter >> 12) & 0x0f) );
            pbszTo[1] = static_cast<uint8_t>( 0x80 | ((uCharacter >> 6) & 0x3f) );
            pbszTo[2] = static_cast<uint8_t>( 0x80 | (uCharacter & 0x3f) );
            return 3;
         }
         else
         {
            pbszTo[0] = static_cast<uint8_t>( 0xf0 | ((uCharacter >> 18) & 0x07) );
            pbszTo[1] = static_cast<uint8_t>( 0x80 | ((uCharacter >> 12) & 0x3f) );
            pbszTo[2] = static_cast<uint8_t>( 0x80 | ((uCharacter >> 6) & 0x3f) );
            pbszTo[2] = static_cast<uint8_t>( 0x80 | (uCharacter & 0x3f) );
            return 4;
         }
         return 0;
      }

      /**
       * @brief convert utf16 to utf8
       * @param pwszUtf16 pointer to utf16 string
       * @param pbszUtf8 pointer to buffer where converted characters are stored
       * @return std::tuple<bool, const char16_t*, char8_t*> true if succeded, false if not. also returns positions where converted pointers ended
      */
      std::tuple<bool, const char16_t*, char8_t*> convert_utf16_to_uft8(const char16_t* pwszUtf16, char8_t* pbszUtf8)
      {
         const char16_t* pwszPosition = pwszUtf16;
         while(*pwszPosition)
         {
            uint32_t uCharacter = gd::utf16::character(pwszPosition);
            uint32_t uSize = size( static_cast<uint16_t>( *pwszPosition ) );
            pwszPosition += uSize;
            uSize = convert(uCharacter, pbszUtf8);
            pbszUtf8 += uSize;
         }

         *pbszUtf8 = '\0';

         return { true, pwszPosition, pbszUtf8 };
      }


      /**
       * @brief convert ascii text to utf8   
       * @param pbszFrom pointer to ascii text that will be converted
       * @param pbszTo pointer to buffer where utf8 characters are stored
       * @return {std::pair<bool, const uint8_t*>} true or false if all characters have been converted, and pointer to position where conversion ended
      */
      std::pair<bool, const uint8_t*> convert_ascii(const uint8_t* pbszFrom, uint8_t* pbszTo )
      {
         const uint8_t* pbszPosition = pbszFrom;
         uint8_t* pbszInsert = pbszTo;

         while(*pbszPosition)
         {
            if(*pbszPosition < 0x80) { *pbszInsert = *pbszPosition; pbszInsert++; }
            else
            {
               pbszInsert[0] = (0xc0 | ((*pbszPosition >> 6) & 0x1f));
               pbszInsert[1] = (0x80 | (*pbszPosition & 0x3f));
               pbszInsert += 2;
            }

            pbszPosition++;
         }

         *pbszInsert = '\0';

         return std::make_pair(true, pbszPosition);
      }


      /**
       * @brief convert ascii text to utf8   
       * @param pbszFrom pointer to ascii text that will be converted
       * @param pbszTo pointer to buffer where utf8 characters are stored
       * @param pbszEnd end of buffer
       * @return {std::pair<bool, const uint8_t*>} true or false if all characters have been converted, and pointer to position where conversion ended
      */
      std::pair<bool, const uint8_t*> convert_ascii(const uint8_t* pbszFrom, uint8_t* pbszTo, const uint8_t* pbszEnd)
      {
         const uint8_t* pbszPosition = pbszFrom;
         uint8_t* pbszInsert = pbszTo;

         pbszEnd--; // reserve one character in buffer, some ascii characters will need two bytes
         while(pbszInsert < pbszEnd && *pbszPosition)
         {
            if(*pbszPosition < 0x80) { *pbszInsert = *pbszPosition; pbszInsert++; }
            else
            {
               pbszInsert[0] = (0xc0 | ((*pbszPosition >> 6) & 0x1f));
               pbszInsert[1] = (0x80 | (*pbszPosition & 0x3f));
               pbszInsert += 2;
            }

            pbszPosition++;
         }

         if(pbszInsert < pbszEnd) *pbszInsert = '\0';

         return std::make_pair(true, pbszPosition);
      }

      /**
       * @brief convert unicode text to utf8   
       * @param pwszFrom pointer to unicode text that will be converted
       * @param pbszTo pointer to buffer where utf8 characters are stored
       * @param pbszEnd end of buffer
       * @return {std::pair<bool, const uint8_t*>} true or false if all characters have been converted, and pointer to position where conversion ended
      */
      std::pair<bool, const uint16_t*> convert_unicode(const uint16_t* pwszFrom, uint8_t* pbszTo, const uint8_t* pbszEnd)
      {
         const uint16_t* pwszPosition = pwszFrom;
         uint8_t* pbszInsert = pbszTo;

         pbszEnd--; // reserve one character in buffer, some ascii characters will need two bytes
         while(pbszInsert < pbszEnd && *pwszPosition)
         {
            if(*pwszPosition < 0x80) { *pbszInsert = static_cast<uint8_t>(*pwszPosition); pbszInsert++; }
            else if( (*pwszPosition & 0xf800) == 0 )
            {
               pbszInsert[0] = (0xc0 | (*pwszPosition >> 6));
               pbszInsert[1] = (0x80 | (*pwszPosition & 0x3f));
               pbszInsert += 2;
            }
            else 
            {
               pbszInsert[0] = (0xE0 | (*pwszPosition >> 12));
               pbszInsert[1] = (0x80 | ((*pwszPosition >> 6) & 0x3F));
               pbszInsert[2] = (0x80 | (*pwszPosition & 0x3F));
               pbszInsert += 3;
            }

            pwszPosition++;
         }

         if(pbszInsert < pbszEnd) *pbszInsert = '\0';

         return std::make_pair(true, pwszPosition);
      }

   }  
}


namespace gd {
   namespace utf8 {
      namespace move {

         /**
          * @brief move to next character in utf8 buffer
          * @param pubszPosition pointer to position where movement starts
          * @return {const uint8_t*} new position
         */
         const uint8_t* next(const uint8_t* pubszPosition)
         {
            if(*pubszPosition != '\0')
            {
               if((*pubszPosition & 0x80) == 0)          return pubszPosition + 1;
               else if((*pubszPosition & 0xc0) == 0xc0)  return pubszPosition + 2;
               else if((*pubszPosition & 0xf0) == 0xe0)  return pubszPosition + 3;
               else if((*pubszPosition & 0xf8) == 0xf0)  return pubszPosition + 4;
               else throw std::runtime_error("invalid UTF-8 (operation = next)");
            }

            return pubszPosition;
         }

         /**
          * @brief Move forward in text
          * @param pubszPosition pointer to current position in text
          * @param uCount number of characters to move
          * @return pointer to new position
         */
         const uint8_t* next( const uint8_t* pubszPosition, uint32_t uCount )
         {
            while(uCount-- > 0) pubszPosition = next( pubszPosition );
            return pubszPosition;
         }

         /**
          * @brief move pointer in utf-8 text
          * @param ppubszPosition pointer to pointer that is moved in text
          * @param uCount how much movement
          * @return {bool} true if pointer was moved, false if not
         */
         bool next(const uint8_t** ppubszPosition, uint32_t uCount) {
            auto pubszPosition = *ppubszPosition;
            
            while(uCount-- > 0)
            {
               auto pubszSave = pubszPosition;
               pubszPosition = next(pubszPosition);
               if(pubszSave == pubszPosition) return false;
               pubszSave = pubszPosition;
            }

            *ppubszPosition = pubszPosition;
            return true;

         }



         /**
          * @brief Move to previous character in utf8 buffer
          * @param pubszPosition pointer to position where movement starts
          * @return {const uint8_t*} new position
         */
         const uint8_t* previous(const uint8_t* pubszPosition)
         {
            auto pubszTest = pubszPosition - 1;
            if((*pubszTest & 0x80) == 0)                return pubszPosition - 1;
            else if((*(pubszTest - 1) & 0xc0) == 0xc0)  return pubszPosition - 2;
            else if((*(pubszTest - 2) & 0xf0) == 0xe0)  return pubszPosition - 3;
            else if((*(pubszTest - 3) & 0xf8) == 0xf0)  return pubszPosition - 4;
            else throw std::runtime_error("invalid UTF-8 (operation = previous)");

            return pubszPosition;
         }

         /**
          * @brief move backwards in text
          * @param pubszPosition pointer to current position in text 
          * @param uCount number of characters to move
          * @return {const uint8_t*} pointer to new position
         */
         const uint8_t* previous(const uint8_t* pubszPosition, uint32_t uCount)
         {
            while(uCount-- > 0) pubszPosition = previous(pubszPosition);
            return pubszPosition;
         }


         /**
          * @brief move to end of utf8 text
          * @param pubszPosition 
          * @return {const uint8_t*} pointer to utf8 text end
         */
         const uint8_t* end(const uint8_t* pubszPosition)
         {
            while(*pubszPosition != '\0') pubszPosition++;
            return pubszPosition;
         }

         const uint8_t* find( const uint8_t* pubszPosition, uint32_t uCharacter )
         {
            uint8_t puBuffer[SIZE32_MAX_UTF_SIZE + 1];
            auto uLength = gd::utf8::convert( uCharacter, puBuffer );
            puBuffer[uLength] = '\0';

            return find_character( pubszPosition, puBuffer, uLength );
         }

         const uint8_t* find( const uint8_t* pubszPosition, const uint8_t* pubszEnd, uint32_t uCharacter )
         {
            uint8_t puBuffer[SIZE32_MAX_UTF_SIZE + 1];
            auto uLength = gd::utf8::convert( uCharacter, puBuffer );
            puBuffer[uLength] = '\0';

            return find_character( pubszPosition, pubszEnd, puBuffer, uLength );
         }

         const uint8_t* find_character( const uint8_t* pubszPosition, const uint8_t* pubszCharacter, uint32_t ulength ) 
         {                                                                                         assert( ulength < 6 );
            auto pubszFind = pubszPosition;
            while( *pubszFind != '\0' )
            {
               if( *pubszFind == *pubszCharacter )
               {
                  switch( ulength )
                  {
                     case 1: return pubszFind;
                     case 2: if( pubszFind[1] == pubszCharacter[1] ) return pubszFind;
                        break;
                     case 3: if( pubszFind[1] == pubszCharacter[1] && pubszFind[2] == pubszCharacter[2] ) return pubszFind;
                        break;
                     case 4: if( pubszFind[1] == pubszCharacter[1] && pubszFind[2] == pubszCharacter[2] && pubszFind[3] == pubszCharacter[3] ) return pubszFind;
                        break;
                     case 5: if( pubszFind[1] == pubszCharacter[1] && pubszFind[2] == pubszCharacter[2] && pubszFind[3] == pubszCharacter[3] && pubszFind[4] == pubszCharacter[4] ) return pubszFind;
                        break;
                     default: throw std::runtime_error("invalid UTF-8 (operation = find_character)");
                  }
               }

               pubszFind++;
            }

            return nullptr;
         }

         /**
          * @brief find string in text
          * @param pubszPosition text that string is searched for
          * @param pubszEnd end of text
          * @param pubszFind string to find
          * @param uSize length of string to find
          * @return 
         */
         const uint8_t* find( const uint8_t* pubszPosition, const uint8_t* pubszEnd, const uint8_t* pubszFind, uint32_t uSize )
         {
            pubszEnd -= uSize;
            uSize--;
            while( pubszPosition < pubszEnd )
            {
               if( *pubszPosition == *pubszFind )
               {
                  if( memcmp( pubszPosition + 1, pubszFind + 1, uSize ) == 0  ) return pubszPosition;
               }

               pubszPosition++;
            }

            return nullptr;
         }


         /**
          * @brief Find utf8 character sequence in buffer
          * @param pubszFind text to look for utf8 character
          * @param pubszEnd where to stop searching
          * @param pubszCharacter character to look for, remember that this need to be a utf8 sequence
          * @param uLength utf8 sequence length
          * @return position to found character or if not found return null
         */
         const uint8_t* find_character( const uint8_t* pubszFind, const uint8_t* pubszEnd, const uint8_t* pubszCharacter, uint32_t uLength ) 
         {                                                                                         assert( uLength < 6 );
            while( pubszFind < pubszEnd )
            {
               if( *pubszFind == *pubszCharacter )
               {
                  switch( uLength )
                  {
                     case 1: return pubszFind;
                     case 2: if( pubszFind[1] == pubszCharacter[1] ) return pubszFind;
                        break;
                     case 3: if( pubszFind[1] == pubszCharacter[1] && pubszFind[2] == pubszCharacter[2] ) return pubszFind;
                        break;
                     case 4: if( pubszFind[1] == pubszCharacter[1] && pubszFind[2] == pubszCharacter[2] && pubszFind[3] == pubszCharacter[3] ) return pubszFind;
                        break;
                     case 5: if( pubszFind[1] == pubszCharacter[1] && pubszFind[2] == pubszCharacter[2] && pubszFind[3] == pubszCharacter[3] && pubszFind[4] == pubszCharacter[4] ) return pubszFind;
                        break;
                     default: throw std::runtime_error("invalid UTF-8 (operation = find_character)");
                  }
               }

               pubszFind++;
            }

            return nullptr;
         }


         
      } // move
   } // utf8


   namespace utf16 {
      uint32_t character(const uint16_t* pubszCharacter)
      {
         if(*pubszCharacter < 0x80) return static_cast<uint32_t>(*pubszCharacter);
         else if(*pubszCharacter < 0x800) return static_cast<uint32_t>(((0x1f & pubszCharacter[0]) << 6) | (0x3f & pubszCharacter[1]));
         else if(*pubszCharacter < 0x10000) return static_cast<uint32_t>(((0x0f & pubszCharacter[0]) << 12) | ((0x3f & pubszCharacter[1]) << 6) | (0x3f & pubszCharacter[2]));
         else if(*pubszCharacter < 0x200000) return static_cast<uint32_t>(((0x07 & pubszCharacter[0]) << 18) | ((0x3f & pubszCharacter[1]) << 12) | ((0x3f & pubszCharacter[2]) << 6) | (0x3f & pubszCharacter[3]));
         else throw std::runtime_error("invalid UTF-8  (operation = character)");
         return 0;
      }

      uint32_t size(uint16_t uCharacter)
      {
         if(uCharacter < 0x80) return 1;
         else if(uCharacter < 0x800) return 2;
         else if(uCharacter < 0x10000) return 3;
         return 4;
      }

   }
} // gd



/*

    template <typename octet_iterator>
    inline typename std::iterator_traits<octet_iterator>::difference_type
    sequence_length(octet_iterator lead_it)
    {
        uint8_t lead = utf8::internal::mask8(*lead_it);
        if (lead < 0x80)
            return 1;
        else if ((lead >> 5) == 0x6)
            return 2;
        else if ((lead >> 4) == 0xe)
            return 3;
        else if ((lead >> 3) == 0x1e)
            return 4;
        else
            return 0;
    }


size_t utf8codepointsize(utf8_int32_t chr) {
  if (0 == ((utf8_int32_t)0xffffff80 & chr)) {
    return 1;
  } else if (0 == ((utf8_int32_t)0xfffff800 & chr)) {
    return 2;
  } else if (0 == ((utf8_int32_t)0xffff0000 & chr)) {
    return 3;
  } else { // if (0 == ((int)0xffe00000 & chr)) {
    return 4;
  }
}



*/