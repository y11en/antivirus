#include "processmonitor.h"
#include <Mail/plugin/p_httpscan.h>
#include <Mail/structs/s_httpscan.h>
#include <Mail/plugin/p_mctask.h>
#include <minmax.h>
#include <AV/iface/i_engine.h>

#ifdef _WIN32
//#include <windows.h>
#endif

#include "netutil.h"

#include <pshpack1.h>
typedef struct tag_SignedDownloadNotification {
	uint8_t  magic[4];
	uint16_t packet_len;
	uint16_t reserved;
	uint32_t size;
	uint8_t  md5[16];
	uint8_t  signer_name[0x30];
	uint16_t url_len;
	uint8_t  url[0x200];
} tSignedDownloadNotification;
#include <poppack.h>

#define _FIELD_OFFSET(type, field)    ((unsigned long)&(((type *)0)->field))

tERROR CProcMon::OnFileDownload(hIO io, cSerializable* pParams)
{

	static tPROPID s_propMailerPID = 0;
	if (!s_propMailerPID)
		g_root->RegisterCustomPropId(&s_propMailerPID, npMAILER_PID, pTYPE_DWORD | pCUSTOM_HERITABLE);
	if (!s_propMailerPID)
		return errUNEXPECTED;
	tDWORD dwPID = io->propGetDWord(s_propMailerPID);

	static tPROPID s_propMessageIsIncoming = 0;
	if (!s_propMessageIsIncoming)
		g_root->RegisterCustomPropId(&s_propMessageIsIncoming, npMESSAGE_IS_INCOMING, pTYPE_BOOL | pCUSTOM_HERITABLE);
	if (!s_propMessageIsIncoming)
		return errUNEXPECTED;
	tBOOL bIncoming = io->propGetDWord(s_propMessageIsIncoming);

	static tPROPID s_propVirtualName = 0;
	if (!s_propVirtualName)
		g_root->RegisterCustomPropId(&s_propVirtualName, npENGINE_VIRTUAL_OBJECT_NAME, pTYPE_STRING);
	if (!s_propVirtualName)
		return errUNEXPECTED;
	cStringObj sUrl;
	sUrl.assign(io, s_propVirtualName);

	tQWORD qwSize = 0;
	io->GetSize(&qwSize, IO_SIZE_TYPE_EXPLICIT);

	// SKIP packet streams now?
	bool bStream = false;
	if (pParams->isBasedOn(cHTTPScanProcessParams::eIID))
	{
		cHTTPScanProcessParams* pScanProcessParams = (cHTTPScanProcessParams*)pParams;
		bStream = (ss_DONT_USE != pScanProcessParams->m_nCheckAsStream);
	}


	PR_TRACE((this, prtNOTIFY, TR "Process %d %sloaded %S size=%I64d stream=%d", dwPID, bIncoming ? "down" : "up", sUrl.data(), qwSize, bStream));

	if (bStream)
		return errOK;

	// skip too big files
	if (qwSize > 0xFFFFFFFF)
		return errOK;

	// SKIP upload
	if (!bIncoming)
		return errOK;

	// check if service enabled

	// get address to send to
	// TODO: select SDN addresses/ports
	cStringObj sAddr = "91.103.67.144:7022";

	unsigned long address;
	unsigned short port;
	tERROR error = NetUtilParseAddress(sAddr.c_str(cCP_ANSI), 7022, &address, &port);
	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtERROR, TR "parse SDN address %S failed, %terr", sAddr.data(), error));
		return error;
	}

	// check file data (search VeriSign at the end of file)
	tBYTE buff[0x2000];
	tDWORD dwDataSize = 0;
	if (qwSize < sizeof(buff))
		return errEOF;
	error = io->SeekRead(&dwDataSize, qwSize-sizeof(buff), buff, sizeof(buff));
	if (PR_FAIL(error))
		return error;

	if (dwDataSize < 0x80)
		return errEOF;

	// guard
	dwDataSize -= 0x80;
	bool bSigned = false;

	// XCN_OID_PKIX_KP_CODE_SIGNING (1.3.6.1.5.5.7.3.3)
	const unsigned char OID_CODE_SIGNING[] = {0x06, 0x08, 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x03};
	// OID_COMMON_NAME {2.5.4.3}
	const unsigned char OID_COMMON_NAME[] = {0x06, 0x03, 0x55, 0x04, 0x03};
	// SPC_COMMERCIAL_SP_KEY_PURPOSE_OBJID {1.3.6.1.4.1.311.2.1.22}
	const unsigned char SPC_COMMERCIAL_SP_KEY_PURPOSE_OBJID[] = {0x06, 0x0A, 0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x02, 0x01, 0x16};

#define SIGNER_NAME_MAX_LEN 0x80
	unsigned char last_name[SIGNER_NAME_MAX_LEN] = {0};
	unsigned char signer_name[SIGNER_NAME_MAX_LEN] = {0};


	for (size_t i=0; i<dwDataSize; i++)
	{
		if (buff[i] != 0x06)
			continue;
		if (0 == memcmp(buff+i, OID_CODE_SIGNING, sizeof(OID_CODE_SIGNING))
			|| 0 == memcmp(buff+i, SPC_COMMERCIAL_SP_KEY_PURPOSE_OBJID, sizeof(SPC_COMMERCIAL_SP_KEY_PURPOSE_OBJID)) )
		{
			bSigned = true;
			if (*last_name)
				memcpy(signer_name, last_name, sizeof(signer_name));
		}
		if (0 == memcmp(buff+i, OID_COMMON_NAME, sizeof(OID_COMMON_NAME)))
		{
			*last_name = 0;
			i+=5;
			size_t len = buff[i+1];
			if ( i + 2 + len > dwDataSize )
				continue;
			if (len < 2) 
				continue;
			if (0 == memcmp(buff+i+2, "TSA", 3))
				continue;
			if (0 == memcmp(buff+i+2, "Class", 5))
				continue;
			if (buff[i+2+len-2] == 'C' && buff[i+2+len-1] == 'A')
				continue;
			len = min(len, sizeof(signer_name)-1);
			memcpy(last_name, buff+i+2, len);
			last_name[len] = 0;
			if (strstr((const char*)last_name, "tamping "))
				*last_name = 0;
			
		}
	}
	if (!bSigned)
		return errOK;

	PR_TRACE((this, prtNOTIFY, TR "%S signed by %s", sUrl.data(), signer_name[0] ? (const char *)signer_name : "?"));

	tSignedDownloadNotification sdn_packet = {0};
	const tBYTE SDN1_MAGIC[4] = {'S','D','N','1'};
	memcpy(sdn_packet.magic, SDN1_MAGIC, 4);
	sdn_packet.size = NetUtil_htonl((unsigned long)qwSize);
	if (*signer_name)
		memcpy(sdn_packet.signer_name, signer_name, min(sizeof(sdn_packet.signer_name), sizeof(signer_name)));
	error = GetIOHashMD5(io, sdn_packet.md5);
	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtERROR, TR "cannot calc hash for %S, %terr", sUrl.data(), error));
		return error;
	}
	sdn_packet.url[sizeof(sdn_packet.url)-1] = 0; // zero last byte
	NetUtilCopyStrToUtf8(sUrl, sdn_packet.url, sizeof(sdn_packet.url));
	if (sdn_packet.url[sizeof(sdn_packet.url)-1]) // if last byte is filled - 
	{
		PR_TRACE((this, prtERROR, TR "url %S too long for SDN1", sUrl.data()));
		return errNOT_OK;
	}
	tWORD url_len = strlen((const char*)&sdn_packet.url[0])+1;
	sdn_packet.url_len = NetUtil_htons(url_len);
	size_t nPacketLen = _FIELD_OFFSET(tSignedDownloadNotification, url) + url_len;
	if (nPacketLen < sizeof(tSignedDownloadNotification) && (nPacketLen % 2) != 0)
		nPacketLen++; // align 2
	sdn_packet.packet_len = NetUtil_htons(nPacketLen);

	// send data
	error = NetUtilUDPSendReceive(address, port, (const char*)&sdn_packet, nPacketLen, 0, 0, 0, 0);
	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtERROR, TR "SDN send for %S failed, %terr", sUrl.data(), error));
		return error;
	}
	
	PR_TRACE((this, prtNOTIFY, TR "SDN send for %S succeeded, %terr", sUrl.data(), error));
	return errOK;
}