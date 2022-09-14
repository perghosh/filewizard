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


_GD_TYPES_END