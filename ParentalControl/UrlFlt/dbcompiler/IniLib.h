#ifndef _INILIB_H_
#define _INILIB_H_

#define INI_SECT_NEWNAME   0x01
#define INI_SECT_HEADER    0x02
	
struct sCommentLine
{
	CHAR*			value;
	sCommentLine*	next;
};
	
struct sIniLine
{
	CHAR*			name;
	CHAR*			value;
	BOOL			b_newname;
	BOOL			b_newvalue;
	sIniLine*		next;
	sCommentLine*	comment_head;
};

struct sIniSection
{
	CHAR*           name;
	BYTE			flags;
	DWORD           line_count;
	sIniLine*       line_head;
	CHAR*           buffer;
	sIniSection*	next;
	sCommentLine*	comment_head;
};

HRESULT		IniLoad(IStream * io, sIniSection** section_head, sCommentLine** pre_comment_head);
HRESULT		IniSave(IStream * io, sIniSection* section_head, sCommentLine* pre_comment_head);
HRESULT		IniFree(sIniSection* section);
HRESULT		FreeSection(sIniSection* section);
HRESULT		FreeLine(sIniLine* line, sCommentLine** comment);
HRESULT		FreeComment(sCommentLine* comment);

CHAR*		IniGetValue(sIniSection* head, const CHAR* section_name, const CHAR* line_name);
HRESULT		IniSetValue(sIniSection*& head, const CHAR* section_name, const CHAR* line_name, const CHAR* value);
DWORD		IniParseValue( CHAR** v,DWORD v_size, CHAR* value);

sIniSection*
			IniGetSection( sIniSection* head, const CHAR* section_name);
sIniLine*	IniGetLine( sIniSection* sec, const CHAR* line_name);

#endif//_INILIB_H_
