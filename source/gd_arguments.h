#pragma once
#include <cassert>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#include "gd_variant.h"
#include "gd_variant_view.h"


/*
| class | Description |
| - | - |
| argument | Manage values in `arguments` |
|   |   |

mdtable

// structured binding
// https://devblogs.microsoft.com/oldnewthing/20201015-00/?p=104369

*/


#pragma warning( disable : 26495 26812 )

#ifndef _GD_ARGUMENT_BEGIN
#define _GD_ARGUMENT_BEGIN namespace gd::argument {
#define _GD_ARGUMENT_END }
_GD_ARGUMENT_BEGIN
#else
_GD_SQL_QUERY_BEGIN
#endif


// ================================================================================================
// ================================================================================= arguments
// ================================================================================================


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

   struct pair_tag {};                                                           // used to select proper method, working with pair items instead of vector
   struct no_initializer_list_tag {};                                            // do not select initializer_list versions


public:

   /**
    * \brief group type for values in argument
    *
    */
   enum enumGroup
   {
      eGroupBoolean     = 0x01000000,  // boolean type
      eGroupInteger     = 0x02000000,  // integer types, both unsigned and signed
      eGroupDecimal     = 0x04000000,  // decimal values
      eGroupString      = 0x08000000,  // text values, both ascii, unicode, utf-32
      eGroupBinary      = 0x10000000,  // binary values
   };


   /**
    * \brief type values used for each argument in arguments class
    *
    */
   enum enumCType
   {
      // ## primitive types
      eTypeNumberUnknown      = 0,
      eTypeNumberBool         = 1,
      eTypeNumberInt8         = 2,
      eTypeNumberUInt8        = 3,
      eTypeNumberInt16        = 4,
      eTypeNumberUInt16       = 5,
      eTypeNumberInt32        = 6,
      eTypeNumberUInt32       = 7,
      eTypeNumberInt64        = 8,
      eTypeNumberUInt64       = 9,

      eTypeNumberFloat        = 10,
      eTypeNumberDouble       = 11,

      eTypeNumberPointer      = 12,  // pointer to memory

      eTypeNumberGuid         = 13, // universal unique identifier

      // ## derived types
      eTypeNumberString       = 14, // ascii string
      eTypeNumberUtf8String   = 15, // utf8 string
      eTypeNumberWString      = 16, // unicode string
      eTypeNumberUtf32String  = 17, // utf32 string

      eTypeNumberBinary       = 18, // binary data

      CType_MAX,


      eType_ParameterName,         // special type for parameter names

      eValueName = 0b00100000,
      eValueLength = 0b01000000,
      eValueArray = 0b10000000,

      eType_MASK = 0b11100000,      // mask for name, length and array markers in byte
      eCType_MASK = 0xffffff00,     // mask to extract byte from full 32 bit number
      eTypeNumber_MASK = (0xffffff00 + eType_MASK), // mask to extract type value
   };

      /*-----------------------------------------*/ /**
    * \brief One single type each supported value in variant
    *
    *
    */
   enum enumType
   {
      eTypeUnknown      = eTypeNumberUnknown,
      eTypeBool         = eTypeNumberBool     | eGroupBoolean,
      eTypeInt8         = eTypeNumberInt8     | eGroupInteger,
      eTypeInt16        = eTypeNumberInt16    | eGroupInteger,
      eTypeInt32        = eTypeNumberInt32    | eGroupInteger,
      eTypeInt64        = eTypeNumberInt64    | eGroupInteger,
      eTypeUInt8        = eTypeNumberUInt8    | eGroupInteger,
      eTypeUInt16       = eTypeNumberUInt16   | eGroupInteger,
      eTypeUInt32       = eTypeNumberUInt32   | eGroupInteger,
      eTypeUInt64       = eTypeNumberUInt64   | eGroupInteger,
      eTypeFloat        = eTypeNumberFloat    | eGroupDecimal,
      eTypeDouble       = eTypeNumberDouble   | eGroupDecimal,
      eTypePointer      = eTypeNumberPointer,
      eTypeGuid         = eTypeNumberGuid     | eGroupBinary,
      eTypeBinary       = eTypeNumberBinary   | eGroupBinary,
      eTypeUtf8String   = eTypeNumberUtf8String | eGroupString,
      eTypeUtf32String  = eTypeNumberUtf32String | eGroupString,
      eTypeString       = eTypeNumberString   | eGroupString,
      eTypeWString      = eTypeNumberWString  | eGroupString,
   };


                                                                                 static_assert(eTypeNumberUInt64 == variant_type::eTypeNumberUInt64); static_assert(eTypeNumberDouble == variant_type::eTypeNumberDouble); static_assert(eTypeNumberBinary == variant_type::eTypeNumberBinary);
                                                                                 static_assert( (CType_MAX & eType_MASK) == 0 );

   static const unsigned ARGUMENTS_NO_LENGTH = eTypeNumberGuid;

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
      argument(): m_eType(arguments::enumType::eTypeUnknown) {}
      //@{
      /// constructors for specified types
      argument(bool b) : m_eType(arguments::eTypeBool) { m_unionValue.b = b; }
      argument(int8_t v) : m_eType(arguments::eTypeInt8) { m_unionValue.v_int8 = v; }
      argument(uint8_t v) : m_eType(arguments::eTypeUInt8) { m_unionValue.v_uint8 = v; }
      argument(int16_t v) : m_eType(arguments::eTypeInt16) { m_unionValue.v_int16 = v; }
      argument(uint16_t v) : m_eType(arguments::eTypeUInt16) { m_unionValue.v_uint16 = v; }
      argument(int32_t v) : m_eType(arguments::eTypeInt32) { m_unionValue.v_int32 = v; }
      argument(uint32_t v) : m_eType(arguments::eTypeUInt32) { m_unionValue.v_uint32 = v; }
      argument(int64_t v) : m_eType(arguments::eTypeInt64) { m_unionValue.v_int64 = v; }
      argument(uint64_t v) : m_eType(arguments::eTypeUInt64) { m_unionValue.v_uint64 = v; }
      argument(float f) : m_eType(arguments::eTypeFloat) { m_unionValue.f = f; }
      argument(double d) : m_eType(arguments::eTypeDouble) { m_unionValue.d = d; }

      //param(binary_support::uuid* p) : m_eCType(params::eCustomTypeUuid) { m_unionValue.p = p; }

      argument(const char* pbsz) : m_eType( enumType(arguments::eTypeString) ) { m_unionValue.pbsz = pbsz; }
      argument(const wchar_t* pwsz) : m_eType( enumType(arguments::eTypeWString) ) { m_unionValue.pwsz = pwsz; }
      argument(arguments::enumCType eType, const char* pbsz) : m_eType(enumType(eType)) { m_unionValue.pbsz = pbsz; }
      argument(arguments::enumCType eType, const wchar_t* pwsz) : m_eType(enumType(eType)) { m_unionValue.pwsz = pwsz; }
      argument(void* p) : m_eType(arguments::eTypePointer) { m_unionValue.p = p; }
      argument(const uint8_t* p) : m_eType( enumType(arguments::eTypeBinary) ) { m_unionValue.puch = p; }
      argument(arguments::enumType eType,const uint8_t* p) : m_eType(eType) { m_unionValue.puch = p; }
      argument(unsigned uType,const uint8_t* p) : m_eType((enumType)uType) { m_unionValue.puch = p; }
      argument(const uint8_t* p, enumType eType) : m_eType(eType) { m_unionValue.puch = p; }
      
      //@}

      argument(const argument& o) {
         common_construct(o);
      }
      argument& operator=(const argument& o) {
         common_construct(o);
         return *this;
      }

      void common_construct(const argument& o) {
         m_eType = o.m_eType;
         m_unionValue.d = o.m_unionValue.d;
      }

      bool operator==(const argument& o) const { return compare_argument_s(*this, o); }
      bool operator==(const gd::variant_view& v) const { return compare_argument_s(*this, v); }


      bool operator !() { return (type_number() == arguments::eTypeNumberUnknown); }

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


      operator bool() const { assert(type_number() == arguments::eTypeNumberBool); return m_unionValue.b; }
      operator int8_t() const { assert(type_number() >= arguments::eTypeNumberInt8 && type_number() <= arguments::eTypeNumberUInt8); return m_unionValue.v_int8; }
      operator uint8_t() const { assert(type_number() >= arguments::eTypeNumberInt8 && type_number() <= arguments::eTypeNumberUInt8); return m_unionValue.v_uint8; }
      operator int16_t() const { assert(type_number() >= arguments::eTypeNumberInt16 && type_number() <= arguments::eTypeNumberUInt16); return m_unionValue.v_int16; }
      operator uint16_t() const { assert(type_number() >= arguments::eTypeNumberInt16 && type_number() <= arguments::eTypeNumberUInt16); return m_unionValue.v_uint16; }
      operator long() const { assert(type_number() >= arguments::eTypeNumberInt32 && type_number() <= arguments::eTypeNumberUInt32); return m_unionValue.v_int32; }
      operator int32_t() const { assert(type_number() >= arguments::eTypeNumberInt32 && type_number() <= arguments::eTypeNumberUInt32); return m_unionValue.v_int32; }
      operator uint32_t() const { assert(type_number() >= arguments::eTypeNumberInt32 && type_number() <= arguments::eTypeNumberUInt32); return m_unionValue.v_uint32; }
      operator int64_t() const { assert(type_number() >= arguments::eTypeNumberInt64 && type_number() <= arguments::eTypeNumberUInt64); return m_unionValue.v_int64; }
      operator uint64_t() const { assert(type_number() >= arguments::eTypeNumberInt64 && type_number() <= arguments::eTypeNumberUInt64); return m_unionValue.v_uint64; }
      operator double() const { assert(type_number() == arguments::eTypeNumberDouble); return m_unionValue.d; }
      operator const char* () const { assert(type_number() == arguments::eTypeNumberUnknown || type_number() == arguments::eTypeNumberString); return (type_number() == arguments::eTypeNumberString) ? m_unionValue.pbsz : ""; }
      operator const wchar_t* () const { assert(type_number() == arguments::eTypeNumberUnknown || type_number() == arguments::eTypeNumberWString); return (type_number() == arguments::eTypeNumberWString) ? m_unionValue.pwsz : L""; }
      operator std::string() const { assert(type_number() == arguments::eTypeNumberUnknown || type_number() == arguments::eTypeNumberString); return (type_number() == arguments::eTypeNumberString) ? m_unionValue.pbsz : ""; }
      operator std::wstring() const { assert(type_number() == arguments::eTypeNumberUnknown || type_number() == arguments::eTypeNumberWString); return (type_number() == arguments::eTypeNumberWString) ? m_unionValue.pwsz : L""; }
      operator void* () const { assert(type_number() == arguments::eTypeNumberUnknown || type_number() == arguments::eTypeNumberPointer); return (type_number() == arguments::eTypeNumberPointer) ? m_unionValue.p : NULL; }

      /// compare two argument values
      bool compare( const argument& o ) const { return compare_argument_s(*this, o); }
      bool compare( const gd::variant_view& o ) const { return compare_s(*this, o); }
      /// compare within group type, if integer all sizes are valid for comparison
      bool compare_group( const argument& o ) const { return compare_argument_group_s(*this, o); }

      /// length in bytes for param
      unsigned int length() const;
      /// get param type
      arguments::enumType type() const { return arguments::enumType((unsigned)m_eType & ~eType_MASK); }
      arguments::enumCType type_number() const { return arguments::enumCType((unsigned)m_eType & ~eTypeNumber_MASK); }
      /// return the raw internal type, this has optional flags for type
      unsigned int ctype() const { return (unsigned int)(m_eType & ~eCType_MASK); }
      /// check if param is empty
      bool empty() const { return (m_eType == arguments::eTypeUnknown); }

      void get_binary_as_hex(std::string& s) const;
      unsigned int get_binary_as_hex(char* pbsz, unsigned int uLength) const;

      /// reset param 
      void reset(const argument* pParam = nullptr) {
         if( pParam != nullptr ) {
            *this = *pParam;
         }
         else {
            m_eType = arguments::eTypeUnknown;
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
      gd::variant_view  get_variant_view() const { return arguments::get_variant_view_s(*this); }
      gd::variant  get_variant( bool ) const { return arguments::get_variant_s(*this, false); } /// for speed, do not copy data
      value        get_value() { return m_unionValue; }
      const value& get_value() const { return m_unionValue; }

      std::string  to_string() const { return get_string(); }

      bool         is_null() const { return (m_eType == arguments::eTypeUnknown); }
      bool         is_bool() const { return (m_eType == arguments::eTypeBool); }
      bool         is_int32() const { return (m_eType == arguments::eTypeInt32); }
      bool         is_uint32() const { return (m_eType == arguments::eTypeUInt32); }
      bool         is_int64() const { return (m_eType == arguments::eTypeInt64); }
      bool         is_uint64() const { return (m_eType == arguments::eTypeUInt64); }
      bool         is_double() const { return (m_eType == arguments::eTypeDouble); }
      bool         is_string() const { return (m_eType == arguments::eTypeString); }
      bool         is_wstring() const { return (m_eType == arguments::eTypeWString); }
      bool         is_true() const;
      bool         is_primitive() const { return (type_number() > arguments::eTypeNumberUnknown && type_number() <= eTypeNumberDouble); } ///< primitive = built in types in C++
      bool         is_text() const { return (m_eType & arguments::eGroupString) != 0;  } ///< text = some sort of string value, ascii, utf8 or unicode
      bool         is_binary() const { return m_eType == arguments::eTypeBinary; } ///< binary = blob data, length is unknown if used in argument (work with this in arguments)
      bool         is_number() const { return (m_eType & (arguments::eGroupInteger|arguments::eGroupDecimal)) != 0; }
      bool         is_decimal() const { return (m_eType & arguments::eGroupDecimal) != 0; }
      bool         is_integer() const { return (m_eType & arguments::eGroupInteger) != 0; }

      void*        get_raw_pointer() const { return m_unionValue.p; }            ///< return raw pointer to value
      void*        get_value_buffer() const { return (void*)&m_unionValue; }     ///< return address pointer to value

      // attributes
   public:
      arguments::enumType m_eType;      // type of value valid for m_unionValue 
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
         m_pValue = move_to_value_s( (pointer)pPosition );
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
   arguments(const std::string_view& stringName, const gd::variant& variantValue, no_initializer_list_tag);


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
   arguments(std::initializer_list<std::pair<std::string_view, gd::variant>> listPair); // construct arguments with vector like {{},{}}

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
protected:
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

   // ## append adds values to stream
   //    note: remember that each value has its type and type in stream is just
   //    one byte. That means that the amount of information about the type is
   //    limited. This is the reason why each type only has it's type number.

   arguments& append(std::nullptr_t) { return append(eTypeNumberUnknown, nullptr, 0); }
   arguments& append(int8_t v) { return append(eTypeNumberInt8, (const_pointer)&v, sizeof(int8_t)); }
   arguments& append(uint8_t v) { return append(eTypeNumberUInt8, (const_pointer)&v, sizeof(uint8_t)); }
   arguments& append(int16_t v) { return append(eTypeNumberInt16, (const_pointer)&v, sizeof(int16_t)); }
   arguments& append(uint16_t v) { return append(eTypeNumberUInt16, (const_pointer)&v, sizeof(uint16_t)); }
   arguments& append(int32_t v) { return append(eTypeNumberInt32, (const_pointer)&v, sizeof(int32_t)); }
   arguments& append(uint32_t v) { return append(eTypeNumberUInt32, (const_pointer)&v, sizeof(uint32_t)); }
   arguments& append(int64_t v) { return append(eTypeNumberInt64, (const_pointer)&v, sizeof(int64_t)); }
   arguments& append(uint64_t v) { return append(eTypeNumberUInt64, (const_pointer)&v, sizeof(uint64_t)); }
   arguments& append(std::string_view v) { return append((eTypeNumberString | eValueLength), (const_pointer)v.data(), (unsigned int)v.length() + 1); }
   arguments& append(std::wstring_view v) { return append((eTypeNumberWString | eValueLength), (const_pointer)v.data(), ((unsigned int)v.length() + 1) * sizeof(wchar_t)); }
   arguments& append(param_type uType, const_pointer pBuffer, unsigned int uLength);

   arguments& append(std::string_view stringName, std::nullptr_t) { return append(stringName, eTypeNumberUnknown, nullptr, 0); }
   arguments& append(std::string_view stringName, int8_t v) { return append(stringName, eTypeNumberInt8, (const_pointer)&v, sizeof(int8_t)); }
   arguments& append(std::string_view stringName, uint8_t v) { return append(stringName, eTypeNumberUInt8, (const_pointer)&v, sizeof(uint8_t)); }
   arguments& append(std::string_view stringName, int16_t v) { return append(stringName, eTypeNumberInt16, (const_pointer)&v, sizeof(int16_t)); }
   arguments& append(std::string_view stringName, uint16_t v) { return append(stringName, eTypeNumberUInt16, (const_pointer)&v, sizeof(uint16_t)); }
   arguments& append(std::string_view stringName, int32_t v) { return append(stringName, eTypeNumberInt32, (const_pointer)&v, sizeof(int32_t)); }
   arguments& append(std::string_view stringName, uint32_t v) { return append(stringName, eTypeNumberUInt32, (const_pointer)&v, sizeof(uint32_t)); }
   arguments& append(std::string_view stringName, int64_t v) { return append(stringName, eTypeNumberInt64, (const_pointer)&v, sizeof(int64_t)); }
   arguments& append(std::string_view stringName, uint64_t v) { return append(stringName, eTypeNumberUInt64, (const_pointer)&v, sizeof(uint64_t)); }
   arguments& append(std::string_view stringName, std::string_view v) { return append(stringName, (eTypeNumberString | eValueLength), (const_pointer)v.data(), (unsigned int)v.length() + 1); }
   arguments& append(std::string_view stringName, std::wstring_view v) { return append(stringName, (eTypeNumberWString | eValueLength), (const_pointer)v.data(), ((unsigned int)v.length() + 1) * sizeof(wchar_t)); }
   arguments& append(std::string_view stringName, param_type uType, const_pointer pBuffer, unsigned int uLength) { return append(stringName.data(), (uint32_t)stringName.length(), uType, pBuffer, uLength); }
   arguments& append(const char* pbszName, uint32_t uNameLength, param_type uType, const_pointer pBuffer, unsigned int uLength);
   template<typename POINTER>
   arguments& append(const char* pbszName, uint32_t uNameLength, param_type uType, POINTER pBuffer, unsigned int uLength) {
      static_assert(std::is_pointer<POINTER>::value);
      return append(pbszName, uNameLength, uType, (const_pointer)pBuffer, uLength);
   }


   arguments& append_argument(std::string_view stringName, argument argumentValue) {
      auto _l = argumentValue.length();
      const_pointer pData = (argumentValue.type_number() <= eTypeNumberPointer ? (const_pointer)&argumentValue.m_unionValue : (const_pointer)argumentValue.get_raw_pointer());
      return append(stringName, argumentValue.ctype(), pData, argumentValue.length());
   }

   arguments& append_argument(const variant& variantValue);

   arguments& append_argument(std::string_view stringName, const gd::variant& variantValue) {
      auto argumentValue = get_argument_s(variantValue);
      const_pointer pData = (argumentValue.type_number() <= eTypeNumberPointer ? (const_pointer)&argumentValue.m_unionValue : (const_pointer)argumentValue.get_raw_pointer());
      unsigned uType = argumentValue.type_number();
      if( uType > ARGUMENTS_NO_LENGTH ) { uType |= eValueLength; }
      return append(stringName, uType, pData, argumentValue.length());
   }

   arguments& append_argument(std::string_view stringName, const gd::variant_view& variantValue);

   arguments& append_argument(const std::pair<std::string_view, gd::variant>& pairArgument) {
      return append_argument(pairArgument.first, pairArgument.second);
   }


   arguments& append_binary( const uint8_t* puData, unsigned int uLength ) { return append(eTypeNumberBinary, puData, uLength); }
   arguments& append_binary(std::string_view stringName, const uint8_t* puData, unsigned int uLength) { return append(stringName, (eTypeNumberBinary | eValueLength), puData, uLength); }
   arguments& append_uuid( const uint8_t* puData ) { return append(eTypeNumberGuid, puData, 16); }
   arguments& append_uuid(std::string_view stringName, const uint8_t* puData ) { return append(stringName, eTypeNumberGuid, puData, 16); }

   arguments& set(std::string_view stringName, std::nullptr_t) { return set(stringName, eTypeNumberBool, nullptr, 0); }
   arguments& set(std::string_view stringName, bool v) { return set(stringName, eTypeNumberBool, (const_pointer)&v, sizeof(bool)); }
   arguments& set(std::string_view stringName, int8_t v) { return set(stringName, eTypeNumberInt8, (const_pointer)&v, sizeof(int8_t)); }
   arguments& set(std::string_view stringName, uint8_t v) { return set(stringName, eTypeNumberUInt8, (const_pointer)&v, sizeof(uint8_t)); }
   arguments& set(std::string_view stringName, int16_t v) { return set(stringName, eTypeNumberInt16, (const_pointer)&v, sizeof(int16_t)); }
   arguments& set(std::string_view stringName, uint16_t v) { return set(stringName, eTypeNumberUInt16, (const_pointer)&v, sizeof(uint16_t)); }
   arguments& set(std::string_view stringName, int32_t v) { return set(stringName, eTypeNumberInt32, (const_pointer)&v, sizeof(int32_t)); }
   arguments& set(std::string_view stringName, uint32_t v) { return set(stringName, eTypeNumberUInt32, (const_pointer)&v, sizeof(uint32_t)); }
   arguments& set(std::string_view stringName, int64_t v) { return set(stringName, eTypeNumberInt64, (const_pointer)&v, sizeof(int64_t)); }
   arguments& set(std::string_view stringName, uint64_t v) { return set(stringName, eTypeNumberUInt64, (const_pointer)&v, sizeof(uint64_t)); }
   arguments& set(std::string_view stringName, std::string_view v) { return set(stringName, (eTypeNumberString | eValueLength), (const_pointer)v.data(), (unsigned int)v.length() + 1); }

   arguments& set(std::string_view stringName, const gd::variant_view& variantValue) {
      auto argumentValue = get_argument_s(variantValue);
      const_pointer pData = (argumentValue.type_number() <= eTypeNumberPointer ? (const_pointer)&argumentValue.m_unionValue : (const_pointer)argumentValue.get_raw_pointer());
      unsigned uType = argumentValue.type_number();
      unsigned uLength;
      if( uType > ARGUMENTS_NO_LENGTH ) 
      { 
         unsigned uZeroEnd = 0;
         if( uType == eTypeNumberWString )
         uType |= eValueLength; 
         uLength = variantValue.length() + get_string_zero_terminate_length_s( uType );
      }
      return set(stringName, uType, pData, uLength);
   }

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
   [[nodiscard]] const_pointer find(const std::pair<std::string_view, gd::variant_view>& pairMatch) const;
   std::vector<const_pointer> find_all(std::string_view stringName) const;
   /// find param value for name
   [[nodiscard]] argument find_argument(std::string_view stringName) const {
      const_pointer pPosition = find(stringName);
      if( pPosition ) return get_argument_s(pPosition);
      return argument();
   }

   /// return argument object that can be used to edit value if found
   [[nodiscard]] argument_edit find_edit_argument(std::string_view stringName);
   /// find param value for name and start from position to search
   [[nodiscard]] argument find_argument(std::string_view stringName, const_pointer pPosition) const;

//@}

/** \name COMPARE
* compare functionality, checks if values in arguments are equal
*///@{
   [[nodiscard]] bool compare(const std::pair<std::string_view, gd::variant_view>& pairMatch) const { return find(pairMatch) != nullptr; }
   [[nodiscard]] bool compare(const std::string_view stringName, const arguments& argumentsCompareTo) const;
//@}

/** \name MOVE
* move operations used to move between values, can't go back. only forward
*///@{
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
//@}

   [[nodiscard]] size_t size() const;

/** \name ARGUMENT
* get argument value from arguments
*///@{
   [[nodiscard]] argument get_argument() const { return get_argument_s(m_pBuffer); }
   [[nodiscard]] argument get_argument(const_pointer pPosition) const {                assert( verify_d(pPosition) );
      return get_argument_s(m_pBuffer); 
   }

   [[nodiscard]] argument get_argument(unsigned int uIndex) const;
   [[nodiscard]] argument get_argument(std::string_view stringName) const { return find_argument(stringName); }
   template<class DEFAULT>
   [[nodiscard]] DEFAULT get_argument(std::string_view stringName, DEFAULT defaultValue) const {
      argument  v = find_argument(stringName);
      if( v.is_null() ) return defaultValue;
      return static_cast<DEFAULT>(v);
   }
   [[nodiscard]] std::string get_argument(std::string_view stringName, const std::string& stringDefault) const {
      argument  v = find_argument(stringName);
      if( v.is_null() ) return stringDefault;
      return v.get_string();
   }

   /**
    * Try to get value for param name, if not found then insert `vInsert` into params 
    * object and return param for that inserted value.
    * Make sure that vInsert is compatible with values that can be stored as param
    */
   template<typename INSERT_VALUE>
   [[nodiscard]] argument get_argument(std::string_view stringName, const INSERT_VALUE& vInsert) { 
      auto paramV = find_argument(stringName); 
      if( paramV.empty() == true ) {
         auto uOffset = get_buffer_end() - get_buffer_start();
         append(stringName, vInsert);
         return arguments::get_param_s(get_buffer_start() + uOffset);
      }
      return argument();
   }

   /// return all values for name
   [[nodiscard]] std::vector<argument> get_argument_all(std::string_view stringName) { return get_argument_all_s(get_buffer_start(), get_buffer_end(), stringName); }
//@}

/** \name PRINT
* Methods used to format argument values into text
*///@{
   std::string print() const;
   std::string print(const_iterator itBegin) const { return print(itBegin, end(), ", "); };
   std::string print( const_iterator itBegin, const_iterator itEnd ) const { return print(itBegin, itEnd, ", "); };
   std::string print(const_iterator itBegin, const_iterator itEnd, std::string_view stringSplit) const;
   std::string print_json() const;

   std::string print(std::string_view stringFormat) const;
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
      return *pPosition == arguments::eType_ParameterName;
   }
   std::string_view get_name(const_pointer pPosition) { return get_name_s( pPosition ); }

//   static pointer move_to_value(pointer pPosition)  { return move_to_value_s( pPosition ); }
//   static const_pointer move_to_value(const_pointer pPosition) { return move_to_value_s( pPosition ); }


/** \name INTERNAL FREE FUNCTIONS
*///@{
   /// ## Move logic
   static pointer move_to_value_s(pointer pPosition);
   static const_pointer move_to_value_s(const_pointer pPosition);


   /// ## Compare logic
   /// Compare argument name if argument has name
   static bool compare_name_s(const_pointer pPosition, std::string_view stringName);
   /// compare arguments
   static bool compare_argument_s(const argument& argument1, const argument& argument2);
   static bool compare_argument_s(const argument& a, const gd::variant_view& v);
   static bool compare_argument_group_s(const argument& argument1, const argument& argument2);
   static bool compare_argument_group_s(const argument& a1, const gd::variant_view& v2);
   /// compare if argument type is fixed size, this is useful when setting values in arguments object
   static constexpr bool is_type_fixed_size_s(unsigned uType) { return (uType & ~eTypeNumber_MASK) <= eTypeNumberGuid; }

   /// compare type for type
   static inline bool compare_type_s(const argument& v1, const argument& v2) { return v1.type_number() == v2.type_number(); }
   static inline bool compare_type_s(const argument& v1, unsigned int uType ) { return v1.type_number() == (uType & ~eTypeNumber_MASK); }

   static bool compare_s(const argument& v1, const gd::variant_view v2);

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
      if( *pPosition == eType_ParameterName ) return 2 + pPosition[1];           // name marker = 1 byte, name length = 1 byte and the total name length
      return 0;
   }

   static constexpr inline unsigned int sizeof_value_prefix(param_type uType) { return uType & eValueLength ? sizeof(uint32_t) + 1 : 1; }

   static inline unsigned int length_name_s(const_pointer pPosition) {
      if( *pPosition == eType_ParameterName ) return  pPosition[1];              // the total name length found in byte with name length
      return 0;
   }

   /// ## Append arguments

   /// append pair to arguments
   static void append_argument_s(arguments& arguments, const std::pair<std::string_view, gd::variant>& pairArgument) {
      arguments.append_argument(pairArgument);
   }

   /// append multiple pairs
   template<typename First, typename... Argument>
   static void append_argument_s(arguments& rArguments, const First& pairArgument, Argument... pairNext) { 
      rArguments.append_argument(pairArgument); 
      append_argument_s(rArguments, pairNext...);
   }

   /// append pairs using initializer list
   static void append_argument_s( arguments& rArguments, std::initializer_list<std::pair<std::string_view, gd::variant>> listPair ) {
      for( auto it : listPair ) rArguments.append_argument(it);
   }

   /// ## Create arguments object
   /// 
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

   /// Create arguments object from arguments
   static arguments create_s(const std::string_view& stringName, const gd::variant& variantValue, no_initializer_list_tag) {
      arguments A(stringName, variantValue, no_initializer_list_tag{});
      return A;
   }


   /// ## dump information about values
   static inline std::string print_s(const_pointer pPosition) { return print_s(pPosition, ePairTypeAll); }
   static std::string print_s(const_pointer pPosition, uint32_t uPairType );

   /// ## find out type for value
   constexpr static unsigned int type_s(unsigned int uType) { return uType & ~eType_MASK; } // only type (no size)
   constexpr static unsigned int ctype_s(unsigned int uType) { return uType & ~eCType_MASK; } // last byte (type and size)
   constexpr static unsigned int type_number_s(unsigned int uType) { return uType & ~eTypeNumber_MASK; }
   constexpr static std::string_view type_name_s(uint32_t uType);
   

   /// count zero terminator length in bytes if type is some sort of string
   constexpr static unsigned int get_string_zero_terminate_length_s(unsigned int uType) { 
      uType = uType & ~eTypeNumber_MASK;  
      switch( uType ) {
         case eTypeNumberString:
         case eTypeNumberUtf8String:
            return 1;
         case eTypeNumberWString:
            return 2;
         case eTypeNumberUtf32String:
            return 4;
      }
      return 0;
   }
   
   /// ## `variant` methods
   /// get argument value as variant
   static gd::variant get_variant_s(const argument& argumentValue);
   static gd::variant get_variant_s(const argument& argumentValue, bool);
   static gd::variant_view get_variant_view_s(const argument& argumentValue);
   static argument get_argument_s(const gd::variant& variantValue);
   static argument get_argument_s(const gd::variant_view& variantValue);

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

// ================================================================================================
// ================================================================================= arguments_return
// ================================================================================================

/**
 * \brief simplifies using type deduction to return value as a pair
 *
 * arguments_return is just to simplify how to write code returning values.
 * constructing `arguments` needs two "{{ }}" and �arguments_return� only needs one like {}
 *
 \code
 // sample on how to return
 return { "return", true }
 \endcode
 */
class arguments_return : public arguments
{
public:
   arguments_return( std::pair<std::string_view, gd::variant> pairArgument ) {          // construct with pair (useful returning arguments from function)
      zero();
      append_argument(pairArgument);
   }
};

/// return argument object that can be used to edit value
inline arguments::argument_edit arguments::find_edit_argument(std::string_view stringName) {
   const_pointer pPosition = find(stringName);
   if( pPosition ) return get_edit_param_s(this, pPosition);
   return argument_edit();
}

/// Get argument value and start from offset position in buffer, use this if there are multiple values with same name
inline arguments::argument arguments::find_argument(std::string_view stringName, const_pointer pPosition) const {
   pPosition = find(stringName, pPosition);
   if( pPosition ) return get_argument_s(pPosition);
   return argument();
}

/// compare if value is equal for specified name
/// comparing text is faster with this internal compare method because no object is created on heap holding argument value
inline bool arguments::compare(const std::string_view stringName, const arguments& argumentsCompareTo) const {
   const argument argumentCompare = get_argument(stringName);
   if( argumentCompare.is_null() ) return false;
   const argument argumentCompareTo = argumentsCompareTo.get_argument(stringName);
   return compare_argument_s(argumentCompare, argumentCompareTo);
}

// ================================================================================================
// ================================================================================= FREE FUNCTIONS
// ================================================================================================


constexpr std::string_view arguments::type_name_s(uint32_t uType)
{
   const auto uNumberType = uType &  ~arguments::eTypeNumber_MASK;
   switch( uNumberType )
   {
   case arguments::eTypeNumberUnknown     : return "unknown";
   case arguments::eTypeNumberBool        : return "bool";
   case arguments::eTypeNumberInt8        : return "int8";
   case arguments::eTypeNumberUInt8       : return "uint8";
   case arguments::eTypeNumberInt16       : return "int16";
   case arguments::eTypeNumberUInt16      : return "uint16";
   case arguments::eTypeNumberInt32       : return "int32";
   case arguments::eTypeNumberUInt32      : return "uint32";
   case arguments::eTypeNumberInt64       : return "int64";
   case arguments::eTypeNumberUInt64      : return "uint64";
   case arguments::eTypeNumberFloat       : return "float";
   case arguments::eTypeNumberDouble      : return "double";
   case arguments::eTypeNumberPointer     : return "pointer";
   case arguments::eTypeNumberGuid        : return "guid";
   case arguments::eTypeNumberString      : return "ascii";
   case arguments::eTypeNumberUtf8String  : return "utf8";
   case arguments::eTypeNumberWString     : return "unicode";
   case arguments::eTypeNumberUtf32String : return "utf32";
   case arguments::eTypeNumberBinary      : return "binary";
   default:                                                                      assert(false);
      return "ERROR";
   }
}


_GD_ARGUMENT_END