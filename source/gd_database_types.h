#pragma once
#include <cassert>
#include <string>
#include <string_view>

#ifndef _GD_DATABASE_BEGIN
#define _GD_DATABASE_BEGIN namespace gd { namespace database {
#define _GD_DATABASE_END } }
_GD_DATABASE_BEGIN
#else
_GD_DATABASE_BEGIN
#endif



/*-----------------------------------------*/ /**
 * \brief Type of database that is connected and script needs to handle
 *
 *
 */
enum enumDatabaseType
{
   eDatabaseTypeNone    = 0,              ///< empty
   eDatabaseTypeODBC    = 1,              ///< ODBC connection
   eDatabaseTypeOLEDB   = 2,              ///< OLEDB connection
   eDatabaseTypeSqlite  = 3,              ///< Sqlite connection
};

enum enumServerName
{
   eServerNameUnknown               = 0,
   eServerNameMicrosoftSqlServer    = 1,
   eServerNameOracle                = 2,
   eServerNameMySql                 = 3,
   eServerNamePostgreSQL            = 4,
   eServerNameMicrosoftAccess       = 5,
   eServerNameSqlite                = 6,
};

enum enumMakeFieldValue
{
   eMakeFieldValueKey      = 0x01,           ///< generate key value for field
   eMakeFieldValueDefault  = 0x02,           ///< generate default value for field
};

/*-----------------------------------------*/ /**
 * \brief 
 *
 *
 */
enum enumColumnTypeGroup
{
   eColumnTypeGroupNumber  = 0x00000100,
   eColumnTypeGroupInteger = 0x00000200,
   eColumnTypeGroupDecimal = 0x00000400,
   eColumnTypeGroupSigned  = 0x00000800,
   eColumnTypeGroupString  = 0x00001000,
   eColumnTypeGroupDate    = 0x00002000,
   eColumnTypeGroupBinary  = 0x00004000,
   eColumnTypeGroupBoolean = 0x00008000,

   eColumnTypeGroup08      = 0x00010000,
   eColumnTypeGroup16      = 0x00020000,
   eColumnTypeGroup32      = 0x00040000,
   eColumnTypeGroup64      = 0x00080000,

   eColumnTypeGroupNull    = 0x00100000,
   eColumnTypeGroupChar    = 0x00200000,
   eColumnTypeGroupWChar   = 0x00400000,
};

/*-----------------------------------------*/ /**
 * \brief database column types
 *
 *
 */
enum enumColumnTypeNumber
{
   eColumnTypeNumberUnknown      = 0,
   eColumnTypeNumberBool         = 1,
   eColumnTypeNumberInt8         = 2,
   eColumnTypeNumberInt16        = 3,
   eColumnTypeNumberInt32        = 4,
   eColumnTypeNumberInt64        = 5,
   eColumnTypeNumberUInt8        = 6,
   eColumnTypeNumberUInt16       = 7,
   eColumnTypeNumberUInt32       = 8,
   eColumnTypeNumberUInt64       = 9,
   eColumnTypeNumberCFloat       = 10,
   eColumnTypeNumberCDouble      = 11,
   eColumnTypeNumberPointer      = 12,
   eColumnTypeNumberGuid         = 13,
   eColumnTypeNumberString       = 14,
   eColumnTypeNumberUtf8String   = 15,
   eColumnTypeNumberWString      = 16,
   eColumnTypeNumberUtf32String  = 17,
   eColumnTypeNumberBinary       = 18,
   eColumnTypeNumberJson         = 19,
   eColumnTypeNumberXml          = 20,
   eColumnTypeNumberVoid         = 21,
   eColumnTypeNumberBit          = 22,
   eColumnTypeNumberBinaryString = 25,
   eColumnTypeNumberDateTime     = 27,
   eColumnTypeNumberDate         = 28,
   eColumnTypeNumberTime         = 29,
   eColumnTypeNumberNumeric      = 30,
   eColumnTypeNumberDecimal      = 31,
   eColumnTypeNumberUuidString   = 32,
};



/*-----------------------------------------*/ /**
 * \brief constant number for different value types used in database management
 *
 */
enum enumColumnType
{
   //TODO: Only user of utf-8 string type is sqlite so remove that type
   eColumnTypeUnknown      = eColumnTypeNumberUnknown    | eColumnTypeGroupNull,
   eColumnTypeBit          = eColumnTypeNumberBit        | eColumnTypeGroupNumber,
   eColumnTypeBool         = eColumnTypeNumberBool       | eColumnTypeGroupNumber | eColumnTypeGroupInteger | eColumnTypeGroupBoolean,
   eColumnTypeInt8         = eColumnTypeNumberInt8       | eColumnTypeGroupNumber | eColumnTypeGroupInteger | eColumnTypeGroupSigned,
   eColumnTypeInt16        = eColumnTypeNumberInt16      | eColumnTypeGroupNumber | eColumnTypeGroupInteger | eColumnTypeGroupSigned,
   eColumnTypeInt32        = eColumnTypeNumberInt32      | eColumnTypeGroupNumber | eColumnTypeGroupInteger | eColumnTypeGroupSigned,
   eColumnTypeInt64        = eColumnTypeNumberInt64      | eColumnTypeGroupNumber | eColumnTypeGroupInteger | eColumnTypeGroupSigned,
   eColumnTypeUInt8        = eColumnTypeNumberUInt8      | eColumnTypeGroupNumber | eColumnTypeGroupInteger,
   eColumnTypeUInt16       = eColumnTypeNumberUInt16     | eColumnTypeGroupNumber | eColumnTypeGroupInteger,
   eColumnTypeUInt32       = eColumnTypeNumberUInt32     | eColumnTypeGroupNumber | eColumnTypeGroupInteger,
   eColumnTypeUInt64       = eColumnTypeNumberUInt64     | eColumnTypeGroupNumber | eColumnTypeGroupInteger,
   eColumnTypeCFloat       = eColumnTypeNumberCFloat     | eColumnTypeGroupNumber | eColumnTypeGroupDecimal | eColumnTypeGroupSigned,
   eColumnTypeCDouble      = eColumnTypeNumberCDouble    | eColumnTypeGroupNumber | eColumnTypeGroupDecimal | eColumnTypeGroupSigned,
   eColumnTypeGuid         = eColumnTypeNumberGuid       | eColumnTypeGroupBinary,
   eColumnTypeBinary       = eColumnTypeNumberBinary     | eColumnTypeGroupBinary,
   eColumnTypeUtf8String   = eColumnTypeNumberUtf8String | eColumnTypeGroupString,
   eColumnTypeString       = eColumnTypeNumberString     | eColumnTypeGroupString,
   eColumnTypeWString      = eColumnTypeNumberWString    | eColumnTypeGroupString,
   eColumnTypeNumeric      = eColumnTypeNumberNumeric    | eColumnTypeGroupString,
   eColumnTypeDecimal      = eColumnTypeNumberDecimal    | eColumnTypeGroupString,
   eColumnTypeGuidString   = eColumnTypeNumberUuidString | eColumnTypeGroupString | eColumnTypeGroupBinary,
   eColumnTypeBinaryString = eColumnTypeNumberBinaryString| eColumnTypeGroupString| eColumnTypeGroupBinary,

   eColumnTypeDateTime     = eColumnTypeNumberDateTime   | eColumnTypeGroupDate,
   eColumnTypeDate         = eColumnTypeNumberDate       | eColumnTypeGroupDate,
   eColumnTypeTime         = eColumnTypeNumberTime       | eColumnTypeGroupDate,


   eColumnType_FilterType  = 0x0000ffff,        ///< flags used to get complete type information
   eColumnType_FilterGroup = 0x0000ff00,        ///< flags used to get group information from type
   eColumnType_FilterTypeGroup = 0x0000ff00,    ///< flags used to get group information from type
   eColumnType_FilterTypeNumber = 0x000000ff,   ///< flags for filter out index for type
};

/*-----------------------------------------*/ /**
 * \brief constant number for different value types used in database management
 *
 */
enum enumColumnTypeComplete
{
   eColumnTypeCompleteUnknown      = eColumnTypeUnknown,   
   eColumnTypeCompleteBit          = eColumnTypeBit        | eColumnTypeGroup08,
   eColumnTypeCompleteBool         = eColumnTypeBool       | eColumnTypeGroup08,
   eColumnTypeCompleteInt8         = eColumnTypeInt8       | eColumnTypeGroup08,
   eColumnTypeCompleteInt16        = eColumnTypeInt16      | eColumnTypeGroup16,
   eColumnTypeCompleteInt32        = eColumnTypeInt32      | eColumnTypeGroup32,
   eColumnTypeCompleteInt64        = eColumnTypeInt64      | eColumnTypeGroup64,
   eColumnTypeCompleteUInt8        = eColumnTypeUInt8      | eColumnTypeGroup08,
   eColumnTypeCompleteUInt16       = eColumnTypeUInt16     | eColumnTypeGroup16,
   eColumnTypeCompleteUInt32       = eColumnTypeUInt32     | eColumnTypeGroup32,
   eColumnTypeCompleteUInt64       = eColumnTypeUInt64     | eColumnTypeGroup64,
   eColumnTypeCompleteCFloat       = eColumnTypeCFloat     | eColumnTypeGroup32,
   eColumnTypeCompleteCDouble      = eColumnTypeCDouble    | eColumnTypeGroup64,
   eColumnTypeCompleteGuid         = eColumnTypeGuid,
   eColumnTypeCompleteBinary       = eColumnTypeBinary,
   eColumnTypeCompleteUtf8String   = eColumnTypeUtf8String | eColumnTypeGroupChar,
   eColumnTypeCompleteString       = eColumnTypeString     | eColumnTypeGroupChar,
   eColumnTypeCompleteWString      = eColumnTypeWString    | eColumnTypeGroupWChar,
   eColumnTypeCompleteNumeric      = eColumnTypeNumeric    | eColumnTypeGroupChar,
   eColumnTypeCompleteDecimal      = eColumnTypeDecimal    | eColumnTypeGroupChar,
   eColumnTypeCompleteGuidString   = eColumnTypeGuidString | eColumnTypeGroupWChar,
   eColumnTypeCompleteBinaryString = eColumnTypeBinaryString| eColumnTypeGroupWChar,
   eColumnTypeCompleteDateTime     = eColumnTypeDateTime    | eColumnTypeGroup64,
   eColumnTypeCompleteDate         = eColumnTypeDate        | eColumnTypeGroup64,
   eColumnTypeCompleteTime         = eColumnTypeTime        | eColumnTypeGroup64,
};

/*----------------------------------------------------------------------------- get_type */ /**
 * 
 * \param stringTypeName
 * \return constexpr uint32_t
 */
/*
constexpr uint32_t get_type(std::string_view stringTypeName)
{
   if(  )
}
*/


_GD_DATABASE_END

// ## Check for compability with core types found in gd::types
#ifdef _GD_TYPES
static_assert( gd::types::enumTypeNumber::eTypeNumberUInt64 == gd::database::enumColumnTypeNumber::eColumnTypeNumberUInt64 );
static_assert( gd::types::enumTypeNumber::eTypeNumberDouble == gd::database::enumColumnTypeNumber::eColumnTypeNumberCDouble );
static_assert( gd::types::enumTypeNumber::eTypeNumberDecimal == gd::database::enumColumnTypeNumber::eColumnTypeNumberDecimal );
#endif