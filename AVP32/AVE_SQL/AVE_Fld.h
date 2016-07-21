#ifndef __AVE_FLD
#define __AVE_FLD

#include "ave_sql.h"


#define FIELD_FULL_NAME           (  1  | FIELD_TYPE_STRING)
#define FIELD_NAME                (  2  | FIELD_TYPE_STRING)
#define FIELD_TYPE                (  3  | FIELD_TYPE_DWORD)
#define FIELD_SUB_TYPE            (  4  | FIELD_TYPE_DWORD)
#define FIELD_AVE_NAME            (  5  | FIELD_TYPE_STRING)

#define FIELD_LENGTH_1            ( 10  | FIELD_TYPE_DWORD)
#define FIELD_OFFSET_1            ( 11  | FIELD_TYPE_DWORD)
#define FIELD_SUM_1               ( 12  | FIELD_TYPE_DWORD)
#define FIELD_SIGNATURE_1         ( 13  | FIELD_TYPE_BINARY)

#define FIELD_LENGTH_2            ( 20  | FIELD_TYPE_DWORD)
#define FIELD_OFFSET_2            ( 21  | FIELD_TYPE_DWORD)
#define FIELD_SUM_2               ( 22  | FIELD_TYPE_DWORD)
#define FIELD_SIGNATURE_2         ( 23  | FIELD_TYPE_BINARY)

#define FIELD_LINK_OBJ            ( 30  | FIELD_TYPE_BINARY)
#define FIELD_LINK_ENTRY          ( 31  | FIELD_TYPE_DWORD)
#define FIELD_LINK_SOURCE_NAME    ( 32  | FIELD_TYPE_STRING)
#define FIELD_LINK_EXPORTS        ( 33  | FIELD_TYPE_STRING)
#define FIELD_LINK_SRC_FULLNAME   ( 34  | FIELD_TYPE_STRING)

#define FIELD_F_CURE_METHOD       ( 40  | FIELD_TYPE_DWORD)
#define FIELD_F_CURE_DATA_1       ( 41  | FIELD_TYPE_DWORD)
#define FIELD_F_CURE_DATA_2       ( 42  | FIELD_TYPE_DWORD)
#define FIELD_F_CURE_DATA_3       ( 43  | FIELD_TYPE_DWORD)
#define FIELD_F_CURE_DATA_4       ( 44  | FIELD_TYPE_DWORD)
#define FIELD_F_CURE_DATA_5       ( 45  | FIELD_TYPE_DWORD)

#define FIELD_S_CURE_OBJECT_1     ( 50  | FIELD_TYPE_DWORD)
#define FIELD_S_CURE_METHOD_1     ( 51  | FIELD_TYPE_DWORD)
#define FIELD_S_CURE_ADDRESS_11   ( 52  | FIELD_TYPE_DWORD)
#define FIELD_S_CURE_ADDRESS_12   ( 53  | FIELD_TYPE_DWORD)
#define FIELD_S_CURE_DATA_1       ( 54  | FIELD_TYPE_DWORD)

#define FIELD_S_CURE_OBJECT_2     ( 60  | FIELD_TYPE_DWORD)
#define FIELD_S_CURE_METHOD_2     ( 61  | FIELD_TYPE_DWORD)
#define FIELD_S_CURE_ADDRESS_21   ( 62  | FIELD_TYPE_DWORD)
#define FIELD_S_CURE_ADDRESS_22   ( 63  | FIELD_TYPE_DWORD)
#define FIELD_S_CURE_DATA_2       ( 64  | FIELD_TYPE_DWORD)

#define FIELD_J_BITMASK_1         ( 70  | FIELD_TYPE_DWORD)
#define FIELD_J_BITMASK_2         ( 71  | FIELD_TYPE_DWORD)
#define FIELD_J_METHOD            ( 72  | FIELD_TYPE_DWORD)
#define FIELD_J_DATA_1            ( 73  | FIELD_TYPE_DWORD)
#define FIELD_J_DATA_2            ( 74  | FIELD_TYPE_DWORD)
#define FIELD_J_DATA_3            ( 75  | FIELD_TYPE_DWORD)

#define FIELD_M_METHOD            ( 80  | FIELD_TYPE_DWORD)
#define FIELD_M_OFFSET            ( 81  | FIELD_TYPE_DWORD)
#define FIELD_M_SEGMENT           ( 82  | FIELD_TYPE_DWORD)
#define FIELD_M_LENGTH            ( 83  | FIELD_TYPE_DWORD)
#define FIELD_M_SUM               ( 84  | FIELD_TYPE_DWORD)
#define FIELD_M_CURE_OFFSET       ( 85  | FIELD_TYPE_DWORD)
#define FIELD_M_CURE_LENGTH       ( 86  | FIELD_TYPE_DWORD)
#define FIELD_M_CURE_DATA         ( 87  | FIELD_TYPE_BINARY)

#define FIELD_COMMENT             (100  | FIELD_TYPE_STRING)
#define FIELD_DETECT_FILENAME     (101  | FIELD_TYPE_STRING)

#endif

