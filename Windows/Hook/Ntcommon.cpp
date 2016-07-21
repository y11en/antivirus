

//Depend order (#define FLTTYPE)
CHAR *NTFunctionTypeName[] = {
	NullStr,
	"Registry functions",
	"Disk Devices",
	"File System request",
	"File System FastIO request",
	"Ring 3 calls",
	"Packets Interceptor",
	NullStr,
	"System",
	"KLPF",
	"IDS",
	"MCHECK",
	"MiniKav",
	NullStr,
	NullStr,
	"Klick",
	"Klin"
};

#ifdef _HOOK_NT_
#ifdef __DBG__
#pragma data_seg(".userd")
#pragma const_seg(".userd")
#endif
#endif
// located in ".user" section
CHAR *NTR3FuncName[]={
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
	"CreateProcessW",
	"LoadLibraryW",
	"LoadLibraryExW",
	"CreateFileW",
	"CopyFileW",
	"CopyFileExW",
	"MoveFileW",
	"MoveFileExW",
	"DeleteFileW",
	"SetFileAttributesW",
	"GetPrivateProfileIntW",
	"GetProfileIntW",
	"WritePrivateProfileSectionW",
	"GetPrivateProfileSectionW",
	"WriteProfileSectionW",
	"GetProfileSectionW",
	"GetPrivateProfileSectionNamesW",
	"WritePrivateProfileStringW",
	"GetPrivateProfileStringW",
	"WriteProfileStringW",
	"GetProfileStringW",
	"WritePrivateProfileStructW",
	"GetPrivateProfileStructW"
};
#ifdef _HOOK_NT_
#ifdef __DBG__
#pragma const_seg()
#pragma data_seg()
#endif
#endif


CHAR *NTMajorFunctionName[] = {
	"CREATE",
	"CREATE_NAMED_PIPE",
	"CLOSE",
	"READ",
	"WRITE",
	"QUERY_INFORMATION",
	"SET_INFORMATION",
	"QUERY_EA",
	"SET_EA",
	"FLUSH_BUFFERS",
	"QUERY_VOLUME_INFORMATION",
	"SET_VOLUME_INFORMATION",
	"DIRECTORY_CONTROL",
	"FILE_SYSTEM_CONTROL",
	"DEVICE_CONTROL",
	"INTERNAL_DEVICE_CONTROL",
	"SHUTDOWN",
	"LOCK_CONTROL",
	"CLEANUP",
	"CREATE_MAILSLOT",
	"QUERY_SECURITY",
	"SET_SECURITY",
	"QUERY_POWER",
	"SET_POWER",
	"DEVICE_CHANGE",
	"QUERY_QUOTA",
	"SET_QUOTA",
	"PNP",
	NullStr,
	"USER_FSCTL"//!!! MUST be last line here!!! This spesial case mapped IRP_MJ_FILE_SYSTEM_CONTROL - IRP_MN_USER_FS_REQUEST
							// index for this value is sizeof(NTMajorFunctionName)/sizeof(CHAR*)-1
};

#define IRP_MJ_USER_FSCTL sizeof(NTMajorFunctionName)/sizeof(CHAR*)-1

CHAR *NTMinorFunction4DirectoryControl[] = {
	NullStr,
	"QUERY_DIRECTORY",
	"NOTIFY_CHANGE_DIRECTORY",
	"QUERY_OLE_DIRECTORY"
};

CHAR *NTMinorFunction4FSControl[] = {
	"USER_FS_REQUEST",
	"MOUNT_VOLUME",
	"VERIFY_VOLUME",
	"LOAD_FILE_SYSTEM",
	"KERNEL_CALL"
};

CHAR *NTMinorFunction4LockControl[] = {
	NullStr,
	"LOCK",
	"UNLOCK_SINGLE",
	"UNLOCK_ALL",
	"UNLOCK_ALL_BY_KEY"
};

CHAR *NTMinorFunction4RW[] = {
	"NORMAL",
	"DPC",
	"MDL",
	"MDL_DPC",
	"COMPLETE",
	NullStr,
	"COMPLETE_MDL",
	"COMPLETE_MDL_DPC",
	"COMPRESSED"
};

CHAR *NTMinorFunction4PNP_Power[] = {
	"START_DEVICE",
	"QUERY_REMOVE_DEVICE",
	"REMOVE_DEVICE",
	"CANCEL_REMOVE_DEVICE",
	"STOP_DEVICE",
	"QUERY_STOP_DEVICE",
	"CANCEL_STOP_DEVICE",
	"QUERY_DEVICE_RELATIONS",      
	"QUERY_INTERFACE",             
	"QUERY_CAPABILITIES",          
	"QUERY_RESOURCES",             
	"QUERY_RESOURCE_REQUIREMENTS", 
	"QUERY_DEVICE_TEXT",           
	"FILTER_RESOURCE_REQUIREMENTS",
	NullStr,
	"READ_CONFIG",                 
	"WRITE_CONFIG",                
	"EJECT",                       
	"SET_LOCK",                    
	"QUERY_ID",                    
	"QUERY_PNP_DEVICE_STATE",      
	"QUERY_BUS_INFORMATION",       
	"DEVICE_USAGE_NOTIFICATION",   
	"SURPRISE_REMOVAL",            
	"QUERY_LEGACY_BUS_INFORMATION",

};


//
// These are the text representations of the classes of IRP_MJ_SET/GET_INFORMATION
// calls
//
CHAR    *NTFileInformation[] = {
    NullStr,
    "FileDirectoryInformation",
    "FileFullDirectoryInformation",
    "FileBothDirectoryInformation",
    "FileBasicInformation",
    "FileStandardInformation",
    "FileInternalInformation",
    "FileEaInformation",
    "FileAccessInformation",
    "FileNameInformation",
    "FileRenameInformation",
    "FileLinkInformation",
    "FileNamesInformation",
    "FileDispositionInformation",
    "FilePositionInformation",
    "FileFullEaInformation",
    "FileModeInformation",
    "FileAlignmentInformation",
    "FileAllInformation",
    "FileAllocationInformation",
    "FileEndOfFileInformation",
    "FileAlternateNameInformation",
    "FileStreamInformation",
    "FilePipeInformation",
    "FilePipeLocalInformation",
    "FilePipeRemoteInformation",
    "FileMailslotQueryInformation",
    "FileMailslotSetInformation",
    "FileCompressionInformation",
    "FileCopyOnWriteInformation",
    "FileCompletionInformation",
    "FileMoveClusterInformation",
    "FileOleClassIdInformation",
    "FileOleStateBitsInformation",
    "FileNetworkOpenInformation",
    "FileObjectIdInformation",
    "FileOleAllInformation",
    "FileOleDirectoryInformation",
    "FileContentIndexInformation",
    "FileInheritContentIndexInformation",
    "FileOleInformation",
    "FileMaximumInformation"
};

//
// These are textual representations of the IRP_MJ_SET/GET_VOLUME_INFORMATION
// classes
//
CHAR *NTVolumeInformation[] = {
    NullStr,
    "FileFsVolumeInformation",
    "FileFsLabelInformation",
    "FileFsSizeInformation",
    "FileFsDeviceInformation",
    "FileFsAttributeInformation",
    "FileFsQuotaQueryInformation",
    "FileFsQuotaSetInformation",
    "FileFsControlQueryInformation",
    "FileFsControlSetInformation",
    "FileFsMaximumInformation"
};

CHAR *NTFastIOFunctionName[] = {
	"FastIoCheckIfPossible",
	"FastIoRead",
	"FastIoWrite",
	"FastIoQueryBasicInfo",
	"FastIoQueryStandardInfo",
	"FastIoLock",
	"FastIoUnlockSingle",
	"FastIoUnlockAll",
	"FastIoUnlockAllByKey",
	"FastIoDeviceControl",
	"AcquireFileForNtCreateSection",
	"ReleaseFileForNtCreateSection",
	"FastIoDetachDevice",
	"FastIoQueryNetworkOpenInfo",
	"AcquireForModWrite",
	"MdlRead",
	"MdlReadComplete",
	"PrepareMdlWrite",
	"MdlWriteComplete",
	"FastIoReadCompressed",
	"FastIoWriteCompressed",
	"MdlReadCompleteCompressed",
	"MdlWriteCompleteCompressed",
	"FastIoQueryOpen",
	"ReleaseForModWrite",
	"AcquireForCcFlush",
	"ReleaseForCcFlush"
};

CHAR *NTRegistryFunctionName[] = {
	"CreateKey",
	"DeleteKey",
	"DeleteValueKey",
	"EnumerateKey",
	"EnumerateValueKey",
	"FlushKey",
	"InitializeRegistry",
	"LoadKey2",
	"NotifyChangeKey",
	"OpenKey",
	"QueryKey",
	"QueryMultipleValueKey",
	"QueryValueKey",
	"ReplaceKey",
	"RestoreKey",
	"SaveKey",
	"SetInformationKey",
	"SetValueKey",
	"UnloadKey"
};

CHAR *NT_FSCTL_Name[]={
	"REQUEST_OPLOCK_LEVEL_1",//0 // -- opportunistic locks
	"REQUEST_OPLOCK_LEVEL_2",// -- opportunistic locks
	"REQUEST_BATCH_OPLOCK",// -- opportunistic locks
	"OPLOCK_BREAK_ACKNOWLEDGE",// -- opportunistic locks
	"OPBATCH_ACK_CLOSE_PENDING",// -- opportunistic locks
	"OPLOCK_BREAK_NOTIFY",// -- opportunistic locks
	"LOCK_VOLUME",// -- volumes
	"UNLOCK_VOLUME",// -- volumes
	"DISMOUNT_VOLUME",// -- volumes
	NullStr,// 9
	"IS_VOLUME_MOUNTED",
	"IS_PATHNAME_VALID",
	"MARK_VOLUME_DIRTY",
	NullStr,// 13
	"QUERY_RETRIEVAL_POINTERS",// --  defragmentation
	"GET_COMPRESSION",// -- compression
	"SET_COMPRESSION",// -- compression
	NullStr,//17
	NullStr,//18
	"MARK_AS_SYSTEM_HIVE",
	"OPLOCK_BREAK_ACK_NO_2",// -- opportunistic locks
	"INVALIDATE_VOLUMES",
	"QUERY_FAT_BPB",
	"REQUEST_FILTER_OPLOCK",// -- opportunistic locks
	"FILESYSTEM_GET_STATISTICS",
	"GET_NTFS_VOLUME_DATA",
	"GET_NTFS_FILE_RECORD",
	"GET_VOLUME_BITMAP",// --  defragmentation
	"GET_RETRIEVAL_POINTERS",// --  defragmentation
	"MOVE_FILE",// --  defragmentation
	"IS_VOLUME_DIRTY",
	"GET_HFS_INFORMATION",//31 decommissioned in XP
	"ALLOW_EXTENDED_DASD_IO",// -- files
	"READ_PROPERTY_DATA",//33 decommissioned in XP
	"WRITE_PROPERTY_DATA",//34 decommissioned in XP
	"FIND_FILES_BY_SID",
	NullStr,//36
	"DUMP_PROPERTY_DATA",//37 decommissioned in XP
	"SET_OBJECT_ID",// -- object identifiers
	"GET_OBJECT_ID",// -- object identifiers
	"DELETE_OBJECT_ID",// -- object identifiers
	"SET_REPARSE_POINT",// -- reparse points
	"GET_REPARSE_POINT",// -- reparse points
	"DELETE_REPARSE_POINT",// -- reparse points
	"ENUM_USN_DATA",// -- change journal
	"SECURITY_ID_CHECK",
	"READ_USN_JOURNAL",// -- change journal
	"SET_OBJECT_ID_EXTENDED",// -- object identifiers
	"CREATE_OR_GET_OBJECT_ID",// -- object identifiers
	"SET_SPARSE",// -- sparse files
	"SET_ZERO_DATA",// -- sparse files
	"QUERY_ALLOCATED_RANGES",// -- sparse files
	"ENABLE_UPGRADE",//52 decommissioned in XP
	"SET_ENCRYPTION",
	"ENCRYPTION_FSCTL_IO",
	"WRITE_RAW_ENCRYPTED",
	"READ_RAW_ENCRYPTED",
	"CREATE_USN_JOURNAL",// -- change journal
	"READ_FILE_USN_DATA",// -- change journal
	"WRITE_USN_CLOSE_RECORD",// -- change journal
	"EXTEND_VOLUME",
	"QUERY_USN_JOURNAL",// -- change journal
	"DELETE_USN_JOURNAL",// -- change journal
	"MARK_HANDLE",// -- change journal
	"SIS_COPYFILE",
	"SIS_LINK_FILES",
	"HSM_MSG",
	"NSS_CONTROL",//67 decommissioned in XP
	"HSM_DATA",
	"RECALL_FILE",// -- files
	"NSS_RCONTROL",//70 decommissioned in XP
	"READ_FROM_PLEX",// -- volumes
	"FILE_PREFETCH"
};


CHAR *NTDiskDeviceIOCTLName[] = {
	"GET_DRIVE_GEOMETRY",
	"GET_PARTITION_INFO",
	"SET_PARTITION_INFO",
	"GET_DRIVE_LAYOUT",
	"SET_DRIVE_LAYOUT",
	"VERIFY",
	"FORMAT_TRACKS",
	"REASSIGN_BLOCKS",
	"PERFORMANCE",
	"IS_WRITABLE",
	"LOGGING",
	"FORMAT_TRACKS_EX",
	"HISTOGRAM_STRUCTURE",
	"HISTOGRAM_DATA",
	"HISTOGRAM_RESET",
	"REQUEST_STRUCTURE",
	"REQUEST_DATA",
	"CONTROLLER_NUMBER",
	"GET_PARTITION_INFO_EX",
	"SET_PARTITION_INFO_EX",
	"GET_DRIVE_LAYOUT_EX",
	"SET_DRIVE_LAYOUT_EX",
	"CREATE_DISK",
	"GET_LENGTH_INFO",
	"PERFORMANCE_OFF",
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	"SMART_GET_VERSION",
	"SMART_SEND_DRIVE_COMMAND",
	"SMART_RCV_DRIVE_DATA",
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	NullStr,
	"GET_DRIVE_GEOMETRY_EX"
	
};

CHAR *NTDiskDeviceRWParamName[] = {
	"SectorNumber"
};

CHAR *NTFunctionPid[] = {
	NullStr,
	"Send packet",
	"Recieve packet",
	"Net attack",
};

CHAR *NTSystem[] = {
	"TerminatePocess",
	"CreateProcess",
	"CreateProcessEx",	
	"OpenProcess",
	"CreateSection",
	"ExitProcess",
	"CreateProcessNotify(ex)",
	"CreateThread"
};

CHAR *NTFunctionKLPF[] = {
	NullStr,
	"Connect",
	"Disconnect",
	"Receive",
	"Send",
	"Open port",
	"Close port",
	"Listen"
};

CHAR *NTFunctionIDS[] = {
	NullStr,
	"Access file",
};

CHAR *NTFunctionMiniKav[] = {
	NullStr,
};

PCHAR vNTGetFunctionStr(PCHAR StrBuff, ULONG Hook_ID, ULONG FunctionMj, ULONG FunctionMi, va_list ParamList)
{
	PCHAR HookStr;
	PCHAR MajStr;
	PCHAR MinStr=NULL;
	PCHAR *ParStrArr=NULL;
	ULONG	ParArrSize=0;
	ULONG Fn,FnMj,FnMin;
	Fn=Hook_ID;
	FnMj=FunctionMj;
	FnMin=FunctionMi;
	__try {
		switch(Fn) {
		case FLTTYPE_MINIKAV:
			HookStr = "MiniKav";
			if(FnMj<sizeof(NTFunctionMiniKav)/sizeof(CHAR*))
				MajStr=NTFunctionMiniKav[FnMj];
			break;
		case FLTTYPE_IDS:
			HookStr = "IDS";
			if(FnMj<sizeof(NTFunctionIDS)/sizeof(CHAR*))
				MajStr=NTFunctionIDS[FnMj];
			break;
		case FLTTYPE_KLPF:
			HookStr = "KLPF";
			if(FnMj<sizeof(NTFunctionKLPF)/sizeof(CHAR*))
				MajStr=NTFunctionKLPF[FnMj];
			break;
		case FLTTYPE_SYSTEM:
			HookStr = "System";
			if(FnMj<sizeof(NTSystem)/sizeof(CHAR*))
				MajStr=NTSystem[FnMj];
			break;
		case FLTTYPE_PID:
			HookStr = "PID";
			if(FnMj<sizeof(NTFunctionPid)/sizeof(CHAR*))
				MajStr=NTFunctionPid[FnMj];
			break;
		case FLTTYPE_FIOR:
			HookStr = "FIOR";
			if(FnMj<sizeof(NTFastIOFunctionName)/sizeof(CHAR*))
				MajStr=NTFastIOFunctionName[FnMj];
			break;
		case FLTTYPE_REGS:
			HookStr = "REG";
			if(FnMj<sizeof(NTRegistryFunctionName)/sizeof(CHAR*))
				MajStr=NTRegistryFunctionName[FnMj];
			break;
		case FLTTYPE_DISK:
		case FLTTYPE_NFIOR:
			if (Fn == FLTTYPE_DISK)	
				HookStr = "DISK";
			else 
				HookStr = "NFIOR";
			if(FnMj<sizeof(NTMajorFunctionName)/sizeof(CHAR*))
				MajStr=NTMajorFunctionName[FnMj];
			switch(FnMj) {
			case IRP_MJ_READ:
			case IRP_MJ_WRITE:
				if(FnMin<sizeof(NTMinorFunction4RW)/sizeof(CHAR*))
					MinStr=NTMinorFunction4RW[FnMin];
				if(Fn==FLTTYPE_DISK) {
					ParStrArr=NTDiskDeviceRWParamName;
					ParArrSize=sizeof(NTDiskDeviceRWParamName)/sizeof(CHAR*);
				}
				break;
			case IRP_MJ_QUERY_INFORMATION:
			case IRP_MJ_SET_INFORMATION:
				if(FnMin<sizeof(NTFileInformation)/sizeof(CHAR*))
					MinStr=NTFileInformation[FnMin];
				break;
			case IRP_MJ_QUERY_VOLUME_INFORMATION:
			case IRP_MJ_SET_VOLUME_INFORMATION:
				if(FnMin<sizeof(NTVolumeInformation)/sizeof(CHAR*))
					MinStr=NTVolumeInformation[FnMin];
				break;
			case IRP_MJ_DIRECTORY_CONTROL:
				if(FnMin<sizeof(NTMinorFunction4DirectoryControl)/sizeof(CHAR*))
					MinStr=NTMinorFunction4DirectoryControl[FnMin];
				break;
			case IRP_MJ_FILE_SYSTEM_CONTROL:
				if(FnMin<sizeof(NTMinorFunction4FSControl)/sizeof(CHAR*))
					MinStr=NTMinorFunction4FSControl[FnMin];
				break;
			case IRP_MJ_LOCK_CONTROL:
				if(FnMin<sizeof(NTMinorFunction4LockControl)/sizeof(CHAR*))
					MinStr=NTMinorFunction4LockControl[FnMin];
				break;
			case IRP_MJ_PNP_POWER:
				if(FnMin<sizeof(NTMinorFunction4PNP_Power)/sizeof(CHAR*))
					MinStr=NTMinorFunction4PNP_Power[FnMin];
				break;
			case IRP_MJ_INTERNAL_DEVICE_CONTROL:
			case IRP_MJ_DEVICE_CONTROL:
				if(FnMin<sizeof(NTDiskDeviceIOCTLName)/sizeof(CHAR*))
					MinStr=NTDiskDeviceIOCTLName[FnMin];
				break;
			case IRP_MJ_USER_FSCTL:
				if(FnMin<sizeof(NT_FSCTL_Name)/sizeof(CHAR*))
					MinStr=NT_FSCTL_Name[FnMin];
			}
			break;
		case FLTTYPE_R3:
			HookStr = "R3";
			MajStr=NullStr;
			if(FnMj<sizeof(NTR3FuncName)/sizeof(CHAR*))
				MajStr=NTR3FuncName[FnMj];
			break;
		case FLTTYPE_FLT:
			HookStr = "FltSystem";
			MajStr=NullStr;
			break;
		default:
			MajStr="\0";
		}
		strcpy(StrBuff,HookStr);
		strcat(StrBuff,": ");
		strcat(StrBuff,MajStr);
		if(MinStr && MinStr!=NullStr) {
			strcat(StrBuff," - ");
			strcat(StrBuff,MinStr);
		} else if(FnMin){
			sprintf(StrBuff+strlen(StrBuff)," - UnkMi(0x%x)",FnMin);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		strcpy(StrBuff,UnknownStr);
	}
	return StrBuff;
}

PCHAR NTGetFunctionStr(PCHAR StrBuff, ULONG Hook_ID, ULONG FunctionMj, ULONG FunctionMi,...)
{
	PCHAR ret;
	va_list ArgPtr;
	
	va_start(ArgPtr,FunctionMi);
	ret=vNTGetFunctionStr(StrBuff, Hook_ID, FunctionMj, FunctionMi,ArgPtr);
	va_end(ArgPtr);
	
	return ret;
}