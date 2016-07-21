
#define CRC_MASK 0xFFFFFFFFL

#define ARJ_SIGNATURE 0xea60

#define FIRST_HDR_SIZE            30
#define FIRST_HDR_SIZE_V          34

#define EA_ID                    'E'    /* EA ID in extended header */
#define UXSPECIAL_ID             'U'    /* UNIX special file ID */
#define OWNER_ID                 'O'    /* Owner ID */
#define OWNER_ID_NUM             'o'    /* Numeric owner ID */
#define COMMENT_MAX             2048
#define EXTENSION_MAX              9    /* For internal procedures */
#define HEADERSIZE_MAX             (FIRST_HDR_SIZE+10+MAX_FILENAME_LEN+COMMENT_MAX)
#define CHAPTERS_MAX             250    /* Limited by 1 byte for chapter # */

#define STD_HDR_SIZE              30    /* Size of standard header */
#define R9_HDR_SIZE               46    /* Minimum size of header that holds DTA/DTC */

#define HEADERSIZE_MAX             (FIRST_HDR_SIZE+10+MAX_FILENAME_LEN+COMMENT_MAX)

#define GARBLED_FLAG            0x01
#define OLD_SECURED_FLAG        0x02
#define ANSICP_FLAG             0x02    /* Matches with the ARJSEC v 1.x */
#define VOLUME_FLAG             0x04
#define EXTFILE_FLAG            0x08
#define PROT_FLAG               0x08    /* Main header only (v 3.02+) */
#define PATHSYM_FLAG            0x10
#define BACKUP_FLAG             0x20
#define SECURED_FLAG            0x40
#define DUAL_NAME_FLAG          0x80    /* ARJ v 2.55+ ("-hf1" mode) */


#pragma pack(1)
typedef struct fir_header 
{
	tWORD sig;		//0
	tWORD size;		//2
	tBYTE first_size;//4
	tBYTE version;// arj_nbr 5
	tBYTE version_needed_to_extract;// arj_x_nbr 6
	tBYTE host_os; //7
	tBYTE arj_flags; //8	
	tBYTE security;      //==2//method 9
	tBYTE file_type;			//==2 a
	tBYTE password_modifier;	//b
	tWORD create_file_time;	//Format Depends on host OS	//c
	tWORD create_file_date;
	tWORD mod_file_time;
	tWORD mod_file_date;
	tDWORD secured_size;
	tDWORD arjsec_offset;	// file_crc
	tWORD filspec_pos;	// entry_pos
	tWORD length_secur_data;	// file_mode
	tBYTE ext_flags;
	tBYTE chapter_number;
}first_header;

typedef struct loc_header 
{
	tWORD sig;
	tWORD size;
	tBYTE first_size;
	tBYTE version;
	tBYTE version_needed_to_extract;
	tBYTE host_os;
	tBYTE arj_flags;
	tBYTE method;	//8
	tBYTE file_type;
	tBYTE password_modifier;
	tWORD mod_file_time;
	tWORD mod_file_date;
	tDWORD compressed_size;
	tDWORD original_size;
	tDWORD original_CRC;
	tWORD entry_pos;
	tWORD file_mode;// OS depend
	tWORD host_data;// arj_nbr < 7
	//tBYTE ext_flags;
	//tBYTE chapter_number;
}local_header;

#pragma pack()



#define	STORED    0
#define	DEFLATED1 1
#define	DEFLATED2 2
#define	DEFLATED3 3
#define	DEFLATED4 4

/*
	if(first)
	{
		arjprot_id=0;
	#if SFX_LEVEL>=ARJ
		prot_blocks=0;
	#endif
		if(first_hdr_size>=FIRST_HDR_SIZE_V)
		{
	#if SFX_LEVEL>=ARJ
			prot_blocks=hget_byte();
	#else
			hget_byte();
	#endif
			arjprot_id=hget_byte();
			hget_word();
			if(arjprot_id&SFXSTUB_FLAG)
				use_sfxstub=1;
		}
	}
	else
	{
		if(first_hdr_size<R9_HDR_SIZE)
		{
			if(arj_flags&EXTFILE_FLAG)
			{
				resume_position=hget_longword();
				continued_prevvolume=1;
				mvfile_type=file_type;
			}
		}
		else                                  // v 2.62+ - resume position is stored anyway, atime/ctime follows it.
		{
			resume_position=hget_longword();
			if(arj_flags&EXTFILE_FLAG)
			{
				continued_prevvolume=1;
				mvfile_type=file_type;
			}
			ts_store(&atime_stamp, host_os, hget_longword());
			ts_store(&ctime_stamp, host_os, hget_longword());
			hget_longword();                     // Reserved in revision 9 headers
		}
	}
*/
