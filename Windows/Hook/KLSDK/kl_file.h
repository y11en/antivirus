#ifndef _KL_FILE_H_
#define _KL_FILE_H_

#include "kldef.h"
#include "klstatus.h"
#include "kl_object.h"

#define FILE_STATE_OPENED   0x1
#define FILE_STATE_CLOSED   0x2
#define FILE_STATE_READ     0x3
#define FILE_STATE_WRITE    0x4


class CKl_File  : public CKl_Object 
{
    void*       m_Handle;
public:
    CKl_File(char*  filename = NULL);
    virtual ~CKl_File();

    char*       m_CurrentFile;
    wchar_t*    m_CurrentFileW;
    ULONG       m_ReadOffset;
    ULONG       m_WriteOffset;
    ULONG       m_FileState;

    KLSTATUS    Open (char* filename = NULL);
    KLSTATUS    Open (wchar_t* filename);
    KLSTATUS    Close();
    KLSTATUS    Read (char* DstBuffer, ULONG Offset, ULONG BytesToRead,  ULONG* BytesRead    );
    KLSTATUS    Write(char* SrcBuffer, ULONG Offset, ULONG BytesToWrite, ULONG* BytesWritten );        
    KLSTATUS    Seek(ULONG  Offset);
    ULONG       GetSize();
};

#endif