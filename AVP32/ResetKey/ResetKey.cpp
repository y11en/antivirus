// ResetKey.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <time.h>
#include <Stuff\text2bin.h>


int main(int argc, char* argv[])
{
	HKEY hKey;
	DWORD disp;
	char res[0x200];

	DWORD key[3];
	time_t tt_first;
	tm _tm;
	memset(&_tm,0,sizeof(_tm));
	_tm.tm_mday=30;
	_tm.tm_mon=8;   //0- jan
	_tm.tm_year=99; //0- 1900
	tt_first=mktime(&_tm);

//	 0013-000003-00000143
	key[0]=0x13;
	key[1]=0x3;
	key[2]=0x143;


	TextToBin("CjaB1L7Tl10",11,res,0x200); //LK.Auto

	if(ERROR_SUCCESS==RegCreateKeyEx(HKEY_CLASSES_ROOT,res,0,REG_NONE,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,NULL,&hKey,&disp))
    {
		DWORD type;
        DWORD size;

		res[BinToText(key,3*sizeof(DWORD),res,0x200)]=0;

		type=REG_BINARY;
		size=sizeof(time_t);
		RegSetValueEx(hKey,res,NULL,type,(LPBYTE)&tt_first,size);

		RegCloseKey(hKey);
	}

	return 0;
}
