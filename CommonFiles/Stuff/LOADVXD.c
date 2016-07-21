static HANDLE LoadVxd(char* name, HMODULE hinst_)
{
	HANDLE h;
	char* lp;
	char dName[0x200];
	strcpy(dName,"\\\\.\\");
	GetModuleFileName(hinst_,dName+4,0x1FB);
	GetShortPathName(dName+4,dName+4,0x1FB);//!!!
	lp=strrchr(dName,'\\');
	lp=lp?(lp+1):(dName+4);
	strcpy(lp,name);

	CharToOem(dName,dName); //!!!!!!!
	h=CreateFile(dName,
		0, FILE_SHARE_READ|FILE_SHARE_WRITE, 
		NULL,OPEN_EXISTING,FILE_FLAG_DELETE_ON_CLOSE,NULL);
#ifdef _DEBUG
	GetLastError();
#endif
	return h;

}