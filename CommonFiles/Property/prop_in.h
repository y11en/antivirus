#ifndef __prop_in_h
#define __prop_in_h

#include <byteorder.h>

#ifdef __cplusplus 
extern "C" {
#endif

#include <Property/Property.h>

#if !defined (__unix__)
	#include <crtdbg.h>
#endif
	
#include <AVPPort.h>

typedef AVP_word WORD;
typedef AVP_dword DWORD;


// ---
typedef struct {
  AVP_word          id;
  AVP_byte          app_id;

#if defined (MACHINE_IS_BIG_ENDIAN)
  AVP_byte          reserved    : 1;
  AVP_byte          array       : 1;
  AVP_byte          type        : 6;
#else
  AVP_byte          type        : 6;
  AVP_byte          array       : 1;
  AVP_byte          reserved    : 1;
#endif

} AVP_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_dword         count;
  AVP_dword         upper;
  AVP_word          delta;
  AVP_word          isize;
  void*             val;
} AVP_Arr_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_char          val;
} AVP_CHAR_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_wchar         val;
} AVP_WCHAR_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_short         val;
} AVP_SHORT_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_long          val;
} AVP_LONG_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_byte          val;
} AVP_BYTE_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_group         val;
} AVP_GROUP_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_word          val;
} AVP_WORD_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_dword         val;
} AVP_DWORD_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_qword         val;
} AVP_QWORD_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_longlong      val;
} AVP_LONGLONG_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_size_t        val;
} AVP_SIZE_T_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_int           val;
} AVP_INT_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_uint          val;
} AVP_UINT_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_bool          val;
} AVP_BOOL_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_date          val;
} AVP_DATE_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_time          val;
} AVP_TIME_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_datetime      val;
} AVP_DATETIME_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_str           val;
} AVP_STR_Property;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_wstr          val;
} AVP_WSTR_Property;


// ---
typedef struct {
  AVP_dword         size;
  void*             data;
} AVP_Bin_Item;


// ---
typedef struct {
  AVP_Property      prop;
  AVP_Bin_Item      val;
} AVP_BIN_Property;


// ---
typedef struct s_AVP_Linked_Property AVP_Linked_Property;
struct s_AVP_Linked_Property {
  AVP_Linked_Property*   next;
  AVP_Property           data;
};

// ---
typedef struct s_AVP_Data AVP_Data;
struct s_AVP_Data {
  AVP_Data*            dad;
  AVP_Data*            child;
  AVP_Data*            next;
  AVP_Linked_Property  value;
};


// ---
typedef struct {
  AVP_byte*   buffer;
  AVP_dword   rsize;
  AVP_dword   bsize;
  AVP_dword   tsize;
  AVP_dword   csize;
  AVP_dword   crc;
  AVP_dword   crc_len;
  AVP_dword   crc_cluster;
	AVP_bool    crc_not_in_use;
	AVP_bool    buff_loaded;
  Buffer_Proc proc;
  void*       data;
} Serialize;

#define CRC_CLUSTER (0x400-sizeof(AVP_dword))

#define AVP_NEXT_END        (0xff)
#define DATA_HANDLE(a)      ((HDATA)(a))
#define PROP_HANDLE(a)      ((HPROP)(a))
#define PROP_ID_MASK        ( 0x7fffffffl )
#define PROP_ID( pid )      ( (AVP_dword)((*((AVP_dword*)(pid))) & PROP_ID_MASK) )

#define FIND_DATA(data,addr)                    \
  if ( addr )                                   \
    data = DATA_Find( data, addr );             \
  if ( !data ) {                                \
    _RPT0( _CRT_ASSERT, "Cannot find data" );   \
    return 0;                                   \
  }

#define FIND_DATA_EX(data,node,addr)            \
  if ( addr )                                   \
    data = (AVP_Data*)DATA_Find( node, addr );  \
  else                                          \
    data = (AVP_Data*)node;                     \
  if ( !data ) {                                \
    _RPT0( _CRT_ASSERT, "Cannot find data" );   \
    return 0;                                   \
  }


extern void* (* allocator)(AVP_size_t);
extern void  (* liberator)(void*);

void             DATA_PARAM  DATA_Free( AVP_Data* data );

AVP_dword        DATA_PARAM  PROP_Init( AVP_Property* prop, AVP_dword pid, AVP_size_t val, AVP_dword size );
AVP_dword        DATA_PARAM  PROP_Predict_Size( AVP_dword prop_id );
AVP_Property*    DATA_PARAM  PROP_Copy( AVP_Property* dst, AVP_Property* src );
void             DATA_PARAM  PROP_Multiplication( AVP_Data* d1, AVP_Data* d2 );
void             DATA_PARAM  PROP_Free( AVP_Property* prop );

AVP_bool         DATA_PARAM  PROP_Arr_Init( AVP_Arr_Property* prop );
AVP_bool         DATA_PARAM  PROP_Arr_Free( AVP_Arr_Property* prop );
AVP_bool         DATA_PARAM  PROP_Arr_Catch_Mem( HPROP prop, AVP_dword count );
AVP_dword        DATA_PARAM  CountCRC32WithCRC( AVP_dword Size, AVP_byte* Buffer, AVP_dword oldcrc32 );
void * 			 DATA_PARAM  DATA_Get_Property_Item_Addr (AVP_Property* prop);


#define PUT_VAL_PROP      ( 1 )
#define PUT_CHILD_DATA    ( 2 )
#define PUT_NEXT_DATA     ( 4 )

#pragma pack(1)

typedef struct
{
	AVP_word	wYear; 
	AVP_word	wMonth; 
	AVP_word	wDayOfWeek; 
	AVP_word	wDay;

} AVP_struct_date;


typedef struct
{
	AVP_word	wHour; 
	AVP_word	wMinute; 
	AVP_word	wSecond; 
	AVP_word	wMilliseconds; 

} AVP_struct_time;


typedef struct
{
	AVP_word	wYear; 
	AVP_word	wMonth; 
	AVP_word	wDayOfWeek; 
	AVP_word	wDay;
	AVP_word	wHour; 
	AVP_word	wMinute; 
	AVP_word	wSecond; 
	AVP_word	wMilliseconds; 

} AVP_struct_datetime;

#pragma pack()

#ifdef __cplusplus 
}
#endif

#endif
