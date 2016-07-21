#include <stdlib.h>
#include <memory.h>
#include "../shared/differr.h"
#include "../shared/types.h"
#include "../shared/crc32.h"
#include "mdiftbl.h"

/* ------------------------------------------------------------------------- */

#ifndef MIN
# define MIN(x,y) (((x)<(y)) ? (x) : (y))
#endif
#ifndef MAX
# define MAX(x,y) (((x)>(y)) ? (x) : (y))
#endif

typedef signed   long   off_t;
typedef unsigned char   u_char;

/* name obfuscation */
#define split     mdt0
#define offtout   mdt1
#define qsufsort  mdt2
#define matchlen  mdt3
#define search    mdt4
#define fmemwrt   mdt5

static void split(off_t *I,off_t *V,off_t start,off_t len,off_t h)
{
	off_t i,j,k,x,tmp,jj,kk;

	if(len<16) {
		for(k=start;k<start+len;k+=j) {
			j=1;x=V[I[k]+h];
			for(i=1;k+i<start+len;i++) {
				if(V[I[k+i]+h]<x) {
					x=V[I[k+i]+h];
					j=0;
				};
				if(V[I[k+i]+h]==x) {
					tmp=I[k+j];I[k+j]=I[k+i];I[k+i]=tmp;
					j++;
				};
			};
			for(i=0;i<j;i++) V[I[k+i]]=k+j-1;
			if(j==1) I[k]=-1;
		};
		return;
	};

	x=V[I[start+len/2]+h];
	jj=0;kk=0;
	for(i=start;i<start+len;i++) {
		if(V[I[i]+h]<x) jj++;
		if(V[I[i]+h]==x) kk++;
	};
	jj+=start;kk+=jj;

	i=start;j=0;k=0;
	while(i<jj) {
		if(V[I[i]+h]<x) {
			i++;
		} else if(V[I[i]+h]==x) {
			tmp=I[i];I[i]=I[jj+j];I[jj+j]=tmp;
			j++;
		} else {
			tmp=I[i];I[i]=I[kk+k];I[kk+k]=tmp;
			k++;
		};
	};

	while(jj+j<kk) {
		if(V[I[jj+j]+h]==x) {
			j++;
		} else {
			tmp=I[jj+j];I[jj+j]=I[kk+k];I[kk+k]=tmp;
			k++;
		};
	};

	if(jj>start) split(I,V,start,jj-start,h);

	for(i=0;i<kk-jj;i++) V[I[jj+i]]=kk-1;
	if(jj==kk-1) I[jj]=-1;

	if(start+len>kk) split(I,V,kk,start+len-kk,h);
}

/* ------------------------------------------------------------------------- */

static void qsufsort(off_t *I,off_t *V,u_char *old,off_t oldsize)
{
	off_t buckets[256];
	off_t i,h,len;

	for(i=0;i<256;i++) buckets[i]=0;
	for(i=0;i<oldsize;i++) buckets[old[i]]++;
	for(i=1;i<256;i++) buckets[i]+=buckets[i-1];
	for(i=255;i>0;i--) buckets[i]=buckets[i-1];
	buckets[0]=0;

	for(i=0;i<oldsize;i++) I[++buckets[old[i]]]=i;
	I[0]=oldsize;
	for(i=0;i<oldsize;i++) V[i]=buckets[old[i]];
	V[oldsize]=0;
	for(i=1;i<256;i++) if(buckets[i]==buckets[i-1]+1) I[buckets[i]]=-1;
	I[0]=-1;

	for(h=1;I[0]!=-(oldsize+1);h+=h) {
		len=0;
		for(i=0;i<oldsize+1;) {
			if(I[i]<0) {
				len-=I[i];
				i-=I[i];
			} else {
				if(len) I[i-len]=-len;
				len=V[I[i]]+1-i;
				split(I,V,i,len,h);
				i+=len;
				len=0;
			};
		};
		if(len) I[i-len]=-len;
	};

	for(i=0;i<oldsize+1;i++) I[V[i]]=i;
}

/* ------------------------------------------------------------------------- */

static off_t matchlen(u_char *old,off_t oldsize,u_char *enew,off_t newsize)
{
	off_t i;

	for(i=0;(i<oldsize)&&(i<newsize);i++)
		if(old[i]!=enew[i]) break;

	return i;
}

/* ------------------------------------------------------------------------- */

static off_t search(off_t *I,u_char *old,off_t oldsize,
		u_char *enew,off_t newsize,off_t st,off_t en,off_t *pos)
{
	off_t x,y;

	if(en-st<2) {
		x=matchlen(old+I[st],oldsize-I[st],enew,newsize);
		y=matchlen(old+I[en],oldsize-I[en],enew,newsize);

		if(x>y) {
			*pos=I[st];
			return x;
		} else {
			*pos=I[en];
			return y;
		}
	};

	x=st+(en-st)/2;
	if(memcmp(old+I[x],enew,MIN(oldsize-I[x],newsize))<0) {
		return search(I,old,oldsize,enew,newsize,x,en,pos);
	} else {
		return search(I,old,oldsize,enew,newsize,st,x,pos);
	};
}

/* ------------------------------------------------------------------------- */

static void offtout(off_t x, u_char* buf)
{
  buf[3] = (u_char)(x);
  buf[2] = (u_char)(x >> 8);
  buf[1] = (u_char)(x >> 16);
  buf[0] = (u_char)(x >> 24);
}

/* ------------------------------------------------------------------------- */

typedef struct s_memfile {
  u_char* buff;
  off_t   bufsize;
  off_t   curofs;
} t_memfile, *p_memfile;

#define MFIL_BLKADD  (1024*1024)

static int fmemwrt(t_memfile* fil, u_char* buf, int cnt)
{
  if ( (fil->curofs + cnt) >= fil->bufsize )
  {
    fil->bufsize += MFIL_BLKADD;
    fil->buff = (u_char*)realloc(fil->buff, fil->bufsize + 16);
  }
  if ( NULL == fil->buff )
    return(0);

  memcpy(fil->buff + fil->curofs, buf, cnt);
  fil->curofs += cnt;
  return(cnt);
}

/* ------------------------------------------------------------------------- */

tINT MakeDiffTable(tBYTE*  Old, tUINT  OldSize, tBYTE* New, tUINT NewSize,
                   tBYTE** Dif, tUINT* DifSize)
{
	u_char *old,*enew;
	off_t  oldsize,newsize;
	off_t  *I,*V;
	off_t  scan,pos,len;
	off_t  lastscan,lastpos,lastoffset;
	off_t  oldscore,scsc;
	off_t  s,Sf,lenf,Sb,lenb;
	off_t  overlap,Ss,lens;
	off_t  i;
	off_t  dblen,eblen;
	u_char *db,*eb;
  u_char buf[16];
  t_memfile fil;

  /* check paramethers */
  if ( NULL == Old || !OldSize || NULL == New || !NewSize )
    return(DIFF_ERR_PARAM);
  if ( NULL == Dif || NULL == DifSize )
    return(DIFF_ERR_PARAM);
  *Dif = NULL; *DifSize = 0;

  old = Old;
  oldsize = OldSize;

  /* alloc tables */
  if ( NULL == (I = (off_t*)malloc((oldsize+1) * sizeof(off_t))) )
    return(DIFF_ERR_NOMEM);
  if ( NULL == (V = (off_t*)malloc((oldsize+1) * sizeof(off_t))) )
  {
    free(I); I = NULL;
    return(DIFF_ERR_NOMEM);
  }

  /* prebuild table */
	qsufsort(I, V, old, oldsize);
	free(V); V = NULL;

  enew = New;
  newsize = NewSize;

  if ( NULL == (db = (u_char*)malloc(newsize+1)) )
  {
    free(I); I = NULL;
    return(DIFF_ERR_NOMEM);
  }
  if ( NULL == (eb = (u_char*)malloc(newsize+1)) )
  {
    free(db); db = NULL;
    free(I);  I  = NULL;
    return(DIFF_ERR_NOMEM);
  }

	dblen = 0;
	eblen = 0;

  /* create memory file */
  memset(&fil, 0, sizeof(fil));
  len = (off_t)MAX(16, 2*newsize);
  if ( NULL == (fil.buff = (u_char*)malloc(len+16)) )
  {
    free(eb); eb = NULL;
    free(db); db = NULL;
    free(I);  I  = NULL;
    return(DIFF_ERR_NOMEM);
  }
  fil.bufsize = len;
  fil.curofs = 16; /* header size */

	scan = 0; len = 0;
	lastscan = 0; lastpos = 0; lastoffset = 0;
	while(scan<newsize)
  {
		oldscore=0;

		for(scsc=scan+=len;scan<newsize;scan++) {
			len=search(I,old,oldsize,enew+scan,newsize-scan,
					0,oldsize,&pos);

			for(;scsc<scan+len;scsc++)
			if((scsc+lastoffset<oldsize) &&
				(old[scsc+lastoffset] == enew[scsc]))
				oldscore++;

			if(((len==oldscore) && (len!=0)) || 
				(len>oldscore+8)) break;

			if((scan+lastoffset<oldsize) &&
				(old[scan+lastoffset] == enew[scan]))
				oldscore--;
		};

		if((len!=oldscore) || (scan==newsize))
    {
			s=0;Sf=0;lenf=0;
			for(i=0;(lastscan+i<scan)&&(lastpos+i<oldsize);) {
				if(old[lastpos+i] == enew[lastscan+i]) s++;
				i++;
				if(s*2-i>Sf*2-lenf) { Sf=s; lenf=i; };
			};

			lenb=0;
			if(scan<newsize) {
				s=0;Sb=0;
				for(i=1;(scan>=lastscan+i)&&(pos>=i);i++) {
					if(old[pos-i]==enew[scan-i]) s++;
					if(s*2-i>Sb*2-lenb) { Sb=s; lenb=i; };
				};
			};

			if(lastscan+lenf>scan-lenb) {
				overlap=(lastscan+lenf)-(scan-lenb);
				s=0;Ss=0;lens=0;
				for(i=0;i<overlap;i++) {
					if(enew[lastscan+lenf-overlap+i]==
					   old[lastpos+lenf-overlap+i]) s++;
					if(enew[scan-lenb+i]==
					   old[pos-lenb+i]) s--;
					if(s>Ss) { Ss=s; lens=i+1; };
				};

				lenf+=lens-overlap;
				lenb-=lens;
			};

			for(i=0;i<lenf;i++)
				db[dblen+i]=enew[lastscan+i]-old[lastpos+i];
			for(i=0;i<(scan-lenb)-(lastscan+lenf);i++)
				eb[eblen+i]=enew[lastscan+lenf+i];

			dblen+=lenf;
			eblen+=(scan-lenb)-(lastscan+lenf);

			offtout(lenf,&buf[0]);
			offtout((scan-lenb)-(lastscan+lenf),&buf[4]);
			offtout((pos-lenb)-(lastpos+lenf),&buf[8]);
      if ( 12 != fmemwrt(&fil, buf, 12) )
      {
        free(fil.buff);
        fil.buff = NULL;
        free(eb); eb = NULL;
        free(db); db = NULL;
        free(I);  I  = NULL;
        return(DIFF_ERR_NOMEM);
      }

			lastscan=scan-lenb;
			lastpos=pos-lenb;
			lastoffset=pos-scan;
		}
	}

  /* diff data header format       */
  /* +00 [dword] : ctrl block len  */
  /* +04 [dword] : new file len    */
  /* +08 [dword] : diff block len  */
  /* +12 [dword] : extra block len */
  
  len = fil.curofs;
	offtout(len-16,  &buf[0]);
  offtout(newsize, &buf[4]);
  offtout(dblen,   &buf[8]);
  offtout(eblen,   &buf[12]);
  memcpy(fil.buff, buf, 16);
  if ( dblen != fmemwrt(&fil, db, dblen) ||
       eblen != fmemwrt(&fil, eb, eblen) )
  {
    free(fil.buff);
    fil.buff = NULL;
    free(eb); eb = NULL;
    free(db); db = NULL;
    free(I);  I  = NULL;
    return(DIFF_ERR_NOMEM);
  }

  free(eb); eb = NULL;
  free(db); db = NULL;
  free(I);  I  = NULL;

  *Dif = fil.buff;
  *DifSize = fil.curofs;
  return(DIFF_ERR_OK); /* all done */
}

/* ------------------------------------------------------------------------- */
