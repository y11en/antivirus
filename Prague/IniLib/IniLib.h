#ifndef _INILIB_H_
#define _INILIB_H_

#if defined( __cplusplus )
extern "C" {
	#ifndef DEF_VAL
	#define DEF_VAL(_v)	= _v
	#endif
#else
	#ifndef DEF_VAL
	#define DEF_VAL(_v)
	#endif
#endif

#include <Prague/prague.h>
#include <Prague/iface/i_io.h>

// ########################################################################

#define INI_SECT_NEWNAME   0x01
#define INI_SECT_HEADER    0x02
	
#define INI_LINE_NEWNAME   0x01
#define INI_LINE_NEWVAL    0x02

typedef struct tagCommentLine
{
	tCHAR*                 value;
	struct tagCommentLine* next;
} sCommentLine;
	
typedef struct tagIniLine
{
	tCHAR*                 name;
	tCHAR*                 value;
	struct tagIniLine*     next;
	
	// NOT ALLOCATED IF IniReadLoad USED !!!
	tBYTE				   flags;
	struct tagCommentLine* comment_head;
} sIniLine;

typedef struct tagIniSection
{
	tCHAR*                 name;
	sIniLine*              line_head;
	struct tagIniSection*  next;
	
	// NOT ALLOCATED IF IniReadLoad USED !!!
	tBYTE				   flags;
	tDWORD                 line_count;
	struct tagCommentLine* comment_head;
	tCHAR *                buffer;
} sIniSection;

typedef struct tagIni
{
	sIniSection*  head;
	sCommentLine* pre_comment_head;
	tDWORD        section_count;
	tBYTE         hdr[3];
} sIni;

#define INI_SAVE_SECCOMMENTSATBOTTOM   0x01
#define INI_SAVE_ACCURATELOOK          0x02
#define INI_SAVE_VOIDVALSNOASSIGN      0x04

typedef struct tagIniSaveParams
{
	tDWORD        flags;
	tDWORD        tab_spaces;
} sIniSaveParams;

typedef struct tagIniAllocator
{
#ifdef __cplusplus
	virtual tERROR Alloc(tPTR* result, tDWORD size) = 0;
	virtual tERROR Free(tPTR ptr) = 0;
#else
	tPTR        pVtbl;
#endif
} sIniAllocator;

#define INI_LOAD_SMALLALLOC      0x01
#define INI_LOAD_MULTILINEVALS   0x02

typedef tBOOL (*tIniLoadFilter)(tPTR ctx, tCHAR* name, tCHAR* value, tBOOL section, tBOOL comment);

// ########################################################################

#define      IniFileInit(ini) memset(ini, 0, sizeof(*ini))

tERROR       IniFileLoad(sIni* ini, hIO io, tDWORD nFlags DEF_VAL(0));
tERROR       IniFileCreateBuf(sIni* ini, hOBJECT obj, tCHAR** buffer, tDWORD* size, sIniSaveParams * prm);
tERROR       IniFileSave(sIni* ini, hIO io, sIniSaveParams * prm DEF_VAL(NULL));
tERROR       IniFileFree(sIni* ini);

tERROR       IniLoad(hIO io, sIniSection** section_head, sCommentLine** pre_comment_head);
tERROR       IniFree(sIniSection* section_head);
tERROR	     FreeSection(sIniSection* section);
tERROR	     FreeLine(sIniLine* line, sCommentLine** comment);
tERROR	     FreeComment(sCommentLine* comment);

tERROR       IniLoadEx(hIO io, sIniSection** section_head, sCommentLine** pre_comment_head DEF_VAL(NULL), tDWORD * section_count DEF_VAL(NULL), sIniAllocator * a DEF_VAL(NULL), tDWORD nFlags DEF_VAL(0), tBYTE * hdr DEF_VAL(NULL), tIniLoadFilter fltfunc DEF_VAL(NULL), tPTR fltctx DEF_VAL(NULL) );

// FOR IniReadLoad AND IniLoad !!!
sIniSection* IniGetSection(sIniSection* head, const tCHAR* section_name, sIniSection** prev DEF_VAL(NULL));
sIniLine*    IniGetLine(sIniSection* sec, const tCHAR* line_name, sIniLine** prev DEF_VAL(NULL));
tCHAR*       IniGetValue(sIniSection* head, const tCHAR* section_name, const tCHAR* line_name, tBOOL bSafe DEF_VAL(cFALSE));
tCHAR*       IniGetValueEx(sIniLine* line, tBOOL bSafe DEF_VAL(cFALSE));
tDWORD       IniParseValue(tCHAR** v,tDWORD v_size, tCHAR* value);

// ONLY FOR IniLoad !!!
sIniSection* IniCreateSection(sIni* ini, const tCHAR* section_name);
sIniSection* IniCreateSectionEx(sIni* ini, sIniSection* prv_sec DEF_VAL(NULL));
sIniLine*    IniCreateLine(sIniSection* sec, const tCHAR* line_name);
tERROR       IniSetValue(sIniLine* line, const tCHAR* value);
tERROR       IniDelLine(sIniSection* sec, const tCHAR* line_name);
tERROR       IniDelLineEx(sIniSection* sec, sIniLine* line, sIniLine* prv_line);
tERROR       IniDelSection(sIni* ini, const tCHAR* section_name);
tERROR       IniDelSectionEx(sIni* ini, sIniSection* sec, sIniSection* prv_sec);
tERROR       IniSetSectionName(sIniSection* sec, const tCHAR* section_name);
tERROR       IniSetLineName(sIniLine* line, const tCHAR* line_name);
sIniSection* IniMoveSection(sIni* ini, sIniSection* sec, sIniSection* prv_sec);

// ########################################################################

#if defined( __cplusplus )
} // extern "C"
#endif

#endif//_INILIB_H_
