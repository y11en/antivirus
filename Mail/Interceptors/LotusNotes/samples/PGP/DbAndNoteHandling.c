/*____________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.
	

	$Id: DbAndNoteHandling.c 12258 2003-02-13 23:57:46Z sdas $
____________________________________________________________________________*/

/*::: MODULE OVERVIEW :::::::::::::
Reüsable, common Notes database- & template-handling library.

--- suggested enhancement ----
2/22/00 PR: see important suggestions in the suggestion documentation to 
	eus_GetDesignNote()

--- revision history ---------
2/22/00 Version 1.0.2
+ addition of eus_GetDesignNote() to accommodate location of design notes by 
  alias name if can't be located by main title
+ documentation adjustment

9/12/99 Version 1.0: Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#include "DbAndNoteHandling.h"


//arbitrary codes within Lotus' PKG_UNDERUTIL1 error-code package, used as 
//	library-specific error codes here
static const WORD  mi_ERR_NOT_MAIL_DB = 0x06D6, 
					mi_ERR_NOT_TEMPLATE = 0x06D7;


/** eus_OpenMailTemplate( ***
Returns a handle to the Notes Mail template specified on the given server, 
once assured that the database is in fact a Notes Mail template.

--- parameters & return ----
pc_FILENM: pointer to the extended filename of the Notes database to be 
	opened
pc_SERVERNM: pointer to the name of the server on which the specified Notes 
	database should reside
ph: Output. Pointer to the handle variable in which to store the handle to 
	the specified Notes Mail template.  Guaranteed to be null if database 
	could not be validated as a true Notes Mail template.
pc_class: Optional Output. Pointer to the buffer to fill with the class 
	(template) name of the specified database, should be of minimum size 
	NSF_INFO_SIZE to be completely safe. Caller may specify NULL for the 
	parameter if the output isn't needed.
RETURN:
	eus_SUCCESS if no error occured
	ERR_DIRECTORY if the specified filename is a sub-directory, not a 
		database
	mi_ERR_NOT_MAIL_DB if a Notes database of the specified name was found, 
		but it doesn't seem to be a Notes Mail database
	mi_ERR_NOT_TEMPLATE if the Notes Mail database has no design class and 
		thus is not considered a template
	the Notes API error code otherwise

--- revision history -------
9/12/99 PR: created			*/
STATUS eus_OpenMailTemplate( char  pc_FILENM[], 
								char  pc_SERVERNM[], 
								DBHANDLE *const  ph, 
								char *const  pc_class)	{
	STATUS  us_err;

	if (!( pc_FILENM && *pc_FILENM && pc_SERVERNM && ph))
		return !eus_SUCCESS;

	if (pc_class)
		*pc_class = NULL;

	//open the specified database and ensure it's a Notes Mail database
	if (us_err = eus_OpenMailDb( pc_FILENM, pc_SERVERNM, ph))
		return us_err;

	if (us_err = eus_TestDbIsTemplate( *ph, pc_class))	{
		NSFDbClose( *ph);
		*ph = NULL;
	}

	return us_err;
} //eus_OpenMailTemplate(


/** eus_getTemplateClass( ***
Obtains a copy of the class name of the given, already opened Notes template 
database.

--- parameters & return ----
H: handle to the already opened Notes template database
pc_class: Output. Pointer to the buffer to fill with the class (template) 
	name of the specified database, should be of minimum size NSF_INFO_SIZE 
	to be completely safe.
RETURN:
	eus_SUCCESS if no error occured
	!eus_SUCCESS if invalid parameters were passed
	the Notes API error code otherwise

--- revision history -------
9/12/99 PR: created			*/
STATUS eus_getTemplateClass( DBHANDLE  H, 
								char *const  pc_class)	{
	char  pc_info[ NSF_INFO_SIZE];
	STATUS  us_err;

	if (!( H && pc_class))
		return !eus_SUCCESS;

	if (us_err = NSFDbInfoGet( H, pc_info))
		return us_err;
	NSFDbInfoParse( pc_info, INFOPARSE_CLASS, pc_class, NSF_INFO_SIZE - 1);

	return eus_SUCCESS;
} //eus_getTemplateClass(


/** eus_RemoveDesignNote( ***
Removes the specified design note from the given database.

--- parameters & return ----
h_DB_TARGET: handle to the target database
us_NOTE_CLASS: the class of the note to be removed from the target database
pc_NOTENM: Optional. Pointer to the name of the design-element note to be 
	removed from the target database. Required if the note class 
	(us_NOTE_CLASS) is not of the unique-note type. If the note class is of 
	the unique-note type, the parameter is ignored.
RETURN:
	eus_SUCCESS if no error occured
	!eus_SUCCESS if invalid parameters were passed
	the Notes API error code otherwise

--- revision history -------
2/22/00 PR: minor documentation adjustment
9/12/99 PR: created			*/
STATUS eus_RemoveDesignNote( DBHANDLE  H, 
								const WORD  us_NOTE_CLASS, 
								char  pc_NOTENM[])	{
	const BOOL  f_SPECIAL_DESIGN_NOTE = !!(us_NOTE_CLASS & 
												NOTE_CLASS_SINGLE_INSTANCE);

	NOTEID  nid;
	STATUS  us_err;

	if (!( H && us_NOTE_CLASS && (!pc_NOTENM ? f_SPECIAL_DESIGN_NOTE : 
																	TRUE)))
		return !eus_SUCCESS;

	//get the NoteID of the design note to be replaced
	if (!f_SPECIAL_DESIGN_NOTE)
		us_err = eus_GetDesignNote( H, us_NOTE_CLASS, pc_NOTENM, &nid);
//us_err = NIFFindDesignNote( H, pc_NOTENM, us_NOTE_CLASS, &nid);
	else
		us_err = NSFDbGetSpecialNoteID( H, (WORD) (SPECIAL_ID_NOTE | 
														us_NOTE_CLASS), &nid);

	//if the note wasn't found, short-circuit with success
	if (us_err)
		return ERR( us_err) == ERR_NOT_FOUND ? eus_SUCCESS : us_err;

	//delete the design note from the target database
	return NSFNoteDelete( H, nid, NULL);
} //eus_RemoveDesignNote(


/** eus_GetDesignNote( ***
Obtain the NoteID of the specified database design note.

--- parameters & return ------
h_DB_TARGET: handle to the target database
us_NOTE_CLASS: The class(es) of design note to be located in the target 
	database. Multiple non-single-instance classes may be specified by ORing 
	them together.
pc_NOTENM: Optional. Pointer to the name of the design-element note to be 
	located in the target database. Required if the note class 
	(us_NOTE_CLASS) is not of the unique-note type. If the note class is of 
	the unique-note type, the parameter is ignored.
pnid: Output. Address of the variable in which to store the NoteID of the 
	located design note.
RETURN:
	eus_SUCCESS if no error occured
	!eus_SUCCESS if invalid parameters were passed
	ERR_NOT_FOUND if specified design note could not be located in the 
		database
	the Notes API error code otherwise

--- suggested enhancement ----
2/22/00 PR
+ should not search for aliases in view/folder, agent or page notes as those 
  do aliasing by pipe delimiter in a regular text field (not text-list), and 
  NIFFindDesignNote() finds aliases in such notes correctly
+ important to allow case-insensitivity when searching aliases, as 
  NIFFindDesignNote() works that way

--- revision history ---------
2/22/00 PR: created			*/
STATUS eus_GetDesignNote( DBHANDLE  H, 
							const WORD  us_NOTE_CLASS, 
							char *const  pc_NOTENM, 
							NOTEID *const  pnid)	{
	const BOOL  f_SPECIAL_DESIGN_NOTE = !!(us_NOTE_CLASS & 
												NOTE_CLASS_SINGLE_INSTANCE);

	HCOLLECTION  hcl;
	COLLECTIONPOSITION  t_pos;
	WORD  us_flags;
	NOTEID  nid, nid_tentative;
	STATUS  us_err, us_errTemp;

	if (!( H && us_NOTE_CLASS && (!pc_NOTENM ? f_SPECIAL_DESIGN_NOTE : 
															TRUE) && pnid))
		return !eus_SUCCESS;

	*pnid = nid = nid_tentative = NULL;

	//if we're looking for a "special" design note (i.e. one-of-a-kind and 
	//	therefore not necessary to "name"), try to get ahold of it and return 
	//	the result
	if (f_SPECIAL_DESIGN_NOTE)
		return NSFDbGetSpecialNoteID( H, (WORD) (SPECIAL_ID_NOTE | 
														us_NOTE_CLASS), pnid);

	//if the named design note can be located by normal means, do so and 
	//	return the result
	if ((us_err = NIFFindDesignNote( H, pc_NOTENM, us_NOTE_CLASS, pnid)) && 
												ERR( us_err) != ERR_NOT_FOUND)
		return us_err;
	if (!us_err)
		return eus_SUCCESS;

	//Since the name wasn't found by main title, we will try to to find a 
	//	matching alias. First, get ahold of the database's design-note 
	//	collection.
	if (us_err = NIFOpenCollection( H, H, NOTE_CLASS_DESIGN | 
									NOTE_ID_SPECIAL, OPEN_SHARED_VIEW_NOTE, 
									NULLHANDLE, &hcl, NULL, NULL, NULL, NULL))
		return us_err;

	//then for each design note...
	memset( &t_pos, NULL, sizeof( COLLECTIONPOSITION));
	do	{
		const DWORD  ul_ALL_ENTRIES = 0xFFFFFFFF;
		const WORD  us_OFFSET_STATIC = sizeof( NOTEID) + sizeof( WORD);

		HANDLE  h;
		DWORD  ul;
		void * pv_entry;
		WORD  us_entry;

		if ((us_err = NIFReadEntries( hcl, &t_pos, NAVIGATE_NEXT, 1, 
									NAVIGATE_NEXT, ul_ALL_ENTRIES, 
									READ_MASK_NOTEID | READ_MASK_NOTECLASS | 
									READ_MASK_SUMMARY, &h, NULL, NULL, &ul, 
									&us_flags)) || !ul)
			goto errJump;
		pv_entry = OSLockObject( h);
		for (us_entry = 0; us_entry < ul; us_entry++, (BYTE *) pv_entry += 
										us_OFFSET_STATIC + ((ITEM_TABLE *) 
										((BYTE *) pv_entry + 
										us_OFFSET_STATIC))->Length)	{
			void * pv;
			WORD  us_typ, us_count, us;

			//if the note is not of the desired design class, loop for the 
			//	next note
			if (!( *(WORD *) ((NOTEID *) pv_entry + 1) & us_NOTE_CLASS))
				continue;

			//if the note has no alias info, loop for the next note
			if (!NSFLocateSummaryValue( (ITEM_TABLE *) ((BYTE *) pv_entry + 
											us_OFFSET_STATIC), FIELD_TITLE, 
											&pv, &us, &us_typ) || 
											us_typ != TYPE_TEXT_LIST)
				continue;

			//if no alias match is found, loop for the next note
			if (!ef_TextListContainsEntry( pv, FALSE, pc_NOTENM, &us, 
																&us_count))
				continue;
			_ASSERTE( us && us_count > 1);

			//if our match is the last alias in the title item, or if the 
			//	last alias is also a match, set the output NoteID, then 
			//	break out of these loops
			if (us == us_count - 1 || ef_TextListEntryMatches( pv, FALSE, 
													(WORD) (us_count - 1), 
													pc_NOTENM))	{
				nid = *(NOTEID *) pv_entry;
				break;
			}

			//if we haven't found another already, note that we've found a 
			//	tentative match
			if (!nid_tentative)
				nid_tentative = *(NOTEID *) pv_entry;
		} //for (us_entry = 0; us_entry < ul

		//free resouces associated with this instance of reading the 
		//	design-note collection
		OSUnlockObject( h);
		OSMemFree( h);
	} while (us_flags & SIGNAL_MORE_TO_DO && !nid);

	//if no "output" NoteID was set...
	if (!nid)
		//if a tentative NoteID match was found...
		if (nid_tentative)
			//we can now bless it as the actual match, so commit it as the 
			//	output NoteID
			*pnid = nid_tentative;
		//else set an error that the note was not found
		else
			us_err = ERR_NOT_FOUND;
	//else commit the "output" NoteID as the actual output
	else
		*pnid = nid;

errJump:
	if (us_errTemp = NIFCloseCollection( hcl))
		if (!us_err)
			us_err = us_errTemp;

	return us_err;
} //eus_GetDesignNote(


/** eus_getDbDesignClass( ***
Obtains a copy of the design-class name of the given, already opened Notes 
database.

--- parameters & return ----
H: handle to the already opened Notes database
pc_class: Output. Pointer to the buffer to fill with the design-class 
	(template inheritance) name of the specified database, should be of 
	minimum size NSF_INFO_SIZE to be completely safe.
RETURN:
	eus_SUCCESS if no error occured
	!eus_SUCCESS if invalid parameters were passed
	the Notes API error code otherwise

--- revision history -------
9/12/99 PR: created			*/
STATUS eus_getDbDesignClass( DBHANDLE  H, 
								char *const  pc_class)	{
	char  pc_info[ NSF_INFO_SIZE];
	STATUS  us_err;

	if (!( H && pc_class))
		return !eus_SUCCESS;

	if (us_err = NSFDbInfoGet( H, pc_info))
		return us_err;
	NSFDbInfoParse( pc_info, INFOPARSE_DESIGN_CLASS, pc_class, 
														NSF_INFO_SIZE - 1);

	return eus_SUCCESS;
} //eus_getDbDesignClass(


/** eus_OpenMailDb( ***
Returns a handle to the Notes Mail database specified on the given server, 
once assured that the database is in fact a Notes Mail database.

--- parameters & return ----
pc_FILENM: pointer to the extended filename of the Notes database to be 
	opened
pc_SERVERNM: pointer to the name of the server on which the specified Notes 
	database should reside
ph: Output. Pointer to the handle variable in which to store the handle to 
	the specified Notes Mail database.  Guaranteed to be null if database 
	could not be validated as a true Notes Mail database.
RETURN: 
	eus_SUCCESS if no error occured
	ERR_DIRECTORY if the specified filename is a directory, not a database
	mi_ERR_NOT_MAIL_DB if a Notes database of the specified name was found, 
		but it doesn't seem to be a Notes Mail template
	the Notes API error code otherwise

--- revision history -------
9/12/99 PR: created			*/
STATUS eus_OpenMailDb( char  pc_FILENM[], 
						char  pc_SERVERNM[], 
						DBHANDLE *const  ph)	{
	char  pc_VIEWNM_TEST1[] = "($VIM23)", pc_VIEWNM_TEST2[] = "($Drafts)", 
			pc_FORMNM_TEST1[] = "(Trace Report)", 
			pc_FORMNM_TEST2[] = "(Personal Stationery)";

	DBHANDLE  h;
	NOTEID  nid;
	STATUS  us_err;

	if (!( pc_FILENM && *pc_FILENM && pc_SERVERNM && ph))
		return !eus_SUCCESS;

	*ph = NULL;

	if (us_err = eus_OpenDb( pc_FILENM, pc_SERVERNM, &h))
		return us_err;

	//ensure that the database is a Notes Mail database
/*	if (us_err = eus_GetDesignNote( h, NOTE_CLASS_VIEW, pc_VIEWNM_TEST1, 
																		&nid))
		goto errJump;
	if (us_err = eus_GetDesignNote( h, NOTE_CLASS_VIEW, pc_VIEWNM_TEST2, 
																		&nid))
		goto errJump;
	if (us_err = eus_GetDesignNote( h, NOTE_CLASS_FORM, pc_FORMNM_TEST1, 
																		&nid))
		goto errJump;
	if (us_err = eus_GetDesignNote( h, NOTE_CLASS_FORM, pc_FORMNM_TEST2, 
																		&nid))
		goto errJump;
*/	if (us_err = NIFFindDesignNote( h, pc_VIEWNM_TEST1, NOTE_CLASS_VIEW, 
																		&nid))
		goto errJump;
	if (us_err = NIFFindDesignNote( h, pc_VIEWNM_TEST2, NOTE_CLASS_VIEW, 
																		&nid))
		goto errJump;
	if (us_err = NIFFindDesignNote( h, pc_FORMNM_TEST1, NOTE_CLASS_FORM, 
																		&nid))
		goto errJump;
	if (us_err = NIFFindDesignNote( h, pc_FORMNM_TEST2, NOTE_CLASS_FORM, 
																		&nid))
		goto errJump;

	*ph = h;
	return eus_SUCCESS;

errJump:
	NSFDbClose( h);

	return ERR( us_err) == ERR_NOT_FOUND ? mi_ERR_NOT_MAIL_DB : us_err;
} //eus_OpenMailDb(


/** eus_OpenDb( ***
Attempts to open the specified Notes database on the given server. Validates 
that the "database" is not really a subdirectory path, the alternative 
return of NSFDbOpen().

--- parameters & return ----
pc_FILENM: pointer to the extended filename of the Notes database to be 
	opened
pc_SERVERNM: pointer to the name of the server on which the specified Notes 
	database resides
ph: Output. Pointer to the handle variable in which to store the handle to 
	the specified Notes database.  Guaranteed to be null if database could 
	not be validated as a true Notes Mail database.
RETURN: 
	eus_SUCCESS if no error occured
	ERR_DIRECTORY if the specified filename is a directory, not a database
	the Notes API error code otherwise

--- revision history -------
9/12/99 PR: created			*/
STATUS eus_OpenDb( char  pc_FILENM[], 
					char  pc_SERVERNM[], 
					DBHANDLE *const  ph)	{
	char  pc[ MAXPATH];
	DBHANDLE  h;
	WORD  us;
	STATUS  us_err;

	if (!( pc_FILENM && *pc_FILENM && pc_SERVERNM && ph))
		return !eus_SUCCESS;

	*ph = NULL;

	//open the specified database on the given server
	OSPathNetConstruct( NULL, pc_SERVERNM, pc_FILENM, pc);
	if (us_err = NSFDbOpen( pc, &h))
		return us_err;
	if (us_err = NSFDbModeGet( h, &us))
		goto errJump;
	if (us_err = (us != DB_LOADED ? ERR_DIRECTORY : eus_SUCCESS))
		goto errJump;

	*ph = h;
	return eus_SUCCESS;

errJump:
	NSFDbClose( h);

	return us_err;
} //eus_OpenDb(


/** eus_OpenTemplate( ***
Attempts to open the specified Notes template database on the given server. 
Validates that the opened database is a template.

--- parameters & return ----
pc_FILENM: pointer to the extended filename of the Notes template database to 
	be opened
pc_SERVERNM: pointer to the name of the server on which the specified Notes 
	template database resides
ph: Output. Pointer to the handle variable in which to store the handle to 
	the specified Notes template.  Guaranteed to be null if database could 
	not be validated as a true template database.
pc_class: Optional Output. Pointer to the buffer to fill with the class 
	(template) name of the specified database, should be of minimum size 
	NSF_INFO_SIZE to be completely safe. Caller may specify NULL for the 
	parameter if the output isn't needed.
RETURN: 
	eus_SUCCESS if no error occured
	ERR_DIRECTORY if the specified filename is a directory, not a database
	mi_ERR_NOT_TEMPLATE if the Notes database has no design class and 
		therefore is not considered a template
	the Notes API error code otherwise

--- revision history -------
9/12/99 PR: created			*/
STATUS eus_OpenTemplate( char  pc_FILENM[], 
							char  pc_SERVERNM[], 
							DBHANDLE *const  ph, 
							char *const  pc_class)	{
	STATUS  us_err;

	if (!( pc_FILENM && *pc_FILENM && pc_SERVERNM && ph))
		return !eus_SUCCESS;

	if (pc_class)
		*pc_class = NULL;

	if (us_err = eus_OpenDb( pc_FILENM, pc_SERVERNM, ph))
		return us_err;

	return eus_TestDbIsTemplate( *ph, pc_class);
} //eus_OpenTemplate(


/** eus_TestDbIsTemplate( ***
Determines whether a given, already opened Notes database is a template 
database by checking whether it has a class name (the only criterion I can 
identify as differentiating a template from a regular Notes database).

--- parameters & return ----
H: handle to the already opened Notes template database
pc_class: Optional Output. Pointer to the buffer to fill with the class 
	(template) name of the specified database, should be of minimum size 
	NSF_INFO_SIZE to be completely safe. Caller may specify NULL for the 
	parameter if the output isn't needed.
RETURN:
	eus_SUCCESS if no error occured
	mi_ERR_NOT_TEMPLATE if the Notes Mail database has no design class and 
		thus is not considered a template
	the Notes API error code otherwise

--- revision history -------
9/12/99 PR: created			*/
STATUS eus_TestDbIsTemplate( DBHANDLE  H, 
								char *const  pc_class)	{
	char  pc_cls[ NSF_INFO_SIZE];
	STATUS  us_err;

	if ( !H)
		return !eus_SUCCESS;

	if (pc_class)
		*pc_class = NULL;

	if (us_err = eus_getTemplateClass( H, pc_class ? pc_class : pc_cls))
		return us_err;

	return !*(pc_class ? pc_class : pc_cls) ? mi_ERR_NOT_TEMPLATE : 
																eus_SUCCESS;
} //eus_TestDbIsTemplate(


/** eus_ReplaceDesignNote( ***
Replaces the specified design-element note in a target database with its 
given counterpart in a source database. Any "Do Not Replace/Refresh" property 
on the design note in the target database is ignored. Upon replacement, any 
specified design-class property is set and the "Do Not Replace/Refresh" 
property is set to false.

--- parameters & return -----
h_DB_TARGET: handle to the target database
us_NOTE_CLASS: the class of the note to be removed from the target database 
	and, if the class is a unique-note class and the source NoteID was not 
	provided (nid_SRC), the class of the replacement note in the source 
	database
pc_NOTENM: Optional. Pointer to the name of the design-element note to be 
	removed from the target database. Required if the note class 
	(us_NOTE_CLASS) is not of the unique-note type. If the note class is of 
	the unique-note type, the parameter is ignored.
h_DB_SRC: handle to the source database
nid_SRC: Optional. NoteID of the replacement design-element. Required if the 
	note class is not of the unique-note type. If of the unique-note type and 
	not provided, procedure will look up the counterpart replacement note 
	using the note-class parameter.
pc_CLASSTITLE: Optional. For design element of non-unique type, pointer to 
	the design-class name to use if the caller wishes to set the design-class 
	property on the replacement design element. For icon notes, parameter 
	serves instead as a flag to indicate whether the $TITLE item on the icon 
	note to be replaced should replace the $TITLE item on the replacement 
	note. (The $TITLE item contains the database title, class name, 
	design-class name and database categories associated with the database.) 
	Parameter is ignored if the class specified in us_NOTE_CLASS is of a 
	different unique type. Pass NULL or the null-string if no design-class 
	name should be set.
pnid: Optional Output. Address of the variable in which to store the NoteID 
	of the replacement note in the target database after replacement has been 
	accomplished. If NULL on pass, output is suppressed.
RETURN:
	eus_SUCCESS if no error occured
	ERR_MEMORY if insufficient-memory condition is encountered
	the Notes API error code otherwise

--- suggested enhancement ---
9/12/99 PR: incorporate flag input for whether to honor the "Do Not 
	Replace/Refresh" property before carrying out the replacement

--- revision history --------
2/22/00 PR
+ minor documentation adjustment
+ to prevent "corrupted note" error message in Notes 4.5.1 client, added 
  OPEN_EXPAND flag when opening notes to be signed

9/12/99 PR
+ recast to accommodate replacement of any design element
+ completed standard documentation
+ moved into standard library

1/22/99 PR: created			*/
STATUS eus_ReplaceDesignNote( DBHANDLE  h_DB_TARGET, 
								const WORD  us_NOTE_CLASS, 
								char  pc_NOTENM[], 
								DBHANDLE  h_DB_SRC, 
								const NOTEID  nid_SRC, 
								char  pc_CLASSTITLE[], 
								NOTEID *const  pnid)	{
	const BOOL  f_SPECIAL_DESIGN_NOTE = !!(us_NOTE_CLASS & 
												NOTE_CLASS_SINGLE_INSTANCE);

	NOTEID  nid, nid_src;
	NOTEHANDLE  h;
	STATUS  us_err;

	if (! ( h_DB_TARGET && us_NOTE_CLASS && (!pc_NOTENM ? 
										f_SPECIAL_DESIGN_NOTE : TRUE) && 
										h_DB_SRC && h_DB_TARGET != 
										h_DB_SRC && (!f_SPECIAL_DESIGN_NOTE ? 
										nid_SRC : TRUE)))
		return !eus_SUCCESS;

	if (pnid)
		*pnid = NULL;

	//if this is a unique design note we're replacing and we don't yet 
	//	have the NoteID of the replacement note, get that NoteID
	if (!(nid_src = nid_SRC) && f_SPECIAL_DESIGN_NOTE)
		if (us_err = NSFDbGetSpecialNoteID( h_DB_SRC, 
													(WORD) (SPECIAL_ID_NOTE | 
													us_NOTE_CLASS), &nid_src))
			return us_err;

	//if we're replacing an icon note but the caller wishes to preserve the 
	//	current set of database information (e.g. its title)...
	if (NOTE_CLASS_ICON == us_NOTE_CLASS && pc_CLASSTITLE)	{
		const WORD US = (WORD) strlen( FIELD_TITLE);

		BLOCKID  bid;

		//open the icon note to be replaced
		if (us_err = NSFDbGetSpecialNoteID( h_DB_TARGET, 
												(WORD) (SPECIAL_ID_NOTE | 
												NOTE_CLASS_ICON), &nid))	{
			if (ERR_SPECIAL_ID != us_err)
				return us_err;
		}else	{
			if (us_err = NSFNoteOpen( h_DB_TARGET, nid, NULL, &h))
				return us_err;

			//if the $TITLE item for some reason isn't present...
			if (us_err = NSFItemInfo( h, FIELD_TITLE, US, &bid, NULL, NULL, 
																	NULL))	{
				if (ERR_ITEM_NOT_FOUND != us_err)
					goto errJump;

				//delete the icon note from the target database in 
				//	preparation for ensuing replacement
				NSFNoteClose( h);
				if (us_err = NSFNoteDelete( h_DB_TARGET, nid, NULL))
					return us_err;
			//else undertake preservation of the original $TITLE item...
			}else	{
				NOTEHANDLE  h_mem;

				//create a holder note in memory and place a copy of the item 
				//	in it
				if (us_err = NSFNoteCreate( h_DB_TARGET, &h_mem))
					goto errJump;
				if (us_err = NSFItemCopy( h_mem, bid))	{
					NSFNoteClose( h_mem);
					goto errJump;
				}

				//close the icon note to be replaced, then carry out the 
				//	replacement
				NSFNoteClose( h);
				if (us_err = NSFNoteDelete( h_DB_TARGET, nid, NULL))	{
					NSFNoteClose( h_mem);
					return us_err;
				}
				if (us_err = NSFDbCopyNote( h_DB_SRC, NULL, NULL, nid_src, 
														h_DB_TARGET, NULL, 
														NULL, &nid, NULL))	{
					NSFNoteClose( h_mem);
					return us_err;
				}

				//open the new icon note in the target database and remove 
				//	its $TITLE item
				if (us_err = NSFNoteOpen( h_DB_TARGET, nid, OPEN_EXPAND, 
																	&h))	{
					NSFNoteClose( h_mem);
					return us_err;
				}
				if (us_err = eus_RemoveItem( h, FIELD_TITLE, 
														ebid_NULLBLOCKID))	{
					NSFNoteClose( h_mem);
					goto errJump;
				}

				//copy the in-memory note's $TITLE item to the icon note, 
				//	then close the in-memory note
				if (us_err = NSFItemInfo( h_mem, FIELD_TITLE, US, &bid, NULL, 
															NULL, NULL))	{
					NSFNoteClose( h_mem);
					goto errJump;
				}
				us_err = NSFItemCopy( h, bid);
				NSFNoteClose( h_mem);
				if (us_err)
					goto errJump;

				//sign & save the new icon note
				if (us_err = NSFNoteSign( h))
					goto errJump;
				if (us_err = NSFNoteUpdate( h, NULL))
					goto errJump;
			} //if (us_err = NSFItemInfo( h, FIELD_TITLE,
		} //if (us_err = NSFDbGetSpecialNoteID( h_DB_TARGET,

		//if $TITLE preservation failed because the icon note or the item 
		//	itself wasn't present...
		if (us_err)	{
			//open, sign, save & close the icon note in the source database
			if (us_err = NSFNoteOpen( h_DB_SRC, nid_src, OPEN_EXPAND, &h))
				return us_err;
			if (us_err = NSFNoteSign( h))
				goto errJump;
			if (us_err = NSFNoteUpdate( h, NULL))
				goto errJump;
			NSFNoteClose( h);

			//copy the replacement note over to the target database
			if (us_err = NSFDbCopyNote( h_DB_SRC, NULL, NULL, nid_src, 
														h_DB_TARGET, NULL, 
														NULL, &nid, NULL))
				return us_err;
		} //if (us_err)
	//else follow a regular replacement process...
	}else	{
		//delete the design note from the target database
		if (us_err = eus_RemoveDesignNote( h_DB_TARGET, us_NOTE_CLASS, 
																pc_NOTENM))
			return us_err;

		//copy the note in the template to the target database
		if (us_err = NSFDbCopyNote( h_DB_SRC, NULL, NULL, nid_src, 
										h_DB_TARGET, NULL, NULL, &nid, NULL))
			return us_err;

		//if requested, change the inheritance name on the form to the class 
		//	name provided and ensure that the flag "Do not allow 
		//	Replace/Refresh to modify" is not set
		if (pc_CLASSTITLE && *pc_CLASSTITLE && !f_SPECIAL_DESIGN_NOTE)	{
			char * pc_flags;

			if (us_err = NSFNoteOpen( h_DB_TARGET, nid, NULL, &h))
				return us_err;

			if (us_err = eus_RemoveItem( h, DESIGN_CLASS, ebid_NULLBLOCKID))
				goto errJump;
			if (us_err = NSFItemSetText( h, DESIGN_CLASS, pc_CLASSTITLE, 
												(WORD) strlen( pc_CLASSTITLE)))
				goto errJump;

			if (us_err = eus_AttemptCopyTextItemContent( h, DESIGN_FLAGS, 
													NULL, &pc_flags, NULL))
				goto errJump;
			if (pc_flags)	{
				char * pc = strrchr( pc_flags, DESIGN_FLAG_PRESERVE);

				if (pc)	{
					if (us_err = eus_RemoveItem( h, DESIGN_FLAGS, 
														ebid_NULLBLOCKID))	{
						free( pc_flags);
						goto errJump;
					}
					memmove( pc, pc + 1, pc_flags + strlen( pc_flags) - pc + 
																		1);
					if (us_err = NSFItemSetText( h, DESIGN_FLAGS, pc_flags, 
												(WORD) strlen( pc_flags)))	{
						free( pc_flags);
						goto errJump;
					}
				} //if (pc)

				free( pc_flags);
			} //if (pc_flags)

			if (us_err = NSFNoteSign( h))
				goto errJump;
			us_err = NSFNoteUpdate( h, NULL);

		errJump:
			NSFNoteClose( h);
		} //if (pc_CLASSTITLE && *pc_CLASSTITLE)
	} //if (NOTE_CLASS_ICON == us_NOTE_CLASS &&

	if (pnid && !us_err)
		*pnid = nid;

	return us_err;
} //eus_ReplaceDesignNote(


