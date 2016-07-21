#ifndef _HOOK_SPEC
#define _HOOK_SPEC

//#define FLTTYPE			0xff000000
#define FLTTYPE_EMPTY		0x00000000
#define FLTTYPE_REGS		0x00000001		// Filter for Registry
#define FLTTYPE_DISK		0x00000002		// Filter for disk device (NT)
#define FLTTYPE_IOS			FLTTYPE_DISK	// Filter for IOS commands (W9x)
#define FLTTYPE_NFIOR		0x00000003		// Filter for NotFastIO FS operations (NT)
#define FLTTYPE_IFS			FLTTYPE_NFIOR	// Filter for IFSHook (W9x)
#define FLTTYPE_FIOR		0x00000004		// Filter for FastIO (NT)
#define FLTTYPE_I21			FLTTYPE_FIOR	// Filter for Int21 (W9x)
#define FLTTYPE_R3 			0x00000005  	// Filter for Ring3
#define FLTTYPE_PID 		0x00000006		// Filter for Packet Interceptor
//#define FLTTYPE_TDIPF     0x00000007     // Filter for Attach driver
#define FLTTYPE_SYSTEM		0x00000008     // Other system routines
#define FLTTYPE_KLPF		0x00000009     // Filter for Attach driver
#define FLTTYPE_IDS			0x0000000a     // Filter for IDS system
#define FLTTYPE_MCHECK		0x0000000b		//Filter for MailChecker driver
#define FLTTYPE_MINIKAV		0x0000000c		//Mini Filter
#define FLTTYPE_PROPROT		0x0000000d		//Process protection
#define FLTTYPE_FLT			0x0000000e		// for unitest - custom filters
#define FLTTYPE_KLICK		0x0000000f		// 
#define FLTTYPE_KLIN		0x00000010		// 
#define FLTTYPE_CKAH_IDS	0x00000011		//


#define HOOK_MAX_ID			17

#define CALLFROMSTACKINDEX	999
#define CALLFROMDATAINDEX	998

// ИДЕНТИФИКАТОРЫ параметров
// длина параметра зависит от вида перехватчика и операции
#define _PARAM_OBJECT_URL				0		// имя объекта над которым выполняется операция
#define _PARAM_OBJECT_BINARYDATA		1		// бинарный набор данных
#define _PARAM_OBJECT_URL_DEST			2		// новое имя объекта (для операции типа rename и move)
#define _PARAM_OBJECT_ACCESSATTR		3		// атрибуты досупа к объекту (типа DesiredAccess - ULONG). 
#define _PARAM_OBJECT_DATALEN			4		// длина данных
#define _PARAM_OBJECT_SECTOR			5		// сектора
#define _PARAM_OBJECT_ORIGINAL_PACKET	6		// see ORIGINAL_PACKET struct
#define _PARAM_OBJECT_SID				7		// SID
#define _PARAM_OBJECT_BYTEOFFSET		8		// disk io - offset	//LARGE_INTEGER
#define _PARAM_OBJECT_SOURCE_ID			9		// hook depended
#define _PARAM_OBJECT_DEST_ID			10		// hook depended

#define _PARAM_OBJECT_URL_W				11		// имя объекта над которым выполняется операция в unicode
#define _PARAM_OBJECT_URL_DEST_W		12		// новое имя объекта (для операции типа rename и move) в unicode

#define _PARAM_OBJECT_CONTEXT_FLAGS		13		// for FileIO this is context flags
#define _CONTEXT_OBJECT_FLAG_NONE						0x0000000
#define _CONTEXT_OBJECT_FLAG_MODIFIED					0x0000001
#define _CONTEXT_OBJECT_FLAG_FIXEDVOLUME				0x0000002
#define _CONTEXT_OBJECT_FLAG_NETWORKTREE				0x0000004	//make object for create network connections (?)
#define _CONTEXT_OBJECT_FLAG_DIRECTACCESS				0x0000008	//like direct volume access
#define _CONTEXT_OBJECT_FLAG_EXECUTE					0x0000010
#define _CONTEXT_OBJECT_HAS_OBJID						0x0000020
#define _CONTEXT_OBJECT_FLAG_NTFS						0x0000100
#define _CONTEXT_OBJECT_FLAG_CREATENEWOBJECT			0x0000200
#define _CONTEXT_OBJECT_FLAG_DELETEPENDING				0x0000400
#define _CONTEXT_OBJECT_FLAG_SRCREMOTE					0x0001000
#define _CONTEXT_OBJECT_FLAG_OBJMODIFIED_AFCREATE		0x0010000
#define _CONTEXT_OBJECT_FLAG_OPERATION_ON_SELF			0x0020000
#define _CONTEXT_OBJECT_FLAG_DIRECTORY					0x0040000
#define _CONTEXT_OBJECT_FLAG_BACKUP						0x0080000
#define _CONTEXT_OBJECT_FLAG_SHAREREAD					0x0100000
#define _CONTEXT_OBJECT_FLAG_SHAREWRITE					0x0200000
#define _CONTEXT_OBJECT_FLAG_FORREAD					0x0400000
#define _CONTEXT_OBJECT_FLAG_FORWRITE					0x0800000
#define _CONTEXT_OBJECT_FLAG_FORDELETE					0x1000000
#define _CONTEXT_OBJECT_FLAG_SL_OPENTAR_DIR				0x2000000
#define _CONTEXT_OBJECT_FLAG_NETWORK_DEVICE				0x4000000
#define _CONTEXT_OBJECT_FLAG_BUFFER_ALIGN				0x8000000


#define _PARAM_USER_FILTER_NAME			14		// user defined filter name (char)
#define _PARAM_USER_FILTER_DESCRIPTION	15		// user defined filter description (char)
#define _PARAM_USER_FSDRVLIB_PROPROT	16		// marker for process protection


#define _PARAM_OBJECT_URL_PARAM_W		17		// command line or the same в unicode


#define CREATE_PROCESS_NOTIFY_FLAG_NONE				0x0000		//_PARAM_OBJECT_CONTEXT_FLAGS
#define CREATE_PROCESS_NOTIFY_FLAG_STDIN_REMOTE		0x0001
#define CREATE_PROCESS_NOTIFY_FLAG_STDOUT_REMOTE	0x0002

#define _PARAM_OBJECT_OBJECTHANDLE		18		// handle or etc
#define _PARAM_OBJECT_CLIENTID1			19		// some id
#define _PARAM_OBJECT_CLIENTID2			20		// some id

#define _PARAM_OBJECT_VOLUME_NAME_W		21		// 
#define _PARAM_OBJECT_VOLUME_NAME_DEST_W		22		// 
#define _PARAM_OBJECT_TYPE					23		// 


#define _PARAM_OBJECT_ADDR1					24		// 
#define _PARAM_OBJECT_ADDR2					25		// 

#define _PARAM_RET_STATUS					26		// 

#define _PARAM_OBJECT_PROCESSNAME			27		//

#define _PARAM_OBJECT_KEYLOGGERID			28

#define _PARAM_OBJECT_LUID					29

#define _PARAM_OBJECT_ATTRIB				30
#define _PARAM_OBJECT_PAGEPROTECTION		31

#define _PARAM_OBJECT_PARAMCOUNT			32

#define _PARAM_OBJECT_REFCOUNT				33

#define _PARAM_OBJECT_BASEPATH				34
#define _PARAM_OBJECT_STARTUP_STR			35

#define _PARAM_OBJECT_MSG_ID				36
#define _PARAM_OBJECT_WPARAM				37
#define _PARAM_OBJECT_LPARAM				38
#define _PARAM_OBJECT_INVIS_TARGET			39

#define _PARAM_OBJECT_INTERNALID			40
#define _PARAM_OBJECT_NETSHARE_W			41

#define _PARAM_OBJECT_CLSID					42
#define _PARAM_HARDLINK_COUNT				43
#define _PARAM_STREAM_W						44

// keylogger IDs
#define KEYLOGGER_GETASYNCKEYSTATE			0
#define KEYLOGGER_GETMESSAGE_PATCH			1
#define KEYLOGGER_PEEKMESSAGE_PATCH			2
#define KEYLOGGER_ATTACHKBD					3
#define KEYLOGGER_SPLICEONREADDISPATCH		4


//#define _PARAM_OBJECT_CLIENTIDN			N		// handle or etc


// -----------------------------------------------------------------------------------------
// IOCTL for disk
#define _AVPG_IOCTL_DISK_GET_GEOMETRY		0x1
#define _AVPG_IOCTL_DISK_GET_PARTITION_INFO	0x2
#define _AVPG_IOCTL_DISK_GET_DEVICE_TYPE	0x3
#define _AVPG_IOCTL_DISK_GET_SECTOR_SIZE	0x4
#define _AVPG_IOCTL_DISK_LOCK				0x5
#define _AVPG_IOCTL_DISK_UNLOCK				0x6
#define _AVPG_IOCTL_DISK_ISLOCKED			0x7

#define _AVPG_IOCTL_DISK_READ				0x10
#define _AVPG_IOCTL_DISK_WRITE				0x11

#define _AVPG_IOCTL_DISK_QUERYNAMES			0x20
#define _AVPG_IOCTL_DISK_QUERYHARDNAME		0x21

#define _AVPG_IOCTL_DISK_ISWRITEPROTECTED	0x31
#define _AVPG_IOCTL_DISK_ISPARTITION		0x32

#define _AVPG_IOCTL_DISK_GETID				0x41

#define _AVPG_IOCTL_FILE_OPEN				0x51

// -----------------------------------------------------------------------------------------
// IOCTL for system
#define _AVPG_IOCTL_FILE_PREFETCH		0x001
#define _AVPG_IOCTL_PROCESS_SUSPEND		0x002
#define _AVPG_IOCTL_PROCESS_RESUME		0x003
#define _AVPG_IOCTL_PROCESS_DISCARDOPS	0x004
#define _AVPG_IOCTL_STAT_DISK			0x005

#define _AVPG_IOCTL_FIDBOX_SET			0x006
#define _AVPG_IOCTL_FIDBOX_GET			0x007

#define _AVPG_IOCTL_GET_PIDLIST			0x008
#define _AVPG_IOCTL_GET_SECTIONLIST		0x009

#define _AVPG_IOCTL_FIDBOX2_GET			0x00a
#define _AVPG_IOCTL_FIDBOX_DISABLE		0x00b

#define _AVPG_IOCTL_GET_DRVLIST			0x00c

#define _AVPG_IOCTL_GET_APP_HASH		0x00d
#define _AVPG_IOCTL_GET_APP_PATH		0x00e

#define _AVPG_IOCTL_FIDBOX_SET_BH		0x00f

#define _AVPG_IOCTL_GET_PREFETCHINFO	0x100
#define _AVPG_IOCTL_CHECK_ACTIVE_SECT	0x200

#define _AVPG_IOCTL_KEYLOGGERS_CHECK	0x300

// -----------------------------------------------------------------------------------------
// Gruzdev
// IOCTL for registry
#define _AVPG_IOCTL_REG_CREATEKEY			0x01
#define _AVPG_IOCTL_REG_DELETEKEY			0x02
#define _AVPG_IOCTL_REG_ENUMKEY				0x03
#define _AVPG_IOCTL_REG_CHECKKEYPRESENT		0x04
#define _AVPG_IOCTL_REG_ENUMVALUE			0x05
#define _AVPG_IOCTL_REG_CHECKVALUEPRESENT	0x06
#define _AVPG_IOCTL_REG_QUERYVALUE			0x07
#define _AVPG_IOCTL_REG_SETVALUE			0x08
#define _AVPG_IOCTL_REG_DELETEVALUE			0x09
#define _AVPG_IOCTL_REG_QUERYINFOKEY		0x0A


// идентификаторы функций доступных в драйвере на остановленной треде

#define __he_GetDriveType				1		// 9x only
#define __he_ForceYeld					2		// 

// хеш для delete file on startup
#define _xor_first_ 14
#define _xor_drv_func(_pbuf, _bufsize, _direction)\
{\
	__int8* pbuf = (__int8*) _pbuf;\
	unsigned __int32 __cou;\
	unsigned __int8 __tmp;\
	unsigned __int8 __prev;\
	unsigned __int8 _xora = _xor_first_;\
	if (_direction)\
	{\
		__prev = _xora;\
		for (__cou = 0; __cou < _bufsize; __cou++)\
		{\
			_xora++;\
			__tmp = pbuf[__cou];\
			pbuf[__cou]  = pbuf[__cou] ^ __prev;\
			__prev = _xora + __tmp;\
		}\
	}\
	else\
	{\
		__prev = _xora;\
		for (__cou = 0; __cou < _bufsize; __cou++)\
		{\
			_xora++;\
			pbuf[__cou]  = pbuf[__cou] ^ __prev;\
			__prev = _xora + pbuf[__cou];\
		}\
	}\
}

#define _xor_dir_back		0
#define _xor_dir_go			1

#define _xor_drv_func_ex(_pbuf, _bufsize, _xor_byte, _direction)\
{\
	__int8* pbuf = (__int8*) _pbuf;\
	unsigned __int32 __cou;\
	unsigned __int8 __tmp;\
	unsigned __int8 __prev;\
	unsigned __int8 _xora = _xor_byte;\
	if (_direction)\
	{\
		__prev = _xora;\
		for (__cou = 0; __cou < _bufsize; __cou++)\
		{\
			_xora++;\
			__tmp = pbuf[__cou];\
			pbuf[__cou]  = pbuf[__cou] ^ __prev;\
			__prev = _xora + __tmp;\
		}\
	}\
	else\
	{\
		__prev = _xora;\
		for (__cou = 0; __cou < _bufsize; __cou++)\
		{\
			_xora++;\
			pbuf[__cou]  = pbuf[__cou] ^ __prev;\
			__prev = _xora + pbuf[__cou];\
		}\
	}\
}

#endif
