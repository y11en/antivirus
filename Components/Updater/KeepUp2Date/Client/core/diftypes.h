
// difference block header
struct tDIFF_HDR
{
    tDIFF_HDR()
      : DiffCrc32(0),
        DiffSize(0),
        Version(0),
        Flags(0),
        BaseSize(0),
        CodeSize(0),
        TblUSize(0),
        TblUCrc32(0)
    {
        memset(Sign, 0, 4);
        memset(BaseHash, 0, 8);
        memset(CodeHash, 0, 8);
    }

	unsigned char Sign[4];                               /* DIFF signature          */
	unsigned int DiffCrc32;                             /* full diff checksum      */
	unsigned int DiffSize;                              /* full diff size          */
	unsigned short Version;                               /* version to extract      */
	unsigned short Flags;                                 /* project flags           */
	unsigned char BaseHash[8];                           /* hash of base file       */
	unsigned char CodeHash[8];                           /* hash of result file     */
	unsigned int BaseSize;                              /* base data size          */
	unsigned int CodeSize;                              /* encoded data size       */
	unsigned int TblUSize;                              /* table unpacked size     */
	unsigned int TblUCrc32;                             /* table unpacked checksum */
};


// magic number to distinguish difference for index compression algorithm
#define DIF_SIGNATURE 0x31444C4B // KLD1
#define DIF_SIGNATURE2 0x32444C4B // KLD2

// difference for index files compression header
struct tXML_DIFF_HDR
{
    tXML_DIFF_HDR()
      : from_ver(0),
        to_ver(0),
        size(0),
        incremental(0)
    {
    }

	long from_ver;
	long to_ver;
	unsigned long size;
	char incremental;
};


struct tDIFF_BIND_MHDR
{
    tDIFF_BIND_MHDR()
      : BDifCrc32(0),
        Version(0),
        Flags(0),
        BDifSize(0)
    {
        memset(Mark, 0, 4);
        *ItemSize = 0;
    }
	unsigned char Mark[4];                      /* marker "BDIF"      */
	unsigned int BDifCrc32;                    /* full bind checksum */
	unsigned short Version;                      /* bind version       */
	unsigned short Flags;                        /* bind flags         */
	unsigned int BDifSize;                     /* full bind size     */
	unsigned int ItemSize[1];                  /* array of dif sizes */
};


const unsigned char DiffMark[5] = "DIFF";
#define DIFF_VERSION          2     /* unpacker version */

const unsigned char DiffBDIFMark[5] = "BDIF";

#define DIFF_BIND_VERSION    1           /* current bind version */
#define DIFF_BIND_LASTENTRY  0xffffffff  /* last entry marker    */

#define SIGN_SIZE  64        /* old sign size */

/* diff values */
#define DIFF_VERSION          2     /* unpacker version                     */
#define DIFF_FLG_PACK         1     /* need pack to avc                     */
#define DIFF_FLG_NAMEOFF2LEN  2     /* offs in name block converted to lens */
#define DIFF_FLG_TBLPACK      4     /* diff tables was packed (by lzma now) */
#define DIFF_FLG_SIGN30       8     /* have v3.0 sign (64 bytes long)       */

// signature
static const unsigned char g_bKLZSign[4] = { 'K', 'L', 'Z', 'F' };

// header
struct KLZ_HDR
{
    KLZ_HDR()
      : m_unpackedSize(0),
        m_packedSize(0),
        m_packedBufferCRC(0),
        m_headerSize(0)
    {
        memset(m_magicNumberSign, 0, 4);
    }

    // magic number signature
	unsigned char m_magicNumberSign[4];
	unsigned int m_unpackedSize;
	unsigned int m_packedSize;
    // CRC32 checksum of packed data
	unsigned int m_packedBufferCRC;
    // sizeof(KLZ_HDR)
	unsigned char m_headerSize;
};
