// ipcInfo.h /////////////////////////////////////////////////////////////////

// Interprocess Communication Information to Client and Server
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

//////////////////////////////////////////////////////////////////////////////

#ifndef IPC_INFO_H
#define IPC_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////

class C_IPC_INFO
{
public:
	inline C_IPC_INFO() { *m_szGUID = 0; *m_szLinkName = 0; }
	bool Get();

	char m_szGUID[ _MAX_PATH ];
	char m_szLinkName[ _MAX_PATH ];

private:
	static DWORD GetGUID( char *pszBuf, DWORD dwBufSize ); // return 0xffffffff on error
};

//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // IPC_INFO_H

// eof ///////////////////////////////////////////////////////////////////////