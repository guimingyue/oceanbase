/**
 * Copyright (c) 2021 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#ifndef OCEANBASE_COMMON_OB_OBJECT_TYPE_H_
#define OCEANBASE_COMMON_OB_OBJECT_TYPE_H_
#include "lib/utility/ob_print_utils.h"
#include "lib/charset/ob_charset.h"

namespace oceanbase {
namespace common {
class ObObjMeta;
class ObAccuracy;
template <class T>
class ObIArray;

// we can append new type only, do NOT delete nor change order,
// modify ObObjTypeClass and ob_obj_type_class when append new object type.
enum ObObjType {
  ObNullType = 0,

  ObTinyIntType = 1,    // int8, aka mysql boolean type
  ObSmallIntType = 2,   // int16
  ObMediumIntType = 3,  // int24
  ObInt32Type = 4,      // int32
  ObIntType = 5,        // int64, aka bigint

  ObUTinyIntType = 6,    // uint8
  ObUSmallIntType = 7,   // uint16
  ObUMediumIntType = 8,  // uint24
  ObUInt32Type = 9,      // uint32
  ObUInt64Type = 10,     // uint64

  ObFloatType = 11,   // single-precision floating point
  ObDoubleType = 12,  // double-precision floating point

  ObUFloatType = 13,   // unsigned single-precision floating point
  ObUDoubleType = 14,  // unsigned double-precision floating point

  ObNumberType = 15,  // aka decimal/numeric
  ObUNumberType = 16,

  ObDateTimeType = 17,
  ObTimestampType = 18,
  ObDateType = 19,
  ObTimeType = 20,
  ObYearType = 21,

  ObVarcharType = 22,  // charset: utf8mb4 or binary
  ObCharType = 23,     // charset: utf8mb4 or binary

  ObHexStringType = 24,  // hexadecimal literal, e.g. X'42', 0x42, b'1001', 0b1001

  ObExtendType = 25,   // Min, Max, NOP etc.
  ObUnknownType = 26,  // For question mark(?) in prepared statement, no need to serialize
  // @note future new types to be defined here !!!
  ObTinyTextType = 27,
  ObTextType = 28,
  ObMediumTextType = 29,
  ObLongTextType = 30,

  ObBitType = 31,
  ObEnumType = 32,
  ObSetType = 33,
  ObEnumInnerType = 34,
  ObSetInnerType = 35,

  ObTimestampTZType = 36,    // timestamp with time zone for oracle
  ObTimestampLTZType = 37,   // timestamp with local time zone for oracle
  ObTimestampNanoType = 38,  // timestamp nanosecond for oracle
  ObRawType = 39,            // raw type for oracle
  ObIntervalYMType = 40,     // interval year to month
  ObIntervalDSType = 41,     // interval day to second
  ObNumberFloatType = 42,    // oracle float, subtype of NUMBER
  ObNVarchar2Type = 43,      // nvarchar2
  ObNCharType = 44,          // nchar
  ObURowIDType = 45,         // UROWID
  ObLobType = 46,            // Oracle Lob
  ObJsonType = 47,           // Json Type
  ObMaxType                  // invalid type, or count of obj type
};

// Oracle type
enum ObObjOType {
  ObONotSupport = 0,
  ObONullType = 1,
  ObOSmallIntType = 2,
  ObOIntType = 3,
  ObONumberFloatType = 4,  // float
  ObOBinFloatType = 5,     // binary float
  ObOBinDoubleType = 6,    // binary double
  ObONumberType = 7,
  ObOCharType = 8,
  ObOVarcharType = 9,
  ObODateType = 10,
  ObOTimestampTZType = 11,   // timestamp with time zone
  ObOTimestampLTZType = 12,  // timestamp with local time zone
  ObOTimestampType = 13,     // timestamp
  ObOIntervalYMType = 14,    // TODO
  ObOIntervalDSType = 15,    // TODO
  ObOLobType = 16,           // blob
  ObOExtendType = 17,        // not sure
  ObOUnknownType = 18,       // not sure
  ObORawType = 19,
  ObONVarchar2Type = 20,
  ObONCharType = 21,
  ObOURowIDType = 22,
  ObOLobLocatorType = 23,
  ObOMaxType  // invalid type, or count of ObObjOType
};

// for cast/cmp map
static ObObjOType OBJ_TYPE_TO_O_TYPE[ObMaxType + 1] = {
    ObONullType,
    ObOSmallIntType,  // int8, aka mysql boolean type
    ObOSmallIntType,  // int16
    ObONotSupport,    // int24
    ObOIntType,       // int32
    ObOIntType,       // int64, aka bigint
    ObONotSupport,    // uint8
    ObONotSupport,    // uint16
    ObONotSupport,    // uint24
    ObONotSupport,    // uint32
    ObONotSupport,    // uint64

    ObOBinFloatType,   // single-precision floating point
    ObOBinDoubleType,  // double-precision floating point

    ObONotSupport,  // unsigned single-precision floating point
    ObONotSupport,  // unsigned double-precision floating point

    ObONumberType,  // aka decimal/numeric
    ObONotSupport,  // ObUNumberType

    ObODateType,    // ObDateTimeType
    ObONotSupport,  // ObTimestampType
    ObODateType,    // ObDateType
    ObONotSupport,  // ObTimeType
    ObONotSupport,  // ObYearType, //TODO

    ObOVarcharType,  // ObVarcharType=22,
    ObOCharType,     // ObCharType=23,     // charset: utf8mb4 or binary

    ObONotSupport,  // ObHexStringType=24, //TODO // hexadecimal literal, e.g. X'42', 0x42, b'1001', 0b1001

    ObOExtendType,   // ObExtendType=25,  //?TODO               // Min, Max, NOP etc.
    ObOUnknownType,  // ObUnknownType=26,  //?TODO             // For question mark(?) in prepared statement, no need to
                     // serialize
    ObONotSupport,   // ObTinyTextType = 27,
    ObONotSupport,   // ObTextType = 28,
    ObONotSupport,   // ObMediumTextType = 29,
    ObOLobType,      // ObLongTextType = 30,
    ObONotSupport,   // ObBitType=31,
    ObONotSupport,   // ObEnumType=32,
    ObONotSupport,   // ObSetType=33,
    ObONotSupport,   // ObEnumInnerType=34,
    ObONotSupport,   // ObSetInnerType=35,
    ObOTimestampTZType,   // ObTimeStampTZType=36,
    ObOTimestampLTZType,  // ObTimeStampLTZType=37,
    ObOTimestampType,     // ObTimeStamp=38,
    ObORawType,           // ObRawType=39,
    ObOIntervalYMType,    // ObIntervalYMType=40,
    ObOIntervalDSType,    // ObIntervalDSType=41,
    ObONumberFloatType,   // ObNumberFloatType=42,
    ObONVarchar2Type,     // ObNVarchar2=43,
    ObONCharType,         // ObNChar=44,
    ObOURowIDType,        // ObURowID=45
    ObOLobLocatorType,    // ObLobType = 46,
    ObONotSupport         // ObMaxType,
};

enum ObObjTypeClass {
  ObNullTC = 0,      // null
  ObIntTC = 1,       // int8, int16, int24, int32, int64.
  ObUIntTC = 2,      // uint8, uint16, uint24, uint32, uint64.
  ObFloatTC = 3,     // float, ufloat.
  ObDoubleTC = 4,    // double, udouble.
  ObNumberTC = 5,    // number, unumber.
  ObDateTimeTC = 6,  // datetime, timestamp.
  ObDateTC = 7,      // date
  ObTimeTC = 8,      // time
  ObYearTC = 9,      // year
  ObStringTC = 10,   // varchar, char, varbinary, binary.
  ObExtendTC = 11,   // extend
  ObUnknownTC = 12,  // unknown
  ObTextTC = 13,     // TinyText,MediumText, Text ,LongText, TinyBLOB,MediumBLOB, BLOB ,LongBLOB
  ObBitTC = 14,      // bit
  ObEnumSetTC = 15,  // enum, set
  ObEnumSetInnerTC = 16,
  ObOTimestampTC = 17,  // timestamp with time zone
  ObRawTC = 18,         // raw
  ObIntervalTC = 19,    // oracle interval type class include interval year to month and interval day to second
  ObRowIDTC = 20,       // oracle rowid typeclass, includes urowid and rowid
  ObLobTC = 21,         // oracle lob typeclass
  ObJsonTC = 22,      // json type class
  ObMaxTC,
  // invalid type classes are below, only used as the result of XXXX_type_promotion()
  // to indicate that the two obj can't be promoted to the same type.
  ObIntUintTC,
  ObLeftTypeTC,
  ObRightTypeTC,
  ObActualMaxTC
};

// Define obj type to obj type class mapping pairs.
#define OBJ_TYPE_TC_PAIRS                                                                                             \
  (ObNullType, ObNullTC), (ObTinyIntType, ObIntTC), (ObSmallIntType, ObIntTC), (ObMediumIntType, ObIntTC),            \
      (ObInt32Type, ObIntTC), (ObIntType, ObIntTC), (ObUTinyIntType, ObUIntTC), (ObUSmallIntType, ObUIntTC),          \
      (ObUMediumIntType, ObUIntTC), (ObUInt32Type, ObUIntTC), (ObUInt64Type, ObUIntTC), (ObFloatType, ObFloatTC),     \
      (ObDoubleType, ObDoubleTC), (ObUFloatType, ObFloatTC), (ObUDoubleType, ObDoubleTC), (ObNumberType, ObNumberTC), \
      (ObUNumberType, ObNumberTC), (ObDateTimeType, ObDateTimeTC), (ObTimestampType, ObDateTimeTC),                   \
      (ObDateType, ObDateTC), (ObTimeType, ObTimeTC), (ObYearType, ObYearTC), (ObVarcharType, ObStringTC),            \
      (ObCharType, ObStringTC), (ObHexStringType, ObStringTC), (ObExtendType, ObExtendTC),                            \
      (ObUnknownType, ObUnknownTC), (ObTinyTextType, ObTextTC), (ObTextType, ObTextTC), (ObMediumTextType, ObTextTC), \
      (ObLongTextType, ObTextTC), (ObBitType, ObBitTC), (ObEnumType, ObEnumSetTC), (ObSetType, ObEnumSetTC),          \
      (ObEnumInnerType, ObEnumSetInnerTC), (ObSetInnerType, ObEnumSetInnerTC), (ObTimestampTZType, ObOTimestampTC),   \
      (ObTimestampLTZType, ObOTimestampTC), (ObTimestampNanoType, ObOTimestampTC), (ObRawType, ObRawTC),              \
      (ObIntervalYMType, ObIntervalTC), (ObIntervalDSType, ObIntervalTC), (ObNumberFloatType, ObNumberTC),            \
      (ObNVarchar2Type, ObStringTC), (ObNCharType, ObStringTC), (ObURowIDType, ObRowIDTC), (ObLobType, ObLobTC),      \
      (ObJsonType, ObJsonTC)

#define SELECT_SECOND(x, y) y
#define SELECT_TC(arg) SELECT_SECOND arg

const ObObjTypeClass OBJ_TYPE_TO_CLASS[ObMaxType] = {LST_DO(SELECT_TC, (, ), OBJ_TYPE_TC_PAIRS)};

#undef SELECT_TC
#undef SELECT_SECOND

const ObObjType OBJ_DEFAULT_TYPE[ObActualMaxTC] = {
    ObNullType,           // null
    ObIntType,            // int
    ObUInt64Type,         // uint
    ObFloatType,          // float
    ObDoubleType,         // double
    ObNumberType,         // number
    ObDateTimeType,       // datetime
    ObDateType,           // date
    ObTimeType,           // time
    ObYearType,           // year
    ObVarcharType,        // varchar
    ObExtendType,         // extend
    ObUnknownType,        // unknown
    ObLongTextType,       // text
    ObBitType,            // bit
    ObUInt64Type,         // enumset
    ObMaxType,            // enumsetInner
    ObTimestampNanoType,  // timestamp nano
    ObRawType,            // raw
    ObMaxType,            // no default type for interval type class
    ObMaxType,            // no default type for rowid type class
    ObLobType,            // lob
    ObJsonType,           // json
    ObMaxType,            // maxtype
    ObUInt64Type,         // int&uint
    ObMaxType,            // lefttype
    ObMaxType,            // righttype
};

// plz check if OB_OBJ_IMPLICIT_CAST_DIRECTION_FOR_ORACLE is also need to be modified after you modify this one
static ObObjTypeClass OBJ_O_TYPE_TO_CLASS[ObOMaxType + 1] = {ObMaxTC,  // ObONotSupport = 1
    ObNullTC,                                                          // ObONullType = 2,
    ObIntTC,                                                           // ObOSmallIntType=3,
    ObIntTC,                                                           // ObOIntType=4,
    ObNumberTC,                                                        // ObOBinFloatType=5,          //float
    ObFloatTC,                                                         // ObOBinFloatType=6,           //binary float
    ObDoubleTC,                                                        // ObOBinDoubleType=7,          //binary double
    ObNumberTC,                                                        // ObONumbertype=8,
    ObStringTC,                                                        // ObOCharType=9,
    ObStringTC,                                                        // ObOVarcharType=10,
    ObDateTimeTC,                                                      // ObODateType11,
    ObOTimestampTC,                                                    // ObOTimestampTZType=12,
    ObOTimestampTC,                                                    // ObOTimestampLTZType=13,
    ObOTimestampTC,                                                    // ObOTimestampType=14,
    ObIntervalTC,                                                      // ObOIntervalYMType=15,
    ObIntervalTC,                                                      // ObOIntervalDSType=16,
    ObTextTC,                                                          // ObOTextType=17,
    ObExtendTC,                                                        // ObOExtendType=18,       //not sure
    ObUnknownTC,                                                       // ObOUnknownType=19,      //not sure
    ObRawTC,                                                           // ObORawType
    ObStringTC,                                                        // ObONVarchar2
    ObStringTC,                                                        // ObONChar
    ObRowIDTC,                                                         // ObOURowID
    ObLobTC,                                                           // ObOLobLocator
    ObMaxTC};

enum ImplicitCastDirection {
  IC_NOT_SUPPORT = -1,
  IC_NO_CAST = 0,
  IC_A_TO_B = 1,
  IC_B_TO_A = 2,
  IC_TO_MIDDLE_TYPE = 3,
  IC_A_TO_C = 4,
  IC_B_TO_C = 5,
};

/*
 * Datetime > double > float > number > int/smallint > char/varchar > others
 */
static ImplicitCastDirection
    OB_OBJ_IMPLICIT_CAST_DIRECTION_FOR_ORACLE[ObObjOType::ObOMaxType + 1][ObObjOType::ObOMaxType + 1] = {
        /*A->B*/
        {
            /*ObONotSupport->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NOT_SUPPORT, /*Null*/
            IC_NOT_SUPPORT, /*SmallInt*/
            IC_NOT_SUPPORT, /*Int*/
            IC_NOT_SUPPORT, /*Float*/
            IC_NOT_SUPPORT, /*BinaryFloat*/
            IC_NOT_SUPPORT, /*BinaryDouble*/
            IC_NOT_SUPPORT, /*Number*/
            IC_NOT_SUPPORT, /*Char*/
            IC_NOT_SUPPORT, /*Varchar*/
            IC_NOT_SUPPORT, /*Date*/
            IC_NOT_SUPPORT, /*TimestampTZ*/
            IC_NOT_SUPPORT, /*TimestampLTZ*/
            IC_NOT_SUPPORT, /*Timestamp*/
            IC_NOT_SUPPORT, /*IntervalYM*/
            IC_NOT_SUPPORT, /*IntervalDS*/
            IC_NOT_SUPPORT, /*Lob*/
            IC_NOT_SUPPORT, /*Extend*/
            IC_NOT_SUPPORT, /*Unknown*/
            IC_NOT_SUPPORT, /*Raw*/
            IC_NOT_SUPPORT, /*LobLocator*/
            IC_NOT_SUPPORT, /*ObOMaxType*/
        },
        {
            /*Null->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NO_CAST,     /*Null*/
            IC_NO_CAST,     /*SmallInt*/
            IC_NO_CAST,     /*Int*/
            IC_NO_CAST,     /*Float*/
            IC_NO_CAST,     /*BinaryFloat*/
            IC_NO_CAST,     /*BinaryDouble*/
            IC_NO_CAST,     /*Number*/
            IC_NO_CAST,     /*Char*/
            IC_NO_CAST,     /*Varchar*/
            IC_NO_CAST,     /*Date*/
            IC_NO_CAST,     /*TimestampTZ*/
            IC_NO_CAST,     /*TimestampLTZ*/
            IC_NO_CAST,     /*Timestamp*/
            IC_NO_CAST,     /*IntervalYM*/
            IC_NO_CAST,     /*IntervalDS*/
            IC_NO_CAST,     /*Lob*/
            IC_NO_CAST,     /*Extend*/
            IC_NO_CAST,     /*Unknown*/
            IC_NO_CAST,     /*Raw*/
            IC_NO_CAST,     /*NVarchar2*/
            IC_NO_CAST,     /*NChar*/
            IC_NO_CAST,     /*URowID*/
            IC_NO_CAST,     /*LobLocator*/
            IC_NOT_SUPPORT, /*ObOMaxType*/
        },
        {
            /*SmallInt->XXX*/
            IC_NOT_SUPPORT,    /*ObONotSupport*/
            IC_NO_CAST,        /*Null*/
            IC_NO_CAST,        /*SmallInt*/
            IC_NO_CAST,        /*Int*/
            IC_TO_MIDDLE_TYPE, /*Float*/
            IC_A_TO_B,         /*BinaryFloat*/
            IC_A_TO_B,         /*BinaryDouble*/
            IC_NO_CAST,        // IC_A_TO_B,  /*Number*/
            IC_TO_MIDDLE_TYPE, /*Char*/
            IC_TO_MIDDLE_TYPE, /*Varchar*/
            IC_NOT_SUPPORT,    /*Date*/
            IC_NOT_SUPPORT,    /*TimestampTZ*/
            IC_NOT_SUPPORT,    /*TimestampLTZ*/
            IC_NOT_SUPPORT,    /*Timestamp*/
            IC_NOT_SUPPORT,    /*IntervalYM*/
            IC_NOT_SUPPORT,    /*IntervalDS*/
            IC_TO_MIDDLE_TYPE, /*Lob*/
            IC_NOT_SUPPORT,    /*Extend*/
            IC_NOT_SUPPORT,    /*Unknown*/
            IC_NOT_SUPPORT,    /*Raw*/
            IC_NOT_SUPPORT,    /*NVarchar2*/
            IC_NOT_SUPPORT,    /*NChar*/
            IC_NOT_SUPPORT,    /*URowID*/
            IC_TO_MIDDLE_TYPE, /*LobLocator*/
            IC_NOT_SUPPORT,    /*ObOMaxType*/
        },
        {
            /*Int->XXX*/
            IC_NOT_SUPPORT,    /*ObONotSupport*/
            IC_NO_CAST,        /*Null*/
            IC_NO_CAST,        /*SmallInt*/
            IC_NO_CAST,        /*Int*/
            IC_TO_MIDDLE_TYPE, /*Float*/
            IC_A_TO_B,         /*BinaryFloat*/
            IC_A_TO_B,         /*BinaryDouble*/
            IC_NO_CAST,        // IC_A_TO_B,  /*Number*/
            IC_TO_MIDDLE_TYPE, /*Char*/
            IC_TO_MIDDLE_TYPE, /*Varchar*/
            IC_NOT_SUPPORT,    /*Date*/
            IC_NOT_SUPPORT,    /*TimestampTZ*/
            IC_NOT_SUPPORT,    /*TimestampLTZ*/
            IC_NOT_SUPPORT,    /*Timestamp*/
            IC_NOT_SUPPORT,    /*IntervalYM*/
            IC_NOT_SUPPORT,    /*IntervalDS*/
            IC_TO_MIDDLE_TYPE, /*Lob*/
            IC_NOT_SUPPORT,    /*Extend*/
            IC_NOT_SUPPORT,    /*Unknown*/
            IC_NOT_SUPPORT,    /*Raw*/
            IC_NOT_SUPPORT,    /*NVarchar2*/
            IC_NOT_SUPPORT,    /*NChar*/
            IC_NOT_SUPPORT,    /*URowID*/
            IC_TO_MIDDLE_TYPE, /*LobLocator*/
            IC_NOT_SUPPORT,    /*ObOMaxType*/
        },
        {
            /*Float->XXX*/
            IC_NOT_SUPPORT,    /*ObONotSupport*/
            IC_NO_CAST,        /*Null*/
            IC_TO_MIDDLE_TYPE, /*SmallInt*/
            IC_TO_MIDDLE_TYPE, /*Int*/
            IC_NO_CAST,        /*Float*/
            IC_A_TO_B,         /*BinaryFloat*/
            IC_A_TO_B,         /*BinaryDouble*/
            IC_A_TO_B,         /*Number*/
            IC_TO_MIDDLE_TYPE, /*Char*/
            IC_TO_MIDDLE_TYPE, /*Varchar*/
            IC_NOT_SUPPORT,    /*Date*/
            IC_NOT_SUPPORT,    /*TimestampTZ*/
            IC_NOT_SUPPORT,    /*TimestampLTZ*/
            IC_NOT_SUPPORT,    /*Timestamp*/
            IC_NOT_SUPPORT,    /*IntervalYM*/
            IC_NOT_SUPPORT,    /*IntervalDS*/
            IC_TO_MIDDLE_TYPE, /*Lob*/
            IC_NOT_SUPPORT,    /*Extend*/
            IC_NOT_SUPPORT,    /*Unknown*/
            IC_NOT_SUPPORT,    /*Raw*/
            IC_TO_MIDDLE_TYPE, /*NVarchar2*/
            IC_TO_MIDDLE_TYPE, /*NChar*/
            IC_NOT_SUPPORT,    /*URowID*/
            IC_B_TO_A,         /*LobLocator*/
            IC_NOT_SUPPORT,    /*ObOMaxType*/
        },
        {
            /*BinaryFloat->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NO_CAST,     /*Null*/
            IC_B_TO_A,      /*SmallInt*/
            IC_B_TO_A,      /*Int*/
            IC_B_TO_A,      /*Float*/
            IC_NO_CAST,     /*BinaryFloat*/
            IC_A_TO_B,      /*BinaryDouble*/
            IC_B_TO_A,      /*Number*/
            IC_B_TO_A,      /*Char*/
            IC_B_TO_A,      /*Varchar*/
            IC_NOT_SUPPORT, /*Date*/
            IC_NOT_SUPPORT, /*TimestampTZ*/
            IC_NOT_SUPPORT, /*TimestampLTZ*/
            IC_NOT_SUPPORT, /*Timestamp*/
            IC_NOT_SUPPORT, /*IntervalYM*/
            IC_NOT_SUPPORT, /*IntervalDS*/
            IC_B_TO_A,      /*Lob*/
            IC_NOT_SUPPORT, /*Extend*/
            IC_NOT_SUPPORT, /*Unknown*/
            IC_NOT_SUPPORT, /*Raw*/
            IC_B_TO_A,      /*NVarchar2*/
            IC_B_TO_A,      /*NChar*/
            IC_NOT_SUPPORT, /*URowID*/
            IC_B_TO_A,      /*LobLocator*/
            IC_NOT_SUPPORT, /*ObOMaxType*/
        },
        {
            /*BinaryDouble->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NO_CAST,     /*Null*/
            IC_B_TO_A,      /*SmallInt*/
            IC_B_TO_A,      /*Int*/
            IC_B_TO_A,      /*Float*/
            IC_B_TO_A,      /*BinaryFloat*/
            IC_NO_CAST,     /*BinaryDouble*/
            IC_B_TO_A,      /*Number*/
            IC_B_TO_A,      /*Char*/
            IC_B_TO_A,      /*Varchar*/
            IC_NOT_SUPPORT, /*Date*/
            IC_NOT_SUPPORT, /*TimestampTZ*/
            IC_NOT_SUPPORT, /*TimestampLTZ*/
            IC_NOT_SUPPORT, /*Timestamp*/
            IC_NOT_SUPPORT, /*IntervalYM*/
            IC_NOT_SUPPORT, /*IntervalDS*/
            IC_B_TO_A,      /*Lob*/
            IC_NOT_SUPPORT, /*Extend*/
            IC_NOT_SUPPORT, /*Unknown*/
            IC_NOT_SUPPORT, /*Raw*/
            IC_B_TO_A,      /*NVarchar2*/
            IC_B_TO_A,      /*NChar*/
            IC_NOT_SUPPORT, /*URowID*/
            IC_B_TO_A,      /*LobLocator*/
            IC_NOT_SUPPORT, /*ObOMaxType*/
        },
        {
            /*Number->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NO_CAST,     /*Null*/
            IC_NO_CAST,     // IC_B_TO_A,  /*SmallInt*/
            IC_NO_CAST,     // IC_B_TO_A,  /*Int*/
            IC_B_TO_A,      /*Float*/
            IC_A_TO_B,      /*BinaryFloat*/
            IC_A_TO_B,      /*BinaryDouble*/
            IC_NO_CAST,     /*Number*/
            IC_B_TO_A,      /*Char*/
            IC_B_TO_A,      /*Varchar*/
            IC_NOT_SUPPORT, /*Date*/
            IC_NOT_SUPPORT, /*TimestampTZ*/
            IC_NOT_SUPPORT, /*TimestampLTZ*/
            IC_NOT_SUPPORT, /*Timestamp*/
            IC_NOT_SUPPORT, /*IntervalYM*/
            IC_NOT_SUPPORT, /*IntervalDS*/
            IC_B_TO_A,      /*Lob*/
            IC_NOT_SUPPORT, /*Extend*/
            IC_NOT_SUPPORT, /*Unknown*/
            IC_NOT_SUPPORT, /*Raw*/
            IC_B_TO_A,      /*NVarchar2*/
            IC_B_TO_A,      /*NChar*/
            IC_NOT_SUPPORT, /*URowID*/
            IC_B_TO_A,      /*LobLocator*/
            IC_NOT_SUPPORT, /*ObOMaxType*/
        },
        {
            /*Char->XXX*/
            IC_NOT_SUPPORT,    /*ObONotSupport*/
            IC_NO_CAST,        /*Null*/
            IC_TO_MIDDLE_TYPE, /*SmallInt*/
            IC_TO_MIDDLE_TYPE, /*Int*/
            IC_TO_MIDDLE_TYPE, /*Float*/
            IC_A_TO_B,         /*BinaryFloat*/
            IC_A_TO_B,         /*BinaryDouble*/
            IC_A_TO_B,         /*Number*/
            IC_NO_CAST,        /*Char*/
            IC_NO_CAST,        /*Varchar*/
            IC_A_TO_B,         /*Date*/
            IC_A_TO_B,         /*TimestampTZ*/
            IC_A_TO_B,         /*TimestampLTZ*/
            IC_A_TO_B,         /*Timestamp*/
            IC_A_TO_B,         /*IntervalYM*/
            IC_A_TO_B,         /*IntervalDS*/
            IC_B_TO_A,         /*Lob*/
            IC_NOT_SUPPORT,    /*Extend*/
            IC_NOT_SUPPORT,    /*Unknown*/
            IC_B_TO_A,         /*Raw*/
            IC_A_TO_B,         /*NVarchar2*/
            IC_A_TO_B,         /*NChar*/
            IC_A_TO_B,         /*URowID*/
            IC_B_TO_A,         /*LobLocator*/
            IC_NOT_SUPPORT,    /*ObOMaxType*/
        },
        {
            /*Varchar->XXX*/
            IC_NOT_SUPPORT,    /*ObONotSupport*/
            IC_NO_CAST,        /*Null*/
            IC_TO_MIDDLE_TYPE, /*SmallInt*/
            IC_TO_MIDDLE_TYPE, /*Int*/
            IC_TO_MIDDLE_TYPE, /*Float*/
            IC_A_TO_B,         /*BinaryFloat*/
            IC_A_TO_B,         /*BinaryDouble*/
            IC_A_TO_B,         /*Number*/
            IC_NO_CAST,        /*Char*/
            IC_NO_CAST,        /*Varchar*/
            IC_A_TO_B,         /*Date*/
            IC_A_TO_B,         /*TimestampTZ*/
            IC_A_TO_B,         /*TimestampLTZ*/
            IC_A_TO_B,         /*Timestamp*/
            IC_A_TO_B,         /*IntervalYM*/
            IC_A_TO_B,         /*IntervalDS*/
            IC_B_TO_A,         /*Lob*/
            IC_NOT_SUPPORT,    /*Extend*/
            IC_NOT_SUPPORT,    /*Unknown*/
            IC_B_TO_A,         /*Raw*/
            IC_A_TO_B,         /*NVarchar2*/
            IC_A_TO_C,         /*NChar*/
            IC_A_TO_B,         /*URowID*/
            IC_B_TO_A,         /*LobLocator*/
            IC_NOT_SUPPORT,    /*ObOMaxType*/
        },
        {
            /*Date->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NO_CAST,     /*Null*/
            IC_NOT_SUPPORT, /*SmallInt*/
            IC_NOT_SUPPORT, /*Int*/
            IC_NOT_SUPPORT, /*Float*/
            IC_NOT_SUPPORT, /*BinaryFloat*/
            IC_NOT_SUPPORT, /*BinaryDouble*/
            IC_NOT_SUPPORT, /*Number*/
            IC_B_TO_A,      /*Char*/
            IC_B_TO_A,      /*Varchar*/
            IC_NO_CAST,     /*Date*/
            IC_A_TO_B,      /*TimestampTZ*/
            IC_A_TO_B,      /*TimestampLTZ*/
            IC_A_TO_B,      /*Timestamp*/
            IC_NOT_SUPPORT, /*IntervalYM*/
            IC_NOT_SUPPORT, /*IntervalDS*/
            IC_B_TO_A,      /*Lob*/
            IC_NOT_SUPPORT, /*Extend*/
            IC_NOT_SUPPORT, /*Unknown*/
            IC_NOT_SUPPORT, /*Raw*/
            IC_B_TO_A,      /*NVarchar2*/
            IC_B_TO_A,      /*NChar*/
            IC_NOT_SUPPORT, /*URowID*/
            IC_B_TO_A,      /*LobLocator*/
            IC_NOT_SUPPORT, /*ObOMaxType*/
        },
        {
            /*TimestampTZ->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NO_CAST,     /*Null*/
            IC_NOT_SUPPORT, /*SmallInt*/
            IC_NOT_SUPPORT, /*Int*/
            IC_NOT_SUPPORT, /*Float*/
            IC_NOT_SUPPORT, /*BinaryFloat*/
            IC_NOT_SUPPORT, /*BinaryDouble*/
            IC_NOT_SUPPORT, /*Number*/
            IC_B_TO_A,      /*Char*/
            IC_B_TO_A,      /*Varchar*/
            IC_B_TO_A,      /*Date*/
            IC_NO_CAST,     /*TimestampTZ*/
            IC_B_TO_A,
            /*TimestampLTZ*/  // oracle called internal_function(TimestampLTZ) when cmp with TimestampTZ
            IC_B_TO_A,        /*Timestamp*/
            IC_NOT_SUPPORT,   /*IntervalYM*/
            IC_NOT_SUPPORT,   /*IntervalDS*/
            IC_B_TO_A,        /*Lob*/
            IC_NOT_SUPPORT,   /*Extend*/
            IC_NOT_SUPPORT,   /*Unknown*/
            IC_NOT_SUPPORT,   /*Raw*/
            IC_B_TO_A,        /*NVarchar2*/
            IC_B_TO_A,        /*NChar*/
            IC_NOT_SUPPORT,   /*URowID*/
            IC_B_TO_A,        /*LobLocator*/
            IC_NOT_SUPPORT,   /*ObOMaxType*/
        },
        {
            /*TimestampLTZ->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NO_CAST,     /*Null*/
            IC_NOT_SUPPORT, /*SmallInt*/
            IC_NOT_SUPPORT, /*Int*/
            IC_NOT_SUPPORT, /*Float*/
            IC_NOT_SUPPORT, /*BinaryFloat*/
            IC_NOT_SUPPORT, /*BinaryDouble*/
            IC_NOT_SUPPORT, /*Number*/
            IC_B_TO_A,      /*Char*/
            IC_B_TO_A,      /*Varchar*/
            IC_B_TO_A,      /*Date*/
            IC_A_TO_B,
            /*TimestampTZ*/  // oracle called internal_function(TimestampLTZ) when cmp with TimestampTZ
            IC_NO_CAST,      /*TimestampLTZ*/
            IC_B_TO_A,       /*Timestamp*/
            IC_NOT_SUPPORT,  /*IntervalYM*/
            IC_NOT_SUPPORT,  /*IntervalDS*/
            IC_B_TO_A,       /*Lob*/
            IC_NOT_SUPPORT,  /*Extend*/
            IC_NOT_SUPPORT,  /*Unknown*/
            IC_NOT_SUPPORT,  /*Raw*/
            IC_B_TO_A,       /*NVarchar2*/
            IC_B_TO_A,       /*NChar*/
            IC_NOT_SUPPORT,  /*URowID*/
            IC_B_TO_A,       /*LobLocator*/
            IC_NOT_SUPPORT,  /*ObOMaxType*/
        },
        {
            /*Timestamp->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NO_CAST,     /*Null*/
            IC_NOT_SUPPORT, /*SmallInt*/
            IC_NOT_SUPPORT, /*Int*/
            IC_NOT_SUPPORT, /*Float*/
            IC_NOT_SUPPORT, /*BinaryFloat*/
            IC_NOT_SUPPORT, /*BinaryDouble*/
            IC_NOT_SUPPORT, /*Number*/
            IC_B_TO_A,      /*Char*/
            IC_B_TO_A,      /*Varchar*/
            IC_B_TO_A,      /*Date*/
            IC_A_TO_B,      /*TimestampTZ*/
            IC_A_TO_B,      /*TimestampLTZ*/
            IC_NO_CAST,     /*Timestamp*/
            IC_NOT_SUPPORT, /*IntervalYM*/
            IC_NOT_SUPPORT, /*IntervalDS*/
            IC_B_TO_A,      /*Lob*/
            IC_NOT_SUPPORT, /*Extend*/
            IC_NOT_SUPPORT, /*Unknown*/
            IC_NOT_SUPPORT, /*Raw*/
            IC_B_TO_A,      /*NVarchar2*/
            IC_B_TO_A,      /*NChar*/
            IC_NOT_SUPPORT, /*ObRowID*/
            IC_B_TO_A,      /*LobLocator*/
            IC_NOT_SUPPORT, /*ObOMaxType*/
        },
        {
            /*IntervalYM->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NO_CAST,     /*Null*/
            IC_NOT_SUPPORT, /*SmallInt*/
            IC_NOT_SUPPORT, /*Int*/
            IC_NOT_SUPPORT, /*Float*/
            IC_NOT_SUPPORT, /*BinaryFloat*/
            IC_NOT_SUPPORT, /*BinaryDouble*/
            IC_NOT_SUPPORT, /*Number*/
            IC_B_TO_A,      /*Char*/
            IC_B_TO_A,      /*Varchar*/
            IC_NO_CAST,     /*Date*/
            IC_NO_CAST,     /*TimestampTZ*/
            IC_NO_CAST,     /*TimestampLTZ*/
            IC_NO_CAST,     /*Timestamp*/
            IC_NO_CAST,     /*IntervalYM*/
            IC_NOT_SUPPORT, /*IntervalDS*/
            IC_B_TO_A,      /*Lob*/
            IC_NOT_SUPPORT, /*Extend*/
            IC_NOT_SUPPORT, /*Unknown*/
            IC_NOT_SUPPORT, /*Raw*/
            IC_B_TO_A,      /*NVarchar2*/
            IC_B_TO_A,      /*NChar*/
            IC_NOT_SUPPORT, /*ObRowID*/
            IC_B_TO_A,      /*LobLocator*/
            IC_NOT_SUPPORT, /*ObOMaxType*/
        },
        {
            /*IntervalDS->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NO_CAST,     /*Null*/
            IC_NOT_SUPPORT, /*SmallInt*/
            IC_NOT_SUPPORT, /*Int*/
            IC_NOT_SUPPORT, /*Float*/
            IC_NOT_SUPPORT, /*BinaryFloat*/
            IC_NOT_SUPPORT, /*BinaryDouble*/
            IC_NOT_SUPPORT, /*Number*/
            IC_B_TO_A,      /*Char*/
            IC_B_TO_A,      /*Varchar*/
            IC_NO_CAST,     /*Date*/
            IC_NO_CAST,     /*TimestampTZ*/
            IC_NO_CAST,     /*TimestampLTZ*/
            IC_NO_CAST,     /*Timestamp*/
            IC_NOT_SUPPORT, /*IntervalYM*/
            IC_NO_CAST,     /*IntervalDS*/
            IC_B_TO_A,      /*Lob*/
            IC_NOT_SUPPORT, /*Extend*/
            IC_NOT_SUPPORT, /*Unknown*/
            IC_NOT_SUPPORT, /*Raw*/
            IC_B_TO_A,      /*NVarchar2*/
            IC_B_TO_A,      /*NChar*/
            IC_NOT_SUPPORT, /*ObRowID*/
            IC_B_TO_A,      /*LobLocator*/
            IC_NOT_SUPPORT, /*ObOMaxType*/
        },
        {
            /*Lob->XXX*/
            IC_NOT_SUPPORT,    /*ObONotSupport*/
            IC_NO_CAST,        /*Null*/
            IC_TO_MIDDLE_TYPE, /*SmallInt*/
            IC_TO_MIDDLE_TYPE, /*Int*/
            IC_TO_MIDDLE_TYPE, /*Float*/
            IC_A_TO_B,         /*BinaryFloat*/
            IC_A_TO_B,         /*BinaryDouble*/
            IC_A_TO_B,         /*Number*/
            IC_A_TO_B,         /*Char*/
            IC_A_TO_B,         /*Varchar*/
            IC_A_TO_B,         /*Date*/
            IC_A_TO_B,         /*TimestampTZ*/
            IC_A_TO_B,         /*TimestampLTZ*/
            IC_A_TO_B,         /*Timestamp*/
            IC_A_TO_B,         /*IntervalYM*/
            IC_A_TO_B,         /*IntervalDS*/
            IC_NO_CAST,        /*Lob*/
            IC_NOT_SUPPORT,    /*Extend*/
            IC_NOT_SUPPORT,    /*Unknown*/
            IC_NOT_SUPPORT,    /*Raw*/
            IC_B_TO_A,         /*NVarchar2*/
            IC_B_TO_A,         /*NChar*/
            IC_NOT_SUPPORT,    /*URowID*/
            IC_B_TO_A,         /*LobLocator*/
            IC_NOT_SUPPORT,    /*ObOMaxType*/
        },
        {
            /*Extend->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NO_CAST,     /*Null*/
            IC_NOT_SUPPORT, /*SmallInt*/
            IC_NOT_SUPPORT, /*Int*/
            IC_NOT_SUPPORT, /*Float*/
            IC_NOT_SUPPORT, /*BinaryFloat*/
            IC_NOT_SUPPORT, /*BinaryDouble*/
            IC_NOT_SUPPORT, /*Number*/
            IC_NOT_SUPPORT, /*Char*/
            IC_NOT_SUPPORT, /*Varchar*/
            IC_NOT_SUPPORT, /*Date*/
            IC_NOT_SUPPORT, /*TimestampTZ*/
            IC_NOT_SUPPORT, /*TimestampLTZ*/
            IC_NOT_SUPPORT, /*Timestamp*/
            IC_NOT_SUPPORT, /*IntervalYM*/
            IC_NOT_SUPPORT, /*IntervalDS*/
            IC_NOT_SUPPORT, /*Lob*/
            IC_NO_CAST,     /*Extend*/
            IC_NOT_SUPPORT, /*Unknown*/
            IC_NOT_SUPPORT, /*Raw*/
            IC_NOT_SUPPORT, /*NVarchar2*/
            IC_NOT_SUPPORT, /*NChar*/
            IC_NOT_SUPPORT, /*URowID*/
            IC_NOT_SUPPORT, /*LobLocator*/
            IC_NOT_SUPPORT, /*ObOMaxType*/
        },
        {
            /*UnKnown->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NO_CAST,     /*Null*/
            IC_NOT_SUPPORT, /*SmallInt*/
            IC_NOT_SUPPORT, /*Int*/
            IC_NOT_SUPPORT, /*Float*/
            IC_NOT_SUPPORT, /*BinaryFloat*/
            IC_NOT_SUPPORT, /*BinaryDouble*/
            IC_NOT_SUPPORT, /*Number*/
            IC_NOT_SUPPORT, /*Char*/
            IC_NOT_SUPPORT, /*Varchar*/
            IC_NOT_SUPPORT, /*Date*/
            IC_NOT_SUPPORT, /*TimestampTZ*/
            IC_NOT_SUPPORT, /*TimestampLTZ*/
            IC_NOT_SUPPORT, /*Timestamp*/
            IC_NOT_SUPPORT, /*IntervalYM*/
            IC_NOT_SUPPORT, /*IntervalDS*/
            IC_NOT_SUPPORT, /*Lob*/
            IC_NOT_SUPPORT, /*Extend*/
            IC_NO_CAST,     /*Unknown*/
            IC_NOT_SUPPORT, /*Raw*/
            IC_NOT_SUPPORT, /*NVarchar2*/
            IC_NOT_SUPPORT, /*NChar*/
            IC_NOT_SUPPORT, /*URowID*/
            IC_NOT_SUPPORT, /*LobLocator*/
            IC_NOT_SUPPORT, /*ObOMaxType*/
        },
        {
            /*Raw->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NO_CAST,     /*Null*/
            IC_NO_CAST,     /*SmallInt*/
            IC_NO_CAST,     /*Int*/
            IC_NO_CAST,     /*Float*/
            IC_NO_CAST,     /*BinaryFloat*/
            IC_NO_CAST,     /*BinaryDouble*/
            IC_NO_CAST,     /*Number*/
            IC_A_TO_B,      /*Char*/
            IC_A_TO_B,      /*Varchar*/
            IC_NO_CAST,     /*Date*/
            IC_NO_CAST,     /*TimestampTZ*/
            IC_NO_CAST,     /*TimestampLTZ*/
            IC_NO_CAST,     /*Timestamp*/
            IC_NOT_SUPPORT, /*IntervalYear*/
            IC_NOT_SUPPORT, /*IntervalDay*/
            IC_NOT_SUPPORT, /*Lob*/
            IC_NO_CAST,     /*Extend*/
            IC_NO_CAST,     /*Unknown*/
            IC_NO_CAST,     /*Raw*/
            IC_A_TO_B,      /*NVarchar2*/
            IC_A_TO_B,      /*NChar*/
            IC_NOT_SUPPORT, /*URowID*/
            IC_NOT_SUPPORT, /*LobLocator*/
            IC_NOT_SUPPORT, /*ObOMaxType*/
        },
        {
            /*NVarchar->XXX*/
            IC_NOT_SUPPORT,    /*ObONotSupport*/
            IC_NO_CAST,        /*Null*/
            IC_TO_MIDDLE_TYPE, /*SmallInt*/
            IC_TO_MIDDLE_TYPE, /*Int*/
            IC_TO_MIDDLE_TYPE, /*Float*/
            IC_A_TO_B,         /*BinaryFloat*/
            IC_A_TO_B,         /*BinaryDouble*/
            IC_A_TO_B,         /*Number*/
            IC_B_TO_A,         /*Char*/
            IC_B_TO_A,         /*Varchar*/
            IC_A_TO_B,         /*Date*/
            IC_A_TO_B,         /*TimestampTZ*/
            IC_A_TO_B,         /*TimestampLTZ*/
            IC_A_TO_B,         /*Timestamp*/
            IC_A_TO_B,         /*IntervalYM*/
            IC_A_TO_B,         /*IntervalDS*/
            IC_B_TO_A,         /*Lob*/
            IC_NOT_SUPPORT,    /*Extend*/
            IC_NOT_SUPPORT,    /*Unknown*/
            IC_B_TO_A,         /*Raw*/
            IC_NO_CAST,        /*NVarchar2*/
            IC_NO_CAST,        /*NChar*/
            IC_A_TO_B,         /*URowID*/
            IC_B_TO_A,         /*LobLocator*/
            IC_NOT_SUPPORT,    /*ObOMaxType*/
        },
        {
            /*NChar->XXX*/
            IC_NOT_SUPPORT,    /*ObONotSupport*/
            IC_NO_CAST,        /*Null*/
            IC_TO_MIDDLE_TYPE, /*SmallInt*/
            IC_TO_MIDDLE_TYPE, /*Int*/
            IC_TO_MIDDLE_TYPE, /*Float*/
            IC_A_TO_B,         /*BinaryFloat*/
            IC_A_TO_B,         /*BinaryDouble*/
            IC_A_TO_B,         /*Number*/
            IC_B_TO_A,         /*Char*/
            IC_B_TO_C,         /*Varchar*/
            IC_A_TO_B,         /*Date*/
            IC_A_TO_B,         /*TimestampTZ*/
            IC_A_TO_B,         /*TimestampLTZ*/
            IC_A_TO_B,         /*Timestamp*/
            IC_A_TO_B,         /*IntervalYM*/
            IC_A_TO_B,         /*IntervalDS*/
            IC_B_TO_A,         /*Lob*/
            IC_NOT_SUPPORT,    /*Extend*/
            IC_NOT_SUPPORT,    /*Unknown*/
            IC_B_TO_A,         /*Raw*/
            IC_NO_CAST,        /*NVarchar2*/
            IC_NO_CAST,        /*NChar*/
            IC_A_TO_B,         /*URowID*/
            IC_B_TO_A,         /*LobLocator*/
            IC_NOT_SUPPORT,    /*ObOMaxType*/
        },
        {
            /*URowID->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NO_CAST,     /*Null*/
            IC_NOT_SUPPORT, /*SmallInt*/
            IC_NOT_SUPPORT, /*Int*/
            IC_NOT_SUPPORT, /*Float*/
            IC_NOT_SUPPORT, /*BinaryFloat*/
            IC_NOT_SUPPORT, /*BinaryDouble*/
            IC_NOT_SUPPORT, /*Number*/
            IC_B_TO_A,      /*Char*/
            IC_B_TO_A,      /*Varchar*/
            IC_NOT_SUPPORT, /*Date*/
            IC_NOT_SUPPORT, /*TimestampTZ*/
            IC_NOT_SUPPORT, /*TimestampLTZ*/
            IC_NOT_SUPPORT, /*Timestamp*/
            IC_NOT_SUPPORT, /*IntervalYM*/
            IC_NOT_SUPPORT, /*IntervalDS*/
            IC_NOT_SUPPORT, /*Lob*/
            IC_NOT_SUPPORT, /*Extend*/
            IC_NOT_SUPPORT, /*Unknown*/
            IC_NOT_SUPPORT, /*Raw*/
            IC_B_TO_A,      /*NVarchar2*/
            IC_B_TO_A,      /*NChar*/
            IC_NO_CAST,     /*URowID*/
            IC_NOT_SUPPORT, /*LobLocator*/
            IC_NOT_SUPPORT, /*ObOMaxType*/
        },
        {
            /*LobLocator->XXX*/
            IC_NOT_SUPPORT,    /*ObONotSupport*/
            IC_NO_CAST,        /*Null*/
            IC_TO_MIDDLE_TYPE, /*SmallInt*/
            IC_TO_MIDDLE_TYPE, /*Int*/
            IC_A_TO_B,         /*Float*/
            IC_A_TO_B,         /*BinaryFloat*/
            IC_A_TO_B,         /*BinaryDouble*/
            IC_A_TO_B,         /*Number*/
            IC_A_TO_B,         /*Char*/
            IC_A_TO_B,         /*Varchar*/
            IC_A_TO_B,         /*Date*/
            IC_A_TO_B,         /*TimestampTZ*/
            IC_A_TO_B,         /*TimestampLTZ*/
            IC_A_TO_B,         /*Timestamp*/
            IC_A_TO_B,         /*IntervalYM*/
            IC_A_TO_B,         /*IntervalDS*/
            IC_A_TO_B,         /*Lob*/
            IC_NOT_SUPPORT,    /*Extend*/
            IC_NOT_SUPPORT,    /*Unknown*/
            IC_NOT_SUPPORT,    /*Raw*/
            IC_A_TO_B,         /*NVarchar2*/
            IC_A_TO_B,         /*NChar*/
            IC_NOT_SUPPORT,    /*URowID*/
            IC_NO_CAST,        /*LobLocator*/
            IC_NOT_SUPPORT,    /*ObOMaxType*/
        },
        {
            /*ObOMaxType->XXX*/
            IC_NOT_SUPPORT, /*ObONotSupport*/
            IC_NOT_SUPPORT, /*Null*/
            IC_NOT_SUPPORT, /*SmallInt*/
            IC_NOT_SUPPORT, /*Int*/
            IC_NOT_SUPPORT, /*Float*/
            IC_NOT_SUPPORT, /*BinaryFloat*/
            IC_NOT_SUPPORT, /*BinaryDouble*/
            IC_NOT_SUPPORT, /*Number*/
            IC_NOT_SUPPORT, /*Char*/
            IC_NOT_SUPPORT, /*Varchar*/
            IC_NOT_SUPPORT, /*Date*/
            IC_NOT_SUPPORT, /*TimestampTZ*/
            IC_NOT_SUPPORT, /*TimestampLTZ*/
            IC_NOT_SUPPORT, /*Timestamp*/
            IC_NOT_SUPPORT, /*IntervalYM*/
            IC_NOT_SUPPORT, /*IntervalDS*/
            IC_NOT_SUPPORT, /*Lob*/
            IC_NOT_SUPPORT, /*Extend*/
            IC_NOT_SUPPORT, /*Unknown*/
            IC_NOT_SUPPORT, /*Raw*/
            IC_NOT_SUPPORT, /*LobLocator*/
            IC_NOT_SUPPORT, /*ObOMaxType*/
        },
};

OB_INLINE bool is_valid_obj_type(const ObObjType type)
{
  return ObNullType <= type && type < ObMaxType;
}

OB_INLINE bool is_valid_oracle_type(const ObObjOType otype)
{
  // not include ObONotSupport
  return ObONotSupport < otype && otype < ObOMaxType;
}

OB_INLINE ObObjOType ob_obj_type_to_oracle_type(const ObObjType type)
{
  return type < ObMaxType ? OBJ_TYPE_TO_O_TYPE[type] : ObOMaxType;
}

OB_INLINE ObObjTypeClass ob_oracle_type_class(const ObObjType type)
{
  return type < ObMaxType ? OBJ_O_TYPE_TO_CLASS[OBJ_TYPE_TO_O_TYPE[type]] : ObMaxTC;
}

OB_INLINE ObObjTypeClass ob_oracle_type_class(const ObObjOType oType)
{
  return oType < ObOMaxType ? OBJ_O_TYPE_TO_CLASS[oType] : ObMaxTC;
}

OB_INLINE ImplicitCastDirection ob_oracle_implict_cast_direction(const ObObjOType aType, const ObObjOType bType)
{
  if (aType < ObOMaxType && bType < ObOMaxType) {
    return OB_OBJ_IMPLICIT_CAST_DIRECTION_FOR_ORACLE[aType][bType];
  } else {
    return ImplicitCastDirection::IC_NOT_SUPPORT;
  }
}

OB_INLINE ObObjTypeClass ob_obj_type_class(const ObObjType type)
{
  return OB_LIKELY(type < ObMaxType) ? OBJ_TYPE_TO_CLASS[type] : ObMaxTC;
}

OB_INLINE ObObjType ob_obj_default_type(const ObObjTypeClass tc)
{
  return OB_LIKELY(tc < ObActualMaxTC) ? OBJ_DEFAULT_TYPE[tc] : ObMaxType;
}

extern const int64_t INT_MIN_VAL[ObMaxType];
extern const int64_t INT_MAX_VAL[ObMaxType];
extern const uint64_t UINT_MAX_VAL[ObMaxType];
extern const double REAL_MIN_VAL[ObMaxType];
extern const double REAL_MAX_VAL[ObMaxType];

OB_INLINE bool ob_is_castable_type_class(ObObjTypeClass tc)
{
  return (ObIntTC <= tc && tc <= ObStringTC) || ObLeftTypeTC == tc || ObRightTypeTC == tc || ObBitTC == tc ||
         ObEnumSetTC == tc || ObEnumSetInnerTC == tc || ObTextTC == tc || ObOTimestampTC == tc || ObRawTC == tc ||
         ObIntervalTC == tc || ObRowIDTC == tc || ObLobTC == tc || ObJsonTC == tc;
}

// used for arithmetic
OB_INLINE bool ob_is_int_uint(ObObjTypeClass left_tc, ObObjTypeClass right_tc)
{
  return (ObIntTC == left_tc && ObUIntTC == right_tc) || (ObIntTC == right_tc && ObUIntTC == left_tc);
}

// print obj type string
const char* ob_obj_type_str(ObObjType type);
const char* ob_sql_type_str(ObObjType type);

// such as "double(10,7)". with accuracy
int ob_sql_type_str(char* buff, int64_t buff_length, int64_t& pos, ObObjType type, int64_t length, int64_t precision,
    int64_t scale, ObCollationType coll_type);
int ob_sql_type_str(const common::ObObjMeta& obj_meta, const common::ObAccuracy& accuracy,
    const common::ObIArray<ObString>& type_info, const int16_t default_length_semantics, char* buff,
    int64_t buff_length, int64_t& pos);

// such as "double". without any accuracy.
int ob_sql_type_str(char* buff, int64_t buff_length, ObObjType type, ObCollationType coll_type);

// print obj type class string
const char* ob_obj_tc_str(ObObjTypeClass tc);
const char* ob_sql_tc_str(ObObjTypeClass tc);

// get obj type size for fixed length type
int32_t ob_obj_type_size(ObObjType type);

inline bool ob_is_valid_obj_type(ObObjType type)
{
  return ObNullType <= type && type < ObMaxType;
}
inline bool ob_is_invalid_obj_type(ObObjType type)
{
  return !ob_is_valid_obj_type(type);
}
inline bool ob_is_valid_obj_tc(ObObjTypeClass tc)
{
  return ObNullTC <= tc && tc < ObMaxTC;
}
inline bool ob_is_invalid_obj_tc(ObObjTypeClass tc)
{
  return !ob_is_valid_obj_tc(tc);
}

inline bool ob_is_valid_obj_o_type(ObObjType type)
{
  return ObNullType <= type && type < ObMaxType ? (OBJ_TYPE_TO_O_TYPE[type] != ObONotSupport) : false;
}
inline bool ob_is_invalid_obj_o_type(ObObjType type)
{
  return !ob_is_valid_obj_o_type(type);
}
inline bool ob_is_valid_obj_o_tc(ObObjTypeClass tc)
{
  return ObNullTC <= tc && tc < ObMaxTC;
}
inline bool ob_is_invalid_obj_o_tc(ObObjTypeClass tc)
{
  return !ob_is_valid_obj_tc(tc);
}
inline bool ob_is_numeric_tc(ObObjTypeClass tc)
{
  return (tc >= ObIntTC && tc <= ObNumberTC) || tc == ObBitTC;
}

inline bool ob_is_int_tc(ObObjType type)
{
  return ObIntTC == ob_obj_type_class(type);
}
inline bool ob_is_uint_tc(ObObjType type)
{
  return ObUIntTC == ob_obj_type_class(type);
}
inline bool ob_is_float_tc(ObObjType type)
{
  return ObFloatTC == ob_obj_type_class(type);
}
inline bool ob_is_double_tc(ObObjType type)
{
  return ObDoubleTC == ob_obj_type_class(type);
}
inline bool ob_is_number_tc(ObObjType type)
{
  return ObNumberTC == ob_obj_type_class(type);
}
inline bool ob_is_datetime_tc(ObObjType type)
{
  return ObDateTimeTC == ob_obj_type_class(type);
}
inline bool ob_is_date_tc(ObObjType type)
{
  return ObDateTC == ob_obj_type_class(type);
}
inline bool ob_is_otimestampe_tc(ObObjType type)
{
  return ObOTimestampTC == ob_obj_type_class(type);
}
inline bool ob_is_time_tc(ObObjType type)
{
  return ObTimeTC == ob_obj_type_class(type);
}
inline bool ob_is_year_tc(ObObjType type)
{
  return ObYearTC == ob_obj_type_class(type);
}
inline bool ob_is_string_tc(ObObjType type)
{
  return ObStringTC == ob_obj_type_class(type);
}
inline bool ob_is_text_tc(ObObjType type)
{
  return ObTextTC == ob_obj_type_class(type);
}
inline bool ob_is_bit_tc(ObObjType type)
{
  return ObBitTC == ob_obj_type_class(type);
}
inline bool ob_is_raw_tc(ObObjType type)
{
  return ObRawTC == ob_obj_type_class(type);
}
inline bool ob_is_interval_tc(ObObjType type)
{
  return ObIntervalTC == ob_obj_type_class(type);
}
inline bool ob_is_lob_tc(ObObjType type)
{
  return ObLobTC == ob_obj_type_class(type);
}
inline bool ob_is_json_tc(ObObjType type) 
{ 
  return ObJsonTC == ob_obj_type_class(type); 
}
inline bool is_lob(ObObjType type)
{
  return ob_is_text_tc(type);
}
inline bool is_lob_locator(ObObjType type)
{
  return ObLobType == type;
}

// test type catalog
inline bool ob_is_integer_type(ObObjType type)
{
  return ObTinyIntType <= type && type <= ObUInt64Type;
}
inline bool ob_is_numeric_type(ObObjType type)
{
  return (ObTinyIntType <= type && type <= ObUNumberType) || type == ObBitType || type == ObNumberFloatType;
}
inline bool ob_is_real_type(ObObjType type)
{
  return ObFloatType <= type && type <= ObUDoubleType;
}
inline bool ob_is_float_type(ObObjType type)
{
  return ObFloatType == type || ObUFloatType == type;
}
inline bool ob_is_double_type(ObObjType type)
{
  return ObDoubleType == type || ObUDoubleType == type;
}
inline bool ob_is_accurate_numeric_type(ObObjType type)
{
  return (ObTinyIntType <= type && type <= ObUInt64Type) || (ob_is_number_tc(type));
}
inline bool ob_is_unsigned_type(ObObjType type)
{
  return (ObUTinyIntType <= type && type <= ObUInt64Type) || ObYearType == type || ObUFloatType == type ||
         ObUDoubleType == type || ObUNumberType == type;
}

inline void convert_unsigned_type_to_signed(ObObjType &type)
{
  switch (type) {
    case (ObUTinyIntType):
      type = ObTinyIntType;
      break;
    case (ObUSmallIntType):
      type = ObSmallIntType;
      break;
    case (ObUMediumIntType):
      type = ObMediumIntType;
      break;
    case (ObUInt32Type):
      type = ObInt32Type;
      break;
    case (ObUInt64Type):
      type = ObIntType;
      break;
    case (ObUFloatType):
      type = ObFloatType;
      break;
    case (ObUDoubleType):
      type = ObDoubleType;
      break;
    case (ObUNumberType):
      type = ObNumberType;
      break;
    default:
      break;
  }
}

inline bool ob_is_oracle_numeric_type(ObObjType type)
{
  return ObIntType == type || ob_is_number_tc(type) || ObFloatType == type || ObDoubleType == type;
}
inline bool ob_is_oracle_temporal_type(ObObjType type)
{
  return ObDateTimeType == type || ob_is_otimestampe_tc(type);
}

// this func means when eumset cast to this type, use numberic value
inline bool ob_is_enumset_numeric_type(ObObjType type)
{
  return (ob_is_numeric_type(type) || ObYearType == type);
}

inline bool ob_is_enum_or_set_type(ObObjType type)
{
  return ObEnumType == type || ObSetType == type;
}
inline bool ob_is_temporal_type(ObObjType type)
{
  return type >= ObDateTimeType && type <= ObYearType;
}
inline bool ob_is_string_or_enumset_type(ObObjType type)
{
  return (type >= ObVarcharType && type <= ObHexStringType) || ObEnumType == type || ObSetType == type ||
         ObNVarchar2Type == type || ObNCharType == type;
}
inline bool ob_is_otimestamp_type(const ObObjType type)
{
  return (ObTimestampTZType <= type && type <= ObTimestampNanoType);
}
inline bool ob_is_varbinary_type(const ObObjType type, const ObCollationType cs_type)
{
  return (ObVarcharType == type && CS_TYPE_BINARY == cs_type);
}
inline bool ob_is_varchar_char_type(const ObObjType type, const ObCollationType cs_type)
{
  return (ObVarcharType == type || ObCharType == type) && CS_TYPE_BINARY != cs_type;
}
inline bool ob_is_varchar_type(const ObObjType type, const ObCollationType cs_type)
{
  return ObVarcharType == type && CS_TYPE_BINARY != cs_type;
}

inline bool ob_is_binary(ObObjType type, ObCollationType cs_type)
{
  return (ObCharType == type && CS_TYPE_BINARY == cs_type);
}
inline bool ob_is_varbinary_or_binary(ObObjType type, ObCollationType cs_type)
{
  return ob_is_varbinary_type(type, cs_type) || ob_is_binary(type, cs_type);
}
inline bool ob_is_varchar(ObObjType type, ObCollationType cs_type)
{
  return ((type == static_cast<uint8_t>(ObVarcharType)) && (CS_TYPE_BINARY != cs_type));
}
inline bool ob_is_char(ObObjType type, ObCollationType cs_type)
{
  return ((type == static_cast<uint8_t>(ObCharType)) && (CS_TYPE_BINARY != cs_type));
}
inline bool ob_is_nstring(ObObjType type)
{
  return type == static_cast<uint8_t>(ObNVarchar2Type) || type == static_cast<uint8_t>(ObNCharType);
}
inline bool ob_is_varchar_or_char(ObObjType type, ObCollationType cs_type)
{
  return ob_is_varchar(type, cs_type) || ob_is_char(type, cs_type);
}
inline bool ob_is_character_type(ObObjType type, ObCollationType cs_type)
{
  return ob_is_nstring(type) || ob_is_varchar_or_char(type, cs_type);
}
inline bool ob_is_string_type(ObObjType type)
{
  return ob_is_string_tc(type) || ob_is_text_tc(type);
}

inline bool ob_is_blob_locator(const ObObjType type, const ObCollationType cs_type)
{
  return ObLobType == type && CS_TYPE_BINARY == cs_type;
}
inline bool ob_is_clob_locator(const ObObjType type, const ObCollationType cs_type)
{
  return ObLobType == type && CS_TYPE_BINARY != cs_type;
}
inline bool ob_is_lob_locator(const ObObjType type)
{
  return ObLobType == type;
}
inline bool ob_is_string_or_lob_type(ObObjType type)
{
  return ob_is_string_tc(type) || ob_is_text_tc(type) || ob_is_lob_locator(type);
}
inline bool ob_is_clob(const ObObjType type, const ObCollationType cs_type)
{
  return ObLongTextType == type && CS_TYPE_BINARY != cs_type;
}
inline bool ob_is_blob(const ObObjType type, const ObCollationType cs_type)
{
  return ObTextTC == ob_obj_type_class(type) && CS_TYPE_BINARY == cs_type;
}
inline bool ob_is_text(const ObObjType type, const ObCollationType cs_type)
{
  return ObTextTC == ob_obj_type_class(type) && CS_TYPE_BINARY != cs_type;
}
inline bool ob_is_null(const ObObjType type)
{
  return ObNullType == type;
}
inline bool ob_is_oracle_datetime_tc(ObObjType type)
{
  return ob_is_otimestampe_tc(type) || ob_is_datetime_tc(type);
}

// inline bool ob_is_enum_set_type(ObObjType type) {return (ObEnumType == type || ObSetType == type); }
inline bool ob_is_enumset_tc(ObObjType type)
{
  return ObEnumSetTC == ob_obj_type_class(type);
}
inline bool ob_is_enumset_inner_tc(ObObjType type)
{
  return ObEnumSetInnerTC == ob_obj_type_class(type);
}
inline bool ob_is_raw(const ObObjType type)
{
  return ObRawType == type;
}
inline bool ob_is_extend(const ObObjType type)
{
  return ObExtendType == type;
}
inline bool ob_is_urowid(const ObObjType type)
{
  return ObURowIDType == type;
}
inline bool ob_is_rowid_tc(const ObObjType type)
{
  return ob_is_urowid(type);
}
inline bool ob_is_accuracy_length_valid_tc(ObObjType type)
{
  return ob_is_string_type(type) || 
         ob_is_raw(type) || 
         ob_is_enumset_tc(type) || 
         ob_is_rowid_tc(type) ||
         ob_is_lob_tc(type) || 
         ob_is_json_tc(type);
}
inline bool ob_is_string_or_enumset_tc(ObObjType type)
{
  return ObStringTC == ob_obj_type_class(type) || ob_is_enumset_tc(type);
}
inline bool ob_is_large_text(ObObjType type)
{
  return ObTextType <= type && ObLongTextType >= type;
}
inline bool ob_is_datetime(const ObObjType type)
{
  return ObDateTimeType == type;
}
inline bool ob_is_timestamp_tz(const ObObjType type)
{
  return ObTimestampTZType == type;
}
inline bool ob_is_timestamp_ltz(const ObObjType type)
{
  return ObTimestampLTZType == type;
}
inline bool ob_is_timestamp_nano(const ObObjType type)
{
  return ObTimestampNanoType == type;
}
inline bool ob_is_interval_ym(const ObObjType type)
{
  return ObIntervalYMType == type;
}
inline bool ob_is_interval_ds(const ObObjType type)
{
  return ObIntervalDSType == type;
}
inline bool ob_is_nvarchar2(const ObObjType type)
{
  return ObNVarchar2Type == type;
}
inline bool ob_is_nchar(const ObObjType type)
{
  return ObNCharType == type;
}
inline bool ob_is_nstring_type(const ObObjType type)
{
  return ob_is_nchar(type) || ob_is_nvarchar2(type);
}
inline bool ob_is_json(const ObObjType type) 
{ 
  return ObJsonType == type; 
}
// to_string adapter
template <>
inline int databuff_print_obj(char* buf, const int64_t buf_len, int64_t& pos, const ObObjType& t)
{
  return databuff_printf(buf, buf_len, pos, "\"%s\"", ob_obj_type_str(t));
}
template <>
inline int databuff_print_key_obj(
    char* buf, const int64_t buf_len, int64_t& pos, const char* key, const bool with_comma, const ObObjType& t)
{
  return databuff_printf(buf, buf_len, pos, WITH_COMMA("%s:\"%s\""), key, ob_obj_type_str(t));
}

bool ob_can_static_cast(const ObObjType src, const ObObjType dst);
int find_type(const common::ObIArray<common::ObString>& type_infos, common::ObCollationType cs_type,
    const common::ObString& val, int32_t& pos);

enum ObOTimestampMetaAttrType { OTMAT_TIMESTAMP_TZ = 0, OTMAT_TIMESTAMP_LTZ, OTMAT_TIMESTAMP_NANO };
}  // end namespace common
}  // end namespace oceanbase

#endif  // OCEANBASE_COMMON_OB_OBJECT_TYPE_H_
