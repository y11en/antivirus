// ipcBuffer.h ///////////////////////////////////////////////////////////////

// Interprocess Communication Buffer
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

//////////////////////////////////////////////////////////////////////////////

#ifndef IPC_BUFFER_H
#define IPC_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////

enum IPC_SIDE
{
	IPC_SIDE_UNKNOWN,
	IPC_SIDE_SERVER,
	IPC_SIDE_CLIENT
};

//////////////////////////////////////////////////////////////////////////////

class C_IPC_BUFFER
{
public:
	C_IPC_BUFFER();

	static const char * FrefixName();

	bool Create( char *pszBufName, SECURITY_ATTRIBUTES *pSA );
	bool Open( char *pszBufName );
	bool Close();
	DWORD Read( BYTE *pbyBuf, DWORD dwBufSize );	// return 0xffffffff on error
	DWORD Write( BYTE *pbyBuf, DWORD dwBufSize );	// return 0xffffffff on error

	static char			*IPC_PREFIX_NAME;

protected:
	static const DWORD	IPC_BUF_SIZE;
	IPC_SIDE			m_enSide;

private:
	HANDLE				m_hFileMap;
	BYTE				*m_pbyBuf;
};

//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // IPC_BUFFER_H

// eof ///////////////////////////////////////////////////////////////////////