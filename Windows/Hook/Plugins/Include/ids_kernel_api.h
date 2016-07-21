#ifndef _IDS_KERNEL_API_H_
#define _IDS_KERNEL_API_H_

#pragma warning( disable : 4200 )

#define IDS_KERNEL_PLUGINNAME   "IDSKernel"

// ioctl message ids
#define IDS_MSGID_SETPARMS      0x00000002
#define IDS_MSGID_GETPARM       0x00000003
#define IDS_MSGID_GETPARMCNT    0x00000004
#define IDS_MSGID_LOAD          0x00000005
#define IDS_MSGID_

struct IDSKERNELDB
{
    ULONG   DBSize;
    ULONG   FragmentCount;
    BYTE    Fragment[0];
};

struct IDSKERNELDBFRAGMENT
{
    CHAR    SectionName[32];
    ULONG   FragmentSize;
    BYTE    FragmentData[0];
};

struct IDSKERNELLOADRET
{
    UINT    Status;
};

struct IDSKERNELSETPARMS
{
    ULONG   VerdictID;
    ULONG   ParmNum;
    ULONG   ParmValue;
};

struct IDSKERNELSETPARMSRET
{
    UINT    Status;
};

struct IDSKERNELGETPARM
{
    ULONG   VerdictID;
    ULONG   ParmNum;
};

struct IDSKERNELGETPARMRET
{
    UINT    Status;
    ULONG   ParmValue;
};

struct IDSKERNELGETPARMCNT
{
    ULONG   VerdictID;
};

struct IDSKERNELGETPARMCNTRET
{
    UINT    Status;
    ULONG   ParmCount;
};

#endif //_IDS_KERNEL_API_H_