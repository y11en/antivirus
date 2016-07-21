#include "stdafx.h"
#include "DDX_Hex.h"



void AFXAPI DDX_TextToBuffer( CDataExchange* pDX, int nIDC, BYTE* buf, int len )
{
	CString string;
	if(!pDX->m_bSaveAndValidate){
		char buffer[6];
		for (int i=0;i<len;i++){
			sprintf(buffer,"%02X",buf[i]);
			string+=buffer;
			if(i!=(len-1))string+=" ";
		}
	}

	DDX_Text(pDX, nIDC, string);

	if(pDX->m_bSaveAndValidate){
		char* stop;
		char* s=(char*)(const char*)string;
		for (int i=0;i<len;i++, s+=3)	buf[i]=(BYTE)strtoul(s,&stop,16);
	}
}

void AFXAPI DDX_TextToBYTE( CDataExchange* pDX, int nIDC, BYTE& data)
{
	CString string;
	if(!pDX->m_bSaveAndValidate){
		sprintf(string.GetBuffer(16),"%02X",data);
		string.ReleaseBuffer();
	}

	DDX_Text(pDX, nIDC, string);

	if(pDX->m_bSaveAndValidate){
		char* stop=(char*)(const char*)string;
		data=(BYTE)strtoul(stop,&stop,16);
	}
}
void AFXAPI DDX_TextToWORD( CDataExchange* pDX, int nIDC, WORD& data)
{
	CString string;
	if(!pDX->m_bSaveAndValidate){
		sprintf(string.GetBuffer(16),"%04X",data);
		string.ReleaseBuffer();
	}

	DDX_Text(pDX, nIDC, string);

	if(pDX->m_bSaveAndValidate){
		char* stop=(char*)(const char*)string;
		data=(WORD)strtoul(stop,&stop,16);
	}
}
void AFXAPI DDX_TextToDWORD( CDataExchange* pDX, int nIDC, DWORD& data)
{
	CString string;
	if(!pDX->m_bSaveAndValidate){
		sprintf(string.GetBuffer(16),"%08X",data);
		string.ReleaseBuffer();
	}

	DDX_Text(pDX, nIDC, string);

	if(pDX->m_bSaveAndValidate){
		char* stop=(char*)(const char*)string;
		data=(DWORD)strtoul(stop,&stop,16);
	}
}
void AFXAPI DDX_TextToSignedWORD( CDataExchange* pDX, int nIDC, WORD& data)
{
	CString string;
	if(!pDX->m_bSaveAndValidate){
		if(data >= 0x8000)
			sprintf(string.GetBuffer(16),"-%04X", -(signed short)data);
		else
			sprintf(string.GetBuffer(16),"%04X",data);

		string.ReleaseBuffer();
	}

	DDX_Text(pDX, nIDC, string);

	if(pDX->m_bSaveAndValidate){
		char* stop=(char*)(const char*)string;
		data=(WORD)strtol(stop,&stop,16);
	}
}
void AFXAPI DDX_TextToSignedBYTE( CDataExchange* pDX, int nIDC, BYTE& data)
{
	CString string;
	if(!pDX->m_bSaveAndValidate){
		if(data >= 0x80)
			sprintf(string.GetBuffer(16),"-%02X", -(signed char)data);
		else
			sprintf(string.GetBuffer(16),"%02X",data);

		string.ReleaseBuffer();
	}

	DDX_Text(pDX, nIDC, string);

	if(pDX->m_bSaveAndValidate){
		char* stop=(char*)(const char*)string;
		data=(BYTE)strtol(stop,&stop,16);
	}
}

