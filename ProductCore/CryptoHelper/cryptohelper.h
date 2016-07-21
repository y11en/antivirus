// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  09 November 2005,  18:06 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Pavlushchik
// File Name   -- cryptohelper.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __cryptohelper_cpp__37f2e1d8_6592_43c9_8444_fb5cf3d32a46 )
#define __cryptohelper_cpp__37f2e1d8_6592_43c9_8444_fb5cf3d32a46
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <ProductCore/plugin/p_cryptohelper.h>
// AVP Prague stamp end

#include <Prague/iface\i_csect.h>
#include <windows.h>
#include "verifytrust.h"

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CCryptoHelper : public cCryptoHelper 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call Encode( tDWORD* p_result, const tBYTE* p_pDecodedData, tDWORD p_nDataSize, tBYTE* p_pEncodedBuffer, tDWORD p_nBufferSize );
	tERROR pr_call Decode( tDWORD* p_result, const tBYTE* p_pEncodedData, tDWORD p_nDataSize, tBYTE* p_pDecodedBuffer, tDWORD p_nBufferSize );
	tERROR pr_call CryptPassword( cSerString* p_password, tDWORD p_flags );
	tERROR pr_call IsMSTrusted( const tWCHAR* p_file_name );
	tERROR pr_call StoreData( tDWORD p_data_id, tPTR p_data, tDWORD p_size );
	tERROR pr_call GetData( tDWORD p_data_id, tPTR p_data, tDWORD* p_size );
	tERROR pr_call GetOldData( tDWORD p_data_id, tPTR p_data, tDWORD* p_size );

// Data declaration
	tCHAR m_szVersion[100]; // --
// AVP Prague stamp end

	tERROR pr_call CryptPasswordEx(cSerString* p_password, tDWORD p_flags);
	CMSDataStore* GetStore(tDWORD p_data_id);

	cCriticalSection* m_lock;
    HANDLE            m_hToken;
	tCHAR             m_sKeyContainer[100];
	tCHAR             m_sStorageName[100];
	tCHAR             m_sPersistentStorageName[100]; 
	tDWORD            m_dwVersion;

	CMSVerifyTrust*   m_msVerifier;
	tDWORD            m_tmVerifyLastRequest;

	CMSDataStore*     m_msStore;
	CMSDataStore*     m_msPersistentStore;
	tDWORD            m_tmStoreLastRequest;
	
	tDWORD            m_nOsVersion;

	CHelperDataStore* m_helperStore;
	BOOL			  m_bIsMSStoreRead;
	BOOL			  m_bIsMSPersistentStoreRead;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CCryptoHelper)
};
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // cryptohelper_cpp
// AVP Prague stamp end

