#include <stdint.h>
#include <string>
#include <string_view>
#include <type_traits>

namespace gd { namespace utf8 {

      ///@{ 
      uint32_t character(const uint8_t* pubszText);
      template <typename UTF8_TYPE>
      uint32_t character(const UTF8_TYPE* pbszText) {
         static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
         return character(reinterpret_cast<const uint8_t*>(pbszText));
      };
      ///@}


      /**
       * Count number of characters in utf8 buffer
       */
      ///@{ 
      std::pair<std::size_t, const uint8_t*> count(const uint8_t* pubszText);
      inline std::pair<std::size_t, const uint8_t*> count(const std::string_view stringText) { return count( reinterpret_cast<const uint8_t*>(stringText.data()) ); };
      template <typename UTF8_TYPE>
      std::pair<std::size_t, const uint8_t*> count(const UTF8_TYPE* pbszText) {
         static_assert( sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
         return count( reinterpret_cast<const uint8_t*>(pbszText) ); 
      };
      inline std::size_t strlen(const uint8_t* pubszText) { return count(pubszText).first; };
      inline std::size_t strlen(const std::string_view stringText) { return count( reinterpret_cast<const uint8_t*>(stringText.data()) ).first; };
      template <typename UTF8_TYPE>
      std::size_t strlen(const UTF8_TYPE* pbszText) {
         static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
         return count(reinterpret_cast<const uint8_t*>(pbszText)).first;
      };
      ///@}


      ///@{
      std::pair<bool, const uint8_t*> convert_ascii(const uint8_t* pbszFrom, uint8_t* pbszTo, const uint8_t* pbszEnd);
      template <typename UTF8_TYPE, typename TYPE>
      std::pair<bool, const uint8_t*> convert_ascii(const UTF8_TYPE* pbszFrom, TYPE* pbszTo, const TYPE* pbszEnd) {
         return convert_ascii(reinterpret_cast<const uint8_t*>(pbszFrom), reinterpret_cast<uint8_t*>(pbszTo), reinterpret_cast<const uint8_t*>(pbszEnd));
      }
      ///@}


      /**
       * @brief move operation for pointer in utf-8 buffer
      */
      namespace move {
         ///@{ 
         const uint8_t* next(const uint8_t* pubszPosition);
         template <typename UTF8_TYPE>
         const UTF8_TYPE* next(const UTF8_TYPE* pubszPosition) {
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>( next( reinterpret_cast<const uint8_t*>(pubszPosition) ) );
         };

         const uint8_t* next(const uint8_t* pubszPosition, std::size_t uCount );
         template <typename UTF8_TYPE>
         const UTF8_TYPE* next(const UTF8_TYPE* pubszPosition, std::size_t uCount ) {
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>( next(reinterpret_cast<const uint8_t*>(pubszPosition), uCount) );
         };
         ///@}




         ///@{ 
         const uint8_t* previous(const uint8_t* pubszPosition);
         template <typename UTF8_TYPE>
         const UTF8_TYPE* previous(const UTF8_TYPE* pubszPosition) {
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>( previous( reinterpret_cast<const uint8_t*>(pubszPosition) ) );
         };

         const uint8_t* previous(const uint8_t* pubszPosition, std::size_t uCount);
         template <typename UTF8_TYPE>
         const UTF8_TYPE* previous( const UTF8_TYPE* pubszPosition, std::size_t uCount ) {
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>( previous( reinterpret_cast<const uint8_t*>(pubszPosition), uCount ) );
         };
         ///@}

         const uint8_t* end(const uint8_t* pubszPosition);
         template <typename UTF8_TYPE>
         const UTF8_TYPE* end(const UTF8_TYPE* pubszPosition) {
            static_assert(sizeof(UTF8_TYPE) == 1, "Value isn't compatible with uint8_t");
            return reinterpret_cast<const UTF8_TYPE*>(end(reinterpret_cast<const uint8_t*>(pubszPosition)));
         };
      }


} }