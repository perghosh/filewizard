#include <stdint.h>
#include <stdexcept>
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
         /*
         if(*pubszText < 0x80) return static_cast<uint32_t>(*pubszText);
         else if((*pubszText & 0xe0) == 0xc0)
         {
            return static_cast<uint32_t>( ((0x1f & pubszText[0]) << 6) | (0x3f & pubszText[1]) );
         }
         else if((*pubszText & 0xf0) == 0xe0)
         {
            return static_cast<uint32_t>( ((0x0f & pubszText[0]) << 12) | ((0x3f & pubszText[1]) << 6) | (0x3f & pubszText[2]) );
         }
         else if((*pubszText & 0xf8) == 0xf0)
         {
            return static_cast<uint32_t>( ((0x07 & pubszText[0]) << 18) | ((0x3f & pubszText[1]) << 12) | ((0x3f & pubszText[2]) << 6) | (0x3f & pubszText[3]) );
         }

         throw std::runtime_error("invalid UTF-8  (operation = character)");
         */
      }

      /**
       * @brief count utf8 characters in buffer
       * @param pubszText pointer to buffer with text where characters are counted
       * @return 
      */
      std::pair<std::size_t, const uint8_t*> count( const uint8_t* pubszText )
      {
         std::size_t uCount = 0; // counted characters in buffer
         const uint8_t* pubszPosition = pubszText;
         while( *pubszPosition != '\0' )
         {
            pubszPosition += pNeededByteCount[*pubszPosition];
            uCount++;

            /*
            if((*pubszPosition & 0x80) == 0)          pubszPosition += 1;
            else if((*pubszPosition & 0xc0) == 0xc0)  pubszPosition += 2;
            else if((*pubszPosition & 0xf0) == 0xe0)  pubszPosition += 3;
            else if((*pubszPosition & 0xf8) == 0xf0)  pubszPosition += 4;
            else throw std::runtime_error("invalid UTF-8  (operation = count)");
            */

         }

         return std::pair<std::size_t, const uint8_t*>(uCount, pubszPosition);
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
         const uint8_t* next( const uint8_t* pubszPosition, std::size_t uCount )
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
         bool next(const uint8_t** ppubszPosition, std::size_t uCount) {
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
         const uint8_t* previous(const uint8_t* pubszPosition, std::size_t uCount)
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
      } // move
   } // utf8
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