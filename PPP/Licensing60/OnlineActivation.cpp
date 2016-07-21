// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  16 April 2007,  18:02 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Guschin
// File Name   -- licensing.cpp
// -------------------------------------------
// AVP Prague stamp end

#include "licensing.h"

#include <pr_cpp.h>

#include <memory>
#include <vector>

#include "ver_mod.h"

#include <plugin/p_win32_reg.h>
#include <iface/i_reg.h>
#include <structs/s_bl.h>

#include <plugin/p_miniarc.h>
#include <plugin/p_minizip.h>
#include <plugin/p_tempfile.h>

#define IMPEX_IMPORT_LIB
    #include <iface/e_ktime.h>
    #include <iface/e_loader.h>
    #include <plugin/p_win32_nfio.h>

#define PRODUCT_TYPE            "%ProductType%"

#define PRODUCT_KAV             "kav"
#define PRODUCT_KIS             "kis"

static char strCustCredRegPath[] = "HKLM\\" VER_COMPANY_REGISTRY_PATH "\0";

void OLATrace(void* hTrace, const char* pszMsg)
{
	PR_TRACE((hTrace, prtNOTIFY, (tSTRING) pszMsg));
}

void Split(std::vector<std::string>& arr, const cStrObj& text, wchar_t* sep)
{
	arr.clear();
	tDWORD beg = 0;
	while (1)
	{
		tDWORD pos = text.find(sep, beg, cStrObj::whole, fSTRING_FIND_FORWARD);
		if (pos == cStrObj::npos)
		{
			cStrObj item(text.substr(beg));
			if (!item.empty())
				arr.push_back((const char*)item.c_str(cCP_ANSI));
			break;
		}
		cStrObj item(text.substr(beg, pos - beg));
		if (!item.empty())
			arr.push_back((const char*)item.c_str(cCP_ANSI));
		beg = pos + 1;
	}
}

cStrObj CLicensing60::GetTrialCode()
{
	cBLSettings blset;
	PR_VERIFY_SUCC(m_hTM->GetProfileInfo(AVP_PROFILE_PROTECTION, &blset));
	if (!blset.m_TrialActCode.empty())
		return blset.m_TrialActCode;

	cStrObj strProductType(PRODUCT_TYPE);
	sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (strProductType), 0, 0);

	if( strProductType == PRODUCT_KIS )
		return L"4YYH1-KX9P4-251PC-AJYMP";
	else if( strProductType == PRODUCT_KAV )
		return L"UGPAZ-5NVEH-97CP5-WAXQR";
	else
		return L"00000-00000-00000-00000";
}

tERROR CLicensing60::ProcessActivationResult(tDWORD p_dwKeyAddMode,
											 hSTRING p_pResForm,
											 cSerializable* p_pResult,
											 tDWORD* p_pdwOLAErrCode,
											 tDWORD* p_pdwOLAErrCodeSpecific)
{
	switch (m_pActivation->GetResult())
	{
	case CActivation::Canceled:
		PR_TRACE((this, prtIMPORTANT, "lic\tOLA process aborted by user"));
		return errOPERATION_CANCELED;
	case CActivation::Error:
		{
			PR_TRACE((this, prtERROR, "lic\tOLA returns a error: code %i, specific %i", m_pActivation->GetErrorType(), m_pActivation->GetErrorCode()));
			switch (m_pActivation->GetErrorType())
			{
			case OnlineActivation::ActivationError::SYSTEM_ERROR:
				switch (m_pActivation->GetErrorCode())
				{
				case ERROR_INTERNET_NAME_NOT_RESOLVED:
				case ERROR_INTERNET_CANNOT_CONNECT:
				case ERROR_INTERNET_DISCONNECTED:
				case ERROR_INTERNET_PROXY_SERVER_UNREACHABLE:
				case ERROR_INTERNET_SERVER_UNREACHABLE:
					return errOLA_SERVER_NOT_AVAILABLE;
				}
				break;
			case OnlineActivation::ActivationError::LOGICAL_ERROR:
				switch (m_pActivation->GetErrorCode())
				{
				case OnlineActivation::LogicalError::INVALID_ACTIVATION_CODE:
					PR_TRACE((this, prtIMPORTANT, "lic\tWrong activation code"));
					return errOLA_WRONG_ACTIVATION_CODE;
				case OnlineActivation::LogicalError::INVALID_CUSTOMER_ID:
				case OnlineActivation::LogicalError::INVALID_CUSTOMER_PASSWORD:
					PR_TRACE((this, prtIMPORTANT, "lic\tWrong CustomerID or password"));
					return errOLA_WRONG_CUSTOMER_ID_OR_PASSWORD;
				}
				break;
			case OnlineActivation::ActivationError::SERVER_ERROR:
				switch (m_pActivation->GetErrorCode())
				{
				case OnlineActivation::ServerError::INTERNAL_SERVER_ERROR:
					return errOLA_INTERNAL_SERVER_ERROR;
				case OnlineActivation::ServerError::KEY_NOT_FOUND:
					return errOLA_WRONG_APPLICATION_ID;
				case OnlineActivation::ServerError::KEY_OR_ACTIVATION_CODE_BLOCKED:
					return errOLA_BLOCKED;
				case OnlineActivation::ServerError::WRONG_CUSTOMER_ID_OR_PASSWORD:
					PR_TRACE((this, prtIMPORTANT, "lic\tWrong CustomerID or password"));
					return errOLA_WRONG_CUSTOMER_ID_OR_PASSWORD;
				case OnlineActivation::ServerError::WRONG_ACTIVATION_CODE:
					PR_TRACE((this, prtIMPORTANT, "lic\tWrong activation code"));
					return errOLA_WRONG_ACTIVATION_CODE;
				case OnlineActivation::ServerError::WRONG_APPLICATION_ID:
					return errOLA_WRONG_APPLICATION_ID;
				case OnlineActivation::ServerError::ACTIVATION_CODE_REQUIRED:
					return errOLA_ACTIVATION_CODE_REQUIRED;
				case OnlineActivation::ServerError::ACTIVATION_PERIOD_EXPIRED:
					return errOLA_ACTIVATION_PERIOD_EXPIRED;
				case OnlineActivation::ServerError::ACTIVATION_ATTEMPTS_EXHAUSTED:
					return errOLA_QUANTITY_OF_ATTEMPTS_EXHAUSTED;
				//case OnlineActivation::ServerError::USE_MOBILE_ACTIVATION:
				//case OnlineActivation::ServerError::MANDATORY_PARAMETERS_MISSING:
				}
				break;
			case OnlineActivation::ActivationError::HTTP_ERROR:
				switch (m_pActivation->GetErrorCode())
				{
				case HTTP_STATUS_BAD_REQUEST       : return errONLINE_CLNT_SYNTHAX;
				case HTTP_STATUS_DENIED            : return errONLINE_CLNT_UNAUTH;
				case HTTP_STATUS_FORBIDDEN         : return errONLINE_CLNT_FORBID;
				case HTTP_STATUS_NOT_FOUND         : return errONLINE_CLNT_NOTFND;
				case HTTP_STATUS_BAD_METHOD        : return errONLINE_CLNT_BADMETH;
				case HTTP_STATUS_NONE_ACCEPTABLE   : return errONLINE_CLNT_NONACPT;
				case HTTP_STATUS_PROXY_AUTH_REQ    : return errONLINE_CLNT_PRXAUTHRQ;
				case HTTP_STATUS_REQUEST_TIMEOUT   : return errONLINE_CLNT_TIMOUT;
				case HTTP_STATUS_CONFLICT          : return errONLINE_CLNT_STCONFL;
				case HTTP_STATUS_GONE              : return errONLINE_CLNT_GONE;
				case HTTP_STATUS_LENGTH_REQUIRED   : return errONLINE_CLNT_LENREQ;
				case HTTP_STATUS_PRECOND_FAILED    : return errONLINE_CLNT_PREFAIL;
				case HTTP_STATUS_REQUEST_TOO_LARGE : return errONLINE_CLNT_TOOLARGE;
				case HTTP_STATUS_URI_TOO_LONG      : return errONLINE_CLNT_URILONG;
				case HTTP_STATUS_UNSUPPORTED_MEDIA : return errONLINE_CLNT_UNSMEDIA;
				}
				return errONLINE_HTTP_OTHER;
			}
			if (p_pdwOLAErrCode)
				*p_pdwOLAErrCode = m_pActivation->GetErrorType();
			if (p_pdwOLAErrCodeSpecific)
				*p_pdwOLAErrCodeSpecific = m_pActivation->GetErrorCode();
			return errUNKNOWN_OLA_SERVER_ERROR;
		}
	case CActivation::HtmlForm:
		PR_TRACE((this, prtIMPORTANT, "lic\tOLA html form received"));
		sysSendMsg(pmc_ONLINE_LICENSING, pm_DOWNLOAD_COMPLETED, NULL, NULL, NULL);
		p_pResForm->ImportFromBuff(NULL
			, (tPTR) m_pActivation->GetData()
			, m_pActivation->GetDataSize()
			, cCP_UNICODE, cSTRING_Z);
		return errOK;
	case CActivation::KeyFile:
		{
			tERROR error;
			PR_TRACE((this, prtIMPORTANT, "lic\tOLA key file received"));
			sysSendMsg(pmc_ONLINE_LICENSING, pm_DOWNLOAD_COMPLETED, NULL, NULL, NULL);
		
			cAutoObj<cIO> pTmpIo;
			if (PR_FAIL(error = g_root->sysCreateObjectQuick(pTmpIo, IID_IO, PID_TEMPIO)))
			{
				PR_TRACE((this, prtERROR, "lic\tcan't create tempio"));
				return error;
			}
			pTmpIo->SeekWrite(NULL, 0, (tPTR) m_pActivation->GetData(), m_pActivation->GetDataSize());

			cAutoObj<cOS> pPackageOS;
			if (PR_FAIL(error = pTmpIo->sysCreateObject((hOBJECT *)&pPackageOS, IID_OS, PID_UNIVERSAL_ARCHIVER))
				|| PR_FAIL(error = pPackageOS->propSetDWord(ppMINI_ARCHIVER_PID, PID_MINIZIP))
				|| PR_FAIL(error = pPackageOS->sysCreateObjectDone()))
			{
				PR_TRACE((this, prtERROR, "lic\tcan't create archiver"));
				return error;
			}

			// extract filename of a key file
			cStringObj keyName;			
			cAutoObj<cObjPtr> hObjPtr;
			if (PR_FAIL(error = pPackageOS->PtrCreate(&hObjPtr, NULL)))
			{
				PR_TRACE((this, prtERROR, "lic\tcan't create archive enumerator"));
				return error;
			}
			while (PR_SUCC(hObjPtr->Next()))
			{
				char filename[MAX_PATH];
				hObjPtr->get_pgOBJECT_NAME(filename, sizeof(filename));
				if (strstr(filename, ".key"))
				{
					keyName = filename;
					break;
				}
			}

			if (keyName.empty())
			{
				PR_TRACE((this, prtERROR, "lic\tcan't find key file in archive"));
				return errOBJECT_NOT_FOUND;
			}

			// extract a key file
			cStringObj keyPath;
			cAutoObj<cIO> pZippedKeyFile;
			if (PR_FAIL(error = pPackageOS->IOCreate(pZippedKeyFile, cAutoString(keyName), fACCESS_READ, fOMODE_OPEN_IF_EXIST)))
			{
				PR_TRACE((this, prtERROR, "lic\tcan't create key file IO"));
				return error;
			}
			error = PrGetTempFileName(cAutoString(keyPath), "key", cCP_ANSI);
			if (PR_FAIL(error = PrCopy(pZippedKeyFile, cAutoString(keyPath), fPR_FILE_REPLACE_EXISTING)))
			{
				PR_TRACE((this, prtERROR, "lic\tcan't save key file"));
				return error;
			}			

			// add a key
			if (p_pResult)
				CheckKeyFile(keyPath.c_str(cCP_UNICODE), p_dwKeyAddMode, p_pResult);
			
			if (PR_FAIL(error = AddKey(keyPath.c_str(cCP_UNICODE), p_dwKeyAddMode)))
			{
				PR_TRACE((this, prtERROR, "lic\tcan't add a key"));
				return error;
			}

			PrDeleteFile(cAutoString(keyPath), 0);

			// Get customer credentials from CustomerInfo.txt if it exist
			//		CustomerID: 0000010399
			//		Password: uqLFVYHR
			cAutoObj<cIO> pCustomerInfo;
			if (PR_SUCC(pPackageOS->IOCreate(pCustomerInfo, cAutoString(cStrObj("CustomerInfo.txt")), fACCESS_READ, fOMODE_OPEN_IF_EXIST)))
			{
				char buf[1024];
				tDWORD size = 0;
				if (PR_SUCC(pCustomerInfo->SeekRead(&size, 0, buf, sizeof(buf))))
				{
					buf[size] = 0;
					char customerId[1024], pwd[1024];
					
					sscanf(buf, "CustomerID: %s\nPassword: %s", customerId, pwd);
					UpdateCustomerCredentials(customerId, pwd);
				}
			}
			sysSendMsg(pmc_ONLINE_LICENSING, pm_DOWNLOAD_COMPLETED, NULL, NULL, NULL);
		}
		return errOK;
	default:
		return errUNEXPECTED;
	}
}

void CLicensing60::OnSendingRequest()
{
	PR_TRACE((g_root, prtNOT_IMPORTANT, "lic\tOLA sending request"));
	sysSendMsg(pmc_ONLINE_LICENSING, pm_POSTING_DATA, NULL, NULL, NULL);
}

void CLicensing60::OnRequestSent(size_t nBytes)
{
	PR_TRACE((g_root, prtNOT_IMPORTANT, "lic\tOLA request is sent (%i bytes)", nBytes));
	sysSendMsg(pmc_ONLINE_LICENSING, pm_SENT, NULL, NULL, NULL);
}

void CLicensing60::OnRequestComplete()
{
	PR_TRACE((g_root, prtNOT_IMPORTANT, "lic\tOLA request is complete"));
	sysSendMsg(pmc_ONLINE_LICENSING, pm_DOWNLOAD_COMPLETED, NULL, NULL, NULL);
}

bool CLicensing60::GetProxyCredentials(char* user, size_t user_size, char* password, size_t password_size)
{
	if (m_blGetPassForProxyFromUser)
	{
		if (!m_hStorage || m_hStorage->propGetIID() != IID_CRYPTOHELPER)
			return false;
		cCryptoHelper* pCrHlp = (cCryptoHelper*) m_hStorage;

		cProxySettings proxySettings;
		proxySettings.m_strProxyLogin = m_auth_info.m_strUserName;
		proxySettings.m_strProxyPassword = m_auth_info.m_strUserPassword;
		pCrHlp->CryptPassword(&cSerString(proxySettings.m_strProxyPassword), 0);

		cProxyCredentialsRequest settingsRequester;
		settingsRequester.m_settings = &proxySettings;

		tERROR settingsRequesterCreateResult = 
			sysSendMsg(pmcASK_ACTION, transportRequest_ProxyCredentials, 0, &settingsRequester, SER_SENDMSG_PSIZE);

		if (PR_FAIL(settingsRequesterCreateResult))
			return false;

		m_auth_info.m_strUserName = proxySettings.m_strProxyLogin;
		m_auth_info.m_strUserPassword = proxySettings.m_strProxyPassword;
		pCrHlp->CryptPassword(&cSerString(m_auth_info.m_strUserPassword), PSWD_DECRYPT);
	}

	m_blGetPassForProxyFromUser = cTRUE;

	cStrBuff userName = m_auth_info.m_strUserName.c_str(cCP_ANSI);
	cStrBuff userPwd = m_auth_info.m_strUserPassword.c_str(cCP_ANSI);
	if (userName.used() >= user_size
		|| userPwd.used() >= password_size)
	{
		memset((tCHAR*)userPwd, 0, userPwd.used());
		return false;
	}
	strncpy_s(user, user_size, userName.ptr(), userName.used());
	strncpy_s(password, password_size, userPwd.ptr(), userPwd.used());
	memset((tCHAR*)userPwd, 0, userPwd.used());
	return true;
}

tERROR CLicensing60::_AddOLAKey( cSerializable* p_pUserData, tDWORD p_dwKeyAddMode, hSTRING p_pResForm, cSerializable* p_pResult, tDWORD* p_pdwOLAErrCode, tDWORD* p_pdwOLAErrCodeSpecific )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Licensing::AddOLAKey method" );

	cAutoCS autoCSState(m_hCSOnline, true) ;

    if (p_pUserData == NULL || !p_pUserData->isBasedOn(cUserData::eIID)
		|| p_pResForm == NULL
		|| p_pResult == NULL || !p_pResult->isBasedOn(cCheckInfo::eIID))
    {
        return errPARAMETER_INVALID;
    }

	const cUserData& userData = *static_cast<cUserData*>(p_pUserData);
	
	PR_TRACE((this, prtIMPORTANT, "lic\tOnline key activation begin..."));

	if (m_pActivation.get())
	{
		PR_TRACE((this, prtERROR, "lic\tAnother key activation is in progress."));
		return errOBJECT_BAD_INTERNAL_STATE;
	}

    sysSendMsg(pmc_ONLINE_LICENSING, pm_INITING, NULL, NULL, NULL);
    sysSendMsg(pmc_ONLINE_LICENSING, pm_CONNECTING, NULL, NULL, NULL);

	OnlineActivation::ApplicationId appId;
	appId.id = m_nProdId;

	OnlineActivation::CustomerId custId;
	if (userData.m_bTrial)
	{
		custId.id = 0;
		memset(custId.password, 0, sizeof(custId.password));
	}
	else
	{
		cStrBuff strCustomerIdBuff = userData.m_strCustomerId.c_str(cCP_ANSI);
		char* pStopChar = NULL;
		custId.id = strtoul(strCustomerIdBuff, &pStopChar, 10);
		if (pStopChar && *pStopChar != '\0')
		{
			PR_TRACE((this, prtERROR, "lic\tWrong customer ID (\"%S\").", userData.m_strCustomerId.data()));
			return errOLA_WRONG_CUSTOMER_ID_OR_PASSWORD;
		}
		memset(custId.password, 0, sizeof(custId.password));
		memcpy(custId.password, static_cast<const char*>(userData.m_strCustomerPwd.c_str(cCP_ANSI)), min(sizeof(custId.password), userData.m_strCustomerPwd.size()));
	}

	cStrObj strKeyNum = userData.m_bTrial ? GetTrialCode() : userData.m_strKeyNum;

	//////////////////////////////////////////////////////////////////////////
	// Prepare ActivationConfig
	OnlineActivation::ActivationCode actCode;
	char part1[6], part2[6], part3[6], part4[6];
	if (4 != sscanf_s(strKeyNum.c_str(cCP_ANSI), "%5s-%5s-%5s-%5s", part1, sizeof(part1), part2, sizeof(part2), part3, sizeof(part3), part4, sizeof(part4)))
		return errOLA_WRONG_ACTIVATION_CODE;
	memcpy(actCode.part1, part1, sizeof(actCode.part1));
	memcpy(actCode.part2, part2, sizeof(actCode.part2));
	memcpy(actCode.part3, part3, sizeof(actCode.part3));
	memcpy(actCode.part4, part4, sizeof(actCode.part4));

	PR_TRACE((this, prtIMPORTANT, "lic\tParams: Customer ID=%S, Customer pwd=%S, Key num=%S, trial - %s, url=%S",
		userData.m_strCustomerId.data(),
		userData.m_strCustomerPwd.data(),
		strKeyNum.data(),
		userData.m_bTrial ? "yes" : "no",
		userData.m_strUrl.data()));

	//////////////////////////////////////////////////////////////////////////
	// Prepare ActivationConfig
	std::vector<std::string> urlListStr;
	Split(urlListStr, userData.m_strUrl, L" ");

	std::vector<const char*> urlList;
	for (size_t i = 0; i < urlListStr.size(); ++i)
		urlList.push_back(urlListStr[i].c_str());
	urlList.push_back(NULL);

	OnlineActivation::ActivationConfig cfg;
	cfg.url_list = &urlList[0];
	cfg.inet_config.access_type = OnlineActivation::InternetConfig::PRECONFIG;
	cfg.inet_config.proxy_list = NULL;
	cfg.inet_config.bypass_list = NULL;

	//////////////////////////////////////////////////////////////////////////
	// Activation
	m_pActivation.reset(new CActivation(this, this));
	if (!m_pActivation.get())
		return errNOT_ENOUGH_MEMORY;

	bool bContinue = m_pActivation->BeginActivation(
		cfg, appId, actCode, (custId.id != 0 && !userData.m_bTrial) ? &custId : NULL);

	error = ProcessActivationResult(p_dwKeyAddMode, p_pResForm, p_pResult, p_pdwOLAErrCode, p_pdwOLAErrCodeSpecific);
	if (PR_SUCC(error))
	{
		// запомним введённый юзером CustomerId
		UpdateCustomerCredentials(userData.m_strCustomerId, userData.m_strCustomerPwd);
	}
	else
	{
		PR_TRACE((this, prtERROR, "lic\tActivation failed: %terr", error));
	}

	if (!bContinue)
	{
		PR_TRACE((this, prtIMPORTANT, "lic\tReleasing OLA library"));
		m_pActivation.reset();
	}
	else
		PR_TRACE((this, prtIMPORTANT, "lic\tActivation first step done, waiting for reply..."));
	
	return error;
}

tERROR CLicensing60::_SubmitOLAForm( cSerializable* p_pForm, tDWORD p_dwKeyAddMode, hSTRING p_pResForm, cSerializable* p_pResult, tDWORD* p_pdwOLAErrCode, tDWORD* p_pdwOLAErrCodeSpecific )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Licensing::SubmitOLAForm method" );
	
    cAutoCS autoCSState(m_hCSOnline, true) ;
	
    if (p_pForm == NULL || !p_pForm->isBasedOn(cPostData::eIID)
		|| p_pResForm == NULL
		|| p_pResult == NULL || !p_pResult->isBasedOn(cCheckInfo::eIID))
    {
        return errPARAMETER_INVALID;
    }

	PR_TRACE((this, prtIMPORTANT, "lic\tActivation submit form step begins..."));

	cPostData& postData = *static_cast<cPostData*>(p_pForm);
	
	if (!m_pActivation.get())
	{
		PR_TRACE((this, prtERROR, "lic\tOLA lib has been already released."));
		return errOBJECT_BAD_INTERNAL_STATE;
	}
	
    sysSendMsg(pmc_ONLINE_LICENSING, pm_INITING, NULL, NULL, NULL);
    sysSendMsg(pmc_ONLINE_LICENSING, pm_CONNECTING, NULL, NULL, NULL);

	bool bContinue = m_pActivation->ContinueActivation(
		postData.m_header.c_str(cCP_ANSI)
		, postData.m_postData.data()
		, postData.m_postData.size());
	
	error = ProcessActivationResult(p_dwKeyAddMode, p_pResForm, p_pResult, p_pdwOLAErrCode, p_pdwOLAErrCodeSpecific);
	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtERROR, "lic\tActivation failed: %terr", error));
	}
	
	if (!bContinue)
	{
		PR_TRACE((this, prtIMPORTANT, "lic\tReleasing OLA library"));
		m_pActivation.reset();
	}
	
	return error;
}

tERROR CLicensing60::_StopOLA()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Licensing::StopOLA method" );

	PR_TRACE((this, prtIMPORTANT, "lic\tTrying to stop OLA..."));

    if (m_pActivation.get())
		m_pActivation->Cancel();

	cAutoCS autoCSState(m_hCSOnline, true) ;

	if (m_pActivation.get())
	{
		PR_TRACE((this, prtIMPORTANT, "lic\tReleasing OLA library"));
		m_pActivation.reset();
	}

	PR_TRACE((this, prtIMPORTANT, "lic\tTrying to stop OLA finished"));

	return error;
}

void PseudoCrypt(BYTE* buf, size_t size, bool crypt)
{
	if (crypt)
	{
		BYTE prev_clear = 0xAB;
		for (size_t i = 0; i < size; ++i)
		{
			BYTE that = buf[i];
			buf[i] ^= prev_clear ^ (0xDE + i);
			prev_clear = that;
		}
	}
	else
	{
		BYTE prev_clear = 0xAB;
		for (size_t i = 0; i < size; ++i)
		{
			buf[i] ^= prev_clear ^ (0xDE + i);
			prev_clear = buf[i];
		}
	}
}

void ReadStrFromReg(cAutoObj<cRegistry>& reg, const char* pszKey, hSTRING result, bool crypt)
{
	tTYPE_ID typeID = crypt ? tid_BINARY : tid_STRING;
	tDWORD res;
	if (PR_FAIL(reg->GetValue(&res, cRegRoot, pszKey, &typeID, NULL, 0)))
		return;

	BYTE* str = (BYTE*) _alloca(res);
	if (PR_FAIL(reg->GetValue(NULL, cRegRoot, pszKey, &typeID, str, res)))
		return;

	if (crypt)
		PseudoCrypt(str, res, false);

	result->ImportFromBuff(NULL, str, res, cCP_ANSI, 0);
}

tERROR CLicensing60::_GetCustomerCredentials( hSTRING strCustomerId, hSTRING strCustomerPassword )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Licensing::GetCustomerCredentials method" );
	
	if (strCustomerId == NULL || strCustomerPassword == NULL)
	{
		error = errPARAMETER_INVALID;
		return error;
	}

	cAutoObj<cRegistry> reg;
	PR_VERIFY_SUCC(error = sysCreateObject((hOBJECT *)&reg, IID_REGISTRY, PID_WIN32_REG));
	if (!reg || PR_FAIL(error))
		return error;
	
	PR_VERIFY_SUCC(reg->set_pgROOT_POINT(strCustCredRegPath, sizeof(strCustCredRegPath), cCP_ANSI));
	PR_VERIFY_SUCC(error = reg->sysCreateObjectDone());
	if (PR_FAIL(error))
		return error;

	ReadStrFromReg(reg, "CustomerId", strCustomerId, false);
	ReadStrFromReg(reg, "CustomerPassword", strCustomerPassword, true);
	
	return error;
}

void CLicensing60::UpdateCustomerCredentials(const cStringObj& strCustomerId,
											 const cStringObj& strPassword)
{
	if (strCustomerId.empty())
		return;

	PR_TRACE((this, prtIMPORTANT, "lic\tNew customer credentials: CustomerId=%S, Password=%S",
		strCustomerId.data(), strPassword.data()));

	tERROR error;

	cAutoObj<cRegistry> reg;
	PR_VERIFY_SUCC(error = sysCreateObject((hOBJECT *)&reg, IID_REGISTRY, PID_WIN32_REG));
	if (PR_FAIL(error))
		return;
	PR_VERIFY_SUCC(reg->set_pgROOT_POINT(strCustCredRegPath, sizeof(strCustCredRegPath), cCP_ANSI));
	PR_VERIFY_SUCC(reg->propSetBool(pgOBJECT_RO, cFALSE));
	PR_VERIFY_SUCC(error = reg->sysCreateObjectDone());
	if (PR_FAIL(error))
		return;
	
	reg->SetValue(cRegRoot, "CustomerId", tid_STRING, strCustomerId.c_str(cCP_ANSI), strCustomerId.size(), cTRUE);
	cStrBuff temp = strPassword.c_str(cCP_ANSI);
	PseudoCrypt((BYTE*)temp.m_ptr, temp.m_used, true);
	reg->SetValue(cRegRoot, "CustomerPassword", tid_BINARY, temp.m_ptr, temp.m_used, cTRUE);

	sysSendMsg(pmc_LICENSING, pm_CUSTOMER_ID_OBTAINED, NULL, NULL, NULL);
}