#ifndef __ONLY_PARAM_DESCR
#include "Params.h"
#endif

void GetSingleParameterInfo(PFILTER_PARAM pParam, CHAR* pName, CHAR* pSingleDescr, DWORD* dwType)
{
	PSINGLE_PARAM pSingleParam;
	pSingleParam = (PSINGLE_PARAM) _MM_Alloc(sizeof(SINGLE_PARAM) + pParam->m_ParamSize);
	if (pSingleParam == NULL)
	{
		lstrcpy(pSingleDescr, "<failed>");
	}
	else
	{
		pSingleParam->ParamFlags = 0;
		pSingleParam->ParamID = pParam->m_ParamID;
		pSingleParam->ParamSize = pParam->m_ParamSize;
		RtlCopyMemory(pSingleParam->ParamValue, pParam->m_ParamValue, pSingleParam->ParamSize);

		GetSingleParameterInfo(pSingleParam, pName, pSingleDescr, dwType);

		_MM_Free(pSingleParam);
	}
}

void GetSingleParameterInfo(PSINGLE_PARAM pParam, CHAR* pName, CHAR* pSingleDescr, DWORD* dwType)
{
	*dwType = _PARAM_BINARY_;
	lstrcpy(pSingleDescr, "");

	DWORD dwParamID = pParam->ParamID;
	PVOID pData = pParam->ParamValue;
	DWORD dwDataLen = pParam->ParamSize;

	switch (dwParamID)
	{
	case _PARAM_OBJECT_CONTEXT_FLAGS:
		lstrcpy(pName, "Context flags");
		wsprintf(pSingleDescr, "%#x", *(DWORD*) pData);
		*dwType = _PARAM_DWORD_;
		break;
	case _PARAM_USER_FILTER_NAME:
		lstrcpy(pName, "Filter name");
		lstrcpy(pSingleDescr, (PCHAR) pData);
		*dwType = _PARAM_STR_;
		break;
	case _PARAM_USER_FILTER_DESCRIPTION:
		lstrcpy(pName, "Filter description");
		lstrcpy(pSingleDescr, (PCHAR) pData);
		*dwType = _PARAM_STR_;
		break;
	case _PARAM_OBJECT_URL:
		lstrcpy(pName, "Url");
		lstrcpy(pSingleDescr, (PCHAR) pData);
		*dwType = _PARAM_STR_;
		break;
	case _PARAM_OBJECT_URL_DEST:
		lstrcpy(pName, "Url (destination)");
		lstrcpy(pSingleDescr, (PCHAR) pData);
		*dwType = _PARAM_STR_;
		break;
	case _PARAM_OBJECT_URL_W:
		lstrcpy(pName, "Url (unicode)");
		wsprintf(pSingleDescr, "%S", (PCHAR) pData);
		*dwType = _PARAM_STR_W;
		break;
	case _PARAM_OBJECT_URL_DEST_W:
		lstrcpy(pName, "Url (destination, unicode)");
		wsprintf(pSingleDescr, "%S", (PCHAR) pData);
		*dwType = _PARAM_STR_W;
		break;
	case _PARAM_OBJECT_BINARYDATA:
		lstrcpy(pName, "Binary data");
		break;
	case _PARAM_OBJECT_ACCESSATTR:
		lstrcpy(pName, "Access attributes");
		wsprintf(pSingleDescr, "%#x", *(DWORD*) pData);
		*dwType = _PARAM_DWORD_;
		break;
	case _PARAM_OBJECT_SOURCE_ID:
		lstrcpy(pName, "Source ID");
		wsprintf(pSingleDescr, "%#x", *(DWORD*) pData);
		*dwType = _PARAM_DWORD_;
		break;
	case _PARAM_OBJECT_DEST_ID:
		lstrcpy(pName, "Destination ID");
		wsprintf(pSingleDescr, "%#x", *(DWORD*) pData);
		*dwType = _PARAM_DWORD_;
		break;
	case _PARAM_OBJECT_OBJECTHANDLE:
		lstrcpy(pName, "Object handle");
		wsprintf(pSingleDescr, "%#x", *(DWORD*) pData);
		*dwType = _PARAM_DWORD_;
		break;
	case _PARAM_OBJECT_CLIENTID1:
		lstrcpy(pName, "client id 1");
		wsprintf(pSingleDescr, "%#x", *(DWORD*) pData);
		*dwType = _PARAM_DWORD_;
		break;
	case _PARAM_OBJECT_CLIENTID2:
		lstrcpy(pName, "client id 2");
		wsprintf(pSingleDescr, "%#x", *(DWORD*) pData);
		*dwType = _PARAM_DWORD_;
		break;
	case _PARAM_OBJECT_DATALEN:
		lstrcpy(pName, "Object's datalen");
		wsprintf(pSingleDescr, "First dword: %d", *(DWORD*) pData);
		*dwType = _PARAM_DWORD_;
		break;
	case _PARAM_OBJECT_SECTOR:
		{
			PLARGE_INTEGER pll = (PLARGE_INTEGER) pData;
			lstrcpy(pName, "Sector");
			wsprintf(pSingleDescr, "Hi: %d Lo %d", pll->HighPart, pll->LowPart);
		}
		*dwType = _PARAM_QDWORD_;
		break;
	case _PARAM_OBJECT_ORIGINAL_PACKET:
		lstrcpy(pName, "Original packet");
		break;
	case _PARAM_OBJECT_SID:
		{
			lstrcpy(pName, "Security identificator");

			DWORD			dwUserLen;
			CHAR			UserName[MAX_PATH];
			DWORD			dwDomainLen;
			CHAR			Domain[MAX_PATH];
			
			SID*			pSid;
			
			pSid = (SID*) pData;
			dwDomainLen = sizeof(Domain);
			dwUserLen = sizeof(UserName);
			
			if (IDriverGetUserFromSid(pSid, Domain, &dwDomainLen, UserName, &dwUserLen))
				wsprintf(pSingleDescr, "%s\\%s", Domain, UserName);
			else
				wsprintf(pSingleDescr, "%s\\%s", Domain, UserName);
		}
		break;
	case _PARAM_OBJECT_BYTEOFFSET:
		lstrcpy(pName, "Offest (in bytes)");
		*dwType = _PARAM_QDWORD_;
		switch (pParam->ParamSize)
		{
		case sizeof(DWORD):
			wsprintf(pSingleDescr, "%d", *(DWORD*)pData);
			*dwType = _PARAM_DWORD_;
			break;
		case sizeof(LARGE_INTEGER):
			PLARGE_INTEGER pll = (PLARGE_INTEGER) pData;
			wsprintf(pSingleDescr, "Hi: %u Lo %u", pll->HighPart, pll->LowPart);
			break;
		}
		break;
	case ID_ETH_DEST:
		lstrcpy(pName, "ETH_DEST");
		break;
	case ID_ETH_SOURCE:
		lstrcpy(pName, "ETH_DEST");
		break;
	case ID_ETH_PROTO:
		lstrcpy(pName, "ETH_PROTO");
		wsprintf(pSingleDescr, "%#x", *(WORD*) pData);
		*dwType = _PARAM_WORD_;
		break;
	case ID_ETH_DSAP:
		lstrcpy(pName, "ETH_DSAP");
		break;
	case ID_ETH_SSAP:
		lstrcpy(pName, "ETH_SSAP");
		break;
	case ID_ETH_LLC_CONTROL:
		lstrcpy(pName, "ETH_LLC_CONTROL");
		break;
	case ID_ETH_LLC_MODE:
		lstrcpy(pName, "ETH_LLC_MODE");
		break;
	case ID_ETH_SNAP_OUI:
		lstrcpy(pName, "ETH_SNAP_OUI");
		break;
	case ID_ETH_SNAP_TYPE:
		lstrcpy(pName, "ETH_SNAP_TYPE");
		break;

	case ID_ARP_HRD_TYPE:
		lstrcpy(pName, "ARP_HRD_TYPE");
		break;
	case ID_ARP_PRO_TYPE:
		lstrcpy(pName, "ARP_PRO_TYPE");
		break;
	case ID_ARP_HRD_SIZE:
		lstrcpy(pName, "ARP_HRD_SIZE");
		break;
	case ID_ARP_PRO_SIZE:
		lstrcpy(pName, "ARP_PRO_SIZE");
		break;
	case ID_ARP_COMMAND:
		lstrcpy(pName, "ARP_COMMAND");
		break;
	case ID_ARP_SENDER_MAC_ADDRESS:
		lstrcpy(pName, "ARP_SENDER_MAC_ADDRESS");
		break;
	case ID_ARP_SENDER_IP_ADDRESS:
		lstrcpy(pName, "ARP_SENDER_IP_ADDRESS");
		break;
	case ID_ARP_TARGET_MAC_ADDRESS:
		lstrcpy(pName, "ARP_TARGET_MAC_ADDRESS");
		break;
	case ID_ARP_TARGET_IP_ADDRESS:
		lstrcpy(pName, "ARP_TARGET_IP_ADDRESS");
		break;
		
	case ID_IP_HDR_LENGTH:
		lstrcpy(pName, "IP_HDR_LENGTH");
		wsprintf(pSingleDescr, "%d", *(DWORD*) pData);
		*dwType = _PARAM_DWORD_;
		break;
	case ID_IP_VERSION:
		lstrcpy(pName, "IP_VERSION");
		wsprintf(pSingleDescr, "%d", *(WORD*) pData);
		*dwType = _PARAM_WORD_;
		break;
	case ID_IP_TYPE_OF_SERVICE:
		lstrcpy(pName, "IP_TYPE_OF_SERVICE");
		wsprintf(pSingleDescr, "%d", *(BYTE*) pData);
		*dwType = _PARAM_BYTE_;
		break;
	case ID_IP_TOTAL_LENGTH:
		lstrcpy(pName, "IP_TOTAL_LENGTH");
		wsprintf(pSingleDescr, "%d", *(WORD*) pData);
		*dwType = _PARAM_WORD_;
		break;
	case ID_IP_IDENTIFICATION:
		lstrcpy(pName, "IP_IDENTIFICATION");
		wsprintf(pSingleDescr, "%d", *(WORD*) pData);
		*dwType = _PARAM_WORD_;
		break;
	case ID_IP_FRAGMENTATION_FLAG:
		lstrcpy(pName, "IP_FRAGMENTATION_FLAG");
		wsprintf(pSingleDescr, "%d", *(WORD*) pData);
		*dwType = _PARAM_WORD_;
		break;
	case ID_IP_FRAGMENTATION_OFFSET:
		lstrcpy(pName, "IP_FRAGMENTATION_OFFSET");
		break;
	case ID_IP_TIME_TO_LIVE:
		lstrcpy(pName, "IP_TIME_TO_LIVE");
		wsprintf(pSingleDescr, "%d", *(BYTE*) pData);
		*dwType = _PARAM_BYTE_;
		break;
	case ID_IP_PROTOCOL:
		lstrcpy(pName, "IP_PROTOCOL");
		wsprintf(pSingleDescr, "%d", *(BYTE*) pData);
		*dwType = _PARAM_BYTE_;
		break;
	case ID_IP_SRC_ADDRESS:
		lstrcpy(pName, "IP_SRC_ADDRESS");
		*dwType = _PARAM_IP4_ADDRESS;
		{
			BYTE *ptr = (BYTE *)pData;
			wsprintf(pSingleDescr, "%d.%d.%d.%d", ptr[3], ptr[2], ptr[1], ptr[0]);
		}
		break;
	case ID_IP_DST_ADDRESS:
		lstrcpy(pName, "IP_DST_ADDRESS");
		*dwType = _PARAM_IP4_ADDRESS;
		{
			BYTE *ptr = (BYTE *)pData;
			wsprintf(pSingleDescr, "%d.%d.%d.%d", ptr[3], ptr[2], ptr[1], ptr[0]);
		}
		break;
	case ID_IP_OPTIONS:
		lstrcpy(pName, "IP_OPTIONS");
		break;
		
	case ID_TCP_SRC_PORT:
		lstrcpy(pName, "TCP_SRC_PORT");
		wsprintf(pSingleDescr, "%d", *(WORD*) pData);
		*dwType = _PARAM_WORD_;
		break;
	case ID_TCP_DST_PORT:
		lstrcpy(pName, "TCP_DST_PORT");
		wsprintf(pSingleDescr, "%d", *(WORD*) pData);
		*dwType = _PARAM_WORD_;
		break;
	case ID_TCP_SEQ_NUMBER:
		lstrcpy(pName, "TCP_SEQ_NUMBER");
		break;
	case ID_TCP_ASK_SEQ_NUMBER:
		lstrcpy(pName, "TCP_ASK_SEQ_NUMBER");
		break;
	case ID_TCP_DATA_OFFSET:
		lstrcpy(pName, "TCP_DATA_OFFSET");
		break;
	case ID_TCP_URG:
		lstrcpy(pName, "TCP_URG");
		break;
	case ID_TCP_ACK:
		lstrcpy(pName, "TCP_ACK");
		break;
	case ID_TCP_PSH:
		lstrcpy(pName, "TCP_PSH");
		break;
	case ID_TCP_RST:
		lstrcpy(pName, "TCP_RST");
		break;
	case ID_TCP_SYN:
		lstrcpy(pName, "TCP_SYN");
		break;
	case ID_TCP_FIN:
		lstrcpy(pName, "TCP_FIN");
		break;
	case ID_TCP_WIN:
		lstrcpy(pName, "TCP_WIN");
		break;
		
	case ID_UDP_SRC_PORT:
		lstrcpy(pName, "UDP_SRC_PORT");
		wsprintf(pSingleDescr, "%d", *(WORD*) pData);
		*dwType = _PARAM_WORD_;
		break;
	case ID_UDP_DST_PORT:
		lstrcpy(pName, "UDP_DST_PORT");
		wsprintf(pSingleDescr, "%d", *(WORD*) pData);
		*dwType = _PARAM_WORD_;
		break;
	case ID_UDP_TOTAL_LENGTH:
		lstrcpy(pName, "UDP_TOTAL_LENGTH");
		break;
	case ID_ICMP_TYPE:
		lstrcpy(pName, "ICMP_TYPE");
		wsprintf(pSingleDescr, "%#x", *(BYTE*) pData);
		*dwType = _PARAM_BYTE_;
		break;
	case ID_ICMP_CODE:
		lstrcpy(pName, "ICMP_CODE");
		wsprintf(pSingleDescr, "%#x", *(BYTE*) pData);
		*dwType = _PARAM_BYTE_;
		break;
	case ID_ICMP_UNI_CODE:
		lstrcpy(pName, "ID_ICMP_UNI_CODE");
		wsprintf(pSingleDescr, "%#x", *(DWORD*) pData);
		*dwType = _PARAM_DWORD_;
		break;
	case ID_FULL_APP_PATH:
		lstrcpy(pName, "ID_FULL_APP_PATH");
//		lstrcpy(pSingleDescr, (PCHAR) pData);
//		*dwType = _PARAM_STR_;
		wsprintf(pSingleDescr, "%S", (PCHAR) pData);
		*dwType = _PARAM_STR_W;
		break;
	case ID_REMOTE_IP:
		lstrcpy(pName, "ID_REMOTE_IP");
		*dwType = _PARAM_IP4_ADDRESS;
		{
			BYTE *ptr = (BYTE *)pData;
			wsprintf(pSingleDescr, "%d.%d.%d.%d", ptr[3], ptr[2], ptr[1], ptr[0]);
		}
		break;
	case ID_LOCAL_IP:
		lstrcpy(pName, "ID_LOCAL_IP");
		*dwType = _PARAM_IP4_ADDRESS;
		{
			BYTE *ptr = (BYTE *)pData;
			wsprintf(pSingleDescr, "%d.%d.%d.%d", ptr[3], ptr[2], ptr[1], ptr[0]);
		}
		break;
	case ID_REMOTE_PORT:
		lstrcpy(pName, "ID_REMOTE_PORT");
		wsprintf(pSingleDescr, "%u", *(WORD*) pData);
		*dwType = _PARAM_WORD_;
		break;
	case ID_LOCAL_PORT:
		lstrcpy(pName, "ID_LOCAL_PORT");
		wsprintf(pSingleDescr, "%u", *(WORD*) pData);
		*dwType = _PARAM_WORD_;
		break;
	case ID_INITIATOR:
		lstrcpy(pName, "ID_INITIATOR");
		lstrcpy(pSingleDescr, (*(DWORD *) pData ) ? "INCOMING" : "OUTGOING" );
		*dwType = _PARAM_DWORD_;
		break;
	case ID_FILE_CHECKSUM:
		lstrcpy(pName, "ID_FILE_CHECKSUM");
		break;
	case PF_DEFAULT_FILTER:
		lstrcpy(pName, "PF_DEFAULT_FILTER");
		wsprintf(pSingleDescr, "%d", *(DWORD*) pData);
		*dwType = _PARAM_DWORD_;
		break;
	case PF_BLOCK_FILTER:
		lstrcpy(pName, "PF_BLOCK_FILTER");
		wsprintf(pSingleDescr, "%d", *(DWORD*) pData);
		*dwType = _PARAM_DWORD_;
		break;
	case PF_FILTER_USER_ID:
		lstrcpy(pName, "PF_FILTER_USER_ID");
		break;
	case PF_FILTER_ORDER_ID:
		lstrcpy(pName, "PF_FILTER_ORDER_ID");
		wsprintf(pSingleDescr, "%d", *(DWORD*) pData);
		*dwType = _PARAM_DWORD_;
		break;
	case PF_FILTER_PLACEHOLDER:
		lstrcpy(pName, "PF_FILTER_PLACEHOLDER");
		break;
	case PF_TERMINATE_PROCESS_FILTER:
		lstrcpy(pName, "PF_TERMINATE_PROCESS_FILTER");
		break;
	case PACKET_FILTER_WIZARD_DATA:
		lstrcpy(pName, "PACKET_FILTER_WIZARD_DATA");
		break;
	case PACKET_FILTER_NAME:
		lstrcpy(pName, "PACKET_FILTER_NAME");
		lstrcpy(pSingleDescr, (PCHAR) pData);
		*dwType = _PARAM_STR_;
		break;
	case APPLICATION_FILTER_WIZARD_DATA:
		lstrcpy(pName, "APPLICATION_FILTER_WIZARD_DATA");
		break;
	case ID_WATCHDOG_PARAM:
		lstrcpy(pName, "ID_WATCHDOG_PARAM");
		break;
	case ID_PENDING_EVENT_MARK:
		lstrcpy(pName, "ID_PENDING_EVENT_MARK");
		break;

	case FW_ID_PROTOCOL:
		lstrcpy(pName, "FW_ID_PROTOCOL");
		wsprintf(pSingleDescr, "%d", *(BYTE*) pData);
		*dwType = _PARAM_BYTE_;
		break;
	case FW_ID_ICMP_TYPE:
		lstrcpy(pName, "FW_ID_ICMP_TYPE");
		wsprintf(pSingleDescr, "%d", *(BYTE*) pData);
		*dwType = _PARAM_BYTE_;
		break;
	case FW_ID_EVENT_TIME:
		lstrcpy(pName, "FW_ID_EVENT_TIME");
		wsprintf(pSingleDescr, "%I64d", *(__int64*) pData);
		*dwType = _PARAM_QDWORD_;
		break;
	case FW_ID_ICMP_CODE:
		lstrcpy(pName, "FW_ID_ICMP_CODE");
		wsprintf(pSingleDescr, "%d", *(BYTE*) pData);
		*dwType = _PARAM_BYTE_;
		break;
	case FW_ID_STREAM_DIRECTION:
		lstrcpy(pName, "FW_ID_STREAM_DIRECTION");
		wsprintf(pSingleDescr, "%d", *(DWORD*) pData);
		*dwType = _PARAM_DWORD_;
		break;

	case FW_ID_TIME_SECOND:
		lstrcpy(pName, "FW_ID_TIME_SECOND");
		wsprintf(pSingleDescr, "%d", *(BYTE*) pData);
		*dwType = _PARAM_BYTE_;
		break;
	case FW_ID_TIME_MINUTE:
		lstrcpy(pName, "FW_ID_TIME_MINUTE");
		wsprintf(pSingleDescr, "%d", *(BYTE*) pData);
		*dwType = _PARAM_BYTE_;
		break;
	case FW_ID_TIME_HOUR:
		lstrcpy(pName, "FW_ID_TIME_HOUR");
		wsprintf(pSingleDescr, "%d", *(BYTE*) pData);
		*dwType = _PARAM_BYTE_;
		break;
	case FW_ID_TIME_DAY:
		lstrcpy(pName, "FW_ID_TIME_DAY");
		wsprintf(pSingleDescr, "%d", *(BYTE*) pData);
		*dwType = _PARAM_BYTE_;
		break;
	case FW_ID_TIME_WEEK_DAY:
		lstrcpy(pName, "FW_ID_TIME_WEEK_DAY");
		wsprintf(pSingleDescr, "%d", *(BYTE*) pData);
		*dwType = _PARAM_BYTE_;
		break;
	case FW_ID_TIME_MONTH:
		lstrcpy(pName, "FW_ID_TIME_MONTH");
		wsprintf(pSingleDescr, "%d", *(BYTE*) pData);
		*dwType = _PARAM_BYTE_;
		break;
	case FW_ID_TIME_YEAR:
		lstrcpy(pName, "FW_ID_TIME_YEAR");
		wsprintf(pSingleDescr, "%d", *(USHORT*) pData);
		*dwType = _PARAM_WORD_;
		break;
	case FW_ID_TIME_DAY_SECOND:
		lstrcpy(pName, "FW_ID_TIME_DAY_SECOND");
		wsprintf(pSingleDescr, "%d", *(ULONG*) pData);
		*dwType = _PARAM_DWORD_;
		break;
		
	default:
		lstrcpy(pName, "Unknown parameter");
	}

	if (pParam->ParamFlags & _SINGLE_PARAM_FLAG_MAPPED_FILTER_PARAM)
		lstrcat(pSingleDescr, " <- mapped from filter");
}

void GetSingleParameterDescription(PSINGLE_PARAM pSingleParam, CHAR *pstrDescr)
{
	CHAR pName[4096];
	CHAR pSingleDescr[4096];
	DWORD dwType;
	
	GetSingleParameterInfo(pSingleParam, pName, pSingleDescr, &dwType);

	wsprintf(pstrDescr, "%s (%d). Size %d. Value: %s", pName, pSingleParam->ParamID, pSingleParam->ParamSize, pSingleDescr);
}

#ifndef __ONLY_PARAM_DESCR

CHAR *NullStr="\0";
CHAR *UnknownStr="-?-";
WCHAR *UnknownStrW=L"-?-";

#include "..\..\ntcommon.cpp"
#include "..\..\vxdcommn.cpp"

CString GetParamString(PFILTER_TRANSMIT pFilter)
{
	CString result;
	result.Empty();

	return result;
}

CString GetRequestTimeoutString(PFILTER_TRANSMIT pFilter)
{
	CString result, strtmp;
	strtmp = L"Request timeout %u (sec)";
	result.Format(strtmp, pFilter->m_Timeout);
	
	return result;
}

PFILTER_PARAM GetFilterParam(PFILTER_TRANSMIT pFilter, ULONG ParamID)
{
	PCHAR pObjName = NULL;
	PFILTER_PARAM	pSingleParam;
	
	pSingleParam = (PFILTER_PARAM) pFilter->m_Params;
	for (ULONG cou = 0; cou < pFilter->m_ParamsCount; cou++)
	{
		if (pSingleParam->m_ParamID == ParamID)
			return pSingleParam;
		pSingleParam = (PFILTER_PARAM)((BYTE*)pSingleParam + sizeof(FILTER_PARAM) + pSingleParam->m_ParamSize);
		
	}
	
	return NULL;
}

BOOL AddFilterParamValue(PFILTER_TRANSMIT pFilter, PFILTER_PARAM	pNewParam)
{
	PFILTER_PARAM pSingleParam = (PFILTER_PARAM) pFilter->m_Params;
	int size = sizeof(FILTER_TRANSMIT);
	for (DWORD cou = 0; cou < pFilter->m_ParamsCount; cou++)
	{
		size += sizeof(FILTER_PARAM) + pSingleParam->m_ParamSize;
		pSingleParam = (PFILTER_PARAM)((BYTE*)pSingleParam + sizeof(FILTER_PARAM) + pSingleParam->m_ParamSize);
	}

	if ((size + sizeof(FILTER_PARAM) + pNewParam->m_ParamSize) > FILTER_BUFFER_SIZE)
		return FALSE;
	
	pSingleParam->m_ParamFlags = pNewParam->m_ParamFlags;
	pSingleParam->m_ParamFlt = pNewParam->m_ParamFlt;
	pSingleParam->m_ParamID = pNewParam->m_ParamID;
	pSingleParam->m_ParamSize = pNewParam->m_ParamSize;
	CopyMemory(pSingleParam->m_ParamValue, pNewParam->m_ParamValue, pSingleParam->m_ParamSize);
	
	pFilter->m_ParamsCount++;
	
	return TRUE;
}

BOOL DelFilterParamValue(PFILTER_TRANSMIT pFilter, DWORD dwIdx)
{
	BYTE pBuff[FILTER_BUFFER_SIZE];
	PFILTER_TRANSMIT pFilterSave;
	
	pFilterSave = (PFILTER_TRANSMIT) pBuff;
	
	CopyMemory(pFilterSave, pFilter, FILTER_BUFFER_SIZE);
	
	int ParamCount = 0;
	
	PFILTER_PARAM	pSingleParam;
	PFILTER_PARAM	pSingleParamSave;
	pSingleParam = (PFILTER_PARAM) pFilter->m_Params;
	pSingleParamSave = (PFILTER_PARAM) pFilterSave->m_Params;
	
	for (DWORD cou = 0; cou < pFilter->m_ParamsCount; cou++)
	{
		if (cou != dwIdx)
		{
			pSingleParam->m_ParamFlags = pSingleParamSave->m_ParamFlags;
			pSingleParam->m_ParamFlt = pSingleParamSave->m_ParamFlt;
			pSingleParam->m_ParamID = pSingleParamSave->m_ParamID;
			pSingleParam->m_ParamSize = pSingleParamSave->m_ParamSize;
			CopyMemory(pSingleParam->m_ParamValue, pSingleParamSave->m_ParamValue, pSingleParam->m_ParamSize);
			
			pSingleParam = (PFILTER_PARAM)((BYTE*)pSingleParam + sizeof(FILTER_PARAM) + pSingleParam->m_ParamSize);
			ParamCount++;
		}
		pSingleParamSave = (PFILTER_PARAM)((BYTE*)pSingleParamSave + sizeof(FILTER_PARAM) + pSingleParamSave->m_ParamSize);
	}
	
	pFilter->m_ParamsCount = ParamCount;
	
	return TRUE;
}
void GetParameterDescription(PFILTER_PARAM pFilterParam, CString *pstrDescr)
{
	CString strSubDescr;
	CString strSubDescrSize;
	CString strSubDescr2;

	CString Operation;
	CString Flags;

	strSubDescrSize.Format("Size %d", pFilterParam->m_ParamSize);

	switch (pFilterParam->m_ParamFlt)
	{
	case FLT_PARAM_WILDCARD:
		Operation = "Wildcard";
		break;
	case FLT_PARAM_WILDCARDSENS:
		Operation = "Wildcard (case sens)";
		break;
	case FLT_PARAM_EUA:
		Operation = "EUA";
		break;
	case FLT_PARAM_AND:
		Operation = "AND";
		break;
	case FLT_PARAM_ABV:
		Operation = "ABV";
		break;
	case FLT_PARAM_BEL:
		Operation = "BEL";
		break;
	case FLT_PARAM_NOP:
		Operation = "NOP";
		break;
	case FLT_PARAM_MASKUNSENS_LIST:
		Operation = "Mask list";
		break;
	case FLT_PARAM_MASK_LIST:
		Operation = "Mask list (case sens)";
		break;
	case FLT_PARAM_EQU_LIST:
		Operation = "EQU list";
		break;
	case FLT_PARAM_MORE:
		Operation = "Greater";
		break;
	case FLT_PARAM_LESS:
		Operation = "Less";
		break;
	case FLT_PARAM_MASK:
		Operation = "Mask";
		break;
	default:
		Operation = "unknown";
	}


	Flags.Empty();
	if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_CACHABLE)
		Flags += "Cachable ";
	if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_INVERS_OP)
		Flags += "Inverse result ";
	if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_CACHEUPREG)
		Flags += "UPReg for calc CacheID ";
	if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_MUSTEXIST)
		Flags += "Must exist";
	
	if (Flags.IsEmpty())
		Flags = "<No flags>";


	CHAR pName[4096];
	CHAR pSingleDescr[4096];
	DWORD dwType;
	GetSingleParameterInfo(pFilterParam, pName, pSingleDescr, &dwType);

	pstrDescr->Format("%s (%d) Operation: %s on -> '%s' with flags '%s'", pName, pFilterParam->m_ParamID, Operation, pSingleDescr, Flags);
}
	
void GetFilterDesciptions(PFILTER_TRANSMIT pFilter, CString *pstrDescr)
{
	CString strSubDescr;
	CString strParam;

	DWORD dwIdx;
	CHAR* pchFunc = FillFuncList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, &dwIdx);
	CHAR* pchMajFunc = FillMajList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, &dwIdx);
	CHAR* pchMinFunc = FillMinList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, &dwIdx);

	PFILTER_PARAM pParam = GetFilterParam(pFilter, _PARAM_OBJECT_URL);
	PCHAR pObjName = NULL;
	if (pParam != NULL)
		pObjName = (PCHAR) pParam->m_ParamValue;
	if (pObjName == NULL)
		pObjName = "";
	
	strSubDescr.Empty();
	if (pchFunc != NULL)
	{
		strSubDescr = pchFunc;
	}
	
	if (pchMajFunc == NULL)
		pchMajFunc = "<all mj>";

	strSubDescr += ".";
	strSubDescr += pchMajFunc;
	
	if (pchMinFunc == NULL)
		pchMinFunc = "<all mi>";
	
	strSubDescr += ".";
	strSubDescr += pchMinFunc;

	
	strParam = GetParamString(pFilter);
	if (!strParam.IsEmpty())
		strSubDescr += "(" + strParam + ")";
	
	
	CString Decision = " - allow";
	if (pFilter->m_Flags & FLT_A_DENY)
		Decision = " - deny";
	if (pFilter->m_Flags & FLT_A_POPUP)
		Decision = " - prompt";
	
	pstrDescr->Format("%s for object '%s' by process '%s'%s", strSubDescr, 
		(lstrcmp(pObjName, "*") == 0) ? "<all>": pObjName, 
		(lstrcmp(pFilter->m_ProcName, "*") == 0) ? "<all>": pFilter->m_ProcName, Decision);
	
	if (pFilter->m_Flags & FLT_A_INFO)
		*pstrDescr += " -- (info)";
	if (pFilter->m_Flags & FLT_A_NOTIFY)
		*pstrDescr += " -- (notify)";

	if (pFilter->m_Flags & FLT_A_DISABLED)
		*pstrDescr += " DISABLED";
	
	if(pFilter->m_Expiration)
	{
		SYSTEMTIME ExpirST;
		FILETIME LocalT;

		CHAR Date[64];
		CHAR Time[64];

		CString strtmp;
		
		FileTimeToLocalFileTime((FILETIME*)&(pFilter->m_Expiration), &LocalT);
		FileTimeToSystemTime(&LocalT, &ExpirST);
		GetDateFormat(LOCALE_USER_DEFAULT,DATE_LONGDATE,&ExpirST,NULL,Date,sizeof(Date));
		GetTimeFormat(LOCALE_USER_DEFAULT,LOCALE_NOUSEROVERRIDE,&ExpirST,NULL,Time,sizeof(Time));
		strtmp.Format("(Will be expired %s at %s)", Date, Time);
		
		*pstrDescr += strtmp;
	}
}
// -----------------------------------------------------------------------------------------
CHAR* FillFunctionsList(CStringList *pStringList, CHAR  **FuncNameArr,DWORD FuncNameArrSize, DWORD CurrentPos, CHAR* pstrFun, DWORD* Idx)
{
	if (FuncNameArr == NULL)
		return NULL;
	if (FuncNameArrSize == 0)
		return NULL;

	DWORD i = 0;
	DWORD Pos = 0;
	CString strtmp;
	if(FuncNameArr)
	{
		if (CurrentPos == 0)
			while (lstrcmp(FuncNameArr[i++], NullStr) == 0)
				CurrentPos++;
		for(i = 0;i<FuncNameArrSize;i++)
		{
			if (lstrcmp(FuncNameArr[i], NullStr) != 0) // hide empty string
			{
				strtmp = FuncNameArr[i];
				if (pStringList != NULL)
				{
//					if (!strtmp.IsEmpty())
						pStringList->AddTail(strtmp);
				}
				else
				{
					if (CurrentPos != -1)
					{
						if (CurrentPos == Pos)
						{	
							*Idx = i;
							return FuncNameArr[i];
						}
					}
					else if (lstrcmp(FuncNameArr[i], pstrFun) == 0)
					{
						*Idx = i;
						TRACE("Generated index for function %s is %d\n", pstrFun, *Idx);
					}
				}
			}
			Pos++;
		}
	}
	return NULL;
}

CHAR* FillFuncList(CStringList *pStringList, DWORD Func, DWORD FuncMj, DWORD FuncMi, CHAR* pstrFun, DWORD* Idx)
{
	DWORD FuncNameArrSize;
	CHAR  **FuncNameArr;
	if(g_OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
		FuncNameArrSize=SIZEOF(NTFunctionTypeName);
		FuncNameArr=NTFunctionTypeName;
	} else {
		FuncNameArrSize=SIZEOF(VxDFunctionTypeName);
		FuncNameArr=VxDFunctionTypeName;
	}
	if (pstrFun != NULL)
		return FillFunctionsList(pStringList, FuncNameArr, FuncNameArrSize, -1, pstrFun, Idx);
	return FillFunctionsList(pStringList, FuncNameArr, FuncNameArrSize, Func, NULL, Idx);
}

CHAR *KlickFuncName[] = {
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	"Packet in",
	"Packet out"
};

CHAR *KlinFuncName[] = {
	NullStr,
	"Connect",
	"Disconnect",
	"Receive",
	"Send",
	"Open port",
	"Close port",
	"Listen",
	"UDP send",
	"UDP receive"
};


CHAR* FillMajList(CStringList *pStringList, DWORD Func, DWORD FuncMj, DWORD FuncMi, CHAR* pstrFun, DWORD* Idx)
{
	DWORD FuncNameArrSize;
	CHAR  **FuncNameArr=NULL;

	if(g_OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
		switch(Func) {
		case FLTTYPE_KLPF:
			FuncNameArrSize=SIZEOF(NTFunctionKLPF);
			FuncNameArr=NTFunctionKLPF;
			break;
		case FLTTYPE_SYSTEM:
			FuncNameArrSize=SIZEOF(NTSystem);
			FuncNameArr=NTSystem;
			break;
		case FLTTYPE_PID:
			FuncNameArrSize=SIZEOF(NTFunctionPid);
			FuncNameArr=NTFunctionPid;
			break;
		case FLTTYPE_FIOR:
			FuncNameArrSize=SIZEOF(NTFastIOFunctionName);
			FuncNameArr=NTFastIOFunctionName;
			break;
		case FLTTYPE_NFIOR:
			FuncNameArrSize=SIZEOF(NTMajorFunctionName);
			FuncNameArr=NTMajorFunctionName;
			break;
		case FLTTYPE_DISK:
			FuncNameArrSize=SIZEOF(NTMajorFunctionName)-1;
			FuncNameArr=NTMajorFunctionName;
			break;
		case FLTTYPE_REGS:
			FuncNameArrSize=SIZEOF(NTRegistryFunctionName);
			FuncNameArr=NTRegistryFunctionName;
			break;
		case FLTTYPE_R3:
			FuncNameArrSize=SIZEOF(NTR3FuncName);
			FuncNameArr=NTR3FuncName;
			break;
		case FLTTYPE_KLICK:
			FuncNameArrSize=SIZEOF(KlickFuncName);
			FuncNameArr=KlickFuncName;
			break;
		case FLTTYPE_KLIN:
			FuncNameArrSize=SIZEOF(KlinFuncName);
			FuncNameArr=KlinFuncName;
			break;
			//		default:
//			FuncNameArr=NULL;
		}
	} else {
		switch(Func) {
		case FLTTYPE_KLPF:
			FuncNameArrSize=SIZEOF(VxDFunctionKLPF);
			FuncNameArr=VxDFunctionKLPF;
			break;
		case FLTTYPE_IOS:
			FuncNameArrSize=SIZEOF(VxDIOSCommandName);
			FuncNameArr=VxDIOSCommandName;
			break;
		case FLTTYPE_IFS:
			FuncNameArrSize=SIZEOF(VxDIFSFuncName);
			FuncNameArr=VxDIFSFuncName;
			break;
		case FLTTYPE_REGS:
			FuncNameArrSize=SIZEOF(VxDRegistryFunctionName);
			FuncNameArr=VxDRegistryFunctionName;
			break;
		case FLTTYPE_I21:
			FuncNameArrSize=SIZEOF(VxDI21FuncName);
			FuncNameArr=VxDI21FuncName;
			break;
		case FLTTYPE_R3:
			FuncNameArrSize=SIZEOF(VxDR3FuncName);
			FuncNameArr=VxDR3FuncName;
			break;
		case FLTTYPE_PID:
			FuncNameArrSize=SIZEOF(VxDFunctionPid);
			FuncNameArr=VxDFunctionPid;
			break;
//		default:
//			FuncNameArr=NULL;
		}
	}
	if (pstrFun != NULL)
		return FillFunctionsList(pStringList, FuncNameArr,FuncNameArrSize,-1, pstrFun, Idx);
	return FillFunctionsList(pStringList, FuncNameArr,FuncNameArrSize, FuncMj, pstrFun, Idx);
}

CHAR* FillMinList(CStringList *pStringList, DWORD Func, DWORD FuncMj, DWORD FuncMi, CHAR* pstrFun, DWORD* Idx)
{
	DWORD FuncNameArrSize;
	CHAR  **FuncNameArr=NULL;;
	DWORD MjFn= FuncMj;
	if(g_OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
		switch(Func) {
		case FLTTYPE_NFIOR:
		case FLTTYPE_DISK:
			switch(MjFn) {
			case IRP_MJ_READ:
			case IRP_MJ_WRITE:
				FuncNameArrSize=SIZEOF(NTMinorFunction4RW);
				FuncNameArr=NTMinorFunction4RW;
				break;
			case IRP_MJ_QUERY_INFORMATION:
			case IRP_MJ_SET_INFORMATION:
				FuncNameArrSize=SIZEOF(NTFileInformation);
				FuncNameArr=NTFileInformation;
				break;
			case IRP_MJ_QUERY_VOLUME_INFORMATION:
			case IRP_MJ_SET_VOLUME_INFORMATION:
				FuncNameArrSize=SIZEOF(NTVolumeInformation);
				FuncNameArr=NTVolumeInformation;
				break;
			case IRP_MJ_DIRECTORY_CONTROL:
				FuncNameArrSize=SIZEOF(NTMinorFunction4DirectoryControl);
				FuncNameArr=NTMinorFunction4DirectoryControl;
				break;
			case IRP_MJ_FILE_SYSTEM_CONTROL:
				FuncNameArrSize=SIZEOF(NTMinorFunction4FSControl);
				FuncNameArr=NTMinorFunction4FSControl;
				break;
			case IRP_MJ_DEVICE_CONTROL:
			case IRP_MJ_INTERNAL_DEVICE_CONTROL:
				FuncNameArrSize=SIZEOF(NTDiskDeviceIOCTLName);
				FuncNameArr=NTDiskDeviceIOCTLName;
			case IRP_MJ_LOCK_CONTROL:
				FuncNameArrSize=SIZEOF(NTMinorFunction4LockControl);
				FuncNameArr=NTMinorFunction4LockControl;
				break;
			case IRP_MJ_PNP_POWER:
				FuncNameArrSize=SIZEOF(NTMinorFunction4PNP_Power);
				FuncNameArr=NTMinorFunction4PNP_Power;
				break;
			case IRP_MJ_USER_FSCTL:
				FuncNameArrSize=SIZEOF(NT_FSCTL_Name);
				FuncNameArr=NT_FSCTL_Name;
				break;
//			default:
//				FuncNameArr=NULL;
			}
			break;
		}
	} else {
		switch(Func) {
		case FLTTYPE_IFS:
			switch(MjFn) {
			case IFSFN_CLOSE:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4Close);
				FuncNameArr=VxDIFSFlags4Close;
				break;
			case IFSFN_DIR:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4Dir);
				FuncNameArr=VxDIFSFlags4Dir;
				break;
			case IFSFN_DASDIO:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4DASDIO);
				FuncNameArr=VxDIFSFlags4DASDIO;
				break;
			case IFSFN_FILEATTRIB:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4FileAttr);
				FuncNameArr=VxDIFSFlags4FileAttr;
				break;
			case IFSFN_FILETIMES:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4FileTimes);
				FuncNameArr=VxDIFSFlags4FileTimes;
				break;
			case IFSFN_FILELOCKS:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4FileLock);
				FuncNameArr=VxDIFSFlags4FileLock;
				break;
			case IFSFN_OPEN:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4OpenAccessMode);
				FuncNameArr=VxDIFSFlags4OpenAccessMode;
				break;
			case IFSFN_SEARCH:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4Search);
				FuncNameArr=VxDIFSFlags4Search;
				break;
			case IFSFN_DISCONNECT:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4Disconnect);
				FuncNameArr=VxDIFSFlags4Disconnect;
				break;
			default:
				FuncNameArr=NULL;
			}
			break;
		case FLTTYPE_I21:
			switch(MjFn) {
			case 0x43:
			case 0x57:
				FuncNameArrSize=SIZEOF(VxDI21FAttrAndDateSubFunc);
				FuncNameArr=VxDI21FAttrAndDateSubFunc;
				break;
			case 0x44:
				FuncNameArrSize=SIZEOF(VxDI21IOCTLSubFunc);
				FuncNameArr=VxDI21IOCTLSubFunc;
				break;
			case 0x71:
				FuncNameArrSize=SIZEOF(VxDI21LFNSubFunc);
				FuncNameArr=VxDI21LFNSubFunc;
				break;
			case 0x73:
				FuncNameArrSize=SIZEOF(VxDI2173SubFunc);
				FuncNameArr=VxDI2173SubFunc;
				break;
			default:
				FuncNameArr=NULL;
			}
			break;
//		case FLTTYPE_IOS:
//		case FLTTYPE_REGS:
//		default:
//			FuncNameArr=NULL;
		}
	}
	if (pstrFun != NULL)
		return FillFunctionsList(pStringList, FuncNameArr, FuncNameArrSize, -1, pstrFun, Idx);
	return FillFunctionsList(pStringList, FuncNameArr, FuncNameArrSize, FuncMi, NULL, Idx);
}

#endif	//__ONLY_PARAM_DESCR
