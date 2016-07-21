/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: FileSystemHandling.c 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/

/*::: MODULE OVERVIEW :::::::::::::

--- revision history --------
3/14/03 Version 1.3 Paul Ryan
+ added buffered read-file functionality [see ef_bufferFileForRead() for 
  starters]
+ added ept_createTmpFile()

9/6/02 Version 1.1 Paul Ryan
+ added ef_WriteMemToFile()

9/16/00 Version 1.0.1 Paul Ryan
+ added ei_FileAccess() wrapper for ANSI access() function

8/6/99 Version 1.0 Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#include "FileSystemHandling.h"

//global-scope constants
const int  ei_ERR_ACCESS = EACCES, ei_ERR_EXIST = ENOENT, 
			ei_FILE_ACCS_EXIST = 0x0, ei_FILE_ACCS_WRITE = 0x2, 
			ei_FILE_ACCS_READ = 0x4, ei_FILE_ACCS_READWRITE = 0x6;

static const size_t  mui_LEN_IOBUF = 0x4000;


/** ef_bufferFileForRead( ***
Establish a buffered-read context for the specified file, and buffer the 
beginning of the file.

--- parameters & return ----
pfl: Optional Input & Output. Address of C run-time bufferred-file object, whose 
	information structure, opaque to caller, will be updated by this procedure. 
	If null, such an object will be formed based on the pc_EXT_FILENM input 
	instead.
pc_EXT_FILENM: Optional, Required if pfl Input not Provided. Address of extended 
	filename of file to be opened for reading. Parameter ignored if pfl input 
	provided.
UI: Optional. The size of the read buffer to allocate. If null, procedure will 
	create a default-sized buffer (currently 32K).
ui_PRFX: Optional. The size of an area of memory that will immediately precede 
	the read buffer. Used to allow client to precede a run of content 
	immediately with other content if wished such that no different buffer 
	need be specially made to accommodate that adjustment. Size limit of 
	MAXBYTE enforced.
pt: Output. Address of the information structure to receive a description of 
	the buffered file.
RETURN: TRUE if successful; FALSE if the memory buffer could not be allocated 
	or specified file opened

--- revision history -------
3/14/03 PR: created			*/
BOOL ef_bufferFileForRead( FILE * *const  ppfl, 
							const char  pc_EXT_FILENM[], 
							const  size_t  UI, 
							const  size_t  ui_PRFX, 
							BufFile *const  pt)	{
	const size_t  ui_LEN_BUF_DFLT = 16 * 0x400, 
					ui_LEN = UI ? UI : ui_LEN_BUF_DFLT;

	size_t  ui;

	if (!( (ppfl && *ppfl || pc_EXT_FILENM && *pc_EXT_FILENM) && pt && 
															ui_PRFX < MAXBYTE))
		return FALSE;

	memset( pt, (int) NULL, sizeof( BufFile));

	if (!( pt->puc = malloc( ui_LEN + ui_PRFX)))
		return FALSE;

	if (!ppfl)	{
		if (!( pt->pfl = fopen( pc_EXT_FILENM, "rb")))
			return FALSE;
	}else	{
		pt->pfl = *ppfl;
		*ppfl = NULL;
	}
	pt->puc_bgn = pt->puc_strt = pt->puc + ui_PRFX;
	pt->ui_lenPrfx = ui_PRFX;
	pt->ui_lenBuf = ui_LEN;

	//buffer the beginning of the file
	if (( ui = fread( pt->puc_strt, 1, pt->ui_lenBuf, pt->pfl)) < 
										pt->ui_lenBuf && ferror( pt->pfl))	{
		fclose( pt->pfl);
		return FALSE;
	}
	pt->puc_end = pt->puc_bgn + ui;
	pt->l_end = pt->l_bgn + ui;

	return TRUE;
} //ef_bufferFileForRead(


/** ef_bufFileClose( ***
Release resources associated with our bufferred file construct.

--- parameter & return ----
pt: address of information structure describing the bufferred file
RETURN: TRUE if successful; FALSE if input is obviously invalid or if assocated 
	C run-time file stream could not be closed.

--- revision history ------
3/14/03 PR: created			*/
BOOL ef_bufFileClose( BufFile *const  pt)	{
	if (!pt)
		return FALSE;

	free( pt->puc);
	return !fclose( pt->pfl);
} //ef_bufFileClose(


/** ef_bufFileCpyCtnt( ***
Copies a length of content from the file to the specified memory buffer.

--- parameters & return ----
l_BGN: starting coördinate in file from which to begin copying
ui_len: length of content to copy
puc: Output. Address of buffer in which to copy the content.
pt: Input & Output. Address of information structure describing the bufferred 
	file.
RETURN: length of content actually copied, less than that requested if 
	end-of-file encountered; i_FAIL if any input is obviously invalid or if any 
	I/O error is encountered

--- revision history -------
3/14/03 PR: created			*/
long el_bufFileCpyCtnt( const long  l_BGN, 
						size_t  ui_len, 
						BYTE *const  puc, 
						BufFile *const  pt)	{
	size_t  ui, ui_, ui_ofst = (size_t) NULL;
	long  l_eof;

	if (!( l_BGN >= 0 && puc && pt && pt->pfl))
		return ei_FAIL;

	//if start of the requested content resides in the current file buffer, 
	//	copy into memory buffer whatever content is present there
	if (pt->l_bgn <= l_BGN && l_BGN <= pt->l_end)	{
		ui_ = pt->l_end - pt->l_bgn + 1 < (int) ui_len ? pt->l_end - 
														pt->l_bgn + 1 : ui_len;
		memcpy( puc, pt->puc_bgn + (l_BGN - pt->l_bgn), ui_);
		if (!( ui_len -= ui_))
			return  ui_;
		ui_ofst = ui_;
	} //if (pt->l_bgn <= l_BGN && l_BGN <= pt->l_end)

	//copy in remaining requested content, in chunks if necessary...
	if (fseek( pt->pfl, 0, SEEK_END))
		return ei_FAIL;
	if (( l_eof = ftell( pt->pfl)) == -1)
		return ei_FAIL;
	if (fseek( pt->pfl, l_BGN + ui_ofst, SEEK_SET))
		return ei_FAIL;
	while (ui_len)	{
		//refresh the file buffer in full
		ui_ = ui_len < pt->ui_lenBuf ? ui_len : pt->ui_lenBuf;
		if (ui_ > l_eof - (l_BGN + ui_ofst))
			ui_ = l_eof - (l_BGN + ui_ofst);
		if (( ui = fread( pt->puc_strt, 1, pt->ui_lenBuf, pt->pfl)) < 
											pt->ui_lenBuf && ferror( pt->pfl))
			return ei_FAIL;
		if (!ui)
			return ui_ofst;
		pt->puc_bgn = pt->puc_strt;
		pt->puc_end = pt->puc_bgn + ui - 1;
		pt->l_bgn = l_BGN + ui_ofst;
		pt->l_end = pt->l_bgn + ui - 1;

		//add the new content to the buffer
		if (ui_ < ui)
			ui = ui_;
		memcpy( puc + ui_ofst, pt->puc_bgn, ui);
		ui_ofst += ui;

		//if we've run out of file, short-circuit with success
		if (ui < ui_)
			return ui_ofst;

		ui_len -= ui;
	} //while (ui_len)

	return ui_ofst;
} //el_bufFileCpyCtnt(


/** ef_bufFileSpliceToPrfx( ***
Moves a length of content from the file to the beginning of its file buffer so 
that caller may result the result as a single memory buffer.

--- parameters & return ----
l_BGN: starting coördinate of the length of content to position
ui_len: length of content to position
pt: Input & Output. Address of information structure describing the bufferred 
	file.
RETURN: TRUE if successful; else FALSE if specified length cannot be obtained 
	in full because end-of-file encountered, or if any input is obviously 
	invalid or any I/O error is encountered

--- revision history -------
3/14/03 PR: created			*/
BOOL ef_bufFileSpliceToPrfx( const long  l_BGN, 
								size_t  ui_len, 
								BufFile *const  pt)	{
	size_t  ui, ui_ = (size_t) NULL;

	if (!( l_BGN >= 0 && pt && pt->pfl && ui_len <= pt->ui_lenBuf))
		return FALSE;

	//if there's not enough content left in the file to complete caller's 
	//	request, short-circuit 
	//	with failure
	if (fseek( pt->pfl, 0, SEEK_END))
		return FALSE;
	if (l_BGN + (int) ui_len > ftell( pt->pfl))
		return FALSE;

	//if start of the requested content resides in the current file buffer...
	if (pt->l_bgn <= l_BGN && l_BGN <= pt->l_end)	{
		//move to beginning of the buffer whatever content is present therein
		ui_ = pt->l_end - l_BGN + 1;
		memmove( pt->puc_strt, pt->puc_bgn + (l_BGN - pt->l_bgn), ui_);
		pt->puc_bgn = pt->puc_strt;
		pt->puc_end = pt->puc_strt + ui_ - 1;
		pt->l_bgn = l_BGN;
		pt->l_end = l_BGN + ui_ - 1;

		//if the requested content is present in its entirety, update the 
		//	bufferred file descriptor and return success
		if (ui_len <= ui_)	{
			pt->puc_bgn = pt->puc_strt;
			pt->puc_end = pt->puc_strt + ui_ - 1;
			pt->l_bgn = l_BGN;
			pt->l_end = l_BGN + ui_ - 1;
			return  TRUE;
		} //if (ui_len <= ui_)
	} //if (l_BGN < l_bgn || l_BGN > l_end)

	//fill up the file buffer, thus appending any remaining requested content 
	//	not yet in the buffer, and update the file descriptor accordingly
	if (fseek( pt->pfl, l_BGN + ui_, SEEK_SET))
		return FALSE;
	if (( ui = fread( pt->puc_strt + ui_, 1, pt->ui_lenBuf - ui_, pt->pfl)) < 
										pt->ui_lenBuf - ui_ && ferror( pt->pfl))
		return FALSE;
	pt->puc_bgn = pt->puc_strt;
	pt->puc_end = pt->puc_bgn + ui - 1;
	pt->l_bgn = l_BGN;
	pt->l_end = l_BGN + ui - 1;

	return TRUE;
} //ef_bufFileSpliceToPrfx(


/** el_bufFileSeekStr( ***
Seek read-buffered file from given location for a specified string.

--- parameters & return ----
PC: address of the null-terminated string to look for
l_BGN: file byte-location from which to begin the seek
l_END: Optional. The file byte-location limit at which any searching should be 
	halted. If null, searching may continue through to end-of-file.
pt: Input & Output. Address of structure descrbing the read-buffered file we're 
	working with. Structure is updated as a result of this call (probably in a 
	way opaque to caller).
RETURN: the file byte-location where the sought string was found; ei_FAIL if 
	not found or if any input is obviously invalid

--- revision history -------
3/14/03 PR: created			*/
long el_bufFileSeekStr( const char  PC[], 
						const long  l_BGN, 
						const long  l_END, 
						BufFile *const  pt)	{
	const size_t  ui_LEN_STR = PC ? strlen( PC) : (size_t) NULL;

	size_t  ui, ui_len;
	BYTE * puc, * puc_end = NULL;
	char * pc;

	if (!( PC && *PC && l_BGN >= 0 && (l_END ? l_END >= l_BGN : TRUE) && pt && 
													pt->pfl && pt->puc && 
													ui_LEN_STR < pt->ui_lenBuf))
		return ei_FAIL;

	//if the specified begin point does not fall within the valid content in 
	//	the file buffer...
	if (l_BGN < pt->l_bgn || pt->l_end < l_BGN)	{
		//reload the buffer starting from the specified point
		if (fseek( pt->pfl, l_BGN, SEEK_SET))
			return ei_FAIL;
		ui = fread( pt->puc_strt, 1, pt->ui_lenBuf, pt->pfl);
		if (!ui || ui < pt->ui_lenBuf && ferror( pt->pfl))
			return ei_FAIL;
		pt->puc_end = (puc = pt->puc_bgn = pt->puc_strt) + ui - 1;
		pt->l_bgn = l_BGN;
		pt->l_end = l_BGN + ui - 1;
	//else we can use the current file buffer for at least some of the search...
	}else
		//note the point whence the buffer search should commence
		puc = pt->puc_bgn + (l_BGN - pt->l_bgn);

	//if a specified end point was given and falls within the current file 
	//	buffer, determine the effective end of the buffer for our purposes
	if (l_END && pt->l_bgn <= l_END && l_END <= pt->l_end)
		puc_end = pt->puc_bgn + (l_END - pt->l_bgn);

	//search the file onwards for the string of interest...
	do	{
		//if the start of a potential match is found...
		if (pc = memchr( puc, *PC, (puc_end ? puc_end : pt->puc_end) - puc + 1))
			//if there's enough content thereafter in the buffer to do a direct 
			//	comparison...
			if (pc + ui_LEN_STR <= (puc_end ? puc_end : pt->puc_end))	{
				//if we have a match...
				 if (memcmp( pc + 1, PC + 1, ui_LEN_STR - 1) == ei_SAME)
					//return the location where the match starts
					return pt->l_bgn + (pc - pt->puc_bgn);
				//else loop to begin searching further on in the buffer
				else	{
					puc = pc + 1;
					continue;
				} //if (memcmp( pc + 1, PC + 1,
			//else if there's no way a match can be had...
			}else if (puc_end)
				//return that no match was found
				return ei_FAIL;
			//else if we have a partial match through the end of the buffer...
			else if (pc == pt->puc_end || memcmp( pc + 1, PC + 1, 
											pt->puc_end - pc) == ei_SAME)	{
				//move the found portion to the beginning of the file buffer, 
				//	thereby noting where further buffer fulfillment should start
				memmove( pt->puc_strt, pc, pt->puc_end - pc + 1);
				pt->l_bgn += pc - pt->puc_bgn;
				pt->puc_bgn = pt->puc_strt;
				pt->puc_end = pt->puc_bgn + (pt->puc_end - pc);
			//else loop to begin searching further on in the buffer
			}else	{
				puc = pc + 1;
				continue;
			} //if (pc + ui_lenStr <= pt->puc_end ?
		//else if we've run out of content...
		else if (puc_end)
			//return that no match was found
			return ei_FAIL;
		//else note that we require a buffer reload
		else	{
			pt->puc_end = NULL;
			pt->l_bgn = pt->l_end + 1;
		} //if (pc = memchr( puc, *PC, (puc_end ?

		//reload the file buffer as needed from what's next in the file
		puc = pt->puc_end;
		ui_len = !puc ? pt->ui_lenBuf : pt->puc_bgn + pt->ui_lenBuf - puc;
		if (fseek( pt->pfl, pt->l_bgn, SEEK_SET))
			return ei_FAIL;
		ui = fread( !puc ? pt->puc_strt : puc + 1, 1, ui_len, pt->pfl);
		if (!ui || ui < pt->ui_lenBuf && ferror( pt->pfl))
			return ei_FAIL;
		pt->puc_bgn = pt->puc_strt;
		pt->puc_end = pt->puc_bgn + ui - 1;
		pt->l_end = pt->l_bgn + ui - 1;

		//if we've hit end-of-file, note the search's hard stop
		if (ui < pt->ui_lenBuf)
			puc_end = pt->puc_bgn + ui;

		//if a specified end point was given and falls within the latest file 
		//	buffer, determine the effective end of the buffer for our purposes
		if (l_END && pt->l_bgn <= l_END && l_END <= pt->l_end)
			puc_end = pt->puc_bgn + (l_END - pt->l_bgn);

		//if we are in the midst of a partial match...
		if (puc)	{
			//if there's not enough content to complete the match, return 
			//	failure
			ui = puc - pt->puc_strt + 1;
			if (puc + ui_LEN_STR - ui > (puc_end ? puc_end : pt->puc_end))
				return ei_FAIL;

			//if the match proves complete...
			if (memcmp( puc + 1, PC + ui, ui_LEN_STR - ui) == ei_SAME)
				//let caller know it
				return pt->l_bgn;
			//else note that the search should commence from the character 
			//	following the start of the match we were entertaining
			else
				puc++;
		//else note that the search should commence from the beginning of the 
		//	file buffer
		}else
			puc = pt->puc_bgn;
	} while (TRUE);
} //el_bufFileSeekStr(


/** ept_createTmpFile( ***
Creates a temporary file and optionally writes initial content into it.

--- parameters & return ----
PUC: Optional. Address of content buffer to write. If null, file is returned 
	empty.
UI: Optional, required if PUC provided. Length of content to write. Ignored if 
	PUC is not provided.
RETURN: handle to the binary read/write temporary file for use in further calls 
	to this module; NULL if any input is obviously invalid or if an error 
	occurred

--- revision history ------
3/14/03 PR: created			*/
FILE * ept_createTmpFile( const BYTE *const  PUC, 
							const size_t  UI)	{
	FILE * pfl;

	if (PUC && !UI)
		return NULL;

	pfl = tmpfile();

	if (PUC)
		if (fwrite( PUC, 1, UI, pfl) != UI)	{
			fclose( pfl);
			return NULL;
		}

	return pfl;
} //ept_createTmpFile(


/** ef_closeAndDeleteFile( ***
Close and delete a particular file from the file system.

--- parameters & return ----
pfl: address of object controlling the open stream-buffered file to be closed
pc_EXT_FILENM: address of file's extended filename
RETURN: TRUE if successful; FALSE if any input is obviously invalid or if an 
	error occurs

--- revision history ------
3/14/03 PR: created			*/
BOOL ef_closeAndDeleteFile( FILE *const  pfl, 
							const char  pc_EXT_FILENM[])	{
	int  i_err;

	if (!( pfl && pc_EXT_FILENM && *pc_EXT_FILENM))
		return FALSE;

	if (i_err = fclose( pfl))
		return FALSE;

	return remove( pc_EXT_FILENM) != ei_FAIL;
} //ef_closeAndDeleteFile(


/** ei_appendFileContent( ***
Append the specified content of a file to another file.

--- parameters & return ----
pfl_trgt: Optional Input & Output. Address of file object controlling the open 
	stream-buffered file to receive the source file's content. That content 
	will be appended to the file. Opaque file object will be updated as a 
	result of the call, with the write position at end-of-file. If null on 
	input, target file will be opened using the pc_TRGT input, and of course no 
	output will occur against this parameter.
pfl_src: Optional Input & Output. Address of file object controlling the open 
	stream-buffered file whose content will be appended to the target file. The 
	content to be appended will commence from the current read position on the 
	file, or at the beginning of the file if the read position is end-of-file. 
	Opaque file object will be updated as a result of the call, with the read 
	position at end-of-file. If null on input, source file will be opened using 
	the pc_SRC input, and of course no output will occur against this parameter.
pc_TRGT: Optional Input, Required if pfl_trgt Input not Provided. Address of 
	target file's filename. Ignored if pfl_trgt input provided. If not ignored, 
	file will be closed after use.
pc_SRC: Optional Input, Required if pfl_src Input not Provided. Address of 
	source file's filename. Ignored if pfl_src input provided. If not ignored, 
	file's content (up to length limit, if any) will be appended to the target 
	file, and then this source file will be closed.
UI: Optional. Length limit on the source content that will be added to the 
	target file.
RETURN: Length of content added to target file; ei_FAIL if any error occurred

--- revision history ------
3/14/03 PR: created			*/
int ei_appendFileContent( FILE * pfl_trgt, 
							FILE * pfl_src, 
							const char  pc_TRGT[], 
							const char  pc_SRC[], 
							const size_t  UI)	{
	const BOOL  f_SRC_FILE = !!pfl_src, f_TRGT_FILE = !!pfl_trgt;

	size_t  ui = (size_t) NULL, ui_len = (size_t) NULL;
	BYTE * puc = NULL;
	BOOL  f_fail;

	if (!( (pfl_trgt || pc_TRGT && *pc_TRGT) && (pfl_src || pc_SRC && *pc_SRC)))
		return ei_FAIL;

	//if no source-file object provided...
	if (!f_SRC_FILE)	{
		//open it by name for reading
		if (!( pfl_src = fopen( pc_SRC, "rb")))
			return ei_FAIL;
	//else if source file's read position is end-of-file, reset it to the 
	//	beginning of the file
	}else	{
		if (fseek( pfl_src, 0, SEEK_CUR))
			return ei_FAIL;
		if (getc( pfl_src) != EOF)	{
			if (fseek( pfl_src, -1, SEEK_CUR))
				return ei_FAIL;
		}else if (ferror( pfl_src) || fseek( pfl_src, 0, SEEK_SET))
			return ei_FAIL;
	} //if (!f_SRC_FILE)

	//if no target-file object provided...
	if (!f_TRGT_FILE)	{
		//open it by name for appendage
		if (f_fail = !( pfl_trgt = fopen( pc_TRGT, "ab")))
			goto errJump;
	//else ensure write location is at end-of-file
	}else
		if (f_fail = !!fseek( pfl_trgt, 0, SEEK_END))
			goto errJump;

	//create memory buffer to intermediate reading and writing
	if (f_fail = !( puc = malloc( UI && UI < mui_LEN_IOBUF ? UI : 
																mui_LEN_IOBUF)))
		goto errJump;

	//append the source content, in chunks as necessary...
	do	{
		//read in as much unread content as possible into the buffer, 
		//	respecting any specified limit
		const size_t  ui_READ = UI && UI - ui_len < mui_LEN_IOBUF ? UI - 
														ui_len : mui_LEN_IOBUF;
		if (f_fail = (ui = fread( puc, 1, ui_READ, pfl_src)) < ui_READ && 
															ferror( pfl_src))
			goto errJump;

		//write the bufferred content to the end of the target file
		if (f_fail = fwrite( puc, 1, ui, pfl_trgt) != ui)
			goto errJump;
		ui_len += ui;
	} while (!feof( pfl_src));

errJump:
	free( puc);

	//if no source- or target-file object provided, close the respective files
	if (!f_SRC_FILE)
		if (fclose( pfl_src))
			f_fail = TRUE;
	if (!f_TRGT_FILE && pfl_trgt)
		if (fclose( pfl_trgt))
			f_fail = TRUE;

	//return: failure if an error occurred, else the length of content actually 
	//	written
	return f_fail ? ei_FAIL : ui_len;
} //ei_appendFileContent(


/** ef_base64encodeFile( ***
Base64 encode a given file.

--- parameters & return ----
pc_SRC: address of extended filename telling the file to encode
pc_TRGT: Address of filename encoded file should receive. If a path-relative 
	filename, file will be placed relative to the directory in which the source 
	file resides.
ppfl_file: Optional Output. Address of pointer variable to receive address of 
	opaque structure describing the target file, opened in read-write binary 
	mode. If null, target file will be closed and this output suppressed.
RETURN: TRUE if source file was successfully encoded; FALSE if any input is 
	obviously invalid, or if given filenames can't be opened/created, or if 
	anything else file-I/O related goes awry

--- revision history ------
3/14/03 PR: created			*/
BOOL ef_base64encodeFile( const char  pc_SRC[], 
							const char  pc_TRGT[], 
							FILE * *const  ppfl_file)	{
	const size_t  ui_LEN_IOBUF = mui_LEN_IOBUF - mui_LEN_IOBUF % 76, 
					ui_LEN_MIM_B64_LINE = 76;

	FILE * pfl_src, * pfl_trgt;
	char  pc_trgt[ _MAX_PATH] = {(char) NULL};
	BYTE * puc_src, * puc_trgt = NULL;
	size_t  ui = (size_t) NULL;
	BOOL  f_fail = FALSE;

	if (!( pc_SRC && *pc_SRC && pc_TRGT && *pc_TRGT))
		return FALSE;

	if (ppfl_file)
		*ppfl_file = NULL;

	//open the source file in read mode
	if (!( pfl_src = fopen( pc_SRC, "rb")))
		return FALSE;

	//create the target file in read-write mode
	if (*pc_TRGT != ec_PATH_SPECIFIER && strncmp( pc_SRC, epc_PATH_UNC_PRFX, 
										eui_LEN_PATH_UNC_PRFX) != ei_SAME)	{ 
		char  pc_drv[ _MAX_DRIVE], pc_dir[ _MAX_DIR];

		_splitpath( pc_TRGT, pc_drv, pc_dir, NULL, NULL);
		if (!*pc_drv)	{
			_splitpath( pc_SRC, pc_drv, pc_dir, NULL, NULL);
			_makepath( pc_trgt, pc_drv, pc_dir, pc_TRGT, NULL);
		}
	} //if (strncmp( pc_TRGT, epc_PATH_UNC_PRFX
	if (!( pfl_trgt = fopen( *pc_trgt ? pc_trgt : pc_TRGT, "w+b")))
		goto errJump;

	//allocate a source buffer in which content to be encoded will be loaded
//suggested enhancement: figure file-length on the _stream_ and use that size 
//	if less than the constant
	if (!( puc_src = malloc( ui_LEN_IOBUF)))
		goto errJump;

	//perform the encoding, in chunks as necessary...
	do	{
		size_t  ui_;

		//load content to be encoded into the source buffer
		if (( ui_ = fread( puc_src, 1, ui_LEN_IOBUF, pfl_src)) < 
											ui_LEN_IOBUF && ferror( pfl_src))
			goto errJump;

		//encode the buffer
		if (!f_base64encode( &puc_trgt, &ui, puc_src, ui_))
			goto errJump;

		//append the encoded content to the target file, line by line
		ui_ = 0;
		do	{
			if (ui < ui_LEN_MIM_B64_LINE)	{
				if (fwrite( puc_trgt + ui_, 1, ui, pfl_trgt) != ui)
					goto errJump;
				ui = 0;
			}else	{
				if (fwrite( puc_trgt + ui_, 1, ui_LEN_MIM_B64_LINE, 
											pfl_trgt) != ui_LEN_MIM_B64_LINE)
					goto errJump;
				if (fwrite( epc_CRLF, 1, eui_LEN_CRLF, pfl_trgt) != 
																eui_LEN_CRLF)
					goto errJump;
				ui_ += ui_LEN_MIM_B64_LINE;
				ui -= ui_LEN_MIM_B64_LINE;
			} //if (ui < ui_LEN_MIM_B64_LINE)
		} while (ui);

		//free the buffer holding the now-unneeded encoded content
		free( puc_trgt);
		puc_trgt = NULL;
	} while (!feof( pfl_src));

	//free the source buffer
	free( puc_src);

	//if caller has requested an open target file...
	if (ppfl_file)	{
		//deliver it with file-position set to the start of the file
		if (!( f_fail = fseek( pfl_trgt, 0, SEEK_SET)))
			*ppfl_file = pfl_trgt;
	//else close it
	}else
		f_fail = fclose( pfl_trgt);

	//close the source file and return
	return fclose( pfl_src) || f_fail ? FALSE : TRUE;

errJump:
	if (puc_src)
		free( puc_src);
	if (puc_trgt)
		free( puc_trgt);
	fclose( pfl_src);
	if (pfl_trgt)
		fclose( pfl_trgt);

	return FALSE;
} //ef_base64encodeFile(


/** ept_openFile( ***
Open a particular file in a given mode. Largely a wrapper for the C run-time 
fopen().

--- parameters & return ----
pc_EXT_FILENM: address of extended filename telling the file to open
pc_MODE: Address of string telling how file should be opened. Acceptable values 
	are those used with the C run-time fopen() function.
RETURN: handle to the opened file for use in further calls to this module; null 
	if any input is obviously invalid or if an error occurred

--- revision history ------
3/14/03 PR: created			*/
FILE * ept_openFile( const char  pc_EXT_FILENM[], 
						const char  pc_MODE[])	{
	if (!( pc_EXT_FILENM && *pc_EXT_FILENM && pc_MODE && *pc_MODE))
		return NULL;

	return fopen( pc_EXT_FILENM, pc_MODE);
} //ept_openFile(


/** el_addToOpenFile( ***
Writes the specified buffer to the C run-time file's current location.

--- parameters & return ----
pfl: Input & Output. Address of C run-time file descriptor, whose opaque 
	information structure will be updated by this procedure.
PUC: address of content buffer to write
UI: length of content to write
f_FORCE_END: tells whether the file's file-position should be forced to the 
	end of the file before conducting the write (TRUE) or the current 
	file-position should be used (FALSE)
RETURN: if successful, file-position at which write began; ei_FAIL if any input 
	is obviously invalid or if an error occurred while writing the file

--- revision history -------
3/14/03 PR: created			*/
long el_addToOpenFile( FILE *const  pfl, 
						const BYTE *const  PUC, 
						const size_t  UI, 
						const BOOL  f_FORCE_END)	{
	long  l;

	if (!( pfl && PUC && UI))
		return ei_FAIL;

	if (f_FORCE_END)
		if (fseek( pfl, 0, SEEK_END))
			return ei_FAIL;

	if (( l = ftell( pfl)) < 0)
		return ei_FAIL;
	if (fwrite( PUC, 1, UI, pfl) != UI)
		return ei_FAIL;

	return l;
} //el_addToOpenFile(


/** ef_closeFile( ***
Close a C run-time file-stream file.

--- parameter & return ----
ppfl: Input & Output. Address of C run-time file-stream pointer. Procedure 
	nullifies the pointer.
RETURN: FALSE if any error occurs, including if input is obviously invalid; 
	else TRUE

--- revision history ------
3/14/03 PR: created			*/
BOOL ef_closeFile( FILE * *const  ppfl)	{
	if (!( ppfl && *ppfl))
		return FALSE;

	if (fclose( *ppfl))
		return FALSE;

	*ppfl = NULL;

	return TRUE;
} //ef_closeFile(


/** ef_writeToNewFile( ***

--- parameters & return ----

--- revision history -------
3/14/03 PR: added parameter to allow output of file object, instead of closing 
	file after the initial write
9/6/02 PR: created			*/
//DOC!!
BOOL ef_writeToNewFile( const unsigned char *const  PUC, 
						const unsigned long  UL, 
						const char PC[], 
						FILE * *const  ppfl)	{
	FILE * pfl;
	BOOL  f_fail;

	if (!( PUC && PC))
		return FALSE;

	if (ppfl)
		*ppfl = NULL;

	if (!( pfl = fopen( PC, "wb")))
		return FALSE;

	fwrite( PUC, 1, UL, pfl);
	f_fail = ferror( pfl);

	if (ppfl)
		*ppfl = pfl;
	else
		if (fclose( pfl) == EOF && !f_fail)
			f_fail = TRUE;

	return !f_fail;
} //ef_writeToNewFile(


/** ei_FileAccess( ***
Tells whether the caller can obtain a particular access level to the specified 
file.

--- parameter & return ----
PC: extended filename of the file whose access level is to be tested
I: access level to be tested, choices are the ei_FILE_ACCS_* constants, 
	mirroring those used with the ANSI access() function
f_EXCL: Flag telling whether exclusive access is to be tested. Only relevant if 
	the access level being tested is beyond mere file existence.
RETURN:
	eus_SUCCESS if the file has the specified access level
	eus_ERR_INVLD_ARG if any input parameter is obviously invalid
	ei_ERR_ACCESS if the file's permission setting does not allow the specified 
		access level
	ei_ERR_EXIST if the specified file was not found on the file system

--- revision history ------
3/14/03 PR: listing format adjustment
9/16/00 PR: created			*/
int ei_FileAccess( const char  PC[], 
					const int  I, 
					const BOOL  f_EXCL)	{
	int  i;

	if (!( PC && I >= ei_FILE_ACCS_EXIST && I <= ei_FILE_ACCS_READWRITE))
		return eus_ERR_INVLD_ARG;

	if (access( PC, I) == ei_FAIL)
		return errno;

	if (I && f_EXCL)
		if ((i = sopen( PC, _O_RDONLY, _SH_DENYRW)) == ei_FAIL)
			return errno;
		else
			close( i);

	return eus_SUCCESS;
} //ei_FileAccess(


/** epc_getTempDirNm( ***
Fills the specified buffer with the name of the system's temporary directory, 
ending with the path-specifier character (e.g. backslash in Win32).

--- parameters & return ----
pc_buf: address of the pre-allocated buffer
ui_LEN_BUF: the amount of space in the buffer
RETURN: pc_buf if successful, else NULL

--- revision history -------
3/14/03 PR: minor documentation adjustment, token renaming, listing format 
	adjustment
8/1/99 PR: created			*/
char * epc_getTempDirNm( char *const  pc_buf, 
							const unsigned int  ui_LEN_BUF)	{
	const char *const  pc_ENV_TMP = getenv( "TEMP");

	if (!( pc_buf && ui_LEN_BUF))
		return NULL;

	if (strlen( pc_ENV_TMP) + strlen( epc_PATH_SPECIFIER) > ui_LEN_BUF - 1)
		return NULL;
	strcat( strcpy( pc_buf, pc_ENV_TMP), (const char *) (strrchr( 
									pc_ENV_TMP, ec_PATH_SPECIFIER) != 
									pc_ENV_TMP + strlen( pc_ENV_TMP) - 1 ? 
									(int) epc_PATH_SPECIFIER : (int) epc_NULL));

	return pc_buf;
} //epc_getTempDirNm(


/** ef_LoadBinaryFile( ***

--- parameters & return ----

--- revision history -------
8/1/99 PR: created			*/
//DOC!!
BOOL ef_LoadBinaryFile( const char  pc_FILENM[], 
						void *const  pv, 
						const int  i_LEN)	{
	FILE * pfl;
	char * pc = NULL;
	BOOL  f_failure;

	if (!( pc_FILENM && pv && i_LEN))
		return FALSE;

	if (!( pfl = fopen( pc_FILENM, "rb")))
		return FALSE;

	if (f_failure = filelength( fileno( pfl)) < i_LEN)
		goto errJump;

	fread( pv, 1, i_LEN, pfl);
	f_failure = ferror( pfl);

errJump:
	if (fclose( pfl) == EOF)
		if (!f_failure)
			f_failure = TRUE;

	return !f_failure;
} //ef_LoadBinaryFile(


/** el_fileLength( ***
Determines the length of a file on the filesystem, specified via a C Run-Time 
file-stream object.

--- parameter & return ----
pfl: address of C Run-Time file-stream object
RETURN: length of the file; ei_FAIL if the input is obviously invalid or if an 
	error is encountered

--- revision history ------
3/14/03 PR: created			*/
long ei_fileLength( FILE *const  pfl)	{
	if (!pfl)
		return ei_FAIL;
	return filelength( fileno( pfl));
} //ei_fileLength(


/** ef_LoadTextFileEntire( ***

--- parameters & return ----

--- revision history -------
8/1/99 PR: created			*/
//DOC!!
/*BOOL ef_LoadTextFileEntire( const char  pc_FILENM[], 
							char * *const  ppc)	{
	FILE * pfl;
	int  i;
	char * pc = NULL;
	BOOL  f_failure;

	if (!( pc_FILENM && ppc))
		return FALSE;

	*ppc = NULL;

	if (!( pfl = fopen( pc_FILENM, "rt")))
		return FALSE;

	if (f_failure = (i = filelength( fileno( pfl))) < 0)
		goto errJump;

	if (i)	{
		if (f_failure = !( pc = malloc( i + 1)))
			goto errJump;

		i = fread( pc, 1, i, pfl);
		if (f_failure = ferror( pfl))
			goto errJump;
		pc[ i] = NULL;
	} //if (i)

errJump:
	if (fclose( pfl) == EOF)
		if (!f_failure)
			f_failure = TRUE;

	if (pc)
		if (!f_failure)
			*ppc = pc;
		else 
			free( pc);

	return !f_failure;
} //ef_LoadTextFileEntire(
*/

