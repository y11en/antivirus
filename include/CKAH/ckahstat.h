#ifndef __CKAHSTAT_H_
#define __CKAHSTAT_H_

#include "ckahdefs.h"
#include "ckahstatdefs.h"

#define CKAHSTATMETHOD_ CKAHSTATEXPORT
#define CKAHSTATMETHOD CKAHSTATMETHOD_ CKAHUM::OpResult 

namespace CKAHSTAT
{
    enum BaseDrvType 
    {
        bdKlick = 1,
        bdKlin
    };
    

    CKAHSTATMETHOD GetConnectionCount( ULONG* pCount );

    CKAHSTATMETHOD GetDroppedConnectionCount( ULONG* pCount );
        
    CKAHSTATMETHOD GetPassDropPktStat( 
                            OUT __int64* nInDropped, 
                            OUT __int64* nInPassed, 
                            OUT __int64* nOutDropped,                             
                            OUT __int64* nOutPassed );

    CKAHSTATMETHOD GetTrafficInfoLastDay( 
                            OUT __int64* nInBytes,
                            OUT __int64* nOutBytes );
}

#endif
