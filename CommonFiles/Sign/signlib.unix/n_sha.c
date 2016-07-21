/*
    n_sha.c

    Digital signature procedures and functions.
    Secure Hash Algorithm functions.

    Last changes: 15.10.96 09:35

    Copyright (c) LAN Crypto. All Rights Reserved.
*/

#define SN_CUR_PROGRAM SN_TYPE_NOTARY

#include "CT_SUPP.h"
#include "CT_FUNCS.h"
#include "sha_defs.h"

void CTAPI CTN_SHAInit( CTN_Hash *hash )
{
    hash->HashLen=0;
    (hash->CT_N_u_Hash).SHA.HV[0]=H0;
    (hash->CT_N_u_Hash).SHA.HV[1]=H1;
    (hash->CT_N_u_Hash).SHA.HV[2]=H2;
    (hash->CT_N_u_Hash).SHA.HV[3]=H3;
    (hash->CT_N_u_Hash).SHA.HV[4]=H4;
    (hash->CT_N_u_Hash).SHA.poshash=0;
    (hash->CT_N_u_Hash).SHA.posh=4;
}

static void CalcHash( CTN_Hash *dsa )
{
    CT_INT i, t;
    CT_ULONG *W;
    CT_ULONG A, B, C, D, E, TEMP;

    A=(dsa->CT_N_u_Hash).SHA.HV[0];
    B=(dsa->CT_N_u_Hash).SHA.HV[1];
    C=(dsa->CT_N_u_Hash).SHA.HV[2];
    D=(dsa->CT_N_u_Hash).SHA.HV[3];
    E=(dsa->CT_N_u_Hash).SHA.HV[4];
    W=(dsa->CT_N_u_Hash).SHA.W;

    for( t=0 ; t < 16 ; t++ )
    {
        TEMP=Sn(A,5)+ft0+E+W[t]+Kt0;
        E=D; D=C; C=Sn(B, 30); B=A; A=TEMP;
    }

    for( ; t < 80 ; t++ )
    {
        i = (CT_INT)(t & 0xF);
        W[i] ^= (W[(i+13)&0xF] ^ W[(i+8)&0xF] ^ W[(i+2)&0xF]);
        TEMP=Sn(A,5)+E+W[i];
        if( t < 20 ) TEMP+=(Kt0+ft0);
        else if( t < 40 ) TEMP+=(Kt1+ft1);
        else if( t < 60 ) TEMP+=(Kt2+ft2);
        else TEMP+=(Kt3+ft3);

        E=D; D=C; C=Sn(B, 30); B=A; A=TEMP;
    }

    (dsa->CT_N_u_Hash).SHA.HV[0] += A;
    (dsa->CT_N_u_Hash).SHA.HV[1] += B;
    (dsa->CT_N_u_Hash).SHA.HV[2] += C;
    (dsa->CT_N_u_Hash).SHA.HV[3] += D;
    (dsa->CT_N_u_Hash).SHA.HV[4] += E;
}

void CTAPI CTN_SHABuffer( CTN_Hash *hash, CT_PTR bufhash, CT_INT len )
{
    CT_ULONG *W;
    CT_UINT posh, poshash;
    CT_PTR buf;

    buf=bufhash;
    hash->HashLen += len;
    W=(hash->CT_N_u_Hash).SHA.W;
    posh=(hash->CT_N_u_Hash).SHA.posh;
    poshash=(hash->CT_N_u_Hash).SHA.poshash;

    while( len-- )
    {
        W[poshash] = (W[poshash] << 8) | (*buf);
        buf++;
        if( !(--posh) )
        {
            poshash++;
            posh=4;

            if( poshash == 16 )
            {
                poshash=0;
                CalcHash( hash );
            }
        }
    }

    (hash->CT_N_u_Hash).SHA.posh = posh;
    (hash->CT_N_u_Hash).SHA.poshash = poshash;
}

void CTAPI CTN_SHAEnd( CTN_Hash *hash )
{
    CT_ULONG *W;
    CT_UINT posh, poshash;
    CT_INT i, j;
    CT_PTR h;

    posh = (hash->CT_N_u_Hash).SHA.posh;
    poshash = (hash->CT_N_u_Hash).SHA.poshash;
    W = (hash->CT_N_u_Hash).SHA.W;

    W[poshash] = (W[poshash] << 8) | 0x80;
    if( --posh ) W[poshash] <<= posh << 3;

    for( i = poshash+1/*++poshash*/; i < 16 ; i++ ) W[i]=0L;

    if( poshash > 13 )
    {
        CalcHash( hash );
        for( i=0 ; i < 16 ; i++ ) W[i]=0L;
    }

    W[15]=hash->HashLen << 3;
    W[14]=hash->HashLen >> (32-3);
    CalcHash( hash );

    h=hash->HashValue;
    W=(hash->CT_N_u_Hash).SHA.HV;
    for( j=0, i=0 ; i < 5 ; i++ )
    {
        h[j++]=(CT_UCHAR)W[i]; W[i] >>= 8;
        h[j++]=(CT_UCHAR)W[i]; W[i] >>= 8;
        h[j++]=(CT_UCHAR)W[i]; W[i] >>= 8;
        h[j++]=(CT_UCHAR)W[i];
    }
}

