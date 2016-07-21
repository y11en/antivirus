/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   BUFFERS.H                                                             */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   defines the functions getBufMem() and freeBufMem() for use            */
/*   in allocating and freeing viedo buffers                               */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#include "tvvo.h"

#if defined( Uses_TVMemMgr ) && !defined( __TVMemMgr )
#define __TVMemMgr

const DEFAULT_SAFETY_POOL_SIZE = 4096;

class TBufListEntry
{

private:

    TBufListEntry( void*& );
    ~TBufListEntry();

    void *operator new( size_t, size_t );
    void *operator new( size_t );
    void operator delete( void * );

    TBufListEntry *next;
    TBufListEntry *prev;
    void*& owner;

    static TBufListEntry *near bufList;
    static Boolean freeHead();

    friend class TVMemMgr;
#ifdef __DOS16__
    friend void * TV_CDECL operator new( size_t );
#else
    friend void * operator new( size_t );
#endif

};

class TVMemMgr
{

public:

    TVMemMgr();

    static void resizeSafetyPool( size_t = DEFAULT_SAFETY_POOL_SIZE );
    static int safetyPoolExhausted();

    static void allocateDiscardable( void *&, size_t );
    static void freeDiscardable( void * );

private:

    static void * near safetyPool;
    static size_t near safetyPoolSize;
    static int near inited;
    static int initMemMgr();

};

#endif  // Uses_TVMemMgr

#include "tvvo2.h"

