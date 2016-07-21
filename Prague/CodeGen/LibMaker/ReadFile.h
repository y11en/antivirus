#include "stdafx.h"

#ifndef __C_READ_FILE
#define __C_READ_FILE

class CReadFile
{
	HANDLE m_file;
	DWORD  m_flen;
	DWORD  m_fpos;
	char*  m_buff;
	DWORD  m_bpos;
	DWORD  m_blen;
	static const m_bsize;
public:
	CReadFile(HANDLE file);
	~CReadFile();
	bool readln(char*& line, int& len);
	operator bool()
	{
		return (m_file != 0) && (m_file != INVALID_HANDLE_VALUE) && (m_flen>0);
	}
private:
	bool update();
};

struct CErrorLine
{
	const char* m_path;
	const char* m_file;
	DWORD       m_line;
	bool        m_add;
	bool        m_is_err;
	const char* m_str_id;
	char*       m_msg;
	
public:
	CErrorLine(char*& src, const char* ignore_path, const char* ignore_f_name);
	~CErrorLine();
};

struct CId
{
	const char* m_method_name;
	DWORD       m_line;
	DWORD       m_cls;
	DWORD       m_subcls;
	DWORD       m_method;
	
	CId(DWORD line, const char* method_name);
	CId(const CId& o);
	~CId();
};

char* dup_s(const char* s, int len = -1);

#endif	//__C_READ_FILE