//ISO 9660 Structures
// types
#define __uint8		unsigned __int8 
#define __uint16	unsigned __int16
#define __uint32	unsigned __int32
#define __uint64	unsigned __int64

//+ ------------------------------------------------------------------------------------------
// iso standart defines
#define _cd_sector_size							2048

#define _vtoc_start_sector						16
#define _vtoc_volume_descriptor_set_terminator	(__uint8)0xff
#define _vtoc_boot_record						(__uint8)0x00

#define _validation_key_C						(__uint8)0x43
#define _validation_key_D						(__uint8)0x44
#define _validation_key_55						(__uint8)0x55
#define _validation_key_aa						(__uint8)0xaa

#define _section_header_indicator				(__uint8)0x90
#define _last_section_header					(__uint8)0x91

#define _section_boot_inidicator				(__uint8)0x88

#include <pshpack1.h>

typedef struct _t_date_and_time_
{
	__uint8			year[4];
	__uint8			month[2];
    __uint8			day[2];
    __uint8			hour[2];
    __uint8			minute[2];
    __uint8			second[2];
    __uint8			hundredths_of_seconds[2];
    __uint8			offset_from_greenwich_mean_time;

}_date_and_time_;

typedef struct _t_rdr_
{
	__uint8			length_of_directory_record;
	__uint8			extended_attribute_record_length;
	__uint64		location_of_extent;
	__uint64		data_length;
	__uint8			year_since_1900;
	__uint8			month;
	__uint8			day;
	__uint8			hour;
	__uint8			minute;
	__uint8			second;
	__uint8			greenwich_offset;
	__uint8			file_flags;
	__uint8			file_unit_size;
	__uint8			interleave_gap_size;
	__uint32		volume_sequence_number;
	__uint8			length_of_file_dentifier;
	__uint8			padding_field;
}_rdr_;

typedef struct _t_cd_vtoc_
{
	__uint8			volume_descriptor_type;
	__uint8			standard_identifier[5];
	__uint8			volume_descriptor_version;
	__uint8			volume_flags;
	__uint8			system_identifier[32];
	__uint8			volume_identifier[32];
	__uint8			unused_field[8];
	__uint64		volume_space_size;
	__uint8			escape_sequences[32];
	__uint32		volume_set_size;
	__uint32		volume_sequence_number;
	__uint32		logical_block_size;
	__uint64		path_table_size;
	__uint32		location_of_type_l_path_table;
	__uint32		location_of_optional_type_l_path_table;
	__uint32		location_of_type_m_path_table;
	__uint32		location_of_optional_type_m_path_table;
	_rdr_			directory_record_for_root_directory;
	__uint8			volume_set_identifier[128];
	__uint8			publisher_identifier[128];
	__uint8			data_preparer_identifier[128];
	__uint8			application_identifier[128];
	__uint8			copyright_file_identifier[37];
	__uint8			abstract_file_identifier[37];
	__uint8			bibliographic_file_identifier[37];
	_date_and_time_	volume_creation_date_and_time;
	_date_and_time_	volume_modification_date_and_time;
	_date_and_time_	volume_expiration_date_and_time;
	_date_and_time_	volume_effective_date_and_time;
	__uint8			file_structure_version;
	__uint8			reserved_for_future_standardization;
	__uint8			application_use[512];
	__uint8			reserved_for_future_standardization2[653];
}_cd_vtoc;

//+ ------------------------------------------------------------------------------------------
//el torito specifications
typedef struct _t_boot_record_volume_descriptor
{
	__uint8			boot_record_indicator;		// must be 0
	__uint8			iso_9660_identifier[5];		// must be “CD001”
	__uint8			version_of_this_descriptor;	// must be 1
	__uint8			boot_system_identifier[32];	// must be “EL TORITO SPECIFICATION” padded with 0’s
	__uint8			unused[32];					// must be 0
	__uint32		absolute_pointer_to_first_sector_of_boot_catalog;
	__uint8			unused2[1973];				// must be 0
}_boot_record_volume_descriptor;

typedef struct _t_validation_entry
{
	__uint8			header_id;					// must be 01
	__uint8			platform_id;				// 0 = 80x86 1=Power PC 2=Mac
	__uint16		reserverd;					// must be 0
	__uint8			id_string[24];				// This is intended to identify the manufacturer/developer of the CD-ROM
	__uint16		checksum_word;				// This sum of all the words in this record should be 0.
	__uint8			key_byte_55;				// must be 55. This value is included in the checksum.
	__uint8			key_byte_aa;				// must be AA. This value is included in the checksum.
}_validation_entry;

typedef struct _t_initial_default_entry
{
	__uint8			boot_indicator;				// 88 = Bootable, 00 = Not Bootable
	__uint8			boot_media_type;			// This specifies what media the boot image is intended to
												// emulate in bits 0-3 as follows, bits 4-7 are reserved and must be 0.
												// Bits 0-3 count as follows:
												// 0 No Emulation / 1 1.2 meg diskette / 2 1.44 meg diskette 
												// 3 2.88 meg diskette / 4 Hard Disk (drive 80) / 5-F Reserved, invalid at this time
	__uint16		load_segment;				// If this value is 0 the system will use the traditional segment of 7C0
	__uint8			system_type;				// This must be a copy of byte 5 (System Type) from the  Partition Table found in the boot image
	__uint8			unused;						// must be 0
	__uint16		sector_count;				// This is the number of virtual/emulated sectors the system will store at Load Segment during the initial boot procedure.
	__uint32		load_rba;					// This is the start address of the virtual disk. CD’s use Relative/Logical block addressing.
	__uint8			unused2[20];				// must be 0
}_initial_default_entry;

typedef struct _t_section_header_entry
{
	__uint8			header_indicator;			// 90 - Header, more headers follow / 91 - Final Header
	__uint8			platform_id;				// 0 = 80x86 / 1=Power PC / 2=Mac
	__uint16		number_of_section_entries_following_this_header;
	__uint8			id_string[28];				//
}_section_header_entry;

typedef struct _t_section_entry
{
	__uint8			boot_indicator;				// 88 = Bootable, 00 = Not Bootable
	__uint8			boot_media_type;			// This specifies what media the boot image is intended to
												// emulate in bits 0-3 as follows, bits 4-7 are reserved and must be 0.
												// Bits 0-3 count as follows:
												// 0 No Emulation / 1 1.2 meg diskette / 2 1.44 meg diskette 
												// 3 2.88 meg diskette / 4 Hard Disk (drive 80) / 5-F Reserved, invalid at this time
	__uint16		load_segment;				// If this value is 0 the system will use the traditional segment of 7C0
	__uint8			system_type;				// This must be a copy of byte 5 (System Type) from the  Partition Table found in the boot image
	__uint8			unused;						// must be 0
	__uint16		sector_count;				// This is the number of virtual/emulated sectors the system will store at Load Segment during the initial boot procedure.
	__uint32		load_rba;					// This is the start address of the virtual disk. CD’s use Relative/Logical block addressing.
	__uint8			selection_criteria_type;	//This defines a vendor unique format for bytes 0D-1F.
												// The following formats have currently been assigned:
												// 0 - No selection criteria
												// 1- Language and Version Information (IBM)
												// 2-FF - Reserved
	__uint8			vendor_unique_selection_criteria[19];
}_section_entry;

#include <poppack.h>

