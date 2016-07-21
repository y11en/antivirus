////////////////////////////////////////////////////////////////////
//
//  LoadKeyImp.cpp
//  Definition for Load AVP key procedures
//  AVP general purposes software
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <stdafx.h>

#include <malloc.h>
#include <serialize/kldtser.h>
#include <_avpio.h>
#include <LoadKeys\LoadKey.h>
#include <Stuff\calcsum.h>
#include <Sign\sign.h>

//#define OLD_KEY_SUPPORT
#undef OLD_KEY_SUPPORT
//!!! WE STOPPED OLD KEY SUPPORT !!!

#ifdef OLD_KEY_SUPPORT
#include <LoadKeys\key.h>
#endif

void* (* LKAllocator)(AVP_uint) = NULL;
void  (* LKLiberator)(void*) = NULL;

// ---
static BOOL LoadNewKeyTree( const TCHAR *pszFileName, HDATA *hRootData ) {
	::KLDT_Init_Library( LKAllocator, LKLiberator );
	return !!::KLDT_DeserializeUsingSWM( pszFileName, hRootData );
}


#ifdef OLD_KEY_SUPPORT
// ---
static int gaAppIDTable[] = {
	AVP_APID_MONITOR_WIN_95       , // (0x10) // 16
	AVP_APID_MONITOR_WIN_NTWS     , // (0x11) // 17
	AVP_APID_SCANNER_DOS_LITE     , // (0x12) // 18
	AVP_APID_SCANNER_DOS_32       , // (0x13) // 19
	AVP_APID_SCANNER_DOS_16				, // (0x14) // 20
	AVP_APID_SCANNER_WIN_95				, // (0x15) // 21
	AVP_APID_SCANNER_WIN_NTWS			, // (0x16) // 22
	AVP_APID_CONTROLCENTER        , // (0x17) // 23
	AVP_APID_INSPECTOR            , // (0x18) // 24
	AVP_APID_UPDATOR              , // (0x20) // 32
	AVP_APID_NOVELL               , // (0x21) // 33
	AVP_APID_NT_SERVER            , // (0x22) // 34
	AVP_APID_OS_2                 , // (0x23) // 35
	AVP_APID_WEBINSPECTOR         , // (0x24) // 36
	AVP_APID_EXCHANGE             , // (0x25) // 37
	AVP_APID_LINUX                , // (0x26) // 38
	AVP_APID_NCC_SERVER           , // (0x27) // 39
	AVP_APID_NCC_CONSOLE          , // (0x28) // 40
	AVP_APID_OFFICEGUARD          , // (0x29) // 41
	AVP_APID_OFFICEMONITOR        , // (0x2a) // 42
	AVP_APID_OFFICEMAILCHECKER    , // (0x2b) // 43
	AVP_APID_FIREWALL             , // (0x2c) // 44
	AVP_APID_FREEBSD              , // (0x2d) // 45
	AVP_APID_BSDIUNIX             , // (0x2e) // 46
	AVP_APID_INVISIMAIL           , // (0x2f) // 47
	AVP_APID_LINUXSERVER          , // (0x30) // 48
	AVP_APID_UNIX_SCANNER         , // (0x31) // 49
	AVP_APID_UNIX_DAEMON          , // (0x32) // 50
};

static int gnAppIDTableCount = sizeof(gaAppIDTable) / sizeof(gaAppIDTable[0]);

// ---
static BOOL CheckAppID( AVP_Key *pKey, int nAppID ) {
	switch ( nAppID ) {
		case AVP_APID_SCANNER_WIN_95:
			return !!(pKey->Platforms & KEY_P_WIN95);
		case AVP_APID_SCANNER_WIN_NTWS:
			return !!(pKey->Platforms & KEY_P_WINNT);
		case AVP_APID_MONITOR_WIN_95:
			return !!(pKey->Platforms & KEY_P_WIN95) && !!(pKey->Options & KEY_O_MONITOR);
		case AVP_APID_MONITOR_WIN_NTWS:
			return !!(pKey->Platforms & KEY_P_WINNT) && !!(pKey->Options & KEY_O_MONITOR);
		case AVP_APID_OS_2:
			return !!(pKey->Platforms & KEY_P_OS2);
		case AVP_APID_SCANNER_DOS_LITE:
			return !!(pKey->Platforms & KEY_P_DOSLITE);
		case AVP_APID_NOVELL:         
			return !!(pKey->Platforms & KEY_P_NOVELL);
		case AVP_APID_SCANNER_DOS_32:
		case AVP_APID_SCANNER_DOS_16:
			return !!(pKey->Platforms & KEY_P_DOS);
		case AVP_APID_CONTROLCENTER:
		case AVP_APID_UPDATOR:       
			return !!(pKey->Platforms & KEY_P_WIN95) || !!(pKey->Platforms & KEY_P_WINNT);
		case AVP_APID_INSPECTOR:   
		case AVP_APID_WEBINSPECTOR:
		case AVP_APID_EXCHANGE:
		case AVP_APID_NT_SERVER:
			return 0;
	}
	return 0;
}


extern DWORD unsqu( BYTE *i_buf, BYTE *o_buf );

// ---
static BOOL LoadOldKeyTree( const TCHAR *pszFileName, HDATA *hRootData ) {
	BOOL bResult = 0;

	HANDLE hFile = AvpCreateFile( pszFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if ( hFile != INVALID_HANDLE_VALUE ) {

		AVP_KeyHeader KeyHeader;
		AVP_Key *pKey;
		DWORD count;
		AvpReadFile( hFile, &KeyHeader, sizeof(AVP_KeyHeader), &count, 0 );
		if ( KeyHeader.Magic == (DWORD)AVP_KEY_MAGIC ) {

			if ( KeyHeader.UncompressedSize > sizeof(AVP_Key) &&
					 KeyHeader.UncompressedSize < 5*1024 ) {

				char *pBuffCompressed		 = (char *)::LKAllocator(sizeof(char) * KeyHeader.CompressedSize);
				char *pBuffUncompressed  = (char *)::LKAllocator(sizeof(char) * KeyHeader.UncompressedSize);

				bResult = 1;

				AvpReadFile( hFile, pBuffCompressed, KeyHeader.CompressedSize, &count, 0 );

				if ( KeyHeader.CompressedCRC == CalcSum((BYTE*)pBuffCompressed, KeyHeader.CompressedSize) ) {

					for( UINT i=0; i<KeyHeader.CompressedSize; i++ ) 
						pBuffCompressed[i] ^= (BYTE)i;

					if ( KeyHeader.UncompressedSize == unsqu((BYTE*)pBuffCompressed, (BYTE*)pBuffUncompressed) ) {

						pKey = (AVP_Key*)pBuffUncompressed;
						
						if ( ::DATA_Init_Library(LKAllocator, LKLiberator) ) {

							*hRootData=::DATA_Add(0,0,AVP_PID_KEYROOT,0,0);
							if ( *hRootData ) {

								DWORD nLevel = 0;

								SYSTEMTIME stExpir;
								memset( &stExpir, 0, sizeof(SYSTEMTIME) );

								stExpir.wYear  = pKey->ExpirYear;
								stExpir.wMonth = pKey->ExpirMonth;
								stExpir.wDay   = pKey->ExpirDay;

								HDATA hInfo;
								hInfo = ::DATA_Add( *hRootData, 0, AVP_PID_KEYINFO, 0, 0 );

								OemToChar(pBuffUncompressed+pKey->RegNumberOffs, pBuffUncompressed+pKey->RegNumberOffs);
								OemToChar(pBuffUncompressed+pKey->CopyInfoOffs, pBuffUncompressed+pKey->CopyInfoOffs);
								OemToChar(pBuffUncompressed+pKey->SupportOffs, pBuffUncompressed+pKey->SupportOffs);

								::DATA_Add( hInfo,0, PID_SERIALSTR, (DWORD)pBuffUncompressed+pKey->RegNumberOffs,0 );
								::DATA_Add( hInfo,0, AVP_PID_KEYPLPOSNAME,  (DWORD)"AVP Key v1.0", 0 );
								::DATA_Add( hInfo,0, AVP_PID_KEYLICENCEINFO, (DWORD)pBuffUncompressed + pKey->CopyInfoOffs, 0 );
								::DATA_Add( hInfo,0, AVP_PID_KEYSUPPORTINFO, (DWORD)pBuffUncompressed + pKey->SupportOffs, 0 );
								::DATA_Add( hInfo,0, AVP_PID_KEYEXPDATE, (DWORD)&stExpir, sizeof(AVP_date) );

								if ( pKey->Options & KEY_O_DISINFECT )
									nLevel=10;

								if ( (pKey->Options & KEY_O_UNPACK) || (pKey->Options & KEY_O_EXTRACT) )
									nLevel=20;

								if ( (pKey->Options & KEY_O_REMOTELAUNCH) || (pKey->Options & KEY_O_REMOTESCAN) )
									nLevel=30;

								for ( int i=0; i<gnAppIDTableCount; i++ ) {
									if ( CheckAppID(pKey, gaAppIDTable[i]) ) {
										AVP_dword nPID = MAKE_AVP_PID(gaAppIDTable[i], AVP_APID_GLOBAL, avpt_dword, 0);
										::DATA_Add( *hRootData, NULL, nPID, nLevel, 0 );
										break;
									}
								}
							}
						}
					}
				}
				::LKLiberator( pBuffCompressed );
				::LKLiberator( pBuffUncompressed );
			}
		}
		AvpCloseHandle( hFile );
	}
	return bResult;
}

#endif //ifdef OLD_KEY_SUPPORT

// ---
BOOL LoadKeyTreeImp( const TCHAR *pszFileName, HDATA *hRootData, BOOL &bVerified ) {
	*hRootData = NULL;

	bVerified = TRUE;
	BOOL bResult = FALSE;

#ifdef OLD_KEY_SUPPORT
	bResult = ::LoadOldKeyTree( pszFileName, hRootData );
#endif //ifdef OLD_KEY_SUPPORT

	if ( !bResult ) {
		bVerified = ::sign_check_file( pszFileName, 1, 0, 0, 0 ) == SIGN_OK;
		bResult = ::LoadNewKeyTree( pszFileName, hRootData );
	}

	return bResult;
}

