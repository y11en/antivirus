// low level disk io for NT

#include "LLDiskIO.h"
#include <NTDDDISK.H>

#include "../drvtypes.h"
#include "../debug.h"
#include "../NameCache.h"
#include "../client.h"

#include "glbenvir.h"

#include "../hook/avpgcom.h"
#include "../hook/HookSpec.h"

#include "stdio.h"
/*
symbolic names
A-Z
Floppy0-Floppy999
00-99 HDD
*/

#ifndef FILE_DEVICE_DVD
#define FILE_DEVICE_DVD                 0x00000033
#endif

typedef struct _TDISKQUERYNAMES
{
	NTSTATUS	m_RetStatus;			// return status
	DWORD		m_Bufferlength;			// total buffer len
	DWORD		m_CurrentDataSize;		// current data size in m_pBuffer
	BYTE*		m_pBuffer;				// buffer for names
	
	PKLG_NAME_CACHE m_pNameCache;			// for eliminate duplicate string
}DISKQUERYNAMES, *PDISKQUERYNAMES;

//+ ------------------------------------------------------------------------------------------
#ifndef IOCTL_STORAGE_GET_MEDIA_TYPES_EX
#define IOCTL_STORAGE_GET_MEDIA_TYPES_EX CTL_CODE(IOCTL_STORAGE_BASE, 0x0301, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define  DFP_SEND_DRIVE_COMMAND   CTL_CODE(IOCTL_DISK_BASE, 0x0021, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define  DFP_RECEIVE_DRIVE_DATA   CTL_CODE(IOCTL_DISK_BASE, 0x0022, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define  IDE_ATA_IDENTIFY           0xEC

typedef enum _STORAGE_MEDIA_TYPE {
    //
    // Following are defined in ntdddisk.h in the MEDIA_TYPE enum
    //
    // Unknown,                // Format is unknown
    // F5_1Pt2_512,            // 5.25", 1.2MB,  512 bytes/sector
    // F3_1Pt44_512,           // 3.5",  1.44MB, 512 bytes/sector
    // F3_2Pt88_512,           // 3.5",  2.88MB, 512 bytes/sector
    // F3_20Pt8_512,           // 3.5",  20.8MB, 512 bytes/sector
    // F3_720_512,             // 3.5",  720KB,  512 bytes/sector
    // F5_360_512,             // 5.25", 360KB,  512 bytes/sector
    // F5_320_512,             // 5.25", 320KB,  512 bytes/sector
    // F5_320_1024,            // 5.25", 320KB,  1024 bytes/sector
    // F5_180_512,             // 5.25", 180KB,  512 bytes/sector
    // F5_160_512,             // 5.25", 160KB,  512 bytes/sector
    // RemovableMedia,         // Removable media other than floppy
    // FixedMedia,             // Fixed hard disk media
    // F3_120M_512,            // 3.5", 120M Floppy
    // F3_640_512,             // 3.5" ,  640KB,  512 bytes/sector
    // F5_640_512,             // 5.25",  640KB,  512 bytes/sector
    // F5_720_512,             // 5.25",  720KB,  512 bytes/sector
    // F3_1Pt2_512,            // 3.5" ,  1.2Mb,  512 bytes/sector
    // F3_1Pt23_1024,          // 3.5" ,  1.23Mb, 1024 bytes/sector
    // F5_1Pt23_1024,          // 5.25",  1.23MB, 1024 bytes/sector
    // F3_128Mb_512,           // 3.5" MO 128Mb   512 bytes/sector
    // F3_230Mb_512,           // 3.5" MO 230Mb   512 bytes/sector
    // F8_256_128,             // 8",     256KB,  128 bytes/sector
    //

    DDS_4mm = 0x20,            // Tape - DAT DDS1,2,... (all vendors)
    MiniQic,                   // Tape - miniQIC Tape
    Travan,                    // Tape - Travan TR-1,2,3,...
    QIC,                       // Tape - QIC
    MP_8mm,                    // Tape - 8mm Exabyte Metal Particle
    AME_8mm,                   // Tape - 8mm Exabyte Advanced Metal Evap
    AIT1_8mm,                  // Tape - 8mm Sony AIT1
    DLT,                       // Tape - DLT Compact IIIxt, IV
    NCTP,                      // Tape - Philips NCTP
    IBM_3480,                  // Tape - IBM 3480
    IBM_3490E,                 // Tape - IBM 3490E
    IBM_Magstar_3590,          // Tape - IBM Magstar 3590
    IBM_Magstar_MP,            // Tape - IBM Magstar MP
    STK_DATA_D3,               // Tape - STK Data D3
    SONY_DTF,                  // Tape - Sony DTF
    DV_6mm,                    // Tape - 6mm Digital Video
    DMI,                       // Tape - Exabyte DMI and compatibles
    SONY_D2,                   // Tape - Sony D2S and D2L
    CLEANER_CARTRIDGE,         // Cleaner - All Drive types that support Drive Cleaners
    CD_ROM,                    // Opt_Disk - CD
    CD_R,                      // Opt_Disk - CD-Recordable (Write Once)
    CD_RW,                     // Opt_Disk - CD-Rewriteable
    DVD_ROM,                   // Opt_Disk - DVD-ROM
    DVD_R,                     // Opt_Disk - DVD-Recordable (Write Once)
    DVD_RW,                    // Opt_Disk - DVD-Rewriteable
    MO_3_RW,                   // Opt_Disk - 3.5" Rewriteable MO Disk
    MO_5_WO,                   // Opt_Disk - MO 5.25" Write Once
    MO_5_RW,                   // Opt_Disk - MO 5.25" Rewriteable (not LIMDOW)
    MO_5_LIMDOW,               // Opt_Disk - MO 5.25" Rewriteable (LIMDOW)
    PC_5_WO,                   // Opt_Disk - Phase Change 5.25" Write Once Optical
    PC_5_RW,                   // Opt_Disk - Phase Change 5.25" Rewriteable
    PD_5_RW,                   // Opt_Disk - PhaseChange Dual Rewriteable
    ABL_5_WO,                  // Opt_Disk - Ablative 5.25" Write Once Optical
    PINNACLE_APEX_5_RW,        // Opt_Disk - Pinnacle Apex 4.6GB Rewriteable Optical
    SONY_12_WO,                // Opt_Disk - Sony 12" Write Once
    PHILIPS_12_WO,             // Opt_Disk - Philips/LMS 12" Write Once
    HITACHI_12_WO,             // Opt_Disk - Hitachi 12" Write Once
    CYGNET_12_WO,              // Opt_Disk - Cygnet/ATG 12" Write Once
    KODAK_14_WO,               // Opt_Disk - Kodak 14" Write Once
    MO_NFR_525,                // Opt_Disk - Near Field Recording (Terastor)
    NIKON_12_RW,               // Opt_Disk - Nikon 12" Rewriteable
    IOMEGA_ZIP,                // Mag_Disk - Iomega Zip
    IOMEGA_JAZ,                // Mag_Disk - Iomega Jaz
    SYQUEST_EZ135,             // Mag_Disk - Syquest EZ135
    SYQUEST_EZFLYER,           // Mag_Disk - Syquest EzFlyer
    SYQUEST_SYJET,             // Mag_Disk - Syquest SyJet
    AVATAR_F2,                 // Mag_Disk - 2.5" Floppy
    MP2_8mm                    // Tape - 8mm Hitachi
} STORAGE_MEDIA_TYPE, *PSTORAGE_MEDIA_TYPE;

typedef enum _STORAGE_BUS_TYPE {
    BusTypeUnknown = 0x00,
    BusTypeScsi,
    BusTypeAtapi,
    BusTypeAta,
    BusType1394,
    BusTypeSsa,
    BusTypeFibre,
    BusTypeUsb,
    BusTypeRAID,
    BusTypeMaxReserved = 0x7F
} STORAGE_BUS_TYPE, *PSTORAGE_BUS_TYPE;


typedef struct _DEVICE_MEDIA_INFO {
  union {
    struct {
      LARGE_INTEGER Cylinders;
      STORAGE_MEDIA_TYPE MediaType;
      ULONG TracksPerCylinder;
      ULONG SectorsPerTrack;
      ULONG BytesPerSector;
      ULONG NumberMediaSides;
      ULONG MediaCharacteristics; 
    } DiskInfo;
    struct {
      LARGE_INTEGER Cylinders;
      STORAGE_MEDIA_TYPE MediaType;
      ULONG TracksPerCylinder;
      ULONG SectorsPerTrack;
      ULONG BytesPerSector;
      ULONG NumberMediaSides;
      ULONG MediaCharacteristics; 
    } RemovableDiskInfo;
    struct {
      STORAGE_MEDIA_TYPE MediaType;
      ULONG  MediaCharacteristics; 
      ULONG  CurrentBlockSize;
      STORAGE_BUS_TYPE  BusType;
      union {
        struct {
          UCHAR MediumType;
          UCHAR DensityCode;
        } ScsiInformation;
      } BusSpecificData;
    } TapeInfo;
  } DeviceSpecific;
} DEVICE_MEDIA_INFO, *PDEVICE_MEDIA_INFO;

typedef struct _GET_MEDIA_TYPES {
    DWORD DeviceType;              // FILE_DEVICE_XXX values
    DWORD MediaInfoCount;
    DEVICE_MEDIA_INFO MediaInfo[1];
} GET_MEDIA_TYPES, *PGET_MEDIA_TYPES;

#define MEDIA_ERASEABLE         0x00000001
#define MEDIA_WRITE_ONCE        0x00000002
#define MEDIA_READ_ONLY         0x00000004
#define MEDIA_READ_WRITE        0x00000008

#define MEDIA_WRITE_PROTECTED   0x00000100
#define MEDIA_CURRENTLY_MOUNTED 0x80000000

#endif // IOCTL_STORAGE_GET_MEDIA_TYPES_EX


//+ ------------------------------------------------------------------------------------------
typedef struct _IDINFO
{
    USHORT  wGenConfig;                 // WORD 0
    USHORT  wNumCyls;                   // WORD 1
    USHORT  wReserved2;                 // WORD 2
    USHORT  wNumHeads;                  // WORD 3
    USHORT  wReserved4;                 // WORD 4
    USHORT  wReserved5;                 // WORD 5
    USHORT  wNumSectorsPerTrack;        // WORD 6
    USHORT  wVendorUnique[3];           // WORD 7-9
    CHAR    sSerialNumber[20];          // WORD 10-19
    USHORT  wBufferType;                // WORD 20
    USHORT  wBufferSize;                // WORD 21
    USHORT  wECCSize;                   // WORD 22:
    CHAR    sFirmwareRev[8];            // WORD 23-26
    CHAR    sModelNumber[40];           // WORD 27-46
    USHORT  wMoreVendorUnique;          // WORD 47
    USHORT  wReserved48;                // WORD 48
    struct {
        USHORT  reserved1:8;
        USHORT  DMA:1;                  //
        USHORT  LBA:1;                  //
        USHORT  DisIORDY:1;             //
        USHORT  IORDY:1;                //
        USHORT  SoftReset:1;            //
        USHORT  Overlap:1;              //
        USHORT  Queue:1;                //
        USHORT  InlDMA:1;               //
    } wCapabilities;                    // WORD 49
    USHORT  wReserved1;                 // WORD 50
    USHORT  wPIOTiming;                 // WORD 51
    USHORT  wDMATiming;                 // WORD 52
    struct {
        USHORT  CHSNumber:1;            // 1=WORD 54-58
        USHORT  CycleNumber:1;          // 1=WORD 64-70
        USHORT  UnltraDMA:1;            // 1=WORD 88
        USHORT  reserved:13;
    } wFieldValidity;                   // WORD 53
    USHORT  wNumCurCyls;                // WORD 54
    USHORT  wNumCurHeads;               // WORD 55
    USHORT  wNumCurSectorsPerTrack;     // WORD 56
    USHORT  wCurSectorsLow;             // WORD 57
    USHORT  wCurSectorsHigh;            // WORD 58
    struct {
        USHORT  CurNumber:8;            //
        USHORT  Multi:1;                //
        USHORT  reserved1:7;
    } wMultSectorStuff;                 // WORD 59
    ULONG  dwTotalSectors;              // WORD 60-61
    USHORT  wSingleWordDMA;             // WORD 62
    struct {
        USHORT  Mode0:1;                // 1 (4.17Mb/s)
        USHORT  Mode1:1;                // 1 (13.3Mb/s)
        USHORT  Mode2:1;                // 1 (16.7Mb/s)
        USHORT  Reserved1:5;
        USHORT  Mode0Sel:1;             //
        USHORT  Mode1Sel:1;             //
        USHORT  Mode2Sel:1;             //
        USHORT  Reserved2:5;
    } wMultiWordDMA;                    // WORD 63
    struct {
        USHORT  AdvPOIModes:8;          //
        USHORT  reserved:8;
    } wPIOCapacity;                     // WORD 64
    USHORT  wMinMultiWordDMACycle;      // WORD 65
    USHORT  wRecMultiWordDMACycle;      // WORD 66
    USHORT  wMinPIONoFlowCycle;         // WORD 67
    USHORT  wMinPOIFlowCycle;           // WORD 68
    USHORT  wReserved69[11];            // WORD 69-79
    struct {
        USHORT  Reserved1:1;
        USHORT  ATA1:1;                 // 1=ATA-1
        USHORT  ATA2:1;                 // 1=ATA-2
        USHORT  ATA3:1;                 // 1=ATA-3
        USHORT  ATA4:1;                 // 1=ATA/ATAPI-4
        USHORT  ATA5:1;                 // 1=ATA/ATAPI-5
        USHORT  ATA6:1;                 // 1=ATA/ATAPI-6
        USHORT  ATA7:1;                 // 1=ATA/ATAPI-7
        USHORT  ATA8:1;                 // 1=ATA/ATAPI-8
        USHORT  ATA9:1;                 // 1=ATA/ATAPI-9
        USHORT  ATA10:1;                // 1=ATA/ATAPI-10
        USHORT  ATA11:1;                // 1=ATA/ATAPI-11
        USHORT  ATA12:1;                // 1=ATA/ATAPI-12
        USHORT  ATA13:1;                // 1=ATA/ATAPI-13
        USHORT  ATA14:1;                // 1=ATA/ATAPI-14
        USHORT  Reserved2:1;
    } wMajorVersion;                    // WORD 80
    USHORT  wMinorVersion;              // WORD 81
    USHORT  wReserved82[6];             // WORD 82-87
    struct {
        USHORT  Mode0:1;                // 1 (16.7Mb/s)
        USHORT  Mode1:1;                // 1 (25Mb/s)
        USHORT  Mode2:1;                // 1 (33Mb/s)
        USHORT  Mode3:1;                // 1 (44Mb/s)
        USHORT  Mode4:1;                // 1 (66Mb/s)
        USHORT  Mode5:1;                // 1 (100Mb/s)
        USHORT  Mode6:1;                // 1 (133Mb/s)
        USHORT  Mode7:1;                // 1 (166Mb/s) ???
        USHORT  Mode0Sel:1;             //
        USHORT  Mode1Sel:1;             //
        USHORT  Mode2Sel:1;             //
        USHORT  Mode3Sel:1;             //
        USHORT  Mode4Sel:1;             //
        USHORT  Mode5Sel:1;             //
        USHORT  Mode6Sel:1;             //
        USHORT  Mode7Sel:1;             //
    } wUltraDMA;                        // WORD 88
    USHORT    wReserved89[167];         // WORD 89-255
} IDINFO, *PIDINFO;

VOID ChangeByteOrder(PCHAR szString, USHORT uscStrSize)
{
	USHORT  i;
	CHAR    temp;
	
	for (i = 0; i < uscStrSize; i+=2)
	{
		temp = szString[i];
		szString[i] = szString[i+1];
		szString[i+1] = temp;
	}
}

NTSTATUS IdentifyDevice(PDEVICE_OBJECT pDevice, PDISK_ID_INFO pOutInfo)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
    PSENDCMDINPARAMS pSCIP;
    PSENDCMDOUTPARAMS pSCOP;
    DWORD dwOutBytes;

	PIRP					Irp;
	KIRQL					currentIrql;
	IO_STATUS_BLOCK			ioStatus;
	KEVENT					event;

	RtlZeroMemory(pOutInfo, sizeof(DISK_ID_INFO));

    pSCIP = (PSENDCMDINPARAMS) ExAllocatePoolWithTag(NonPagedPool, sizeof(SENDCMDINPARAMS) - 1, '?bos');
    pSCOP = (PSENDCMDOUTPARAMS) ExAllocatePoolWithTag(NonPagedPool, sizeof(SENDCMDOUTPARAMS) + sizeof(IDINFO) - 1, '?bos');

	if (pSCIP && pSCOP)
	{
		KeInitializeEvent(&event, NotificationEvent, FALSE);

		RtlZeroMemory(pSCIP, sizeof(SENDCMDINPARAMS) - 1);
		RtlZeroMemory(pSCOP, sizeof(SENDCMDOUTPARAMS) + sizeof(IDINFO) - 1);
  
		pSCIP->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
  
		pSCIP->cBufferSize = 0;
		pSCOP->cBufferSize = sizeof(IDINFO);

		if(Irp = IoBuildDeviceIoControlRequest(DFP_RECEIVE_DRIVE_DATA/*IRP_MJ_DEVICE_CONTROL*/, pDevice, pSCIP, sizeof(SENDCMDINPARAMS) - 1, 
			pSCOP, sizeof(SENDCMDOUTPARAMS) + sizeof(IDINFO) - 1, FALSE, &event, &ioStatus)) 
		{
			ntStatus = IoCallDriver(pDevice, Irp);
			if(ntStatus == STATUS_PENDING)
			{
				KeWaitForSingleObject(&event, Suspended, KernelMode, FALSE, NULL);
				ntStatus = ioStatus.Status;
			}

			if (NT_SUCCESS(ntStatus) && !ioStatus.Information)
			{
				ntStatus = STATUS_UNSUCCESSFUL;
				DbgBreakPoint();
			}
			
			if (NT_SUCCESS(ntStatus))
			{
				PIDINFO pinfo = (PIDINFO) pSCOP->bBuffer;

				ChangeByteOrder(pinfo->sModelNumber, sizeof(pinfo->sModelNumber));
				ChangeByteOrder(pinfo->sFirmwareRev, sizeof(pinfo->sFirmwareRev));
				ChangeByteOrder(pinfo->sSerialNumber, sizeof(pinfo->sSerialNumber));

				memcpy(pOutInfo->m_ModelNumber, pinfo->sModelNumber, sizeof(pinfo->sModelNumber));
				memcpy(pOutInfo->m_FirmwareRev, pinfo->sFirmwareRev, sizeof(pinfo->sFirmwareRev));
				memcpy(pOutInfo->m_SerialNumber, pinfo->sSerialNumber, sizeof(pinfo->sSerialNumber));
			}
		}
	}
  
	if (pSCOP)
		ExFreePool(pSCOP);
	if (pSCIP)
		ExFreePool(pSCIP);
  
    return ntStatus;
}

int GetHarddiskId(PWCHAR pwchDir)
{
	NTSTATUS			ntStatus;
	int					nRet = -1;

	OBJECT_ATTRIBUTES	ObjAttr;
	HANDLE				hDirectoryObject;
	UNICODE_STRING		Name;
	
	WCHAR				DriverStr[32];
	PWCHAR				pdrvid;
	int					len ;

	PMY_QUERY_DIRECTORY_STRUCT	qds;
	POBJECT_NAMETYPE_INFO		p;
	ULONG						RegionSize;

	int cou;
	
	qds = NULL;
	RegionSize = sizeof(MY_QUERY_DIRECTORY_STRUCT);

	if(!NT_SUCCESS(ntStatus = ZwAllocateVirtualMemory((HANDLE)-1, (PVOID*) &qds, 0, &RegionSize, 
		MEM_COMMIT,PAGE_READWRITE)))
	{
		return nRet;
	}

	wcscpy(DriverStr, L"DR0");
	len = wcslen(DriverStr);
	pdrvid = &DriverStr[len - 1];

	RtlInitUnicodeString(&Name, pwchDir);
	Name.Length -= sizeof(WCHAR);
	InitializeObjectAttributes(&ObjAttr, &Name, OBJ_CASE_INSENSITIVE, NULL, NULL);
	ntStatus = ZwOpenDirectoryObject(&hDirectoryObject, DIRECTORY_QUERY, &ObjAttr);

	if(NT_SUCCESS(ntStatus))
	{
		BOOLEAN First = TRUE;
		
		while(NtQueryDirectoryObject(hDirectoryObject, &qds->Buffer, QUERY_DIRECTORY_BUFF_SIZE, TRUE, First, 
			&qds->Index, &qds->Retlen) >= 0 && nRet == -1)
		{
			PWCHAR obj_name;
			int tmp_len;
			p = (POBJECT_NAMETYPE_INFO)&qds->Buffer;
			First = FALSE;
			
			obj_name = p->ObjectName.Buffer;
			tmp_len = p->ObjectName.Length / sizeof(WCHAR);

			if (tmp_len >= 3)
			{
				// TAK POLUCHILOS! :)
				if (obj_name[0] == L'D' && obj_name[1] == L'R')
				{
					BOOLEAN bisdigit = TRUE;
					int cou2;
					int ntmp = 0;
					for (cou2 = 2; cou2 < tmp_len; cou2++)
					{
						if (obj_name[cou2] < L'0' || obj_name[cou2] > L'9')
						{
							bisdigit = FALSE;
							break;
						}
						ntmp = ntmp * 10 + (obj_name[cou2] - L'0');
					}

					if (bisdigit)
						nRet = ntmp;
				}
			}
		}

		ZwClose(hDirectoryObject);
	}

	_pfZwFreeVirtualMemory((HANDLE)-1,(PVOID*) &qds, &RegionSize,MEM_RELEASE);
	return nRet;
}

NTSTATUS
FindHardDiskForPartition(PUNICODE_STRING pNameUnicodeString, PWCHAR pOutRequest, ULONG OutRequestSize, ULONG *pRetSize)
{
	NTSTATUS ntRetStatus = STATUS_NOT_FOUND;
	NTSTATUS ntStatus;

	BYTE					cou;
	OBJECT_ATTRIBUTES	ObjAttr;
	HANDLE				hDirectoryObject;
	BOOLEAN				First;
	POBJECT_NAMETYPE_INFO p;
	HANDLE				DrvHandle;
	PDRIVER_OBJECT		pDrv;
	UNICODE_STRING		Name;
	PWCHAR				DrvName;
	PWCHAR				DriverStr=L"Partition";
	WCHAR				Dir[128];
	ULONG				RegionSize;
	PMY_QUERY_DIRECTORY_STRUCT qds;
	
	qds = NULL;
	RegionSize = sizeof(MY_QUERY_DIRECTORY_STRUCT);

	if(!NT_SUCCESS(ntStatus = ZwAllocateVirtualMemory((HANDLE)-1, (PVOID*) &qds, 0, &RegionSize, // ^ - Whistler beta 2 doesn't work with 8
		MEM_COMMIT,PAGE_READWRITE)))
	{
		DbPrint(DC_LLDISKIO, DL_ERROR,("ZwAllocateVirtualMemory fail. Status=%x\n", ntStatus));
	}
	else
	{
		PWCHAR pdrvid;
		for(cou = 0; cou <= 9 && ntRetStatus != STATUS_SUCCESS; cou++)
		{
			wcscpy(Dir, L"\\Device\\Harddisk0");
			pdrvid = &Dir[wcslen(Dir) - 1];
			*(BYTE*)pdrvid += cou;
			
			RtlInitUnicodeString(&Name, Dir);
			InitializeObjectAttributes(&ObjAttr, &Name, OBJ_CASE_INSENSITIVE, NULL, NULL);
			ntStatus=ZwOpenDirectoryObject(&hDirectoryObject, DIRECTORY_QUERY, &ObjAttr);
			if(NT_SUCCESS(ntStatus))
			{
				First=TRUE;
				DrvName=Dir+wcslen(Dir);
				*DrvName++='\\';
				while(NtQueryDirectoryObject(hDirectoryObject, &qds->Buffer, QUERY_DIRECTORY_BUFF_SIZE, TRUE, First, 
					&qds->Index, &qds->Retlen) >= 0 && ntRetStatus != STATUS_SUCCESS)
				{
					p=(POBJECT_NAMETYPE_INFO)&qds->Buffer;
					First=FALSE;
					*DrvName=0;
					if(wcsncmp(p->ObjectName.Buffer, DriverStr, 9)) 
						continue;
					else
					{
						HANDLE				hLink;

						InitializeObjectAttributes(&ObjAttr, &p->ObjectName, OBJ_CASE_INSENSITIVE, 
							hDirectoryObject, NULL);
						
						ntStatus = ZwOpenSymbolicLinkObject(&hLink, SYMBOLIC_LINK_QUERY, &ObjAttr);
						if(NT_SUCCESS(ntStatus))
						{
							WCHAR				targetNameBuffer[260];
							UNICODE_STRING      targetNameUnicodeString;

							RtlZeroMemory(targetNameBuffer, sizeof(targetNameBuffer));
							targetNameUnicodeString.Buffer = targetNameBuffer;
							targetNameUnicodeString.MaximumLength = sizeof(targetNameBuffer);
							ntStatus = ZwQuerySymbolicLinkObject(hLink, &targetNameUnicodeString, NULL);
							
							if(NT_SUCCESS(ntStatus) && (RtlCompareUnicodeString(&targetNameUnicodeString, 
								pNameUnicodeString, FALSE) == 0))
							{
								int number = GetHarddiskId(Dir);
								if (number != -1)
								{
									int cou2;
									int drvstrlen;
									char drstr[32];
									sprintf(drstr, "%d", number);
									drvstrlen = strlen(drstr);

									wcscpy(pOutRequest, Dir);
									wcscat(pOutRequest, L"DR");
									pdrvid = &pOutRequest[wcslen(pOutRequest) - 1] + 1;
									
									for (cou2 = 0; cou2 < drvstrlen; cou2++)
									{
										*pdrvid = (WCHAR) (drstr[cou2]);
										pdrvid++;
									}
									*pdrvid = 0;

									DbPrint(DC_LLDISKIO, DL_INFO, ("FindHardDiskForPartition found %S\n", pOutRequest));
									
									*pRetSize = (wcslen(pOutRequest) + 1 ) * 2;
									ntRetStatus = STATUS_SUCCESS;
								}
							}
							ZwClose(hLink);
						}
					}

				}
				ZwClose(hDirectoryObject);
			}
		}
	}
	RegionSize = 0;

	_pfZwFreeVirtualMemory((HANDLE)-1,(PVOID*) &qds,&RegionSize,MEM_RELEASE);

	return ntRetStatus;
}

// -----------------------------------------------------------------------------------------
VOID Disk_FindDiskDev(PDRIVER_OBJECT pDrv, PVOID pUserValue)
{
	PDEVICE_OBJECT		pDev, tmp;
	NTSTATUS			refStatus;
	ULONG				i;
	BOOLEAN				Disk;
	PWCHAR				pDeviceName;

	PDISKQUERYNAMES		pQueryName = (PDISKQUERYNAMES) pUserValue;
	ULONG				Key;

	BOOLEAN bOverBuf;
	
	pDev = pDrv->DeviceObject;
	Disk = FALSE;
	while(pDev && NT_SUCCESS(pQueryName->m_RetStatus))
	{
		refStatus = ObReferenceObjectByPointer(pDev,STANDARD_RIGHTS_REQUIRED,NULL,KernelMode);
		if(!NT_SUCCESS(refStatus))
		{
			DbPrint(DC_LLDISKIO, DL_ERROR, ("FindDiskDev ObReferenceObjectByPointer 0x%x failed, status 0x%x\n",
				pDev, refStatus));
		}
		
		if(pDev->DeviceType == FILE_DEVICE_DISK || pDev->DeviceType == FILE_DEVICE_CD_ROM || 
			pDev->DeviceType == FILE_DEVICE_DVD)
		{
			if (pDev->Flags & DO_DEVICE_HAS_NAME)
			{
				Disk = TRUE;
				if ((pDev->AttachedDevice != NULL) && (pDev->AttachedDevice->Flags & DO_DEVICE_HAS_NAME))
				{
					// skip this device because attached has name
				}
				else
				{
					pDeviceName = ExAllocatePoolWithTag(NonPagedPool, MAXPATHLEN * sizeof(WCHAR), 'WboS');
					if (pDeviceName != NULL)
					{
						if (GetDeviceName(pDev, pDeviceName, MAXPATHLEN * sizeof(WCHAR)) != NULL)
						{
							ULONG NameLen;
							NameLen = wcslen(pDeviceName);
							if (NameLen == 0) 
							{
								//DbPrint(DC_LLDISKIO, DL_WARNING, ("Drv=0x%x Device=0x%x Type=%s has DO_HASNNAME flag but GetDeviceName return name with len == 0\n",pDrv,pDev,
								//(pDev->DeviceType<sizeof(FileDevTypeName)/sizeof(CHAR*))?FileDevTypeName[pDev->DeviceType]:NullStr));
							}
							else
							{
								//DbPrint(DC_LLDISKIO, DL_NOTIFY, ("Drv=0x%x Device=0x%x Type=%s name %S\n",pDrv,pDev,	
								//(pDev->DeviceType < sizeof(FileDevTypeName) / sizeof(CHAR*)) ? FileDevTypeName[pDev->DeviceType] : NullStr, pDeviceName));
								NameLen++;
								NameLen *= sizeof(WCHAR);
								
								Key = CalcCacheID(NameLen, (BYTE*) pDeviceName, 0);
								bOverBuf = FALSE;
								if (NameCacheGet(pQueryName->m_pNameCache, Key, NULL, 0, &bOverBuf))
								{
									// name already exist
									DbPrint(DC_SYS, DL_WARNING, ("NameCacheTruncating - buffer too small in Disk_FindDiskDev\n"));
									DbgBreakPoint();
								}
								else
								{
									if ((pQueryName->m_Bufferlength + sizeof(WCHAR)) < (pQueryName->m_CurrentDataSize + NameLen))
									{
										DbPrint(DC_LLDISKIO, DL_NOTIFY, ("Disk_FindDiskDev - to small buffer\n"));
										pQueryName->m_RetStatus = STATUS_BUFFER_TOO_SMALL;
									}
									else
									{
										NameCacheStore(pQueryName->m_pNameCache, Key, pDeviceName, NameLen, 
											FALSE, 'DboS');
										
										if (pQueryName->m_pBuffer != NULL)
										{
											RtlCopyMemory(pQueryName->m_pBuffer + pQueryName->m_CurrentDataSize, 
												pDeviceName, NameLen);
										}

										pQueryName->m_CurrentDataSize += NameLen;
									}
								}
							}
						}
						
						ExFreePool(pDeviceName);
					}
				}
			}
		}
		
		tmp = pDev;
		pDev = pDev->NextDevice;

		if(NT_SUCCESS(refStatus))
			ObDereferenceObject(tmp);
	}
}


// -----------------------------------------------------------------------------------------
PDEVICE_OBJECT
Disk_GetDeviceObjectByLetter(WCHAR DriveLetter)
{
	PDEVICE_OBJECT		pDevice = NULL;
	NTSTATUS			ntStatus;
	
	HANDLE				hDir;
	UNICODE_STRING		us;
	OBJECT_ATTRIBUTES	ObjAttr;
	
	HANDLE				hLink;
	WCHAR				targetNameBuffer[260];
	UNICODE_STRING      targetNameUnicodeString;
	
	PFILE_OBJECT		fileObject;
	IO_STATUS_BLOCK		ioStatus;

	WCHAR				cDrive[3] = L"A:";

	RtlInitUnicodeString(&us, (PWCHAR)L"\\??");
	InitializeObjectAttributes(&ObjAttr, &us, OBJ_CASE_INSENSITIVE, NULL, NULL);
	ntStatus = ZwOpenDirectoryObject(&hDir, DIRECTORY_QUERY,&ObjAttr);
	if(!NT_SUCCESS(ntStatus))
	{
		DbPrint(DC_LLDISKIO, DL_ERROR, ("ZwOpenDirectoryObject %S failed, status %x\n",us.Buffer, ntStatus));
		return NULL;
	}

	cDrive[0] = DriveLetter;
	RtlInitUnicodeString(&us,cDrive);

	InitializeObjectAttributes(&ObjAttr, &us, OBJ_CASE_INSENSITIVE, hDir, NULL);
	ntStatus = ZwOpenSymbolicLinkObject(&hLink, SYMBOLIC_LINK_QUERY, &ObjAttr);
	if(NT_SUCCESS(ntStatus))
	{
		RtlZeroMemory(targetNameBuffer, sizeof(targetNameBuffer));
		targetNameUnicodeString.Buffer = targetNameBuffer;
		targetNameUnicodeString.MaximumLength = sizeof(targetNameBuffer);
		ntStatus = ZwQuerySymbolicLinkObject(hLink, &targetNameUnicodeString, NULL);
		if(NT_SUCCESS(ntStatus))
		{
			if(!wcsncmp(targetNameBuffer,L"\\Device",7))
			{
				HANDLE hFile;
				ntStatus = ZwCreateFile(&hFile, SYNCHRONIZE | FILE_ANY_ACCESS, &ObjAttr, &ioStatus, NULL, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
				if(NT_SUCCESS(ntStatus))
				{
					//!FILE_DEVICE_MASS_STORAGE or FILE_DEVICE_TAPE
					ntStatus = ObReferenceObjectByHandle(hFile, FILE_READ_DATA, NULL, KernelMode, (PVOID*) &fileObject, NULL);
					if(NT_SUCCESS(ntStatus))
					{
						pDevice = fileObject->DeviceObject;
						if (pDevice->Vpb != NULL)
						{
							if (pDevice->Vpb->RealDevice != NULL)
								pDevice = pDevice->Vpb->RealDevice;
						}
						if(pDevice->DeviceType == FILE_DEVICE_DISK || pDevice->DeviceType == FILE_DEVICE_CD_ROM || pDevice->DeviceType == FILE_DEVICE_DVD)
						{
							if(!NT_SUCCESS(ObReferenceObjectByPointer(pDevice, STANDARD_RIGHTS_REQUIRED, *IoDeviceObjectType, KernelMode)))	//dereference will be in caller proc
								pDevice = NULL;
						}
						else
							pDevice = NULL;

						ObDereferenceObject(fileObject);
					}

					ZwClose(hFile);
				}
			}
		}
		ZwClose(hLink);
	}
					
	ZwClose(hDir);

	return pDevice;
}

NTSTATUS Disk_GetPartitionLinkName(PWCHAR DriveLetter, WCHAR* pwchName, USHORT NameLen)
{
	PDEVICE_OBJECT pDevice = NULL;
	PFILE_OBJECT pFileObject;
	NTSTATUS ntStatusRet = STATUS_UNSUCCESSFUL;

	UNICODE_STRING ObjectName;

	RtlInitUnicodeString(&ObjectName, DriveLetter);
	
	if (ObjectName.Length == sizeof(WCHAR))
	{
		NTSTATUS			ntStatus;
		
		HANDLE				hDir;
		UNICODE_STRING		us;
		OBJECT_ATTRIBUTES	ObjAttr;
		
		HANDLE				hLink;
		UNICODE_STRING      targetNameUnicodeString;
		
		PFILE_OBJECT		fileObject;
		IO_STATUS_BLOCK		ioStatus;

		WCHAR				cDrive[3] = L"A:";

		RtlInitUnicodeString(&us, (PWCHAR)L"\\??");
		InitializeObjectAttributes(&ObjAttr, &us, OBJ_CASE_INSENSITIVE, NULL, NULL);
		ntStatus = ZwOpenDirectoryObject(&hDir, DIRECTORY_QUERY,&ObjAttr);
		if(!NT_SUCCESS(ntStatus))
		{
			DbPrint(DC_LLDISKIO, DL_ERROR, ("ZwOpenDirectoryObject %S failed, status %x\n",us.Buffer, ntStatus));
			return ntStatusRet;
		}

		cDrive[0] = DriveLetter[0];
		RtlInitUnicodeString(&us,cDrive);

		InitializeObjectAttributes(&ObjAttr, &us, OBJ_CASE_INSENSITIVE, hDir, NULL);
		ntStatus = ZwOpenSymbolicLinkObject(&hLink, SYMBOLIC_LINK_QUERY, &ObjAttr);
		if(NT_SUCCESS(ntStatus))
		{
			RtlZeroMemory(pwchName, NameLen);
			targetNameUnicodeString.Buffer = pwchName;
			targetNameUnicodeString.MaximumLength = NameLen;
			ntStatus = ZwQuerySymbolicLinkObject(hLink, &targetNameUnicodeString, NULL);
			if(NT_SUCCESS(ntStatus))
			{
				if(!wcsncmp(pwchName,L"\\Device",7))
					ntStatusRet = STATUS_SUCCESS;
			}
			
			ZwClose(hLink);
		}

		ZwClose(hDir);		
	}

	return ntStatusRet;
}

PDEVICE_OBJECT
Disk_GetDeviceByName(PWCHAR DriveName)
{
	PDEVICE_OBJECT pDevice = NULL;
	PFILE_OBJECT pFileObject;
	NTSTATUS ntStatus;

	UNICODE_STRING ObjectName;

	RtlInitUnicodeString(&ObjectName, DriveName);
	
	if (ObjectName.Length == sizeof(WCHAR))
		return Disk_GetDeviceObjectByLetter(*DriveName);
	else
	{
		HANDLE DeviceHandle;
		OBJECT_ATTRIBUTES ObjAttr;
		IO_STATUS_BLOCK ioStatus;

		InitializeObjectAttributes(&ObjAttr, &ObjectName, OBJ_CASE_INSENSITIVE, NULL, NULL);

		ntStatus = ZwCreateFile(&DeviceHandle, SYNCHRONIZE | FILE_ANY_ACCESS, &ObjAttr, &ioStatus, NULL, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
		if(NT_SUCCESS(ntStatus))
		{
			ntStatus = ObReferenceObjectByHandle(DeviceHandle, STANDARD_RIGHTS_REQUIRED, NULL, KernelMode, (VOID**)&pFileObject, NULL);
			if(NT_SUCCESS(ntStatus))
			{
				if (pFileObject->DeviceObject != NULL)
				{
					pDevice = pFileObject->DeviceObject;
					
					if (pDevice->Vpb != NULL)
					{
						if (pDevice->Vpb->RealDevice != NULL)
							pDevice = pDevice->Vpb->RealDevice;
					}
					
					if (pDevice->Flags & DO_DEVICE_INITIALIZING)
						pDevice = NULL;
					else
					{
						if(pDevice->DeviceType == FILE_DEVICE_DISK || pDevice->DeviceType == FILE_DEVICE_CD_ROM || pDevice->DeviceType == FILE_DEVICE_DVD)
						{
							if (!NT_SUCCESS(ObReferenceObjectByPointer(pDevice, STANDARD_RIGHTS_REQUIRED, *IoDeviceObjectType, KernelMode)))
								pDevice = NULL;
						}
						else
							pDevice = NULL;
					}
				}
				
				ObDereferenceObject(pFileObject);
			}
			
			ZwClose(DeviceHandle);
		}
	}

	return pDevice;
}

// -----------------------------------------------------------------------------------------
NTSTATUS
Disk_GetSectorSize(PDEVICE_OBJECT pDevice, USHORT* pBuffer)
{
	NTSTATUS ntstatus = STATUS_OBJECT_NAME_NOT_FOUND;
	if (pDevice->SectorSize == 0)
	{
		if ((FILE_DEVICE_CD_ROM == pDevice->DeviceType) || (FILE_DEVICE_DVD == pDevice->DeviceType))
			*pBuffer = 2048;
		else
			*pBuffer = 512;
	}
	else
		*pBuffer = pDevice->SectorSize;
	
	ntstatus = STATUS_SUCCESS;
	
	return ntstatus;
}

// -----------------------------------------------------------------------------------------

NTSTATUS
ReadDiskSector(PDEVICE_OBJECT pDevice, __int64 Offset, ULONG *Size, PVOID OutBuff)
{
	PIRP					Irp;
	PIO_STACK_LOCATION		irpStack;
	KIRQL					currentIrql;
	NTSTATUS				ntStatus;
	IO_STATUS_BLOCK			ioStatus;
	KEVENT					event;
	USHORT	kratnost = 512;
	
	Disk_GetSectorSize(pDevice, &kratnost);

	DbPrint(DC_LLDISKIO, DL_INFO, ("ReadSector pDevice:%x Offset:%I64x Size:%x\n", pDevice, Offset, *Size));

	if(*Size % kratnost)
		*Size = (*Size) - (*Size) % kratnost;

	if (0 == *Size)
	{
		DbgBreakPoint();
		return STATUS_INVALID_PARAMETER;
	}

	KeInitializeEvent(&event, NotificationEvent, FALSE);
	
	if(!(Irp = IoBuildSynchronousFsdRequest(IRP_MJ_READ, pDevice, OutBuff, *Size, (LARGE_INTEGER*) &Offset, &event, &ioStatus))) {
		DbPrint(DC_LLDISKIO, DL_ERROR, ("BuildRequest for read disk failed\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	irpStack = IoGetNextIrpStackLocation(Irp);
	irpStack->Flags |= SL_OVERRIDE_VERIFY_VOLUME;
		
	ntStatus = IoCallDriver(pDevice, Irp);
	if(ntStatus == STATUS_PENDING)
	{
		KeWaitForSingleObject(&event, Suspended, KernelMode, FALSE, NULL);
		ntStatus = ioStatus.Status;
	}
	*Size = ioStatus.Information;
	return ntStatus;
}

NTSTATUS
WriteDiskSector(PDEVICE_OBJECT pDevice, __int64 Offset, ULONG *Size, PVOID InBuff)
{
	PIRP					Irp;
	PIO_STACK_LOCATION		irpStack;
	KIRQL					currentIrql;
	NTSTATUS				ntStatus;
	IO_STATUS_BLOCK			ioStatus;
	KEVENT					event;
	USHORT	kratnost = 512;
	
	Disk_GetSectorSize(pDevice, &kratnost);

	DbPrint(DC_LLDISKIO, DL_INFO,("WriteSector pDevice:%x Offset:%I64x Size:%x\n", pDevice, Offset, *Size));

	if(*Size % kratnost)
		*Size=(*Size) - (*Size) % kratnost;

	if (0 == *Size)
	{
		DbgBreakPoint();
		return STATUS_INVALID_PARAMETER;
	}

	KeInitializeEvent(&event, NotificationEvent, FALSE);
	
	
	if(!(Irp = IoBuildSynchronousFsdRequest(IRP_MJ_WRITE, pDevice, InBuff, *Size, (LARGE_INTEGER*) &Offset, &event, &ioStatus))) {
		DbPrint(DC_LLDISKIO, DL_ERROR, ("BuildRequest for write disk failed\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	irpStack = IoGetNextIrpStackLocation(Irp);
	irpStack->Flags |= SL_OVERRIDE_VERIFY_VOLUME;
	
	ntStatus = IoCallDriver(pDevice, Irp);
	if(ntStatus == STATUS_PENDING)
	{
		KeWaitForSingleObject(&event, Suspended, KernelMode, FALSE, NULL);
		ntStatus = ioStatus.Status;
	}
	*Size = ioStatus.Information;
	return ntStatus;
}

// -----------------------------------------------------------------------------------------
NTSTATUS
GetDiskGeometry(PDEVICE_OBJECT pDevice, IN PDISK_GEOMETRY pGeometry)
{
	PIRP Irp;
	PIO_STACK_LOCATION		irpStack;
	KEVENT Event;
    NTSTATUS Status;
    IO_STATUS_BLOCK Iosb;

	memset(pGeometry, 0, sizeof(DISK_GEOMETRY));
	
    KeInitializeEvent( &Event, NotificationEvent, FALSE );
	
    Irp = IoBuildDeviceIoControlRequest(IOCTL_DISK_GET_DRIVE_GEOMETRY, pDevice, 
		NULL, 0, pGeometry, sizeof(DISK_GEOMETRY), FALSE, &Event, &Iosb);
	
    if (Irp == NULL)
	{
		DbPrint(DC_LLDISKIO, DL_WARNING, ("GetDiskGeometry failed. No free resource\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	irpStack = IoGetNextIrpStackLocation(Irp);
	irpStack->Flags |= SL_OVERRIDE_VERIFY_VOLUME;
	
    Status = IoCallDriver(pDevice, Irp);
	
    if ( Status == STATUS_PENDING )
	{
		KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, (PLARGE_INTEGER)NULL);
		Status = Iosb.Status;
	}

	if (NT_SUCCESS(Status) && !Iosb.Information)
	{
		Status = STATUS_UNSUCCESSFUL;
		DbgBreakPoint();
	}

	DbPrint(DC_LLDISKIO, DL_INFO, ("GetDiskGeometry result %#x\n", Status));

    return Status;
}

NTSTATUS
IsMediaProtected(PDEVICE_OBJECT pDevice, BOOLEAN *pbProtected)
{
	PIRP Irp;
	KEVENT Event;
    NTSTATUS Status;
    IO_STATUS_BLOCK Iosb;

	ULONG reqsize = 4096 * 2;
	
	BYTE* pBuffer = ExAllocatePoolWithTag(NonPagedPool, reqsize, '.boS');
	if (pBuffer == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	RtlZeroMemory(pBuffer, reqsize);
	
    KeInitializeEvent( &Event, NotificationEvent, FALSE );
	
    Irp = IoBuildDeviceIoControlRequest(IOCTL_STORAGE_GET_MEDIA_TYPES_EX, pDevice, 
		NULL, 0, pBuffer, reqsize, FALSE, &Event, &Iosb);
	
    if (Irp == NULL)
	{
		DbPrint(DC_LLDISKIO, DL_WARNING, ("GetDeviceMediaInfo failed. No free resource\n"));
		ExFreePool(pBuffer);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
    Status = IoCallDriver(pDevice, Irp);
	
    if ( Status == STATUS_PENDING )
	{
		KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, (PLARGE_INTEGER)NULL);
		Status = Iosb.Status;
	}

	if (NT_SUCCESS(Status) && Iosb.Information < sizeof(GET_MEDIA_TYPES))
	{
		Status = STATUS_UNSUCCESSFUL;
		DbgBreakPoint();
	}
	
	if (NT_SUCCESS(Status))
	{
		PGET_MEDIA_TYPES pGetTypes = (PGET_MEDIA_TYPES) pBuffer;
		
		if (sizeof(GET_MEDIA_TYPES) + pGetTypes->MediaInfoCount * sizeof(DEVICE_MEDIA_INFO) > reqsize)
		{
			DbPrint(DC_LLDISKIO, DL_ERROR, ("returned GET_MEDIA_TYPES is invalid\n"));
		}
		else
		{
			PDEVICE_MEDIA_INFO pDevInfo = (PDEVICE_MEDIA_INFO) pGetTypes->MediaInfo;
			
			*pbProtected = FALSE;
			
			while (pGetTypes->MediaInfoCount != 0)
			{
				if ((pGetTypes->DeviceType == FILE_DEVICE_TAPE_FILE_SYSTEM) || (pGetTypes->DeviceType == FILE_DEVICE_TAPE))
				{
					if (pDevInfo->DeviceSpecific.TapeInfo.MediaCharacteristics & (MEDIA_READ_ONLY | MEDIA_WRITE_PROTECTED))
						*pbProtected = TRUE;
				}
				else
				{
					if (pDevInfo->DeviceSpecific.DiskInfo.MediaCharacteristics & (MEDIA_READ_ONLY | MEDIA_WRITE_PROTECTED))
						*pbProtected = TRUE;
				}
				
				pDevInfo = (PDEVICE_MEDIA_INFO) ((BYTE*)pDevInfo + sizeof(DEVICE_MEDIA_INFO));
				pGetTypes->MediaInfoCount--;
			}
		}
	}
	
	DbPrint(DC_LLDISKIO, DL_INFO, ("GetDeviceMediaInfo result %#x\n", Status));
	ExFreePool(pBuffer);
	
    return Status;
}

// -----------------------------------------------------------------------------------------
NTSTATUS
GetPartitionInfo(PDEVICE_OBJECT pDevice, IN PPARTITION_INFORMATION PartitionInformation)
{
    PIRP Irp;
	PIO_STACK_LOCATION		irpStack;
    KEVENT Event;
    NTSTATUS Status;
    IO_STATUS_BLOCK Iosb;
	
    KeInitializeEvent( &Event, NotificationEvent, FALSE );
	
    Irp = IoBuildDeviceIoControlRequest(IOCTL_DISK_GET_PARTITION_INFO, pDevice, 
		NULL, 0, PartitionInformation, sizeof(PARTITION_INFORMATION), FALSE, &Event, &Iosb);
	
    if (Irp == NULL)
	{
		DbPrint(DC_LLDISKIO, DL_WARNING, ("GetPartitionInfo failed. No free resource\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	irpStack = IoGetNextIrpStackLocation(Irp);
	irpStack->Flags |= SL_OVERRIDE_VERIFY_VOLUME;

    Status = IoCallDriver(pDevice, Irp);
		
    if ( Status == STATUS_PENDING )
	{
		KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, (PLARGE_INTEGER)NULL);
		Status = Iosb.Status;
	}

	if (NT_SUCCESS(Status) && !Iosb.Information)
	{
		Status = STATUS_UNSUCCESSFUL;
		DbgBreakPoint();
	}
		
	DbPrint(DC_LLDISKIO, DL_INFO, ("GetPartitionInfo result %#x\n", Status));
		
    return Status;
}

// -----------------------------------------------------------------------------------------
NTSTATUS
Disk_GetGeometry(PDEVICE_OBJECT pDevice, PVOID pBuffer)
{
	NTSTATUS ntstatus;
	ULONG i;
	ntstatus = GetDiskGeometry(pDevice, (PDISK_GEOMETRY)pBuffer);

	if (STATUS_NOT_IMPLEMENTED == ntstatus)
	{
		PDISK_GEOMETRY pgeom = (PDISK_GEOMETRY) pBuffer;
		USHORT sector_size;

		Disk_GetSectorSize(pDevice, &sector_size);
		pgeom->BytesPerSector = sector_size;

		pgeom->Cylinders.QuadPart = 1;
		pgeom->MediaType = Unknown;
		pgeom->SectorsPerTrack = 1;
		pgeom->TracksPerCylinder = 1;

		ntstatus = STATUS_SUCCESS;

		return ntstatus;
	}

	if (ntstatus == STATUS_VERIFY_REQUIRED)
	{
		DbPrint(DC_LLDISKIO, DL_WARNING, ("Disk_GetGeometry: !!! Why??? Was overryded!!! (Podavlen!!!) Verify required (status = %#x)\n",ntstatus));
		DbgBreakPoint();
		
		ntstatus = IoVerifyVolume(pDevice, TRUE);
		
		DbPrint(DC_LLDISKIO, DL_WARNING, ("Disk_GetGeometry: Verify request result status = %#x\n",ntstatus));

//		if (ntstatus == STATUS_SUCCESS || STATUS_WRONG_VOLUME)
			ntstatus = GetDiskGeometry(pDevice, (PDISK_GEOMETRY)pBuffer);
	}

	for(i=0;i<3 && ntstatus == STATUS_IO_DEVICE_ERROR;i++) {
		LARGE_INTEGER Timeout;
		*(__int64*)&Timeout= -((__int64)5L*1000000L);// 0.5 sec
		KeDelayExecutionThread(KernelMode,TRUE,&Timeout);
		ntstatus = GetDiskGeometry(pDevice, (PDISK_GEOMETRY)pBuffer);
		DbPrint(DC_LLDISKIO, DL_WARNING, ("Disk_GetGeometry (try %d) status= %#x\n",i+1, ntstatus));
	}

	return ntstatus;
}

// -----------------------------------------------------------------------------------------
NTSTATUS
Disk_GetDeviceType(PDEVICE_OBJECT pDevice, PVOID pBuffer)
{
	NTSTATUS ntstatus = STATUS_OBJECT_NAME_NOT_FOUND;

	*(ULONG*)pBuffer = pDevice->DeviceType;
	ntstatus = STATUS_SUCCESS;
	
	return ntstatus;
}

// -----------------------------------------------------------------------------------------
NTSTATUS
Disk_GetPartitionInfo(PDEVICE_OBJECT pDevice, PVOID pBuffer)
{
	NTSTATUS ntstatus = STATUS_OBJECT_NAME_NOT_FOUND;
	
	ntstatus = GetPartitionInfo(pDevice, (PPARTITION_INFORMATION) pBuffer);

	return ntstatus;
}


// -----------------------------------------------------------------------------------------
NTSTATUS
DoDiskRequest(PEXTERNAL_DRV_REQUEST pInRequest, PVOID pOutRequest, ULONG OutRequestSize, ULONG *pRetSize)
{
	NTSTATUS			ntStatus = STATUS_OBJECT_NAME_NOT_FOUND;
	PDEVICE_OBJECT		pDevice = NULL;

	ULONG			DrvNameLen;
	WCHAR*			pDriveName;

	__int64			ByteOffset;
	ULONG*			pDataSize;
	ULONG			SizeOfBuffer;
	BYTE*			pBuffer;

	PDISK_IO_REQUEST pDiskRequestTmp = NULL;
	PMDL pMdl = NULL;

	//!map memory - check incoming buffer

	if (pOutRequest && OutRequestSize)
	{
		pMdl = IoAllocateMdl(pOutRequest,OutRequestSize, FALSE, FALSE, NULL);
		if (!pMdl)
		{
			return STATUS_NO_MEMORY;
		}
		else
		{
			__try
			{
				MmProbeAndLockPages(pMdl, KernelMode, IoModifyAccess);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				DbgBreakPoint();
				IoFreeMdl(pMdl);
				
				return STATUS_INVALID_PARAMETER;
			}
		}
	}

	pDiskRequestTmp = (PDISK_IO_REQUEST) pInRequest->m_Buffer;

	if ((pInRequest->m_BufferSize < sizeof(DISK_IO_REQUEST)) || (pDiskRequestTmp->m_Tag != -1))
	{
		PDISK_IO_REQUEST_OLD pDiskRequestOld = NULL;
		if (pInRequest->m_BufferSize < sizeof(DISK_IO_REQUEST_OLD))
		{
			DbPrint(DC_LLDISKIO, DL_WARNING, ("DoDiskRequest - input buffer too small %d (required %d)\n", pInRequest->m_BufferSize, sizeof(DISK_IO_REQUEST)));
			
			if (pMdl)
			{
				MmUnlockPages(pMdl);
				IoFreeMdl(pMdl);
			}

			return STATUS_INVALID_BLOCK_LENGTH;
		}

		pDiskRequestOld = (PDISK_IO_REQUEST_OLD) pInRequest->m_Buffer;
		ByteOffset = pDiskRequestOld->m_ByteOffset;
		pDataSize = &pDiskRequestOld->m_DataSize;
		SizeOfBuffer = pDiskRequestOld->m_SizeOfBuffer;
		pBuffer = pDiskRequestOld->m_Buffer;

		DrvNameLen = DISKNAMEMAXLEN_OLD;
		pDriveName = pDiskRequestOld->m_DriveName;
	}
	else
	{
		PDISK_IO_REQUEST pDiskRequest = (PDISK_IO_REQUEST) pInRequest->m_Buffer;
		ByteOffset = pDiskRequest->m_ByteOffset;
		pDataSize = &pDiskRequest->m_DataSize;
		SizeOfBuffer = pDiskRequest->m_SizeOfBuffer;
		pBuffer = pDiskRequest->m_Buffer;

		DrvNameLen = DISKNAMEMAXLEN;
		pDriveName = pDiskRequest->m_DriveName;
	}


	DbPrint(DC_LLDISKIO, DL_SPAM, ("DoDiskRequest - request %d (size %d : %d)\n", pInRequest->m_IOCTL, *pDataSize, SizeOfBuffer));

	if (pInRequest->m_IOCTL != _AVPG_IOCTL_DISK_QUERYNAMES)
	{
		pDriveName[DrvNameLen - 1] = L'0';
		
		DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - request for %S\n", pDriveName));

		if (pInRequest->m_IOCTL != _AVPG_IOCTL_DISK_QUERYHARDNAME)
		{
			pDevice = Disk_GetDeviceByName(pDriveName);

			if (pDevice == NULL)
			{
				DbPrint(DC_LLDISKIO, DL_WARNING, ("DoDiskRequest - device %S not found\n", pDriveName));
				
				if (pMdl)
				{
					MmUnlockPages(pMdl);
					IoFreeMdl(pMdl);
				}

				return STATUS_NOT_FOUND;
			}

			DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - found device %#x \n", pDevice));
		}
	}
		
	switch (pInRequest->m_IOCTL)
	{
// -----------------------------------------------------------------------------------------
	case _AVPG_IOCTL_DISK_QUERYHARDNAME:
		{
			// query har disk volume name form letter
			HANDLE				hDir;
			UNICODE_STRING		us;
			OBJECT_ATTRIBUTES	ObjAttr;

			RtlInitUnicodeString(&us, (PWCHAR)L"\\??");
			InitializeObjectAttributes(&ObjAttr, &us, OBJ_CASE_INSENSITIVE, NULL, NULL);
			ntStatus = ZwOpenDirectoryObject(&hDir, DIRECTORY_QUERY,&ObjAttr);
			if(!NT_SUCCESS(ntStatus))
			{
				DbPrint(DC_LLDISKIO, DL_ERROR, ("ZwOpenDirectoryObject %S failed, status %x\n",us.Buffer, ntStatus));
			}
			else
			{
				HANDLE hLink;
				WCHAR cDrive[3] = L"A:";
				cDrive[0] = pDriveName[0];
				RtlInitUnicodeString(&us, cDrive);
				
				InitializeObjectAttributes(&ObjAttr, &us, OBJ_CASE_INSENSITIVE, hDir, NULL);
				ntStatus = ZwOpenSymbolicLinkObject(&hLink, SYMBOLIC_LINK_QUERY, &ObjAttr);
				if(NT_SUCCESS(ntStatus))
				{
					WCHAR targetNameBuffer[260];
					UNICODE_STRING targetNameUnicodeString;

					RtlZeroMemory(targetNameBuffer, sizeof(targetNameBuffer));
					targetNameUnicodeString.Buffer = targetNameBuffer;
					targetNameUnicodeString.MaximumLength = sizeof(targetNameBuffer);
					ntStatus = ZwQuerySymbolicLinkObject(hLink, &targetNameUnicodeString, NULL);
					if(NT_SUCCESS(ntStatus))
					{
						if (*NtBuildNumber > 1381)
							ntStatus = FindHardDiskForPartition(&targetNameUnicodeString, pOutRequest, OutRequestSize, pRetSize);
						else
						{
							Uwcsncpy(pOutRequest, OutRequestSize, targetNameUnicodeString.Buffer, targetNameUnicodeString.Length);
							*pRetSize = wcslen(pOutRequest);
							((WCHAR*)pOutRequest)[*pRetSize - 1] = L'0';
							*pRetSize = (*pRetSize + 1) * sizeof(WCHAR);
						}
					}
					
					ZwClose(hLink);
				}
	
				ZwClose(hDir);
			}
		}
		break;
	case _AVPG_IOCTL_DISK_GET_GEOMETRY:
		if (pOutRequest == NULL || OutRequestSize < sizeof(DISK_GEOMETRY))
		{
			DbPrint(DC_LLDISKIO, DL_INFO, ("Get geometry failed. too small output buffer %d (required %d)\n", OutRequestSize, sizeof(DISK_GEOMETRY)));
			ntStatus = STATUS_BUFFER_TOO_SMALL;
		}
		else
		{
			*pRetSize = sizeof(DISK_GEOMETRY);
			DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - GET_GEOMETRY\n"));
			ntStatus = Disk_GetGeometry(pDevice, pOutRequest);
		}
		break;
// -----------------------------------------------------------------------------------------
	case _AVPG_IOCTL_DISK_GET_PARTITION_INFO:
		if (pOutRequest == NULL || OutRequestSize < sizeof(PARTITION_INFORMATION))
		{
			ntStatus = STATUS_BUFFER_TOO_SMALL;
			DbPrint(DC_LLDISKIO, DL_INFO, ("Get partition information failed. too small output buffer %d (required %d)\n", OutRequestSize, sizeof(PARTITION_INFORMATION)));
		}
		else
		{
			DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - GET_PARTITION_INFO\n"));
			*pRetSize = sizeof(PARTITION_INFORMATION);
			ntStatus = Disk_GetPartitionInfo(pDevice, pOutRequest);
		}
		break;
// -----------------------------------------------------------------------------------------
	case _AVPG_IOCTL_DISK_GET_DEVICE_TYPE:
		{
			if (pOutRequest == NULL || OutRequestSize < sizeof(DEVICE_TYPE))
			{
				ntStatus = STATUS_BUFFER_TOO_SMALL;
				DbPrint(DC_LLDISKIO, DL_INFO, ("Get device type failed. too small output buffer %d (required %d)\n", OutRequestSize, sizeof(DEVICE_TYPE)));
			}
			else
			{
				DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - GET_DEVICE_TYPE\n"));
				*pRetSize = sizeof(DEVICE_TYPE);
				ntStatus = Disk_GetDeviceType(pDevice, pOutRequest);
			}
		}
		break;
		// -----------------------------------------------------------------------------------------
	case _AVPG_IOCTL_DISK_GET_SECTOR_SIZE:
		{
			if (pOutRequest == NULL || OutRequestSize < sizeof(USHORT))
			{
				ntStatus = STATUS_BUFFER_TOO_SMALL;
				DbPrint(DC_LLDISKIO, DL_INFO, ("Get sector size failed. too small output buffer %d (required %d)\n", OutRequestSize, sizeof(USHORT)));
			}
			else
			{
				DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - GET_SECTOR_SIZE\n"));
				*pRetSize = sizeof(USHORT);
				ntStatus = Disk_GetSectorSize(pDevice, pOutRequest);
			}
		}
		break;
// -----------------------------------------------------------------------------------------
	case _AVPG_IOCTL_DISK_READ:
		{
			DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - DISK_READ\n"));
			ntStatus = ReadDiskSector(pDevice, ByteOffset, pDataSize, pOutRequest);
			if (NT_SUCCESS(ntStatus))
				*pRetSize = *pDataSize;
		}
		break;
// -----------------------------------------------------------------------------------------
	case _AVPG_IOCTL_DISK_WRITE:
		{
			*pRetSize = 0;
			DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - DISK_WRITE\n"));
			ntStatus = WriteDiskSector(pDevice, ByteOffset, pDataSize, pBuffer);
			if (NT_SUCCESS(ntStatus))
			{
				if (pOutRequest && sizeof(ULONG) == OutRequestSize)
				{
					*(ULONG*) pOutRequest = *pDataSize;
					*pRetSize = sizeof(ULONG);
				}
			}
		}
		break;
// -----------------------------------------------------------------------------------------		
	case _AVPG_IOCTL_DISK_QUERYNAMES:
		{
			DISKQUERYNAMES QueryNames;
			QueryNames.m_RetStatus = STATUS_SUCCESS;
			QueryNames.m_Bufferlength = OutRequestSize;
			QueryNames.m_CurrentDataSize = 0;
			QueryNames.m_pBuffer = pOutRequest;
			QueryNames.m_pNameCache = NULL;

			DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - DISK_QUERYNAMES\n"));

			RtlZeroMemory(pOutRequest, OutRequestSize);

			NameCacheInit(&QueryNames.m_pNameCache);
			
			EnumDrivers(Disk_FindDiskDev, &QueryNames);
				
			ntStatus = QueryNames.m_RetStatus;

			NameCacheDone(&QueryNames.m_pNameCache);
			
			if (NT_SUCCESS(ntStatus))
				*pRetSize = QueryNames.m_CurrentDataSize;
		}
		break;
// -----------------------------------------------------------------------------------------		
	case _AVPG_IOCTL_DISK_ISWRITEPROTECTED:
		DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - IsWriteProtected\n"));
		{
			BOOLEAN bProtected;
			if (NT_SUCCESS(IsMediaProtected(pDevice, &bProtected)))
			{
				ntStatus = STATUS_SUCCESS;
				*(ULONG*) pOutRequest = bProtected;
				*pRetSize = sizeof(ULONG);
			}
		}
		break;
	case _AVPG_IOCTL_DISK_ISPARTITION:
		DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - Ispartition\n"));
		if (pDevice->Characteristics & FILE_FLOPPY_DISKETTE)
			ntStatus = STATUS_UNSUCCESSFUL;
		else
		{
			if (_pfIoReadPartitionTableEx)
			{
				VOID* pInfomation;
				ntStatus = _pfIoReadPartitionTableEx(pDevice, &pInfomation);
				if (NT_SUCCESS(ntStatus))
					ExFreePool(pInfomation);
			}
			else
			{
				PDRIVE_LAYOUT_INFORMATION pInfomation = NULL;
				ntStatus = _pfIoReadPartitionTable(pDevice, pDevice->SectorSize, FALSE, &pInfomation);
				if (NT_SUCCESS(ntStatus))
					ExFreePool(pInfomation);
			}
		}
		break;
	case _AVPG_IOCTL_DISK_GETID:
		{
			DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - Get disk ID\n"));
			if (pOutRequest == NULL || OutRequestSize < sizeof(DISK_ID_INFO))
			{
				ntStatus = STATUS_BUFFER_TOO_SMALL;
				DbPrint(DC_LLDISKIO, DL_INFO, ("Get disk ID -> too small output buffer %d (required %d)\n", OutRequestSize, sizeof(DISK_ID_INFO)));
			}
			else
			{
				PDISK_ID_INFO pInfoRet = (PDISK_ID_INFO) pOutRequest;
				ntStatus = IdentifyDevice(pDevice, pInfoRet);
				if (NT_SUCCESS(ntStatus))
				{
					Disk_GetPartitionLinkName(pDriveName, pInfoRet->m_PartitionName, sizeof(pInfoRet->m_PartitionName));

					*pRetSize = sizeof(DISK_ID_INFO);
				}
			}
		}
		break;
	default:
		DbPrint(DC_LLDISKIO, DL_WARNING, ("Unknown request\n"));
		DbgBreakPoint();
		break;
	}

	if (pDevice != NULL)
		ObDereferenceObject(pDevice);

	DbPrint(DC_LLDISKIO, DL_SPAM, ("DoDiskRequest - result %#x\n", ntStatus));	

	if (pMdl)
	{
		MmUnlockPages(pMdl);
		IoFreeMdl(pMdl);
	}

	return ntStatus;
}