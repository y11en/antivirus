/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	FFRedirect.cpp
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/15/2005 11:29:46 AM
*		
*		Example:	
*		
*		
*		
*/		

#include <windows.h>

#include <_avpio.h>

#define AVPIOSTATIC

#include <_avpff.h>
#include <Stuff/CPointer.h>
#include <Stuff/PathStr.h>


typedef struct tag_FFContent {
	HANDLE						m_hFFHandle;
	WIN32_FIND_DATA   m_FFData;
	TCHAR            *m_pDirName;
} FFContent;

//! \fn				: WIOFindFirstFile
//! \brief			:	
//! \return			: HANDLE WINAPI 
//! \param          : LPCTSTR lpFileFolder
HANDLE WINAPI WIOFindFirstFile( LPCTSTR lpFileFolder, LPTSTR lpFoundFileName, DWORD dwFileNameSize ) {
	CPointer<FFContent> pContent = new FFContent;

	if ( pContent ) {
		memset( pContent, 0, sizeof(FFContent) );

		pContent->m_pDirName = CPathStr(lpFileFolder).Relinquish();

		CPathStr rcFindName( lpFileFolder );
		rcFindName ^= _T("*");

		pContent->m_hFFHandle = ::FindFirstFile( rcFindName, &pContent->m_FFData );

		if ( pContent->m_hFFHandle != INVALID_HANDLE_VALUE ) {
			rcFindName = pContent->m_pDirName;
			rcFindName ^= pContent->m_FFData.cFileName;
			_tcsncpy( lpFoundFileName, rcFindName, dwFileNameSize );

			return (HANDLE)pContent.Relinquish();
		}
	}
	return NULL;
}


//! \fn				: WIOFindNextFile
//! \brief			:	
//! \return			: BOOL WINAPI 
//! \param          :  HANDLE hFindFile
//! \param          : LPTSTR lpFoundFileName
//! \param          : DWORD dwFileNameSize
BOOL WINAPI WIOFindNextFile( HANDLE hFindFile, LPTSTR lpFoundFileName, DWORD dwFileNameSize ) {
	FFContent *pContent = (FFContent *)hFindFile;
	if ( pContent && pContent->m_hFFHandle != INVALID_HANDLE_VALUE ) {
		if ( ::FindNextFile(pContent->m_hFFHandle, &pContent->m_FFData) ) {
			CPathStr rcFindName( pContent->m_pDirName );
			rcFindName ^= pContent->m_FFData.cFileName;
			_tcsncpy( lpFoundFileName, rcFindName, dwFileNameSize );
			return TRUE;
		}
	}
	return FALSE;
}



//! \fn				: WIOFindClose
//! \brief			:	
//! \return			: BOOL WINAPI 
//! \param          : HANDLE hFindFile
BOOL WINAPI WIOFindClose( HANDLE hFindFile ) {
	FFContent *pContent = (FFContent *)hFindFile;
	if ( pContent ) {
		BOOL bResult = ::FindClose( pContent->m_hFFHandle );
		if ( pContent->m_pDirName )
			delete [] pContent->m_pDirName;
		delete pContent;
		return bResult;
	}
	return FALSE;
}



//! \fn				: LoadFF
//! \brief			:	
//! \return			: BOOL 
//! \param          : void
BOOL LoadFF(void) {
  AvpFindFirstFile		= WIOFindFirstFile;
  AvpFindNextFile			= WIOFindNextFile;
  AvpFindClose				= WIOFindClose;

	return TRUE;
}
