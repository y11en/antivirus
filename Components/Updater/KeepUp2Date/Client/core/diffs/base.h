#ifndef _BASE_H_131EF17A_A7D3_40ed_A658_BB6CBC008ECC
#define _BASE_H_131EF17A_A7D3_40ed_A658_BB6CBC008ECC

#pragma pack(1)

struct tSYSTEMTIME
{
    unsigned short wYear;
    unsigned short wMonth;
    unsigned short wDayOfWeek;
    unsigned short wDay;
    unsigned short wHour;
    unsigned short wMinute;
    unsigned short wSecond;
    unsigned short wMilliseconds;
};

#define AVP_BASE_VERSION_0x0300
#define AVP_MAGIC       0x382E4B45    /*"EK",VxD id ->45,4B,2E,38 */


// this header pos is *0x40
struct AVP_BaseHeader
{
	unsigned char Text[0x40];
	unsigned char Authenticity[0x40];    /*Authenticity Any data*/
	unsigned int Magic;
	union
	{
		unsigned short Version;          /*version 03,00*/
		struct
		{
			unsigned char VerH;             /*version 03*/
			unsigned char VerL;             /*subversion 00*/
		} BXs;
	} VXs;

	unsigned short Flags;     /*editable=1*/
	unsigned int CreatorID;
	unsigned int FileSize;
	unsigned int BlockHeaderTableFO;
	unsigned int BlockHeaderTableSize;
	tSYSTEMTIME     CreationTime;
	tSYSTEMTIME     ModificationTime;
	unsigned short NumberOfSingleBlockTypes;
	unsigned short NumberOfRecordBlockTypes;
	unsigned short NumberOfRecordTypes;
	unsigned int AuthenticityCRC;
	unsigned int HeaderCRC;
	
};

struct AVP_BlockHeader
{
    unsigned short BlockType;    /* BT_* */
    unsigned short RecordType;   /* RT_* */
    unsigned int BlockFO;
    unsigned int CompressedSize;
    unsigned int UncompressedSize;
    unsigned short Compression;
    unsigned short RecordSize;
    unsigned int NumRecords;
    unsigned int CRC;
    unsigned int OptHeaderFO;
    unsigned int OptHeaderSize;
};


#ifdef AVP_BASE_VERSION_0x0300
	#define NUMBER_OF_SINGLE_BLOCK_TYPES     3
	#define NUMBER_OF_RECORD_BLOCK_TYPES     3
	#define NUMBER_OF_RECORD_TYPES           8
#endif


#define BT_RECORD               0x0000
#define BT_LINK16               0x0001
#define BT_LINK32               0x0002

#define BT_NAME                 0x0100
#define BT_COMMENT              0x0101
#define BT_EDIT                 0x0102

#define RT_KERNEL               0x0000
#define RT_JUMP                 0x0001
#define RT_MEMORY               0x0002
#define RT_SECTOR               0x0003
#define RT_FILE                 0x0004
#define RT_CA                   0x0005
#define RT_UNPACK               0x0006
#define RT_EXTRACT              0x0007

#define RT_SEPARATOR                    0x0100   /*Not a record, used in editor*/

#pragma pack()

#endif	// _BASE_H_131EF17A_A7D3_40ed_A658_BB6CBC008ECC
