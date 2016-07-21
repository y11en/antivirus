
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef void voidp;
typedef unsigned char  uchar;   /*  8 bits or more */
typedef unsigned int   uint;    /* 16 - 32 bits or more */
typedef unsigned short ushort;  /* 16 bits or more */
typedef unsigned long  ulong;   /* 32 bits or more */

#define CHAR_BIT  (8)
#define LONG_MAX  (0x7FFFFFFFL)
#define USHRT_BIT   (CHAR_BIT * sizeof(ushort))

#define OS                  0
#define PATH_SEPARATORS     "\\:"
#define PATH_CHAR           '\\'
#define MAXSFX              25000L
#define ARJ_SUFFIX          ".ARJ"

#define CODE_BIT          16
#define NULL_CHAR       '\0'
#define MAXMETHOD          4

#define ARJ_VERSION        3
#define ARJ_M_VERSION      6    /* ARJ version that supports modified date. */
#define ARJ_X_VERSION      3    /* decoder version */
#define ARJ_X1_VERSION     1
#define DEFAULT_METHOD     1
#define DEFAULT_TYPE       0    /* if type_sw is selected */
#define HEADER_ID     0xEA60
#define HEADER_ID_HI    0xEA
#define HEADER_ID_LO    0x60
#define FIRST_HDR_SIZE    30
#define FIRST_HDR_SIZE_V  34
#define COMMENT_MAX     2048
#define FNAME_MAX		 256
#define HEADERSIZE_MAX   (FIRST_HDR_SIZE + 10 + FNAME_MAX + COMMENT_MAX)
#define BINARY_TYPE        0    /* This must line up with binary/text strings */
#define TEXT_TYPE          1
#define COMMENT_TYPE       2
#define DIR_TYPE           3
#define LABEL_TYPE         4

#define GARBLE_FLAG     0x01
#define VOLUME_FLAG     0x04
#define EXTFILE_FLAG    0x08
#define PATHSYM_FLAG    0x10
#define BACKUP_FLAG     0x20

typedef ulong UCRC;     /* CRC-32 */

#define CRC_MASK        0xFFFFFFFFL

#define ARJ_PATH_CHAR   '/'

#define FA_RDONLY       0x01            /* Read only attribute */
#define FA_HIDDEN       0x02            /* Hidden file */
#define FA_SYSTEM       0x04            /* System file */
#define FA_LABEL        0x08            /* Volume label */
#define FA_DIREC        0x10            /* Directory */
#define FA_ARCH         0x20            /* Archive */

static long compsize;
static long origsize;
static ushort bitbuf;

static uchar subbitbuf;
static int bitcount;
static void NEAR  fillbuf(int n);
static void NEAR  init_getbits(void);

static void NEAR  _decode(void);
static void NEAR  decode_f(void);
//static void NEAR  _decode( int _f);

#define UCHAR_MAX    255

#define THRESHOLD    3
#define DDICSIZ      26624
//#define DDICSIZ      0x4000
#define MAXDICBIT   16
#define MATCHBIT     8
#define MAXMATCH   256
#define NC          (UCHAR_MAX + MAXMATCH + 2 - THRESHOLD)
#define NP          (MAXDICBIT + 1)
#define CBIT         9
#define NT          (CODE_BIT + 3)
#define PBIT         5
#define TBIT         5

#if NT > NP
#define NPT NT
#else
#define NPT NP
#endif

#define CTABLESIZE  4096
#define PTABLESIZE   256

#define STRTP          9
#define STOPP         13

#define STRTL          0
#define STOPL          7

/* Local functions */

static int    NEAR make_table(int nchar, uchar *bitlen, int tablebits, ushort FAR* table, int tablesize);
static void   NEAR read_pt_len(int nn, int nbit, int i_special);
static void   NEAR read_c_len(void);
static ushort NEAR decode_c(void);
static ushort NEAR decode_p(void);
static void   NEAR decode_start(void);
static short  NEAR decode_ptr(void);
static short  NEAR decode_len(void);

/* Local variables */

static uchar  *text;// DDICSIZ (24k)
static ushort *left;//[2 * NC - 1];
static ushort *right;//[2 * NC - 1];
static uchar  *c_len;//[NC];
static ushort *pt_table;//[PTABLESIZE];
static ushort FAR* c_table;//[CTABLESIZE];    (8k)

static uchar  pt_len[NPT];

static short  getlen;
static short  getbuf;
static ushort blocksize;
