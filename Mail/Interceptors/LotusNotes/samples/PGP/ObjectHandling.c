/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: ObjectHandling.c 12963 2003-03-15 00:18:21Z sdas $
______________________________________________________________________________*/

/*::: MODULE OVERVIEW :::::::::::::
Library/abstract data type that facilitates object handling within a Notes 
database.

--- suggested enhancement ---
9/16/00 PR: complete the stubbed-out eus_getAttchOriginalNm()

--- revision history --------
3/14/03 Version 1.3 Paul Ryan
+ adjustments in support of globals-initialization reörganizaiton

9/6/02 Version 1.2 Paul Ryan
+ listing format adjustment

9/16/00 Version 1.1.3 Paul Ryan
+ stubbed out a eus_getAttchOriginalNm() function
+ minor improvements to eus_getAttachmentInfo(), including documentation 
  addition

8/9/00 Version 1.1.2 Paul Ryan
+ added eus_AttachFileAsObject()
+ documentation addition

3/20/00 Version 1.1.1 Paul Ryan
+ minor documentation adjustment

9/12/99 Version 1.1 Paul Ryan
+ genericization, documentation of eus_getObjectContentsInBuffer()

12/10/98 Version 1.0 Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#include "ObjectHandling.h"


//global-scope declaration
char  epc_ITMNM_STD_ATTCH[] = ITEM_NAME_ATTACHMENT;

//module-scope declaration
#define mus_MAXBUF_EXTFILENM  MAXPATH


/** us_AttachObjectItem( ***


--- parameters & return ----

RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
3/14/03 PR
+ signature change in support of maintaining constness as much as possible
+ listing format adjustment, token renaming

1/30/99 PR: created			*/
//DOC!!
static STATUS us_AttachObjectItem( const NOTEHANDLE  h_NOTE, 
									const DWORD  ul_OBJ_ID, 
									const char  pc_ITMNM[], 
									const char  pc_OBJNM[], 
									const DWORD  ul_OBJ_SIZE, 
									const WORD  us_HOST_TYPE)	{
	const char * pc_objNm;
	BLOCKID  bid;
	WORD * pus, us_lenItm;
	TIMEDATE  td;
	FILEOBJECT * pt_itmInfo;
	WORD  us;
	STATUS  us_err;

	if (!( h_NOTE && ul_OBJ_ID && pc_ITMNM && pc_OBJNM && ul_OBJ_SIZE))
		return !eus_SUCCESS;

	//if we're dealing with a simple filename (no path)...
	if (!( pc_objNm = strrchr( pc_OBJNM, ec_PATH_SPECIFIER)))
		//use that filename as the object name
		pc_objNm = pc_OBJNM;
	//else step to the character immediately following the last backslash and 
	//	have that tail-end simple filenmae be the object name
	else
		pc_objNm++;

	if (us_err = OSMemAlloc( NULL, us_lenItm = sizeof( WORD) + sizeof( 
												FILEOBJECT) + (us = (WORD) 
												strlen( pc_objNm)), &bid.pool))
		return us_err;
	bid.block = NULLBLOCK;

	pus = OSLockBlock( WORD, bid);
	*pus = TYPE_OBJECT;

	//Fill in the item structure used to describe the object to the note 
	//	"hosting" the object. A host type of "MSDOS" in needed in order to 
	//	ensure that the object travels with the rich-text field when the 
	//	message is forwarded or replied to "with history" ("unknown" works as 
	//	well). I'm not sure what the "in-doc" flag does, but Notes seems to put 
	//	it on every attachment, so we will too.
	pt_itmInfo = (FILEOBJECT *) (pus + 1);
	pt_itmInfo->Header.ObjectType = OBJECT_FILE;
	pt_itmInfo->Header.RRV = ul_OBJ_ID;
	pt_itmInfo->FileNameLength = us;
	pt_itmInfo->HostType = us_HOST_TYPE;
	pt_itmInfo->FileAttributes = NULL;	//i.e. Read/Write & Public
	pt_itmInfo->Flags = FILEFLAG_INDOC;
	pt_itmInfo->FileSize = ul_OBJ_SIZE;
	pt_itmInfo->CompressionType = NULL;
	OSCurrentTIMEDATE( &td);
	pt_itmInfo->FileCreated = pt_itmInfo->FileModified = td;

	memcpy( (BYTE *) pt_itmInfo + sizeof( FILEOBJECT), pc_objNm, us);
	OSUnlockBlock( bid);

	us_err = NSFItemAppendObject( (NOTEHANDLE) h_NOTE, NULL, (char *) pc_ITMNM, 
													(WORD) strlen( pc_ITMNM), 
													bid, us_lenItm, TRUE);

	if (us_err)
		OSMemFree( bid.pool);

	return us_err;
} //us_AttachObjectItem(


/** eus_getAttachmentInfo( ***
Obtain descriptive information on the attachment specified by name.

--- parameters & return ----
h_NOTE: handle to the note containing the item descriptor associated with the 
	specified attachment
pc_ITM_NM: Optional. Address of the name of the item descriptor associated with 
	the attachement. If null, the default item name will be used.
pc_OBJNM: Optional. Address of the unique internal name (object name) of the 
	attachment for which information is sought. If null, information will be 
	returned on the first item found.
pul_objId: Optional Output. Address of variable in which to store the RRV ID 
	associated with the specified object (see Notes API documentation re what 
	this is). If null, the output will be suppressed.
pul_size: Optional Output. Address of variable in which to store the original 
	(i.e. uncompressed) size of the attachment. If null, the output will be 
	suppressed.
pf_cmprssd: Optional Output. Address of variable in which to store whether the 
	attachment is stored compressed (TRUE) or not (FALSE). If null, the output 
	will be suppressed.
pus_host: Optional Output. Address of variable in which to store the host type 
	associated with the object. If null, the output will be skipped.
pbid_itm: Optional Output. Address of BlockID structure in which to store 
	the unlocked _item_ BlockID associated with the item descriptor for the 
	attachment. If null, the output will be suppressed.
pbid_ctnt: Optional Output. Address of the BlockID structure in which to store 
	the unlocked _content_ BlockID that contains attachment information. If 
	null, the output will be suppressed.
RETURN:
	eus_ERR_INVLD_ARG if any input parameter is obviously invalid
	ERR_ITEM_DATATYPE if the item found is not an attachment descriptor
	the Notes API error code if any error other than ERR_ITEM_NOT_FOUND occurred
	eus_SUCCESS otherwise

--- revision history -------
3/14/03 PR
+ signature change in support of maintaining appropriate constness
+ minor documentation adjustment

9/6/02 PR
+ extended to allow output of object type, prompting a signature change
+ extended to allow the object-name input to be optional
+ fixed logic error that was probably causing a memory leak
+ fixed bug by which false-positive matches could be returned
+ listing format adjustment, minor documentation adjustment, minor token 
  renaming

9/16/00 PR
+ made the string matching case insensitive
+ logic shortening
+ minor token renaming, error-handling improvement
+ completed standard documentation

9/12/99 PR: added compressed-flag return
1/31/99 PR: created			*/
STATUS eus_getAttachmentInfo( const NOTEHANDLE  h_NOTE, 
								const char  pc_ITM_NM[], 
								const char  pc_OBJNM[], 
								DWORD *const  pul_objId, 
								DWORD *const  pul_size, 
								BOOL *const  pf_cmprssd, 
								WORD *const  pus_host, 
								BLOCKID *const  pbid_itm, 
								BLOCKID *const  pbid_ctnt)	{
	const char *const  pc_ITMNM = pc_ITM_NM ? pc_ITM_NM : ITEM_NAME_ATTACHMENT;
	const WORD  us_LEN_ITEMNM = strlen( pc_ITMNM), 
				us_LEN_OBJNM = pc_OBJNM ? strlen( pc_OBJNM) : NULL;

	BLOCKID  bid_itm, bid_ctnt, bid_prvItm;
	BYTE * puc;
	const FILEOBJECT * pflo;
	STATUS  us_err;

	if (!h_NOTE)
		return eus_ERR_INVLD_ARG;

	if (pul_objId)
		*pul_objId = NULL;
	if (pul_size)
		*pul_size = NULL;
	if (pf_cmprssd)
		*pf_cmprssd = NULL;
	if (pus_host)
		*pus_host = NULL;
	if (pbid_itm)
		*pbid_itm = ebid_NULLBLOCKID;
	if (pbid_ctnt)
		*pbid_ctnt = ebid_NULLBLOCKID;

	//Get information about the first attachment item. If no such item exists, 
	//	return success with the output variables null, signifying this 
	//	condition.
	if (us_err = NSFItemInfo( (NOTEHANDLE) h_NOTE, (char *) pc_ITMNM, 
													us_LEN_ITEMNM, &bid_itm, 
													NULL, &bid_ctnt, NULL))
		if (ERR( us_err) == ERR_ITEM_NOT_FOUND)
			return eus_SUCCESS;
		else
			return us_err;

	//if the type of the item doesn't support attachments, short-curcuit with 
	//	failure
	if (!*(WORD *) (puc = OSLockBlock( BYTE, bid_ctnt)) == TYPE_OBJECT || 
								(pflo = (FILEOBJECT *) (puc + sizeof( 
								WORD)))->Header.ObjectType != OBJECT_FILE)	{
		us_err = ERR_ITEM_DATATYPE;
		goto errJump;
	}

	//if this isn't the item we're looking for, see if an ensuing one might 
	//	be...
	if (us_LEN_OBJNM)
		while (!( us_LEN_OBJNM == pflo->FileNameLength && strnicmp( pc_OBJNM, 
												(char *) (pflo + 1), 
												us_LEN_OBJNM) == ei_SAME))	{
			//if no next attachment-descriptor item can be obtained...
			OSUnlockBlock( bid_ctnt);
			bid_prvItm = bid_itm;
			if (us_err = NSFItemInfoNext( (NOTEHANDLE) h_NOTE, bid_prvItm, 
											(char *) pc_ITMNM, us_LEN_ITEMNM, 
											&bid_itm, NULL, &bid_ctnt, NULL))
				//if it's because no further item by that name exists...
				if (ERR( us_err) == ERR_ITEM_NOT_FOUND)
					//return success with the output variables null, signifying 
					//	this condition
					return eus_SUCCESS;
				//else short-circuit with failure
				else
					return us_err;

			//if the type of the item can't describe an attachment, 
			//	short-curcuit with failure
			if (!*(WORD *) (puc = OSLockBlock( BYTE, bid_ctnt)) == 
								TYPE_OBJECT || (pflo = (FILEOBJECT *) 
								(puc + sizeof( WORD)))->Header.ObjectType != 
								OBJECT_FILE)	{
				us_err = ERR_ITEM_DATATYPE;
				goto errJump;
			}
		} //while (!( us_LEN_OBJNM == pflo->FileNameLength && strnicmp(

	//fill outputs as requested
	if (pul_objId)
		*pul_objId = pflo->Header.RRV;
	if (pul_size)
		*pul_size = pflo->FileSize;
	if (pf_cmprssd)
		*pf_cmprssd = !!pflo->CompressionType;
	if (pus_host)
		*pus_host = pflo->HostType;
	if (pbid_itm)
		*pbid_itm = bid_itm;
	if (pbid_ctnt)
		*pbid_ctnt = bid_ctnt;

errJump:
	OSUnlockBlock( bid_ctnt);

	return us_err;
} //eus_getAttachmentInfo(


/** eus_DeleteAttachment( ***
Deletes the identified attachment from its note and database.

--- parameters & return ----
h_NOTE: handle to the note holding the identified attachment
pc_OBJNM: Optional. The object-name of the attachment (as opposed to the 
	filename that might be otherwise stored in a rich-text attachment hotspot). 
	If null, the first file-attachment item found using the pc_ITMNM input will 
	be processed.
pc_ITMNM: Optional. The name of the item associated with the attachment to be 
	deleted (as well as the item). If null, the default item name will be used.
RETURN:
	eus_SUCCESS if no error occured
	eus_ERR_INVLD_ARG if any input parameter is obviously invalid
	ERR_ITEM_DATATYPE if the item found is not an attachment descriptor
	ERR_ITEM_NOT_FOUND if the identified attachment could not be located
	the Notes API error code otherwise

--- revision history ------
3/14/03 PR: signature change toward maintaining appropriate constness
9/6/02 PR: created			*/
STATUS eus_DeleteAttachment( const NOTEHANDLE  h_NOTE, 
								const char  pc_OBJNM[], 
								char  pc_ITMNM[])	{
	BLOCKID  bid_itm;
	STATUS  us_err;

	if (!h_NOTE)
		return eus_ERR_INVLD_ARG;

	//get the info we need on the requested attachment
	if (us_err = eus_getAttachmentInfo( h_NOTE, pc_ITMNM, pc_OBJNM, NULL, NULL, 
													NULL, NULL, &bid_itm, NULL))
		return us_err;
	if (!bid_itm.pool)
		return ERR_ITEM_NOT_FOUND;

	//delete the attachment by regular means
	return NSFNoteDetachFile( h_NOTE, bid_itm);
} //eus_DeleteAttachment(


/** eus_objCopy( ***
Copies the identified object from one open database to another.

--- parameters & return ----
h_DB_TRGT: handle to the open database to receive the copy
h_DB_SRC: handle to the open database from which the object is to be copied
ul_OBJ_ID_SRC: identifier of the object in the source database
us_OBJ_TYP: type of the object to be copied
pul_objIdTrgt: Output. Address of variable to receive the identifier of the 
	object copy in the target database
RETURN:
	eus_SUCCESS if no error occured
	eus_ERR_INVLD_ARG if any input parameter is obviously invalid
	the Notes API error code otherwise

--- revision history ------
3/14/03 PR: created			*/
STATUS eus_objCopy( const DBHANDLE  h_DB_TRGT, 
					const DBHANDLE  h_DB_SRC, 
					const DWORD  ul_OBJ_ID_SRC, 
					const WORD  us_OBJ_TYP, 
					DWORD *const  pul_objIdTrgt)	{
	const DWORD ul_BUF = 0x400 * 0x100;

	DWORD  ul_len, ul_objId, ul = NULL, ul_;
	HANDLE  h = NULL;
	STATUS  us_err;
BYTE * puc;
	if (!( h_DB_TRGT && h_DB_SRC && ul_OBJ_ID_SRC && pul_objIdTrgt))
		return eus_ERR_INVLD_ARG;

	if (us_err = NSFDbGetObjectSize( h_DB_SRC, ul_OBJ_ID_SRC, us_OBJ_TYP, 
														&ul_len, NULL, NULL))
		return us_err;
	if (us_err = NSFDbAllocObject( h_DB_TRGT, ul_len, us_OBJ_TYP, NULL, 
																	&ul_objId))
		return us_err;

	do	{
		if (us_err = NSFDbReadObject( h_DB_SRC, ul_OBJ_ID_SRC, ul, ul_ = 
										ul_len < ul_BUF ? ul_len : ul_BUF, &h))
			goto errJump;
puc = OSLockObject( h);
OSUnlockObject( h);
		if (us_err = NSFDbWriteObject( h_DB_TRGT, ul_objId, h, ul, ul_))
			goto errJump;
		OSMemFree( h);
		ul += ul_;
	} while (ul_len -= ul_);

	*pul_objIdTrgt = ul_objId;
	return eus_SUCCESS;

errJump:
	if (h)
		OSMemFree( h);
	NSFDbFreeObject( h_DB_TRGT, ul_objId);

	return us_err;
} //eus_objCopy(


/** eus_getAttachInfoNext( ***
Obtain descriptive information on the next attachment found associated with a 
given item name.

--- parameters & return ----
h_NOTE: handle to the note containing the item descriptor associated with the 
	sought-for attachment
pc_ITM_NM: Optional. Address of the name of the item descriptor associated with 
	the attachement. If null, the default item name will be used.
pbid_itm: Input & Output. Address of the item-BLOCKID structure associated with 
	the item descriptor of the attachment preceding the attachment about which 
	information is being sought. If null, information about the first 
	attachment found will be output. For output, parameter will be reset to the 
	address of the item-BLOCKID structure associated with the item descriptor 
	of the next attachment found. If a next one is not found, address will be 
	nullified.
pc_objNm: Optional Output. Address of buffer into which the name of the object 
	is copied. If null, the output will be skipped.
pul_objId: Optional Output. Address of variable in which to store the RRV ID 
	associated with the specified object (see Notes API documentation re what 
	this is). If null, the output will be skipped.
pul_size: Optional Output. Address of variable in which to store the original 
	(i.e. uncompressed) size of the attachment. If null, the output will be 
	skipped.
pf_compressed: Optional Output. Address of variable in which to store whether 
	the attachment is stored compressed (TRUE) or not (FALSE). If null, the 
	output will be skipped.
pus_host: Optional Output. Address of variable in which to store the host type 
	associated with the object. If null, the output will be skipped.
pbid_ctnt: Optional Output. Address of the BlockID structure in which to store 
	the _content_ BlockID that contains attachment information. If null, the 
	output will be skipped.
RETURN:
	eus_ERR_INVLD_ARG if any input parameter is obviously invalid
	ERR_ITEM_DATATYPE if an item is found (by name) that cannot be an 
		attachment descriptor
	the Notes API error code if any error other than ERR_ITEM_NOT_FOUND occurred
	eus_SUCCESS otherwise

--- revision history -------
3/14/03 PR
+ signature change in support of appropriate constness
+ minor token renaming

9/6/02 PR: created			*/
STATUS eus_getAttachInfoNext( const NOTEHANDLE  h_NOTE, 
								const char  pc_ITM_NM[], 
								BLOCKID *const  pbid_itm, 
								char  pc_objNm[], 
								DWORD *const  pul_objId, 
								DWORD *const  pul_size, 
								BOOL *const  pf_compressed, 
								WORD *const  pus_host, 
								BLOCKID *const  pbid_ctnt)	{
	const char *const  pc_ITMNM = pc_ITM_NM ? pc_ITM_NM : ITEM_NAME_ATTACHMENT;
	const WORD  us_LEN_ITMNM = strlen( pc_ITMNM);

	BLOCKID  bid_itm, bid_ctnt;
	BYTE * puc;
	const FILEOBJECT * pflo;
	STATUS  us_err;

	if (!( h_NOTE && pbid_itm))
		return eus_ERR_INVLD_ARG;

	if (pc_objNm)
		*pc_objNm = NULL;
	if (pul_objId)
		*pul_objId = NULL;
	if (pul_size)
		*pul_size = NULL;
	if (pf_compressed)
		*pf_compressed = NULL;
	if (pus_host)
		*pus_host = NULL;
	if (pbid_ctnt)
		*pbid_ctnt = ebid_NULLBLOCKID;

	//if it's the first attachment-descriptor item that's being requested...
	if (ISNULLBLOCKID( (*pbid_itm)))	{
		//if no first attachment-descriptor item can be obtained...
		if (us_err = NSFItemInfo( (NOTEHANDLE) h_NOTE, (char *) pc_ITMNM, 
													us_LEN_ITMNM, &bid_itm, 
													NULL, &bid_ctnt, NULL))
			//if it's because no item by that name exists...
			if (ERR( us_err) == ERR_ITEM_NOT_FOUND)
				//return success with the output variables null, signifying 
				//	this condition
				return eus_SUCCESS;
			//else short-circuit with failure
			else
				return us_err;
	//else if no next attachment-descriptor item can be obtained...
	}else if (us_err = NSFItemInfoNext( (NOTEHANDLE) h_NOTE, *pbid_itm, 
											(char *) pc_ITMNM, us_LEN_ITMNM, 
											&bid_itm, NULL, &bid_ctnt, NULL))
		//if it's because no further item by that name exists...
		if (ERR( us_err) == ERR_ITEM_NOT_FOUND)	{
			//return success with all output variables null, signifying this 
			//	condition
			if (pbid_itm)
				*pbid_itm = ebid_NULLBLOCKID;
			return eus_SUCCESS;
		//else short-circuit with failure
		}else
			return us_err;

	//if the type of the item can't describe an attachment, short-curcuit with 
	//	failure
	if (!*(WORD *) (puc = OSLockBlock( BYTE, bid_ctnt)) == TYPE_OBJECT || 
								(pflo = (FILEOBJECT *) (puc + sizeof( 
								WORD)))->Header.ObjectType != OBJECT_FILE)	{
		us_err = ERR_ITEM_DATATYPE;
		goto errJump;
	}

	//fill outputs as requested
	if (pc_objNm)	{
		memcpy( pc_objNm, (char *) (pflo + 1), pflo->FileNameLength);
		pc_objNm[ pflo->FileNameLength] = NULL;
	}
	if (pul_objId)
		*pul_objId = pflo->Header.RRV;
	if (pul_size)
		*pul_size = pflo->FileSize;
	if (pf_compressed)
		*pf_compressed = !!pflo->CompressionType;
	if (pus_host)
		*pus_host = pflo->HostType;
	if (pbid_itm)
		*pbid_itm = bid_itm;
	if (pbid_ctnt)
		*pbid_ctnt = bid_ctnt;

errJump:
	OSUnlockBlock( bid_ctnt);

	return us_err;
} //eus_getAttachInfoNext(


/** eus_PrepObjectWrite( ***
Prepares a context by which a caller may begin writing an object to a Notes 
database in sequential chunks.

--- parameters & return ----
h_DB: handle to database to contain the object
ul_LEN: size object is expected to be
pt_obj: Output. Address of information structure, opaque to caller, to receive 
	context information needed by this module for its work with the database 
	object.
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any input parameter is obviously invalid
	the Notes API error code otherwise

--- revision history ------
3/14/03 PR: created			*/
STATUS eus_PrepObjectWrite( const DBHANDLE  h_DB, 
							const DWORD  ul_LEN, 
							ObjInfo *const  pt_obj)	{
	STATUS  us_err;

	if (!( h_DB && ul_LEN && pt_obj))
		return eus_ERR_INVLD_ARG;

	memset( pt_obj, NULL, sizeof( ObjInfo));


	if (us_err = NSFDbAllocObject( h_DB, ul_LEN, NOTE_CLASS_DOCUMENT, NULL, 
																&pt_obj->ul_id))
		return us_err;
	pt_obj->ul_len = ul_LEN;
	pt_obj->h_db = h_DB;

	return eus_SUCCESS;
} //eus_PrepObjectWrite(


/** eus_AppendToObject( ***
Appends a length of content to an object.

--- parameters & return ----
H: handle to Notes memory object holding the content to be appended
ul_LEN: length of content to append
pt_obj: Input & Output. Address of information structure, opaque to caller, 
	that provides and receives context information needed by this module for 
	its work with the database object.
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any input parameter is obviously invalid
	the Notes API error code otherwise

--- revision history ------
3/14/03 PR: created		*/
STATUS eus_AppendToObject( const HANDLE  H, 
							const DWORD  ul_LEN, 
							ObjInfo *const  pt_obj)	{
	STATUS  us_err;

	if (!( H && ul_LEN && pt_obj && pt_obj->ul_id && pt_obj->ul_wrtn + 
													ul_LEN < pt_obj->ul_len))
		return eus_ERR_INVLD_ARG;

	//copy the object content in the Notes-memory buffer into the space 
	//	allocated for the object in the target database
	if (us_err = NSFDbWriteObject( pt_obj->h_db, pt_obj->ul_id, H, 
												pt_obj->ul_wrtn, ul_LEN))	{
		NSFDbFreeObject( pt_obj->h_db, pt_obj->ul_id);
		pt_obj->ul_id = NULL;
		return us_err;
	}

	//update the object descriptor
	pt_obj->ul_wrtn += ul_LEN;

	return eus_SUCCESS;
} //eus_AppendToObject(


/** eus_releaseObject( ***
Releases resources associated with an object caller has decided not to maintian.

--- parameter & return ----
pt_obj: Input & Output. Address of information structure, opaque to caller, 
	that provides and receives context information needed by this module for 
	its work with the database object. Structure will be invalidated by this 
	call, for safety's sake.
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any input parameter is obviously invalid
	the Notes API error code otherwise

--- revision history -----
3/14/03 PR: created		*/
STATUS eus_releaseObject( ObjInfo *const  pt_obj)	{
	STATUS  us_err;

	if (!pt_obj)
		return eus_ERR_INVLD_ARG;

	if (!pt_obj->ul_id)
		return eus_SUCCESS;

	us_err = NSFDbFreeObject( pt_obj->h_db, pt_obj->ul_id);
	pt_obj->ul_id = NULL;

	return us_err;
} //eus_releaseObject(


/** eus_CommitObject( ***
Finishes the creation of a new database object with its commission to a 
document.

--- parameters & return ----
h_NOTE: handle to Notes document to be associated with the object
pc_ITMNM: Optional. Address of the name of the object-association item to be 
	added to the specified document. If null, default item name will be used.
pc_OBJNM: address of the name to be associated with the object within the 
	document. If the name is an extended filename, the path portion will be 
	ignored.
us_HOST_TYP: the Notes "host type" to associate with the object and the 
	document's object-association item
H: Optional. Handle to Notes memory object holding the final content to be 
	appended to the object. If null, so further appendage will occur.
ul_LEN: Optional, Required if H Input Provided. Length of the final content to 
	append to the object.
pt_obj: Input & Output. Address of information structure, opaque to caller, 
	that provides and receives context information needed by this module for 
	its work with the database object. Upon successful completion, procedure 
	will invalidate the structure, for safety's sake.
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any input parameter is obviously invalid
	the Notes API error code otherwise

--- revision history ------
3/14/03 PR: created		*/
STATUS eus_CommitObject( const NOTEHANDLE  h_NOTE, 
							const char  pc_ITMNM[], 
							const char  pc_OBJNM[], 
							const WORD  us_HOST_TYP, 
							const HANDLE  H, 
							const DWORD  ul_LEN, 
							ObjInfo *const  pt_obj)	{
	STATUS  us_err;

	if (!( h_NOTE && pc_OBJNM && *pc_OBJNM && pt_obj && pt_obj->ul_id))
		return eus_ERR_INVLD_ARG;

	if (H)
		if (us_err = eus_AppendToObject( H, ul_LEN, pt_obj))
			return us_err;

	//realloc object size if shorter than that allocated (NOT YET DONE)

	//add the object-associate item to the note to tie it to the object that's 
	//	been created
	if (us_err = us_AttachObjectItem( h_NOTE, pt_obj->ul_id, pc_ITMNM && 
									*pc_ITMNM ? pc_ITMNM : epc_ITMNM_STD_ATTCH, 
									pc_OBJNM, pt_obj->ul_wrtn, us_HOST_TYP))
		NSFDbFreeObject( pt_obj->h_db, pt_obj->ul_id);

	pt_obj->ul_id = NULL;

	return us_err;
} //eus_CommitObject(


/** eus_getAttchOriginalNm( ***
Given the internal object name of an attachment, determine its associated 
original filename.

--- parameters & return ----
pc_OBJNM: address of the internal object name of the target attachment
H: handle to the note "containing" the attachment
pc_ITMNM: address of the name of the rich-text field containing the 
	attachment hotspot associated with the target attachment
f_COPY: flag telling whether the caller should be given a duplicate copy of 
	the original filename found (TRUE) or is content with the address of the 
	filename within internal Notes memory which should not be manipulated
ppc: address of pointer variable in which to output the address of the 
	original filename associated with the attachment
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any input parameter is obviously invalid
	the Notes API error code otherwise

--- revision history ------
?/?/0? PR: created			*/
/*PROTO!!
STATUS eus_getAttchOriginalNm( const char  pc_OBJNM[], 
								NOTEHANDLE  H, 
								char  pc_ITMNM[], 
								const BOOL  f_COPY, 
								char *const * ppc)	{
} //eus_getAttchOriginalNm(
*/

/** eus_AttachBufferAsObject( ***
Construct an object in the target database and note that contains the specified 
content.

--- parameters & return ----
PUC: pointer to the buffer containing the object's content
ul_LEN: length of the object's content
h_DB: Optional. Handle of the database to hold the consturcted object. If NULL, 
	will be manufactured from the h_NOTE input.
h_NOTE: handle of the note to hold a reference item to the object
pc_ITMNM: address of the name of the reference item to be added to the 
	specified note
pc_OBJNM: Address of the name to be associated with the object. If the name is 
	an extended filename, the path portion will be ignored.
us_HOST_TYPE: the Notes "host type" to associate with the object and the 
	reference item
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any input parameter is obviously invalid
	the Notes API error code otherwise

--- revision history -------
9/6/02 PR: listing format adjustment, minor exception-handling adjustment

8/9/00 PR
+ standard documentation
+ exception-handling adjustment, token renaming

1/30/99 PR: created			*/
STATUS eus_AttachBufferAsObject( const BYTE *const  PUC, 
									const DWORD  ul_LEN, 
									DBHANDLE  h_DB, 
									NOTEHANDLE  h_NOTE, 
									char  pc_ITMNM[], 
									const char  pc_OBJNM[], 
									const WORD  us_HOST_TYPE)	{
	DBHANDLE  h_Db;
	DWORD  ul_objId;
	HANDLE  h;
	STATUS  us_err;

	if (!( PUC && ul_LEN && h_NOTE && pc_ITMNM && pc_OBJNM))
		return eus_ERR_INVLD_ARG;

	//allocate space for the object within the target database
	if (!( h_Db = h_DB))
		NSFNoteGetInfo( h_NOTE, _NOTE_DB, &h_Db);
	if (us_err = NSFDbAllocObject( h_Db, ul_LEN, NOTE_CLASS_DOCUMENT, NULL, 
																	&ul_objId))
		return us_err;

	//allocate generic Notes space for and copy in the object content
	if (us_err = OSMemAlloc( NULL, ul_LEN, &h))
		goto errJump;
	memcpy( OSLockObject( h), PUC, ul_LEN);
	OSUnlockObject( h);

	//add an "object" item to the note that references the object we're creating
	if (us_err = us_AttachObjectItem( h_NOTE, ul_objId, pc_ITMNM, pc_OBJNM, 
														ul_LEN, us_HOST_TYPE))
		goto errJump;

	//copy the object content in the Notes-memory buffer into the space 
	//	allocated for the object in the target database
	us_err = NSFDbWriteObject( h_Db, ul_objId, h, 0L, ul_LEN);

errJump:
	OSMemFree( h);
	if (us_err && ul_objId)
		NSFDbFreeObject( h_Db, ul_objId);

	return us_err;
} //eus_AttachBufferAsObject(


/** eus_AttachFileAsObject( ***
Construct an object in the target database and note from the specified file.

--- parameters & return ----
pc_EXTFILENM: address of the extended filename of the source file to be copied
h_DB: Optional. Handle of the database to hold the consturcted object. If null, 
	will be manufactured from the h_NOTE input.
h_NOTE: handle of the note to hold a reference item to the object
pc_ITMNM: address of the name of the reference item to be added to the 
	specified note
pc_OBJNM: Optional. Address of the name to be associated with the object. If 
	the name is an extended filename, the path portion will be ignored. If 
	null, the filename portion of the source extended filename will be used.
us_HOST_TYPE: the Notes "host type" to associate with the object and the 
	reference item
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any input parameter is obviously invalid
	the Notes API error code otherwise

--- revision history -------
3/14/03 PR: signature change in support of switch to casting away const when 
	needed due to Notes' disregard of constness
9/6/02 PR: listing format adjustment, minor exception-handling adjustment, 
	minor documentation adjustment
8/9/00 PR: created			*/
STATUS eus_AttachFileAsObject( const char  pc_EXTFILENM[], 
								const DBHANDLE  h_DB, 
								const NOTEHANDLE  h_NOTE, 
								const char  pc_ITMNM[], 
								const char  pc_OBJNM[], 
								const WORD  us_HOST_TYPE)	{
	DBHANDLE  h_db;
	DWORD  ul_objId;
	FILE * pfl;
	int  i;
	HANDLE  h = NULL;
	STATUS  us_err;

	if (!( pc_EXTFILENM && h_NOTE && pc_ITMNM))
		return eus_ERR_INVLD_ARG;

	//allocate space for the object within the target database
	if (!(pfl = fopen( pc_EXTFILENM, "rb")))
		return !eus_SUCCESS;
	if (us_err = ((i = filelength( fileno( pfl))) == ei_FAIL))
		goto errJump;
	if (!( h_db = (DBHANDLE) h_DB))
		NSFNoteGetInfo( (NOTEHANDLE) h_NOTE, _NOTE_DB, &h_db);
	if (us_err = NSFDbAllocObject( h_db, i, NOTE_CLASS_DOCUMENT, NULL, 
																	&ul_objId))
		goto errJump;

	//allocate generic Notes space for and copy in the object content
	if (us_err = OSMemAlloc( NULL, i, &h))
		goto errJump;
	if (fread( OSLockObject( h), 1, i, pfl) != (UINT) i)
		goto errJump;
	OSUnlockObject( h);

	//add an "object" item to the note that references the object we're creating
	if (us_err = us_AttachObjectItem( h_NOTE, ul_objId, pc_ITMNM, pc_OBJNM ? 
													pc_OBJNM : pc_EXTFILENM, 
													i, us_HOST_TYPE))
		goto errJump;

	//copy the object content in the Notes-memory buffer into the space 
	//	allocated for the object in the target database
	us_err = NSFDbWriteObject( h_db, ul_objId, h, 0L, i);

errJump:
	if (fclose( pfl) == EOF && us_err)
		us_err = !eus_SUCCESS;
	if (h)
		OSMemFree( h);
	if (us_err && ul_objId)
		NSFDbFreeObject( h_db, ul_objId);

	return us_err;
} //eus_AttachFileAsObject(


/** eus_getObjectContentsInBuffer( ***
Copies the contents of a database object (usually a file) into a handled 
buffer. If the object is not compressed, it is more efficient to provide the 
object ID instead of the item BlockID associated with the item associated with 
the item on the note. (The former executes wholly in memory, the latter through 
by intermediation through the file system.)

--- parameters & return ------
ul_OBJID: Provisionally Optional. The ID (RRV) of the object in the database. 
	Ignored if bid_ITM is provided; required otherwise. If used, object will 
	be read into memory directly.
bid_ITM: Optional. The item BlockID associated with the note item referring to 
	the object. If provided, procedure will do its work using the file system 
	as an intermediary.
ul_LEN_CTNT: Optional. The length of the content to be read into the handled 
	buffer. If omitted or equal to the API's MAXDWORD constant, the entire 
	content of the object will be read into the buffer.
h_NOTE: Provisionally Optional. If bid_ITM is provided, the handle to the note 
	containing the item referring to the object. Else if bid_ITM not provided, 
	the handle to any note in the database containing the object, although not 
	necessary if h_DB is provided.
h_DB: Optional. Handle to the database containing the object. Ignored if 
	bid_ITM is provided. Otherwise if not provided, the handle will be 
	determined using by means of the h_NOTE parameter.
ph_ctnt: Address of the variable to receive the handle to the buffer filled 
	with the object's content.
RETURN: eus_SUCCESS if no error occured. The Notes API error code if an API 
	error occurred. !eus_SUCCESS if the object contained less content than 
	that specified by ul_LEN_CTNT.

--- suggested enhancement ----
3/14/03 PR
+ Should have a smarter way of generating temporary filenames or employ some 
  synchronization measure so this procedure can be thread safe

--- revision history ---------
3/14/03 PR
+ minor signature adjustment to provide appropriate constness
+ documentation adjustment, listing format adjustment, token renaming

9/12/99 PR
+ overhauled to make more generic
+ full documentation added

12/12/98 PR: created		*/
STATUS eus_getObjectContentsInBuffer( const DWORD  ul_OBJID, 
										const BLOCKID  bid_ITM, 
										const DWORD  ul_LEN_CTNT, 
										const NOTEHANDLE  h_NOTE, 
										const DBHANDLE  h_DB, 
										HANDLE *const  ph_ctnt)	{
	static const char  pc_FILENM_TMP[] = "~gocib.tmp";

	static char  pc_extFileNmTmp[ mus_MAXBUF_EXTFILENM - 
														sizeof( pc_FILENM_TMP)];

	DBHANDLE  h_Db;
	HANDLE  h = NULL;
	STATUS  us_err = NULL;
	BOOL  f_fail = FALSE;

	if (!( (ul_OBJID || bid_ITM.pool) && (bid_ITM.pool ? (const BOOL) h_NOTE : 
													(!h_DB ? (const BOOL) 
													h_NOTE : TRUE)) && ph_ctnt))
		return !eus_SUCCESS;

	*ph_ctnt = NULL;

	//if we need to work via the file system here...
	if (bid_ITM.pool)	{
		unsigned long  ul;
		int  i_err;

		//if this is the first time through with a file, initialize the 
		//	variable to hold the path to the user's temporary directory
		if (!*pc_extFileNmTmp)	{
			if (!epc_getTempDirNm( pc_extFileNmTmp, mus_MAXBUF_EXTFILENM - 2))
				return !eus_SUCCESS;
			strcat( pc_extFileNmTmp, pc_FILENM_TMP);
		}

		//extract the file as a temporary file on the file system
		if (us_err = NSFNoteExtractFile( (NOTEHANDLE) h_NOTE, bid_ITM, 
														pc_extFileNmTmp, NULL))
			return us_err;

		//if necessary, determine the length of the content to be read into the 
		//	buffer
		if (!ul_LEN_CTNT || ul_LEN_CTNT == MAXDWORD)	{
			struct stat  st;

			if (f_fail = !stat( pc_extFileNmTmp, &st))
				goto errJump;

			if (f_fail = (ul = st.st_size) <= 0)
				goto errJump;
		}else
			ul = ul_LEN_CTNT;

		//allocate a handled buffer to accommodate the content we're going to 
		//	read
		if (us_err = OSMemAlloc( NULL, ul, &h))
			goto errJump;

		//read the specified amount of content from the file into the buffer
		f_fail = !ef_LoadBinaryFile( pc_extFileNmTmp, OSLockObject( h), ul);
		OSUnlockObject( h);
		if (f_fail)
			goto errJump;

		//delete the temporary file, ignoring failure
		i_err = remove( pc_extFileNmTmp);
		_ASSERTE( i_err == eus_SUCCESS);
	//else read the object into memory directly
	}else {
		if (!( h_Db = h_DB))
			NSFNoteGetInfo( (NOTEHANDLE) h_NOTE, _NOTE_DB, &h_Db);
		if (us_err = NSFDbReadObject( h_Db, ul_OBJID, 0, ul_LEN_CTNT ? 
											ul_LEN_CTNT : MAXDWORD, &h))	{
			_ASSERTE( !h);
			return us_err;
		}
	} //if (bid_ITM.pool)

	*ph_ctnt = h;

	return eus_SUCCESS;

errJump:
	if (f_fail)
		remove( pc_extFileNmTmp);

	return us_err + f_fail;
} //eus_getObjectContentsInBuffer(


