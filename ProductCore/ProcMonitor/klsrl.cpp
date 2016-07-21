#include <Prague/prague.h>
#include "processmonitor.h"
#include "klsrl.h"
#include "netutil.h"

#define WAIT_TIME_ON_TIMEOUT 60

tERROR CProcMon::GetKLSRLConnectParams(unsigned long* paddress, unsigned short* pport, uint32* phid, uint32* pseqn)
{
	if (!paddress || !pport)
		return errPARAMETER_INVALID;
	cAutoCS _lock(m_lock, true);
	// TODO: select KLSRL addresses/ports
	if (m_LastTimeoutTime)
	{
		if ((time_t)cDateTime::now_utc() - m_LastTimeoutTime < WAIT_TIME_ON_TIMEOUT )
			return errNOT_READY;
		m_LastTimeoutTime = 0;
	}

	if (m_CurrentKlsrlAddress == 0)
	{
		// TODO: select KLSRL addresses/ports
		cStringObj sAddress = "91.103.67.144:7024";

		tERROR error = NetUtilParseAddress(sAddress.c_str(cCP_ANSI), 7024, &m_CurrentKlsrlAddress, &m_CurrentKlsrlPort);
		if (PR_FAIL(error))
		{
			PR_TRACE((this, prtERROR, TR "GetKLSRLConnectParams() parse address failed %terr", error));
			return error;
		}
	}
	*paddress = m_CurrentKlsrlAddress;
	*pport = m_CurrentKlsrlPort;
	if (!m_hid)
	{
		// generate hid
		char buff[64]={0};
		int len = 0, i;

		m_hid = 0x12345678;
		if (!gethostname(buff, sizeof(buff)))
			len = strlen(buff);
		for(i=0; i<len; i++) 
		{
			m_hid += buff[i];
			m_hid = (m_hid << 7) | (m_hid >> 25);
			m_hid *= m_hid;
		}
		m_seqn = m_hid ^ cDateTime();
	}
	if (phid)
		*phid = m_hid;
	m_seqn++;
	if (pseqn)
		*pseqn = m_seqn;
	return errOK;
}

static const uint32_t crc16_table[256] = {
	0x00000000,0x00001021,0x00002042,0x00003063,
	0x00004084,0x000050a5,0x000060c6,0x000070e7,
	0x00008108,0x00009129,0x0000a14a,0x0000b16b,
	0x0000c18c,0x0000d1ad,0x0000e1ce,0x0000f1ef,
	0x00001231,0x00000210,0x00003273,0x00002252,
	0x000052b5,0x00004294,0x000072f7,0x000062d6,
	0x00009339,0x00008318,0x0000b37b,0x0000a35a,
	0x0000d3bd,0x0000c39c,0x0000f3ff,0x0000e3de,
	0x00002462,0x00003443,0x00000420,0x00001401,
	0x000064e6,0x000074c7,0x000044a4,0x00005485,
	0x0000a56a,0x0000b54b,0x00008528,0x00009509,
	0x0000e5ee,0x0000f5cf,0x0000c5ac,0x0000d58d,
	0x00003653,0x00002672,0x00001611,0x00000630,
	0x000076d7,0x000066f6,0x00005695,0x000046b4,
	0x0000b75b,0x0000a77a,0x00009719,0x00008738,
	0x0000f7df,0x0000e7fe,0x0000d79d,0x0000c7bc,
	0x000048c4,0x000058e5,0x00006886,0x000078a7,
	0x00000840,0x00001861,0x00002802,0x00003823,
	0x0000c9cc,0x0000d9ed,0x0000e98e,0x0000f9af,
	0x00008948,0x00009969,0x0000a90a,0x0000b92b,
	0x00005af5,0x00004ad4,0x00007ab7,0x00006a96,
	0x00001a71,0x00000a50,0x00003a33,0x00002a12,
	0x0000dbfd,0x0000cbdc,0x0000fbbf,0x0000eb9e,
	0x00009b79,0x00008b58,0x0000bb3b,0x0000ab1a,
	0x00006ca6,0x00007c87,0x00004ce4,0x00005cc5,
	0x00002c22,0x00003c03,0x00000c60,0x00001c41,
	0x0000edae,0x0000fd8f,0x0000cdec,0x0000ddcd,
	0x0000ad2a,0x0000bd0b,0x00008d68,0x00009d49,
	0x00007e97,0x00006eb6,0x00005ed5,0x00004ef4,
	0x00003e13,0x00002e32,0x00001e51,0x00000e70,
	0x0000ff9f,0x0000efbe,0x0000dfdd,0x0000cffc,
	0x0000bf1b,0x0000af3a,0x00009f59,0x00008f78,
	0x00009188,0x000081a9,0x0000b1ca,0x0000a1eb,
	0x0000d10c,0x0000c12d,0x0000f14e,0x0000e16f,
	0x00001080,0x000000a1,0x000030c2,0x000020e3,
	0x00005004,0x00004025,0x00007046,0x00006067,
	0x000083b9,0x00009398,0x0000a3fb,0x0000b3da,
	0x0000c33d,0x0000d31c,0x0000e37f,0x0000f35e,
	0x000002b1,0x00001290,0x000022f3,0x000032d2,
	0x00004235,0x00005214,0x00006277,0x00007256,
	0x0000b5ea,0x0000a5cb,0x000095a8,0x00008589,
	0x0000f56e,0x0000e54f,0x0000d52c,0x0000c50d,
	0x000034e2,0x000024c3,0x000014a0,0x00000481,
	0x00007466,0x00006447,0x00005424,0x00004405,
	0x0000a7db,0x0000b7fa,0x00008799,0x000097b8,
	0x0000e75f,0x0000f77e,0x0000c71d,0x0000d73c,
	0x000026d3,0x000036f2,0x00000691,0x000016b0,
	0x00006657,0x00007676,0x00004615,0x00005634,
	0x0000d94c,0x0000c96d,0x0000f90e,0x0000e92f,
	0x000099c8,0x000089e9,0x0000b98a,0x0000a9ab,
	0x00005844,0x00004865,0x00007806,0x00006827,
	0x000018c0,0x000008e1,0x00003882,0x000028a3,
	0x0000cb7d,0x0000db5c,0x0000eb3f,0x0000fb1e,
	0x00008bf9,0x00009bd8,0x0000abbb,0x0000bb9a,
	0x00004a75,0x00005a54,0x00006a37,0x00007a16,
	0x00000af1,0x00001ad0,0x00002ab3,0x00003a92,
	0x0000fd2e,0x0000ed0f,0x0000dd6c,0x0000cd4d,
	0x0000bdaa,0x0000ad8b,0x00009de8,0x00008dc9,
	0x00007c26,0x00006c07,0x00005c64,0x00004c45,
	0x00003ca2,0x00002c83,0x00001ce0,0x00000cc1,
	0x0000ef1f,0x0000ff3e,0x0000cf5d,0x0000df7c,
	0x0000af9b,0x0000bfba,0x00008fd9,0x00009ff8,
	0x00006e17,0x00007e36,0x00004e55,0x00005e74,
	0x00002e93,0x00003eb2,0x00000ed1,0x00001ef0		
};

uint16_t crc16_get(uint8_t *buf, int len)
{
	uint32_t reg = 0xFFFF;

	if(len<=2) return 0;

	while(len-- > 0) reg = (reg<<8) ^ crc16_table[((reg>>8) ^ *buf++) & 0xFFL];
	reg &= 0xFFFF;

	return (uint16_t) reg;
}

tERROR CProcMon::GetKLSRLInfo(cModuleInfoKLSRL* pInfo, sProcMonInternalContext& ctx)
{
	tERROR error;
	if (!pInfo)
		return errPARAMETER_INVALID;
	pInfo->clear();

	if (PR_SUCC(GetCachedFileInfo(pInfo, ctx)))
	{
		PR_TRACE((this, prtNOTIFY, TR "KLSRL retrieved from cache for %S#%d", ctx.m_sFileName.data(), ctx.m_nCurrentFileRevision));
		return errOK;
	}

	error = GetObjectHashMD5(pInfo, ctx);
	if (PR_FAIL(error))
		return pInfo->m_errStatus = error;

	tBYTE* md5 = pInfo->m_HashMD5;

	PR_TRACE((this, prtNOTIFY, TR "GetKLSRLInfo(%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X)", 
		md5[0], md5[1], md5[2], md5[3], md5[4], md5[5], md5[6], md5[7], md5[8], md5[9], md5[10], md5[11], md5[12], md5[13], md5[14], md5[15]));

	uint32_t hid;
	uint32_t seqn;
	unsigned long address;
	unsigned short port;

	// определение настроек
	error = GetKLSRLConnectParams(&address, &port, &hid, &seqn);
	if (PR_FAIL(error))
		return pInfo->m_errStatus = error;

	// build packet
	uint8_t buff[MAX_PKT_SIZE_UDP];
	uint16_t* pcrc = 0;
	struct klsrl_md5_query *req = (struct klsrl_md5_query *)buff;
	int len = sizeof(struct klsrl_md5_query);
	memset(req, 0, len);
	req->hdr.ver = KLSRL_PROTO_VERSION;
	req->hdr.sn  = NetUtil_htonl(seqn); // sequence number
	req->hdr.hid = NetUtil_htonl(hid); // host id
	req->hdr.len = NetUtil_htons(len + sizeof(uint16_t)); // len + crc16
	req->hdr.sr = OP_GET_MD5;
	memcpy(req->md5, md5, 16);
	pcrc = (uint16_t*)(buff+len);
	*pcrc = NetUtil_htons(crc16_get(buff, len)); // calculate crc16
	len += sizeof(uint16_t); // +sizeof(crc16)

	size_t n;
	error = NetUtilUDPSendReceive(address, port, (const char*)buff, len, (char*)buff, sizeof(buff), &n, 1000);
	if (PR_FAIL(error))
		return error;

	// Анализ принятого с сервера сообщения
	error = errOBJECT_DATA_CORRUPTED;
	do
	{
		struct hips_hdr* hdr = (struct hips_hdr*)buff;
		if (n < sizeof(struct hips_hdr))
			break;
		if (hdr->ver != KLSRL_PROTO_VERSION)
			break;
		// fixup fields
		len = NetUtil_ntohs(hdr->len);
		if (n < (unsigned)len)
			break;
		len -= sizeof(uint16_t); // - sizeof(crc16)
		pcrc = (uint16_t*)(buff+len);
		if (crc16_get(buff, len) != NetUtil_ntohs(*pcrc)) // calculate crc16
			break;
		hdr->hid = NetUtil_ntohl(hdr->hid);
		hdr->sn = NetUtil_ntohl(hdr->sn);
		if (hid != hdr->hid || seqn != hdr->sn)
			break;
		switch (hdr->sr)
		{
		case RESP_OP_OK:
			{
				struct klsrl_md5_response* resp = (struct klsrl_md5_response*)buff;
				if (n < sizeof(struct klsrl_md5_response))
					break;
				pInfo->m_nMajorVersion = NetUtil_ntohl(resp->prod_ver_mj);
				uint8_t* pvendor = resp->prod_name + resp->prod_name_len;
				if (len < ((pvendor+1) - buff))
					break;
				if (len < ((pvendor+1+*pvendor) - buff))
					break;
				if (resp->prod_name_len)
					NetUtilAssignStrFromUtf8(pInfo->m_sProduct, resp->prod_name, resp->prod_name_len);
				if (*pvendor)
					NetUtilAssignStrFromUtf8(pInfo->m_sVendor, pvendor+1, *pvendor);
				pInfo->m_vTrustedFlags.resize(sizeof(resp->flags));
				for (size_t i=0; i<sizeof(resp->flags); i++)
					pInfo->m_vTrustedFlags[i] = resp->flags[i];
				uint8_t* fl = pInfo->m_vTrustedFlags.data();
				Now(&pInfo->m_dtQueryTime);
				PR_TRACE((this, prtIMPORTANT, TR "GetKLSRLInfo() successed: %S,%S,%d,%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", 
					pInfo->m_sVendor.data(),
					pInfo->m_sProduct.data(),
					pInfo->m_nMajorVersion,
					fl[0], fl[1], fl[2], fl[3], fl[4], fl[5], fl[6], fl[7], fl[8], fl[9], fl[10], fl[11], fl[12], fl[13], fl[14], fl[15]
				));
				error = errOK;
			}
			break;
		case RESP_OP_NOTOK:
			Now(&pInfo->m_dtQueryTime);
			error = warnFALSE;
			break;
		case RESP_OP_BUSY:
			error = errTIMEOUT;
			break;
		case RESP_OP_CRIT:
		default:
			error = errUNEXPECTED;
			break;
		}
	} while (false);
	

	if (PR_SUCC(error) && PR_SUCC(SetCachedFileInfo(pInfo, ctx))) // cache it
		PR_TRACE((this, prtNOTIFY, TR "KLSRL data stored in cache for %S#%d", ctx.m_sFileName.data(), ctx.m_nCurrentFileRevision));

	if (PR_FAIL(error))
		m_LastTimeoutTime = cDateTime::now_utc();

	pInfo->m_errStatus = error;
	return error;
}