#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#ifdef  UNICODE
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif

typedef std::list<tstring> tstringlist;

typedef std::list<string> sstringlist;
typedef std::list<wstring> wstringlist;

#endif