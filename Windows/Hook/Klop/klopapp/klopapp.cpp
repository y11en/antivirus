// klopapp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

char	MyKey[11] = "HelloWorld";

int main(int argc, char* argv[])
{

    /*
	if ( KLOPUM::srOK == KLOPUM::KlopInit(MyKey, sizeof ( MyKey ) ) )
	{
		ULONG	TimeToWait = 60000;
		ULONG	KeyCount;

		KLOPUM::KlopActivate( TimeToWait );

		printf ("Wait for %d msec\n", TimeToWait);
		getch();

		if ( KLOPUM::srOK == KLOPUM::KlopGetCount( &KeyCount ) )
		{
			printf ("%d Licence key found\n", KeyCount );
		}
		else
		{
			printf("Error\n");
		}


		KLOPUM::KlopDone();
	}
	else
	{
		printf ("Init unsuccessful\n");
	}
    */
    HANDLE  hKlop;
    DWORD   dwRet;
    BOOL    Res;

    printf ("Klopapp. macs tool\n" );

    // открываем драйвер через Symbolic Link
    hKlop = CreateFile(
        "\\\\.\\"KLOP_DRV_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
        );
    
    if ( hKlop == INVALID_HANDLE_VALUE )
    {
        // скорей всего просто не загружен драйвер.
        printf ( "unable to open klop\n" );
        
        return -1;
    }

    char* buf = (char*)malloc ( 0x10000 );

    if ( buf )
    {
        Res = DeviceIoControl( hKlop, KLOP_CP_GETMACS, NULL, 0, buf, 0x10000, &dwRet, NULL );

        if ( Res )
        {
            KlopKeyItem* Item = (KlopKeyItem*)buf;
            int dwCount = dwRet / sizeof( KlopKeyItem );

            printf( "got replies from : \n" );

            while ( dwCount-- )
            {
                printf ( "[%02x.%02x.%02x.%02x.%02x.%02x]\n",
                    (UCHAR)Item[dwCount].MacAddr[0],
                    (UCHAR)Item[dwCount].MacAddr[1],
                    (UCHAR)Item[dwCount].MacAddr[2],
                    (UCHAR)Item[dwCount].MacAddr[3],
                    (UCHAR)Item[dwCount].MacAddr[4],
                    (UCHAR)Item[dwCount].MacAddr[5]);
            }

            printf( ".\n" );
        }
        else
        {
            printf ("KLOP_CP_GETMACS not supported. err = %d\n", GetLastError() );
        }

        free ( buf );
    }
    else
    {
        printf ("unabel to allocate 0x10000 bytes\n" );
    }

    CloseHandle( hKlop );

	return 0;
}
