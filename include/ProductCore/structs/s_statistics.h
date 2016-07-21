//
// s_statistics.h
//

#ifndef __S_STATISTICS_H__
#define __S_STATISTICS_H__



#include <ProductCore/plugin/p_klstatistics.h>
#include <ProductCore/structs/s_taskmanager.h>



struct cKLStatisticsSettings : public cTaskSettings
{
    cKLStatisticsSettings()
    {
        memset(m_dtMaxPeriod, 0, sizeof(tDATETIME)) ;
    }

    DECLARE_IID_SETTINGS(cKLStatisticsSettings, cTaskSettings, PID_KLSTATISTICS) ;

    cStrObj             m_strConfigFile ;
    cStrObj             m_strRegion ;
    tDATETIME           m_dtMaxPeriod ;
    cVector<cStrObj>    m_aComponent ;
    cStrObj             m_strUserID ;
    cStrObj             m_strDataFolder ;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cKLStatisticsSettings, 0)
    SER_BASE  ( cTaskSettings,   0                             )
    SER_VALUE ( m_strConfigFile, tid_STRING_OBJ, "config_file" )
    SER_VALUE ( m_strRegion,     tid_STRING_OBJ, "region"      )
    SER_VALUE ( m_dtMaxPeriod,   tid_DATETIME,   "max_period"  )
    SER_VECTOR( m_aComponent,    tid_STRING_OBJ, "components"  )
    SER_VALUE ( m_strUserID,     tid_STRING_OBJ, "user_id"     )
    SER_VALUE ( m_strDataFolder, tid_STRING_OBJ, "data_folder" )
IMPLEMENT_SERIALIZABLE_END();


struct cStatisticsInfo : cSerializable
{
    DECLARE_IID(cStatisticsInfo, cSerializable, PID_KLSTATISTICS, 1) ;

    tDATETIME   m_dtWhen ;
    cStrObj     m_strMD5 ;
    tDWORD      m_dwSize ;
    cStrObj     m_strVerdict ;
    cStrObj     m_strFileName ;
    tDWORD      m_dwLocalPort ;
    tDWORD      m_dwProto ;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cStatisticsInfo, 0)
    SER_VALUE( m_dtWhen,       tid_DATETIME,   "when"       )
    SER_VALUE( m_strMD5,       tid_STRING_OBJ, "md5"        )
    SER_VALUE( m_dwSize,       tid_DWORD,      "size"       )
    SER_VALUE( m_strVerdict,   tid_STRING_OBJ, "verdict"    )
    SER_VALUE( m_strFileName,  tid_STRING_OBJ, "file_name"  )
    SER_VALUE( m_dwLocalPort,  tid_DWORD,      "local_port" )
    SER_VALUE( m_dwProto,      tid_DWORD,      "proto" )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------

#endif//  __S_STATISTICS_H__
