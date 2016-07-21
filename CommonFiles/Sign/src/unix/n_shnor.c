/*
    n_shnor.c

    Digital signature procedures and functions.
    Shnor Digital Signature Algorithm functions.

    Last changes: 02.01.97 14:16

    Copyright (c) LAN Crypto. All Rights Reserved.
*/

#define SN_CUR_PROGRAM SN_TYPE_NOTARY

#include <string.h>
#include "ct_supp.h"
#include "ct_funcs.h"
#ifdef __FULL_DEBUG
#include "debug.h"
#endif

#define CT_N_LEN_INTERVAL_RESULT    64

CT_INT CTAPI CTN_LANSNot( CTN_Notary *not )
{

    CT_UINT i;
    CT_PTR buf;
    CTN_Hash h;
    CT_ULONG *HV;
#ifdef __FULL_DEBUG
    int debug_i;
#endif

    HV=h.CT_N_u_Hash.SHA.HV;
    buf = not->buffer;
    memcpy( buf, (not->Public).UserSN, CT_N_LEN_ONLYSN );
    memcpy( &buf[CT_N_LEN_ONLYSN], (not->Public).UserName, CT_N_LEN_USERNAME );
    _not_sh( (not->RS).r, (not->RS).s, (not->Public).B1, (not->Public).B2,
             (not->Public).B3, &buf[CT_N_LEN_ONLYSN+CT_N_LEN_USERNAME]);
#ifdef __FULL_DEBUG
    if( debugfile != NULL )
    {
        fprintf( debugfile, "    Interval result :\n    " );
        for( debug_i = 0;
             debug_i < CT_N_LEN_ONLYSN+CT_N_LEN_USERNAME+CT_N_LEN_INTERVAL_RESULT;
             debug_i++ )
            fprintf( debugfile, "0x%02X,", buf[debug_i] );
        fprintf( debugfile, "\n" );
    }
#endif
    CTN_SHAInit( &h );
    for( i=0 ; i < CT_N_HASH_LEN_DSA/4 ; i++ )
        HV[i]=(not->Hash).CT_N_u_Hash.SHA.HV[i];
    CTN_SHABuffer( &h, buf, CT_N_LEN_ONLYSN+CT_N_LEN_USERNAME +
                    CT_N_LEN_INTERVAL_RESULT);
    CTN_SHAEnd( &h );
#ifdef __FULL_DEBUG
    if( debugfile != NULL )
    {
        fprintf( debugfile, "    True Hash Value :\n    " );
        for( debug_i = 0; debug_i < CT_N_LEN_MAXHASH; debug_i++ )
            fprintf( debugfile, "0x%02X,", h.HashValue[debug_i] );
        fprintf( debugfile, "\n" );
    }
#endif
    for( i = 0; i < CT_N_HASH_LEN_LANS; i++  )
    {
        if( h.HashValue[i] != (not->RS).s[i]) break;
    }
    if( i != CT_N_HASH_LEN_LANS) return CT_ERR_N_BAD_SIGNATURE;
    else                         return CT_ERR_OK;
}

