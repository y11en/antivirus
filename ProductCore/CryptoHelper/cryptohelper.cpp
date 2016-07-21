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

// AVP Prague stamp begin( Interface version,  )
#define CryptoHelper_VERSION ((tVERSION)1)
// AVP Prague stamp end

// AVP Prague stamp begin( Includes for interface,  )
#include "cryptohelper.h"
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <ProductCore/plugin/p_cryptohelper.h>
// AVP Prague stamp end

#include <windows.h>
#include <wincrypt.h>
#include <malloc.h>
#include ".\str_enc.h"

#include <Prague/pr_cpp.h>
#include <Prague/pr_remote.h>
#include "..\prague\core\remote\pr_process.h"
#include <Prague/iface/e_loader.h>
#include <ProductCore/iface/i_taskmanager.h>
#include "ver_mod.h"

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "crypt32.lib")

static WTSApi g_WTSApi;

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "CryptoHelper". Static(shared) property table variables
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CCryptoHelper::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CryptoHelper::ObjectInitDone method" );

	// Place your code here
    m_hToken = g_WTSApi.GetSystemToken();

	tMsgHandlerDescr hdls[] =
	{
		{ *this, rmhLISTENER, pmcTM_EVENTS, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
	};
	error = g_root->sysRegisterMsgHandlerList(hdls, countof(hdls));

	if( PR_SUCC(error) )
		error = sysCreateObjectQuick((hOBJECT*)&m_lock, IID_CRITICAL_SECTION);

	cStrObj strProduct("%ProductType%");
	if( errOK_DECIDED != sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strProduct), 0, 0) )
		strProduct = L"AVP";

	cStringObj strVersion = "%ProductVersion%";
	if( errOK_DECIDED != sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strVersion), 0, 0) )
		strVersion = VER_PRODUCTVERSION_STR;

	tDWORD v0 = VER_PRODUCTVERSION_HIGH, v1, v3;
	m_dwVersion = VER_PRODUCTVERSION_BUILD;

	sscanf((tSTRING)strVersion.c_str(cCP_ANSI), "%d.%d.%d.%d", &v0, &v1, &m_dwVersion, &v3);
	sprintf(m_sKeyContainer, "%s6", (char*)strProduct.c_str(cCP_ANSI));
	sprintf(m_sStorageName, "%s%d", (char*)strProduct.c_str(cCP_ANSI), v0);
	sprintf(m_sPersistentStorageName, "%s_pers", (char*)strProduct.c_str(cCP_ANSI));
	
	m_nOsVersion = GetVersion();

	cStrObj strHelperStoreFileName("%Data%\\perfm.dat");
	if( errOK_DECIDED != sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strHelperStoreFileName), 0, 0) )
		strHelperStoreFileName = "perfm.dat";
	m_helperStore = new CHelperDataStore(strHelperStoreFileName);
	m_helperStore->Load();

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR CCryptoHelper::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CryptoHelper::ObjectPreClose method" );
	
	// Place your code here
	if( m_hToken )
		CloseHandle(m_hToken);
	
	if( m_msVerifier )
		delete m_msVerifier;

	if( m_msStore )
		delete m_msStore;

	if( m_msPersistentStore )
		delete m_msPersistentStore;

	if( m_helperStore )
	{
		m_helperStore->Save();
		delete m_helperStore;
	}

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
//   "msg_cls_id"    : Message class identifier
//   "msg_id"        : Message identifier
//   "send_point"    : Object where the message was send initially
//   "ctx"           : Context of the object processing
//   "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
//   "par_buf"       : Buffer of the parameters
//   "par_buf_len"   : Lenght of the buffer of the parameters
tERROR CCryptoHelper::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CryptoHelper::MsgReceive method" );

	// Place your code here
	if( p_msg_cls_id == pmcTM_EVENTS && p_msg_id == pmTM_EVENT_IDLE )
	{
		cAutoCS cs(m_lock, true);
		tDWORD tmNow = PrGetTickCount();

		if( m_msVerifier && tmNow > m_tmVerifyLastRequest && tmNow - m_tmVerifyLastRequest > 10000 )
			delete m_msVerifier, m_msVerifier = NULL;

		if( m_msStore && tmNow > m_tmStoreLastRequest && tmNow - m_tmStoreLastRequest > 10000 )
			delete m_msStore, m_msStore = NULL;

		if( m_msPersistentStore && tmNow > m_tmStoreLastRequest && tmNow - m_tmStoreLastRequest > 10000 )
			delete m_msPersistentStore, m_msPersistentStore = NULL;
	}

	return error;
}
// AVP Prague stamp end

// BUG FIX: Проблема с криптопровайдером от Инфотекса:
// Кажется нам удалось локализовать проблему, но пока не понятно, как ее решить.
// У вас вызывается функция CryptAcquireContext( &hProv, NULL, NULL, 1, CRYPT_VERIFYCONTEXT).
// Поскольку явно имя желаемого провайдера не указано, то мы на самом деле открываем 2 (свой
// и RSA). Это необходимо для обеспечения проверки ГОСТовских сертификатов. Обычно таким
// образом открывают провайдер на проверку подписи и проблем не возникает.
// Дальше все еще интереснее.
// 1. Функция CryptGetUserKey естественно возвращает ошибку, но не NTE_NO_KEY, а NTE_NOT_FOUND.
// Это наша ошибка, но если бы ее не было, то тогда функция CryptGenKey "подвесила" сервис,
// т.к. у нас на генерацию ключа запускается окно интерактивного биологического ДСЧ.
// 2. При коде ошибки, отличном от NTE_NO_KEY вы пытаетесь экспортировать открытый ключ,
// вызывая CryptExportPublickKeyInfo. Эта функция "обламывается" и возвращает мусор. Код
// ошибки у вас не анализируется и полученный неверный блоб идет на функцию кодирования
// CertInfo. Asn кодировщик неправильно определяет размер данных (те самые 500М) и дальше
// эта выделенная память нигде не освобождается.
// 
// Мое предложение:
// Проще всего исправить у вас. Достаточно явно указать имя криптопровайдера, добавив
// 1 строчку кода:
// CryptGetDefaultProvider( ... 1, ..., szProviderName )
// CryptAcquireContext( &hProv, NULL, szProviderName , 1, CRYPT_VERIFYCONTEXT).
// В этом случае мы не будем вмешиваться в работу функции и ошибка пропадет.
// С нашей стороны наверное тоже можно поправить, но это существенно сложнее, так как
// повлияет на работу других приложений, осуществляющих проверку подписи как для ГОСТ
// так и для RSA сертификатов.
cStrObj GetDefaultProvider()
{
	cStrObj res;
	HMODULE hAdvapi = GetModuleHandle("advapi32");
	typedef BOOL (WINAPI *pfnCryptGetDefaultProviderA)(DWORD,DWORD*,DWORD,LPSTR,DWORD*);
	pfnCryptGetDefaultProviderA _CryptGetDefaultProviderA =
		(pfnCryptGetDefaultProviderA) GetProcAddress(hAdvapi, "CryptGetDefaultProviderA");
	if (!_CryptGetDefaultProviderA)
		return res;
	
	char buf[1024];
	DWORD size = sizeof(buf);
	if (_CryptGetDefaultProviderA(PROV_RSA_FULL, NULL, CRYPT_MACHINE_DEFAULT, buf, &size))
		res = buf;
	return res;
}

// AVP Prague stamp begin( External (user called) interface method implementation, Encode )
// Parameters are:
tERROR CCryptoHelper::Encode( tDWORD* p_result, const tBYTE* p_pDecodedData, tDWORD p_nDataSize, tBYTE* p_pEncodedBuffer, tDWORD p_nBufferSize )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CryptoHelper::Encode method" );

	// Place your code here
	cAutoCS cs(m_lock, true);
	CAutoToken auto_token(m_hToken);

	HCRYPTPROV hCryptProv = NULL;
	HCRYPTKEY hKey = NULL, hExchangeKey = NULL;
	PBYTE pbKeyBlob = NULL;
	DWORD dwBlobSize = 0, dwKeyBlobLength = 0, dwEncryptedBufferLength = 0;         // -- it's length

	// Get the handle to the provider.
	cStrObj prov = GetDefaultProvider();
	CryptAcquireContext(&hCryptProv, m_sKeyContainer, prov.empty() ? NULL : (LPCSTR)prov.c_str(cCP_ANSI), PROV_RSA_FULL, 0);

	if( !hCryptProv )
		CryptAcquireContext(&hCryptProv, m_sKeyContainer, prov.empty() ? NULL : (LPCSTR)prov.c_str(cCP_ANSI), PROV_RSA_FULL, CRYPT_NEWKEYSET);

	if( !hCryptProv )
	{
        PR_TRACE((this, prtERROR, "crypt\tError 0x%x during CryptAcquireContext", GetLastError()));
		return errNOT_OK;
	}

	if( !CryptGetUserKey(hCryptProv, AT_KEYEXCHANGE, &hExchangeKey) )
	{
		hExchangeKey = NULL;
		if( GetLastError() == NTE_NO_KEY )
		{
			if( !CryptGenKey(hCryptProv, AT_KEYEXCHANGE, CRYPT_EXPORTABLE, &hExchangeKey ) )
			{
				hExchangeKey = NULL;
				PR_TRACE((this, prtERROR, "crypt\tError 0x%x during CryptGenKey", GetLastError()));
			}
		}
		else
			PR_TRACE((this, prtERROR, "crypt\tError 0x%x during CryptGetUserKey", GetLastError()));
	}

	if( hExchangeKey && !CryptGenKey(hCryptProv, CALG_RC4,  CRYPT_EXPORTABLE, &hKey) )
	{
		hKey = NULL;
		PR_TRACE((this, prtERROR, "crypt\tError 0x%x during CryptGenKey", GetLastError()));
	}

	if( hKey && !CryptExportKey(hKey, hExchangeKey, SIMPLEBLOB, 0, NULL, &dwKeyBlobLength) )
	{
		dwKeyBlobLength = 0;
		PR_TRACE((this, prtERROR, "crypt\tError 0x%x during CryptExportKey", GetLastError()));
	}

	if( dwKeyBlobLength )
	{
		dwEncryptedBufferLength = p_nDataSize;
		if( !CryptEncrypt(hKey, 0, TRUE, 0, NULL, &dwEncryptedBufferLength, 0) )
			PR_TRACE((this, prtERROR, "crypt\tError 0x%x during CryptEncrypt", GetLastError()));
	}

	if( dwEncryptedBufferLength )
	{
		dwBlobSize = sizeof(DWORD) + dwKeyBlobLength + dwEncryptedBufferLength;
	}

	if( !dwBlobSize )
		error = errUNEXPECTED;

	else if( dwBlobSize <= p_nBufferSize )
	{
		PBYTE pData = (PBYTE)p_pEncodedBuffer;

		*(DWORD*)pData = dwKeyBlobLength;
		pData += sizeof(DWORD);

		CryptExportKey(hKey, hExchangeKey, SIMPLEBLOB, 0, pData, &dwKeyBlobLength);
		pData += dwKeyBlobLength;

		memcpy(pData, p_pDecodedData, p_nDataSize);
		if( !CryptEncrypt(hKey, 0, TRUE, 0, pData, &dwEncryptedBufferLength, dwEncryptedBufferLength) )
			PR_TRACE((this, prtERROR, "crypt\tError 0x%x during CryptEncrypt", GetLastError()));
	}
	else
		error = errBUFFER_TOO_SMALL;

	if( p_result )
		*p_result = dwBlobSize;

	if( hExchangeKey )
		CryptDestroyKey(hExchangeKey);

	if( hKey )
		CryptDestroyKey(hKey);

	if( hCryptProv )
		CryptReleaseContext(hCryptProv, 0);

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, Decode )
// Parameters are:
tERROR CCryptoHelper::Decode( tDWORD* p_result, const tBYTE* p_pEncodedData, tDWORD p_nDataSize, tBYTE* p_pDecodedBuffer, tDWORD p_nBufferSize )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CryptoHelper::Decode method" );

	// Place your code here
	cAutoCS cs(m_lock, true);
	CAutoToken auto_token(m_hToken);

	if( p_nDataSize < sizeof(DWORD) )
		return errPARAMETER_INVALID;

	HCRYPTPROV hCryptProv = NULL;
	HCRYPTKEY hKey = NULL, hExchangeKey = NULL;
	
	PBYTE pData = (PBYTE)p_pEncodedData;
	DWORD dwKeyBlobLength = *(DWORD *)pData; pData += sizeof(DWORD);
	LONG dwBlobSize = p_nDataSize - dwKeyBlobLength - sizeof(DWORD);

	if( dwBlobSize < 0 )
		return errPARAMETER_INVALID;

	cStrObj prov = GetDefaultProvider();
	CryptAcquireContext(&hCryptProv, m_sKeyContainer, prov.empty() ? NULL : (LPCSTR)prov.c_str(cCP_ANSI), PROV_RSA_FULL, 0);
	if( !hCryptProv )
	{
        PR_TRACE((this, prtERROR, "crypt\tError 0x%x during CryptAcquireContext", GetLastError()));
		return errNOT_OK;
	}

	if( !CryptGetUserKey(hCryptProv, AT_KEYEXCHANGE, &hExchangeKey) )
		PR_TRACE((this, prtERROR, "crypt\tError 0x%x during CryptGetUserKey", GetLastError()));

	if( hExchangeKey && !CryptImportKey( hCryptProv, pData, dwKeyBlobLength, hExchangeKey, 0, &hKey) )
		PR_TRACE((this, prtERROR, "crypt\tError 0x%x during CryptImportKey", GetLastError()));

	error = errUNEXPECTED;
	if( hKey )
	{
		pData += dwKeyBlobLength;
		if( CryptDecrypt(hKey, NULL, TRUE, 0, pData, (PDWORD)&dwBlobSize) )
		{
			error = errOK;
			if( p_nBufferSize < (tDWORD)dwBlobSize )
				error = p_pDecodedBuffer ? errBUFFER_TOO_SMALL : errOK;
			else if( p_pDecodedBuffer )
				memcpy(p_pDecodedBuffer, pData, dwBlobSize);

			if( p_result )
				*p_result = dwBlobSize;

			error = errOK;
		}
		else
			PR_TRACE((this, prtERROR, "crypt\tError 0x%x during CryptDecrypt", GetLastError()));
	}

	if( hExchangeKey )
		CryptDestroyKey(hExchangeKey);

	if( hKey )
		CryptDestroyKey(hKey);

	if( hCryptProv )
		CryptReleaseContext(hCryptProv, 0);

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, CryptPassword )
// Parameters are:
tERROR CCryptoHelper::CryptPassword( cSerString* p_password, tDWORD p_flags )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CryptoHelper::CryptPassword method" );

	if( (m_nOsVersion & 0x80000000) || (p_flags & PSWD_RECRYPT) )
		p_flags |= PSWD_SIMPLE;

	if( !m_hToken && !(p_flags & PSWD_SIMPLE) )
		return errNOT_SUPPORTED;

	// Place your code here
	return CryptPasswordEx(p_password, p_flags);
}
// AVP Prague stamp end

tERROR CCryptoHelper::CryptPasswordEx(cSerString* p_password, tDWORD p_flags)
{
	// шифрование в debug simple (чтобы работало хоть как-то)
#ifdef _DEBUG
	p_flags &= ~PSWD_SIMPLE;
#endif

	tERROR error = errOK;
	if( p_flags & PSWD_DECRYPT )
	{
		cStrBuff& strBuff = p_password->m_str.c_str(cCP_ANSI);

		tDWORD nBase64EncodedSize = strBuff.used();
		tBYTE *pBase64EncodedData = (tBYTE*)alloca(nBase64EncodedSize);
		ATL::Base64Decode(strBuff.ptr(), nBase64EncodedSize, pBase64EncodedData, (PINT)&nBase64EncodedSize);

		tBYTE  pDecryptedData[2048] = "";
        tDWORD nDecryptedSize = sizeof(pDecryptedData);

		if( p_flags & PSWD_SIMPLE )
		{
			XORString(pBase64EncodedData, nBase64EncodedSize, pDecryptedData, &nDecryptedSize);
		}
		else
		{
			if( PR_FAIL( error = Decode(&nDecryptedSize, pBase64EncodedData, nBase64EncodedSize,
					pDecryptedData, sizeof(pDecryptedData))) )
				return error;
		}

		p_password->m_str = (tWCHAR*)pDecryptedData;

		if( p_flags & PSWD_RECRYPT )
			error = CryptPassword(p_password, 0);
	}
	else
	{
		if( p_flags & PSWD_RECRYPT )
			if( PR_FAIL(error = CryptPassword(p_password, PSWD_DECRYPT)) )
				return error;

		tBYTE  pCryptedData[2048] = "";
        tDWORD nCryptedSize = sizeof(pCryptedData);

		if( p_flags & PSWD_SIMPLE )
		{
			XORString((tBYTE*)p_password->m_str.data(), p_password->m_str.memsize(cCP_UNICODE),
				pCryptedData, &nCryptedSize);
		}
		else
		{
			if( PR_FAIL( error = Encode(&nCryptedSize, (tBYTE*)p_password->m_str.data(),
					p_password->m_str.memsize(cCP_UNICODE), pCryptedData, sizeof(pCryptedData))) )
				return error;
		}

		tDWORD nBase64CodedSize = ATL::Base64EncodeGetRequiredLength(nCryptedSize);
		tCHAR *pBase64CodedData = (tCHAR *)alloca(nBase64CodedSize + 1);
		ATL::Base64Encode(pCryptedData, nCryptedSize, pBase64CodedData, (PINT)&nBase64CodedSize, ATL_BASE64_FLAG_NOCRLF);
		pBase64CodedData[nBase64CodedSize] = 0;
		p_password->m_str = pBase64CodedData;
	}

	return error;
}

// AVP Prague stamp begin( External (user called) interface method implementation, IsMSTrusted )
// Parameters are:
tERROR CCryptoHelper::IsMSTrusted( const tWCHAR* p_file_name )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CryptoHelper::IsMSTrusted method" );

	// Place your code here
	cAutoCS cs(m_lock, true);

	if( !m_msVerifier )
		m_msVerifier = new CMSVerifyTrust();

	m_tmVerifyLastRequest = PrGetTickCount();

	if( m_msVerifier->IsMicrosoftSigned(p_file_name) )
		return errOK_DECIDED;

	return errNOT_OK;
}
// AVP Prague stamp end

CMSDataStore* CCryptoHelper::GetStore(tDWORD p_data_id)
{
	if (p_data_id < CRHLP_PERSISTENT_DATA_ID)
	{
		if (!m_msStore)
		{
			m_msStore = new CMSDataStore(m_sStorageName, m_dwVersion);
			if (m_helperStore && !m_bIsMSStoreRead)
			{
				m_helperStore->ReadMSStore(*m_msStore);
				m_bIsMSStoreRead = true;
			}
		}
		return m_msStore;
	}
	else
	{
		if (!m_msPersistentStore)
		{
			m_msPersistentStore = new CMSDataStore(m_sPersistentStorageName, 0);
			if (m_helperStore && !m_bIsMSPersistentStoreRead)
			{
				m_helperStore->ReadMSStore(*m_msPersistentStore);
				m_bIsMSPersistentStoreRead = true;
			}
		}
		return m_msPersistentStore;
	}
}

// AVP Prague stamp begin( External (user called) interface method implementation, StoreData )
// Parameters are:
tERROR CCryptoHelper::StoreData( tDWORD p_data_id, tPTR p_data, tDWORD p_size )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CryptoHelper::StoreData method" );

	// Place your code here
	cAutoCS cs(m_lock, true);
	CAutoToken auto_token(m_hToken);

	m_tmStoreLastRequest = PrGetTickCount();

	m_helperStore->StoreData(p_data_id, p_data, p_size, TRUE);

	if( !GetStore(p_data_id)->StoreData(p_data_id, p_data, p_size) )
		return errUNEXPECTED;

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetData )
// Parameters are:
tERROR CCryptoHelper::GetData( tDWORD p_data_id, tPTR p_data, tDWORD* p_size )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CryptoHelper::GetData method" );

	// Place your code here
	cAutoCS cs(m_lock, true);
	CAutoToken auto_token(m_hToken);

	m_tmStoreLastRequest = PrGetTickCount();

	tDWORD requestedSize = 0;
	if (p_size)
		requestedSize = *p_size;
	if (!GetStore(p_data_id)->GetData(p_data_id, p_data, (DWORD*)p_size))
	{
		if (p_size)
			*p_size = requestedSize;
		if (m_helperStore->GetData(p_data_id, p_data, (DWORD*)p_size))
		{
			if (p_data && !GetStore(p_data_id)->StoreData(p_data_id, p_data, *p_size))
				return errUNEXPECTED;
		}
		else
		{
			return errUNEXPECTED;
		}
	}
	else
	{
		m_helperStore->StoreData(p_data_id, p_data, *p_size, FALSE);
	}

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetOldData )
// Parameters are:
tERROR CCryptoHelper::GetOldData( tDWORD p_data_id, tPTR p_data, tDWORD* p_size )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CryptoHelper::GetOldData method" );

	// Place your code here
	cAutoCS cs(m_lock, true);
	CAutoToken auto_token(m_hToken);

	// Ищём данные в MSDataStore для версий 6.0.0.x и 6.0.1.x
	cStrObj sOldStorageName("%ProductType%");
	if( errOK_DECIDED != sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(sOldStorageName), 0, 0) )
		sOldStorageName = L"AVP";
	sOldStorageName += L"6";

	tDWORD size = *p_size;
	for (int dwVer = 1; dwVer >= 0; --dwVer)
	{
		CMSDataStore msStore(sOldStorageName.c_str(cCP_ANSI), dwVer);
		*p_size = size; // GetData обнуляет p_size, вёрнём размер на место
		if( msStore.GetData(p_data_id, p_data, (DWORD*)p_size) )
			return error = errOK;
	}

	return error = errOBJECT_NOT_FOUND;
}
// AVP Prague stamp end

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "CryptoHelper". Register function
tERROR CCryptoHelper::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end

// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(CryptoHelper_PropTable)
	mpLOCAL_PROPERTY_BUF( pgPRODUCT_VERSION, CCryptoHelper, m_szVersion, cPROP_BUFFER_READ_WRITE )
mpPROPERTY_TABLE_END(CryptoHelper_PropTable)
// AVP Prague stamp end

// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(CryptoHelper)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(CryptoHelper)
// AVP Prague stamp end

// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(CryptoHelper)
	mEXTERNAL_METHOD(CryptoHelper, Encode)
	mEXTERNAL_METHOD(CryptoHelper, Decode)
	mEXTERNAL_METHOD(CryptoHelper, CryptPassword)
	mEXTERNAL_METHOD(CryptoHelper, IsMSTrusted)
	mEXTERNAL_METHOD(CryptoHelper, StoreData)
	mEXTERNAL_METHOD(CryptoHelper, GetData)
	mEXTERNAL_METHOD(CryptoHelper, GetOldData)	
mEXTERNAL_TABLE_END(CryptoHelper)
// AVP Prague stamp end

// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "CryptoHelper::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_CRYPTOHELPER,                       // interface identifier
		PID_CRYPTOHELPER,                       // plugin identifier
		0x00000000,                             // subtype identifier
		CryptoHelper_VERSION,                   // interface version
		VID_MIKE,                               // interface developer
		&i_CryptoHelper_vtbl,                   // internal(kernel called) function table
		(sizeof(i_CryptoHelper_vtbl)/sizeof(tPTR)),// internal function table size
		&e_CryptoHelper_vtbl,                   // external function table
		(sizeof(e_CryptoHelper_vtbl)/sizeof(tPTR)),// external function table size
		CryptoHelper_PropTable,                 // property table
		mPROPERTY_TABLE_SIZE(CryptoHelper_PropTable),// property table size
		sizeof(CCryptoHelper)-sizeof(cObjImpl), // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"CryptoHelper(IID_CRYPTOHELPER) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end

// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call CryptoHelper_Register( hROOT root ) { return CCryptoHelper::Register(root); }
// AVP Prague stamp end

// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end

