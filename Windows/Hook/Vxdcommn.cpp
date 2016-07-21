
//Depend order (#define FLTTYPE)
CHAR *VxDFunctionTypeName[] = {
	NullStr,
	"Registry functions",
	"IOS command",
	"IFS request",
	"Int21 FileIO",
	"Ring 3 calls",
	"Packets Interceptor",
	NullStr,
	NullStr,
	"KLPF",
};

CHAR *VxDR3FuncName[]={
	"CreateProcessA",
	"WinExec",
	"LoadModule",
	"LoadLibraryA",
	"LoadLibraryExA",
	"CreateFileA",
	"OpenFile",
	"_lopen",
	"_lcreat",// Для монитора не нужна
	"CopyFileA",
	"CopyFileExA",
	"MoveFileA",
	"MoveFileExA",
	"DeleteFileA",
	"SetFileAttributesA",
	"GetPrivateProfileIntA",
	"GetProfileIntA",
	"WritePrivateProfileSectionA",
	"GetPrivateProfileSectionA",
	"WriteProfileSectionA",
	"GetProfileSectionA",
	"GetPrivateProfileSectionNamesA",
	"WritePrivateProfileStringA",
	"GetPrivateProfileStringA",
	"WriteProfileStringA",
	"GetProfileStringA",
	"WritePrivateProfileStructA",
	"GetPrivateProfileStructA",
	"TerminateProcess",
//	"CreateProcessW",
//	"LoadLibraryW",
//	"LoadLibraryExW",
//	"CreateFileW",
//	"K32_NtCreateFile",
//	"CopyFileW",
//	"CopyFileExW",
//	"MoveFileW",
//	"MoveFileExW",
//	"DeleteFileW",
//	"SetFileAttributesW"
};

CHAR *VxDIFSFuncName[]={
	"READ",
	"WRITE",	
	"FINDNEXT",
	"FCNNEXT",
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	"SEEK",
	"CLOSE",	
	"COMMIT",	
	"FILELOCKS",
	"FILETIMES",
	"PIPEREQUEST",	
	"HANDLEINFO",
	"ENUMHANDLE",
	"FINDCLOSE",
	"FCNCLOSE",
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
	"CONNECT",
	"DELETE",
	"DIR",
	"FILEATTRIB",
	"FLUSH",
	"GETDISKINFO",
	"OPEN",
	"RENAME",
	"SEARCH",
	"QUERY",
	"DISCONNECT",
	"UNCPIPEREQ",
	"IOCTL16DRIVE",
	"GETDISKPARMS",
	"FINDOPEN",
	"DASDIO"
};


CHAR *VxDIFSFlags4Close[]={
	"HANDLE",
	"FOR_PROCESS",
	"FINAL"
};

CHAR *VxDIFSFlags4Dir[]={
	"CREATE",
	"DELETE",
	"CHECK",
	"QUERY83",
	"QUERYLONG"
};

CHAR *VxDIFSFlags4DASDIO[]={
	"ABS_READ_SECTORS",
	"ABS_WRITE_SECTORS",
	"SET_LOCK_CACHE_STATE",
	"SET_DPB_FOR_FORMAT"
};

CHAR *VxDIFSFlags4FileAttr[]={
	"GET_ATTRIBUTES",
	"SET_ATTRIBUTES",
	"GET_ATTRIB_COMP_FILESIZE",
	"SET_ATTRIB_MODIFY_DATETIME",
	"GET_ATTRIB_MODIFY_DATETIME",
	"SET_ATTRIB_LAST_ACCESS_DATETIME",
	"GET_ATTRIB_LAST_ACCESS_DATETIME",
	"SET_ATTRIB_CREATION_DATETIME",
	"GET_ATTRIB_CREATION_DATETIME",
	"GET_ATTRIB_FIRST_CLUSTER"
};

CHAR *VxDIFSFlags4FileTimes[]={
	"GET_MODIFY_DATETIME",
	"SET_MODIFY_DATETIME",
	NullStr,
	NullStr,
	"GET_LAST_ACCESS_DATETIME",
	"SET_LAST_ACCESS_DATETIME",
	"GET_CREATION_DATETIME",
	"SET_CREATION_DATETIME"
};

CHAR *VxDIFSFlags4FileLock[]={
	"LOCK_REGION",
	"UNLOCK_REGION"
};

/*
CHAR *VxDIFSFlags4MountVolume[]={
	"MOUNT",
	"DISMOUNT",
	"VERIFY",
	"UNLOAD",
	"MOUNT_CHILD",
	"MAP_DRIVE",
	"UNMAP_DRIVE"
};
*/

CHAR *VxDIFSFlags4OpenAccessMode[]={
	"READONLY",
	"WRITEONLY",
	"READWRITE",
	"EXECUTE"
};

CHAR *VxDIFSFlags4Search[]={
	"FIRST",
	"NEXT"
};

CHAR *VxDIFSFlags4Disconnect[]={
	"NORMAL",
	"NO_IO",
	"SINGLE"
};

CHAR *VxDRegistryFunctionName[] = {
	"OpenKey",
	"CloseKey",
	"CreateKey",
	"DeleteKey",
	"EnumKey",
	"QueryValue",
	"SetValue",
	"DeleteValue",
	"EnumValue",
	"QueryValueEx",
	"SetValueEx",
	"FlushKey",
	"QueryInfoKey",
	"RemapPreDefKey",
	"CreateDynKey",
	"QueryMultipleValues",
	"QueryInfoKey16",
	"LoadKey",
	"UnLoadKey",
	"SaveKey",
	"ReplaceKey",
	"NotifyChangeKeyValue"
};


CHAR *VxDIOSCommandName[] = {
	"READ",
	"WRITE",
	"VERIFY",
	"CANCEL",
	"WRITEV",
	"MEDIA_CHECK",
	"MEDIA_CHECK_RESET",
	"LOAD_MEDIA",
	"EJECT_MEDIA",
	"LOCK_MEDIA",
	"UNLOCK_MEDIA",
	"REQUEST_SENSE",
	"COMPUTE_GEOM",
	"GEN_IOCTL",
	"FORMAT",
	"SCSI_PASS_THROUGH",
	"CLEAR_QUEUE",
	"DOS_RESET",
	"SCSI_REQUEST",
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
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	"SET_WRITE_STATUS",
	"RESTART_QUEUE",
	"ABORT_QUEUE",
	"SPIN_DOWN",
	"SPIN_UP",
	"FLUSH_DRIVE",
	"FLUSH_DRIVE_AND_DISCARD",
	"FSD_EXT",
	"FLOPTICAL_MODE_SENSE",
	"IDE_PASS_THROUGH",
	"FLOPTICAL_INQUIRY",
	"ENUMERATE_DEVICES",
	"ASYNCHRONOUS_DRIVE_SPINUP"
};




CHAR *VxDIOSRWParamName[] = {
	"SectorNumber"
};

CHAR *VxDIOSIOCTLParamName[] = {
	"BlockDevMinorFunc"
};

CHAR *VxDI21FuncName[] = {
	NullStr,// 0
	NullStr,// 1
	NullStr,// 2
	NullStr,// 3
	NullStr,// 4
	NullStr,// 5
	NullStr,// 6
	NullStr,// 7
	NullStr,// 8
	NullStr,// 9
	NullStr,// A
	NullStr,// B
	NullStr,// C
	NullStr,// D
	NullStr,// E
	"OPEN FCB",// F
	"CLOSE FCB",// 10
	"FINDFIRST FCB",// 11
	NullStr,//"FINDNEXT FCB",// 12
	"DELETE FCB",// 13
	"SEQUENTIAL READ FCB",// 14
	"SEQUENTIAL WRITE FCB",// 15
	"CREATE FCB",// 16
	"RENAME FCB",// 17
	NullStr,// 18
	NullStr,// 19
	NullStr,// 1A
	NullStr,// 1B
	NullStr,// 1C
	NullStr,// 1D
	NullStr,// 1E
	NullStr,// 1F
	NullStr,// 20
	"READ RECORD FCB",// 21
	"WRITE RECORD FCB",// 22
	NullStr,// 23
	NullStr,// 24
	NullStr,// 25
	NullStr,// 26
	"READ BLOCK FCB",// 27
	"WRITE BLOCK FCB",// 28
	NullStr,// 29
	NullStr,// 2A
	NullStr,// 2B
	NullStr,// 2C
	NullStr,// 2D
	NullStr,// 2E
	NullStr,// 2F
	NullStr,// 30
	NullStr,// 31
	NullStr,// 32
	NullStr,// 33
	NullStr,// 34
	NullStr,// 35
	NullStr,// 36
	NullStr,// 37
	NullStr,// 38
	"MKDIR",// 39
	"RMDIR",// 3A
	"CHDIR",// 3B
	"CREATE",// 3C
	"OPEN",// 3D
	"CLOSE",// 3E
	"READ",// 3F
	"WRITE",// 40
	"DELETE",// 41
	"LSEEK",// 42
	"FILE ATTRIBUTES",// 43
	"IOCTL",// 44
	"DUPLICATE FILE HANDLE",// 45
	"FORCE DUPLICATE FILE HANDLE",// 46
	NullStr,// 47
	NullStr,// 48
	NullStr,// 49
	NullStr,// 4A
	"EXEC",// 4B
	NullStr,// 4C
	NullStr,// 4D
	"FINDFIRST",// 4E
	NullStr,//"FINDNEXT",// 4F
	NullStr,// 50
	NullStr,// 51
	NullStr,// 52
	NullStr,// 53
	NullStr,// 54
	NullStr,// 55
	"RENAME",// 56
	"FILE'S DATE-TIME",// 57 See FileAttrib
	NullStr,// 58
	NullStr,// 59
	"CREATE TEMP",// 5A
	"CREATE NEW",// 5B
	"FLOCK",// 5C
	NullStr,// 5D
	NullStr,// 5E
	NullStr,// 5F
	NullStr,// 60
	NullStr,// 61
	NullStr,// 62
	NullStr,// 63
	NullStr,// 64
	NullStr,// 65
	NullStr,// 66
	NullStr,// 67
	"FFLUSH",// 68
	NullStr,// 69
	"COMMIT",// 6A
	NullStr,// 6B
	"EXT OPEN/CREATE",// 6C  //!! ax==6c00
	NullStr,// 6D
	NullStr,// 6E
	NullStr,// 6F
	NullStr,// 70
	"LFN",// 71
	NullStr,// 72
	"FAT32"// 73
};

CHAR *VxDI21FAttrAndDateSubFunc[] = {
	"GET",
	"SET"
};

CHAR *VxDI21IOCTLSubFunc[] = {
	"GET DEV INFO",//0
	"SET DEV INFO",//1
	"READ CHAR DEV",//2
	"WRITE CHAR DEV",//3
	"READ BLOCK DEV",//4
	"WRITE BLOCK DEV",//5
	"GET INPUT STATUS",//6
	"GET OUTPUT STATUS",//7
	"CHECK IF BLOCK DEVICE REMOVABLE",//8
	"CHECK IF BLOCK DEVICE REMOTE",//9
	"CHECK IF HANDLE IS REMOTE",//a
	"SET SHARING RETRY COUNT",//b
	"GENERIC CHARACTER DEVICE REQUEST",//c
	"GENERIC BLOCK DEVICE REQUEST",//d
	"GET LOGICAL DRIVE MAP",//e
	"SET LOGICAL DRIVE MAP",//f
	"QUERY GENERIC IOCTL CAPABILITY (HANDLE)",//10
	"QUERY GENERIC IOCTL CAPABILITY (DRIVE)"//11
};

CHAR *VxDI21LFNSubFunc[] = {
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
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	"MKDIR",//39
	"RMDIR",//3a
	"CHDIR",//3b
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	"DELETE",//41
	NullStr,
	"FILE ATTRIBUTES",//43
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
	"FINDFIRST",//4e
	NullStr,//"FINDNEXT",//4f
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	"RENAME",//56
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
	NullStr,
	"OPEN"//6c
};

CHAR *VxDI2173SubFunc[] = {
	"DRIVE LOCKING GET",
	"DRIVE LOCKING SET",
	"GET EXTENDED DPB",
	"GET EXTENDED FREE SPACE ON DRIVE",
	"SET DPB TO USE FOR FORMATTING",
	"EXTENDED ABSOLUTE DISK READ/WRITE"
};

CHAR *VxDFunctionPid[] = {
	NullStr,
	"Send packet",
	"Recieve packet",
	"Net attack",
};

CHAR *VxDFunctionKLPF[] = {
	NullStr,
	"Connect",
	"Disconnect",
	"Receive",
	"Send",
	"Open port",
	"Close port",
	"Listen"
};

PCHAR vVxDGetFunctionStr(PCHAR StrBuff, ULONG Hook_ID, ULONG FunctionMj, ULONG FunctionMi, va_list ParamList)
{
PCHAR HookStr;
PCHAR MajStr=UnknownStr;
PCHAR MinStr=NULL;
ULONG Fn,FnMj,FnMin;
PCHAR *ParStrArr=NULL;
ULONG	ParArrSize=0;
//ULONG	Val;
//ULONG	i;
	Fn=Hook_ID;
	FnMj=FunctionMj;
	FnMin=FunctionMi;
	StrBuff[0]=0;
	switch(Fn) {
	case FLTTYPE_KLPF:
		HookStr = "KLPF";
		MajStr=VxDFunctionKLPF[FnMj];
		break;
	case FLTTYPE_PID:
		HookStr = "PID";
		if(FnMj<sizeof(VxDFunctionPid)/sizeof(CHAR*))
			MajStr=VxDFunctionPid[FnMj];
		break;
	case FLTTYPE_IOS:
		HookStr = "IOS";
		if(FnMj<sizeof(VxDIOSCommandName)/sizeof(CHAR*))
			MajStr=VxDIOSCommandName[FnMj];
		switch(FnMj) {
		case IOR_READ:
		case IOR_WRITE:
		case IOR_WRITEV:
			ParStrArr=VxDIOSRWParamName;
			ParArrSize=sizeof(VxDIOSRWParamName)/sizeof(CHAR*);
			break;
		case IOR_GEN_IOCTL:
			ParStrArr=VxDIOSIOCTLParamName;
			ParArrSize=sizeof(VxDIOSIOCTLParamName)/sizeof(CHAR*);
			break;
		}
		break;
	case FLTTYPE_REGS:
		HookStr = "REG";
		if(FnMj<sizeof(VxDRegistryFunctionName)/sizeof(CHAR*))
			MajStr=VxDRegistryFunctionName[FnMj];
		break;
	case FLTTYPE_IFS:
		HookStr = "IFS";
		if(FnMj<sizeof(VxDIFSFuncName)/sizeof(CHAR*))
			MajStr=VxDIFSFuncName[FnMj];
		switch(FnMj) {
		case IFSFN_CLOSE:
			if(FnMin<sizeof(VxDIFSFlags4Close)/sizeof(CHAR*))
				MinStr=VxDIFSFlags4Close[FnMin];
			break;
		case IFSFN_DIR:
			if(FnMin<sizeof(VxDIFSFlags4Dir)/sizeof(CHAR*))
				MinStr=VxDIFSFlags4Dir[FnMin];
			break;
		case IFSFN_DASDIO:
			if(FnMin<sizeof(VxDIFSFlags4DASDIO)/sizeof(CHAR*))
				MinStr=VxDIFSFlags4DASDIO[FnMin];
			break;
		case IFSFN_FILEATTRIB:
			if(FnMin<sizeof(VxDIFSFlags4FileAttr)/sizeof(CHAR*))
				MinStr=VxDIFSFlags4FileAttr[FnMin];
			break;
		case IFSFN_FILETIMES:
			if(FnMin<sizeof(VxDIFSFlags4FileTimes)/sizeof(CHAR*))
				MinStr=VxDIFSFlags4FileTimes[FnMin];
			break;
		case IFSFN_FILELOCKS:
			if(FnMin<sizeof(VxDIFSFlags4FileLock)/sizeof(CHAR*))
				MinStr=VxDIFSFlags4FileLock[FnMin];
			break;
		case IFSFN_OPEN:
			if(FnMin<sizeof(VxDIFSFlags4OpenAccessMode)/sizeof(CHAR*))
				MinStr=VxDIFSFlags4OpenAccessMode[FnMin];
			break;
		case IFSFN_SEARCH:
			if(FnMin<sizeof(VxDIFSFlags4Search)/sizeof(CHAR*))
				MinStr=VxDIFSFlags4Search[FnMin];
			break;
		case IFSFN_DISCONNECT:
			if(FnMin<sizeof(VxDIFSFlags4Disconnect)/sizeof(CHAR*))
				MinStr=VxDIFSFlags4Disconnect[FnMin];
			break;
		}
		break;
	case FLTTYPE_I21:
		HookStr = "I21";
		MajStr=NullStr;
		sprintf(StrBuff,"(i21-%02x%02x) ",FnMj,FnMin);
		if(FnMj<sizeof(VxDI21FuncName)/sizeof(CHAR*))
			MajStr=VxDI21FuncName[FnMj];
		switch(FnMj) {
		case 0x43:
		case 0x57:
			if(FnMin<sizeof(VxDI21FAttrAndDateSubFunc)/sizeof(CHAR*))
				MinStr=VxDI21FAttrAndDateSubFunc[FnMin];
			break;
		case 0x44:
			if(FnMin<sizeof(VxDI21IOCTLSubFunc)/sizeof(CHAR*))
				MinStr=VxDI21IOCTLSubFunc[FnMin];
			break;
		case 0x71:
			if(FnMin<sizeof(VxDI21LFNSubFunc)/sizeof(CHAR*))
				MinStr=VxDI21LFNSubFunc[FnMin];
			break;
		case 0x73:
			if(FnMin<sizeof(VxDI2173SubFunc)/sizeof(CHAR*))
				MinStr=VxDI2173SubFunc[FnMin];
			break;
		}
		break;
	case FLTTYPE_R3:
		HookStr = "R3";
		MajStr=NullStr;
		if(FnMj<sizeof(VxDR3FuncName)/sizeof(CHAR*))
			MajStr=VxDR3FuncName[FnMj];
		break;
	case FLTTYPE_FLT:
		HookStr = "FltSystem";
		MajStr=NullStr;
		break;
	default:
		MajStr=NullStr;
	}
	strcat(StrBuff,HookStr);
	strcat(StrBuff,": ");
	strcat(StrBuff,MajStr);
	if(MinStr) {
		strcat(StrBuff," - ");
		strcat(StrBuff,MinStr);
	}
	/*if(ParStrArr) {
		strcat(StrBuff," ( ");
		Val=va_arg(ParamList,ULONG);
		for(i=0;i<ParArrSize;i++) {
			if(Val==SKIP_PARAM || Val==STOP_PARAM ) {
				sprintf(StrBuff+strlen(StrBuff),"%s-Not specified ",ParStrArr[i]);
			} else {
				sprintf(StrBuff+strlen(StrBuff),"%s=%u ",ParStrArr[i],Val);
			}
			if(Val!=(ULONG)-1)
				Val=va_arg(ParamList,ULONG);
		}   
		strcat(StrBuff,") ");
	}*/
	return StrBuff;
}

PCHAR VxDGetFunctionStr(PCHAR StrBuff, ULONG Hook_ID, ULONG FunctionMj, ULONG FunctionMi,...)
{
PCHAR ret;
va_list ArgPtr;
  va_start(ArgPtr,FunctionMi);
  ret=vVxDGetFunctionStr(StrBuff, Hook_ID, FunctionMj, FunctionMi,ArgPtr);
  va_end(ArgPtr);
  return ret;
}
