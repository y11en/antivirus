/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: MimeHandling.c 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/

/*::: MODULE OVERVIEW :::::::::::::
Provides services for manipulating MIME in a Lotus Notes context.

--- revision history --------
3/14/03 Version 1.0 Paul Ryan	*/

#include "MimeHandling.h"


const char  epc_MIMHDR_CTNT_TYP[] = "Content-Type: ", 
			epc_MIMHDR_CTNT_ENC[] = "Content-Transfer-Encoding: ", 
			epc_MIM_CTNTENC_B64[] = "base64", 
			epc_MIMHDR_CTNT_ATCH[] = "Content-Disposition: attachment;", 
			epc_MIM_HDRS_DLIM[] = "\r\n\r\n", 
			epc_MIM_BOUNDRY[] = "boundary=\"", 
			epc_MIM_BNDRY_TACK_ON[] = "--";
const WORD  eus_LEN_MIMHDR_CTNT_TYP = sizeof( epc_MIMHDR_CTNT_TYP) - 1, 
			eus_LEN_MIMHDR_CTNT_ENC = sizeof( epc_MIMHDR_CTNT_ENC) - 1, 
			eus_LEN_MIM_CTNTENC_B64 = sizeof( epc_MIM_CTNTENC_B64) - 1, 
			eus_LEN_MIMHDR_CTNT_ATCH = sizeof( epc_MIMHDR_CTNT_ATCH) - 1, 
			eus_LEN_MIM_HDRS_DLIM = sizeof( epc_MIM_HDRS_DLIM) - 1, 
			eus_LEN_MIM_BNDRY_TACK_ON = sizeof( epc_MIM_BNDRY_TACK_ON) - 1, 
			eus_LEN_MIM_BOUNDRY = sizeof( epc_MIM_BOUNDRY) - 1;

static const char  mpc_MIMHDR_ATCH_FILENM[] = "filename=\"";
static const WORD  mus_LEN_MIMHDR_ATCH_FILENM = 
											sizeof( mpc_MIMHDR_ATCH_FILENM) - 1;


/** eus_MimePartItmInfo( ***
Provides certain requested information about a particular MIME-part item in a 
note.

--- parameters & return ----
h_NOTE: handle to the note containing the MIME content
pc_ITMNM: address of the name of the item containing the MIME content
us_LEN_ITMNM_: Optional. Length of the item's name. If null, procedure will 
	assume the name to be null-terminated and will compute the length itself.
bid_ITM_PRV: Optional. Address of the _item_ BlockID of the same-named item 
	that precedes the item being queried. Therefore only useful when working 
	with multiple-instance items. If null, procedure seeks the first item with 
	the given name.
pbid_itm: Optional Output. Address of the variable to receive the item BlockID 
	of the target item. If null, output is suppressed.
pbid_ctnt: Optional Output, Required if the content-descriptor output is 
	requested. Address of the variable to receive the content BlockID of the 
	target item. If the content-descriptor output is requested, the block will 
	be locked on return and the caller is responsible for unlocking it. If 
	null, output is suppressed. If a handled error occurs, block will be 
	unlocked before return.
pt_ctnt: Optional Output. Address of the content-descriptor structure to 
	receive information about the content of the MIME-part. If null, output is 
	suppressed.
pbid_itmFileIncl: Optional Output. Address of BlockID to receive the _item_ 
	block associated with the file-attacment include involved with the 
	MIME-part.
pbid_ctntFileIncl: Optional Output. Address of BlockID to receive the content 
	block associated with the file-attacment include involved with the 
	MIME-part. If null, the output is suppressed.
RETURN:
	eus_ERR_INVLD_ARG if any input is obviously invalid
	eus_ERR_MEM if an attempted memory allocation failed
	eus_SUCCESS if no error occured
	the Notes API error code otherwise

--- revision history -------
3/14/03 PR: created			*/
STATUS eus_MimePartItmInfo( const NOTEHANDLE  h_NOTE, 
							const char  pc_ITMNM[], 
							const WORD  us_LEN_ITMNM_, 
							const BLOCKID  bid_ITM_PRV, 
							BLOCKID *const  pbid_itm, 
							BLOCKID *const  pbid_ctnt, 
							MimePrtItmCtnt *const  pt_ctnt, 
							BLOCKID *const  pbid_itmFileIncl, 
							BLOCKID *const  pbid_ctntFileIncl)	{
	const WORD  us_LEN_ITMNM = us_LEN_ITMNM_ ? us_LEN_ITMNM_ : pc_ITMNM ? 
													strlen( pc_ITMNM) : NULL;

	BLOCKID  bid_ctnt;
	WORD  us_typ, us_lenCtnt;
	DWORD  ul;
	MIME_PART * pt;
	BYTE * puc_ctnt, * puc;
	WORD  us;
	STATUS  us_err;

	if (!( h_NOTE && pc_ITMNM && *pc_ITMNM && (pt_ctnt ? (BOOL) pbid_ctnt : 
																		TRUE)))
		return eus_ERR_INVLD_ARG;

	if (pbid_itm)
		*pbid_itm = ebid_NULLBLOCKID;
	if (pbid_ctnt)
		*pbid_ctnt = ebid_NULLBLOCKID;
	if (pt_ctnt)
		memset( pt_ctnt, NULL, sizeof( MimePrtItmCtnt));
	if (pbid_itmFileIncl)
		*pbid_itmFileIncl = ebid_NULLBLOCKID;
	if (pbid_ctntFileIncl)
		*pbid_ctntFileIncl = ebid_NULLBLOCKID;

	//if the item isn't a valid MIME-part, short-cicuit with failure
	if (!bid_ITM_PRV.pool)	{
		if (us_err = NSFItemInfo( (NOTEHANDLE) h_NOTE, (char *) pc_ITMNM, 
													us_LEN_ITMNM, pbid_itm, 
													&us_typ, &bid_ctnt, &ul))
			return us_err;
	}else if (us_err = NSFItemInfoNext( (NOTEHANDLE) h_NOTE, bid_ITM_PRV, 
											(char *) pc_ITMNM, us_LEN_ITMNM, 
											pbid_itm, &us_typ, &bid_ctnt, &ul))
		return us_err;
	if (!( us_typ == TYPE_MIME_PART && ul >= sizeof( WORD) + 
															sizeof( MIME_PART)))
		return eus_ERR_INVLD_ARG;

	//if request requires no content annotatation or extraction, short-circuit 
	//	with success
	if (!( pt_ctnt || pbid_itmFileIncl || pbid_ctntFileIncl))	{
		if (pbid_ctnt)
			*pbid_ctnt = bid_ctnt;
		return eus_SUCCESS;
	}

	//annotate and extract content as requested
	pt = (MIME_PART *) (OSLockBlock( WORD, bid_ctnt) + 1);
	puc_ctnt = puc = (BYTE *) (pt + 1);
	if (pt_ctnt)	{
		pt_ctnt->puc_ctnt = puc_ctnt;
		pt_ctnt->pt = pt;
	}
	us_lenCtnt = pt->wByteCount;
	if (puc = epc_strInBuf( epc_MIMHDR_CTNT_TYP, puc, us_lenCtnt, 
												eus_LEN_MIMHDR_CTNT_TYP))	{
		puc += eus_LEN_MIMHDR_CTNT_TYP;
		if (pt_ctnt)
			pt_ctnt->puc_typ = puc;
		if (!( puc = epc_strInBuf( epc_MIM_HDRS_DLIM, puc, us_lenCtnt, 
														eus_LEN_MIM_HDRS_DLIM)))
			goto errJump;
		puc += eus_LEN_MIM_HDRS_DLIM;
		if (pt_ctnt)
			pt_ctnt->puc_bdy = puc;
		if (pt->dwFlags & MIME_PART_BODY_IN_DBOBJECT && (pbid_itmFileIncl || 
								pbid_ctntFileIncl) && (us = pt->wByteCount - 
								(puc - puc_ctnt)) && us < MAXPATH)	{
			char *const pc = malloc( us + 1);
			if (!pc)	{
				us_err = eus_ERR_MEM;
				goto errJump;
			}
			pc[ us] = NULL;
			memcpy( pc, puc, us);
			us_err = eus_getAttachmentInfo( h_NOTE, NULL, pc, NULL, NULL, NULL, 
									NULL, pbid_itmFileIncl, pbid_ctntFileIncl);
			if (!us_err && (pbid_itmFileIncl && pbid_itmFileIncl->pool || 
								pbid_ctntFileIncl && pbid_ctntFileIncl->pool))
				pt_ctnt->pc_objNm = pc;
			else
				free( pc);
			if (us_err)
				goto errJump;
		} //if (pt->dwFlags & MIME_PART_BODY_IN_DBOBJECT && (pbid_itmFileIncl ||
	}else if (us_lenCtnt > pt->wBoundaryLen + eus_LEN_MIM_BNDRY_TACK_ON + 
																eui_LEN_CRLF)
		goto errJump;

	if (!pt_ctnt)
		OSUnlockBlock( bid_ctnt);
	if (pbid_ctnt)
		*pbid_ctnt = bid_ctnt;

	return eus_SUCCESS;

errJump:
	OSUnlockBlock( bid_ctnt);
	return us_err ? us_err : eus_ERR_INVLD_ARG;
} //eus_MimePartItmInfo(


/** eus_MimePartInfo( ***
Analyzes and informs caller about the MIME part which ensues in a memory buffer 
or bufferred file.

--- parameters & return ----
PC: Optional. Address of string buffer holding the MIME part in question (and 
	follow-on content which is not of concern to this procedure).
l_BGN: Optional, Required Positive if PC input not provided and ppt_bndrys 
	contains a boundary-string list. Location in bufferred file where the MIME 
	part in question begins.
pt_bufFl: Optional Input & Output, Required if PC Input not Provided. Address 
	of information structure describing the bufferred file managed via the 
	LibFileSystem.h module. If PC input provided, input is ignored. If used, 
	the opaque information structure will likely be adjusted by the procedure.
ppt_bndrys: Input & Output. Address of pointer to a LIFO list (stack) of 
	boundary strings, with the first node positioned on the currently active 
	string. If procedure discovers a new boundary (because part is a new 
	multipart), its string will be pushed onto the list to become the new 
	current boundary. If an epilog part is reached, procedure will pop to the 
	next boundary in the list (procedure output). If a null list node is 
	provided, procedure assumes it's dealing with a top-level part, so it will 
	determine whether the part specifies a boundary string (because it's a 
	multipart prolog part) and, if so, begin the list with it.
pt_ctnt: Output. Address of information structure to be populated by procedure 
	such that it describes the MIME part.
RETURN:
	eus_ERR_INVLD_ARG if any input is obviously invalid or the suppossed MIME 
		part has an invalid structure
	eus_ERR_MEM if an attempted memory allocation failed
	eus_SUCCESS if no error occured
	!eus_SUCCESS otherwise

--- revision history -------
3/14/03 PR: created			*/
STATUS eus_MimePartInfo( const char  PC[], 
							const long  l_BGN, 
							BufFile *const  pt_bufFl, 
							StringNode * *const  ppt_bndrys, 
							MimePrtCtnt *const  pt_ctnt)	{
	const BOOL  f_BUF = PC && *PC;
	const size_t  ui_LEN_MIMHDR_LIMT = 0x300;		//arbitrary

	StringNode * pt_bndry_, * pt_bndry, * pt_newBndry = NULL;
	long  l_strt, l_hdrsEnd, l, l_;
	size_t  ui = NULL;
	char * pc_strt, * pc_hdrsEnd, * pc, * pc_;
	STATUS us_err;

	if (!( (f_BUF || pt_bufFl) && ppt_bndrys && pt_ctnt && (pt_bufFl && 
												*ppt_bndrys ? l_BGN : TRUE)))
		return eus_ERR_INVLD_ARG;

	memset( pt_ctnt, NULL, sizeof( MimePrtCtnt));

	//if we're dealing with a top-level part...
	if (!( pt_bndry_ = pt_bndry = *ppt_bndrys))	{
		//if the beginning of the file starts invalidly for a top-level part, 
		//	short-circuit with failure
		if (f_BUF)	{
			if (*PC == *epc_MIM_BNDRY_TACK_ON)
				return eus_ERR_INVLD_ARG;
		}else	{
			char  c;
			if (el_bufFileCpyCtnt( 0, 1, &c, pt_bufFl) == ei_FAIL || c == 
														*epc_MIM_BNDRY_TACK_ON)
				return eus_ERR_INVLD_ARG;
		} //if (f_BUF)

		//locate the Content-Type header, which we consider to be the start of 
		//	the part (this works around Eudora plug-in bug of including invalid 
		//	Mime-Version header in top-level prolog parts)
		if (f_BUF)	{
			if (!( pc_strt = epc_strInBuf( epc_MIMHDR_CTNT_TYP, PC, 0x40, 
													eus_LEN_MIMHDR_CTNT_TYP)))
				return eus_ERR_INVLD_ARG;
			pt_ctnt->puc_ctnt = pc_strt;
			pt_ctnt->puc_typ = pc_strt + eus_LEN_MIMHDR_CTNT_TYP;
		}else	{
			if (( l_strt = el_bufFileSeekStr( epc_MIMHDR_CTNT_TYP, 0, 0x40, 
														pt_bufFl)) == ei_FAIL)
				return eus_ERR_INVLD_ARG;
			pt_ctnt->puc_ctnt = (BYTE *) l_strt;
			pt_ctnt->puc_typ = (BYTE *) l_strt + eus_LEN_MIMHDR_CTNT_TYP;
		} //if (f_BUF)

		//determine the end of the part's header content
		if (f_BUF)	{
			if (!( pc_hdrsEnd = epc_strInBuf( epc_MIM_HDRS_DLIM, pc_strt,
														ui_LEN_MIMHDR_LIMT, 
														eus_LEN_MIM_HDRS_DLIM)))
				return eus_ERR_INVLD_ARG;
		}else
			if (( l_hdrsEnd = el_bufFileSeekStr( epc_MIM_HDRS_DLIM, l_strt, 
														ui_LEN_MIMHDR_LIMT, 
														pt_bufFl)) == ei_FAIL)
				return eus_ERR_INVLD_ARG;

		//if there's no boundary string, this must be a sole-body message...
		if (f_BUF)	{
			if (pc = epc_strInBuf( epc_MIM_BOUNDRY, pt_ctnt->puc_typ, 
												pc_hdrsEnd - pt_ctnt->puc_typ, 
												eus_LEN_MIM_BOUNDRY))	{
				pc += eus_LEN_MIM_BOUNDRY;
				if (!( pc_ = memchr( pc, ec_QUOTES, pc_hdrsEnd - pc)))
					return eus_ERR_INVLD_ARG;
				ui = pc_ - pc;
			}
		}else	{
			if (( l = el_bufFileSeekStr( epc_MIM_BOUNDRY, (long) 
												pt_ctnt->puc_typ, l_hdrsEnd, 
												pt_bufFl)) != ei_FAIL)	{
				l += eus_LEN_MIM_BOUNDRY;
				if (( l_ = el_bufFileSeekStr( epc_QUOTES, l, l_hdrsEnd, 
														pt_bufFl)) == ei_FAIL)
					return eus_ERR_INVLD_ARG;
				ui = l_ - l;
			} //if (( l = el_bufFileSeekStr( epc_MIM_BOUNDRY,
		} //if (f_BUF)
		if (!ui)	{
			//note where the part's body content begins, and determine its 
			//	overall length
			if (f_BUF)	{
				pt_ctnt->puc_bdy = pc_hdrsEnd + eus_LEN_MIM_HDRS_DLIM;
				pt_ctnt->ui_len = strlen( pc_strt);
			}else	{
				pt_ctnt->puc_bdy = (BYTE *) l_hdrsEnd + eus_LEN_MIM_HDRS_DLIM;
				if ((int) (ui = (size_t) ei_fileLength( pt_bufFl)) == ei_FAIL)
					return eus_ERR_INVLD_ARG;
				pt_ctnt->ui_len = ui - (size_t) l_strt;
			} //if (f_BUF)
		//else we're dealing with the prolog part of a multipart shell...
		}else	{
			//start the boundary list with the boundary string
			if (!( pc_ = malloc( ui + eus_LEN_MIM_BNDRY_TACK_ON + 1)))
				return eus_ERR_MEM;
			pc_[ ui + eus_LEN_MIM_BNDRY_TACK_ON] = NULL;
			strcpy( pc_, epc_MIM_BNDRY_TACK_ON);
			if (f_BUF)
				memcpy( pc_ + eus_LEN_MIM_BNDRY_TACK_ON, pc, ui);
			else	{
				if (el_bufFileCpyCtnt( l, ui, pc_ + eus_LEN_MIM_BNDRY_TACK_ON, 
													pt_bufFl) == ei_FAIL)	{
					free( pc_);
					return !eus_SUCCESS;
				}
			} //if (f_BUF)
			if (!ef_AddStringNode( pc_, FALSE, FALSE, &pt_newBndry))	{
				free( pc_);
				return eus_ERR_MEM;
			}

			//determine the overall length of the part
			if (f_BUF)	{
				if (!( pc = strstr( pc_hdrsEnd + 1, pc_)))	{
					us_err = eus_ERR_INVLD_ARG;
					goto errJump;
				}
				pt_ctnt->ui_len = pc - pt_ctnt->puc_ctnt;
			}else	{
				if (( l = el_bufFileSeekStr( pc_, l_hdrsEnd, NULL, 
													pt_bufFl)) == ei_FAIL)	{
					us_err = eus_ERR_INVLD_ARG;
					goto errJump;
				}
				pt_ctnt->ui_len = l - (long) pt_ctnt->puc_ctnt;
			} //if (f_BUF)
		} //if (!ui)
	//else we've got a part within a multipart structure...
	}else	{
		char  pc_buf[ 2 + 80 + 1];
		size_t  ui_;

		//note the beginning of the part
		pt_ctnt->puc_ctnt = f_BUF ? (BYTE *) PC : (BYTE *) l_BGN;

		//validate and get the coördinates of the boundary string
		ui = strlen( pt_bndry->pc);
		ui_ = eui_LEN_CRLF + ui + eus_LEN_MIM_BNDRY_TACK_ON;
		if (f_BUF)	{
			pt_ctnt->puc_bndry = (BYTE *) PC;
			pc = (BYTE *) PC;
			if (memcmp( PC, epc_CRLF, eui_LEN_CRLF) == ei_SAME)	{
				pt_ctnt->puc_bndry += eui_LEN_CRLF;
				pc += eui_LEN_CRLF;
			}
		}else	{
			if (( l = el_bufFileCpyCtnt( l_BGN, ui_, pc_buf, pt_bufFl)) != 
																	(int) ui_)
				return eus_ERR_INVLD_ARG;
			pt_ctnt->puc_bndry = (BYTE *) l_BGN;
			pc = pc_buf;
			if (memcmp( pc_buf, epc_CRLF, eui_LEN_CRLF) == ei_SAME)	{
				pt_ctnt->puc_bndry += eui_LEN_CRLF;
				pc += eui_LEN_CRLF;
			}
		} //if (f_BUF)
		if (memcmp( pc, pt_bndry->pc, ui) != ei_SAME)
			return eus_ERR_INVLD_ARG;
		pt_ctnt->us_lenBndry = ui + eui_LEN_CRLF;

		//if this is an epilog part...
		if (memcmp( pc + ui, epc_MIM_BNDRY_TACK_ON, 
									eus_LEN_MIM_BNDRY_TACK_ON) == ei_SAME)	{
			//add the tack-on length to the boundary length
			pt_ctnt->us_lenBndry += eus_LEN_MIM_BNDRY_TACK_ON;

			//move to the next-higher boundary in the list
			pt_bndry = pt_bndry->pt_next;

			//fill in the part descriptor of the epilog part
			_ASSERTE( pt_ctnt->puc_bndry == pt_ctnt->puc_ctnt + eui_LEN_CRLF);
			pt_ctnt->ui_len = ui_ + eui_LEN_CRLF;
		//else we're dealing with a prolog or body part...
		}else	{
			//determine where the part's headers end
			if (f_BUF)	{
				pc_ = pt_ctnt->puc_bndry + ui + eui_LEN_CRLF;
				if (!( pc_hdrsEnd = epc_strInBuf( epc_MIM_HDRS_DLIM, pc_, 
														ui_LEN_MIMHDR_LIMT, 
														eus_LEN_MIM_HDRS_DLIM)))
					return eus_ERR_INVLD_ARG;
			}else	{
				l_ = (long) pt_ctnt->puc_bndry + ui + eui_LEN_CRLF;
				if (( l_hdrsEnd = el_bufFileSeekStr( epc_MIM_HDRS_DLIM, l_, 
													l_ + ui_LEN_MIMHDR_LIMT, 
													pt_bufFl)) == ei_FAIL)
					return eus_ERR_INVLD_ARG;
			} //if (f_BUF)

			//locate the Content-Type header
			if (f_BUF)	{
				if (!( pc = epc_strInBuf( epc_MIMHDR_CTNT_TYP, pc_, 
													pc_hdrsEnd - pc_, 
													eus_LEN_MIMHDR_CTNT_TYP)))
					return eus_ERR_INVLD_ARG;
				pt_ctnt->puc_typ = pc += eus_LEN_MIMHDR_CTNT_TYP;
			}else	{
				if (( l = el_bufFileSeekStr( epc_MIMHDR_CTNT_TYP, l_, 
											l_hdrsEnd, pt_bufFl)) == ei_FAIL)
					return eus_ERR_INVLD_ARG;
				pt_ctnt->puc_typ = (BYTE *) l + eus_LEN_MIMHDR_CTNT_TYP;
			} //if (f_BUF)

			//if there's a boundary within the headers, this must be another 
			//	prolog part to a multipart shell...
			ui = NULL;
			if (f_BUF)	{
				if (pc = epc_strInBuf( epc_MIM_BOUNDRY, pc, pc_hdrsEnd - pc, 
													eus_LEN_MIM_BOUNDRY))	{
					pc += eus_LEN_MIM_BOUNDRY;
					if (!( pc_ = memchr( pc, ec_QUOTES, pc_hdrsEnd - pc)))
						return eus_ERR_INVLD_ARG;
					ui = pc_ - pc;
				}
			}else	{
				if (( l = el_bufFileSeekStr( epc_MIM_BOUNDRY, (long) 
												pt_ctnt->puc_typ, l_hdrsEnd, 
												pt_bufFl)) != ei_FAIL)	{
					if (( l_ = el_bufFileSeekStr( epc_QUOTES, l += 
												eus_LEN_MIM_BOUNDRY, l_hdrsEnd, 
												pt_bufFl)) == ei_FAIL)
						return eus_ERR_INVLD_ARG;
					ui = l_ - l;
				} //if (( l = el_bufFileSeekStr( epc_MIM_BOUNDRY,
			} //if (f_BUF)
			if (ui)	{
				//Construct a new boundary list member to be added to the list 
				//	on final procedure output. This is now the active boundary.
				if (!( pc_ = malloc( ui + eus_LEN_MIM_BNDRY_TACK_ON + 1)))
					return eus_ERR_MEM;
				pc_[ ui + eus_LEN_MIM_BNDRY_TACK_ON] = NULL;
				strcpy( pc_, epc_MIM_BNDRY_TACK_ON);
				if (f_BUF)
					memcpy( pc_ + eus_LEN_MIM_BNDRY_TACK_ON, pc, ui);
				else	{
					if (el_bufFileCpyCtnt( l, ui, pc_ + 
													eus_LEN_MIM_BNDRY_TACK_ON, 
													pt_bufFl) == ei_FAIL)	{
						free( pc_);
						return !eus_SUCCESS;
					}
				} //if (f_BUF)
				if (!ef_AddStringNode( pc_, FALSE, FALSE, &pt_newBndry))	{
					free( pc_);
					return eus_ERR_MEM;
				}

				//determine the overall length of the part
				if (f_BUF)	{
					if (!( pc = strstr( pc_hdrsEnd, pc_)))	{
						us_err = eus_ERR_INVLD_ARG;
						goto errJump;
					}
					pt_ctnt->ui_len = pc - pt_ctnt->puc_ctnt;
				}else	{
					if (( l = el_bufFileSeekStr( pc_,l_hdrsEnd, NULL, 
													pt_bufFl)) == ei_FAIL)	{
						us_err = eus_ERR_INVLD_ARG;
						goto errJump;
					}
					pt_ctnt->ui_len = l - (long) pt_ctnt->puc_ctnt;
				} //if (f_BUF)
			//else we're dealing with a body part
			}else	{
				//if we're working within a memory buffer...
				if (f_BUF)	{
					//note where the part's body content begins
					pt_ctnt->puc_bdy = pc_hdrsEnd + eus_LEN_MIM_HDRS_DLIM;

					//if the body content is encoded, note the type of encoding
					if (pc = epc_strInBuf( epc_MIMHDR_CTNT_ENC, 
											pt_ctnt->puc_typ + 1, pc_hdrsEnd - 
											(pt_ctnt->puc_typ + 1), 
											eus_LEN_MIMHDR_CTNT_ENC))
						pt_ctnt->puc_encdg = pc + eus_LEN_MIMHDR_CTNT_ENC;

					//if the body content is an attachment, note its filename
					if (pc = epc_strInBuf( epc_MIMHDR_CTNT_ATCH, 
											pt_ctnt->puc_typ + 1, pc_hdrsEnd - 
											(pt_ctnt->puc_typ + 1), 
											eus_LEN_MIMHDR_CTNT_ATCH))	{
						pc += eus_LEN_MIMHDR_CTNT_ATCH;
						if (pc = epc_strInBuf( mpc_MIMHDR_ATCH_FILENM, pc, 
											pc_hdrsEnd - pc, 
											mus_LEN_MIMHDR_ATCH_FILENM))	{
							pc += mus_LEN_MIMHDR_ATCH_FILENM;
							if (!( pc_ = memchr( pc, ec_QUOTES, pc_hdrsEnd - 
																		pc)))
								return eus_ERR_INVLD_ARG;
							pt_ctnt->puc_atchNm = pc;
							pt_ctnt->us_lenAtchNm = pc_ - pc;
						} //if (pc = epc_strInBuf( mpc_MIMHDR_ATCH_FILENM,
					} //if (pc = epc_strInBuf( epc_MIMHDR_CTNT_ATCH,

					//determine the overall length of the part
					if (!( pc = strstr( pc_hdrsEnd, pt_bndry->pc)))	{
						us_err = eus_ERR_INVLD_ARG;
						goto errJump;
					}
					pt_ctnt->ui_len = pc - pt_ctnt->puc_ctnt - eui_LEN_CRLF;
				//else we're working with file-based content...
				}else	{
					//note where the part's body content begins
					pt_ctnt->puc_bdy = (BYTE *) l_hdrsEnd + 
														eus_LEN_MIM_HDRS_DLIM;

					//if the body content is encoded, note the type of encoding
					if (( l = el_bufFileSeekStr( epc_MIMHDR_CTNT_ENC, (long) 
											pt_ctnt->puc_typ + 1, l_hdrsEnd, 
											pt_bufFl)) != ei_FAIL)
						pt_ctnt->puc_encdg = (BYTE *) l + 
														eus_LEN_MIMHDR_CTNT_ENC;

					//if the body content is an attachment, note its filename
					if (!( (l = el_bufFileSeekStr( epc_MIMHDR_CTNT_ATCH, (long) 
										pt_ctnt->puc_typ + 1, l_hdrsEnd, 
										pt_bufFl)) == ei_FAIL || 
										(l = el_bufFileSeekStr( 
										mpc_MIMHDR_ATCH_FILENM, l += 
										eus_LEN_MIMHDR_CTNT_ATCH, l_hdrsEnd, 
										pt_bufFl)) == ei_FAIL))	{
						if (( l_ = el_bufFileSeekStr( epc_QUOTES, l += 
											mus_LEN_MIMHDR_ATCH_FILENM, 
											l_hdrsEnd, pt_bufFl)) == ei_FAIL)
							return eus_ERR_INVLD_ARG;
						pt_ctnt->puc_atchNm = (BYTE *) l;
						pt_ctnt->us_lenAtchNm = (WORD) (l_ - l);
					} //if (!( (l = el_bufFileSeekStr( epc_MIMHDR_CTNT_ATCH,

					//determine the overall length of the part
					if (( l = el_bufFileSeekStr( pt_bndry->pc, l_hdrsEnd, NULL, 
													pt_bufFl)) == ei_FAIL)	{
						us_err = eus_ERR_INVLD_ARG;
						goto errJump;
					}
					pt_ctnt->ui_len = l - (long) pt_ctnt->puc_ctnt - 
																eui_LEN_CRLF;
				} //if (f_BUF)
			} //if (ui)
		} //if (memcmp( pc + ui, epc_MIM_BNDRY_TACK_ON,
	} //if (!( pt_bndry_ = pt_bndry = *ppt_bndrys))

	//if a new boundary string was found...
	if (pt_newBndry)	{
		//add it at the head of the full list
		pt_newBndry->pt_next = *ppt_bndrys;
		*ppt_bndrys = pt_newBndry;
	//else if a MIME section ended (epilog), update the current boundary for 
	//	caller
	}else if (pt_bndry != pt_bndry_)
		*ppt_bndrys = pt_bndry;

	return eus_SUCCESS;

errJump:
	e_FreeList( &pt_newBndry, TRUE);

	return us_err;
} //eus_MimePartInfo(


/** eus_appendAtchPartToFile( ***
Appends the contents of a body-with-attachment Notes MIME-part to a file opened 
and prepared to be written to. The content of the Notes MIME-part item is 
expanded to include the attachment, and the attachment is base64 encoded if 
necessary.

--- parameters & return ----
pv_file: Input & Output. Address of opaque file object for use with 
	LibFileSystem module. Object will be updated as a part of the call. 
	Procedure assumes file is writable.
pt_CTNT: address of information structure describing the source Notes MIME-part 
	information
BID: BlockID associated with the item describing the attachment on the note
h_NOTE: handle to the note containing the attachment
pui_len: Output. Address of variable to receive the length of the resulting 
	body content, including the length of any headers.
RETURN:
	eus_ERR_INVLD_ARG if any input is obviously invalid
	!eus_SUCCESS if a temporary file couldn't be created or base64 encoded
	eus_SUCCESS if no error occured
	the Notes API error code otherwise

--- revision history -------
3/14/03 PR: created			*/
STATUS eus_appendAtchPartToFile( void *const  pv_file, 
									const MimePrtItmCtnt *const  pt_CTNT, 
									const BLOCKID  BID, 
									const NOTEHANDLE  h_NOTE, 
									DWORD *const  pui_len)	{
	char * pc_extFileNmDtch, * pc_extFileNmDecd;
	size_t  ui_, ui;
	BYTE * puc;
	FILE * pv_fl = NULL;
	STATUS  us_err;
	BOOL  f_fail;

	if (!( pv_file && pt_CTNT && BID.pool && h_NOTE && pui_len))
		return eus_ERR_INVLD_ARG;

	*pui_len = NULL;

	//detach the in-line attachment to a temporary file
//suggested enhancment: could read the contents directly out of the attachment 
//	object if object is not compressed (and do the base64 encoding all in 
//	memory, as necessary)
	if (!(pc_extFileNmDtch = _tempnam( NULL, epc_TILDE)))
		return !eus_SUCCESS;
	if (us_err = NSFNoteExtractFile( (NOTEHANDLE) h_NOTE, BID, 
												pc_extFileNmDtch, NULL))	{
		free( pc_extFileNmDtch);
		return us_err;
	}

	//append all of the part's contents except its body
	ui_ = pt_CTNT->puc_bdy - pt_CTNT->puc_ctnt;
	if (f_fail = el_addToOpenFile( pv_file, pt_CTNT->puc_ctnt, ui_, FALSE) == 
																		ei_FAIL)
		goto errJump;

	//if a transfer-encoding header is present...
	if (puc = epc_strInBuf( epc_MIMHDR_CTNT_ENC, pt_CTNT->puc_typ, 
										pt_CTNT->puc_bdy - pt_CTNT->puc_typ, 
										eus_LEN_MIMHDR_CTNT_ENC))	{
		//if the encoding is base64...
		if (strnicmp( puc + eus_LEN_MIMHDR_CTNT_ENC, epc_MIM_CTNTENC_B64, 
										eus_LEN_MIM_CTNTENC_B64) == ei_SAME)	{
			//base64 encode the associated file and keep it opened
			if (f_fail = !(pc_extFileNmDecd = _tempnam( NULL, epc_TILDE)))
				goto errJump;
			if (f_fail = !ef_base64encodeFile( pc_extFileNmDtch, 
												pc_extFileNmDecd, &pv_fl))	{
				free( pc_extFileNmDecd);
				goto errJump;
			}

			//append the base64-encoded file's contents to the file being 
			//	prepared for PGP-decoding, then get rid of the base64-encoded 
			//	file
			if (f_fail = ((int) ui = (UINT) ei_appendFileContent( pv_file, 
															pv_fl, NULL, NULL, 
															NULL)) == ei_FAIL)
				goto errJump;
			if (f_fail = !ef_closeAndDeleteFile( pv_fl, pc_extFileNmDecd))
				goto errJump;
			free( pc_extFileNmDecd);
			pv_fl = NULL;
		//else we don't handle that encoding type yet, so set up failure
		}else
			us_err = eus_ERR_INVLD_ARG;
	//else assume the attachment is to be streamed in as is
	}else
		f_fail = (int) (ui = (UINT) ei_appendFileContent( pv_file, NULL, NULL, 
											pc_extFileNmDtch, NULL)) == ei_FAIL;

	//tell caller the total length of the part, including the attachment 
	//	content
	if (!( us_err + f_fail))
		*pui_len = ui + ui_;

errJump:
	if (pv_fl)	{
		if (!( ef_closeAndDeleteFile( pv_fl, pc_extFileNmDecd) || us_err + 
																		f_fail))
			f_fail = TRUE;
		free( pc_extFileNmDecd);
	}
	if (pc_extFileNmDtch)	{
		//delete the detached file, ignoring failure
		remove( pc_extFileNmDtch);
		free( pc_extFileNmDtch);
	}

	return us_err + f_fail;
} //eus_appendAtchPartToFile(


/** eus_MimeAtchFileCtnt( ***
Saves the body content of a MIME part as a file attachment to the specified 
Notes document, base64 decoding it if necessary, in the manner Notes itself 
does with MIME body-part attachments.

--- parameters & return ----
pt_CTNT: address of information structure describing the raw MIME part and the 
	type of attachment (and its name) that needs to be accomplished
pt_bufFl: Optional Input & Output. Address of a buffered-file object managed by 
	the LibFileSystem.h module. If input is provided, the raw MIME part is 
	contained within this file, and all content pointers in the pt_CTNT 
	information structure represent relative offsets into the file. Opaque file 
	object will undergo change as a result of this procedure's work. If null, 
	procedure infers that the MIME part resides in a memory buffer, and this 
	input-output parameter is ignored.
h_NOTE: handle to the Notes document to which the file attachment should be made
ppc: Optional Output. Address of pointer variable to receive address of 
	filename associated with the attachment. Caller is responsible for freeing 
	the memory allocated to the string. If null, output is supressed.
RETURN:
	eus_ERR_INVLD_ARG if any input is obviously invalid
	!eus_SUCCESS if a base64 decoding error occurred
	eus_SUCCESS if no error occured
	the Notes API error code otherwise

--- revision history -------
3/14/03 PR: created			*/
STATUS eus_MimeAtchFileCtnt( const MimePrtCtnt *const  pt_CTNT, 
								BufFile *const  pt_bufFl, 
								const NOTEHANDLE  h_NOTE, 
								char * *const  ppc)	{
	const DWORD  ul_LEN_BUF_DFLT = 0x200 * 0x400;
	const WORD  us_LEN_LINE_MAX = 0x50;

	size_t  ui_lenPrt, ui, ui_len = NULL;
	DWORD  ul_lenObj, ul_lenObjBuf;
	char  pc_atchNm[ MAXPATH];
	DBHANDLE  h_db;
	ObjInfo  t_obj;
	HANDLE  h = NULL;
	char * pc, * pc_end;
	long  l, l_, l_end;
	BYTE * puc, * puc_, * puc_end, * puc_bufIn = NULL;
	STATUS  us_err;
	BOOL  f_fail = FALSE;

	if (!( pt_CTNT && pt_CTNT->ui_len && pt_CTNT->puc_ctnt && 
								pt_CTNT->puc_atchNm && pt_CTNT->us_lenAtchNm && 
								pt_CTNT->puc_bdy && h_NOTE))
		return eus_ERR_INVLD_ARG;

	*ppc = NULL;

	//determine the length of the body part and ending coördinate
	if (!( ui_lenPrt = pt_CTNT->ui_len - (pt_CTNT->puc_bdy - 
															pt_CTNT->puc_ctnt)))
		return eus_ERR_INVLD_ARG;

	//Note the encoding involved, if any, and get ahold of the attachment name. 
	//	(Only base64 and no-encoding are currently supported.)
	if (!pt_bufFl)	{
		if (pt_CTNT->puc_encdg && strnicmp( epc_MIM_CTNTENC_B64, 
											pt_CTNT->puc_encdg, 
											eus_LEN_MIM_CTNTENC_B64) != ei_SAME)
			return eus_ERR_INVLD_ARG;
		memcpy( pc_atchNm, pt_CTNT->puc_atchNm, pt_CTNT->us_lenAtchNm);
	}else	{
		if (pt_CTNT->puc_encdg)	{
			if (el_bufFileCpyCtnt( (long) pt_CTNT->puc_encdg, 
											eus_LEN_MIM_CTNTENC_B64, pc_atchNm, 
											pt_bufFl) < eus_LEN_MIM_CTNTENC_B64)
				return !eus_SUCCESS;
			if (strnicmp( epc_MIM_CTNTENC_B64, pc_atchNm, 
											eus_LEN_MIM_CTNTENC_B64) != ei_SAME)
				return eus_ERR_INVLD_ARG;
		}
		if (el_bufFileCpyCtnt( (long) pt_CTNT->puc_atchNm, 
											pt_CTNT->us_lenAtchNm, pc_atchNm, 
											pt_bufFl) < pt_CTNT->us_lenAtchNm)
			return !eus_SUCCESS;
	} //if (!pt_bufFl)
	pc_atchNm[ pt_CTNT->us_lenAtchNm] = NULL;

	//begin a Notes database object of the size ultimately needed
	ul_lenObj = pt_CTNT->puc_encdg ? ui_lenPrt * 3 / 4 : ui_lenPrt;
	NSFNoteGetInfo( h_NOTE, _NOTE_DB, &h_db);
	if (!h_db)
		return eus_ERR_INVLD_ARG;
	if (us_err = eus_PrepObjectWrite( h_db, ul_lenObj, &t_obj))
		return us_err;

	//allocate a Notes memory block for use in populating the database object
	ul_lenObjBuf = ui_lenPrt < ul_LEN_BUF_DFLT ? ui_lenPrt : ul_LEN_BUF_DFLT;
	if (us_err = OSMemAlloc( NULL, ul_lenObjBuf, &h))
		goto errJump;

	//if base64 decoding needs to be done...
	if (pt_bufFl)	{
		l = (long) pt_CTNT->puc_bdy;
		l_end = l + ui_lenPrt;
	}else	{
		puc = pt_CTNT->puc_bdy;
		puc_end = puc + ui_lenPrt;
	}
	if (pt_CTNT->puc_encdg)
		//until all the attachment content has been processed...
		do	{
			BYTE * puc_bufOut;
			size_t  ui_lenOut;

			//set up the Notes-owned buffer
			pc = puc_bufIn = OSLockObject( h);
			pc_end = pc + ul_lenObjBuf;

			//fill a Notes-owned buffer with base64 encoded content, CRLFs 
			//	stripped out...
			do	{
				//locate the end of the current line, append it to the buffer, 
				//	and get in position for next
				if (pt_bufFl)	{
					if (( l_ = el_bufFileSeekStr( epc_CRLF, l, 
													l + us_LEN_LINE_MAX, 
													pt_bufFl)) == ei_FAIL)	{
						us_err = eus_ERR_INVLD_ARG;
						goto errJump;
					}
					ui = l_ - l;
					if (f_fail = el_bufFileCpyCtnt( l, ui, pc, pt_bufFl) < 
																	(int) ui)
						goto errJump;
					l = l_ + eui_LEN_CRLF;
				}else	{
					if (!( puc_ = epc_strInBuf( epc_CRLF, puc, us_LEN_LINE_MAX, 
															eui_LEN_CRLF)))	{
						us_err = eus_ERR_INVLD_ARG;
						goto errJump;
					}
					ui = puc_ - puc;
					memcpy( pc, puc, ui);
					puc = puc_ + eui_LEN_CRLF;
				} //if (pt_bufFl)

				//position on the next-write buffer position
				pc += ui;
			} while (pc < pc_end - us_LEN_LINE_MAX * 3 / 4 && (pt_bufFl ? l < 
														l_end : puc < puc_end));
			*pc = NULL;

			//base64 decode the buffer
			if (f_fail = !f_base64decode( &puc_bufOut, &ui_lenOut, puc_bufIn))
				goto errJump;

			//copy the decoded output back into the Notes buffer and unlock the 
			//	buffer, then write the decoded output to the attachment object
			memcpy( puc_bufIn, puc_bufOut, ui_lenOut);
			free( puc_bufOut);
			OSUnlockObject( h);
			puc_bufIn = NULL;
			if (us_err = eus_AppendToObject( h, ui_lenOut, &t_obj))
				goto errJump;

			//keep track of the total length written to database object
			ui_len += ui_lenOut;
		} while (pt_bufFl ? l < l_end : puc < puc_end);
	//else simply write the part's body content directly into the attachment 
	//	object...
	else
		//until all the attachment content has been processed...
		do	{
			//set up the Notes-owned buffer
			pc = puc_bufIn = OSLockObject( h);
			pc_end = pc + ul_lenObjBuf;

			//fill a Notes-owned buffer with content
			ui = ui_lenPrt - ui_len < ul_lenObjBuf ? ui_lenPrt - ui_len : 
																ul_lenObjBuf;
			if (pt_bufFl)	{
				if (f_fail = el_bufFileCpyCtnt( l, ui, pc, pt_bufFl) < (int) ui)
					goto errJump;
				l += ui;
			}else	{
				memcpy( pc, puc, ui);
				puc += ui;
			} //if (pt_bufFl)

			//write the content to the attachment object
			OSUnlockObject( h);
			puc_bufIn = NULL;
			if (us_err = eus_AppendToObject( h, ui, &t_obj))
				goto errJump;

			//keep track of the total length written to database object
			ui_len += ui;
		} while (pt_bufFl ? l < l_end : puc < puc_end);
		OSMemFree( h);

	//commit the now fully compiled object to the note
	if (us_err = eus_CommitObject( h_NOTE, NULL, pc_atchNm, HOST_MSDOS, NULL, 
																NULL, &t_obj))
		goto errJump;

	//if we're working via a bufferred file, reflow the body part content 
	//	since overwritten
	if (pt_bufFl)
		f_fail = !ef_bufFileSpliceToPrfx( (long) pt_CTNT->puc_ctnt, 
												pt_CTNT->puc_bdy - 
												pt_CTNT->puc_ctnt, pt_bufFl);

	if (ppc && !f_fail)
		if (*ppc = malloc( pt_CTNT->us_lenAtchNm + 1))	{
			memcpy( *ppc, pc_atchNm, pt_CTNT->us_lenAtchNm);
			(*ppc)[ pt_CTNT->us_lenAtchNm] = NULL;
		}else
			us_err = eus_ERR_MEM;

	return us_err + f_fail;

errJump:
	if (h)	{
		if (puc_bufIn)
			OSUnlockObject( h);
		OSMemFree( h);
	}
	eus_releaseObject( &t_obj);

	return us_err + f_fail;
} //eus_MimeAtchFileCtnt(


