#pragma once

#include <stdint.h>
#include <string>
#include <string_view>
#include <type_traits>

namespace gd { namespace utf8 {

      ///@{ 
      void normalize( uint32_t uCharacter, char8_t& value );
      uint32_t character(const uint8_t* pubszText);
      template <typename UTF8_TYPE>
      uint32_t character(const UTF8_TYPE* pbszText) {
         static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
         return character(reinterpret_cast<const uint8_t*>(pbszText));
      };
      uint32_t character(const uint16_t* pubszText);
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
      inline uint32_t strlen(const uint8_t* pubszText) { return count(pubszText).first; };         // count utf8 characters
      inline uint32_t strlen(const std::string_view stringText) { return count( reinterpret_cast<const uint8_t*>(stringText.data()) ).first; };
      template <typename UTF8_TYPE>
      uint32_t strlen(const UTF8_TYPE* pbszText) {                                                 // count utf8 characters
         static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
         return count(reinterpret_cast<const uint8_t*>(pbszText)).first;
      };

      ///@}

      ///@{ 
      /// Count number of utf8 bytes needed to store char text
      uint32_t size( const char* pbszText, uint32_t uLength );
      ///@}



      ///@{
      uint32_t convert(uint8_t uCharacter, uint8_t* pbszTo);
      uint32_t convert(uint16_t uCharacter, uint8_t* pbszTo);
      uint32_t convert(uint32_t uCharacter, uint8_t* pbszTo);

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

         const uint8_t* next(const uint8_t* pubszPosition);                                        // move to next utf8 character in buffer pointer points at
         inline uint8_t* next(uint8_t* pubszPosition) { return const_cast<uint8_t*>( next( static_cast<const uint8_t*>(pubszPosition) ) ); }// move to next utf8 character in buffer pointer points at

         template <typename UTF8_TYPE>
         const UTF8_TYPE* next(const UTF8_TYPE* pubszPosition) {                                   // move to next utf8 character in buffer pointer points at
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>( next( reinterpret_cast<const uint8_t*>(pubszPosition) ) );
         };
         template <typename UTF8_TYPE>
         UTF8_TYPE* next(UTF8_TYPE* pubszPosition) {                                               // move to next utf8 character in buffer pointer points at
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<UTF8_TYPE*>( next( reinterpret_cast<uint8_t*>(pubszPosition) ) );
         };

         const uint8_t* next(const uint8_t* pubszPosition, uint32_t uCount );                      // move specified count in buffer pointer points at
         inline uint8_t* next(uint8_t* pubszPosition, uint32_t uCount ) { return const_cast<uint8_t*>( next( static_cast<const uint8_t*>(pubszPosition), uCount ) ); } // move specified count in buffer pointer points at
         template <typename UTF8_TYPE>
         const UTF8_TYPE* next(const UTF8_TYPE* pubszPosition, uint32_t uCount ) {                 // move specified count in buffer pointer points at
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>( next(reinterpret_cast<const uint8_t*>(pubszPosition), uCount) );
         };

         inline bool next(const uint8_t** ppubszPosition) {                                        // move pointer to next utf8 character in buffer pointer to pointer points at, return true or false if it succeeded
            const uint8_t* p = next(*ppubszPosition);
            if(p != *ppubszPosition) { ppubszPosition = &p; return true; }
            return false;
         }
         template <typename UTF8_TYPE>
         bool next(const UTF8_TYPE** ppubszPosition) {                                             // move pointer to next utf8 character in buffer pointer to pointer points at, return true or false if it succeeded
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>(next(reinterpret_cast<const uint8_t**>(ppubszPosition)));
         };
         bool next(const uint8_t** ppubszPosition, uint32_t uCount);
         template <typename UTF8_TYPE>
         bool next(const UTF8_TYPE** ppubszPosition, uint32_t uCount) {
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>(next(reinterpret_cast<const uint8_t**>(ppubszPosition), uCount));
         };
         ///@}


         ///@{ 
         ///@}


         ///@{ 
         const uint8_t* previous(const uint8_t* pubszPosition);                                    // move to previous utf8 character in buffer pointer points at
         inline uint8_t* previous(uint8_t* pubszPosition) { return const_cast<uint8_t*>( previous( static_cast<const uint8_t*>(pubszPosition) ) ); }// move to previous utf8 character in buffer pointer points at
         template <typename UTF8_TYPE>
         const UTF8_TYPE* previous(const UTF8_TYPE* pubszPosition) {                               // move to previous utf8 character in buffer pointer points at
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>( previous( reinterpret_cast<const uint8_t*>(pubszPosition) ) );
         };
         template <typename UTF8_TYPE>
         UTF8_TYPE* previous(UTF8_TYPE* pubszPosition) {                                           // move to previous utf8 character in buffer pointer points at
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<UTF8_TYPE*>( previous( reinterpret_cast<uint8_t*>(pubszPosition) ) );
         };

         const uint8_t* previous(const uint8_t* pubszPosition, uint32_t uCount);                   // move backwards specified count in utf8 character buffer pointer points at
         inline uint8_t* previous(uint8_t* pubszPosition, uint32_t uCount ) { return const_cast<uint8_t*>( previous( static_cast<const uint8_t*>(pubszPosition), uCount ) ); } // // move backwards specified count in utf8 character buffer pointer points at
         template <typename UTF8_TYPE>
         const UTF8_TYPE* previous( const UTF8_TYPE* pubszPosition, uint32_t uCount ) {            // move backwards specified count in utf8 character buffer pointer points at
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>( previous( reinterpret_cast<const uint8_t*>(pubszPosition), uCount ) );
         };
         ///@}

         const uint8_t* end(const uint8_t* pubszPosition);                                         // move to end of string (finds zero character and stop)
         inline uint8_t* end(uint8_t* pubszPosition) { return const_cast<uint8_t*>( end( static_cast<const uint8_t*>(pubszPosition) ) ); }// move to end of string (finds zero character and stop)
         template <typename UTF8_TYPE>
         const UTF8_TYPE* end(const UTF8_TYPE* pubszPosition) {
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>(end(reinterpret_cast<const uint8_t*>(pubszPosition)));
         };
      }


} }