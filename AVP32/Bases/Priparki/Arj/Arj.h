#define INBUFSIZ  PAGE_A_SIZE
#define OUTBUFSIZ         INBUFSIZ

#define CRC_MASK 0xFFFFFFFFL


typedef struct ini_header {
	   WORD sig;
	   WORD size;
}init_header;

typedef struct fir_header {
	   uchar first_size;
	   uchar version;
	   uchar version_needed_to_extract;
	   uchar host_os;
	   uchar flags;
	   uchar security;      //==2
	   uchar type;			//==2
	   uchar reserv1;
	   WORD create_file_time;
	   WORD create_file_date;
	   WORD mod_file_time;
	   WORD mod_file_date;
	   ulong file_size;
	   ulong secur_pos;
	   WORD filspec_pos;
	   WORD length_secur_data;
}first_header;

typedef struct loc_header {
	   uchar first_size;
	   uchar version;
	   uchar version_needed_to_extract;
	   uchar host_os;
	   uchar flags;
	   uchar method;
	   uchar type;
	   uchar reserv1;
	   WORD mod_file_time;
	   WORD mod_file_date;
	   ulong compressed_size;
	   ulong original_size;
	   ulong original_CRC;
	   WORD filspec_pos;
	   WORD access_mode;
}local_header;

//static local_header*	headerl;
//static first_header*	headerf;
//static init_header*	headeri;

static uchar *inbuf;
static uchar *outbuf;

static long last_len;

#define fwrite_txt_crc Write
#define	disp_clock() Rotate(1)
#define	headerl	((local_header*)Page_C)
#define	headerf	((first_header*)Page_C)
#define	headeri	((init_header*)Page_C)




#define	STORED    0
#define	DEFLATED1 1
#define	DEFLATED2 2
#define	DEFLATED3 3
#define	DEFLATED4 4


#ifdef _DEBUG
static char *meth[]={
"STORED   ",
"DEFLATED1",
"DEFLATED2",
"DEFLATED3",
"DEFLATED4"};
#endif
