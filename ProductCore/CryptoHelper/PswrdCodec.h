//////////////////////////////////////////////////////////////////////
// CryptoHelper.h: interface for the CPasswordCodec class.
#if !defined(AFX_CRYPTOHELPER_H__A2D846C5_271F_45C5_8E74_9047FF186F66__INCLUDED_)
#define AFX_CRYPTOHELPER_H__A2D846C5_271F_45C5_8E74_9047FF186F66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <wincrypt.h>
#include <prague.h>


class CPasswordCodec
{
public:
	CPasswordCodec();

	enum CryptoErr { CErr_Ok = 0, 
					 CErr_CreateKey,
					 CErr_GetKeyContainer,
					 CErr_GenKey,
					 CErr_GetUserKey,
					 CErr_GenSessionKey,
					 CErr_DestroyExKey,
					 CErr_CompBLOB,
					 CErr_OutOfMem,
					 CErr_EncryptSession,
					 CErr_Encrypting,
					 CErr_DecryptSession,
					 CErr_Decrypting,
					};

	CryptoErr encryptPassword(DATA_BLOB PasswordBlob, DATA_BLOB &EncryptedPasswordBlob);
	CryptoErr decryptPassword(DATA_BLOB EncryptedPasswordBlob, DATA_BLOB &PasswordBlob);

	tERROR CryptoErr2PragueError(CryptoErr cerr);
	TCHAR* CryptoErrDescr(CryptoErr cerr);

private:
	TCHAR m_szContainerName[MAX_PATH];
};

#endif // !defined(AFX_CRYPTOHELPER_H__A2D846C5_271F_45C5_8E74_9047FF186F66__INCLUDED_)
