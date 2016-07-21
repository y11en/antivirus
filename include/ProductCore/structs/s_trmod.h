#ifndef __S_TRMOD_H
#define __S_TRMOD_H

#include <Prague/pr_time.h>
#include <Prague/pr_vector.h>
#include <ProductCore/structs/s_ip.h>
#include <ProductCore/structs/s_report.h>
#include <ProductCore/structs/s_taskmanager.h>
#include <AV/structs/s_avs.h>
#include <AV/iface/i_avs.h>

#define PID_PROCESSMONITOR  ((tPID)(49035)) // 0x0000BF8B

#define pmc_PRODUCT 0x2937151d //

	#define pm_PRODUCT_STATE_CHANGED 0xc1d5efa5 // (3252023205) --
	#define pm_PRODUCT_STATISTICS_CHANGED 0xd51f059b // (3575580059) --

#define pmc_PRODUCT_REQUEST 0x776c7d0b //
	#define pm_PRODUCT_REQUEST_TRUSTEDAPPS 0xaf7f7d63	//
	#define pm_PRODUCT_REQUEST_APPHASH  0x9d378a56		//
	#define pm_PRODUCT_TRUSTEDAPPS_CHANGED 0xbaa5cc18	// (3131427864)


//+ ------------------------------------------------------------------------------------------
enum _ePM_IID
{
	pmst_cBLTrustedApp = 0x80,   // the numbers 0-0x7f are already used in ppp\include\structs\s_settings.h
	pmst_cBLTrustedApps,
	pmst_cModuleInfo,
	pmst_cModuleInfoHash,
	pmst_cModuleInfoEx,
	pmst_cModuleInfoKLSRL,
	pmst_cProcessInfo,
	pmst_cProcessPidInfo,
	pmst_cProcessModuleInfo,
	pmst_cFileHashMD5,
	pmst_cSecurityRating,
	pmst_cCalcSecurityRatingProgress,
	pmst_cModuleInfoSignature,
	pmst_cFileHashSHA1,
	pmst_cFileHashSHA1Norm,
	pmst_cModuleInfoByAppId,
	pmst_cProcMonNotifyEx,
};

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

struct cBLTrustedApp : public cSerializable
{
	enum Triggers
	{
		fExclNone      = 0x00,
		fExclOpenFiles = 0x01,
		fExclNet       = 0x02,
		fExclNetHost   = 0x04,
		fExclNetPort   = 0x08,
		fExclBehavior  = 0x10,
		fExclRegistry  = 0x20,
	};
	
	cBLTrustedApp() :
		m_bEnabled(cTRUE),
		m_nHost_(0),
		m_nPort_(0),
		m_nTriggers(fExclOpenFiles),
		m_Hash(0),
		m_SSLOnly(cFALSE)
	{}
	
	DECLARE_IID( cBLTrustedApp, cSerializable, PID_ANY, pmst_cBLTrustedApp );

    void pr_call finalizeStreamDeser( tERROR err, tUINT field_num, const cSerDescriptor* dsc )
    {
        if (field_num == 7) // pre-сIP version
        {
            if (m_nHost_)
                m_Hosts.push_back(cIP(m_nHost_));
            if (m_nPort_)
                m_Ports.push_back(m_nPort_);
        }
    }

    void pr_call finalizeRegDeser( tERROR err, tUINT field_num, const cSerDescriptor* dsc )
    {
        if (field_num == 7) // pre-сIP version
        {
            if (m_nHost_)
                m_Hosts.push_back(cIP(m_nHost_));
            if (m_nPort_)
                m_Ports.push_back(m_nPort_);
        }
    }

	bool IsExclOpenFiles() { return m_bEnabled && (m_nTriggers & fExclOpenFiles); }
	bool IsExclNet()       { return m_bEnabled && (m_nTriggers & fExclNet); }
	bool IsExclBehavior()  { return m_bEnabled && (m_nTriggers & fExclBehavior); }
	bool IsExclRegistry()  { return m_bEnabled && (m_nTriggers & fExclRegistry); }
	
	tBOOL      m_bEnabled;
	cStringObj m_sImagePath;   // Полное имя файла
	tDWORD     m_nHost_;       // deprecated
	tWORD      m_nPort_;       // deprecated
	tDWORD     m_nTriggers;    // Triggers...
	tQWORD     m_Hash;
	tBOOL      m_SSLOnly;
    cVector<cIP>	m_Hosts;   // IP-адреса сервера
    cVector<tWORD>  m_Ports;   // порты сервера
};

IMPLEMENT_SERIALIZABLE_BEGIN( cBLTrustedApp, 0 )
	SER_VALUE_M( bEnabled,            tid_BOOL )
	SER_VALUE_M( sImagePath,          tid_STRING_OBJ )
	SER_VALUE_M( nHost_,              tid_DWORD )
	SER_VALUE_M( nPort_,              tid_WORD )
	SER_VALUE_M( nTriggers,           tid_DWORD )
	SER_VALUE_M( Hash,                tid_QWORD )
	SER_VALUE_M( SSLOnly,             tid_BOOL )
    SER_VECTOR_M( Hosts,              cIP::eIID )
    SER_VECTOR_M( Ports,              tid_WORD )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cModuleInfo : public cSerializable
{
	cModuleInfo() :
//		m_nPid(0),
		m_tCreation(-1),
		m_tModify(-1),
		m_nObjectSize(0)
	{}

	DECLARE_IID( cModuleInfo, cSerializable, PID_ANY, pmst_cModuleInfo );

//	tQWORD          m_nPid;
	cStringObj		m_sImagePath;
	cStringObj		m_sProduct;
	cStringObj		m_sVersion;
	cStringObj		m_sVendor;
	cStringObj		m_sDescription;
	time_t			m_tCreation;
	time_t			m_tModify;
	tQWORD			m_nObjectSize;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cModuleInfo, 0 )
//	SER_VALUE_M( nPid,                  tid_QWORD )
	SER_VALUE_M( sImagePath,            tid_STRING_OBJ )
	SER_VALUE_M( sProduct,              tid_STRING_OBJ )
	SER_VALUE_M( sVersion,              tid_STRING_OBJ )
	SER_VALUE_M( sVendor,               tid_STRING_OBJ )
	SER_VALUE_M( sDescription,          tid_STRING_OBJ )
	SER_VALUE_M( tCreation,             tid_DWORD )
	SER_VALUE_M( tModify,               tid_DWORD )
	SER_VALUE_M( nObjectSize,           tid_QWORD )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cModuleInfoHash : public cModuleInfo
{
	cModuleInfoHash() :
		m_nHash(0)
	{}

	DECLARE_IID( cModuleInfoHash, cModuleInfo, PID_ANY, pmst_cModuleInfoHash );

	tQWORD          m_nHash;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cModuleInfoHash, 0 )
	SER_BASE(cModuleInfo, 0)
	SER_VALUE_M( nHash,                 tid_QWORD )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------
/*
// flags for nRequestInfoFlags and nFilledInfoFlags
#define MIF_VERSION_INFO   0x0001
#define MIF_HASH_CACHE     0x0004
#define MIF_HASH_CALC      0x0008
#define MIF_HASH           (MIF_HASH_CACHE|MIF_HASH_CALC)
#define MIF_KLSRL_CACHE    0x0010
#define MIF_KLSRL_QUERY    0x0020
#define MIF_KLSRL          (MIF_KLSRL_CACHE|MIF_KLSRL_QUERY)
#define MIF_UNIQ_PID       0x0040

#define MIF_ALL            cMAX_LONGLONG
#define MIF_DEFAULT        (MIF_VERSION_INFO \
							|MIF_UNIQ_PID \
							|MIF_KLSRL_CACHE \
							|MIF_HASH \
							)

struct cModuleInfoEx : public cSerializable
{
	cModuleInfoEx() :
	m_nRequestInfoFlags(0), // MIF_xxx
	m_nFilledInfoFlags(0),  // MIF_xxx
	m_nQueryTimeout(30000)  // milliseconds
	{
	}
	
	DECLARE_IID( cModuleInfoEx, cSerializable, PID_ANY, pmst_cModuleInfoEx );

	// input info
	tQWORD           m_nPid;        // native or uniq process id
	cStringObj		 m_sImagePath;  // name of image file (if empty - will be resolved from pid)
	tQWORD           m_nRequestInfoFlags; // MIF_xxx flags
	tQWORD           m_nFilledInfoFlags;  // MIF_xxx flags
	tDWORD           m_nQueryTimeout;     // query timeout for online queries
	
	// output info
	cModuleInfo      m_ModuleInfo;      // module version info
	cModuleInfoKLSRL m_ModuleInfoKLSRL; // KLSRL info
	cVector<tBYTE>   m_HashMD5;         // file MD5 hash
	tObjectId        m_nImageFileId;    // id for sImagePath
};

IMPLEMENT_SERIALIZABLE_BEGIN( cModuleInfoEx, 0 )
	SER_VALUE_M( nPid,                        tid_QWORD )
	SER_VALUE_M( sImagePath,                  tid_STRING_OBJ )
	SER_VALUE_M( nRequestInfoFlags,           tid_QWORD )
	SER_VALUE_M( nFilledInfoFlags,            tid_QWORD )
	SER_VALUE_M( nQueryTimeout,               tid_DWORD )
	SER_VALUE_M( ModuleInfo,                  cModuleInfo::eIID )
	SER_VALUE_M( ModuleInfoKLSRL,             cModuleInfoKLSRL::eIID )
	SER_VECTOR_M( HashMD5,                    tid_BYTE )
IMPLEMENT_SERIALIZABLE_END( );
*/
//-----------------------------------------------------------------------------

#define cUNKNOWN_FILE_REVISION cMAX_UINT

//-----------------------------------------------------------------------------

struct cFileHashMD5 : public cSerializable
{
	cFileHashMD5()
	{
	}
	
	DECLARE_IID( cFileHashMD5, cSerializable, PID_PROCESSMONITOR, pmst_cFileHashMD5 );

	tBYTE            m_HashMD5[16];
};

IMPLEMENT_SERIALIZABLE_BEGIN( cFileHashMD5, 0 )
	SER_VALUE_M( HashMD5,    tid_BINARY )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cFileHashSHA1 : public cSerializable
{
	cFileHashSHA1()
	{
	}
	DECLARE_IID( cFileHashSHA1, cSerializable, PID_PROCESSMONITOR, pmst_cFileHashSHA1 );

	tBYTE            m_HashSHA1[20];
};

IMPLEMENT_SERIALIZABLE_BEGIN( cFileHashSHA1, 0 )
	SER_VALUE_M( HashSHA1,    tid_BINARY )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cFileHashSHA1Norm : public cSerializable
{
	cFileHashSHA1Norm()
	{
	}
	DECLARE_IID( cFileHashSHA1Norm, cSerializable, PID_PROCESSMONITOR, pmst_cFileHashSHA1Norm );

	tBYTE            m_HashSHA1Norm[20];
};

IMPLEMENT_SERIALIZABLE_BEGIN( cFileHashSHA1Norm, 0 )
	SER_VALUE_M( HashSHA1Norm,    tid_BINARY )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------


struct cModuleInfoKLSRL : public cFileHashMD5
{
	cModuleInfoKLSRL() : 
	cFileHashMD5(),
	m_nMajorVersion(0),
	m_errStatus(errOBJECT_NOT_INITIALIZED)
	{
		memset(&m_dtQueryTime, 0, sizeof(m_dtQueryTime));
	}

	DECLARE_IID( cModuleInfoKLSRL, cFileHashMD5, PID_PROCESSMONITOR, pmst_cModuleInfoKLSRL );

	cStringObj		m_sVendor;
	cStringObj		m_sProduct;
	tDWORD          m_nMajorVersion;
	tDATETIME       m_dtQueryTime;
	// Trusted Flags: total 160 bits
	// bits 1-32 - HIPS privileges
	cVector<tBYTE>  m_vTrustedFlags;
	tERROR          m_errStatus;

	bool IsHipsPriviledgeDisabled(
		tDWORD nPriviledge // value in range 1-32
		)
	{
		if (nPriviledge < 1 || nPriviledge > 32)
			return false;
		size_t i = nPriviledge / 8; // byte number
		if (i >= m_vTrustedFlags.size())
			return false;
		size_t b = nPriviledge % 8; // bit number in byte
		return !!(m_vTrustedFlags[i] & b);
	}

	bool IsTrustedFlags(tQWORD nFlags)
	{
		if (!nFlags)
			return false;
		size_t i = 4; // first 4 bytes reserved for HIPS privileges
		while (i<m_vTrustedFlags.size())
		{
			tBYTE nByteFlags = (tBYTE)nFlags;
			if ((m_vTrustedFlags[i] & nByteFlags) != nByteFlags)
				return false;
			nFlags >>= 8;
			if (!nFlags)
				return true;
			i++;
		}
		return false;
	}
};

IMPLEMENT_SERIALIZABLE_BEGIN( cModuleInfoKLSRL, 0 )
	SER_BASE( cFileHashMD5, 0 )
	SER_VALUE_M( sVendor,               tid_STRING_OBJ )
	SER_VALUE_M( sProduct,              tid_STRING_OBJ )
	SER_VALUE_M( nMajorVersion,         tid_DWORD )
	SER_VALUE_M( dtQueryTime,           tid_DATETIME )
	SER_VECTOR_M( vTrustedFlags,        tid_BYTE )
	SER_VALUE_M( errStatus,             tid_ERROR )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------


struct cModuleInfoSignature : public cSerializable
{
	cModuleInfoSignature() : 
	m_nTrustFlags(0),
	m_nWVTResult(0x800B0100L) // TRUST_E_NOSIGNATURE
	{
		memset(&m_dtSignedTime, 0, sizeof(m_dtSignedTime));
	}

	DECLARE_IID( cModuleInfoSignature, cSerializable, PID_PROCESSMONITOR, pmst_cModuleInfoSignature );

#define TF_SIGNED     0x001 
#define TF_REVOKED    0x002 // CERT_E_REVOKED
#define TF_TRUSTED    0x004
#define TF_TRUSTED2   (0x008 | TF_TRUSTED) // explicitly signed (MS, KL)
#define TF_EXPIRED    0x010 // CERT_E_EXPIRED
#define TF_FAILURE    0x020 // TRUST_E_FAIL, TRUST_E_PROVIDER_UNKNOWN, etc
#define TF_UNTRUSTED  0x040 // CERT_E_UNTRUSTEDROOT, TRUST_E_SUBJECT_NOT_TRUSTED, etc
#define TF_INVALID    0x080 // CERT_E_MALFORMED, CERT_E_INVALID_NAME, etc

	bool IsTrusted() // signature is trusted
	{
		return !!(m_nTrustFlags & TF_TRUSTED);
	}

	bool IsSigned() // object is signed
	{
		return !!(m_nTrustFlags & TF_SIGNED);
	}

	tDWORD          m_nTrustFlags;
	tDATETIME       m_dtSignedTime;
	cStringObj		m_sSignerCommonName;
	cStringObj		m_sSignerOrganization;
	tLONG           m_nWVTResult;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cModuleInfoSignature, 0 )
	SER_VALUE_M( nTrustFlags,         tid_DWORD)
	SER_VALUE_M( dtSignedTime,        tid_DATETIME )
	SER_VALUE_M( sSignerCommonName,   tid_STRING_OBJ )
	SER_VALUE_M( sSignerOrganization, tid_STRING_OBJ )
	SER_VALUE_M( nWVTResult,          tid_LONG )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cProcessPidInfo : public cSerializable
{
	cProcessPidInfo() : 
	m_NativePid(0),
	m_ParentNativePid(0),
	m_UniqPid(0),
	m_ParentUniqPid(0),
	m_AppId(0)
	{
	}
	DECLARE_IID( cProcessPidInfo, cSerializable, PID_PROCESSMONITOR, pmst_cProcessPidInfo );

	// output
	tDWORD       m_NativePid;
	tDWORD       m_ParentNativePid;
	tQWORD       m_UniqPid;
	tQWORD       m_ParentUniqPid;
	tDWORD       m_AppId;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cProcessPidInfo, 0 )
	SER_VALUE_M( NativePid,           tid_DWORD )
	SER_VALUE_M( ParentNativePid,     tid_DWORD )
	SER_VALUE_M( UniqPid,             tid_QWORD )
	SER_VALUE_M( ParentUniqPid,       tid_QWORD )
	SER_VALUE_M( AppId,               tid_DWORD )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cProcessInfo : public cProcessPidInfo
{
	cProcessInfo() : 
	cProcessPidInfo(),
	m_ImageFileID(0),
	m_StartEventID(0),
	m_ExitEventID(0),
	m_UserID(0)
	{
	}
	DECLARE_IID( cProcessInfo, cProcessPidInfo, PID_PROCESSMONITOR, pmst_cProcessInfo );

	// input pid - native or uniq pid
	//tQWORD       ProcessPidInfo.m_nPid;

	// output
	cStringObj   m_sImageFile;
	cStringObj   m_sCmdLine;
	tQWORD       m_ImageFileID;
	cDateTime    m_dtStartTime;
	tQWORD       m_StartEventID;
	cDateTime    m_dtExitTime;
	tQWORD       m_ExitEventID;
	tQWORD       m_UserID;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cProcessInfo, 0 )
	SER_BASE( cProcessPidInfo, 0 )
	SER_VALUE_M( sImageFile,          tid_STRING_OBJ )
	SER_VALUE_M( sCmdLine,            tid_STRING_OBJ )
	SER_VALUE_M( ImageFileID,         tid_QWORD )
	SER_VALUE_M( dtStartTime,         tid_DATETIME )
	SER_VALUE_M( StartEventID,        tid_QWORD )
	SER_VALUE_M( dtExitTime,          tid_DATETIME )
	SER_VALUE_M( ExitEventID,         tid_QWORD )
	SER_VALUE_M( UserID,              tid_QWORD )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cProcessModuleInfo : public cProcessInfo
{
	DECLARE_IID( cProcessModuleInfo, cProcessInfo, PID_PROCESSMONITOR, pmst_cProcessModuleInfo );

	cModuleInfo  m_Module;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cProcessModuleInfo, 0 )
	SER_BASE( cProcessInfo, 0 )
	SER_VALUE_M( Module,          cModuleInfo::eIID )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cModuleInfoByAppId : public cModuleInfo
{
	DECLARE_IID( cModuleInfoByAppId, cModuleInfo, PID_PROCESSMONITOR, pmst_cModuleInfoByAppId );

	tQWORD  m_AppId;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cModuleInfoByAppId, 0 )
	SER_BASE( cModuleInfo, 0 )
	SER_VALUE_M( AppId,          tid_QWORD )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cSecurityRating : public cSerializable
{
	cSecurityRating() : 
	m_ScanDetected(cFALSE),
	m_ScanError(errOBJECT_NOT_INITIALIZED)
	{
	}
	DECLARE_IID( cSecurityRating, cSerializable, PID_PROCESSMONITOR, pmst_cSecurityRating );

	// output
	cModuleInfoKLSRL      m_KlsrlInfo;

	cProtectionStatistics m_ScanStat;
	tBOOL                 m_ScanDetected;
	tERROR                m_ScanError;

	tDWORD                m_EmulRating;

	cModuleInfoSignature  m_Signature;
	cModuleInfo           m_ModuleInfo;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cSecurityRating, 0 )
	SER_VALUE_M( KlsrlInfo,           cModuleInfoKLSRL::eIID )
	SER_VALUE_M( ScanStat,            cProtectionStatistics::eIID )
	SER_VALUE_M( ScanDetected,        tid_BOOL )
	SER_VALUE_M( ScanError,           tid_ERROR )
	SER_VALUE_M( EmulRating,          tid_DWORD )
	SER_VALUE_M( Signature,           cModuleInfoSignature::eIID )
	SER_VALUE_M( ModuleInfo,          cModuleInfo::eIID )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cCalcSecurityRatingProgress : public cTaskAskAction
{
	cCalcSecurityRatingProgress() : 
	cTaskAskAction(),
	m_nProgress(0),
	m_pContext(NULL)
	{
	}
	DECLARE_IID( cCalcSecurityRatingProgress, cTaskAskAction, PID_PROCESSMONITOR, pmst_cCalcSecurityRatingProgress );

	tDWORD       m_nProgress; // 0-100
	tPTR         m_pContext;  // internal ProcMon context

	enum eAction
	{
		eShowNotification = 1,
		eUpdataNotification,
		eCloseNotification,
		eCancelNotification,
	};
};

IMPLEMENT_SERIALIZABLE_BEGIN( cCalcSecurityRatingProgress, 0 )
	SER_BASE( cTaskAskAction, 0 )
	SER_VALUE_M( nProgress,    tid_DWORD )
	SER_VALUE_M( pContext,     tid_PTR )
	
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

#endif//  __S_TRMOD_H
