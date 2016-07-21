#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h> 
#include <string.h>

#include "IniLib.h"

/////////////////////////////////////////////////////////////////////////////////////
// Read

tDWORD IniParseLine(tCHAR* ptr, tCHAR** name, tCHAR** value, tBOOL* section, tBOOL* comment)
{
	tDWORD i=0;
	tDWORD j=0;
	tDWORD k=0;
	tBOOL eol = cFALSE;
	*section=0;
	*name=0;
	*value=0;
	*comment=0;
	
	for(;*ptr;ptr++)
	{
		i++;
		if((*ptr==0x0d || *ptr==0x0a) && !eol)
		{
			*ptr=0;
			if(*(ptr+1)==0x0d || *(ptr+1)==0x0a) 
			{
				*(ptr+1)=0;
				i++;
				k++;
				eol = cFALSE;
//				continue;
			}
			if(*name)
				break;
			if ((j+k+1)-i==0)
			{
				*name=ptr-j;
				*comment=1;
				break;
			}
			eol = cTRUE;
			continue;
		}
		if(*ptr==0x0a) 
		{
			*ptr=0;
			k++;
			eol = cFALSE;
			continue;
		}
		if (eol) 
		{
			k++;
			eol = cFALSE;
		}
		if(*comment)
			continue;

		switch(*ptr)
		{
		case ';':
			if(/*i==0||(*/(j+k+1)-i==0)/*)*/
			{
				*name=ptr-j;
				*comment=1;
			}
			continue;
			
		case '=':
			if(*value)
				continue;
			if(*name)
			{
				tCHAR* p=ptr;
				for(;*p=='=' || *p==' ' || *p==0x09; p--)
					*p=0;
				
				p=ptr+1;
				while(*p==' ' || *p==0x09)
					p++;
				*value=p;
			}
			continue;
			
		case '[':
			if(*value)
				continue;
			if(*name)
				continue;
			*section=1;
			continue;
			
		case ']':
			if(*value)
				continue;
			if(*name)
				*ptr=0;
			continue;
			
		case ' ':
		case 0x09:
			j++;
			continue;

		default:
			if(*value)
				continue;
			if(*name)
				continue;
			*name=ptr;
			continue;
		}
	}
	return i;
}

tERROR IniFree(sIniSection* section_head)
{
	tERROR err = errOK;

	sIniSection* section = section_head;
	while( section )
	{
		sIniSection* next = section->next;
		err = FreeSection(section);
		section = next;
	}
	
	return err;
}

tERROR IniFileFree(sIni* ini)
{
	tERROR err = FreeComment(ini->pre_comment_head);
	if( PR_FAIL(err) )
		return err;

	return IniFree(ini->head);
}

tERROR FreeSection(sIniSection* section)
{
	tERROR error = errOK;
	sCommentLine* comment = NULL;
	sIniLine* line = section->line_head;
	while(line)
	{
		sIniLine* line_next = line->next;
		error = FreeLine(line, NULL);
		line = line_next;
	}
	error = FreeComment(section->comment_head);
//	comment=section->comment_head;
//	while (comment)
//	{
//		sCommentLine* comment_next=comment->next;
//		error=CALL_SYS_ObjHeapFree(g_root,comment);
//		comment=comment_next;
//	}
	if( section->flags & INI_SECT_NEWNAME )
		error = CALL_SYS_ObjHeapFree(g_root, section->name);

	if( section->buffer ) 
		error = CALL_SYS_ObjHeapFree(g_root, section->buffer);

	error = CALL_SYS_ObjHeapFree(g_root, section);
	return error;
}

tERROR FreeLine(sIniLine* line, sCommentLine** comment)
{
	tERROR error=errOK;
	if (comment) *comment=line->comment_head;
	else
	{
		error=FreeComment(line->comment_head);
//		sCommentLine* comment_cur=line->comment_head;
//		while (comment_cur)
//		{
//			sCommentLine* comment_next=comment_cur->next;
//			error=CALL_SYS_ObjHeapFree(g_root,comment_cur);
//			comment_cur=comment_next;
//		}
	}
	if (line->flags & INI_LINE_NEWVAL) 
		error=CALL_SYS_ObjHeapFree(g_root,line->value);
	if (line->flags & INI_LINE_NEWNAME) 
		error=CALL_SYS_ObjHeapFree(g_root,line->name);
	error=CALL_SYS_ObjHeapFree(g_root,line);
	return error;
}

tERROR FreeComment(sCommentLine* comment)
{
	tERROR error = errOK;
	sCommentLine* comment_cur = comment;
	while( comment_cur )
	{
		sCommentLine* comment_next = comment_cur->next;
		error = CALL_SYS_ObjHeapFree(g_root, comment_cur);
		comment_cur = comment_next;
	}
	
	return error;
}

struct _A : public sIniAllocator
{
	tERROR Alloc(tPTR* result, tDWORD size) { return CALL_SYS_ObjHeapAlloc(g_root, result, size); }
	tERROR Free(tPTR ptr) { return CALL_SYS_ObjHeapFree(g_root, ptr); }
} g_StdAllocator;

tERROR IniFileLoad(sIni* ini, hIO io, tDWORD nFlags)
{
	return IniLoadEx(io, &ini->head, &ini->pre_comment_head, &ini->section_count, &g_StdAllocator, nFlags, ini->hdr);
}

tERROR IniLoad(hIO io, sIniSection** section_head, sCommentLine** pre_comment_head)
{
	return IniLoadEx(io, section_head, pre_comment_head, NULL, &g_StdAllocator);
}

#define INILINE_SMALL_SIZE		offsetof(sIniLine, flags)
#define INILINE_FULL_SIZE		sizeof(sIniLine)
#define INISECTION_SMALL_SIZE	offsetof(sIniSection, flags)
#define INISECTION_FULL_SIZE	sizeof(sIniSection)

tERROR IniLoadEx(hIO io, sIniSection** section_head, sCommentLine** pre_comment_head, tDWORD * section_count, sIniAllocator * a, tDWORD nFlags, tBYTE * hdr, tIniLoadFilter fltfunc, tPTR fltctx)
{
	if( !section_head )
		return warnFALSE;

	tERROR err;
	tCHAR * buffer;
	
	{
		tQWORD qsize;
		if( PR_FAIL(err = CALL_IO_GetSize(io, &qsize, IO_SIZE_TYPE_EXPLICIT)) )
			return err;
		
		tDWORD size = (tDWORD)qsize;
		if( !size )
			return warnFALSE;

		tQWORD pos = 0;
		const int utf8_flag_size = 3;
		tBYTE c[3];
		if ( PR_SUCC(err = CALL_IO_SeekRead(io, &size, 0, c, utf8_flag_size)) )
		{
			if ( 
				size == utf8_flag_size &&
				c[0] == 0xEF &&
				c[1] == 0xBB &&
				c[2] == 0xBF
				)
			{
				pos += utf8_flag_size;
				if ( hdr )
				{
					hdr[0] = 0xEF;
					hdr[1] = 0xBB;
					hdr[2] = 0xBF;
				}
			}
		}

		size = (tDWORD)qsize - (tDWORD)pos;
		if( PR_FAIL(err = a->Alloc((tPTR *)&buffer, size + 1)) )
			return err;
		buffer[size] = 0;

		if( PR_FAIL(err = CALL_IO_SeekRead(io, &size, pos, buffer, size)) )
		{
			a->Free(buffer);
			return err;
		}
	}

	sIniSection * section_tail = *section_head;
	if( section_tail )
		while( section_tail->next )
			section_tail = section_tail->next;

	tCHAR*			ptr = buffer;
	tCHAR*			name;
	tCHAR*			value;
	tBOOL			b_section=0;
	tBOOL			b_comment=0;
	sIniSection*	sect=0;
	sIniLine*		line_tail=0;
	sCommentLine*	comment_tail=0;
	tDWORD			line_count=0;
	tDWORD			readed;
	
	while( readed = IniParseLine(ptr, &name, &value, &b_section, &b_comment) )
	{
		ptr += readed;

        if (fltfunc && !fltfunc(fltctx, name, value, b_section, b_comment))
        {
            // skip lines filtered out by external filter function
            continue;
        }

		// If header of file is exist (in case regedit - REGEDIT4)
		if( !sect && !section_tail && name && !value && !b_section && !b_comment )
		{
			if( !(nFlags & INI_LOAD_SMALLALLOC) && PR_SUCC(err = a->Alloc((tPTR *)&sect, sizeof(sIniSection))) )
			{
				sect->name = name;
				sect->flags |= INI_SECT_HEADER;
				*section_head = sect;
				section_tail = sect;
			}
		
			continue;
		}
		
		// Section
		if( b_section && name )
		{
			if( !(nFlags & INI_LOAD_SMALLALLOC) && sect && line_count )
				sect->line_count = line_count;
			
			line_count = 0;
			line_tail = 0;
			comment_tail = 0;
			
			if( PR_SUCC(err = a->Alloc((tPTR *)&sect, (nFlags & INI_LOAD_SMALLALLOC) ? INISECTION_SMALL_SIZE : INISECTION_FULL_SIZE)) )
			{
				sect->name = name;
				if( section_tail )
					section_tail->next = sect;
				else
					*section_head = sect;
				section_tail = sect;

				if( section_count )
					(*section_count)++;
			}
		
			continue;
		}
		
		// Comment
		if( b_comment && name )
		{
			sCommentLine* comment;
			if( !(nFlags & INI_LOAD_SMALLALLOC) && PR_SUCC(err = a->Alloc((tPTR *)&comment, sizeof(sCommentLine))) )
			{
				comment->value = name;
				if( comment_tail )
					comment_tail->next = comment;
				else
				{
					if( line_tail )
						line_tail->comment_head = comment;
					else
					{
						if( sect )
							sect->comment_head = comment;
						else 
						{
							if( pre_comment_head )
								*pre_comment_head = comment;
                            else
                                a->Free(comment), comment = NULL;
						}
					}
				}
				
				comment_tail = comment;
			}
		
			continue;
		}

		// Line
		if( sect && name )
		{
			sIniLine* line;

			if( (nFlags & INI_LOAD_MULTILINEVALS) && line_tail )
			{
				int len = (int)strlen(line_tail->value);
				if( len && line_tail->value[len - 1] == '\\' )
				{
					int len_name = strlen(name);
					memmove(line_tail->value + len - 1, name, len_name + 1);
					if( value && *value )
					{
						// В строке точно был '=', поэтому присваиваем вручную, буфера хватит
						
						len += len_name;
						line_tail->value[len - 1] = '=';
						memmove(line_tail->value + len, value, strlen(value) + 1);
					}
					continue;
				}
			}
			
			comment_tail = 0;
			if( PR_SUCC(err = a->Alloc((tPTR *)&line, (nFlags & INI_LOAD_SMALLALLOC) ? INILINE_SMALL_SIZE : INILINE_FULL_SIZE)) )
			{
				line_count++;
				line->name = name;
				if( !value )
					value = "";
				line->value = value;
				
				if( line_tail )
					line_tail->next = line;
				else
					sect->line_head = line;
				line_tail = line;
			}
		
			continue;
		}
	}
	
	if( sect )
	{
		if( !(nFlags & INI_LOAD_SMALLALLOC) )
		{
			sect->buffer = buffer;
			sect->line_count = line_count;
		}
	}
	else
		a->Free(buffer);

	if( PR_FAIL(err) && *section_head )
		err = warnFALSE;
	
	return err;
}

#define _LCASE(c) (c >= 'A' ? (c <= 'Z' ? c|0x20 : c) : c)

int fast_stricmp(const char * _Str1, const char * _Str2)
{
	unsigned char c1, c2;
	for (;;_Str1++, _Str2++)
	{
		c1 = *_Str1;
		c2 = *_Str2;
		if (c1 == 0 || c2 == 0)
			break;
		if (c1 == c2)
			continue;
		c1 = _LCASE(c1);
		c2 = _LCASE(c2);
		if (c1 == c2)
			continue;
		if (c1 >= 0x80 || c2 >= 0x80)
			return _stricmp(_Str1, _Str2);
		break;
	}
	if (c1 < c2)
		return -1;
	if (c1 > c2)
		return 1;
	return 0;
}

sIniSection* IniGetSection(sIniSection* head, const tCHAR* name, sIniSection** prev)
{
	sIniSection * p = NULL;
	while( head )
	{
		if( !fast_stricmp(name, head->name) )
		{
			if( prev )
				*prev = p;
			return head;
		}
		
		p = head;
		head = head->next;
	}
	
	if( prev )
		*prev = NULL;
	return NULL;
}

sIniLine* IniGetLine(sIniSection* sec, const tCHAR* name, sIniLine** prev)
{
	if( !sec )
		return NULL;
	
	sIniLine* head = sec->line_head;
	sIniLine * p = NULL;
	while( head )
	{
		if( !fast_stricmp(name, head->name) )
		{
			if( prev )
				*prev = p;
			return head;
		}

		p = head;
		head = head->next;
	}
	
	if( prev )
		*prev = NULL;
	return NULL;
}

static tCHAR empty[] = "";

tCHAR* IniGetValue(sIniSection* head, const tCHAR* section_name, const tCHAR* line_name, tBOOL bSafe)
{
	sIniSection* section = head;
	while( section && (section = IniGetSection(section, section_name)) )
	{
		sIniLine* line = IniGetLine(section, line_name);
		if( line )
			return IniGetValueEx(line, bSafe);
		section = section->next;
	}
	return bSafe ? empty : NULL;
}

tCHAR* IniGetValueEx(sIniLine* line, tBOOL bSafe)
{
	tCHAR* v = line ? line->value : NULL;
	return v ? v : (bSafe ? empty : NULL);
}

tDWORD IniParseValue(tCHAR** v, tDWORD v_size, tCHAR* value)
{
	tCHAR* first=0;
	tCHAR* white=0;
	tDWORD len=(tDWORD)strlen(value)+1;
	tDWORD i=0;
	for(;len;len--,value++)
	{
		switch(*value)
		{
		case 0:
		case ',':
		case ';':
//			*value=0;
			v[i++]=first;
			if(i==v_size)
			{
				if(white)
					*white=0;
				return i;
			}
			first=0;
			break;

		case 0x09:
		case ' ':
			if(first && !white)
				white=value;
			break;
		default:
			if(!first)
				first=value;
			white=0;
			break;
		}
	}
	return i;
}

// Modify

tERROR IniFileSave(sIni* ini, hIO io, sIniSaveParams * prm)
{
	tERROR err = errOK;
	tCHAR* buffer=NULL;
	tDWORD size=0;
	tDWORD w_size=0;
	
	if( PR_FAIL(err = IniFileCreateBuf(ini, (hOBJECT)io, &buffer, &size, prm)) )
		return err;

	tQWORD pos = 0;
	if ( ini->hdr[0] != 0x00 )
	{
		if ( PR_SUCC(err = CALL_IO_SeekWrite(io, &w_size, 0, buffer, size)) )
			pos = 3;
		else
			return err = errNOT_ENOUGH_MEMORY;
	}

	if( PR_SUCC(err = CALL_IO_SeekWrite(io, &w_size, pos, buffer, size)) )
	{
		if( w_size != size )
			err = errBAD_SIZE;
		else
			err = CALL_IO_SetSize(io, size);
	}
	
	CALL_SYS_ObjHeapFree(g_root, buffer);
	return err;
}

struct _wstb_ctx
{
	tCHAR * buf;
	tDWORD  size;
};

static _wstb_ctx& _wstb(_wstb_ctx& _ctx, const tCHAR * str, tDWORD n = -1)
{
	if( n == -1 )
		n = strlen(str);
	
	if( _ctx.buf )
	{
		memcpy(_ctx.buf, str, n);
		_ctx.buf += n;
	}
	
	_ctx.size += n;
	return _ctx;
}

static _wstb_ctx& _wctb(_wstb_ctx& _ctx, tCHAR chr, tDWORD n = 1)
{
	if( _ctx.buf )
	{
		memset(_ctx.buf, chr, n);
		_ctx.buf += n;
	}
	
	_ctx.size += n;
	return _ctx;
}

void __IniFile_WriteSecLines(_wstb_ctx& _ctx, sIniSection*& sec, sIniSaveParams * prm)
{
	bool bAccurate = prm && (prm->flags & INI_SAVE_ACCURATELOOK);
	bool bVoidValsNoAssign = prm && (prm->flags & INI_SAVE_VOIDVALSNOASSIGN);
	
	tDWORD nLineNameAligment = 0;
	if( bAccurate )
	{
		for(sIniLine* line = sec->line_head; line; line = line->next)
		{
			tDWORD len = strlen(line->name) + 1;
			if( len > nLineNameAligment )
				nLineNameAligment = len;
		}

		if( prm->tab_spaces && sec->line_head && sec->line_head->next )
		{
			tDWORD sp = nLineNameAligment % prm->tab_spaces;
			if( sp )
				nLineNameAligment += prm->tab_spaces - sp;
		}
	}

	sIniLine* line = sec->line_head;
	while( line )
	{
		tDWORD len = strlen(line->name);

		_wstb(_ctx, line->name, len);
		if( *line->value || !bVoidValsNoAssign )
		{
			if( bAccurate )
				_wctb(_ctx, ' ', nLineNameAligment - len);
			_wstb(_wstb(_ctx, bAccurate ? "= " : "=", bAccurate ? 2 : 1), line->value);
		}
		_wstb(_ctx, "\r\n", 2);
		
		sCommentLine* comment = line->comment_head;
		while( comment )
		{
			if( comment->next || *comment->value )
				_wstb(_wstb(_ctx, comment->value), "\r\n", 2);
			
			comment = comment->next;
		}

		line = line->next;
	}
}

void __IniFile_WriteSecComments(_wstb_ctx& _ctx, sIniSection*& sec, sIniSaveParams * prm)
{
	sCommentLine * comment = sec->comment_head;
	while( comment )
	{
		if( comment->next || *comment->value )
			_wstb(_wstb(_ctx, comment->value), "\r\n", 2);
		
		comment = comment->next;
	}
}

void __IniFile_Write(_wstb_ctx& _ctx, sIniSection* section, sCommentLine* pre_comment, sIniSaveParams * prm)
{
	sCommentLine* comment = pre_comment;
	while(comment)
	{
		_wstb(_wstb(_ctx, comment->value), "\r\n", 2);
		comment = comment->next;
	}
	
	sIniSection* sec = section;
	while( sec )
	{
		if( sec->flags & INI_SECT_HEADER )
			_wstb(_wstb(_ctx, sec->name), "\r\n", 2);
		else
			_wstb(_wstb(_wstb(_ctx, "["), sec->name), "]\r\n", 3);
		
		if( prm && prm->flags & INI_SAVE_SECCOMMENTSATBOTTOM )
		{
			__IniFile_WriteSecLines(_ctx, sec, prm);
			__IniFile_WriteSecComments(_ctx, sec, prm);
		}
		else
		{
			__IniFile_WriteSecComments(_ctx, sec, prm);
			__IniFile_WriteSecLines(_ctx, sec, prm);
		}
		
		sec = sec->next;

		if( sec )
			_wstb(_ctx, "\r\n", 2);
	}
}

tERROR IniFileCreateBuf(sIni* ini, hOBJECT obj, tCHAR** buffer, tDWORD* size, sIniSaveParams * prm)
{
	sIniSection* section = ini->head;
	sCommentLine* pre_comment = ini->pre_comment_head;

	if( !buffer || !size )
		return errPARAMETER_INVALID;

	tBOOL      b_write = 0;
	_wstb_ctx  _ctx;
	
	for(;;)
	{
		_ctx.buf = NULL;
		if( b_write )
		{
			tERROR error = CALL_SYS_ObjHeapAlloc(obj, (tPTR *)buffer, _ctx.size);
			if( PR_FAIL(error) )
				return error;
			
			_ctx.buf = *buffer;
		}
		_ctx.size = 0;

		__IniFile_Write(_ctx, section, pre_comment, prm);
		
		if( b_write )
			break;
		b_write = 1;
	}
	
	*size = _ctx.size;
	return errOK;
}

sIniSection* IniCreateSection(sIni* ini, const tCHAR* section_name)
{
	tERROR err = errOK;
	
	if( !ini || !section_name )
		return NULL;
	
	sIniSection* sec = IniGetSection(ini->head, section_name);
	if( sec )
		return sec;
	
	sec = ini->head;
	if( sec )
		while( sec->next )
			sec = sec->next;

	sIniSection* new_sec = IniCreateSectionEx(ini, sec);
	if( !new_sec )
		return NULL;

	if( PR_FAIL(err = IniSetSectionName(new_sec, section_name)) )
	{
		IniDelSectionEx(ini, new_sec, sec);
		return NULL;
	}
	
	return new_sec;
}

sIniSection* IniCreateSectionEx(sIni* ini, sIniSection* prv_sec)
{
	if( !ini )
		return NULL;

	tERROR err = errOK;

	sIniSection* new_sec = 0;
	if( PR_FAIL(err = CALL_SYS_ObjHeapAlloc(g_root, (tPTR*)&new_sec, sizeof(sIniSection))) )
		return NULL;
	
	new_sec->name = "";
	ini->section_count++;

	if( prv_sec )
	{
		new_sec->buffer = prv_sec->buffer;
		prv_sec->buffer = NULL;
		new_sec->next = prv_sec->next;
		prv_sec->next = new_sec;
	}
	else
		ini->head = new_sec;

	return new_sec;
}

sIniLine* IniCreateLine(sIniSection* sec, const tCHAR* line_name)
{
	if( !sec || !line_name )
		return NULL;
	
	sIniLine* line = IniGetLine(sec, line_name);
	if( line )
		return line;
	
	tERROR error = errOK;

	sIniLine* new_line=0;
	if( PR_FAIL(error = CALL_SYS_ObjHeapAlloc(g_root, (tPTR*)&new_line, sizeof(sIniLine))) )
		return NULL;
	
	tCHAR* name; tSIZE_T nameSz = strlen(line_name) + 1;
	if( PR_FAIL(error = CALL_SYS_ObjHeapAlloc(g_root, (tPTR*)&name, nameSz)))
	{
		CALL_SYS_ObjHeapFree(g_root, new_line);
		return NULL;
	}

	line = sec->line_head;
	if( line )
		while( line->next )
			line = line->next;

	sec->line_count++;

	strcpy_s(name, nameSz, line_name);
	new_line->name = name;
	new_line->flags |= INI_LINE_NEWNAME;
	new_line->value = "";
	
	if( line )
		line->next = new_line;
	else
		sec->line_head = new_line;

	return new_line;
}

tERROR IniSetValue(sIniLine* line, const tCHAR* value)
{
	if( !line || !value )
		return warnFALSE;
	
	tERROR error = errOK;

	tCHAR* name; tSIZE_T nameSz = strlen(value) + 1;
	if( PR_FAIL(error = CALL_SYS_ObjHeapAlloc(g_root, (tPTR*)&name, nameSz)) )
		return error;

	strcpy_s(name, nameSz, value);
	if( line->flags & INI_LINE_NEWVAL )
		error = CALL_SYS_ObjHeapFree(g_root, line->value);
	line->value = name;
	line->flags |= INI_LINE_NEWVAL;
	return error;
}

tERROR IniDelLine(sIniSection* sec, const tCHAR* line_name)
{
	if( !sec || !line_name )
		return warnFALSE;
	
	sIniLine* prv_line=0;
	sIniLine* line = IniGetLine(sec, line_name, &prv_line);
	if( !line )
		return warnFALSE;

	IniDelLineEx(sec, line, prv_line);
	return errOK;
}

tERROR IniDelLineEx(sIniSection* sec, sIniLine* line, sIniLine* prv_line)
{
	tERROR error = errOK;

	sCommentLine* comment=0;
	if( prv_line )
		prv_line->next = line->next;
	else
		sec->line_head = line->next;
	
	sec->line_count--;
	error = FreeLine(line, &comment);
	
	if( prv_line )
	{
		if( !prv_line->comment_head )
			prv_line->comment_head = comment;
		else
		{
			sCommentLine* next = prv_line->comment_head;
			while( next->next )
				next = next->next;
			next->next = comment;
		}
	}
	else
	{
		if( !sec->comment_head )
			sec->comment_head = comment;
		else
		{
			sCommentLine* next = sec->comment_head;
			while( next->next )
				next = next->next;
			next->next = comment;
		}
	}

	return error;
}

tERROR IniDelSection(sIni* ini, const tCHAR* section_name)
{
	sIniSection* prv_sec = 0;
	sIniSection* sec = IniGetSection(ini->head, section_name, &prv_sec);
	if( !sec )
		return warnFALSE;
	
	return IniDelSectionEx(ini, sec, prv_sec);
}

tERROR IniDelSectionEx(sIni* ini, sIniSection* sec, sIniSection* prv_sec)
{
	tERROR error = errOK;

	if( !prv_sec )
		ini->head = sec->next;
	else
	{
		prv_sec->next = sec->next;
		if( sec->buffer && !prv_sec->buffer )
			prv_sec->buffer = sec->buffer, sec->buffer = NULL;
	}
	
	if( PR_SUCC(error = FreeSection(sec)) )
		ini->section_count--;

	return error;
}

tERROR IniSetSectionName(sIniSection* sec, const tCHAR* name)
{
	if( !sec || !name || !*name )
		return warnFALSE;
	
	tDWORD n = strlen(name) + 1;
	
	tERROR err;
	if( sec->flags & INI_SECT_NEWNAME )
		err = CALL_SYS_ObjHeapRealloc(g_root, (tPTR*)&sec->name, (tPTR)sec->name, n);
	else
	{
		sec->name = NULL;
		err = CALL_SYS_ObjHeapAlloc(g_root, (tPTR*)&sec->name, n);
		if( sec->name )
			sec->flags |= INI_SECT_NEWNAME;
	}

	if( !sec->name )
		sec->name = "";
	else
		memcpy(sec->name, name, n);

	return err;
}

tERROR IniSetLineName(sIniLine* line, const tCHAR* name)
{
	if( !line || !name || !*name )
		return warnFALSE;
	
	tDWORD n = strlen(name) + 1;
	
	tERROR err;
	if( line->flags & INI_LINE_NEWNAME )
		err = CALL_SYS_ObjHeapRealloc(g_root, (tPTR*)&line->name, (tPTR)line->name, n);
	else
	{
		line->name = NULL;
		err = CALL_SYS_ObjHeapAlloc(g_root, (tPTR*)&line->name, n);
		if( line->name )
			line->flags |= INI_LINE_NEWNAME;
	}

	if( !line->name )
		line->name = "";
	else
		memcpy(line->name, name, n);

	return err;
}

sIniSection* IniMoveSection(sIni* ini, sIniSection* sec, sIniSection* prv_sec)
{
	if( !sec )
		return NULL;

	if( prv_sec )
	{
		if( prv_sec->next == sec )
			return sec;
	}
	else
	{
		if( ini->head == sec )
			return sec;
	}

	sIniSection* new_sec = IniCreateSectionEx(ini, prv_sec);
	if( !new_sec )
		return NULL;

	if( PR_FAIL(IniSetSectionName(new_sec, sec->name)) )
	{
		IniDelSectionEx(ini, new_sec, prv_sec);
		return NULL;
	}

	sIniLine* line = sec->line_head;
	while(line)
	{
		sIniLine* new_line = IniCreateLine(new_sec, line->name);
		if( !new_line || PR_FAIL(IniSetValue(new_line, line->value)) )
		{
			IniDelSectionEx(ini, new_sec, prv_sec);
			return NULL;
		}

		sIniLine* line_next = line->next;
		IniDelLineEx(sec, line, NULL);
		line = line_next;
	}

	if( sec == ini->head )
		prv_sec = NULL;
	else
	{
		for(prv_sec = ini->head; prv_sec; prv_sec = prv_sec->next)
			if( prv_sec->next == sec )
				break;
	}
	
	IniDelSectionEx(ini, sec, prv_sec);
	return new_sec;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
