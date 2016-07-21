// -------------------------------------------

#include <Prague/prague.h>
#include <Prague/pr_oid.h>
#include <klava/klav_props.h>
#include <klava/klav_prague.h>
#include <Extract/plugin/p_windiskio.h>

typedef enum _MEDIA_TYPE {
    Unknown,                // Format is unknown
    F5_1Pt2_512,            // 5.25", 1.2MB,  512 bytes/sector
    F3_1Pt44_512,           // 3.5",  1.44MB, 512 bytes/sector
    F3_2Pt88_512,           // 3.5",  2.88MB, 512 bytes/sector
    F3_20Pt8_512,           // 3.5",  20.8MB, 512 bytes/sector
    F3_720_512,             // 3.5",  720KB,  512 bytes/sector
    F5_360_512,             // 5.25", 360KB,  512 bytes/sector
    F5_320_512,             // 5.25", 320KB,  512 bytes/sector
    F5_320_1024,            // 5.25", 320KB,  1024 bytes/sector
    F5_180_512,             // 5.25", 180KB,  512 bytes/sector
    F5_160_512,             // 5.25", 160KB,  512 bytes/sector
    RemovableMedia,         // Removable media other than floppy
    FixedMedia,             // Fixed hard disk media
    F3_120M_512,            // 3.5", 120M Floppy
    F3_640_512,             // 3.5" ,  640KB,  512 bytes/sector
    F5_640_512,             // 5.25",  640KB,  512 bytes/sector
    F5_720_512,             // 5.25",  720KB,  512 bytes/sector
    F3_1Pt2_512,            // 3.5" ,  1.2Mb,  512 bytes/sector
    F3_1Pt23_1024,          // 3.5" ,  1.23Mb, 1024 bytes/sector
    F5_1Pt23_1024,          // 5.25",  1.23MB, 1024 bytes/sector
    F3_128Mb_512,           // 3.5" MO 128Mb   512 bytes/sector
    F3_230Mb_512,           // 3.5" MO 230Mb   512 bytes/sector
    F8_256_128              // 8",     256KB,  128 bytes/sector
} MEDIA_TYPE, *PMEDIA_TYPE;

// -------------------------------------------

void InitBootProps(cObj* obj, KLAV_IO_Object* pObj, KLAV_IO_Object*& pTmp)
{
	tDWORD Disk = 0x80;
	tDWORD Drive = CALL_SYS_PropertyGetDWord(obj, plDriveNumber);
	tDWORD Cylinders = 0;
	tDWORD TotalSectors = 0;
	tDWORD BytesPerSector = CALL_SYS_PropertyGetDWord(obj, plBytesPerSector);
	tDWORD SectorsPerTrack = CALL_SYS_PropertyGetDWord(obj, plSectorsPerTrack);
	tDWORD TracksPerCylinder = CALL_SYS_PropertyGetDWord(obj, plTracksPerCylinder);
	cObj*  PhysicalIO = NULL;

	tBOOL  bIsPartition = CALL_SYS_PropertyGetBool(obj, plIsPartition);

	if( !bIsPartition )
		Disk = CALL_SYS_PropertyGetDWord(obj, plDriveNumber);
		
	if( BytesPerSector == 0x200 )
	{
		tBYTE buf[0x200];
		tDWORD count;
		if( PR_SUCC(CALL_IO_SeekRead((hIO)obj,&count,0,buf,0x200)) )
		{
			if( count==0x200 && *(tWORD*)(buf + 0x1fe)==0xAA55 )
			{
				switch(buf[0x15])
				{
				case 0xf0:          // "high-density 3.5-inch floppy disk";
				case F5_1Pt2_512:   // "5.25, 1.2MB,  512 bytes/sector";
				case F3_1Pt44_512:  // "3.5,  1.44MB, 512 bytes/sector";
				case F3_2Pt88_512:  // "3.5,  2.88MB, 512 bytes/sector";
				case F3_720_512:    // "3.5,  720KB,  512 bytes/sector";
				case F5_360_512:    // "5.25, 360KB,  512 bytes/sector";
				case F5_320_512:    // "5.25, 320KB,  512 bytes/sector";
				case F5_180_512:    // "5.25, 180KB,  512 bytes/sector";
				case F5_160_512:    // "5.25, 160KB,  512 bytes/sector";
				case F3_640_512:    // "3.5 ,  640KB,  512 bytes/sector";
				case F5_640_512:    // "5.25,  640KB,  512 bytes/sector";
				case F5_720_512:    // "5.25,  720KB,  512 bytes/sector";
				case F3_1Pt2_512:   // "3.5 ,  1.2Mb,  512 bytes/sector";
					BytesPerSector = *(tWORD*)(buf+ 0x0b);
					SectorsPerTrack = *(tWORD*)(buf+ 0x18);
					TracksPerCylinder = *(tWORD*)(buf+ 0x1a);
					TotalSectors = *(tWORD*)(buf+ 0x13);
					
					if( TracksPerCylinder && SectorsPerTrack )
						Cylinders = TotalSectors/SectorsPerTrack/TracksPerCylinder;

					Drive = Disk = 0;
					PR_TRACE((g_root, prtIMPORTANT, "AVP1\tBootDisk - known format %x", (tDWORD)buf[0x15]));
					break;

				default:
					PR_TRACE((g_root, prtIMPORTANT, "AVP1\tBootDisk - unknown format %x", (tDWORD)buf[0x15]));
					break;
				}
			}
		}
	}

	if( bIsPartition && !TotalSectors )
	{
		cAutoObj<cObj> disk_io;
		if( PR_SUCC(obj->sysCreateObject(disk_io, IID_IO, PID_WINDISKIO)) )
		{
			cStrObj sName;
			sName.assign(obj, pgOBJECT_FULL_NAME);
			sName.copy(disk_io, pgOBJECT_FULL_NAME);

			CALL_SYS_PropertySetDWord(disk_io, pgOBJECT_ORIGIN, OID_PHYSICAL_DISK);

			if( PR_SUCC(disk_io->sysCreateObjectDone()) )
				PhysicalIO = disk_io.relinquish();
		}

		if( PhysicalIO )
		{
			BytesPerSector = CALL_SYS_PropertyGetDWord(PhysicalIO, plBytesPerSector);
			SectorsPerTrack = CALL_SYS_PropertyGetDWord(PhysicalIO, plSectorsPerTrack);
			TracksPerCylinder = CALL_SYS_PropertyGetDWord(PhysicalIO, plTracksPerCylinder);
		}
	}

	KLAV_Properties* io_props = pObj->get_properties();

	KLAV_Set_Property_Int(io_props, KLAV_PROPID_DISK, Disk);
	KLAV_Set_Property_Int(io_props, KLAV_PROPID_DISK_NUMBER, Drive);

	KLAV_Set_Property_Int(io_props, KLAV_PROPID_DISK_BYTES_PER_SECTOR, BytesPerSector);
	KLAV_Set_Property_Int(io_props, KLAV_PROPID_DISK_TRACKS_PER_CYLINDER, TracksPerCylinder);
	KLAV_Set_Property_Int(io_props, KLAV_PROPID_DISK_SECTORS_PER_TRACK, SectorsPerTrack);
	KLAV_Set_Property_Int(io_props, KLAV_PROPID_DISK_CYLINDERS, Cylinders);

	KLAV_Set_Property_Ptr(io_props, KLAV_PROPID_DISK_OBJECT_LOGICAL, pObj);

	if( PhysicalIO )
	{
		KLAV_Pr_Create_IO_Object(PhysicalIO, (hIO)PhysicalIO, false, &pTmp);
		KLAV_Set_Property_Ptr(io_props, KLAV_PROPID_DISK_OBJECT_PHYSICAL, pTmp);
	}
	else if( BytesPerSector )
		KLAV_Set_Property_Ptr(io_props, KLAV_PROPID_DISK_OBJECT_PHYSICAL, pObj);
}

void InitMbrProps(cObj* obj, KLAV_IO_Object* pObj, KLAV_IO_Object*& pTmp)
{
	tDWORD Disk = CALL_SYS_PropertyGetDWord(obj, plDriveNumber);
	tDWORD BytesPerSector = CALL_SYS_PropertyGetDWord(obj, plBytesPerSector);
	tDWORD SectorsPerTrack = 0;
	tDWORD TracksPerCylinder = 0;
	cObj*  LogicalIO = NULL;

	if( cFILE_DEVICE_DISK == CALL_SYS_PropertyGetDWord(obj, plDriveType)
	&& (cMEDIA_TYPE_FIXED == CALL_SYS_PropertyGetDWord(obj, plMediaType)
	||  cMEDIA_TYPE_REMOVABLE == CALL_SYS_PropertyGetDWord(obj, plMediaType) ))
		Disk |= 0x80;

	cAutoObj<cObj> disk_io;
	if( PR_SUCC(obj->sysCreateObject(disk_io, IID_IO, PID_WINDISKIO)) )
	{
		cStrObj sName;
		sName.assign(obj, pgOBJECT_FULL_NAME);
		sName.copy(disk_io, pgOBJECT_FULL_NAME);

		CALL_SYS_PropertySetDWord(disk_io, pgOBJECT_ORIGIN, OID_LOGICAL_DRIVE);

		if( PR_SUCC(disk_io->sysCreateObjectDone()) )
			LogicalIO = disk_io.relinquish();
	}

	if( LogicalIO )
	{
		if( !BytesPerSector )
			BytesPerSector = CALL_SYS_PropertyGetDWord(obj, plBytesPerSector);

		SectorsPerTrack = CALL_SYS_PropertyGetDWord(LogicalIO, plSectorsPerTrack);
		TracksPerCylinder = CALL_SYS_PropertyGetDWord(LogicalIO, plTracksPerCylinder);
	}

	KLAV_Properties* io_props = pObj->get_properties();

	KLAV_Set_Property_Int(io_props, KLAV_PROPID_DISK, Disk);
	KLAV_Set_Property_Int(io_props, KLAV_PROPID_DISK_NUMBER, Disk);

	KLAV_Set_Property_Int(io_props, KLAV_PROPID_DISK_BYTES_PER_SECTOR, BytesPerSector);
	KLAV_Set_Property_Int(io_props, KLAV_PROPID_DISK_TRACKS_PER_CYLINDER, TracksPerCylinder);
	KLAV_Set_Property_Int(io_props, KLAV_PROPID_DISK_SECTORS_PER_TRACK, SectorsPerTrack);

	KLAV_Set_Property_Ptr(io_props, KLAV_PROPID_DISK_OBJECT_PHYSICAL, pObj);

	if( LogicalIO )
	{
		KLAV_Pr_Create_IO_Object(LogicalIO, (hIO)LogicalIO, false, &pTmp);
		KLAV_Set_Property_Ptr(io_props, KLAV_PROPID_DISK_OBJECT_LOGICAL, LogicalIO);
	}
}

// -------------------------------------------
