#ifndef __STRINGDEFS_H__
#define __STRINGDEFS_H__

#ifdef  UNICODE
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif

#endif