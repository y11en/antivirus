#ifndef __NAG_REGPROTECTION_H__
#define __NAG_REGPROTECTION_H__

//! Non-zero if anti-virus application is installed
#define KLNAG_REGPRT_AVINSTALLED    L"Protection_AvInstalled"   // REG_DWORD

//! Non-zero if anti-virus application is installed and running
#define KLNAG_REGPRT_AVRUNNING      L"Protection_AvRunning"     // REG_DWORD

//! State of RTP (AppRtpState)
#define KLNAG_REGPRT_RTPSTATE       L"Protection_RtpState"      // REG_DWORD

//! has rtp state
#define KLNAG_REGPRT_HASRTP         L"Protection_HasRtp"        // REG_DWORD

//! Time (as time_t) of last full scan 
#define KLNAG_REGPRT_LASTFSCAN      L"Protection_LastFscan"     // REG_SZ

//! Time (as time_t) of anti-virus bases
#define KLNAG_REGPRT_BASESDATE      L"Protection_BasesDate"     // REG_SZ

//! Time of last connection to the administration server
#define KLNAG_REGPRT_LASTCONNECTED  L"Protection_LastConnected" // REG_SZ


/*!
  \brief	Returns registry key name

            Usage:
                KLSTD::klwstr_t wstrKeyName;
                KLNAGREGPRT_GetKeyName(wstrKeyName.outref());                
                ... wstrKeyName.c_str() ...
                ...........................

  \param	wstrKeyName OUT name of registry relative to HKEY_LOCAL_MACHINE
*/
KLCSNAGT_DECL void KLNAGREGPRT_GetKeyName(KLSTD::AKWSTR& wstrKeyName);

#endif //__NAG_REGPROTECTION_H__
