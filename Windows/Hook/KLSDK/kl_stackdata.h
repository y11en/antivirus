#ifndef _KL_STACK_DATA_H_
#define _KL_STACK_DATA_H_

#include "kl_buffer.h"

class CKl_StackData : public CKl_Buffer
{
public:
    CKl_StackData() : CKl_Buffer(NULL,0) {};
    virtual ~CKl_StackData() {};

    virtual ULONG PushData(PVOID  srcBuf, ULONG Size) = 0;  // Помещает данные в объект
    virtual ULONG PopData (PVOID  dstBuf, ULONG Size) = 0;  // забирает Size байт из объекта
};

class CKl_Fifo : public CKl_StackData
{
public:
    CKl_Fifo() {};
    virtual ~CKl_Fifo() {};

    virtual ULONG PushData(PVOID  srcBuf, ULONG Size);
    virtual ULONG PopData (PVOID  dstBuf, ULONG Size);
};


#endif
