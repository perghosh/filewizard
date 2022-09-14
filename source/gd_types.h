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
   *
   */
enum enumTypeNumber
{
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