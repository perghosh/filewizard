#pragma once

#include <stdint.h>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace gd { 
   namespace utf8 {

      constexpr uint32_t SIZE8_MAX_UTF_SIZE = 2;
      constexpr uint32_t SIZE16_MAX_UTF_SIZE = 3;
      constexpr uint32_t SIZE32_MAX_UTF_SIZE = 6;


      ///@{ 
      void normalize( uint32_t uCharacter, char8_t& value );
      uint32_t character(const uint8_t* pubszText); // -------------------------------------------- character
      template <typename UTF8_TYPE>
      uint32_t character(const UTF8_TYPE* pbszText) { // ------------------------------------------ character
         static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
         return character(reinterpret_cast<const uint8_t*>(pbszText));
      };
      ///@}


      /**
       * Count number of characters in utf8 buffer
       */
      ///@{ 
      std::pair<uint32_t, const uint8_t*> count(const uint8_t* pubszText);                         // count utf8 characters
      inline std::pair<uint32_t, const uint8_t*> count(const std::string_view stringText) { return count( reinterpret_cast<const uint8_t*>(stringText.data()) ); };
      template <typename UTF8_TYPE>
      std::pair<uint32_t, const uint8_t*> count(const UTF8_TYPE* pbszText) {                       // count utf8 characters
         static_assert( sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
         return count( reinterpret_cast<const uint8_t*>(pbszText) ); 
      };
      std::pair<uint32_t, const uint8_t*> count( const uint8_t* pubszText¨, const uint8_t* pubszEnd );
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

      /// calculate needed size to store character as utf8 value
      uint32_t size( uint8_t ch ); // ------------------------------------------------------------- size
      uint32_t size( uint16_t ch ); // ------------------------------------------------------------ size
      /// count needed size to store char string as utf8 string
      inline uint32_t size( const char* pbsz ) { // ----------------------------------------------- size
         uint32_t uSize = 0;
         while( *pbsz++ != 0 ) uSize += size( static_cast<uint8_t>(*pbsz) );
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
      uint32_t size( const char* pbszText, uint32_t uLength );
      ///@}



      ///@{
      uint32_t convert(uint8_t uCharacter, uint8_t* pbszTo); // ----------------------------------- convert
      uint32_t convert(uint16_t uCharacter, uint8_t* pbszTo); // ---------------------------------- convert
      uint32_t convert(uint32_t uCharacter, uint8_t* pbszTo); // ---------------------------------- convert
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

      std::pair<bool, const uint8_t*> convert_ascii(const uint8_t* pbszFrom, uint8_t* pbszTo);
      template <typename UTF8_TYPE, typename TYPE>
      std::pair<bool, const uint8_t*> convert_ascii(const UTF8_TYPE* pbszFrom, TYPE* pbszTo) {
         return convert_ascii(reinterpret_cast<const uint8_t*>(pbszFrom), reinterpret_cast<uint8_t*>(pbszTo));
      }
      std::pair<bool, const uint8_t*> convert_ascii(const uint8_t* pbszFrom, uint8_t* pbszTo, const uint8_t* pbszEnd);
      template <typename UTF8_TYPE, typename TYPE>
      std::pair<bool, const uint8_t*> convert_ascii(const UTF8_TYPE* pbszFrom, TYPE* pbszTo, const TYPE* pbszEnd) {
         return convert_ascii(reinterpret_cast<const uint8_t*>(pbszFrom), reinterpret_cast<uint8_t*>(pbszTo), reinterpret_cast<const uint8_t*>(pbszEnd));
      }

      std::pair<bool, const uint16_t*> convert_unicode(const uint16_t* pbszFrom, uint8_t* pbszTo, const uint8_t* pbszEnd);
      ///@}


      /**
       * @brief move operation for pointer in utf-8 buffer
      */
      namespace move {

         ///@{ 

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
         ///@}


         ///@{ 
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
      uint32_t character(const UTF16_TYPE* pbszText) { // ------------------------------------------ character
         static_assert(sizeof(UTF16_TYPE) == 2, "Value isn't compatible with uint8_t");
         return character(reinterpret_cast<const uint16_t*>(pbszText));
      };

      uint32_t size(uint16_t ch);

   }

}