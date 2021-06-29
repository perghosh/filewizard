#include <stdint.h>
#include <stdexcept>
#include "gd_utf.hpp"

namespace gd { 
   namespace utf8 {

      uint32_t character(const uint8_t* pubszText)
      {
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
            if((*pubszPosition & 0x80) == 0)          pubszPosition += 1;
            else if((*pubszPosition & 0xc0) == 0xc0)  pubszPosition += 2;
            else if((*pubszPosition & 0xf0) == 0xe0)  pubszPosition += 3;
            else if((*pubszPosition & 0xf8) == 0xf0)  pubszPosition += 4;
            else throw std::runtime_error("invalid UTF-8  (operation = count)");

            uCount++;
         }

         return std::pair<std::size_t, const uint8_t*>(uCount, pubszPosition);
      }



      std::pair<bool, const uint8_t*> convert_ascii(const uint8_t* pbszFrom, uint8_t* pbszTo, const uint8_t* pbszEnd)
      {
         const uint8_t* pbszPosition = pbszFrom;
         uint8_t* pbszInsert = pbszTo;


         while(pbszInsert < pbszEnd && *pbszPosition)
         {
            if(*pbszPosition < 0x80) { *pbszInsert = *pbszPosition; pbszInsert++; }
            else
            {
               *pbszInsert = (uint16_t)((0xc0 | ((*pbszPosition >> 6) & 0x1f)) << 8) | (0x80 | (*pbszPosition & 0x3f));
               pbszInsert += 2;
            }

            pbszPosition++;
         }

         if(pbszInsert < pbszEnd) *pbszInsert = '\0';

         return std::make_pair(true, pbszPosition);
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