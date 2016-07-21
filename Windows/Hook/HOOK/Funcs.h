/********************************* NT ***************************************/
#ifndef __FUNC_H
#define __FUNC_H

#ifndef _NTDDK_

#define FILE_DEVICE_DISK                0x00000007


#define IRP_MJ_CREATE                   0x00
#define IRP_MJ_CREATE_NAMED_PIPE        0x01
#define IRP_MJ_CLOSE                    0x02
#define IRP_MJ_READ                     0x03
#define IRP_MJ_WRITE                    0x04
#define IRP_MJ_QUERY_INFORMATION        0x05
#define IRP_MJ_SET_INFORMATION          0x06
#define IRP_MJ_QUERY_EA                 0x07
#define IRP_MJ_SET_EA                   0x08
#define IRP_MJ_FLUSH_BUFFERS            0x09
#define IRP_MJ_QUERY_VOLUME_INFORMATION 0x0a
#define IRP_MJ_SET_VOLUME_INFORMATION   0x0b
#define IRP_MJ_DIRECTORY_CONTROL        0x0c
#define IRP_MJ_FILE_SYSTEM_CONTROL      0x0d
#define IRP_MJ_DEVICE_CONTROL           0x0e
#define IRP_MJ_INTERNAL_DEVICE_CONTROL  0x0f
#define IRP_MJ_SHUTDOWN                 0x10
#define IRP_MJ_LOCK_CONTROL             0x11
#define IRP_MJ_CLEANUP                  0x12
#define IRP_MJ_CREATE_MAILSLOT          0x13
#define IRP_MJ_QUERY_SECURITY           0x14
#define IRP_MJ_SET_SECURITY             0x15
#define IRP_MJ_QUERY_POWER              0x16
#define IRP_MJ_SET_POWER                0x17
#define IRP_MJ_DEVICE_CHANGE            0x18
#define IRP_MJ_QUERY_QUOTA              0x19
#define IRP_MJ_SET_QUOTA                0x1a
#define IRP_MJ_PNP_POWER                0x1b
#define IRP_MJ_MAXIMUM_FUNCTION         0x1b



// -----------------------------------------------------------------------------------------
//
// File system control minor function codes.  Note that "user request" is
// assumed to be zero by both the I/O system and file systems.  Do not change
// this value.
//

#define IRP_MN_USER_FS_REQUEST          0x00
#define IRP_MN_MOUNT_VOLUME             0x01
#define IRP_MN_VERIFY_VOLUME            0x02
#define IRP_MN_LOAD_FILE_SYSTEM         0x03
#define IRP_MN_TRACK_LINK               0x04    // To be obsoleted soon
#define IRP_MN_KERNEL_CALL              0x04

#define IRP_MN_UNMOUNT_VOLUME			0xf1	// for minifilter

// -----------------------------------------------------------------------------------------


#define	IRP_MN_NORMAL				0
#define	IRP_MN_DPC					1
#define	IRP_MN_MDL					2
#define	IRP_MN_MDL_DPC				3
#define	IRP_MN_COMPLETE			4
#define	IRP_MN_COMPLETE_MDL		6
#define	IRP_MN_COMPLETE_MDL_DPC	7
#define	IRP_MN_COMPRESSED			8


#define FastIoCheckIfPossible				0
#define FastIoRead							1
#define FastIoWrite							2
#define FastIoQueryBasicInfo				3
#define FastIoQueryStandardInfo			4
#define FastIoLock							5
#define FastIoUnlockSingle					6
#define FastIoUnlockAll						7
#define FastIoUnlockAllByKey				8
#define FastIoDeviceControl				9
#define AcquireFileForNtCreateSection	10
#define ReleaseFileForNtCreateSection	11
#define FastIoDetachDevice					12
#define FastIoQueryNetworkOpenInfo		13
#define AcquireForModWrite					14
#define MdlRead								15
#define MdlReadComplete						16
#define PrepareMdlWrite						17
#define MdlWriteComplete					18
#define FastIoReadCompressed				19
#define FastIoWriteCompressed				20
#define MdlReadCompleteCompressed		21
#define MdlWriteCompleteCompressed		22
#define FastIoQueryOpen						23
#define	ReleaseForModWrite				24
#define	AcquireForCcFlush					25
#define ReleaseForCcFlush					26


#define FileDirectoryInformation			1
#define FileFullDirectoryInformation	2
#define FileBothDirectoryInformation	3
#define FileBasicInformation				4
#define FileStandardInformation			5
#define FileInternalInformation			6
#define FileEaInformation					7
#define FileAccessInformation				8
#define FileNameInformation				9
#define FileRenameInformation				10
#define FileLinkInformation				11
#define FileNamesInformation				12
#define FileDispositionInformation		13
#define FilePositionInformation			14
#define FileFullEaInformation				15
#define FileModeInformation				16
#define FileAlignmentInformation			17
#define FileAllInformation					18
#define FileAllocationInformation		19
#define FileEndOfFileInformation			20
#define FileAlternateNameInformation	21
#define FileStreamInformation				22
#define FilePipeInformation				23
#define	FilePipeLocalInformation		24
#define	FilePipeRemoteInformation		25
#define FileMailslotQueryInformation	26
#define FileMailslotSetInformation		27
#define FileCompressionInformation		28
#define FileCopyOnWriteInformation		29
#define FileCompletionInformation		30
#define FileMoveClusterInformation		31
#define FileOleClassIdInformation		32
#define FileOleStateBitsInformation		33
#define FileNetworkOpenInformation		34
#define FileObjectIdInformation			35
#define FileOleAllInformation				36
#define FileOleDirectoryInformation		37
#define FileContentIndexInformation		38
#define FileInheritContentIndexInformation	39
#define FileOleInformation					40
#define FileMaximumInformation			41

/*
#define GET_DRIVE_GEOMETRY	0
#define GET_PARTITION_INFO	1
#define SET_PARTITION_INFO	2
#define GET_DRIVE_LAYOUT	3
#define SET_DRIVE_LAYOUT	4
#define VERIFY					5
#define FORMAT_TRACKS		6
#define REASSIGN_BLOCKS		7
#define PERFORMANCE			8
#define IS_WRITABLE			9
#define LOGGING				10
#define FORMAT_TRACKS_EX	11
#define HISTOGRAM_STRUCTURE	12
#define HISTOGRAM_DATA		13
#define HISTOGRAM_RESET		14
#define REQUEST_STRUCTURE	15
#define REQUEST_DATA			16
#define CONTROLLER_NUMBER	17
*/
#endif
/********************************* VxD ***************************************/

/** IFS Function IDs passed to IFSMgr_CallProvider */
#define IFSFN_READ			0		/* read a file */
#define IFSFN_WRITE			1		/* write a file */
#define IFSFN_FINDNEXT		2		/* LFN handle based Find Next */
#define IFSFN_FCNNEXT		3		/* Find Next Change Notify */

#define IFSFN_SEEK			10		/* Seek file handle */
#define IFSFN_CLOSE			11		/* close handle */
#define IFSFN_COMMIT			12		/* commit buffered data for handle*/
#define IFSFN_FILELOCKS		13		/* lock/unlock byte range */
#define IFSFN_FILETIMES		14		/* get/set file modification time */
#define IFSFN_PIPEREQUEST	15		/* named pipe operations */
#define IFSFN_HANDLEINFO	16		/* get/set file information */
#define IFSFN_ENUMHANDLE	17		/* enum file handle information */
#define IFSFN_FINDCLOSE		18		/* LFN find close */
#define IFSFN_FCNCLOSE		19		/* Find Change Notify Close */

#define IFSFN_CONNECT		30		/* connect or mount a resource */
#define IFSFN_DELETE			31		/* file delete */
#define IFSFN_DIR				32		/* directory manipulation */
#define IFSFN_FILEATTRIB	33		/* DOS file attribute manipulation */
#define IFSFN_FLUSH			34		/* flush volume */
#define IFSFN_GETDISKINFO	35		/* query volume free space */
#define IFSFN_OPEN			36		/* open file */
#define IFSFN_RENAME			37		/* rename path */
#define IFSFN_SEARCH			38		/* search for names */
#define IFSFN_QUERY			39		/* query resource info (network only) */
#define IFSFN_DISCONNECT	40		/* disconnect from resource (net only) */
#define IFSFN_UNCPIPEREQ	41		/* UNC path based named pipe operations */
#define IFSFN_IOCTL16DRIVE	42		/* drive based 16 bit IOCTL requests */
#define IFSFN_GETDISKPARMS	43		/* get DPB */
#define IFSFN_FINDOPEN		44		/* open	an LFN file search */
#define IFSFN_DASDIO			45		/* direct volume access */


/** Values for ir_flags for VFN_DASDIO:	*/
#define DIO_ABS_READ_SECTORS		0	/* Absolute disk read */
#define DIO_ABS_WRITE_SECTORS		1	/* Absolute disk write */
#define DIO_SET_LOCK_CACHE_STATE	2	/* Set cache state during volume lock */
#define DIO_SET_DPB_FOR_FORMAT	3	/* Set DPB for format for Fat32 */

#define OpenKeyVxD					0
#define CloseKeyVxD					1
#define CreateKeyVxD					2
#define DeleteKeyVxD					3
#define EnumKeyVxD					4
#define QueryValueVxD				5
#define SetValueVxD					6
#define DeleteValueVxD				7
#define EnumValueVxD					8
#define QueryValueExVxD				9
#define SetValueExVxD				10
#define FlushKeyVxD					11
#define QueryInfoKeyVxD				12
#define RemapPreDefKeyVxD			13
#define CreateDynKeyVxD				14
#define QueryMultipleValuesVxD	15
#define QueryInfoKey16VxD			16
#define LoadKeyVxD					17
#define UnLoadKeyVxD					18
#define SaveKeyVxD					19
#define ReplaceKeyVxD				20
#define NotifyChangeKeyValueVxD	21

#define  IOR_READ						0x00	  
#define  IOR_WRITE					0x01	  
#define  IOR_VERIFY					0x02	  
#define  IOR_CANCEL					0x03	  
#define  IOR_WRITEV					0x04	 
#define  IOR_MEDIA_CHECK			0x05	 
#define  IOR_MEDIA_CHECK_RESET	0x06	  
#define  IOR_LOAD_MEDIA				0x07	 
#define  IOR_EJECT_MEDIA			0x08	  
#define  IOR_LOCK_MEDIA 			0x09	 
#define  IOR_UNLOCK_MEDIA			0x0A	  
#define  IOR_REQUEST_SENSE			0x0B	  
#define  IOR_COMPUTE_GEOM			0x0C	  
#define  IOR_GEN_IOCTL				0x0D	
#define  IOR_FORMAT					0x0E	  
#define  IOR_SCSI_PASS_THROUGH	0x0F
#define  IOR_CLEAR_QUEUE			0x10	
#define  IOR_DOS_RESET				0x11	 
#define  IOR_SCSI_REQUEST 			0x12	  


// -----------------------------------------------------------------------------------------
// transfer packets (PID)
#define	MJ_SEND_PACKET			0x1
#define	MJ_RECEIVE_PACKET		0x2


#define MJ_SYSTEM_TERMINATE_PROCESS			0x00
#define MJ_SYSTEM_CREATE_PROCESS			0x01
#define MJ_SYSTEM_CREATE_PROCESS_EX			0x02
#define MJ_SYSTEM_OPEN_PROCESS				0x03
#define MJ_SYSTEM_CREATE_SECTION			0x04
#define MJ_EXIT_PROCESS						0x05
#define MJ_CREATE_PROCESS_NOTIFY_EXTEND		0x06
#define MJ_SYSTEM_CREATE_THREAD				0x07
#define MJ_SYSTEM_CREATE_REMOTE_THREAD		0x08
#define MJ_SYSTEM_QUERY_INFO				0x09
#define MJ_SYSTEM_TERMINATE_THREAD_NOTIFY	0x0a
#define MJ_SYSTEM_WRITE_PROCESS_MEMORY		0x0b
#define MJ_SYSTEM_LOAD_IMAGE_NOTIFY			0x0c
#define MJ_SYSTEM_SET_THREAD_CONTEXT		0x0d
#define MJ_CREATE_PROCESS_NOTIFY			0x0e
#define MJ_SYSTEM_DUPLICATE_OBJECT			0x0f
#define MJ_SYSTEM_USER_SET_WINDOWS_HOOK_EX	0x10
#define MJ_SYSTEM_USER_POST_MESSAGE			0x11
#define MJ_SYSTEM_SYSENTER_CHANGED			0x12
#define MJ_SYSTEM_ACCESS_PHYSICAL_MEM		0x13
#define MJ_SYSTEM_KEYLOGGER_DETECTED		0x14
#define MJ_SYSTEM_SENDINPUT					0x15
#define MJ_SYSTEM_SETREGSECURITY			0x16
#define MJ_SYSTEM_INTERPROC_MESSAGE			0x17
#define MJ_SYSTEM_FINDWINDOW				0x18
#define MJ_SYSTEM_USER_SETWINEVENTHOOK		0x19
#define MJ_SYSTEM_LOADDRIVER				0x1a
#define MJ_SYSTEM_BITBLT					0x1b
#define MJ_SYSTEM_DEBUGCONTROL				0x1c
#define MJ_SYSTEM_SETFOSECURITY				0x1d
#define MJ_SYSTEM_HIPS						0x1e
#define MJ_SYSTEM_LOADANDCALL				0x1f
#define MJ_SYSTEM_VIRT						0x20
#define MJ_SYSTEM_KLFLTDEV					0x21
#define MJ_SYSTEM_RASDIAL					0x22
#define MJ_SYSTEM_PROTECTEDSTORAGE			0x23
#define MJ_SYSTEM_RAWINPUTDEVICES			0x24
#define MJ_SYSTEM_SHUTDOWN					0x25
#define MJ_SYSTEM_VOLUME_ACCESS				0x26
#define MJ_SYSTEM_GET_FILESECTORS			0x27

#define MI_SYSTEM_HIPS_ASK					0x00
#define MI_SYSTEM_HIPS_LOG					0x01

#define MI_SYSTEM_VIRT_ASK					0x00
#define MI_SYSTEM_VIRT_LOG					0x01

// registry functions

#define Interceptor_CreateKey					0
#define Interceptor_DeleteKey					1
#define Interceptor_DeleteValueKey			2
#define Interceptor_EnumerateKey				3
#define Interceptor_EnumerateValueKey		4
#define Interceptor_FlushKey					5
#define Interceptor_InitializeRegistry		6
#define Interceptor_LoadKey2					7
#define Interceptor_NotifyChangeKey			8
#define Interceptor_OpenKey					9
#define Interceptor_QueryKey					10
#define Interceptor_QueryMultipleValueKey	11
#define Interceptor_QueryValueKey			12
#define Interceptor_ReplaceKey				13
#define Interceptor_RestoreKey				14
#define Interceptor_SaveKey					15
#define Interceptor_SetInformationKey		16
#define Interceptor_SetValueKey				17
#define Interceptor_UnloadKey				18
#define Interceptor_RenameKey				19

#define R3_ACTIVEDESKTOP_SETWALLPAPER		0x80
#define R3_BACKGROUNDCOPYJOB_RESUME			0x81
#define R3_SHOWHTMLDIALOG					0x82
#define R3_REGISTERINPUTDEVICES				0x83

#endif //__FUNC_H
