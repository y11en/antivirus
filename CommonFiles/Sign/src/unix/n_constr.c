/*
    n_constr.c

    Digital signature procedures and functions.
    Constructor and destructor functions.

    Last changes: 15.10.96 09:34

    Copyright (c) LAN Crypto. All Rights Reserved.
*/

#define SN_CUR_PROGRAM SN_TYPE_NOTARY

#include <string.h>
#include <stdlib.h>

#include "ct_funcs.h"

CTN_Notary * CTAPI CTN_NotConstructor()
{
    CTN_Notary *not;

    if( (not=(CTN_Notary *)malloc( sizeof( CTN_Notary ) )) ==
             (CTN_Notary *)CT_NULL ) return (CTN_Notary *)CT_NULL;

    CTN_NotInitVariables( not );
    return not;
}

void CTAPI CTN_NotInitVariables( CTN_Notary *not )
{
    memset( not, 0, sizeof( CTN_Notary ) );
}

void CTAPI CTN_NotDestructor( CTN_Notary *not )
{
    memset( not, 0, sizeof( CTN_Notary ) );
    free( (CT_PTR)not );
}

