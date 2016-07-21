/*-------------------------------------------------------------------------*
 *                        Kaspersky antivirus engine                       *
 *-------------------------------------------------------------------------*/
/** @file
@brief       API to the engine tracer in product

@copyright   (C) Kaspersky Lab 2003, 2006, 2007
@date        10/2003
@date        03/2006
@date        10/2007

This file contain Trace Interface daclaration for KLAVA (C++ abstract class).

This is an interface which should be supplied to KLAVA by the client code.
This interface provide means for processing of trace messages. However, this
interface is not directly used by the engine and the links.

Definition of types and constants, used by this API.
*/
#ifndef IncludeFile_basework_trace_api_h
#define IncludeFile_basework_trace_api_h


/*-------------------------------------------------------------------------*/
#include <stdarg.h>
#include <klava/bases/klavcore.h>
#include <klava/trace.h>


/*-------------------------------------------------------------------------*/
typedef struct  KLAT_Scope_s            KLAT_Scope_t;
typedef struct  KLAT_Region_s           KLAT_Region_t;


/*-------------------------------------------------------------------------*/
/*              KLAT_Scope_s                                               */
/*-------------------------------------------------------------------------*/
struct KLAT_Scope_s
{
                enum KLAT_Scope_e                       Id;
                const char*                             Name;
};


/*-------------------------------------------------------------------------*/
/*              KLAT_Region_s                                              */
/*-------------------------------------------------------------------------*/
struct KLAT_Region_s
{
                const char*                             Name;
                const void*                             Ptr;
                size_t                                  Size;
                int                                     Direction;
                enum KLAT_DataType_e                    Type;
};


/*-------------------------------------------------------------------------*/
/*              IKLAT_Tracer                                               */
/*-------------------------------------------------------------------------*/
struct IKLAT_Tracer
{
public:
                enum Save_Op_e
                {
                    Save_begin,
                    Save_cont,
                    Save_end
                };

    virtual     void    KLAV_CALL                       Enter_Scope(const KLAT_Scope_t* AScope, int ACount);
    virtual     void    KLAV_CALL                       Leave_Scope(const KLAT_Scope_t* AScope, int ACount);

    virtual     void    KLAV_CALL                       Dump_Buffer(KLAT_Msgclass_t AMessage_Class, const KLAT_Region_t* ARegion);
    virtual     void    KLAV_CALL                       Dump_State(KLAT_Msgclass_t AMessage_Class);
    virtual     void    KLAV_CALL                       Message(KLAT_Msgclass_t AMessage_Class, const char* AMessage);
    virtual     bool    KLAV_CALL                       Save_Object(Save_Op_e AOperation, const void* ABuf, size_t ABufSize);

    virtual     bool    KLAV_CALL                       Set_Region(const KLAT_Region_t* ARegion);
    virtual     bool    KLAV_CALL                       Get_Region(const char* ARegion_Name, KLAT_Region_t* ARegion) const;

    virtual     void    KLAV_CALL                       Get_Ptr_Description(const void* APtr, KLAT_Ptr_Desc_t* ADescription) const;

    virtual     bool    KLAV_CALL                       VFormat(KLAT_Message_t* AResult, const char* AFormat, va_list Args) const;
    virtual     bool    KLAV_CALL                       Get_Description(KLAT_Message_t* AResult, KLAT_Descr_e AType, const void* AObject) const;
    virtual     void    KLAV_CALL                       Free_Message(KLAT_Message_t* AMessage) const;

    virtual     bool    KLAV_CALL                       Get_Msg_Mask(void* AMask_Dst, size_t AMask_Size) const;

    virtual     void*   KLAV_CALL                       Mem_Alloc(KLAT_Heap_t* AHeap, size_t ASize);
    virtual     void    KLAV_CALL                       Mem_Free(void* APtr);
};


/*-------------------------------------------------------------------------*/
#endif /* ifndef IncludeFile_basework_trace_api_h */



