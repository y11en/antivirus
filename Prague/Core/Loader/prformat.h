#if !defined( __PrFormats_h__ )
#define __PrFormats_h__


#include <Prague/pr_types.h>


#pragma pack(1)
typedef struct _tag_PrModuleHeader_
{
	tDWORD dwSignature;
	tWORD  wCPUType;
	tWORD  wNumOfObjects;
	tDWORD dwEntryPoint;
	tDWORD dwImageBase;
	tDWORD dwOSVersion;
	tDWORD dwPluginVirtualSize;
	tDWORD dwReloSectionOffset;
	tDWORD dwReloSectionSize;
	tDWORD dwMetaDataSectionOffset;
	tDWORD dwMetaDataSectionSize;
}PrFileHeader, *LPPrFileHeader;

typedef struct _tag_PrSectionTable_
{
//	tCHAR  chSectionName[8];
	tDWORD dwVirtualSize;
	tDWORD dwSectionRVA;
	tDWORD dwPhisicalSize;
	tDWORD dwSectionOffset;
	tDWORD dwFlags;
}SectionTable, *LPSectionTable;

typedef struct _tag_RelocHeader_
{
	tDWORD dwPageRVA;
	tDWORD dwBlockSize;
	tWORD  wRecord;
}RelocTable, *LPRelocTable;
#pragma pack()



#define cINTEL_32_CPU  1
#define cINTEL_16_CPU  2
#define cINTEL_64_CPU  3
#define cAMD_64_CPU    4
#define cMIPS_CPU      5
#define cPCODE         6

#define cHEADER_SIGNATURE 0x4c505250 /* 'LPRP' */	//Prague Plugin


#endif //__PrFormats_h__
