/*
    n_signsh.c

    Digital signature procedures and functions.
    Main digital signature functions (SHNOR-144).

    Last changes: 14.12.96 13:21

    Copyright (c) LAN Crypto. All Rights Reserved.
*/
#include "ct_supp.h"

#define LENPRIME    64      /* length of prime number (in bytes) */
#define LENEXP      18      /* length of exponent (in bytes) */
#define MEMSIZE     0       /* length of static memory: 0 - small, 1 - large */

#define SINT        2
#define SPRIME      ((LENPRIME+SINT-1)/SINT)
#define SEXP        ((LENEXP+SINT-1)/SINT)
#define MINT        (CT_UINT)(0xFFFF)

static void pack(CT_PTR, CT_UINT *, CT_INT);
static void unpack(CT_UINT *, CT_PTR, CT_INT);
static void multmod(CT_UINT *, CT_UINT *, CT_UINT *, CT_UINT *, CT_UINT, CT_INT);
static void priv(CT_UINT *, CT_UINT *, CT_UINT, CT_INT, CT_INT, CT_INT);
static void stel( CT_PTR, CT_INT *, CT_INT *, CT_INT);
static void expmod(CT_UINT *, CT_PTR, CT_UINT *, CT_PTR);

#include "dat_s144.h"

void CTAPI _not_sh( CT_PTR r, CT_PTR s, CT_PTR b1, CT_PTR b2, CT_PTR b3,
                    CT_PTR answer)
/* inputs :
    r     -  first part of the sign, its length is LENEXP bytes;
    s     -  second part of the sign, its length is LENEXP/2 bytes;
    b1,b2,b3 -  public key, length each part is LENPRIME bytes;
   output :
    answer - its length is LENPRIME bytes.
*/
{
    CT_INT i;
    CT_UINT  bb[BLOKS*SPRIME],rab[SPRIME];
    CT_UCHAR x2[LENEXP];

    for(i=0;i<LENEXP/2;i++)x2[i]=s[i];
    for(;i<LENEXP;i++)x2[i]=0;
    pack(b1,bb,LENPRIME);
    multmod(bb,bb,&bb[SPRIME],p,p1,SPRIME);
    multmod(bb,&bb[SPRIME],&bb[SPRIME],p,p1,SPRIME);
    pack(b2,&bb[2*SPRIME],LENPRIME);
    multmod(&bb[2*SPRIME],&bb[2*SPRIME],&bb[3*SPRIME],p,p1,SPRIME);
    multmod(&bb[2*SPRIME],&bb[3*SPRIME],&bb[3*SPRIME],p,p1,SPRIME);
    pack(b3,&bb[4*SPRIME],LENPRIME);
    multmod(&bb[4*SPRIME],&bb[4*SPRIME],&bb[5*SPRIME],p,p1,SPRIME);
    multmod(&bb[4*SPRIME],&bb[5*SPRIME],&bb[5*SPRIME],p,p1,SPRIME);
    expmod(rab,r,bb,x2);
    priv(rab,p,p1,SPRIME,SPRIME,SPRIME);
    unpack(rab,answer,LENPRIME);
    return;
}

void priv(CT_UINT *u, CT_UINT *v, CT_UINT v1, CT_INT lu, CT_INT lv, CT_INT count)
/*
    input : u  - an array
            lu - size of u
            v  - an array
            lv - size of v
            v1 = -v[0]^-1 mod 256^SINT
            count - integer
    output: array u is modify at a such way, that u = u/c mod v,
        where c = 256^(lv*count*SINT) mod v.
*/
{
    CT_INT i,j;
    CT_ULONG k,t;

    for(i=count;i>0;i--)                      /* step */
        {
        t=((CT_ULONG)v1*u[0])&MINT;
        k=(t*v[0]+u[0])>>(8*SINT);
        for(j=1;j<lv;j++)
            {
            k+=t*v[j]+u[j];
            u[j-1]=(CT_UINT)(k&MINT);
            k>>=(8*SINT);
            }
        for(;j<lu;j++)
            {
            k+=u[j];
            u[j-1]=(CT_UINT)(k&MINT);
            k>>=(8*SINT);
            }
        u[j-1]=(CT_UINT)k;
        }
    return;
}

static void multmod(CT_UINT *v, CT_UINT *u, CT_UINT *w, CT_UINT *g, CT_UINT g1,
             CT_INT l)
/*
	input : v - multiplicand;
		u - multiplier;
		q - module;
		l - leght of v,u and g;
		g1 = -(g[0]^-1) mod 256^SINT.
	output : w, as Montgomery product of v and u on module g, t.i.
		w = v*u/r mod g, where r = 256^(l*SINT) mod g.
*/
{
CT_INT i,j;                                    /* counters */
CT_ULONG k,t;                     /* temporary registers */
CT_UINT d[SPRIME+2];                      /* temporary array */
	for(i=0;i<=l+1;i++)d[i]=0;              /* clear d */
	for(i=0;i<l;i++)		/* loop multiplication */
		{
		t=u[i];
		k=0;
		for(j=0;j<l;j++)       /* multiply on digit u[i] */
			{
			k=k+t*v[j]+d[j];
			d[j]=(CT_UINT)(k&MINT);
			k>>=(8*SINT);
			}
		k+=d[l];
		d[l]=(CT_UINT)(k&MINT);
		d[l+1]=(CT_UINT)(k>>(8*SINT));
		t=(unsigned long)g1*d[0];
		t&=MINT;
		k=t*g[0]+d[0];
		k>>=(8*SINT);
		for(j=1;j<l;j++)
			{
			k+=t*g[j]+d[j];
			d[j-1]=(CT_UINT)(k&MINT);
			k>>=(8*SINT);
			}
		k+=d[l];
		d[l-1]=(CT_UINT)(k&MINT);
		k>>=(8*SINT);
		k+=d[l+1];
		d[l]=(CT_UINT)(k&MINT);
		d[l+1]=(CT_UINT)(k>>(8*SINT));
		}
	t=MINT;t=t+1;
	while(d[l]!=0)
		{
		for(k=0,i=0;i<l;i++)
			{
			k=((t+d[i])-g[i])-k;
			d[i]=(CT_UINT)(k&MINT);
			k=1-(k>>(8*SINT));
			}
		d[l]-=(CT_UINT)k;
		}
	for(i=0;i<l;i++)w[i]=d[i];
}

#if SINT == 2
static void pack(CT_PTR v, CT_UINT *u, CT_INT l)
{
	CT_INT m, i;
	for( m = 0, i = 0; i < l; i+=2, m++ )
	{
		if( i+1 < l ) u[m] = ((CT_UINT)v[i+1] << 8)|v[i];
		else          u[m] = (CT_UINT)v[i];
	}
}

static void unpack(CT_UINT *v, CT_PTR u, CT_INT l)
/*
	input : v ;
	output  u = v, u - unsigned char, its lenght is l byts.
*/
{
	CT_INT i, m;
	for( m = 0, i = 0; i < l; i+=2, m++ )
	{
		u[i] = (CT_UCHAR)v[m];
		if( i+1 < l ) u[i+1] = (CT_UCHAR)(v[m] >> 8 );
	}
}
#else
#error No function
#endif

void expmod(CT_UINT *r, CT_PTR x1, CT_UINT *v, CT_PTR x2)
/*
    input : v - the database of second exponent;
            x1 - the order of first exponent,its length is LENEXP bytes;
            x2 - the order of second exponent,its length is LENEXP bytes.
    output : r = a^x1 * v^x2 mod p, where original products is
             changed by Mongomery products, a & p - global constant.
*/
{
    CT_INT i,j[2*BLOKS],jn[2*BLOKS],n,nm,k;

    n=-1;
    for(i=0;i<2*BLOKS;i++)j[i]=(8*LENEXP)/BLOKS;
    for(i=0;i<BLOKS;i++)
        {
        stel(&x1[i*(LENEXP/BLOKS)],&j[i],&jn[i],BLOKSIZE);
        if(j[i]>n){n=j[i],nm=i;};
        stel(&x2[i*(LENEXP/BLOKS)],&j[i+BLOKS],&jn[i+BLOKS],2);
        if(j[i+BLOKS]>n){n=j[i+BLOKS];nm=i+BLOKS;};
        }
    if(nm>=BLOKS)
        {
        k=SPRIME*(2*(nm-BLOKS)+jn[nm]);
        for(i=0;i<SPRIME;i++)r[i]=v[i+k];
        stel(&x2[(nm-BLOKS)*(LENEXP/BLOKS)],&j[nm],&jn[nm],2);
        }
    else
        {
        k=SPRIME*((8+8*MEMSIZE)*nm+jn[nm]);
        for(i=0;i<SPRIME;i++)r[i]=a[i+k];
        stel(&x1[nm*(LENEXP/BLOKS)],&j[nm],&jn[nm],BLOKSIZE);
        }
    for(i=0;i<BLOKS;i++)
        {
        if(j[i]==n)
            {
            k=SPRIME*((8+8*MEMSIZE)*i+jn[i]);
            multmod(r,&a[k],r,p,p1,SPRIME);
            stel(&x1[i*(LENEXP/BLOKS)],&j[i],&jn[i],BLOKSIZE);
            }
        if(j[i+BLOKS]==n)
            {
            k=SPRIME*(2*i+jn[i+BLOKS]);
            multmod(r,&v[k],r,p,p1,SPRIME);
            stel(&x2[i*(LENEXP/BLOKS)],&j[i+BLOKS],&jn[i+BLOKS],2);
            }
        }
    for(n--;n>=0;n--)
        {
        multmod(r,r,r,p,p1,SPRIME);
        for(i=0;i<BLOKS;i++)
            {
            if(j[i]==n)
                {
                k=SPRIME*((8+8*MEMSIZE)*i+jn[i]);
                multmod(r,&a[k],r,p,p1,SPRIME);
                stel(&x1[i*(LENEXP/BLOKS)],&j[i],&jn[i],BLOKSIZE);
                }
            if(j[i+BLOKS]==n)
                {
                k=SPRIME*(2*i+jn[i+BLOKS]);
                multmod(r,&v[k],r,p,p1,SPRIME);
                stel(&x2[i*(LENEXP/BLOKS)],&j[i+BLOKS],&jn[i+BLOKS],2);
                }
            }
        }
    return;
}

void stel(CT_PTR x, CT_INT *j, CT_INT *jk, CT_INT l)
{
    CT_INT i,n;
    CT_ULONG s;
    CT_UCHAR t;

    j[0]--;
    if(j[0]<0)return;
    i=j[0]>>3;n=(j[0]&0x7)+1;
    s=1;s=(s<<n)-1;t=(CT_UCHAR)s;
    for(;(i>=0)&&((x[i]&t)==0);i--,t=0xff,n=8);
    if(i<0){j[0]=i;return;};
    s=t&x[i];
    for(t=(t>>1)+1;(t&x[i])==0;t>>=1,n--);
    if((n<l)&&(i>0)){i--;n+=8;s=(s<<8)+x[i];};
    i<<=3;
    if(n>l){i+=n-l;s=s>>(n-l);};
    for(t=(CT_UCHAR)s;(t&1)==0;i++,t>>=1);
    j[0]=i;jk[0]=t>>1;
    return;
}


