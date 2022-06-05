#pragma once
#include <cassert>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#include "gd_variant.h"


/*
| class | Description |
| - | - |
| argument | Manage values in `arguments` |
|   |   |

mdtable

// structured binding
// https://devblogs.microsoft.com/oldnewthing/20201015-00/?p=104369

*/


#pragma warning( disable : 26495 )

#ifndef _GD_ARGUMENT_BEGIN
namespace gd::argument {
#else
_GD_PARAM_BEGIN
#endif

struct pair_value { std::string_view m_stringKey; gd::variant m_variantValue; };


/**
 * \brief
 *
 *
 *
 \code
 \endcode
 */
class arguments
{
public:
   typedef uint8_t            value_type;
   typedef value_type*        pointer;
   typedef const value_type*  const_pointer;
   typedef value_type&        reference;
   typedef const value_type&  const_reference;
   typedef size_t             size_type;
   typedef ptrdiff_t          difference_type;

   typedef uint8_t            param_type;


public:

   /**
    * \brief type values used in arguments class
    *
    */
   enum enumCType
   {
      // ## primitive types
      eCTypeNone     = 0,
      eCTypeBool     = 1,
      eCTypeInt8     = 2,
      eCTypeUInt8    = 3,
      eCTypeInt16    = 4,
      eCTypeUInt16   = 5,
      eCTypeInt32    = 6,
      eCTypeUInt32   = 7,
      eCTypeInt64    = 8,
      eCTypeUInt64   = 9,

      eCTypeFloat,
      eCTypeDouble,

      // ## derived types
      eCTypePointer,       // pointer to memory

      eCTypeBinaryUuid,    // universal unique identifier

      eCTypeString,        // ascii string
      eCTypeUtf8String,    // utf8 string
      eCTypeWString,       // unicode string

      eCTypeBinary,

      CType_MAX,


      eCType_ParameterName,         // special type for parameter names

      eValueName = 0b00100000,
      eValueLength = 0b01000000,
      eValueArray = 0b10000000,

      CType_MASK = 0b11100000,
   };

   const unsigned ARGUMENTS_NO_LENGTH = eCTypeBinaryUuid;

   /**
    * \brief member type for complete value in arguments list
    *
    */
   enum enumPairType
   {
      ePairTypeKey   = (1<<0),        // Key (name for value) in argument list
      ePairTypeValue = (1<<1),        // Value in argument list
      ePairTypeAll   = ePairTypeKey | ePairTypeValue,
   };

   /**
    * Base64 flags used to specify how base64 formated text is formated
    */
   enum enumBase64
   {
      eBase64NoCrLf = 0x01,  ///< don't and carriage return and line feed for each 76 char section
      eBaseNoPad = 0x02,  ///< don't pad with '='
   };

public:
   struct argument
   {
      union value;   // forward declare
      /// default constructor
      argument(): m_eCType(arguments::enumCType::eCTypeNone) {}
      //@{
      /// constructors for specified types
      argument(bool b) : m_eCType(arguments::eCTypeBool) { m_unionValue.b = b; }
      argument(int8_t v) : m_eCType(arguments::eCTypeInt8) { m_unionValue.v_int8 = v; }
      argument(uint8_t v) : m_eCType(arguments::eCTypeUInt8) { m_unionValue.v_uint8 = v; }
      argument(int16_t v) : m_eCType(arguments::eCTypeInt16) { m_unionValue.v_int16 = v; }
      argument(uint16_t v) : m_eCType(arguments::eCTypeUInt16) { m_unionValue.v_uint16 = v; }
      argument(int32_t v) : m_eCType(arguments::eCTypeInt32) { m_unionValue.v_int32 = v; }
      argument(uint32_t v) : m_eCType(arguments::eCTypeUInt32) { m_unionValue.v_uint32 = v; }
      argument(int64_t v) : m_eCType(arguments::eCTypeInt64) { m_unionValue.v_int64 = v; }
      argument(uint64_t v) : m_eCType(arguments::eCTypeUInt64) { m_unionValue.v_uint64 = v; }
      argument(float f) : m_eCType(arguments::eCTypeFloat) { m_unionValue.f = f; }
      argument(double d) : m_eCType(arguments::eCTypeDouble) { m_unionValue.d = d; }

      //param(binary_support::uuid* p) : m_eCType(params::eCustomTypeUuid) { m_unionValue.p = p; }

      argument(const char* pbsz) : m_eCType( enumCType(arguments::eCTypeString) ) { m_unionValue.pbsz = pbsz; }
      argument(const wchar_t* pwsz) : m_eCType( enumCType(arguments::eCTypeWString) ) { m_unionValue.pwsz = pwsz; }
      argument(arguments::enumCType eType, const char* pbsz) : m_eCType(enumCType(eType)) { m_unionValue.pbsz = pbsz; }
      argument(arguments::enumCType eType, const wchar_t* pwsz) : m_eCType(enumCType(eType)) { m_unionValue.pwsz = pwsz; }
      argument(void* p) : m_eCType(arguments::eCTypePointer) { m_unionValue.p = p; }
      argument(const uint8_t* p) : m_eCType( enumCType(arguments::eCTypeBinary) ) { m_unionValue.puch = p; }
      argument(arguments::enumCType eType,const uint8_t* p) : m_eCType(eType) { m_unionValue.puch = p; }
      argument(const uint8_t* p, enumCType eType) : m_eCType(eType) { m_unionValue.puch = p; }
      
      //@}

      argument(const argument& o) {
         common_construct(o);
      }
      argument& operator=(const argument& o) {
         common_construct(o);
         return *this;
      }

      void common_construct(const argument& o) {
         m_eCType = o.m_eCType;
         m_unionValue.d = o.m_unionValue.d;
      }

      bool operator==(const argument& o) { return compare_argument_s(*this, o); }


      bool operator !() { return (m_eCType == arguments::eCTypeNone); }

      operator variant() { return get_variant(); }
      operator variant() const { return get_variant(false); }

      const argument& operator>>(int& v) const { v = get_int(); return *this; }
      const argument& operator>>(unsigned int& v) const { v = get_uint(); return *this; }
      const argument& operator>>(std::string& v) const { v = get_string(); return *this; }
      template <typename VALUE>
      const argument& operator>>(std::pair<std::string_view,VALUE&> pair) const { 
         auto pPosition = find( pair.first );
         if( pPosition ) pair.second = get_argument_s(pair.first);
         return *this;
      }


      operator bool() const { assert(type() == arguments::eCTypeBool); return m_unionValue.b; }
      operator int8_t() const { assert(type() >= arguments::eCTypeInt8 && type() <= arguments::eCTypeUInt8); return m_unionValue.v_int8; }
      operator uint8_t() const { assert(type() >= arguments::eCTypeInt8 && type() <= arguments::eCTypeUInt8); return m_unionValue.v_uint8; }
      operator int16_t() const { assert(type() >= arguments::eCTypeInt16 && type() <= arguments::eCTypeUInt16); return m_unionValue.v_int16; }
      operator uint16_t() const { assert(type() >= arguments::eCTypeInt16 && type() <= arguments::eCTypeUInt16); return m_unionValue.v_uint16; }
      operator long() const { assert(type() >= arguments::eCTypeInt32 && type() <= arguments::eCTypeUInt32); return m_unionValue.v_int32; }
      operator int32_t() const { assert(type() >= arguments::eCTypeInt32 && type() <= arguments::eCTypeUInt32); return m_unionValue.v_int32; }
      operator uint32_t() const { assert(type() >= arguments::eCTypeInt32 && type() <= arguments::eCTypeUInt32); return m_unionValue.v_uint32; }
      operator int64_t() const { assert(type() >= arguments::eCTypeInt64 && type() <= arguments::eCTypeUInt64); return m_unionValue.v_int64; }
      operator uint64_t() const { assert(type() >= arguments::eCTypeInt64 && type() <= arguments::eCTypeUInt64); return m_unionValue.v_uint64; }
      operator double() const { assert(type() == arguments::eCTypeDouble); return m_unionValue.d; }
      operator const char* () const { assert(type() == arguments::eCTypeNone || type() == arguments::eCTypeString); return (type() == arguments::eCTypeString) ? m_unionValue.pbsz : ""; }
      operator const wchar_t* () const { assert(type() == arguments::eCTypeNone || type() == arguments::eCTypeWString); return (type() == arguments::eCTypeWString) ? m_unionValue.pwsz : L""; }
      operator std::string() const { assert(type() == arguments::eCTypeNone || type() == arguments::eCTypeString); return (type() == arguments::eCTypeString) ? m_unionValue.pbsz : ""; }
      operator std::wstring() const { assert(type() == arguments::eCTypeNone || type() == arguments::eCTypeWString); return (type() == arguments::eCTypeWString) ? m_unionValue.pwsz : L""; }
      operator void* () const { assert(type() == arguments::eCTypeNone || type() == arguments::eCTypePointer); return (type() == arguments::eCTypePointer) ? m_unionValue.p : NULL; }

      /// compare two argument values
      bool compare( const argument& o ) const { return compare_argument_s(*this, o); }
      /// compare within group type, if integer all sizes are valid for comparison
      bool compare_group( const argument& o ) const { return compare_argument_group_s(*this, o); }

      /// length in bytes for param
      unsigned int length() const;
      /// get param type
      arguments::enumCType type() const { return arguments::enumCType((unsigned)m_eCType & ~CType_MASK); }
      /// return the raw internal type, this has optional flags for type
      unsigned int ctype() const { return (unsigned int)m_eCType; }
      bool type(arguments::enumCType eCType) const { return (m_eCType == eCType); }
      /// check if param is empty
      bool empty() const { return (m_eCType == arguments::eCTypeNone); }

      void get_binary_as_hex(std::string& s) const;
      unsigned int get_binary_as_hex(char* pbsz, unsigned int uLength) const;

      /// reset param 
      void reset(const argument* pParam = nullptr) {
         if( pParam != nullptr ) {
            *this = *pParam;
         }
         else {
            m_eCType = arguments::eCTypeNone;
         }
      }

      unsigned int as_uint() const { return get_uint(); }
      unsigned int as_int() const { return get_int(); }
      unsigned int as_variant() const { return get_variant(); }


      int          get_int() const;
      unsigned int get_uint() const;
      int64_t      get_int64() const;
      double       get_double() const;
      std::string  get_string() const;
      gd::variant  get_variant() const { return arguments::get_variant_s(*this); }
      gd::variant  get_variant( bool ) const { return arguments::get_variant_s(*this, false); } /// for speed, do not copy data
      value        get_value() { return m_unionValue; }
      const value& get_value() const { return m_unionValue; }

      std::string  to_string() const { return get_string(); }

      bool         is_null() const { return (m_eCType == arguments::eCTypeNone); }
      bool         is_bool() const { return (m_eCType == arguments::eCTypeBool); }
      bool         is_int32() const { return (m_eCType == arguments::eCTypeInt32); }
      bool         is_uint32() const { return (m_eCType == arguments::eCTypeUInt32); }
      bool         is_int64() const { return (m_eCType == arguments::eCTypeInt64); }
      bool         is_uint64() const { return (m_eCType == arguments::eCTypeUInt64); }
      bool         is_double() const { return (m_eCType == arguments::eCTypeDouble); }
      bool         is_string() const { return (m_eCType == arguments::eCTypeString); }
      bool         is_wstring() const { return (m_eCType == arguments::eCTypeWString); }
      bool         is_true() const;
      bool         is_primitive() const { return (m_eCType > arguments::eCTypeNone && m_eCType < eCTypeDouble); } ///< primitive = built in types in C++
      bool         is_text() const { return (m_eCType >= arguments::eCTypeString && m_eCType <= eCTypeWString); } ///< text = some sort of string value, ascii, utf8 or unicode
      bool         is_binary() const { return m_eCType == arguments::eCTypeBinary; } ///< binary = blob data, length is unknown if used in argument (work with this in arguments)
      bool         is_number() const { return (m_eCType >= arguments::eCTypeInt32 && m_eCType <= arguments::eCTypeDouble); }
      bool         is_decimal() const { return (m_eCType >= arguments::eCTypeFloat && m_eCType <= arguments::eCTypeDouble); }
      bool         is_integer() const { return (m_eCType >= arguments::eCTypeInt32 && m_eCType <= arguments::eCTypeUInt64); }

      void*        get_raw_pointer() const { return m_unionValue.p; }            ///< return raw pointer to value
      void*        get_value_buffer() const { return (void*)&m_unionValue; }     ///< return address pointer to value

      // attributes
   public:
      arguments::enumCType m_eCType;      // type of value valid for m_unionValue 
      union value
      {
         bool b;
         char ch;
         unsigned char uch;
         short s;
         wchar_t wch;
         int8_t  v_int8;
         uint8_t v_uint8;
         int16_t  v_int16;
         uint16_t v_uint16;
         int32_t  v_int32;
         uint32_t v_uint32;
         int64_t v_int64;
         uint64_t v_uint64;
         float f;
         double d;
         const char* pbsz;
         const wchar_t* pwsz;
         const uint8_t* puch;
         void* p;

         value(): v_uint64(0) {}
      } m_unionValue;
   };

   struct argument_edit : public argument
   {
      argument_edit() : argument(), m_pArguments(nullptr) {}

      argument_edit(const argument& o) {
         common_construct(o);
      }

      void common_construct(const argument& o) {
         memcpy(this, &o, sizeof(argument_edit));
      }

      argument_edit& operator=(argument argumentSet) { set(argumentSet); return *this; }


      template<typename ARGUMENT_TYPE>
      argument_edit(arguments* parguments, arguments::const_pointer pPosition, ARGUMENT_TYPE AG): m_pArguments(parguments), m_pPosition(pPosition), argument( AG ) {
         m_pValue = move_to_value( (pointer)pPosition );
      }

      void set(argument argumentSet);

      arguments* m_pArguments;
      arguments::const_pointer m_pPosition;
      arguments::pointer m_pValue;
   };


   /**
    * @brief iterator for iterating values in params object.
   */
   struct const_iterator
   {
      typedef const_iterator     self;

      const_iterator() : m_pArguments(nullptr), m_pPosition(nullptr) {}
      const_iterator(arguments::const_pointer pPosition ): m_pPosition(pPosition) {}
      const_iterator(const arguments* pparams, arguments::const_pointer pPosition) : m_pArguments(pparams), m_pPosition(pPosition) {}
      const_iterator(const const_iterator& o) { m_pArguments = o.m_pArguments; m_pPosition = o.m_pPosition; }
      const_iterator& operator=(const const_iterator& o) { m_pArguments = o.m_pArguments; m_pPosition = o.m_pPosition; return *this; }

      argument operator*() const {                                               assert( m_pArguments->verify_d( m_pPosition ));
         return get_argument();
      }
      self& operator++() {                                                       assert( m_pArguments->verify_d( m_pPosition ));
         m_pPosition = m_pArguments->next(m_pPosition); return *this;
      }
      self& operator++(int) {                                                    assert( m_pArguments->verify_d( m_pPosition ));
         m_pPosition = m_pArguments->next(m_pPosition); return *this;
      }
      bool operator==(const_iterator& o) { return m_pPosition == o.m_pPosition; }
      bool operator!=(const_iterator& o) { return m_pPosition != o.m_pPosition; }

      operator const arguments*() const { return m_pArguments; }
      operator arguments::const_pointer() const { return m_pPosition; }

      std::string name() const {                                                 assert( m_pArguments->verify_d( m_pPosition ));
         if( arguments::is_name_s(m_pPosition) == true )
         {
            return std::string(arguments::get_name_s(m_pPosition));
         }
         return std::string();
      }

      bool compare_name(std::string_view stringName) const { 
         if( arguments::is_name_s(m_pPosition) == true )
         {
            if( arguments::get_name_s(m_pPosition) == stringName ) return true;
         }
         return false;
      }

      argument get_argument() {
         return arguments::get_argument_s(m_pPosition);
      }
      const argument get_argument() const { 
         return arguments::get_argument_s(m_pPosition);
      }


      template<std::size_t uIndex>
      auto get() const
      {
         static_assert(uIndex < 2, "Allowed index are 0 and 1, above is not valid");
         if constexpr( uIndex == 0 ) return name();
         if constexpr( uIndex == 1 ) return get_argument();
      }


      // attributes
   public:
      const arguments* m_pArguments;
      arguments::const_pointer m_pPosition;
   };


   // ## construction -------------------------------------------------------------
public:
   arguments() : m_bOwner(false), m_pBuffer(nullptr), m_uLength(0), m_uBufferLength(0) {}

   /** Set buffer and size, use this to avoid heap allocations (if internal data grows over buffer size you will get heap allocation)  */
   arguments(pointer pBuffer, unsigned int uSize) : m_bOwner(false), m_pBuffer(pBuffer), m_uLength(0), m_uBufferLength(uSize) {}

   //explicit arguments(std::string_view stringName, const gd::variant& variantValue ) : m_bOwner(false), m_pBuffer(nullptr), m_uLength(0), m_uBufferLength(0) { append_argument(stringName, variantValue); }
   arguments(std::pair<std::string_view, gd::variant> pairArgument);
   template <typename... Arguments>
   arguments(std::pair<std::string_view, gd::variant> pairArgument, Arguments... arguments)
   {
      zero();
      auto _argument = get_argument_s(pairArgument.second);
      append_argument(pairArgument.first, _argument);
      append_argument(arguments...);
   }
   arguments(std::initializer_list<std::pair<std::string_view, gd::variant>> listPair);

   // copy
   arguments(const arguments& o): m_pBuffer(nullptr), m_uBufferLength(0) { common_construct(o); }
   arguments(arguments&& o) noexcept { common_construct((arguments&&)o); }
   // assign
   arguments& operator=(const arguments& o) { common_construct(o); return *this; }
   arguments& operator=(arguments&& o) noexcept { common_construct(o); return *this; }

   arguments& operator=(std::initializer_list<std::pair<std::string_view, gd::variant>> listPair);

   ~arguments() { 
      if( m_bOwner ) delete[] m_pBuffer;
   }
private:
   // common copy
   void common_construct(const arguments& o) {
      if( o.m_uLength )
      {
         reserve_no_copy(o.m_uLength);
         memcpy(m_pBuffer, o.m_pBuffer, o.m_uLength);
      }
      m_bOwner = o.m_bOwner;
      m_uLength = o.m_uLength;
   }

   void common_construct(arguments&& o) noexcept {
      memcpy(this, &o, sizeof(arguments));
      o.m_pBuffer = nullptr;
      o.m_bOwner = false;
      o.m_uLength = 0;
      o.m_uBufferLength = 0;
   }

   void zero() { m_bOwner = false; m_pBuffer = nullptr; m_uLength = 0; m_uBufferLength = 0; };

   // ## operator -----------------------------------------------------------------
public:
   const argument operator[](unsigned uIndex) const { return get_argument(uIndex); }
   const argument operator[](std::string_view stringName) const { return get_argument(stringName); }

   argument_edit operator[](unsigned uIndex) {
      const_pointer pPosition = find(uIndex);
      if( pPosition != nullptr ) { return arguments::get_edit_param_s(this, pPosition); }
      return argument_edit();
   }

   argument_edit operator[](std::string_view stringName) { 
      const_pointer pPosition = find(stringName);
      if( pPosition != nullptr ) { return arguments::get_edit_param_s(this, pPosition); }
      return argument_edit();
   }


   // ## methods ------------------------------------------------------------------
public:
/** \name GET/SET
*///@{
/// return start position to buffer where values are stored
   pointer get_buffer_start() { return m_pBuffer; }
   const_pointer get_buffer_start() const { return m_pBuffer; }
   /// return last position for buffer where values are stored
   pointer get_buffer_end() { return m_pBuffer + m_uLength; }
   const_pointer get_buffer_end() const { return m_pBuffer + m_uLength; }
//@}

/** \name OPERATION
*///@{

   arguments& append(int8_t v) { return append(eCTypeInt8, (const_pointer)&v, sizeof(int8_t)); }
   arguments& append(uint8_t v) { return append(eCTypeUInt8, (const_pointer)&v, sizeof(uint8_t)); }
   arguments& append(int16_t v) { return append(eCTypeInt16, (const_pointer)&v, sizeof(int16_t)); }
   arguments& append(uint16_t v) { return append(eCTypeUInt16, (const_pointer)&v, sizeof(uint16_t)); }
   arguments& append(int32_t v) { return append(eCTypeInt32, (const_pointer)&v, sizeof(int32_t)); }
   arguments& append(uint32_t v) { return append(eCTypeUInt32, (const_pointer)&v, sizeof(uint32_t)); }
   arguments& append(int64_t v) { return append(eCTypeInt64, (const_pointer)&v, sizeof(int64_t)); }
   arguments& append(uint64_t v) { return append(eCTypeUInt64, (const_pointer)&v, sizeof(uint64_t)); }
   arguments& append(std::string_view v) { return append((eCTypeString | eValueLength), (const_pointer)v.data(), (unsigned int)v.length() + 1); }
   arguments& append(std::wstring_view v) { return append((eCTypeWString | eValueLength), (const_pointer)v.data(), ((unsigned int)v.length() + 1) * sizeof(wchar_t)); }
   arguments& append(param_type uType, const_pointer pBuffer, unsigned int uLength);

   arguments& append(std::string_view stringName, int8_t v) { return append(stringName, eCTypeInt8, (const_pointer)&v, sizeof(int8_t)); }
   arguments& append(std::string_view stringName, uint8_t v) { return append(stringName, eCTypeUInt8, (const_pointer)&v, sizeof(uint8_t)); }
   arguments& append(std::string_view stringName, int16_t v) { return append(stringName, eCTypeInt16, (const_pointer)&v, sizeof(int16_t)); }
   arguments& append(std::string_view stringName, uint16_t v) { return append(stringName, eCTypeUInt16, (const_pointer)&v, sizeof(uint16_t)); }
   arguments& append(std::string_view stringName, int32_t v) { return append(stringName, eCTypeInt32, (const_pointer)&v, sizeof(int32_t)); }
   arguments& append(std::string_view stringName, uint32_t v) { return append(stringName, eCTypeUInt32, (const_pointer)&v, sizeof(uint32_t)); }
   arguments& append(std::string_view stringName, int64_t v) { return append(stringName, eCTypeInt64, (const_pointer)&v, sizeof(int64_t)); }
   arguments& append(std::string_view stringName, uint64_t v) { return append(stringName, eCTypeUInt64, (const_pointer)&v, sizeof(uint64_t)); }
   arguments& append(std::string_view stringName, std::string_view v) { return append(stringName, (eCTypeString | eValueLength), (const_pointer)v.data(), (unsigned int)v.length() + 1); }
   arguments& append(std::string_view stringName, std::wstring_view v) { return append(stringName, (eCTypeWString | eValueLength), (const_pointer)v.data(), ((unsigned int)v.length() + 1) * sizeof(wchar_t)); }
   arguments& append(std::string_view stringName, param_type uType, const_pointer pBuffer, unsigned int uLength) { return append(stringName.data(), (uint32_t)stringName.length(), uType, pBuffer, uLength); }
   arguments& append(const char* pbszName, uint32_t uNameLength, param_type uType, const_pointer pBuffer, unsigned int uLength);
   template<typename POINTER>
   arguments& append(const char* pbszName, uint32_t uNameLength, param_type uType, POINTER pBuffer, unsigned int uLength) {
      static_assert(std::is_pointer<POINTER>::value);
      return append(pbszName, uNameLength, uType, (const_pointer)pBuffer, uLength);
   }

   arguments& append_argument(std::string_view stringName, argument argumentValue) {
      auto _l = argumentValue.length();
      const_pointer pData = (argumentValue.type() <= eCTypePointer ? (const_pointer)&argumentValue.m_unionValue : (const_pointer)argumentValue.get_raw_pointer());
      return append(stringName, argumentValue.ctype(), pData, argumentValue.length());
   }

   arguments& append_argument(std::string_view stringName, const gd::variant& variantValue) {
      auto argumentValue = get_argument_s(variantValue);
      const_pointer pData = (argumentValue.type() <= eCTypePointer ? (const_pointer)&argumentValue.m_unionValue : (const_pointer)argumentValue.get_raw_pointer());
      unsigned uType = argumentValue.type();
      if( uType > ARGUMENTS_NO_LENGTH ) { uType |= eValueLength; }
      return append(stringName, uType, pData, argumentValue.length());
   }


   arguments& append_argument(const std::pair<std::string_view, gd::variant>& pairArgument) {
      return append_argument(pairArgument.first, pairArgument.second);
   }


   arguments& append_binary( const uint8_t* puData, unsigned int uLength ) { return append(eCTypeBinary, puData, uLength); }
   arguments& append_binary(std::string_view stringName, const uint8_t* puData, unsigned int uLength) { return append(stringName, (eCTypeBinary | eValueLength), puData, uLength); }
   arguments& append_uuid( const uint8_t* puData ) { return append(eCTypeBinaryUuid, puData, 16); }
   arguments& append_uuid(std::string_view stringName, const uint8_t* puData ) { return append(stringName, eCTypeBinaryUuid, puData, 16); }

   arguments& set(std::string_view stringName, int16_t v) { return set(stringName, eCTypeInt16, (const_pointer)&v, sizeof(int16_t)); }
   arguments& set(std::string_view stringName, uint16_t v) { return set(stringName, eCTypeUInt16, (const_pointer)&v, sizeof(uint16_t)); }
   arguments& set(std::string_view stringName, int32_t v) { return set(stringName, eCTypeInt32, (const_pointer)&v, sizeof(int32_t)); }
   arguments& set(std::string_view stringName, uint32_t v) { return set(stringName, eCTypeUInt32, (const_pointer)&v, sizeof(uint32_t)); }
   arguments& set(std::string_view stringName, int64_t v) { return set(stringName, eCTypeInt64, (const_pointer)&v, sizeof(int64_t)); }
   arguments& set(std::string_view stringName, uint64_t v) { return set(stringName, eCTypeUInt64, (const_pointer)&v, sizeof(uint64_t)); }
   arguments& set(std::string_view stringName, std::string_view v) { return set(stringName, (eCTypeString | eValueLength), (const_pointer)v.data(), (unsigned int)v.length() + 1); }

   arguments& set(std::string_view stringName, param_type uType, const_pointer pBuffer, unsigned int uLength) { return set(stringName.data(), (uint32_t)stringName.length(), uType, pBuffer, uLength); }
   arguments& set(const char* pbszName, uint32_t uNameLength, param_type uType, const_pointer pBuffer, unsigned int uLength);
   arguments& set(pointer pPosition, param_type uType, const_pointer pBuffer, unsigned int uLength);

   // TODO: Implement set methods

   const_iterator begin() const { return const_iterator( this, m_pBuffer ); }
   const_iterator end() const { return const_iterator( nullptr ); }

   [[nodiscard]] unsigned int capacity() const { assert(m_pBuffer != nullptr); return m_uBufferLength; }

/** \name COUNT
*///@{
   bool empty() const { return m_uLength == 0; }
   unsigned int count(std::string_view stringName) const;
//@}

/** \name FIND
* Find methods, finds position or param value for name
*///@{
   [[nodiscard]] pointer find(unsigned int uIndex);
   [[nodiscard]] pointer find(std::string_view stringName);
   [[nodiscard]] const_pointer find(std::string_view stringName) const;
   [[nodiscard]] const_pointer find(std::string_view stringName, const_pointer pPosition) const;
   std::vector<const_pointer> find_all(std::string_view stringName) const;
   /// find param value for name
   [[nodiscard]] argument find_param(std::string_view stringName) const {
      const_pointer pPosition = find(stringName);
      if( pPosition ) return get_argument_s(pPosition);
      return argument();
   }

   [[nodiscard]] argument_edit find_edit_param(std::string_view stringName) {
      const_pointer pPosition = find(stringName);
      if( pPosition ) return get_edit_param_s(this, pPosition);
      return argument_edit();
   }

   /// find param value for name
   [[nodiscard]] argument find_param(std::string_view stringName, const_pointer pPosition) const {
      pPosition = find(stringName, pPosition);
      if( pPosition ) return get_argument_s(pPosition);
      return argument();
   }
//@}

   [[nodiscard]] pointer next() { return m_uLength > 0 ? m_pBuffer : nullptr; }
   [[nodiscard]] const_pointer next() const { return m_uLength > 0 ? m_pBuffer : nullptr; }
   [[nodiscard]] pointer next(pointer pPosition) {                               assert( verify_d(pPosition) );
      auto p = next_s(pPosition);
      return p < get_buffer_end() ? p : nullptr;
   }
   [[nodiscard]] const_pointer next(const_pointer pPosition) const {             assert( verify_d(pPosition) );
      auto p = next_s(pPosition);
      return p < get_buffer_end() ? p : nullptr;
   }
   [[nodiscard]] size_t size() const;

   [[nodiscard]] argument get_argument() const { return get_argument_s(m_pBuffer); }
   [[nodiscard]] argument get_argument(const_pointer pPosition) const {                assert( verify_d(pPosition) );
      return get_argument_s(m_pBuffer); 
   }

   [[nodiscard]] argument get_argument(unsigned int uIndex) const;
   [[nodiscard]] argument get_argument(std::string_view stringName) const { return find_param(stringName); }
   template<class DEFAULT>
   [[nodiscard]] DEFAULT get_argument(std::string_view stringName, DEFAULT defaultValue) const {
      argument  v = find_param(stringName);
      if( v.is_null() ) return defaultValue;
      return static_cast<DEFAULT>(v);
   }
   [[nodiscard]] std::string get_argument(std::string_view stringName, const std::string& defaultValue) const {
      argument  v = find_param(stringName);
      if( v.is_null() ) return defaultValue;
      return v.get_string();
   }


   /**
    * Try to get value for param name, if not found then insert `vInsert` into params 
    * object and return param for that inserted value.
    * Make sure that vInsert is compatible with values that can be stored as param
    */
   template<typename INSERT_VALUE>
   [[nodiscard]] argument get_argument(std::string_view stringName, const INSERT_VALUE& vInsert) { 
      auto paramV = find_param(stringName); 
      if( paramV.empty() == true ) {
         auto uOffset = get_buffer_end() - get_buffer_start();
         append(stringName, vInsert);
         return arguments::get_param_s(get_buffer_start() + uOffset);
      }
      return argument();
   }

   /// return all values for name
   [[nodiscard]] std::vector<argument> get_argument_all(std::string_view stringName) { return get_argument_all_s(get_buffer_start(), get_buffer_end(), stringName); }

/** \name PRINT
* Methods used to format argument values into text
*///@{
   std::string print() const;
   std::string print(const_iterator itBegin) const { return print(itBegin, end(), ", "); };
   std::string print( const_iterator itBegin, const_iterator itEnd ) const { return print(itBegin, itEnd, ", "); };
   std::string print(const_iterator itBegin, const_iterator itEnd, std::string_view stringSplit) const;
   std::string print_json() const;
//@}

   


#ifdef _DEBUG
   bool verify_d(const_pointer pPosition) const { return pPosition >= m_pBuffer && pPosition < (m_pBuffer + m_uLength) ? true : false; }
#endif
//@}

/** \name BUFFER
*///@{
   /// make sure internal buffer can hold specified number of bytes, buffer data is copied if increased
   void reserve(unsigned int uCount);
   /// Remove param starting at position, remember that if you are string positions in buffer they are invalidated with this method
   void remove(const_pointer pPosition);
   void remove(const_iterator it) { remove(it); }
   /// make sure internal buffer can hold specified number of bytes, no copying just reserving data
   pointer reserve_no_copy(unsigned int uCount) {
      if( m_bOwner == false || m_pBuffer == nullptr || uCount > m_uBufferLength ) { return _reserve_no_copy(uCount + (uCount >> 1)); }
      return m_pBuffer;
   }
   pointer _reserve_no_copy(unsigned int uCount);

   int resize(pointer pPosition, int iOffset, int iNewOffset);
//@}

   static bool is_name_s(const_pointer pPosition) {
      assert(*pPosition != 0);
      return *pPosition == arguments::eCType_ParameterName;
   }
   std::string_view get_name(const_pointer pPosition) { return get_name_s( pPosition ); }

   static pointer move_to_value(pointer pPosition);
   static const_pointer move_to_value(const_pointer pPosition);


/** \name INTERNAL FREE FUNCTIONS
*///@{
   /// ## Compare logic
   /// Compare argument name if argument has name
   static bool compare_name_s(const_pointer pPosition, std::string_view stringName);
   /// compare arguments
   static bool compare_argument_s(const argument& argument1, const argument& argument2);
   static bool compare_argument_group_s(const argument& argument1, const argument& argument2);
   /// compare if argument type is fixed size, this is useful when setting values in arguments object
   static constexpr bool is_type_fixed_size_s(unsigned uType) { return uType <= eCTypeBinaryUuid; }

   /// compare type for type
   static inline bool compare_type_s(const argument& v1, const argument& v2) { return v1.type() == v2.type(); }
   static inline bool compare_type_s(const argument& v1, unsigned int uType ) { return v1.type() == (uType & ~CType_MASK); }

   /// ## name and type methods
   static std::string_view get_name_s(const_pointer pPosition) {
      assert(arguments::is_name_s(pPosition));
      pPosition++;                                                               // move past type (now on name length)
      return std::string_view(reinterpret_cast<const char*>(pPosition + 1), *pPosition);// generate string_view for name
   }

   /// ## argument methods
   /// return param the position points to
   static argument get_argument_s(const_pointer pPosition);
   /// return editable param based on position
   static argument_edit get_edit_param_s(arguments* parguments, const_pointer pPosition);
   /// count internal param length in bytes
   static unsigned int get_total_param_length_s(const_pointer pPosition);
   static unsigned int get_total_param_length_s(std::string_view stringName, const argument argumentValue);
   static std::vector<argument> get_argument_all_s(const_pointer pBegin, const_pointer pEnd, std::string_view stringName);

   /// ## move methods
   /// move pointer to next value in buffer
   static pointer next_s(pointer pPosition);
   static const_pointer next_s(const_pointer pPosition);

   /// ## Calculate size in bytes needed for argument values stored in arguments object
   static unsigned int sizeof_s(const argument& argumentValue);
   static unsigned int sizeof_s(uint32_t uNameLength, param_type uType, unsigned int uLength);
   static inline unsigned int sizeof_name_s(uint32_t uNameLength) { return uNameLength + 2; }
   static inline unsigned int sizeof_name_s(const_pointer pPosition) { 
      if( *pPosition == eCType_ParameterName ) return 2 + pPosition[1];          // name marker = 1 byte, name length = 1 byte and the total name length
      return 0;
   }

   static constexpr inline unsigned int sizeof_value_prefix(param_type uType) { return uType & eValueLength ? sizeof(uint32_t) + 1 : 1; }

   static inline unsigned int length_name_s(const_pointer pPosition) {
      if( *pPosition == eCType_ParameterName ) return  pPosition[1];            // the total name length found in byte with name length
      return 0;
   }

   /// ## Append arguments
   static void append_argument_s(arguments& arguments, const std::pair<std::string_view, gd::variant>& pairArgument) {
      arguments.append_argument(pairArgument);
   }

   //static void append_argument_s(arguments& arguments, const std::pair<std::string_view, gd::variant>& pairArgument) { arguments.append_argument(pairArgument); }
   template<typename First, typename... Argument>
   static void append_argument_s(arguments& rArguments, const First& pairArgument, Argument... pairNext) { 
      rArguments.append_argument(pairArgument); 
      append_argument_s(rArguments, pairNext...);
   }

   static void append_argument_s( arguments& rArguments, std::initializer_list<std::pair<std::string_view, gd::variant>> listPair ) {
      for( auto it : listPair ) rArguments.append_argument(it);
   }

   /// ## Create arguments object
   /// Create arguments object from pair
   static arguments create_s(const std::pair<std::string_view, gd::variant>& pairArgument) {
      arguments A;
      append_argument_s(A, pairArgument);
      return A;
   }

   /// Create arguments object from pair list
   static arguments create_s(std::initializer_list<std::pair<std::string_view, gd::variant>> listPair) {
      arguments A;
      for( auto it : listPair ) A.append_argument(it);
      return A;
   }

   /// ## dump information about values
   static inline std::string print_s(const_pointer pPosition) { return print_s(pPosition, ePairTypeAll); }
   static std::string print_s(const_pointer pPosition, uint32_t uPairType );

   /// Get raw type for value
   constexpr static unsigned int type_s(unsigned int uType) { return uType & ~CType_MASK; }
   
   /// ## `variant` methods
   /// get argument value as variant
   static gd::variant get_variant_s(const argument& argumentValue);
   static gd::variant get_variant_s(const argument& argumentValue, bool);
   static argument get_argument_s(const gd::variant& variantValue);
   //static arguments read_json_s(const argument& argumentValue);
//@}


   // ## attributes ----------------------------------------------------------------
public:
   bool           m_bOwner;      ///< if buffer is owned (delete in destructor)
   pointer        m_pBuffer;     ///< pointer to byte array
   unsigned int   m_uLength;     ///< length in use
   unsigned int   m_uBufferLength;///< length for byte array

   static size_type npos;

};




} // namespace _GD_PARAM_BEGIN