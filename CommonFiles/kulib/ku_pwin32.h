


#define KU_IS_BIG_ENDIAN 0


typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;

#ifdef _MSC_VER
    typedef signed __int64 int64_t;
    typedef unsigned __int64 uint64_t;
#else
    typedef signed long long int64_t;
    typedef unsigned long long uint64_t;
#endif


#define INT8_MIN (-128)
#define INT16_MIN (-32768)
#define INT32_MIN (-2147483647 - 1)
#define INT64_MIN (-9223372036854775807i64 - 1)  /* (-9223372036854775807LL - 1LL) */

#define INT8_MAX (127)
#define INT16_MAX (32767)
#define INT32_MAX (2147483647)
#define INT64_MAX 9223372036854775807i64 /* (9223372036854775807LL) */

#define UINT8_MAX (255)
#define UINT16_MAX (65535)
#define UINT32_MAX (4294967295UL)
#define UINT64_MAX 0xffffffffffffffffui64 /* (18446744073709551615ULL) */
