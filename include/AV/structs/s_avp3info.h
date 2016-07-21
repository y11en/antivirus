#ifndef __S_AVP3INFO_H__
#define __S_AVP3INFO_H__

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/pr_serdescriptor.h>
#include <Prague/pr_serializable.h>
#include <Prague/iface/i_root.h>

#include <AV/plugin/p_avp3info.h>

//--------------------------- cAVP3FileInfo -------------------------

struct cAVP3FileInfo : public cSerializable
{
    cAVP3FileInfo()
       : m_dwViruseCount(0) 
    {
       memset(m_dtFileDate, 0, sizeof(m_dtFileDate)) ;
    }

    DECLARE_IID(cAVP3FileInfo, cSerializable, PID_AVP3INFO, 1) ;

    tDWORD m_dwViruseCount ;
    tDT    m_dtFileDate ;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAVP3FileInfo, "AVP3FileInfo")
	SER_VALUE( m_dwViruseCount, tid_DWORD,    "ViruseCount" )
   SER_VALUE( m_dtFileDate,    tid_DATETIME, "FileDate" )
IMPLEMENT_SERIALIZABLE_END();

//--------------------------- cAVP3BasesInfo ------------------------

struct cAVP3BasesInfo : public cSerializable
{
    cAVP3BasesInfo()
       : m_dwViruseCount(0) 
    {
       memset(m_dtBasesDate, 0, sizeof(m_dtBasesDate)) ;
    }

    DECLARE_IID(cAVP3BasesInfo, cSerializable, PID_AVP3INFO, 2) ;

    tDWORD m_dwViruseCount ;
    tDT    m_dtBasesDate ;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAVP3BasesInfo, "AVP3FileInfo")
	SER_VALUE( m_dwViruseCount, tid_DWORD,    "ViruseCount" )
   SER_VALUE( m_dtBasesDate,   tid_DATETIME, "BasesDate" )
IMPLEMENT_SERIALIZABLE_END();

//-------------------------------------------------------------------

#endif //  __S_AVP3INFO_H__
