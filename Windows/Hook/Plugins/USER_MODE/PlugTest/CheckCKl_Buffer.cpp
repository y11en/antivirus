#include "kl_buffer.h"

void
Check_CKl_Buffer()
{
    printf ("*********************************************************************\n");
    printf ("*******************Check_CKl_Buffer**********************************\n");
    printf ("*********************************************************************\n");

    char*   SrcBuffer       = "Hello World WorldA";
    char*   DstBuffer[]    = { "WorldA", "WorldB", "Hello", "World", "ldAp", "Hello World WorldA" };

    printf ("Src String = %s\n", SrcBuffer);

    ULONG   StringCount = sizeof ( DstBuffer ) / sizeof (char*);

    CKl_Buffer  MyBuffer(SrcBuffer, strlen ( SrcBuffer) );
    
    printf("Start Check Find ----------------\n");
    // Check Find
    for ( ULONG i = 0; i < StringCount; ++ i )
    {
        if ( MyBuffer.Find( DstBuffer[i], strlen( (DstBuffer[i]) ), NULL ) )
        {
            printf ("%s found !\n", DstBuffer[i]);
        }
        else
        {
            printf ("%s NOT found !\n", DstBuffer[i]);
        }
    }

    printf("Start Check Allocate\\Free ----------------\n");
    // Check Allocate/Free
    MyBuffer.Allocate( 100 );
    MyBuffer.Free();

    printf("Start CopyFrom ----------------\n");
    // Check CopyFrom
    MyBuffer.Allocate( 100 );
    MyBuffer.CopyFrom(SrcBuffer, strlen(SrcBuffer) );
    // Check Find
    for ( i = 0; i < StringCount; ++ i )
    {
        if ( MyBuffer.Find( DstBuffer[i], strlen( (DstBuffer[i]) ), NULL ) )
        {
            printf ("%s found !\n", DstBuffer[i]);
        }
        else
        {
            printf ("%s NOT found !\n", DstBuffer[i]);
        }
    }
    MyBuffer.Free();

}

bool
Test1()
{    
    CKl_BasicBuffer<ULONG> TestBuffer( 100 );
    TestBuffer.Zeromem();
    
    TestBuffer[4] = 1;

    ULONG    m = TestBuffer[4];
    ULONG    n = TestBuffer[3];    

    return true;
}


void TestInitCKl_Buffer(std::vector<TestFunc>& Test)
{
    printf ("*********************************************************************\n");
    printf ("*******************Check_CKl_Buffer**********************************\n");
    printf ("*********************************************************************\n");
    
    Test.push_back( &Test1           );
    
}