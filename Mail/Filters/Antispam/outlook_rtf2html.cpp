// outlook_rtf2html.cpp : Defines the entry point for the console application.
//

#include "file_al/file_al.h"

#include <Prague/prague.h>
#include <Prague/iface/i_io.h>

#include <windows.h>
#include <stdio.h>

enum enRTF2HTML_State
{
	STATE_UNKNOWN,
	STATE_RTF_TAG,
};

class cCachedWrite 
{
	unsigned char out_buff[0x100];
	size_t out_size;
	cFile* pFile;
public:
	cCachedWrite(cFile* _pFile)
	{
		pFile = _pFile;
		out_size = 0;
#ifdef _DEBUG
		memset(out_buff,0,sizeof(out_buff));
#endif
	}
	~cCachedWrite()
	{
		if (pFile)
		{
			Flush();
		}
	}
	bool Write(unsigned char b)
	{
		out_buff[out_size++] = b;
		if (out_size == sizeof(out_buff))
			return Flush();
		return true;
	}
	bool Write(void* pData, size_t nSize, size_t* pnBytesWritten)
	{
		Flush();
		return pFile->Write(pData, nSize, pnBytesWritten);
	}
	bool Flush()
	{
		if (out_size)
		{
			pFile->Write(out_buff, out_size, NULL);
			out_size = 0;
#ifdef _DEBUG
			memset(out_buff,0,sizeof(out_buff));
#endif
		}
		return pFile->Flush();
	}
};

class cCachedRead
{
	unsigned char in_buff[0x100];
#ifdef _DEBUG
	unsigned char* read_ptr;
#endif
	size_t in_size;
	size_t in_ptr;
	cFile* pFile;
public:
	cCachedRead(cFile* _pFile)
	{
		pFile = _pFile;
		in_size = 0;
		in_ptr = 0;
#ifdef _DEBUG
		read_ptr = in_buff;
#endif
	}
	~cCachedRead()
	{
	}
	bool Read(unsigned char* pb)
	{
		if (in_ptr == in_size)
		{
			in_ptr = 0;
			if (!pFile->Read(in_buff, sizeof(in_buff), &in_size))
				return false;
		}
		if (in_size == 0)
			return false;
#ifdef _DEBUG
		read_ptr = in_buff + in_ptr;
#endif
		*pb = in_buff[in_ptr++];
		return true;
	}
};

class cTag {
	char buff[0x40];
	size_t m_size;
public:
	cTag()
	{
		clean();
	}
	operator const char*()
	{
		buff[m_size] = 0;
		return (const char*)buff;
	}
	void append(char b)
	{
		if (m_size < sizeof(buff)-1)
			buff[m_size++] = b;
	}
	size_t size()
	{
		return m_size;
	}
	void clean()
	{
		m_size = 0;
	}
	
};

char hex2char(const char* hex)
{
	char b;
	b =  (hex[0] >= 'a' ? hex[0]-'a'+10 : hex[0]-'0')*16;
	b += (hex[1] >= 'a' ? hex[1]-'a'+10 : hex[1]-'0');
	return b;
}

inline bool _isspace(char b)
{
	if (b<=' ')
		return true;
	return false;
}

inline bool _isalpha(char b)
{
	if (b>='A' && b<='Z')
		return true;
	if (b>='a' && b<='z')
		return true;
	return false;
}

inline bool _isdigit(char b)
{
	if (b>='0' && b<='9')
		return true;
	return false;
}

inline bool _isdelim(char b)
{
	if (_isalpha(b))
		return false;
	if (_isdigit(b))
		return false;
	return true;
}

bool ConvertOutlookRtf2Html(cFile* pRtf, cFile* pHtml)
{
	int nesting_lev;
	enRTF2HTML_State state = STATE_UNKNOWN;
	cCachedWrite output(pHtml);
	cCachedRead input(pRtf);
	cTag rtf_tag;
	unsigned char b;
	unsigned char repeat_byte;
	unsigned char last_b = 0x0a;
	bool htmlrtf = false;
	
	while(input.Read(&b))
	{
		do {
			repeat_byte = 0;
			switch (state)
			{
			case STATE_UNKNOWN:
				switch (b) {
				case '\\': 
					state = STATE_RTF_TAG; 
				case '{':
				case '}':
					break;
				default:
					if (htmlrtf == false)
					{
						if (b < ' ')
							break;
						output.Write(b);
						last_b = b;
					}
				}
				break;
			case STATE_RTF_TAG:
				if (!_isdelim(b) || b == '-')
				{
					rtf_tag.append(b);
					break;
				}
				if (rtf_tag.size() == 0)
				{
					state = STATE_UNKNOWN;
					if (b == '\'')
					{
						input.Read(&b);
						rtf_tag.append(b);
						input.Read(&b);
						rtf_tag.append(b);
						repeat_byte = hex2char(rtf_tag);
						rtf_tag.clean();
					}
					else if (b == '}' || b == '{')
						repeat_byte = b;
					break;
				}
				//if(0)
				if (b == '{')
				{
					nesting_lev=1;
					while (input.Read(&b))
					{
						if (b == '{')
							nesting_lev++;
						else if (b == '}')
						{
							nesting_lev--;
							if (nesting_lev == 0)
								break;
						}
					}
				}
				else
				{
					if (b!=' ' && b!=';')
						repeat_byte = b;
				}

				if (0 == strncmp(rtf_tag, "htmlrtf", 7))
				{
					if (rtf_tag[7] == '0') // "htmlrtf0"
						htmlrtf = false;
					else
						htmlrtf = true;
				} 
				else if (0 == strcmp(rtf_tag, "par"))
				{
					if (last_b != 0x0a)
						output.Write(0x0a);
					last_b = 0x0a;
				}
				else if (0 == strcmp(rtf_tag, "tab"))
				{
					repeat_byte = 0x09;
				}
				
				rtf_tag.clean();
				state = STATE_UNKNOWN;
				break;
			default:
				// unknown state?
				break;
			}
			b = repeat_byte;
		} while (b);
	} 
	return true;
}

//int main(int argc, char* argv[])
//{
//	if (argc < 3)
//	{
//		printf("Usage: %s <in_file_rtf> <out_file_html>\n", argv[0]);
//		return 1;
//	}
//	cFile input(argv[1], "rb");
//	if (!input.IsInitialized())
//	{
//		printf("Cannot init source RTF file '%s'\n", argv[1]);
//		return 2;
//	}
//	cFile output(argv[2], "wb");
//	if (!output.IsInitialized())
//	{
//		printf("Cannot init destination HTML file '%s'\n", argv[2]);
//		return 3;
//	}
//
//	ConvertOutlookRtf2Html(&input, &output);
//
//	input.Close();
//	output.Close();
//	
//	return 0;
//}

int ConvertOutlookRtf2Html(hIO in, hIO out)
{
	cFile sSrc(in), sDst(out);

	ConvertOutlookRtf2Html(&sSrc, &sDst);

	sSrc.Close();
	sDst.Close();

	return 0;
}
