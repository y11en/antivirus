/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: LibNotesMime.h 17650 2003-10-02 02:07:12Z wjb $
______________________________________________________________________________*/

#ifndef Included_LibNotesMime_h	/* [ */
#define Included_LibNotesMime_h

//#include "mimeods.h" For 602 API

//if applicable, include stuff of interest not in our version of the Notes C API
#ifndef TYPE_MIME_PART
#define TYPE_MIME_PART  25 + CLASS_NOCOMPUTE
typedef struct {
	WORD	wVersion;      /* MIME_PART Version */
	DWORD	dwFlags;
	#define MIME_PART_HAS_BOUNDARY			0x00000001
	#define MIME_PART_HAS_HEADERS			0x00000002
	#define MIME_PART_BODY_IN_DBOBJECT		0x00000004
	#define MIME_PART_SHARED_DBOBJECT		0x00000008	/*	Used only with MIME_PART_BODY_IN_DBOBJECT. */
	#define MIME_PART_SKIP_FOR_CONVERSION	0x00000010	/* only used during MIME->CD conversion */
	BYTE	cPartType;     /* Type of MIME_PART body */
	BYTE	cSpare;
	WORD	wByteCount;    /* Bytes of variable length part data
					NOT including data in DB object*/

	WORD	wBoundaryLen;  /* Length of the boundary string */
	WORD	wHeadersLen;   /* Length of the headers */
	WORD	wSpare;
	DWORD	dwSpare;

	/*	Followed by the actual part data */
} MIME_PART;
typedef enum tagMIMEPartType
	{
	MIME_PART_PROLOG		= 1,
	MIME_PART_BODY			= 2,
	MIME_PART_EPILOG		= 3,
	MIME_PART_RETRIEVE_INFO	= 4,
	MIME_PART_MESSAGE		= 5
	} MIMEPARTTYPE;
#endif

typedef struct	{
	MIME_PART * pt;				//address of the MIME_PART section of the 
						//	item's contents that describes the MIME-part 
						//	contained there
	const BYTE * puc_ctnt;		//address where the part's content begins 
	const BYTE * puc_typ;		//address where the value portion of the part's 
						//	"Content-Type" header begins (e.g. text/plain)
	const BYTE * puc_bdy;		//address where the body of the part's content 
						//	begins (the payload)
	const char * pc_objNm;		//address of the name of the attachment object 
						//	that holds the part's body content (allocated on 
						//	heap)
}  MimePrtItmCtnt;

typedef struct	{
	UINT  ui_len;				//length of part
	BYTE * puc_ctnt;			//address where the part's content begins
	BYTE * puc_bndry;			//address where the part's boundary string 
						//	begins
	WORD  us_lenBndry;			//length of boundary string
	BYTE * puc_typ;				//address where the value portion of the part's 
						//	"Content-Type" header begins (e.g. text/plain)
	BYTE * puc_atchNm;			//address where filename of attachment part 
						//	begins
	WORD  us_lenAtchNm;			//length of filename of attachment part
	BYTE * puc_encdg;			//address where the body content's 
						//	transfer-encoding is specified
	BYTE * puc_bdy;				//address where the body of the part's content 
						//	begins (the payload)
}  MimePrtCtnt;


extern char  epc_MIMHDR_CTNT_TYP[], epc_MIM_HDR_DLIM[], epc_MIM_BOUNDRY[], 
				epc_MIM_BNDRY_TACK_ON[], epc_MIM_CTNTENC_B64[];
extern WORD  eus_LEN_MIMHDR_CTNT_TYP, eus_LEN_MIM_HDRS_DLIM, 
				eus_LEN_MIM_BNDRY_TACK_ON, eus_LEN_MIM_BOUNDRY, 
				eus_LEN_MIM_CTNTENC_B64;


STATUS eus_MimePartItmInfo( const NOTEHANDLE, const char[], const WORD, 
										const BLOCKID, BLOCKID *const, 
										BLOCKID *const, MimePrtItmCtnt *const, 
										BLOCKID *const, BLOCKID *const);
STATUS eus_appendAtchPartToFile( void *const, const MimePrtItmCtnt *const, 
								const BLOCKID, const NOTEHANDLE, DWORD *const);
STATUS eus_MimePartInfo( const char[], const long, BufFile *const, 
									StringNode * *const, MimePrtCtnt *const);
STATUS eus_MimeAtchFileCtnt( const MimePrtCtnt *const, BufFile *const, 
											const NOTEHANDLE, char * *const);


#endif /* ] Included_LibNotesMime_h */
