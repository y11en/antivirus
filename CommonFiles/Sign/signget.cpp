#include "signget.h"

#include <io.h>
#include <fcntl.h>
#include <string>
#include <windows.h>

#if defined(UNICODE) || defined(_UNICODE)
#undef sign_check_file
#endif

//два объ€влени€ вз€ты, чтобы собиралось с _UNICODE и _MBCS
extern "C" int SIGNAPI sign_check_fileW( const wchar_t* file, int kasp_key, void* pub_keys[], int key_count, int all_of );
extern "C" int SIGNAPI sign_check_file( const char* file, int kasp_key, void* pub_keys[], int key_count, int all_of );

//-----------------------------------------------------------------------------

static int SIGNAPI sign_get_file( int hFile, 
                                  TSignBuffer& vSign )
{
    int nRet = SIGN_OK;

    while( 1 )
    {
        const long lSize = _filelength( hFile );
        if( lSize < 0 )
        {
            nRet = SIGN_BAD_FILE;
            break;
        }

        if ( 0 > ::_lseek( hFile, lSize - DEF_SIGN_BUFFER - 2, SEEK_SET ) )
        {
            nRet = SIGN_BAD_FILE;
            break;
        }

        const size_t uiBytes = ::_read( hFile, vSign, DEF_SIGN_BUFFER );

        if (uiBytes != DEF_SIGN_BUFFER)
        {
            nRet = SIGN_BAD_FILE;
            break;
        }

        break;

    } //while

    ::_close( hFile );

    return nRet;
}

//-----------------------------------------------------------------------------

static std::string UnicodeToMbcs( const wchar_t *pSourceStr ) 
{
    const int nSize = ::WideCharToMultiByte( CP_ACP, 0, 
                                             const_cast<wchar_t*> ( pSourceStr ),
                                             -1, NULL, 0, NULL, NULL );
    char* szDestStr = new char[ nSize+1 ];
    ::WideCharToMultiByte( CP_ACP, 0, 
                           const_cast<wchar_t*> ( pSourceStr ),
                           -1, szDestStr, nSize, NULL, NULL );
    const std::string stResult( szDestStr );
    delete[] szDestStr;
    return stResult;
}

//-----------------------------------------------------------------------------

int SIGNAPI sign_get_file( const char* szFileName, 
                           TSignBuffer& vSign )
{
    int nRet = sign_check_file( szFileName, 1, NULL, 0, 0 );
    if( SIGN_OK != nRet )
        return nRet;

    int hFile = ::_open( szFileName, _O_RDONLY | _O_BINARY );
    if ( hFile < 0 )
        return SIGN_BAD_FILE;

    return sign_get_file( hFile, vSign );
}

//-----------------------------------------------------------------------------

int SIGNAPI sign_get_file( const wchar_t* wszFileName, 
                           TSignBuffer& vSign )
{
    if ( ( ::GetVersion() & 0x80000000 ) )
    { //WIN9X
        const std::string stFileName = UnicodeToMbcs( wszFileName );
        return sign_get_file( stFileName.c_str(), vSign );
    }

    int nRet = sign_check_fileW( wszFileName, 1, NULL, 0, 0 );
    if( SIGN_OK != nRet )
        return nRet;

    int hFile = ::_wopen( wszFileName, _O_RDONLY | _O_BINARY );
    if ( hFile < 0 )
        return SIGN_BAD_FILE;

    return sign_get_file( hFile, vSign );
}


//-----------------------------------------------------------------------------


