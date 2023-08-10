/**
 * \file gd_types.h
 * 
 * \brief Core constants for gd type system
 * 
 * gd code is based on a type system where each primitive type has a number. This
 * type system is vital for all gd code and with that type system different logic
 * within gd is able to communicate without knowing about code in different parts.
 * 
 */



#pragma once

#include <cassert>
#include <stdint.h>
#include <string_view>
#include <vector>

#ifndef _GD_TYPES
#define _GD_TYPES
#endif

#ifndef _GD_TYPES_BEGIN
#define _GD_TYPES_BEGIN namespace gd { namespace types {
#define _GD_TYPES_END } }
_GD_TYPES_BEGIN
#else
_GD_TYPES_BEGIN
#endif


#if defined( __clang__ )
   #pragma GCC diagnostic push
   #pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#elif defined( __GNUC__ )
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#elif defined( _MSC_VER )
   #pragma warning(push)
   #pragma warning( disable : 4267 26495 26812 )
#endif

/// methods used to ask compiler for information
struct tag_ask_compiler {};




/*-----------------------------------------*/ /**
   * \brief type numbers for common data types
   *
   * Numbers for common types used i gd code.
   * Using these numbers as a form of meta data in code, it will simply
   * to make code more compatible. Simplify to move data between objects.
   */
enum enumTypeNumber
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
   eTypeNumberPointer      = 12,
   eTypeNumberGuid         = 13,

   // ## derived types
   eTypeNumberString       = 14,
   eTypeNumberUtf8String   = 15,
   eTypeNumberWString      = 16,
   eTypeNumberUtf32String  = 17,
   eTypeNumberBinary       = 18,
   eTypeNumberJson         = 19,
   eTypeNumberXml          = 20,
   eTypeNumberCsv          = 21,
   eTypeNumberVoid         = 22,
   eTypeNumberBit          = 23,
   eTypeNumberInt128       = 24,
   eTypeNumberUInt128      = 25,
   eTypeNumberInt256       = 26,
   eTypeNumberUInt256      = 27,
   eTypeNumberInt512       = 28,
   eTypeNumberUInt512      = 29,
   eTypeNumberHex          = 30,
   eTypeNumberBase32       = 31,
   eTypeNumberDateTime     = 32,
   eTypeNumberDate         = 33,
   eTypeNumberTime         = 34,
   eTypeNumberNumeric      = 35,
   eTypeNumberDecimal      = 36,
   eTypeNumberUuidString   = 37,

   eTypeNumberMAX          = 37,
};

/*-----------------------------------------*/ /**
 * \brief Flags for common type groups for value types
 * 
 * Group type values to make it easer to build logic around them.
 */
enum enumTypeGroup
{
   eTypeGroupNumber        = 0x0000'0100,                                      // type = number value
   eTypeGroupInteger       = 0x0000'0200,                                      // type = integer value
   eTypeGroupDecimal       = 0x0000'0400,                                      // type = decimal value
   eTypeGroupSigned        = 0x0000'0800,                                      // type = signed number value
   eTypeGroupString        = 0x0000'1000,                                      // type = string type
   eTypeGroupDate          = 0x0000'2000,                                      // type = date type
   eTypeGroupBinary        = 0x0000'4000,                                      // type = binary data
   eTypeGroupBoolean       = 0x0000'8000,                                      // type = boolean

   eTypeGroupSize08        = 0x0001'0000,                                      // size = 8 bit
   eTypeGroupSize16        = 0x0002'0000,                                      // size = 16 bit
   eTypeGroupSize32        = 0x0004'0000,                                      // size = 32 bit
   eTypeGroupSize64        = 0x0008'0000,                                      // size = 64 bit
   eTypeGroupSize128       = 0x0010'0000,                                      // size = 128 bit
   eTypeGroupSize256       = 0x0020'0000,                                      // size = 256 bit
   eTypeGroupSize512       = 0x0040'0000,                                      // size = 512 bit
};

enum enumTypeDetail
{
   eTypeDetailReference    = 0x0100'0000,                                      // reference to value
};

/*-----------------------------------------*/ /**
   * \brief Combined information for type
   *
   *
   */
enum enumType
{
   eTypeUnknown      = eTypeNumberUnknown,
   eTypeBool         = eTypeNumberBool       | eTypeGroupBoolean  | eTypeGroupSize08,
   eTypeInt8         = eTypeNumberInt8       | eTypeGroupInteger  | eTypeGroupSize08   | eTypeGroupSigned,
   eTypeInt16        = eTypeNumberInt16      | eTypeGroupInteger  | eTypeGroupSize16   | eTypeGroupSigned,
   eTypeInt32        = eTypeNumberInt32      | eTypeGroupInteger  | eTypeGroupSize32   | eTypeGroupSigned,
   eTypeInt64        = eTypeNumberInt64      | eTypeGroupInteger  | eTypeGroupSize64   | eTypeGroupSigned,

   eTypeInt128       = eTypeNumberInt128     | eTypeGroupInteger  | eTypeGroupSize128,
   eTypeInt256       = eTypeNumberInt256     | eTypeGroupInteger  | eTypeGroupSize256,
   eTypeInt512       = eTypeNumberInt512     | eTypeGroupInteger  | eTypeGroupSize512,

   eTypeUInt8        = eTypeNumberUInt8      | eTypeGroupInteger  | eTypeGroupSize08,
   eTypeUInt16       = eTypeNumberUInt16     | eTypeGroupInteger  | eTypeGroupSize16,
   eTypeUInt32       = eTypeNumberUInt32     | eTypeGroupInteger  | eTypeGroupSize32,
   eTypeUInt64       = eTypeNumberUInt64     | eTypeGroupInteger  | eTypeGroupSize64,

   eTypeUInt128      = eTypeNumberUInt128    | eTypeGroupInteger  | eTypeGroupSize128,
   eTypeUInt256      = eTypeNumberUInt256    | eTypeGroupInteger  | eTypeGroupSize256,
   eTypeUInt512      = eTypeNumberUInt512    | eTypeGroupInteger  | eTypeGroupSize512,

   eTypeCFloat       = eTypeNumberFloat      | eTypeGroupDecimal  | eTypeGroupSize32,
   eTypeCDouble      = eTypeNumberDouble     | eTypeGroupDecimal  | eTypeGroupSize64,
   eTypePointer      = eTypeNumberPointer,
   eTypeGuid         = eTypeNumberGuid       | eTypeGroupBinary   | eTypeGroupSize128,
   eTypeBinary       = eTypeNumberBinary     | eTypeGroupBinary,
   eTypeString       = eTypeNumberString     | eTypeGroupString,
   eTypeUtf8String   = eTypeNumberUtf8String | eTypeGroupString,
   eTypeWString      = eTypeNumberWString    | eTypeGroupString,
   eTypeUtf32String  = eTypeNumberUtf32String | eTypeGroupString,
   eTypeJson         = eTypeNumberJson       | eTypeGroupString,
   eTypeXml          = eTypeNumberXml        | eTypeGroupString,
   eTypeVoid         = eTypeNumberVoid,
   eTypeBit          = eTypeNumberBit        | eTypeGroupBoolean,

   eTypeRBinary      = eTypeNumberBinary     | eTypeGroupBinary  | eTypeDetailReference,
   eTypeRString      = eTypeNumberString     | eTypeGroupString  | eTypeDetailReference,
   eTypeRUtf8String  = eTypeNumberUtf8String | eTypeGroupString  | eTypeDetailReference,
   eTypeRWString     = eTypeNumberUtf8String | eTypeGroupString  | eTypeDetailReference

};

namespace detail {
   /// helper method used to convert first four characters into 32 bit unsigned integer value
   constexpr uint32_t hash_type( std::string_view stringType )
   {
      uint32_t uHashValue = (uint32_t)stringType[0];
      uHashValue += (uint32_t)stringType[1] << 8;
      uHashValue += (uint32_t)stringType[2] << 16;
      uHashValue += (uint32_t)stringType[3] << 24;

      return uHashValue;
   }
}


/** ---------------------------------------------------------------------------
 * @brief Is type number a c primitive value
 * @param eTypeNumber type to check for primitive
 * @return true if primitive, false if not
*/
constexpr bool is_primitive_g( enumTypeNumber eTypeNumber )
{
   return static_cast<unsigned>( eTypeNumber ) > static_cast<unsigned>( eTypeUnknown ) && static_cast<unsigned>( eTypeNumber ) <= static_cast<unsigned>( eTypeNumberDouble );
}

/// wrapper, cast to `enumTypeNumber`
constexpr bool is_primitive_g( unsigned uTypeNumber ) { return is_primitive_g( static_cast<enumTypeNumber>( uTypeNumber & 0x000000ff ) ); }

/// Check if reference flag is set, what this is used for depends on object but may be that this item is stored as a reference (pointer)
constexpr bool is_reference_g( unsigned uType ) { return uType & eTypeDetailReference; }

/** ---------------------------------------------------------------------------
 * @brief extract group type from complete type
 * @param uType complete type
 * @return unsigned group part from complete value type
*/
constexpr unsigned value_group_type_g( unsigned uType ) { return (uType & 0x0000ff00); }


constexpr unsigned value_size_g(enumTypeNumber eTypeNumber)
{
   switch( eTypeNumber )
   {
   case eTypeNumberUnknown : return 0;    
   case eTypeNumberBool : return sizeof(uint8_t);
   case eTypeNumberInt8 : return sizeof(int8_t);       
   case eTypeNumberUInt8 : return sizeof(uint8_t);
   case eTypeNumberInt16 : return sizeof(int16_t);
   case eTypeNumberUInt16 : return sizeof(uint16_t);
   case eTypeNumberInt32 : return sizeof(int32_t);
   case eTypeNumberUInt32 : return sizeof(uint32_t);
   case eTypeNumberInt64 : return sizeof(int64_t);
   case eTypeNumberUInt64 : return sizeof(uint64_t);
   case eTypeNumberFloat : return sizeof(float);
   case eTypeNumberDouble : return sizeof(double);
   case eTypeNumberPointer : return sizeof(void*);
   case eTypeNumberGuid : return  16;
   case eTypeNumberString : return 0;     
   case eTypeNumberUtf8String : return 0; 
   case eTypeNumberWString : return 0;    
   case eTypeNumberUtf32String : return 0;
   case eTypeNumberBinary : return 0;     
   case eTypeNumberJson : return 0;       
   case eTypeNumberXml : return 0;        
   case eTypeNumberVoid : return 0;       
   case eTypeNumberBit : return sizeof(uint8_t);        
   case eTypeNumberInt128 : return sizeof(uint64_t) + sizeof(uint64_t);
   case eTypeNumberUInt128 : return sizeof(uint64_t) + sizeof(uint64_t);
   case eTypeNumberHex : return 0;        
   case eTypeNumberBase32 : return 0;     
   case eTypeNumberDateTime : return sizeof(double);
   case eTypeNumberDate : return sizeof(double);
   case eTypeNumberTime : return sizeof(double);
   case eTypeNumberNumeric : return 0;    
   case eTypeNumberDecimal : return 0;    
   case eTypeNumberUuidString : return 0; 
   default: return 0;
   }
}

/** ---------------------------------------------------------------------------
 * @brief get size for type number
 * @param uTypeNumber type value size is returned for
 * @return unsigned size in bytes for type
*/
constexpr unsigned value_size_g( unsigned uTypeNumber )
{
   return value_size_g( static_cast<enumTypeNumber>(uTypeNumber & 0x00000000ff) );
}

/** ---------------------------------------------------------------------------
 * @brief Return column memory size needed to store data for type number
 * This method returns known size for specific types in bytes. Note that string types adds size for
 * zero termination.
 * @param uTypeNumber type number size is returned for
 * @param uCount number of values of type to calculate size for
 * @return size for type
*/
constexpr unsigned value_size_g(unsigned uTypeNumber, unsigned uCount)
{
   switch( uTypeNumber & 0x00000000ff )
   {
   case eTypeNumberUnknown : return 0;    
   case eTypeNumberBool : return sizeof(uint8_t) * uCount;
   case eTypeNumberInt8 : return sizeof(int8_t) * uCount;       
   case eTypeNumberUInt8 : return sizeof(uint8_t) * uCount;
   case eTypeNumberInt16 : return sizeof(int16_t) * uCount;
   case eTypeNumberUInt16 : return sizeof(uint16_t) * uCount;
   case eTypeNumberInt32 : return sizeof(int32_t) * uCount;
   case eTypeNumberUInt32 : return sizeof(uint32_t) * uCount;
   case eTypeNumberInt64 : return sizeof(int64_t) * uCount;
   case eTypeNumberUInt64 : return sizeof(uint64_t) * uCount;
   case eTypeNumberFloat : return sizeof(float) * uCount;
   case eTypeNumberDouble : return sizeof(double) * uCount;
   case eTypeNumberPointer : return sizeof(void*) * uCount;
   case eTypeNumberGuid : return 16 * uCount;
   case eTypeNumberString : return (sizeof(int8_t) * uCount) + sizeof(int8_t);
   case eTypeNumberUtf8String : return (sizeof(uint8_t) * uCount) + sizeof(int8_t); 
   case eTypeNumberWString : return (sizeof(uint16_t) * uCount) + sizeof(uint16_t);
   case eTypeNumberUtf32String : return (sizeof(uint32_t) * uCount) + sizeof(uint32_t);
   case eTypeNumberBinary : return sizeof(uint8_t) * uCount;     
   case eTypeNumberJson : return sizeof(uint8_t) * uCount;
   case eTypeNumberXml : return sizeof(uint8_t) * uCount;
   case eTypeNumberVoid : return 0;       
   case eTypeNumberBit : return sizeof(uint8_t) * uCount;        
   case eTypeNumberUInt128 : return (sizeof(uint64_t) + sizeof(uint64_t)) * uCount;
   case eTypeNumberDecimal : return 0; 
   default: return 0;
   }
}

/** ---------------------------------------------------------------------------
 * @brief Checks if type number is a valid type
 * The lower byte in  number is used to set number type
 * @param uTypeNumber type to check
 * @return true if type is ok, false if not
*/
constexpr bool validate_number_type_g( unsigned uTypeNumber )
{
   unsigned uCheckType = uTypeNumber & 0x000000ff;
   return uCheckType < eTypeNumberMAX;
}


/** ---------------------------------------------------------------------------
 * @brief Convert type name from string to constant type value
 @code
enumType eType = type_g("int32");   assert( eType == eTypeInt32 );
eType = type_g("int8");             assert( eType == eTypeInt8 );
 @endcode
 * @param stringType type sent as string
 * @return {enumType} type constant
*/
constexpr enumType type_g( const std::string_view& stringType )
{                                                                                                  assert( stringType.length() >= 4 );
   using namespace detail;

   enumType eType = eTypeUnknown;

   uint32_t uTypeName = hash_type( stringType ); 

   switch( uTypeName )
   {
   case hash_type("bina"): eType = eTypeBinary;  break;                        // binary
   case hash_type("bool"): eType = eTypeBool;  break;                          // bool
   case hash_type("doub"): eType = eTypeCDouble;  break;                       // double
   case hash_type("floa"): eType = eTypeCFloat;  break;                        // float

   case hash_type("i128"): eType = eTypeInt128;  break;                        // int128
   case hash_type("i256"): eType = eTypeInt256;  break;                        // int254
   case hash_type("i512"): eType = eTypeInt512;  break;                        // int512

   case hash_type("int8"): eType = eTypeInt8;  break;                          // int8
   case hash_type("int1"): eType = eTypeInt16;  break;                         // int16
   case hash_type("int3"): eType = eTypeInt32;  break;                         // int32
   case hash_type("int6"): eType = eTypeInt64;  break;                         // int64
   case hash_type("poin"): eType = eTypePointer;  break;                       // pointer

   case hash_type("rbin"): eType = eTypeRBinary;  break;                       // rbinary (binary reference)
   case hash_type("rstr"): eType = eTypeRString;  break;                       // rstring (string reference)
   case hash_type("rutf"): eType = eTypeRUtf8String;  break;                   // rutf8 (utf8 reference)

   case hash_type("stri"): eType = eTypeString;  break;

   case hash_type("u128"): eType = eTypeUInt128;  break;                       // uint128
   case hash_type("u256"): eType = eTypeUInt256;  break;                       // uint256
   case hash_type("u512"): eType = eTypeUInt512;  break;                       // uint512

   case hash_type("uint"):                                                     // uint8, uint16, uint32, uint64
      {
         if( stringType[4] == '8' ) eType = eTypeUInt8;
         else if( stringType[4] == '1' ) eType = eTypeUInt16;
         else if( stringType[4] == '3' ) eType = eTypeUInt32;
         else if( stringType[4] == '6' ) eType = eTypeUInt64;
         else { static_assert("invalid type name"); assert( false ); }
      }
      break;
   case hash_type("uuid"): eType = eTypeGuid;  break;                          // uuid
   case hash_type("utf8"): eType = eTypeUtf8String;  break;                    // utf8
   case hash_type("wstr"): eType = eTypeWString;  break;                       // wstring
   case hash_type("utf3"): eType = eTypeUtf32String;  break;                   // uft32
   default: assert(false);
   }

   return eType;
}

template<typename TYPE>
constexpr enumType type_g( TYPE, tag_ask_compiler )
{
   if      constexpr( std::is_same<bool, TYPE>::value ) return eTypeBool;
   else if constexpr( std::is_same<int8_t, TYPE>::value ) return eTypeInt8;
   else if constexpr( std::is_same<uint8_t, TYPE>::value ) return eTypeUInt8;
   else if constexpr( std::is_same<int16_t, TYPE>::value ) return eTypeInt16;
   else if constexpr( std::is_same<uint16_t, TYPE>::value ) return eTypeUInt16;
   else if constexpr( std::is_same<int32_t, TYPE>::value ) return eTypeInt32;
   else if constexpr( std::is_same<uint32_t, TYPE>::value ) return eTypeUInt32;
   else if constexpr( std::is_same<int64_t, TYPE>::value ) return eTypeInt64;
   else if constexpr( std::is_same<uint64_t, TYPE>::value ) return eTypeUInt64;
   else if constexpr( std::is_same<float, TYPE>::value ) return eTypeCFloat;
   else if constexpr( std::is_same<double, TYPE>::value ) return eTypeCDouble;
   else if constexpr( std::is_same<void*, TYPE>::value ) return eTypePointer;
   else if constexpr( std::is_same<char*, TYPE>::value ) return eTypeString;
   else if constexpr( std::is_same<std::string_view, TYPE>::value ) return eTypeString;
   else if constexpr( std::is_same<std::string, TYPE>::value ) return eTypeString;
   else if constexpr( std::is_same<std::wstring_view, TYPE>::value ) return eTypeWString;
   else if constexpr( std::is_same<std::wstring, TYPE>::value ) return eTypeWString;
   //else static_assert( false, "Invalid type");
}

template<typename TYPE>
constexpr enumType type_g( tag_ask_compiler )
{
   if      constexpr( std::is_same<bool, TYPE>::value ) return eTypeBool;
   else if constexpr( std::is_same<int8_t, TYPE>::value ) return eTypeInt8;
   else if constexpr( std::is_same<uint8_t, TYPE>::value ) return eTypeUInt8;
   else if constexpr( std::is_same<int16_t, TYPE>::value ) return eTypeInt16;
   else if constexpr( std::is_same<uint16_t, TYPE>::value ) return eTypeUInt16;
   else if constexpr( std::is_same<int32_t, TYPE>::value ) return eTypeInt32;
   else if constexpr( std::is_same<uint32_t, TYPE>::value ) return eTypeUInt32;
   else if constexpr( std::is_same<int64_t, TYPE>::value ) return eTypeInt64;
   else if constexpr( std::is_same<uint64_t, TYPE>::value ) return eTypeUInt64;
   else if constexpr( std::is_same<float, TYPE>::value ) return eTypeCFloat;
   else if constexpr( std::is_same<double, TYPE>::value ) return eTypeCDouble;
   else if constexpr( std::is_same<void*, TYPE>::value ) return eTypePointer;
   else if constexpr( std::is_same<char*, TYPE>::value ) return eTypeString;
   else if constexpr( std::is_same<std::string_view, TYPE>::value ) return eTypeString;
   else if constexpr( std::is_same<std::string, TYPE>::value ) return eTypeString;
   else if constexpr( std::is_same<std::wstring_view, TYPE>::value ) return eTypeWString;
   else if constexpr( std::is_same<std::wstring, TYPE>::value ) return eTypeWString;
   //else static_assert( false, "Invalid type");
}


/** ---------------------------------------------------------------------------
 * @brief return name for type
 * Type is found in first byte where the gd type number logic is used
 * 
 * @code
// Sample to show how to get type name from table called table_column_buffer
gd::table::table_column_buffer tableIterate( 100 );
tableIterate.column_add( { { "int64", 0, "c1"}, { "int32", 0, "c2"}, { "int16", 0, "c3"} }, gd::table::tag_type_name{} );
tableIterate.prepare();

auto columns_ = tableIterate.columns();
for( auto& it : columns_ ) { std::cout << gd::types::type_name_g( it.ctype() ); }
 * @endcode
 * 
 * @param uType type value type name is returned for
 * @return std::string_view name for type
*/
constexpr std::string_view type_name_g(uint32_t uType)
{
   switch( uType & 0x000000ff )
   {
   case eTypeNumberUnknown: return "unknown";
   case eTypeNumberBool: return "boolean";
   case eTypeNumberInt8: return "int8";
   case eTypeNumberInt16: return "int16";
   case eTypeNumberInt32: return "int32";
   case eTypeNumberInt64: return "int64";
   case eTypeNumberUInt8: return "uint8";
   case eTypeNumberUInt16: return "uint16";
   case eTypeNumberUInt32: return "uint32";
   case eTypeNumberUInt64: return "uint64";
   case eTypeNumberFloat: return "float";
   case eTypeNumberDouble: return "double";
   case eTypeNumberGuid:  return "guid";
   case eTypeNumberUtf8String: return "utf8";
   case eTypeNumberUtf32String: return "utf32";
   case eTypeNumberString: return "string";
   case eTypeNumberWString: return "wstring";
   case eTypeNumberBinary:  return "binary";
   case eTypeNumberJson: return "json";
   case eTypeNumberXml: return "xml";
   case eTypeNumberVoid: return "void";
   case eTypeNumberBit: return "bit";
   case eTypeNumberInt128: return "i128";
   case eTypeNumberUInt128: return "u128";
   case eTypeNumberInt256: return "i256";
   case eTypeNumberUInt256: return "u256";
   case eTypeNumberInt512: return "i512";
   case eTypeNumberUInt512: return "u512";
      break;
   }

   return std::string_view();
}

/** ---------------------------------------------------------------------------
 * @brief Convert type names to vector with type numbers
 * @param listName list with names that is converted to type numbers
 * @return std::vector<unsigned> vector with type numbers for type names
*/
inline std::vector<unsigned> types_g( std::initializer_list< const char*> listName )
{
   std::vector<unsigned> vectorType;
   for( auto it : listName )
   {
      vectorType.push_back( type_g( it ) );
   }

   return vectorType;
}



#if defined(__clang__)
   #pragma clang diagnostic pop
#elif defined(__GNUC__)
   #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
   #pragma warning(pop)
#endif


_GD_TYPES_END