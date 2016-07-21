// sigatt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

ULONG
GetFileSize( const char* FileName )
{
    ULONG  dwSize = 0;
    HANDLE hFile;
    
    hFile = CreateFileA( FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

    if ( hFile == INVALID_HANDLE_VALUE )
        return -1;
    
    dwSize = GetFileSize (hFile, NULL) ; 

    CloseHandle ( hFile );

    return dwSize;
}

int
MyReadFile( char* filename, std::vector<char>& Data, ULONG* pSize )
{
    *pSize = GetFileSize( filename );

    FILE* ff = fopen ( filename, "rb" );

    if ( ff )
    {
        std::vector<char> TheFile( *pSize );
        
        fread( &TheFile[0], 1, *pSize, ff );
        
        fclose( ff );
        
        Data = TheFile;
    }

    return *pSize;
}

std::vector<char> g_Key1;
std::vector<char> g_Key2;
std::vector<char> g_Sign;
std::vector<char> g_File;

std::vector<char>
MakeCryptocSign(
                std::vector<char> in_Key1,
                std::vector<char> in_Key2,
                std::vector<char> in_Sign )
{    
    SIGN_STRUCT     CommonHeader;
    CRYPTOC_SIGN    MyHeader;

    MyHeader.Key1Size = in_Key1.size();
    MyHeader.Key2Size = in_Key2.size();
    MyHeader.RegSize  = in_Sign.size();

    CommonHeader.AlgID = ALG_CRYPTOC;
    CommonHeader.Size = sizeof ( SIGN_STRUCT  ) + 
                        sizeof ( CRYPTOC_SIGN ) + 
                          MyHeader.Key1Size + 
                          MyHeader.Key2Size + 
                          MyHeader.RegSize;
    
    std::vector<char> Out( CommonHeader.Size );
    ULONG offs = 0;
    memset ( &Out[0], 'G', CommonHeader.Size );
    
    RtlCopyMemory( &Out[0] + offs, &CommonHeader, sizeof (CommonHeader) );
    offs += sizeof (CommonHeader);
    
    RtlCopyMemory( &Out[0] + offs, &MyHeader, sizeof (MyHeader) );
    offs += sizeof (MyHeader);
    
    RtlCopyMemory( &Out[0] + offs, &in_Key1[0], MyHeader.Key1Size );
    offs += MyHeader.Key1Size;

    RtlCopyMemory( &Out[0] + offs, &in_Key2[0], MyHeader.Key2Size );
    offs += MyHeader.Key2Size;

    RtlCopyMemory( &Out[0] + offs, &in_Sign[0], MyHeader.RegSize );
    offs += MyHeader.RegSize;

    return Out;
}


int main(int argc, char* argv[])
{
    if ( argc < 5 )
    {
        printf ( "Invalid format \n" );
        printf ( "Usage \n" );
        printf ( "Sigatt Key1 Key2 Reg file\n " );

        return -1;
    }

    ULONG FileSize;

    if ( !MyReadFile ( argv[1], g_Key1, &FileSize ) )
        return -1;
    
    if ( !MyReadFile ( argv[2], g_Key2, &FileSize ) )
        return -1;
    
    if ( !MyReadFile ( argv[3], g_Sign, &FileSize ) )
        return -1;

    std::vector<char> CryptocSign =  MakeCryptocSign( g_Key1, g_Key2, g_Sign );

    SIGN_HEADER header;
    SIGN_FOOTER footer;

    header.Tag = SIGN_HEADER_SIGNATURE;
    footer.Tag = SIGN_FOOTER_SIGNATURE;
    header.TotalSize = footer.TotalSize = sizeof ( header ) + sizeof ( footer ) + CryptocSign.size();

    
    FILE* out = fopen ( argv[4], "ab+" );

    if ( out )
    {  
        fwrite( &header, 1, sizeof ( header ), out );
        
        fwrite( &CryptocSign[0], 1, CryptocSign.size(), out );
        
        fwrite( &footer, 1, sizeof ( footer ), out );

        fclose( out );        
    }

    
	return 0;
}
