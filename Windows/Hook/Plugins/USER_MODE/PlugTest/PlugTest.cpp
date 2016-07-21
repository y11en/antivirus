// PlugTest.cpp : Defines the entry point for the console application.


#include "kldef.h"
#include "kl_object.h"
#include "kl_timer.h"
#include "streamtest.h"
#include "conio.h"
#include "..\hook\klick_api.h"

#define TCP 0x6
#define UDP 0x11

struct MyStream {
    ULONG srcIP;
    USHORT srcPort;
    ULONG dstIP;
    USHORT dstPort;
    UCHAR Protocol;
    bool isIncoming;
};

MyStream ts[] = {
    { 1, 1, 100, 100, TCP, true },
    { 1, 2, 100, 100, TCP, true },
    { 1, 3, 100, 100, TCP, true },
    { 2, 1, 100, 100, TCP, true },
    { 3, 2, 100, 100, TCP, true },
    { 4, 3, 100, 100, TCP, true },
    { 1, 1, 200, 100, TCP, true },
    { 1, 1, 300, 100, TCP, true },
    { 1, 1, 400, 100, TCP, true },

    { 1, 1, 200, 500, TCP, true },
    { 1, 1, 300, 600, TCP, true },
    { 1, 1, 400, 700, TCP, true },

    { 100, 100, 1, 1, TCP, true },
    { 100, 100, 1, 2, TCP, true },
    { 100, 100, 1, 3, TCP, true },
    { 100, 100, 1, 4, TCP, true },    
    { 200, 100, 1, 4, TCP, true },    
    { 300, 100, 1, 4, TCP, true },    
    
};

CKl_PktConns* Conns;

void
PrintStreams( char* Buffer, ULONG BufferSize )
{
    StreamTable* st = (StreamTable*)Buffer;

    for ( int i = 0; i < st->Count; i++ )
    {
        printf( "%d:%d : %d:%d %s\n", 
            st->tbl[i].srcIp,
            st->tbl[i].srcPort,
            st->tbl[i].dstIp,
            st->tbl[i].dstPort,
            st->tbl[i].isIncoming ? "IN" : "OUT"
            );
    }
}

int main(int argc, char* argv[])
{
    Conns = new CKl_PktConns();
    Conns->Initialize();

    for ( int i = 0; i < sizeof( ts ) / sizeof ( MyStream ); i++ )
    {
        if ( !Conns->FindStream( ts[i].srcIP, ts[i].srcPort, ts[i].dstIP, ts[i].dstPort, ts[i].Protocol ) )
            Conns->AddStream( ts[i].srcIP, ts[i].srcPort, ts[i].dstIP, ts[i].dstPort, ts[i].Protocol, ts[i].isIncoming );
        else
        {
            printf( "stream duplicates\n" );
        }
    }

    ULONG MemSize = sizeof( StreamTable ) * 100;
    char* Mem = (char*) malloc ( MemSize );
    
    if  ( Mem )
    {
        Conns->GetStreamList( TCP, Mem, MemSize );

        PrintStreams( Mem, MemSize );

        free ( Mem );
    }

    CPktStream *S1, *S2, *S3, *S4, *S5, *S6;

    S1 = Conns->FindStream( 1, 2, 100, 100, TCP);
    S2 = Conns->FindStream( 100, 100, 1, 2, TCP);
    S3 = Conns->FindStream( 0, 2, 100, 100, TCP);    
    S5 = Conns->FindStream( 1, 2, 0, 100, TCP);    

    S1 = Conns->FindStream( 100, 100, 1, 4, TCP);

    PVOID Handle;
    ULONG isIncoming;

    KLSTATUS KlStatus;

    KlStatus = Conns->GetStreamInfo( TCP, 1, 2, 100, 100, &Handle, &isIncoming, NULL, NULL );
    
 	return 0;
}
