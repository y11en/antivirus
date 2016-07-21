#include <windows.h>
#include "inilib.h"

DWORD IniParseLine(CHAR* ptr, CHAR** name, CHAR** value, BOOL* section, BOOL* comment )
{
	DWORD i=0;
	DWORD j=0;
	DWORD k=0;
	BOOL eol = FALSE;
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
				eol = FALSE;
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
			eol = TRUE;
			continue;
		}
		if(*ptr==0x0a) 
		{
			*ptr=0;
			k++;
			eol = FALSE;
			continue;
		}
		if (eol) 
		{
			k++;
			eol = FALSE;
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
				CHAR* p=ptr;
				for(;*p=='=' || *p==' ' || *p==0x09; p--)
					*p=0;
				*value=ptr+1;
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

HRESULT IniFree(sIniSection* section)
{
	HRESULT error=S_OK;
	while(section)
	{
		sIniSection* next=section->next;
		error=FreeSection(section);
		section=next;
	}
	return error;
}

HRESULT FreeSection(sIniSection* section)
{
	HRESULT error=S_OK;
	sCommentLine* comment=0;
	sIniLine* line=section->line_head;
	while(line)
	{
		sIniLine* line_next=line->next;
		error=FreeLine(line,0);
		line=line_next;
	}
	error=FreeComment(section->comment_head);
	if (section->flags & INI_SECT_NEWNAME) 
		free(section->name);

	if (section->buffer) 
		free(section->buffer);

	free(section);
	return error;
}

HRESULT FreeLine(sIniLine* line, sCommentLine** comment)
{
	HRESULT error=S_OK;
	if (comment)
		*comment=line->comment_head;
	else
		error=FreeComment(line->comment_head);
	
	if (line->b_newvalue) 
		free(line->value);
	if (line->b_newname) 
		free(line->name);
	free(line);
	return error;
}

HRESULT FreeComment(sCommentLine* comment)
{
	HRESULT error=S_OK;
	sCommentLine* comment_cur=comment;
	while (comment_cur)
	{
		sCommentLine* comment_next=comment_cur->next;
		free(comment_cur);
		comment_cur=comment_next;
	}
	return error;
}

HRESULT IniLoad(IStream * io, sIniSection** section_head, sCommentLine** pre_comment_head)
{
	HRESULT error = S_OK;
	DWORD size;
	CHAR* buffer;
	sIniSection* section_tail;

	if (!section_head)
		return E_INVALIDARG;

	section_tail=*section_head;
	while(section_tail && section_tail->next)
		section_tail = section_tail->next;

	{
		STATSTG _stg;
		if( FAILED(io->Stat(&_stg, STATFLAG_NONAME)) )
			_stg.cbSize.QuadPart = 0;
		size = _stg.cbSize.LowPart;
	}
	
	if(size)
	{
		size++;
		
		buffer = (CHAR*)malloc(size);
		if(buffer)
		{
			buffer[size - 1] = 0;
			if(SUCCEEDED(error=io->Read(buffer, size - 1, &size)))
			{
				CHAR* ptr=buffer;
				CHAR* name;
				CHAR* value;
				BOOL  b_section=0;
				BOOL  b_comment=0;
				sIniSection* sect=0;
				sIniLine* line_tail=0;
				sCommentLine* comment_tail=0;
				DWORD line_count=0;
				DWORD readed;
				while( readed=IniParseLine(ptr,&name,&value,&b_section,&b_comment) )
				{
					ptr+=readed;

					// If header of file is exist (in case regedit - REGEDIT4)
					if(!sect && !section_tail && name && !value && !b_section && !b_comment)
					{
						if(sect = (sIniSection *)calloc(1, sizeof(sIniSection)))
						{
							sect->name=name;
							sect->flags |= INI_SECT_HEADER;
							*section_head=sect;
							section_tail=sect;
						}
						else
							error = E_OUTOFMEMORY;
					
						continue;
					}
					
					if(b_section && name)
					{
						if(sect && line_count)
							sect->line_count=line_count;
						line_count=0;
						line_tail=0;
						comment_tail=0;
						
						if(sect = (sIniSection *)calloc(1, sizeof(sIniSection)))
						{
							sect->name=name;
							if(section_tail)
								section_tail->next=sect;
							else
								*section_head=sect;
							section_tail=sect;
						}
						else
							error = E_OUTOFMEMORY;
					
						continue;
					}
					
					if(b_comment && name)
					{
						sCommentLine* comment;
						if(comment = (sCommentLine *)calloc(1, sizeof(sCommentLine)))
						{
							comment->value=name;
							if (comment_tail)
								comment_tail->next=comment;
							else
							{
								if(line_tail)
									line_tail->comment_head=comment;
								else
								{
									if (sect) sect->comment_head=comment;
									else 
									{
										if (pre_comment_head)
											*pre_comment_head=comment;
									}
								}
							}
							comment_tail=comment;
						}
						else
							error = E_OUTOFMEMORY;
					
						continue;
					}

					if(sect && name)
					{
						sIniLine* line;

						if(line_tail)
						{
							int len = (int)strlen(line_tail->value);
							if(len && line_tail->value[len - 1]=='\\')
							{
								memmove(line_tail->value + len - 1, name, strlen(name) + 1);
								continue;
							}
						}
						
						comment_tail=0;
						if(line = (sIniLine *)calloc(1, sizeof(sIniLine)))
						{
							line_count++;
							line->name=name;
							if(!value)value="";
							line->value=value;
							if(line_tail)
								line_tail->next=line;
							else
								sect->line_head=line;
							line_tail=line;
						}
						else
							error = E_OUTOFMEMORY;
					
						continue;
					}
				}
				if(sect)
				{
					sect->buffer=buffer;
					sect->line_count=line_count;
				}
				else
					free(buffer);
			}
		}
		else
			error = E_OUTOFMEMORY;
	}

	if(FAILED(error) && *section_head)
		error=S_FALSE;
	
	return error;
}

HRESULT IniSave(IStream * io, sIniSection* section, sCommentLine* pre_comment)
{
	HRESULT			error=S_OK;
	sIniSection*	sec=0;
	sIniLine*		line=0;
	sCommentLine*	comment=0;
	
	if( !section )
		return E_INVALIDARG;

	sec=section;
	comment=pre_comment;
	while(comment)
	{
		io->Write(comment->value, strlen(comment->value), NULL);
		io->Write("\r\n", 2, NULL);
		
		char b = *comment->value;
		do comment=comment->next; while (0==b && comment && 0==*comment->value);
	}

	comment=0;
	while(sec)
	{
		if(sec->flags & INI_SECT_HEADER)
		{
			io->Write(sec->name, strlen(sec->name), NULL);
			io->Write("\r\n", 2, NULL);
		}
		else
		{
			io->Write("[", 1, NULL);
			io->Write(sec->name, strlen(sec->name), NULL);
			io->Write("]\r\n", 3, NULL);
		}

		comment=sec->comment_head;
		while (comment)
		{
			io->Write(comment->value, strlen(comment->value), NULL);
			io->Write("\r\n", 2, NULL);
			
			char b = *comment->value;
			do comment=comment->next; while (0==b && comment && 0==*comment->value);
		}
		
		line=sec->line_head;
		while(line)
		{
			io->Write(line->name, strlen(line->name), NULL);
			io->Write("=", 1, NULL);
			io->Write(line->value, strlen(line->value), NULL);
			io->Write("\r\n", 2, NULL);
			
			comment=line->comment_head;
			while (comment)
			{
				io->Write(comment->value, strlen(comment->value), NULL);
				io->Write("\r\n", 2, NULL);
				
				char b = *comment->value;
				do comment=comment->next; while (0==b && comment && 0==*comment->value);
			}
			
			line=line->next;
		}
		
		sec=sec->next;
	}

	return error;
}

sIniSection* IniGetSection( sIniSection* head, const CHAR* name)
{
	while(head)
	{
		if(!_stricmp(name,head->name))
			return head;
		head=head->next;
	}
	return NULL;
}
sIniLine* IniGetLine(sIniSection* sec, const CHAR* name)
{
	sIniLine* head=sec->line_head;
	while(head)
	{
		if(!_stricmp(name,head->name))
			return head;
		head=head->next;
	}
	return NULL;
}
CHAR* IniGetValue( sIniSection* head, const CHAR* section_name, const CHAR* line_name)
{
	sIniSection* section=head;
	while(section && (section=IniGetSection(section, section_name)) )
	{
		sIniLine* line=IniGetLine(section, line_name);
		if(line)
			return line->value;
		section=section->next;
	}
	return NULL;
}

DWORD IniParseValue( CHAR** v,DWORD v_size, CHAR* value)
{
	CHAR* first=0;
	CHAR* white=0;
	DWORD len=(DWORD)strlen(value)+1;
	DWORD i=0;
	for(;len;len--,value++)
	{
		switch(*value)
		{
		case 0:
		case ',':
		case ';':
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

HRESULT IniSetValue(sIniSection*& head, const CHAR* section_name, const CHAR* line_name, const CHAR* value)
{
	HRESULT error = S_OK;

	CHAR* name;
	BOOL  b_free=1;
	sIniLine* line=0;
	sIniSection* sec=IniGetSection(head,section_name);
	if (sec)
	{
		line=IniGetLine(sec,line_name);
		if (line)
		{
			if(name = (CHAR *)malloc(strlen(value)+1))
			{
				strcpy(name,value);
				if (line->b_newvalue)
					free(line->value);
				line->value=name;
				line->b_newvalue=1;
			}
			else
				error = E_OUTOFMEMORY;
		}
		else
		{
			line=sec->line_head;
			if (line) while (line->next) line=line->next;
			
			sIniLine* new_line=0;
			if(new_line = (sIniLine *)calloc(1, sizeof(sIniLine)))
			{
				sec->line_count++;
				if(name = (CHAR *)malloc(strlen(line_name)+1))
				{
					strcpy(name,line_name);
					new_line->name=name;
					new_line->b_newname=1;
					if(name = (CHAR *)malloc(strlen(value)+1))
					{
						strcpy(name,value);
						new_line->value=name;
						new_line->b_newvalue=1;
						if (line) line->next=new_line;
						else sec->line_head=new_line;
						b_free=0;
					}
					else
						error = E_OUTOFMEMORY;
				}
				else
					error = E_OUTOFMEMORY;

				if (b_free) FreeLine(new_line,0);
			}
			else
				error = E_OUTOFMEMORY;
		}
	}
	else
	{
		sec=head;
		if (sec) while (sec->next) sec=sec->next;
		
		sIniSection* new_sec=0;
		if(new_sec = (sIniSection *)calloc(1, sizeof(sIniSection)))
		{
			if(name = (CHAR *)malloc(strlen(section_name)+1))
			{
				strcpy(name,section_name);
				new_sec->name=name;
				new_sec->flags|=INI_SECT_NEWNAME;

				if(sec)
				{
					new_sec->buffer=sec->buffer;
					sec->buffer=NULL;
					sec->next=new_sec;
				}
				else
					head=new_sec;
				b_free=0;

				if(line_name)
				{
					if(line = (sIniLine *)calloc(1, sizeof(sIniLine)))
					{
						new_sec->line_head=line;
						new_sec->line_count=1;
						if(name = (CHAR *)malloc(strlen(line_name)+1))
						{
							strcpy(name,line_name);
							line->name=name;
							line->b_newname=1;
							if(value)
							{
								if(name = (CHAR *)malloc(strlen(value)+1))
								{
									strcpy(name,value);
									line->value=name;
									line->b_newvalue=1;
								}
								else
									error = E_OUTOFMEMORY;
							}
						}
						else
							error = E_OUTOFMEMORY;
					}
					else
						error = E_OUTOFMEMORY;
				}
			}
			else
				error = E_OUTOFMEMORY;
			
			if (b_free) FreeSection(new_sec);
		}
		else
			error = E_OUTOFMEMORY;
	}
	return error;
}
