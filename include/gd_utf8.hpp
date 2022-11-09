﻿/*
## Overview
| Name | Description |
| - | - |
| character | get one character from buffer as uint32_t |
| convert | convert character into bytes and place it in specified buffer |
| count | count utf8 characters in buffer |
| size | size needed in bytes to store characters |


*/

#pragma once
#include <cassert>
#include <stdint.h>
#include <cstring>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

/*


Links: https://github.com/nemtrif/utfcpp


| method | description |
| - | - |
| `character` | return character number |
| `convert` | convert character value and store it in buffer pointer points to what is also sent to method, returns length in bytes needed to insert utf8 character to buffer |

| `convert_utf16_to_uft8` | converts unicode to utf8 |
| `convert_ascii` | convert ascii characters to utf8 into byte buffer |
| `copy` | copy utf8 character from source buffer to target buffer |
| `size` | calculate number of bytes needed to store character in utf8 format, text sent to count needed characters for should not be in utf8 format |
| `` |  |



*/

namespace gd { 
   namespace utf8 {

      // tag dispatcher for utf8 if no support for char8_t
      struct utf8_tag {};

      constexpr uint32_t SIZE8_MAX_UTF_SIZE = 2;
      constexpr uint32_t SIZE16_MAX_UTF_SIZE = 3;
      constexpr uint32_t SIZE32_MAX_UTF_SIZE = 6;


      ///@{ 
      void normalize( uint32_t uCharacter, char& value );
      uint32_t character(const uint8_t* pubszText); // -------------------------------------------- character
      template <typename UTF8_TYPE>
      uint32_t character(const UTF8_TYPE* pbszText) { // ------------------------------------------ character
         static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
         return character(reinterpret_cast<const uint8_t*>(pbszText));
      };
      ///@}

      // ## is* methods

      /// check if size value is any `npos` value returned from string search methods that returns position or `npos` if not found
      template<typename POSITION>
      bool is_npos(const POSITION uPosition) {
         if constexpr( sizeof(POSITION) == sizeof(uint32_t) ) return static_cast<int32_t>(uPosition) < 0;
         else if constexpr( sizeof(POSITION) == sizeof(uint64_t) ) return static_cast<int64_t>(uPosition) < 0;
         //else static_assert(false, "invalid size type compare to npos");
         assert(false);
         return false;
      }

      template<typename POSITION>
      bool is_found(const POSITION uPosition) {
         return !is_npos(uPosition);
      }

      // ## `count` methods is used to count number of characters (`strlen` are wrappers)

      /**
       * Count number of characters in utf8 buffer
       */
      ///@{ 
      std::pair<uint32_t, const uint8_t*> count(const uint8_t* pubszText);                         // count utf8 characters
      inline std::pair<uint32_t, const uint8_t*> count(const std::string_view stringText) { 
         if( stringText.empty() == false ) return count( reinterpret_cast<const uint8_t*>(stringText.data()) );
         return { 0, nullptr };
      };
      template <typename UTF8_TYPE>
      std::pair<uint32_t, const uint8_t*> count(const UTF8_TYPE* pbszText) {                       // count utf8 characters
         static_assert( sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
         return count( reinterpret_cast<const uint8_t*>(pbszText) ); 
      };
      std::pair<uint32_t, const uint8_t*> count( const uint8_t* pubszText, const uint8_t* pubszEnd );
      template <typename UTF8_TYPE>
      std::pair<uint32_t, const uint8_t*> count(const UTF8_TYPE* pbszText, const UTF8_TYPE* pbszEnd) { // count utf8 characters
         static_assert( sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
         return count( reinterpret_cast<const uint8_t*>(pbszText), reinterpret_cast<const uint8_t*>(pbszEnd) ); 
      };
      inline uint32_t strlen(const uint8_t* pubszText) { return count(pubszText).first; };         // count utf8 characters
      inline uint32_t strlen(const std::string_view stringText) { return count( reinterpret_cast<const uint8_t*>(stringText.data()) ).first; };
      template <typename UTF8_TYPE>
      uint32_t strlen(const UTF8_TYPE* pbszText) {                                                 // count utf8 characters
         static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
         return count(reinterpret_cast<const uint8_t*>(pbszText)).first;
      };

      /// ## `size` methods is used calculate needed size to store character as utf8 value

      uint32_t size( uint8_t ch ); // ------------------------------------------------------------- size
      uint32_t size( uint16_t ch ); // ------------------------------------------------------------ size
      uint32_t size( uint32_t ch ); // ------------------------------------------------------------ size
      uint32_t size( wchar_t ch ); // ------------------------------------------------------------- size
      /// count needed size to store char string as utf8 string
      inline uint32_t size( const char* pbsz ) { // ----------------------------------------------- size
         uint32_t uSize = 0;
         for( ; *pbsz != 0; pbsz++ ) uSize += size( static_cast<uint8_t>(*pbsz) );
         return uSize;
      }
      inline uint32_t size(const std::string_view& stringCountSize) { // -------------------------- size
         uint32_t uSize = 0;
         for( auto it : stringCountSize  ) uSize += size(static_cast<uint8_t>(it));
         return uSize;
      }
      inline uint32_t size(const wchar_t* pwsz) { // ---------------------------------------------- size
         uint32_t uSize = 0;
         for( ; *pwsz != 0; pwsz++ ) uSize += size(static_cast<wchar_t>(*pwsz));
         return uSize;
      }
      inline uint32_t size(const wchar_t* pwsz, const wchar_t* pwszEnd) { // ---------------------- size
         uint32_t uSize = 0;
         for( ; pwsz != pwszEnd; pwsz++ ) uSize += size(static_cast<wchar_t>(*pwsz));
         return uSize;
      }

      /// count needed size to store list of char values as utf8 string
      inline uint32_t size( std::initializer_list<char> listString ) { // ------------------------- size
         uint32_t uSize = 0;
         for( auto it : listString ) uSize += size( static_cast<uint8_t>(it) );
         return uSize;
      }

      ///@}

      ///@{ 
      /// Count number of utf8 bytes needed to store char text
      uint32_t size( const char* pbszText, uint32_t uLength ); // --------------------------------- size
      ///@}

      /// calculate size in bytes needed to store character
      uint32_t get_character_size( const uint8_t* pubszText ); // --------------------------------- get_character_size


      ///@{
      uint32_t convert(uint8_t uCharacter, uint8_t* pbszTo); // ----------------------------------- convert
      uint32_t convert(uint16_t uCharacter, uint8_t* pbszTo); // ---------------------------------- convert
      uint32_t convert(uint32_t uCharacter, uint8_t* pbszTo); // ---------------------------------- convert

#if defined(__cpp_char8_t)
      inline uint32_t convert(uint32_t uCharacter, char8_t* pbszTo) { return convert(uCharacter, reinterpret_cast<uint8_t*>(pbszTo)); }

      /// convert from utf16 to utf8. make sure that buffer is large enough to hold text
      std::tuple<bool, const char16_t*, char8_t*> convert_utf16_to_uft8(const char16_t* pwszUtf16, char8_t* pbszUtf8 );// convert
      template <typename UTF16_TYPE, typename UTF8_TYPE>
      std::tuple<bool, const UTF16_TYPE*, UTF8_TYPE*> convert_utf16_to_uft8(const UTF16_TYPE* pwszUtf16, UTF8_TYPE* pbszUtf8) {
         static_assert(sizeof(UTF16_TYPE) == 2, "Value isn't compatible with char16_t");
         static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with char8_t");
         auto _result = convert_utf16_to_uft8( reinterpret_cast<const char16_t*>(pwszUtf16), reinterpret_cast<char8_t*>(pbszUtf8) );
         return std::tuple<bool, const UTF16_TYPE*, UTF8_TYPE*>(
            std::get<0>(_result), 
            reinterpret_cast<const UTF16_TYPE*>(std::get<1>(_result)), 
            reinterpret_cast<UTF8_TYPE*>(std::get<2>(_result))
         );
      }
#endif
      std::tuple<bool, const wchar_t*, char*> convert_utf16_to_uft8(const wchar_t* pwszUtf16, char* pbszUtf8, utf8_tag );// convert

      /// Convert Unicode buffer to stl string where text is stored in utf8 format
      std::tuple<bool, const uint16_t*> convert_utf16_to_uft8(const uint16_t* pwszUtf16, std::string& stringUtf8);

      /// convert utf8 buffer to stl wstring
      std::tuple<bool, const uint8_t*> convert_utf8_to_uft16(const uint8_t* pbszUtf8, std::wstring& stringUtf16);

      std::pair<bool, const uint8_t*> convert_ascii(const uint8_t* pbszFrom, uint8_t* pbszTo); // - convert_ascii
      template <typename UTF8_TYPE, typename TYPE>
      std::pair<bool, const uint8_t*> convert_ascii(const UTF8_TYPE* pbszFrom, TYPE* pbszTo) {
         return convert_ascii(reinterpret_cast<const uint8_t*>(pbszFrom), reinterpret_cast<uint8_t*>(pbszTo));
      }
      std::pair<bool, const uint8_t*> convert_ascii(const uint8_t* pbszFrom, uint8_t* pbszTo, const uint8_t* pbszEnd);
      template <typename UTF8_TYPE, typename TYPE>
      std::pair<bool, const uint8_t*> convert_ascii(const UTF8_TYPE* pbszFrom, TYPE* pbszTo, const TYPE* pbszEnd) {
         return convert_ascii(reinterpret_cast<const uint8_t*>(pbszFrom), reinterpret_cast<uint8_t*>(pbszTo), reinterpret_cast<const uint8_t*>(pbszEnd));
      }

      /// convert unicode text to utf8
      std::pair<bool, const uint16_t*> convert_unicode(const uint16_t* pbszFrom, uint8_t* pbszTo, const uint8_t* pbszEnd);
      template <typename UNICODE_TYPE, typename UTF8_TYPE>
      std::pair<bool, const uint16_t*> convert_unicode(const UNICODE_TYPE* pwszFrom, UTF8_TYPE* pbszTo, UTF8_TYPE* pbszEnd) {
         static_assert(sizeof(UNICODE_TYPE) == 2, "Value isn't compatible with uint16_t");
         static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
         return convert_unicode(reinterpret_cast<const uint16_t*>(pwszFrom), reinterpret_cast<uint8_t*>(pbszTo), reinterpret_cast<const uint8_t*>(pbszEnd));
      }
      ///@}

      /// Converts unicode text to ascii, characters that can't be copied (above ascii codes) are skipped
      std::pair<const wchar_t*, const char*> convert_unicode_to_ascii(const wchar_t* pwszFrom, char* pbszTo, const char* pbszEnd);
      std::string convert_unicode_to_ascii( std::wstring_view stringUnicode );

      /// Converts ascii text to unicode
      std::pair<const char*, const wchar_t*> convert_ascii_to_unicode(const char* pszFrom, wchar_t* pwszTo, const wchar_t* pwszEnd);
      std::wstring convert_ascii_to_unicode(std::string_view stringAscii);


      // ## convert number to text

      uint8_t* itoa( int32_t iNumber, uint8_t* pbszTo );
      uint8_t* utoa( uint32_t uNumber, uint8_t* pbszTo );
      uint8_t* itoa( int64_t iNumber, uint8_t* pbszTo );
      uint8_t* utoa( uint64_t uNumber, uint8_t* pbszTo );
      [[nodiscard]] inline std::string itoa_to_string(int32_t iNumber) { uint8_t p[12]; itoa(iNumber, p); return std::string((char*)p); }
      [[nodiscard]] inline std::string utoa_to_string(uint32_t uNumber) { uint8_t p[12]; utoa(uNumber, p); return std::string((char*)p); }
      [[nodiscard]] inline std::string itoa_to_string(int64_t iNumber) { uint8_t p[23]; itoa(iNumber, p); return std::string((char*)p); }
      [[nodiscard]] inline std::string utoa_to_string(uint64_t uNumber) { uint8_t p[23]; utoa(uNumber, p); return std::string((char*)p); }


      std::intptr_t distance(const uint8_t* p1, const uint8_t* p2);


      ///@{
      uint8_t* copy_character( uint8_t* puCopyTo, const uint8_t* puCopyFrom );
      ///@}


      /**
       * @brief move operation for pointer in utf-8 buffer
      */
      namespace move {

         /** \name NEXT OPERATIONS
         moves to next...
*move operations*
| name | description |
| - | - |
| `advance` | advance specified number of characters (if negative move backwards |
| `end` | moves to end of text |
| `find` | tries to find character within the selected range of characters |
| `find_character` | tries to find utf8 character within the selected range of characters |
| `next` | moves to next character or to specified amount of characters|
| `next_space` | moves to first space character (space = space, tab or carriage return) |
| `next_non_space` | moves to next character |
| `previous` | move backwards |
         *///@{ 

         // move to next utf8 character in buffer pointer points at
         const uint8_t* next(const uint8_t* pubszPosition); // ------------------------------------ next                                       
         // move to next utf8 character in buffer pointer points at
         inline uint8_t* next(uint8_t* pubszPosition) { return const_cast<uint8_t*>( next( static_cast<const uint8_t*>(pubszPosition) ) ); } // next
         // move to next utf8 character in buffer pointer points at
         template <typename UTF8_TYPE>
         const UTF8_TYPE* next(const UTF8_TYPE* pubszPosition) { // ------------------------------- next
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>( next( reinterpret_cast<const uint8_t*>(pubszPosition) ) );
         };
         // move to next utf8 character in buffer pointer points at
         template <typename UTF8_TYPE>
         UTF8_TYPE* next(UTF8_TYPE* pubszPosition) { // ------------------------------------------- next
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<UTF8_TYPE*>( next( reinterpret_cast<uint8_t*>(pubszPosition) ) );
         };

         // move specified count in buffer pointer points at
         const uint8_t* next(const uint8_t* pubszPosition, uint32_t uCount ); // ------------------ next
         inline uint8_t* next(uint8_t* pubszPosition, uint32_t uCount ) { return const_cast<uint8_t*>( next( static_cast<const uint8_t*>(pubszPosition), uCount ) ); } // move specified count in buffer pointer points at
         template <typename UTF8_TYPE>
         const UTF8_TYPE* next(const UTF8_TYPE* pubszPosition, uint32_t uCount ) {                 // move specified count in buffer pointer points at
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>( next(reinterpret_cast<const uint8_t*>(pubszPosition), uCount) );
         };

         // move pointer to next utf8 character in buffer pointer to pointer points at, return true or false if it succeeded
         inline bool next(const uint8_t** ppubszPosition) { // ------------------------------------ next
            const uint8_t* p = next(*ppubszPosition);
            if(p != *ppubszPosition) { ppubszPosition = &p; return true; }
            return false;
         }
         // move pointer to next utf8 character in buffer pointer to pointer points at, return true or false if it succeeded
         template <typename UTF8_TYPE>
         bool next(const UTF8_TYPE** ppubszPosition) { // ----------------------------------------- next
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>(next(reinterpret_cast<const uint8_t**>(ppubszPosition)));
         };
         bool next(const uint8_t** ppubszPosition, uint32_t uCount); // --------------------------- next
         template <typename UTF8_TYPE>
         bool next(const UTF8_TYPE** ppubszPosition, uint32_t uCount) { // ------------------------ next
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>(next(reinterpret_cast<const uint8_t**>(ppubszPosition), uCount));
         };

         // move pointer to next space character (space, carriage return, tab or new line)
         const uint8_t* next_space(const uint8_t* pubszPosition); // ------------------------------ next_space
         template <typename UTF8_TYPE>
         const UTF8_TYPE* next_space(const UTF8_TYPE* pubszPosition) { // ------------------------- next_space
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>(next_space(reinterpret_cast<const uint8_t*>(pubszPosition)));
         };

         // move pointer to next non space character
         const uint8_t* next_non_space(const uint8_t* pubszPosition); // -------------------------- next_non_space
         template <typename UTF8_TYPE>
         const UTF8_TYPE* next_non_space(const UTF8_TYPE* pubszPosition) { // --------------------- next_non_space
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>(next_non_space(reinterpret_cast<const uint8_t*>(pubszPosition)));
         };

         ///@}



         ///@{ 
         // move to previous utf8 character in buffer pointer points at
         const uint8_t* previous(const uint8_t* pubszPosition); // -------------------------------- previous
         inline uint8_t* previous(uint8_t* pubszPosition) { return const_cast<uint8_t*>( previous( static_cast<const uint8_t*>(pubszPosition) ) ); }// move to previous utf8 character in buffer pointer points at
         template <typename UTF8_TYPE>
         const UTF8_TYPE* previous(const UTF8_TYPE* pubszPosition) {                               // move to previous utf8 character in buffer pointer points at
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>( previous( reinterpret_cast<const uint8_t*>(pubszPosition) ) );
         };
         // move to previous utf8 character in buffer pointer points at
         template <typename UTF8_TYPE>
         UTF8_TYPE* previous(UTF8_TYPE* pubszPosition) { // --------------------------------------- previous
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<UTF8_TYPE*>( previous( reinterpret_cast<uint8_t*>(pubszPosition) ) );
         };

         const uint8_t* previous(const uint8_t* pubszPosition, uint32_t uCount);                   // move backwards specified count in utf8 character buffer pointer points at
         inline uint8_t* previous(uint8_t* pubszPosition, uint32_t uCount ) { return const_cast<uint8_t*>( previous( static_cast<const uint8_t*>(pubszPosition), uCount ) ); } // // move backwards specified count in utf8 character buffer pointer points at
         // move backwards specified count in utf8 character buffer pointer points at
         template <typename UTF8_TYPE>
         const UTF8_TYPE* previous( const UTF8_TYPE* pubszPosition, uint32_t uCount ) { // -------- previous     
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>( previous( reinterpret_cast<const uint8_t*>(pubszPosition), uCount ) );
         };
         ///@}

         ///@{
         /// advance specified number of characters (if negative move backwards)
         inline const uint8_t* advance(const uint8_t* pubszPosition, int iOffset) {
            if(iOffset > 0) return next(pubszPosition, iOffset);
            else if(iOffset < 0) return previous(pubszPosition, -iOffset);
            return pubszPosition;
         }
         /// advance specified number of characters (if negative move backwards)
         inline uint8_t* advance(uint8_t* pubszPosition, int iOffset) {
            if(iOffset > 0) return next(pubszPosition, iOffset);
            else if(iOffset < 0) return previous(pubszPosition, -iOffset);
            return pubszPosition;
         }
         ///@}

         // move to end of string (finds zero character and stop)
         const uint8_t* end(const uint8_t* pubszPosition); // ------------------------------------- end 
         inline uint8_t* end(uint8_t* pubszPosition) { return const_cast<uint8_t*>( end( static_cast<const uint8_t*>(pubszPosition) ) ); }// move to end of string (finds zero character and stop)
         template <typename UTF8_TYPE>
         const UTF8_TYPE* end(const UTF8_TYPE* pubszPosition) { // -------------------------------- end
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>(end(reinterpret_cast<const uint8_t*>(pubszPosition)));
         };


         const uint8_t* find( const uint8_t* pubszPosition, uint32_t uCharacter ); // ------------- find
         template <typename UTF8_TYPE, typename CHARACTER>
         const UTF8_TYPE* find( const UTF8_TYPE* pubszPosition, CHARACTER Character ) { // -------- find
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            uint32_t uCharacter;
            if constexpr( sizeof(Character) == sizeof(uint8_t) ) uCharacter = static_cast<uint8_t>(Character);       // 1 byte
            else if constexpr( sizeof(Character) == sizeof(uint16_t) ) uCharacter = static_cast<uint16_t>(Character);// 2 byte value
            else uCharacter = static_cast<uint32_t>(Character);                                                      // 4 byte or over
            return reinterpret_cast<const UTF8_TYPE*>( find( reinterpret_cast<const uint8_t*>(pubszPosition), uCharacter ) );
         }
         const uint8_t* find( const uint8_t* pubszPosition, const uint8_t* pubszEnd, uint32_t uCharacter ); // find
         template <typename UTF8_TYPE, typename CHARACTER>
         const UTF8_TYPE* find( const UTF8_TYPE* pubszPosition, const UTF8_TYPE* pubszEnd, CHARACTER Character ) { // find
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            uint32_t uCharacter;
            if constexpr( sizeof(Character) == sizeof(uint8_t) ) uCharacter = static_cast<uint8_t>(Character);       // 1 byte
            else if constexpr( sizeof(Character) == sizeof(uint16_t) ) uCharacter = static_cast<uint16_t>(Character);// 2 byte value
            else uCharacter = static_cast<uint32_t>(Character);                                                      // 4 byte or over
            return reinterpret_cast<const UTF8_TYPE*>( find( reinterpret_cast<const uint8_t*>(pubszPosition), reinterpret_cast<const uint8_t*>(pubszEnd), uCharacter ) );
         }
         const uint8_t* find_character( const uint8_t* pubszPosition, const uint8_t* pubszCharacter, uint32_t ulength );
         const uint8_t* find_character( const uint8_t* pubszPosition, const uint8_t* pubszEnd, const uint8_t* pubszCharacter, uint32_t ulength );
         inline const uint8_t* find_character( const uint8_t* pubszPosition, const uint8_t* pubszCharacter ) { return find_character( pubszPosition, pubszCharacter, static_cast<uint32_t>(std::strlen( reinterpret_cast<const char*>(pubszCharacter)) ) ); }

         const uint8_t* find( const uint8_t* pubszPosition, const uint8_t* pubszEnd, const uint8_t* pubszFind, uint32_t uSize ); // find
         inline const uint8_t* find( const uint8_t* pubszText, const uint8_t* pubszFind ) { return find( pubszText, pubszText + std::strlen( reinterpret_cast<const char*>(pubszText) ), pubszFind, static_cast<uint32_t>( std::strlen( reinterpret_cast<const char*>(pubszFind) ) ) ); }
         inline const uint8_t* find( const uint8_t* pubszText, const uint8_t* pubszEnd, const uint8_t* pubszFind ) { return find( pubszText, pubszEnd, pubszFind, static_cast<uint32_t>( std::strlen( reinterpret_cast<const char*>(pubszFind) ) ) ); }
      }

} 

   namespace utf16 {

      uint32_t character(const uint16_t* pubszText); // ------------------------------------------- character
      template <typename UTF16_TYPE>
      uint32_t character(const UTF16_TYPE* pbszText) { // ----------------------------------------- character
         //static_assert(sizeof(UTF16_TYPE) == 2, "Value isn't compatible with uint8_t");          // #GCC generates error here, try to fix this 2022-10-10
         return character(reinterpret_cast<const uint16_t*>(pbszText));
      };

      uint32_t size(uint16_t ch);
   }

}


namespace gd {
   namespace utf8 {

      /**
       * @brief find positions in text where characters start and ends
       * @param puText pointer to text scanned for first and last character
       * @param puEnd end of text
       * @return std::pair<const uint8_t*,const uint8_t*> first and last position in string where first non space is found
       */
      inline std::pair<const uint8_t*,const uint8_t*> trim(const uint8_t* puText, const uint8_t* puEnd)
      {                                                                       assert( puText < puEnd ); assert( (puEnd - puText) < 0x20000 ); // check for proper order and realistic value
         auto puFirst = puText;
         while( puFirst != puEnd )
         {
            puFirst++;
            if( *puFirst > ' ' ) break;
         }

         auto* puLast = puEnd;
         while( puLast != puFirst )
         {
            puLast--;
            if( *puLast > ' ' ) break;
         }

         return { puFirst, puLast + 1 };
      }

      /// find positions in text where characters start
      inline std::pair<const uint8_t*, const uint8_t*> trim(const uint8_t* puText) { return trim(puText, puText + std::strlen((const char*)puText)); }
      inline std::pair<const uint8_t*, const uint8_t*> trim(const std::string_view& stringText) { return trim((const uint8_t*)stringText.data(), (const uint8_t*)stringText.data() + stringText.length()); }
      /**
       * @brief find positions in text where characters start
       * @param puText pointer to text scanned for first and last character
       * @param puEnd end of text
       * @return std::pair<const uint8_t*,const uint8_t*> first and last position in string where first non space is found
       */
      inline std::pair<const uint8_t*,const uint8_t*> trim_left(const uint8_t* puText, const uint8_t* puEnd)
      {                                                                       assert( puText < puEnd ); assert( (puEnd - puText) < 0x20000 ); // check for proper order and realistic value
         auto puFirst = puText;
         while( puFirst != puEnd )
         {
            puFirst++;
            if( *puFirst > ' ' ) break;
         }

         return { puFirst, puEnd };
      }

      /// find positions in text where characters start
      inline std::pair<const uint8_t*, const uint8_t*> trim_left(const uint8_t* puText) { return trim(puText, puText + std::strlen((const char*)puText)); }
      inline std::pair<const uint8_t*, const uint8_t*> trim_left(const std::string_view& stringText) { return trim_left((const uint8_t*)stringText.data(), (const uint8_t*)stringText.data() + stringText.length()); }


      /**
       * @brief find positions in text where characters ends
       * @param puText pointer to text scanned for first and last character
       * @param puEnd end of text
       * @return std::pair<const uint8_t*,const uint8_t*> first and last position in string where first non space is found
       */
      inline std::pair<const uint8_t*,const uint8_t*> trim_right(const uint8_t* puText, const uint8_t* puEnd)
      {                                                                       assert( puText < puEnd ); assert( (puEnd - puText) < 0x20000 ); // check for proper order and realistic value
         auto* puLast = puEnd;
         while( puLast != puText )
         {
            puLast--;
            if( *puLast > ' ' ) break;
         }

         return { puText, puLast + 1 };
      }

      /// find positions in text where characters ends
      inline std::pair<const uint8_t*, const uint8_t*> trim_right(const uint8_t* puText) { return trim(puText, puText + std::strlen((const char*)puText)); }
      inline std::pair<const uint8_t*, const uint8_t*> trim_right(const std::string_view& stringText) { return trim_right((const uint8_t*)stringText.data(), (const uint8_t*)stringText.data() + stringText.length()); }

      inline std::string trim_to_string(const uint8_t* puText, const uint8_t* puEnd)
      {
         auto [first_, last_] = trim( puText, puEnd );
         return std::string(first_, last_);
      }
      inline std::string trim_to_string(const uint8_t* puText) { return trim_to_string(puText, puText + std::strlen((const char*)puText)); }
      inline std::string trim_to_string(const std::string_view& stringText) { return trim_to_string((const uint8_t*)stringText.data(), (const uint8_t*)stringText.data() + stringText.length()); }

      inline std::string trim_left_to_string(const uint8_t* puText, const uint8_t* puEnd)
      {
         auto [first_, last_] = trim(puText, puEnd);
         return std::string(first_, last_);
      }
      inline std::string trim_left_to_string(const uint8_t* puText) { return trim_left_to_string(puText, puText + std::strlen((const char*)puText)); }
      inline std::string trim_left_to_string(const std::string_view& stringText) { return trim_left_to_string((const uint8_t*)stringText.data(), (const uint8_t*)stringText.data() + stringText.length()); }

      inline std::string trim_right_to_string(const uint8_t* puText, const uint8_t* puEnd)
      {
         auto [first_, last_] = trim(puText, puEnd);
         return std::string(first_, last_);
      }
      inline std::string trim_right_to_string(const uint8_t* puText) { return trim_right_to_string(puText, puText + std::strlen((const char*)puText)); }
      inline std::string trim_right_to_string(const std::string_view& stringText) { return trim_right_to_string((const uint8_t*)stringText.data(), (const uint8_t*)stringText.data() + stringText.length()); }
   }
}
