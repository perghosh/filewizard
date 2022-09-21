#pragma once

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


/*-----------------------------------------*/ /**
   * \brief type numbers for common data types
   *
   * Numbers for common types used i software development.
   * Using thsese numbers as a form of metadata in code, it will simplfy
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
   eTypeNumberVoid         = 21,
   eTypeNumberBit          = 22,
   eTypeNumberInt128       = 23,
   eTypeNumberUInt128      = 24,
   eTypeNumberHex          = 25,
   eTypeNumberBase32       = 26,
   eTypeNumberDateTime     = 27,
   eTypeNumberDate         = 28,
   eTypeNumberTime         = 29,
   eTypeNumberNumeric      = 30,
   eTypeNumberDecimal      = 31,
   eTypeNumberUuidString   = 32,
};

/*-----------------------------------------*/ /**
 * \brief Flags for common type groups for value types
 * 
 * Group type values to make it easer to build logic around them.
 */
enum enumTypeGroup
{
   eTypeGroupNumber        = 0x00000100,
   eTypeGroupInteger       = 0x00000200,
   eTypeGroupDecimal       = 0x00000400,
   eTypeGroupSigned        = 0x00000800,
   eTypeGroupString        = 0x00001000,
   eTypeGroupDate          = 0x00002000,
   eTypeGroupBinary        = 0x00004000,
   eTypeGroupBoolean       = 0x00008000,

   eTypeGroupSize08        = 0x00010000,
   eTypeGroupSize16        = 0x00020000,
   eTypeGroupSize32        = 0x00040000,
   eTypeGroupSize64        = 0x00080000,
   eTypeGroupSize128       = 0x00100000,
};


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


_GD_TYPES_END