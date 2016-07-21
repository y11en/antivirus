#if !defined( __zip_h__ )
#define __zip_h__

#define LOCAL_HDR_SIG             0x04034b50
#define UFO_HDR_SIG               0x08074b50
#define HEADER_FILE_SIG           0x02014b50
#define END_RECORD_SIG            0x06054b50
#define END64_RECORD_SIG          0x06064b50
#define END64_RECORD_LOCATOR_SIG  0x07064b50

#pragma pack(1)

typedef struct _tag_EndRecord64
{
	tDWORD sig;
	tQWORD qwSizeOfEndOfCentralDirRecord;
	tWORD  wVersion;
	tWORD  wVersionToExtract;
	tDWORD dwNumberOfThisDisk;
	tDWORD dwNumberOfDiskWithStartOfDir;
	tQWORD qwNumberOfEntriesOnThisDisk;
	tQWORD qwNumberOfEntries;
	tQWORD qwSizeOfCentralDirectory;
	tQWORD qwOffsetOfStartOfCentralDirecory;
}EndRecord64;

typedef struct _tag_EndOfCentralDirectoryLocator
{
	tDWORD sig;
	tDWORD dwNumberOfDiskWithStartOfDir;
	tQWORD qwOffsetOfStartOfCentralDirecory;
}EndRecordLocator64;

typedef struct _tag_LocalFileHeader 
{
	tDWORD sig; // local file header signature
	tWORD version_needed_to_extract;
/*
          The lower byte indicates the version number of the
          software used to encode the file.  The value/10
          indicates the major version number, and the value
          mod 10 is the minor version number.
*/
	tWORD general_purpose_bit_flag;
/*
          Bit 0: If set, indicates that the file is encrypted.

          (For Method 6 - Imploding)
          Bit 1: If the compression method used was type 6,
                 Imploding, then this bit, if set, indicates
                 an 8K sliding dictionary was used.  If clear,
                 then a 4K sliding dictionary was used.
          Bit 2: If the compression method used was type 6,
                 Imploding, then this bit, if set, indicates
                 3 Shannon-Fano trees were used to encode the
                 sliding dictionary output.  If clear, then 2
                 Shannon-Fano trees were used.

          (For Method 8 - Deflating)
          Bit 2  Bit 1
            0      0    Normal (-en) compression option was used.
            0      1    Maximum (-ex) compression option was used.
            1      0    Fast (-ef) compression option was used.
            1      1    Super Fast (-es) compression option was used.

          Note:  Bits 1 and 2 are undefined if the compression
                 method is any other.

          Bit 3: If this bit is set, the fields crc-32, compressed 
                 size and uncompressed size are set to zero in the 
                 local header.  The correct values are put in the 
                 data descriptor immediately following the compressed
                 data.  (Note: PKZIP version 2.04g for DOS only 
                 recognizes this bit for method 8 compression, newer 
                 versions of PKZIP recognize this bit for any 
                 compression method.)

          Bit 4: Reserved for use with method 8, for enhanced
                 deflating. 

          Bit 5: If this bit is set, this indicates that the file is 
                 compressed patched data.  (Note: Requires PKZIP 
                 version 2.70 or greater)

          Bit 6,7,8,9,10,11: Currently unused.

          Bit 12,13,14,15: Reserved by PKWARE for enhanced compression.
*/
	tWORD compression_method; // compression method
/*
          0 - The file is stored (no compression)
          1 - The file is Shrunk
          2 - The file is Reduced with compression factor 1
          3 - The file is Reduced with compression factor 2
          4 - The file is Reduced with compression factor 3
          5 - The file is Reduced with compression factor 4
          6 - The file is Imploded
          7 - Reserved for Tokenizing compression algorithm
          8 - The file is Deflated
          9 - Enhanced Deflating Deflate64
         10 - PKWARE Date Compression Library Imploding
*/
	tWORD last_mod_file_time; // -- 
	tWORD last_mod_file_date; // -- 
	tDWORD crc32; // -- 
	tDWORD compressed_size; // -- 
	tDWORD uncompressed_size; // -- 
	tWORD filename_length; // -- 
	tWORD extra_field_length; // -- 
/*
          The length of the filename, extra field, and comment
          fields respectively.  The combined length of any
          directory record and these three fields should not
          generally exceed 65,535 bytes.  If input came from standard
          input, the filename length is set to zero.
*/
} LocalFileHeader;

typedef struct _tag_DataDescriptor
{
	tDWORD Crc32;
	tDWORD CompressedSize;
	tDWORD UncompressedSize;
}DataDescriptor;

typedef struct _tag_FileHeader
{
	tDWORD entral_file_header_signature;
	tWORD  version_made_by;
/*
          0 - MS-DOS and OS/2 (FAT / VFAT / FAT32 file systems)
          1 - Amiga                     2 - VAX/VMS
          3 - Unix                      4 - VM/CMS
          5 - Atari ST                  6 - OS/2 H.P.F.S.
          7 - Macintosh                 8 - Z-System
          9 - CP/M                     10 - Windows NTFS
         11 thru 255 - unused
          The lower byte indicates the version number of the
          software used to encode the file.  The value/10
          indicates the major version number, and the value
          mod 10 is the minor version number.
*/
	tWORD  version_needed_to_extract;
	tWORD  general_purpose_bit_flag;
	tWORD  compression_method;
	tWORD  last_mod_file_time;
	tWORD  last_mod_file_date;
	tDWORD crc32;
	tDWORD compressed_size;
	tDWORD uncompressed_size;
	tWORD  filename_length;
	tWORD  extra_field_length;
	tWORD  file_comment_length;
/*
          The length of the filename, extra field, and comment
          fields respectively.  The combined length of any
          directory record and these three fields should not
          generally exceed 65,535 bytes.  If input came from standard
          input, the filename length is set to zero.
*/

	tWORD  disk_number_start;
	tWORD  internal_file_attributes;
/*
          The lowest bit of this field indicates, if set, that
          the file is apparently an ASCII or text file.  If not
          set, that the file apparently contains binary data.
          The remaining bits are unused in version 1.0.

          Bits 1 and 2 are reserved for use by PKWARE.
*/
	tDWORD external_file_attributes;
/*
          The mapping of the external attributes is
          host-system dependent (see 'version made by').  For
          MS-DOS, the low order byte is the MS-DOS directory
          attribute byte.  If input came from standard input, this
          field is set to zero.
*/
	tDWORD relative_offset_of_local_header;
/*
          This is the offset from the start of the first disk on
          which this file appears, to where the local header should
          be found.
*/
//	filename (variable size)
//	extra field (variable size)
//	file comment (variable size)
}FileHeader;

typedef struct _tag_EndRecord
{
	tDWORD end_of_dir_signature;
	tWORD  number_of_this_disk;
	tWORD  disk_with_start_of_the_central_directory;
//The number of the disk on which this file begins.
	tWORD  total_entries_on_this_disk;
	tWORD  total_entries;
	tDWORD size_of_the_central_directory;
	tDWORD offset_to_starting_directory;
	tWORD  comment_length;
//	zipfile comment (variable size)
}EndRecord;

#pragma pack()

#define STORED            0    /* compression methods */
#define SHRUNK            1
#define REDUCED1          2
#define REDUCED2          3
#define REDUCED3          4
#define REDUCED4          5
#define IMPLODED          6
#define TOKENIZED         7
#define DEFLATED          8
#define DEFLATED64        9
#define IMPLODE           10
#define NUM_METHODS       11    /* index of last method + 1 */


#endif //__zip_h__