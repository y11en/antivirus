/*-------------------------------------------------------------------------*
 *                      Kaspersky antivirus engine v3                      *
 *-------------------------------------------------------------------------*/
/** @file
@brief       Trace macros for creating a log of engine execution

@copyright   (C) Kaspersky Lab 2003, 2006, 2007
@date        10/2003
@date        03/2006
@date        10/2007
*/
#ifndef IncludeFile_links_klava_trace_h
#define IncludeFile_links_klava_trace_h


/* KLAT stands for "KLAva Tracer" */


/*-------------------------------------------------------------------------*/
/* Two definitions below should actually be placed in a configuration header  */
#define         KLAT_ENABLE_TRACE       1
#define         KLAT_ENABLE_TRACE_ASM   0


/*-------------------------------------------------------------------------*/
#include <stdarg.h>


/*-------------------------------------------------------------------------*/
struct IKLAT_Tracer;
typedef struct  KLAT_Tracer_s           KLAT_Tracer_t;
typedef struct  KLAT_Message_Obj_s      KLAT_Message_Obj_t;
typedef struct  KLAT_Scope_Obj_s        KLAT_Scope_Obj_t;
typedef struct  KLAT_Ptr_Desc_s         KLAT_Ptr_Desc_t;
typedef struct  KLAT_Message_s          KLAT_Message_t;
typedef struct  KLAT_Heap_s             KLAT_Heap_t;


/*-------------------------------------------------------------------------*/
enum KLAT_Scope_e
{
    KTS_NONE            =  0,
    KTS_FUNC            =  1,
    KTS_LINK            =  2,
    KTS_MODULE          =  3,
    KTS_OBJECT          =  4,
    KTS_TASK            =  5,

    KTS_COUNT
};

/*-------------------------------------------------------------------------*/
enum KLAT_DataType_e
{
    KDT_HexBuf          =  1,
    KDT_LE_int          =  2,
    KDT_BE_int          =  3,
    KDT_Native_int      =  4
};

/*-------------------------------------------------------------------------*/
enum KLAT_Msgclass_e
{
    KTF_Always          =  0,
    KTF_Engine_Calls    =  1,
    KTF_IO_Dump         =  2,
    KTF_Engine_State    =  3,
    KTF_Link_Calls      =  4,
    KTF_Scan_Matching   =  5,
    KTF_Asm_Trace       =  6,
    KTF_Call_Graph      =  7,
    KTF_Why_Error       =  8,
    KTF_Hierarchy       =  9,

    KTF_COUNT
};
typedef enum    KLAT_Msgclass_e         KLAT_Msgclass_t;


/*-------------------------------------------------------------------------*/
enum KLAT_Descr_e
{
    KDS_Pointer         =  1,
    KDS_Scope           =  2,
    KDS_AVP3_Signature  =  3,
    KDS_AVP3_Link       =  4,
    KDS_AVP3_Record     =  5
};


/*-------------------------------------------------------------------------*/
/*              KLAT_Ptr_Desc_s                                            */
/*-------------------------------------------------------------------------*/
struct KLAT_Ptr_Desc_s
{
                const char*                             Name;
                size_t                                  Abs_Offset;
                int                                     Offset_Sign;
                char                                    Sign_Char;
};


/*-------------------------------------------------------------------------*/
/*              KLAT_Message_s                                             */
/*-------------------------------------------------------------------------*/
struct KLAT_Message_s
{
                char*                                   Buffer;
                char*                                   Dyn_Buffer;
                size_t                                  Buffer_Size;
                size_t                                  Msg_Len;
};




enum { KTF_MASK_SIZE = ((KTF_COUNT + (sizeof(unsigned int)*8) - 1) % (sizeof(unsigned int)*8)) };

/*-------------------------------------------------------------------------*/
/*              KLAT_Tracer_s                                              */
/*-------------------------------------------------------------------------*/
struct KLAT_Tracer_s
{
                struct IKLAT_Tracer*                    Tracer;
                KLAT_Heap_t*                            Heap;
                KLAT_Message_Obj_t*                     Tmp_Str;
                unsigned int                            Msg_Class_Mask[KTF_MASK_SIZE];
};

/*-------------------------------------------------------------------------*/
#if defined(__cplusplus)

/*-------------------------------------------------------------------------*/
/*              KLAT_Message_Obj_s                                         */
/*-------------------------------------------------------------------------*/
struct KLAT_Message_Obj_s
{
 public:
                explicit                                KLAT_Message_Obj_s(KLAT_Tracer_t* ATracer);
                                                       ~KLAT_Message_Obj_s();

                void                                    clear();
                void                                    vformat(const char* AFormat, va_list Args);
                void                                    format(const char* AFormat, ...);
  inline        const char*                             c_str() const { return Message.Buffer; }

 private:
                KLAT_Message_t                          Message;
                KLAT_Tracer_t*                          Tracer;

                /* Disable copy and assignment */
                                                        KLAT_Message_Obj_s(const KLAT_Message_Obj_s&);
                void                                    operator=(const KLAT_Message_Obj_s&);
};


/*-------------------------------------------------------------------------*/
/*              KLAT_Scope_Obj_s                                           */
/*-------------------------------------------------------------------------*/
struct KLAT_Scope_Obj_s
{
 public:
                                                        KLAT_Scope_Obj_s(KLAT_Tracer_t* ATracer, KLAT_Scope_e AScope_1, const char* AName_1);
                                                        KLAT_Scope_Obj_s(KLAT_Tracer_t* ATracer, KLAT_Scope_e AScope_1, const char* AName_1, KLAT_Scope_e AScope_2, const char* AName_2);
                                                       ~KLAT_Scope_Obj_s();

 private:
                KLAT_Tracer_t*                          Tracer;
                KLAT_Scope_e                            Scope_1;
                KLAT_Scope_e                            Scope_2;
                int                                     Num_Scopes;

                /* Disable copy and assignment */
                                                        KLAT_Scope_Obj_s(const KLAT_Scope_Obj_s&);
                                                        void operator=(const KLAT_Scope_Obj_s&);
};

/*-------------------------------------------------------------------------*/
#endif /* if defined(__cplusplus) */


/*-------------------------------------------------------------------------*/
#if defined(__cplusplus)
extern "C" {
#endif

/*-------------------------------------------------------------------------*/
struct CAVPCtxIo;

KLAT_Tracer_t*  KLAT_Tracer_Create   (KLAT_Heap_t* AHeap, struct IKLAT_Tracer* ATracer); /* Returns NULL if ATracer is NULL */
void            KLAT_Tracer_Destroy  (KLAT_Tracer_t* ATracer);
void            KLAT_Register_Buffer (KLAT_Tracer_t* ATracer, const char* ABufName, const void* ABuf, size_t ABufSize, int ADir, KLAT_DataType_e ABufType);
void            KLAT_On_Object_Start (KLAT_Tracer_t* ATracer, struct CAVPCtxIo* AHandle);

void            KLAT_Dump_String     (KLAT_Tracer_t* ATracer, KLAT_Msgclass_t AClass, const char* AMessage);
void            KLAT_Dump_Format     (KLAT_Tracer_t* ATracer, KLAT_Msgclass_t AClass, const char* AFormat, ...);
void            KLAT_Dump_State      (KLAT_Tracer_t* ATracer, KLAT_Msgclass_t AClass);
void            KLAT_Dump_Buffer     (KLAT_Tracer_t* ATracer, KLAT_Msgclass_t AClass, const char* ABufName, const void* AData, size_t ASize);

void            KLAT_Get_Ptr_Desc    (KLAT_Tracer_t* ATracer, const void* APtr, KLAT_Ptr_Desc_t* ADesc);
int             KLAT_Is_Active       (KLAT_Tracer_t* ATracer, KLAT_Msgclass_t AClass);

KLAT_Message_Obj_t*  KLAT_Get_Temp_String(KLAT_Tracer_t* ATracer);
void                 KLAT_On_Asm_Step(); /* Must be called from ASM code only. It assume special context value is in ebp */


/*-------------------------------------------------------------------------*/
#if defined(__cplusplus)
}
#endif


/*-------------------------------------------------------------------------*/
#if !defined(KLAT_ENABLE_TRACE)
#  error Your configuration files should have defined KLAT_ENABLE_TRACE
#endif /* if !defined(KLAT_ENABLE_TRACE) */

/*-------------------------------------------------------------------------*/
#if KLAT_ENABLE_TRACE

#  define       KLAT_SCOPE_(Tracer, Id_1, Name_1)                              KLAT_Scope_Obj_t Dummy_bs93vxl3gs02((Tracer), (Id_1), (Name_1));
#  define       KLAT_SCOPE2_(Tracer, Id_1, Name_1, Id_2, Name_2)               KLAT_Scope_Obj_t Dummy_d3e0e9wb4jbd((Tracer), (Id_1), (Name_1), (Id_2), (Name_2));

#  define       KLAT_DUMP_STATE_(Tracer, Class, Format)                        do { if (KLAT_Is_Active((Tracer), (Class))) { KLAT_Message_Obj_t* String_82gduy28wgslk2 = KLAT_Get_Temp_String((Tracer)); String_82gduy28wgslk2->format Format; KLAT_Dump_String((Tracer), (Class), String_82gduy28wgslk2->c_str()); KLAT_Dump_State((Tracer), (Class)); }; } while (0)
#  define       KLAT_MESSAGE_(Tracer, Class, Format)                           do { if (KLAT_Is_Active((Tracer), (Class))) { KLAT_Message_Obj_t* String_nbdi239df3k5ud = KLAT_Get_Temp_String((Tracer)); String_nbdi239df3k5ud->format Format; KLAT_Dump_String((Tracer), (Class), String_nbdi239df3k5ud->c_str()); }; } while (0)
#  define       KLAT_DUMP_BUFFER_(Tracer, Class, BufName, Buf, BufSize)        do { KLAT_Dump_Buffer((Tracer), (Class), (BufName), (Buf), (BufSize)); } while (0)
#  define       KLAT_REGISTER_BUFFER_(Tracer, BufName, Buf, BufSize, Dir, BufType)  do { KLAT_Register_Buffer((Tracer), (BufName), (Buf), (BufSize), (Dir), (BufType)); } while (0)
#  define       KLAT_GET_PTR_DESCRIPTION_(Tracer, Ptr, Desc)                   do { KLAT_Get_Ptr_Desc((Tracer), (Ptr), (Desc)); } while (0)
#  define       KLAT_IF_TRACE_(Tracer)                                         if (!(Tracer)) {} else
#  define       KLAT_TRACE_DO(Statement)                                       Statement

#else

#  define       KLAT_SCOPE_(Tracer, Id_1, Name_1)
#  define       KLAT_SCOPE2_(Tracer, Id_1, Name_1, Id_2, Name_2)

#  define       KLAT_DUMP_STATE_(Tracer, Class, Format)                        do { } while (0)
#  define       KLAT_MESSAGE_(Tracer, Class, Format)                           do { } while (0)
#  define       KLAT_DUMP_BUFFER_(Tracer, Class, BufName, Buf, BufSize)        do { } while (0)
#  define       KLAT_REGISTER_BUFFER_(Tracer, BufName, Buf, BufSize, Dir, BufType)  do { } while (0)
#  define       KLAT_GET_PTR_DESCRIPTION_(Tracer, Ptr, Desc)                   do { } while (0)
#  define       KLAT_IF_TRACE_(Tracer)                                         if (1) {} else
#  define       KLAT_TRACE_DO(Statement)

#endif /* if KLAT_ENABLE_TRACE */

/*-------------------------------------------------------------------------*/
#define         KLAT_DEFAULT_TRACER                                     (AVP3_CTX->m_Tracer)

#define         KLAT_SCOPE(Id_1, Name_1)                                KLAT_SCOPE_(KLAT_DEFAULT_TRACER,  Id_1, Name_1)
#define         KLAT_SCOPE2(Id_1, Name_1, Id_2, Name_2)                 KLAT_SCOPE2_(KLAT_DEFAULT_TRACER, Id_1, Name_1, Id_2, Name_2)

#define         KLAT_DUMP_STATE(Format)                                 KLAT_DUMP_STATE_(KLAT_DEFAULT_TRACER, KTF_ENGINESTATE, Format)
#define         KLAT_MESSAGE(Class, Format)                             KLAT_MESSAGE_(KLAT_DEFAULT_TRACER, Class, Format)
#define         KLAT_DUMP_BUFFER(Class, BufName, Buf, BufSize)          KLAT_DUMP_BUFFER_(KLAT_DEFAULT_TRACER, Class, BufName, Buf, BufSize)
#define         KLAT_REGISTER_BUFFER(BufName, Buf, BufSize, Dir, BufType) KLAT_REGISTER_BUFFER_(KLAT_DEFAULT_TRACER, BufName, Buf, BufSize, Dir, BufType)

#define         KLAT_GET_PTR_DESCRIPTION(Ptr, Desc)                     KLAT_GET_PTR_DESCRIPTION_(KLAT_DEFAULT_TRACER, Ptr, Desc)
#define         KLAT_IF_TRACE                                           KLAT_IF_TRACE_(KLAT_DEFAULT_TRACER)


/*-------------------------------------------------------------------------*/
#endif /* ifndef IncludeFile_links_klava_trace_h */


