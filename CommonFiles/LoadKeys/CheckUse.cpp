////////////////////////////////////////////////////////////////////
//
//  CheckUse.cpp
//  AVP key loading and preprocessing (old and new formats)
//  AVP project
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include <time.h>
#include <imagehlp.h>
#include <Stuff\text2bin.h>
#include "CheckUse.h"


BOOL CheckKeyUseTime(AVP_dword* keynumber, AVP_dword life, SYSTEMTIME& stExpir, AVP_dword* PLPIfTrial)
{
	return CheckKeyUseTimeEx(keynumber, life, stExpir, PLPIfTrial, TRUE);
}

void StrConvert( const char *pSrc, TCHAR *pDst, AVP_dword dwDstSize ) {
#if defined(UNICODE) || defined(_UNICODE)
	::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pSrc, -1, pDst, dwDstSize );
#else
	lstrcpyn( pDst, pSrc, dwDstSize );
#endif
}


BOOL CheckKeyUseTimeEx(AVP_dword* keynumber, AVP_dword life, SYSTEMTIME& stExpir, AVP_dword* PLPIfTrial, AVP_bool bRegister)
{
	time_t tt_first;
	time_t tt_now;
	time( &tt_now );
	tt_first=tt_now;

	HKEY hKey;
	DWORD disp;
	char res[0x200];
	TCHAR resW[0x200];
	
	TextToBin("CjaB1L7Tl10",11,res,0x200);

	StrConvert( res, resW, sizeof(resW) );
	if(ERROR_SUCCESS==RegCreateKeyEx(HKEY_CLASSES_ROOT,resW,0,REG_NONE,REG_OPTION_NON_VOLATILE, bRegister ? KEY_ALL_ACCESS : KEY_READ,NULL,&hKey,&disp))
  {
		DWORD type;
    DWORD size;

		if(PLPIfTrial)
			res[BinToText(PLPIfTrial,sizeof(AVP_dword),res,0x200)]=0;
		else
			res[BinToText(keynumber,3*sizeof(AVP_dword),res,0x200)]=0;

		type=REG_BINARY;
		size=sizeof(time_t);

		// VIK 27.03.02 14:10
		// Обход ошибки в билде 3.5.5.4 - выполнялась регистрация с нулевыми полями в имени значения
		// Нулевых значений у нас быть не может - trial-ключи регистрируются с номером позиции PL,
		// а они считаются с 1
		StrConvert( res, resW, sizeof(resW) );
    if( !PLPIfTrial && ERROR_SUCCESS != RegQueryValueEx(hKey,resW,NULL,&type,(LPBYTE)&tt_first,&size)	) {
			char  buff[0x200];
			TCHAR buffW[0x200];
			AVP_dword number[3] = {0};
			time_t ttf_first;
/*

			buff[BinToText(number,3*sizeof(AVP_dword),buff,0x200)]=0;
			type=REG_BINARY;
			size=sizeof(time_t);
			StrConvert( buff, buffW, sizeof(buffW) );
			if( ERROR_SUCCESS == RegQueryValueEx(hKey,buffW,NULL,&type,(LPBYTE)&ttf_first,&size)	) {
				RegDeleteValue(hKey,buff);
				if ( ttf_first < tt_first )
					tt_first = ttf_first;
			}
*/
			number[0] = 0;
			number[1] = 0;
			number[2] = keynumber[2];
			buff[BinToText(number,3*sizeof(AVP_dword),buff,0x200)]=0;
			type=REG_BINARY;
			size=sizeof(time_t);
			StrConvert( buff, buffW, sizeof(buffW) );
			if( ERROR_SUCCESS == RegQueryValueEx(hKey,buffW,NULL,&type,(LPBYTE)&ttf_first,&size)	) {
				RegDeleteValue(hKey,buffW);
				if ( ttf_first < tt_first )
					tt_first = ttf_first;
			}
			number[0] = 0;
			number[1] = keynumber[1];
			number[2] = keynumber[2];
			buff[BinToText(number,3*sizeof(AVP_dword),buff,0x200)]=0;
			type=REG_BINARY;
			size=sizeof(time_t);
			StrConvert( buff, buffW, sizeof(buffW) );
			if( ERROR_SUCCESS == RegQueryValueEx(hKey,buffW,NULL,&type,(LPBYTE)&ttf_first,&size)	) {
				RegDeleteValue(hKey,buffW);
				if ( ttf_first < tt_first )
					tt_first = ttf_first;
			}
			number[0] = keynumber[0];
			number[1] = 0;
			number[2] = keynumber[2];
			type=REG_BINARY;
			buff[BinToText(number,3*sizeof(AVP_dword),buff,0x200)]=0;
			size=sizeof(time_t);
			StrConvert( buff, buffW, sizeof(buffW) );
			if( ERROR_SUCCESS == RegQueryValueEx(hKey,buffW,NULL,&type,(LPBYTE)&ttf_first,&size)	) {
				RegDeleteValue(hKey,buffW);
				if ( ttf_first < tt_first )
					tt_first = ttf_first;
			}
			tt_now = tt_first;
		}

		type=REG_BINARY;
		size=sizeof(time_t);
		if(ERROR_SUCCESS!=RegQueryValueEx(hKey,resW,NULL,&type,(LPBYTE)&tt_first,&size)
			|| tt_first > tt_now)
		{
			tt_first=tt_now;
			RegSetValueEx(hKey,resW,NULL,type,(LPBYTE)&tt_first,size);
		}
			
		RegCloseKey(hKey);
	}
	tt_first+=life*60*60*24;
	if(gmtime(&tt_first)){
		tm tm_first=*gmtime(&tt_first);
		tm_first.tm_year+=1900;
		tm_first.tm_mon+=1;
		if(stExpir.wYear>tm_first.tm_year)
		{
			stExpir.wYear=tm_first.tm_year;
			stExpir.wMonth=tm_first.tm_mon;
			stExpir.wDay=tm_first.tm_mday;
		}
		else if(stExpir.wYear==tm_first.tm_year){
			if(stExpir.wMonth>tm_first.tm_mon)
			{
				stExpir.wMonth=tm_first.tm_mon;
				stExpir.wDay=tm_first.tm_mday;
			}
			else if(stExpir.wMonth==tm_first.tm_mon){
				if(stExpir.wDay>tm_first.tm_mday)
					stExpir.wDay=tm_first.tm_mday;
			}
		}
	}

	BOOL Valid=TRUE;
	SYSTEMTIME stCurrent;
//	GetSystemTime(&stCurrent);
	GetLocalTime(&stCurrent);

	if(stCurrent.wYear > stExpir.wYear) Valid=FALSE;
	else if(stCurrent.wYear < stExpir.wYear) ;
	else if(stCurrent.wMonth > stExpir.wMonth) Valid=FALSE;
	else if(stCurrent.wMonth < stExpir.wMonth) ;
	else if(stCurrent.wDay > stExpir.wDay) Valid=FALSE;

	return Valid;
}

