#pragma pack(push,pack_key)
#pragma pack(1)

#define AVP_KEY_MAGIC	0x382E4441    //"AD",VxD id ->41,44,2E,38
#define AVPEDIT_KEY_MAGIC	0x382E6441    //"Ad",VxD id ->41,44,2E,38

typedef struct _AVP_KeyHeader
{
	BYTE		Text[0x20];
	DWORD		Magic;
	union{
		WORD	Version;          //version 03,00
		struct{
		BYTE	VerH;             //version 03
		BYTE	VerL;             //subversion 00
		};
	};
	DWORD		CompressedCRC;
	DWORD		CompressedSize;
	DWORD		UncompressedSize;

}AVP_KeyHeader;

typedef struct _AVP_Key
{
	DWORD		NameOffs;       //Offsets from the begin of unpacked AVP_Key block.
	DWORD		OrgOffs;
	DWORD		RegNumberOffs;
	DWORD		CopyInfoOffs;
	DWORD		SellerOffs;
	DWORD		SupportOffs;
	DWORD		Reserved[10];

	DWORD		Platforms;		//KEY_P_...
	DWORD		Options;		//KEY_O_...
	DWORD		Flags;			//KEY_F_...
	DWORD		AlarmDays;
	DWORD		BaseDelay;      //Month count

	WORD		ExpirYear;
	WORD		ExpirMonth;
	WORD		ExpirDay;

}AVP_Key;

#define	KEY_P_DOSLITE	0x0001
#define	KEY_P_DOS		0x0002
#define	KEY_P_WIN31		0x0004
#define	KEY_P_WIN95		0x0008
#define	KEY_P_WINNT		0x0010
#define	KEY_P_OS2		0x0020
#define	KEY_P_NOVELL	0x0040

#define KEY_O_DISINFECT		0x0001
#define KEY_O_UNPACK		0x0002
#define KEY_O_EXTRACT		0x0004
#define KEY_O_CA			0x0008
#define KEY_O_REMOTELAUNCH	0x0010
#define KEY_O_REMOTESCAN	0x0020
#define KEY_O_MONITOR		0x0040
#define KEY_O_CRC			0x0080
#define KEY_O_MAILBASES		0x0100
#define KEY_O_MAILPLAIN		0x0200

#define KEY_F_REGISTERED		0x0001
#define KEY_F_INFOEVERYLAUNCH	0x0002

#pragma pack(pop,pack_key)