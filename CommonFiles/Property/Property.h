#ifndef __property_h
#define __property_h

#include "../AVP_data.h"

#if defined( __cplusplus )
extern "C" {
#endif

  // ---
  typedef enum { 
    avpt_nothing = 1,
    avpt_char,
    avpt_wchar,
    avpt_short,
    avpt_long,
    avpt_byte,
    avpt_group,
    avpt_word,
    avpt_dword,
    avpt_bool,
    avpt_date,
    avpt_time,
    avpt_datetime,
		avpt_int,
		avpt_uint,
		avpt_qword,
		avpt_longlong,
		avpt_size_t,
    avpt_str  = 40,
    avpt_wstr,
    avpt_bin
  } AVP_TYPE;

  // ---
  typedef struct tagHDATA {
    AVP_byte _;
  } *HDATA;

  // ---
  typedef struct tagHPROP {
    AVP_byte _;
  } *HPROP;

#if defined (MACHINE_IS_BIG_ENDIAN)
  #define MAKE_AVP_PID( id, app, type, array )  ( ((0x3f&(AVP_dword)(type))) + ((0xff&(AVP_dword)(app))<<8) + ((0xffff&(AVP_dword)(id))<<16) + (((AVP_dword)(!!(array)))<<6) )
  #define GET_AVP_PID_ID( pid )                 ( (AVP_dword)(pid) >> 16 )
  #define GET_AVP_PID_APP( pid )                ( 0xff & (((AVP_dword)(pid)) >> 8) )
  #define GET_AVP_PID_TYPE( pid )               ( 0x3f & ((AVP_dword)(pid)) )
  #define GET_AVP_PID_ARR( pid )                ( !!(0x40&((AVP_dword)(pid))) )
  
#else

  #define MAKE_AVP_PID( id, app, type, array )  ( (((AVP_dword)(!!(array)))<<30) + ((0x3f&(AVP_dword)(type))<<24) + ((0xff&(AVP_dword)(app))<<16) + (0xffff&(AVP_dword)(id)) )
  #define GET_AVP_PID_ID( pid )                 ( (0xffff&((AVP_dword)(pid))) )
  #define GET_AVP_PID_APP( pid )                ( (0xff0000&((AVP_dword)(pid)))>>16 )
  #define GET_AVP_PID_TYPE( pid )               ( (0x3f000000&((AVP_dword)(pid)))>>24 )
  #define GET_AVP_PID_ARR( pid )                ( !!(0x40000000&((AVP_dword)(pid))) )

#endif /* defined (MACHINE_IS_BIG_ENDIAN) */

  #define GET_AVP_PID_ID_MAX_VALUE              ( GET_AVP_PID_ID(0xffffffff) )
  #define GET_AVP_PID_APP_MAX_VALUE             ( GET_AVP_PID_APP(0xffffffff) )
  #define GET_AVP_PID_TYPE_MAX_VALUE            ( GET_AVP_PID_TYPE(0xffffffff) )
  #define GET_AVP_PID_ARR_MAX_VALUE             ( GET_AVP_PID_ARR(0xffffffff) )

  #define AVP_AID_SYSTEM               0

  #define AVP_PID_NOTHING              MAKE_AVP_PID(0,AVP_AID_SYSTEM,avpt_nothing,0)
  #define AVP_PID_VALUE                AVP_PID_NOTHING 
  #define AVP_PID_ID                   AVP_PID_NOTHING 
  #define AVP_PID_END                  MAKE_AVP_PID(1,AVP_AID_SYSTEM,avpt_dword,0)
  #define AVP_PID_QMARK                MAKE_AVP_PID(1,AVP_AID_SYSTEM,avpt_group,0)

  #define AVP_PID_VERSION              MAKE_AVP_PID(20,AVP_AID_SYSTEM,avpt_dword,0)

  #define MAKE_ADDR1( name, id0 )                                               AVP_dword name[] = { id0, 0 };
  #define MAKE_ADDR2( name, id0, id1 )                                          AVP_dword name[] = { id0, id1, 0 };
  #define MAKE_ADDR3( name, id0, id1, id2 )                                     AVP_dword name[] = { id0, id1, id2, 0 };
  #define MAKE_ADDR4( name, id0, id1, id2, id3 )                                AVP_dword name[] = { id0, id1, id2, id3, 0 };
  #define MAKE_ADDR5( name, id0, id1, id2, id3, id4 )                           AVP_dword name[] = { id0, id1, id2, id3, id4, 0 };
  #define MAKE_ADDR6( name, id0, id1, id2, id3, id4, id5 )                      AVP_dword name[] = { id0, id1, id2, id3, id4, id5, 0 };
  #define MAKE_ADDR7( name, id0, id1, id2, id3, id4, id5, id6 )                 AVP_dword name[] = { id0, id1, id2, id3, id4, id5, id6, 0 };
  #define MAKE_ADDR8( name, id0, id1, id2, id3, id4, id5, id6, id7 )            AVP_dword name[] = { id0, id1, id2, id3, id4, id5, id6, id7, 0 };
  #define MAKE_ADDR9( name, id0, id1, id2, id3, id4, id5, id6, id7, id8 )       AVP_dword name[] = { id0, id1, id2, id3, id4, id5, id6, id7, id8, 0 };
  #define MAKE_ADDR10( name, id0, id1, id2, id3, id4, id5, id6, id7, id8, id9 ) AVP_dword name[] = { id0, id1, id2, id3, id4, id5, id6, id7, id8, id9, 0 };

  #define PROP_ARR_LAST   ( (AVP_dword)-1 )              

  typedef enum { 
    kmc_leave,  
    kmc_replace 
  } AVP_Knot_Merge_Code;

  typedef enum { 
    nmc_leave,  
    nmc_replace,  
    nmc_add, 
    nmc_sub, 
    nmc_mul, 
    nmc_div, 
    nmc_and, 
    nmc_or, 
    nmc_xor,
    nmc_min,
    nmc_max
  } AVP_Numeric_Merge_Code;

  typedef enum { 
    smc_leave,  
    smc_replace,  
    smc_add, 
    smc_sub 
  } AVP_String_Merge_Code;

  typedef enum { 
    bmc_leave,  
    bmc_replace,  
    bmc_and, 
    bmc_or, 
    bmc_xor 
  } AVP_Bool_Merge_Code;

  typedef enum { 
    dmc_leave,  
    dmc_replace,  
    dmc_add, 
    dmc_sub,
    dmc_min,
    dmc_max
  } AVP_Date_Merge_Code;

  typedef enum { 
    bnmc_leave, 
    bnmc_replace 
  } AVP_Binary_Merge_Code;

	#ifdef __MEGRE_FUNC__
		typedef struct {
			AVP_Numeric_Merge_Code  num_code  : 8;
			AVP_String_Merge_Code   str_code  : 6;
			AVP_Bool_Merge_Code     bool_code : 6;
			AVP_Date_Merge_Code     date_code : 6;
			AVP_Binary_Merge_Code   bin_code  : 2;
			//AVP_byte              reserved;
		} AVP_Merge_Code;
	#endif

  typedef DATA_PROC AVP_bool  (DATA_PARAM *Buffer_Proc)( void* buffer, AVP_dword size_in, AVP_dword* size_out, void* user_data );
  typedef DATA_PROC AVP_dword (DATA_PARAM *For_Each_Proc)( HDATA node, void* user_data );
  typedef DATA_PROC AVP_bool  (DATA_PARAM *First_That_Proc)( HDATA node, void* user_data );
	#ifdef __MEGRE_FUNC__
		typedef DATA_PROC AVP_bool  (DATA_PARAM *Merge_Proc)( HPROP prop1, HPROP prop2, void* user_data, AVP_Merge_Code* merge_code );
	#endif
	
  // ---
	#ifdef __MEGRE_FUNC__
		typedef struct {
			AVP_Knot_Merge_Code   data_code;
			AVP_Knot_Merge_Code   prop_code;
			AVP_Merge_Code        value_code;
			Merge_Proc            proc;
			void*                 user_data;
		} AVP_Merge_Param;
	#endif
		
  #define DATA_IF_CHILDREN_FIRST  0
  #define DATA_IF_SIBLINGS_FIRST  1
  #define DATA_IF_ROOT_EXCLUDE    2
  #define DATA_IF_ROOT_INCLUDE    0
  #define DATA_IF_ROOT_SIBLINGS   4
  #define DATA_IF_ROOT_CHILDREN   0
  #define DATA_IF_DOESNOT_GO_DOWN 8


  // ---
  typedef struct {
    AVP_dword   version;
    AVP_dword*  props_include;
    AVP_dword*  props_exclude;
		AVP_bool    root_siblings;
    Buffer_Proc proc;
    void*       user_data;
  } AVP_Serialize_Data;

  // these function MUST be called ONLY ONCE and will return FALSE on following calls
  // however, you can call DATA_Deinit_Library for cleaning up BUT be careful if you have an undesroyed data tree at the moment
  // because of unpredictable result on destroing tree by another memory system
  DATA_PROC   AVP_bool   DATA_PARAM   DATA_Init_Library   ( void* (*alloc)(AVP_size_t), void (*free)(void*) );
  DATA_PROC   AVP_bool   DATA_PARAM   DATA_Init_LibraryEx ( void* (*alloc)(AVP_size_t), void (*free)(void*), void (*lock)(void *), void (*unlock)(void *), void* context);
  DATA_PROC   AVP_bool   DATA_PARAM   DATA_Deinit_Library ( void );

#if !defined (MACHINE_IS_LITTLE_ENDIAN)
  DATA_PROC   AVP_bool   DATA_PARAM   DATA_Serialize      ( HDATA data, AVP_dword* addr, AVP_Serialize_Data* opts_in, void* buffer, AVP_dword size_in, AVP_dword* size_out );
#endif
  DATA_PROC   HDATA      DATA_PARAM   DATA_Deserialize    ( AVP_Serialize_Data* opts_out, void* buffer, AVP_dword size_in, AVP_dword* size_out );
                         
  DATA_PROC   AVP_dword* DATA_PARAM   DATA_Sequence       ( AVP_dword* seq, ... );
  DATA_PROC   AVP_dword* DATA_PARAM   DATA_Alloc_Sequence ( AVP_dword  id0, ... );
  DATA_PROC   AVP_dword* DATA_PARAM   DATA_Add_Sequence   ( AVP_dword* seq, ... );
  DATA_PROC   AVP_dword* DATA_PARAM   DATA_Make_Sequence  ( HDATA data, AVP_dword* addr, HDATA stop_data, AVP_dword** seq );
  DATA_PROC   AVP_bool   DATA_PARAM   DATA_Comp_Sequences ( AVP_dword* seq1, AVP_dword* seq2 );
  DATA_PROC   AVP_bool   DATA_PARAM   DATA_Remove_Sequence( AVP_dword* seq );

  DATA_PROC   HDATA      DATA_PARAM   DATA_Add            ( HDATA data, AVP_dword* addr, AVP_dword id, AVP_size_t val, AVP_dword size );
  DATA_PROC   HDATA      DATA_PARAM   DATA_Copy           ( HDATA data, AVP_dword* addr, HDATA copy, AVP_dword flags );
  DATA_PROC   HDATA      DATA_PARAM   DATA_Replace        ( HDATA data, AVP_dword* addr, AVP_dword id, AVP_size_t val, AVP_dword size );
  DATA_PROC   HDATA      DATA_PARAM   DATA_Attach         ( HDATA data, AVP_dword* addr, HDATA data2, AVP_dword flags );
  DATA_PROC   HDATA      DATA_PARAM   DATA_Insert         ( HDATA data, AVP_dword* addr, HDATA data2 );
  DATA_PROC   AVP_bool   DATA_PARAM   DATA_Multiplication ( HDATA data, AVP_dword* addr, HDATA data2, AVP_dword* addr2, HDATA* result, AVP_dword flags );
#ifdef __MEGRE_FUNC__
  DATA_PROC   AVP_bool   DATA_PARAM   DATA_Merge          ( HDATA data, AVP_dword* addr, HDATA data2, AVP_dword* addr2, HDATA* result, AVP_dword flags, AVP_Merge_Param* param );
#endif
  DATA_PROC   HDATA      DATA_PARAM   DATA_Detach         ( HDATA data, AVP_dword* addr );
  DATA_PROC   AVP_bool   DATA_PARAM   DATA_Remove         ( HDATA data, AVP_dword* addr );
  DATA_PROC   HDATA      DATA_PARAM   DATA_Find           ( HDATA data, AVP_dword* addr );
  DATA_PROC   AVP_dword  DATA_PARAM   DATA_Get_Id         ( HDATA data, AVP_dword* addr );
  DATA_PROC   HDATA      DATA_PARAM   DATA_Get_Dad        ( HDATA data, AVP_dword* addr );
  DATA_PROC   HDATA      DATA_PARAM   DATA_Get_FirstEx    ( HDATA data, AVP_dword* addr, AVP_dword flags );
  DATA_PROC   HDATA      DATA_PARAM   DATA_Get_LastEx     ( HDATA data, AVP_dword* addr, AVP_dword flags );
  DATA_PROC   HDATA      DATA_PARAM   DATA_Get_Next       ( HDATA data, AVP_dword* addr );
  DATA_PROC   HDATA      DATA_PARAM   DATA_Get_Prev       ( HDATA data, AVP_dword* addr );
  DATA_PROC   AVP_dword  DATA_PARAM   DATA_For_Each       ( HDATA data, AVP_dword* addr, AVP_dword flags, For_Each_Proc proc, void* user_data );
  DATA_PROC   HDATA      DATA_PARAM   DATA_First_That     ( HDATA data, AVP_dword* addr, AVP_dword flags, First_That_Proc proc, void* user_data );
  DATA_PROC   AVP_bool   DATA_PARAM   DATA_Replace_App_ID ( HDATA data, AVP_dword* addr, AVP_byte  app_id, AVP_dword flags );
  DATA_PROC   AVP_bool   DATA_PARAM   DATA_Replace_ID     ( HDATA data, AVP_dword* addr, AVP_word  id );
  DATA_PROC   AVP_bool   DATA_PARAM   DATA_Compare        ( HDATA data, AVP_dword* addr, HDATA data2, AVP_dword* addr2, AVP_dword flags );

  DATA_PROC   HPROP      DATA_PARAM   DATA_Add_Prop       ( HDATA data, AVP_dword* addr, AVP_dword pid, AVP_size_t val, AVP_dword size );
  DATA_PROC   HPROP      DATA_PARAM   DATA_Replace_Prop   ( HDATA data, AVP_dword* addr, AVP_dword from, AVP_dword to, AVP_size_t val, AVP_dword size );
  DATA_PROC   AVP_bool   DATA_PARAM   DATA_Remove_Prop_ID ( HDATA data, AVP_dword* addr, AVP_dword pid );
  DATA_PROC   AVP_bool   DATA_PARAM   DATA_Remove_Prop_H  ( HDATA data, AVP_dword* addr, HPROP     prop );
  DATA_PROC   HPROP      DATA_PARAM   DATA_Find_Prop      ( HDATA data, AVP_dword* addr, AVP_dword id );
  DATA_PROC   HPROP      DATA_PARAM   DATA_Get_First_Prop ( HDATA data, AVP_dword* addr );
  
  DATA_PROC   AVP_dword  DATA_PARAM   DATA_Set_Val        ( HDATA data, AVP_dword* addr, AVP_dword pid, AVP_size_t val, AVP_dword size );
  DATA_PROC   AVP_dword  DATA_PARAM   DATA_Get_Val        ( HDATA data, AVP_dword* addr, AVP_dword pid, void* val, AVP_dword size );
  DATA_PROC   AVP_TYPE   DATA_PARAM   DATA_Get_Type       ( HDATA data, AVP_dword* addr, AVP_dword pid );
                                                          
  DATA_PROC   HPROP      DATA_PARAM   PROP_Get_Next       ( HPROP prop );
  DATA_PROC   AVP_dword  DATA_PARAM   PROP_Set_Val        ( HPROP prop, AVP_size_t val, AVP_dword size );
  DATA_PROC   AVP_dword  DATA_PARAM   PROP_Get_Val        ( HPROP prop, void* val,      AVP_dword size );
  DATA_PROC   AVP_TYPE   DATA_PARAM   PROP_Get_Type       ( HPROP prop );
  DATA_PROC   AVP_dword  DATA_PARAM   PROP_Get_Id         ( HPROP prop );
  DATA_PROC   HDATA      DATA_PARAM   PROP_Get_Dad        ( HPROP prop );
  DATA_PROC   AVP_dword  DATA_PARAM   PROP_Get_SSize      ( HPROP prop );
  DATA_PROC   HPROP      DATA_PARAM   PROP_Replace				( HPROP prop, AVP_dword pid, AVP_size_t val, AVP_dword size );
  DATA_PROC   AVP_bool   DATA_PARAM   PROP_Replace_App_ID ( HPROP prop, AVP_byte  app_id );
  DATA_PROC   AVP_bool   DATA_PARAM   PROP_Replace_ID     ( HPROP prop, AVP_word  id );
  DATA_PROC   AVP_bool   DATA_PARAM   PROP_Compare        ( HPROP prop1, HPROP prop2 );

  #define PROP_DELTA_DONT_CHANGE 0
  DATA_PROC   AVP_dword  DATA_PARAM   PROP_Arr_Count      ( HPROP prop );
  DATA_PROC   AVP_word   DATA_PARAM   PROP_Arr_Delta      ( HPROP prop, AVP_word  new_del );
  DATA_PROC   AVP_dword  DATA_PARAM   PROP_Arr_Insert     ( HPROP prop, AVP_dword pos, void* from, AVP_dword count );
  DATA_PROC   AVP_dword  DATA_PARAM   PROP_Arr_Set_Items  ( HPROP prop, AVP_dword pos, void* from, AVP_dword count ); 
  DATA_PROC   AVP_dword  DATA_PARAM   PROP_Arr_Get_Items  ( HPROP prop, AVP_dword pos, void* to,   AVP_dword count ); 
  DATA_PROC   AVP_dword  DATA_PARAM   PROP_Arr_Get_SSize  ( HPROP data );
  DATA_PROC   AVP_dword  DATA_PARAM   PROP_Arr_Remove     ( HPROP prop, AVP_dword pos, AVP_dword count );


#define DATA_Get_First(d,a)  DATA_Get_FirstEx(d,a,DATA_IF_ROOT_CHILDREN)
#define DATA_Get_Last(d,a)   DATA_Get_LastEx(d,a,DATA_IF_ROOT_CHILDREN)

#if defined( __cplusplus )
}
#endif


#endif
