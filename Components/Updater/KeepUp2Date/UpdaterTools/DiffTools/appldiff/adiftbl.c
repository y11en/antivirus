#include <stdlib.h>
#include <memory.h>
#include "../shared/differr.h"
#include "../shared/types.h"
#include "../shared/crc32.h"
#include "adiftbl.h"

/* ------------------------------------------------------------------------- */

/* name obfuscation */
#define loopread  adt0

typedef signed   long  off_t;
typedef unsigned char  u_char;
typedef unsigned long  ssize_t;

/* ------------------------------------------------------------------------- */

/* memory stream */
typedef struct s_memfile {
  u_char*  buff;
  ssize_t  bufsize;
  ssize_t  curoffs;
} t_memfile, *p_memfile;

/* inlined memcpy() */
#define dg_memcpy(destination, source, count) { \
  char* dgmcdst = (char*)(destination);         \
  char* dgmcsrc = (char*)(source);              \
  unsigned int dgmccnt = (unsigned int)(count); \
  while( dgmccnt-- ) *dgmcdst++ = *dgmcsrc++;   \
}

/* read block from memory stream */
static ssize_t loopread(t_memfile* fil, u_char* buf, ssize_t cnt)
{
  if ( (fil->curoffs + cnt) > fil->bufsize )
    cnt -= (cnt - (fil->bufsize - fil->curoffs));

  dg_memcpy(buf, &fil->buff[fil->curoffs], cnt);
  fil->curoffs += cnt;
  return(cnt);
}

/* decode big-endian signed dword */
#define offtin(buf) (off_t)(((unsigned char*)(buf))[3] |               \
  (((unsigned char*)(buf))[2]<<8) | (((unsigned char*)(buf))[1]<<16) | \
  (((unsigned char*)(buf))[0]<<24))

/* ------------------------------------------------------------------------- */

tINT ApplyDiffTable(tBYTE*  Old, tUINT  OldSize, tBYTE* Dif, tUINT DifSize,
                    tBYTE** New, tUINT* NewSize)
{
  t_memfile fil;
  t_memfile ctrlpipe;
  t_memfile diffpipe;
  t_memfile extrpipe;
  u_char    buf[16];
  ssize_t   oldsize,newsize;
  ssize_t   bzctrllen,bzdatalen,bzextralen;
  u_char    *old, *enew;
  off_t     oldpos,newpos;
  off_t     ctrl[3];
  off_t     lenread;
  off_t     i;

  /* check paramethers */
  if ( NULL == Old || !OldSize || NULL == Dif || !DifSize )
    return(DIFF_ERR_PARAM);
  if ( NULL == New || NULL == NewSize )
    return(DIFF_ERR_PARAM);
  *New = NULL; *NewSize = 0;

  /* init mem file */
  fil.buff    = Dif;
  fil.bufsize = DifSize;
  fil.curoffs = 0;

  /* read diff header */
  if ( 16 != loopread(&fil, buf, 16) )
    return(DIFF_ERR_CORRUPT);

  /* diff table header format      */
  /* +00 [dword] : ctrl block len  */
  /* +04 [dword] : new file len    */
  /* +08 [dword] : diff block len  */
  /* +12 [dword] : extra block len */
  bzctrllen  = (ssize_t)offtin(buf);
  newsize    = (ssize_t)offtin(buf + 4);
  bzdatalen  = (ssize_t)offtin(buf + 8);
  bzextralen = (ssize_t)offtin(buf + 12);

  /* check total diff size */
  if ( (bzctrllen + bzdatalen + bzextralen + 16) > DifSize )
    return(DIFF_ERR_CORRUPT);

  /* init mem files */
  ctrlpipe.buff    = (u_char*)(Dif + 16);
  ctrlpipe.bufsize = bzctrllen;
  ctrlpipe.curoffs = 0;
  diffpipe.buff    = (u_char*)(Dif + 16 + bzctrllen);
  diffpipe.bufsize = bzdatalen;
  diffpipe.curoffs = 0;
  extrpipe.buff    = (u_char*)(Dif + 16 + bzctrllen + bzdatalen);
  extrpipe.bufsize = bzextralen;
  extrpipe.curoffs = 0;

  /* alloc new buffer */
  if ( NULL == (enew = (u_char*)malloc(newsize+16)) )
    return(DIFF_ERR_NOMEM);

  old = Old;
  oldsize = OldSize;

  /* apply patch */
  oldpos = 0; newpos = 0;
  while( (ssize_t)newpos < newsize )
  {
    for( i = 0; i < 3; i++ )
    {
      if ( 4 != (lenread = loopread(&ctrlpipe, buf, 4)) )
      {
        free(enew); enew = NULL;
        return(DIFF_ERR_CORRUPT);
      }
      ctrl[i]=offtin(buf);
    }

    if( (ssize_t)(newpos + ctrl[0]) > newsize )
    {
      free(enew); enew = NULL;
      return(DIFF_ERR_CORRUPT);
    }
    if( ctrl[0] != (lenread = loopread(&diffpipe, enew+newpos, ctrl[0])) )
    {
      free(enew); enew = NULL;
      return(DIFF_ERR_CORRUPT);
    }

    for(i=0;i<ctrl[0];i++)
      if( (oldpos+i>=0) && (oldpos+i<(off_t)oldsize))
        enew[newpos+i]+=old[oldpos+i];
    newpos+=ctrl[0];
    oldpos+=ctrl[0];

    if( (ssize_t)(newpos+ctrl[1]) > newsize )
    {
      free(enew); enew = NULL;
      return(DIFF_ERR_CORRUPT);
    }
    if( ctrl[1] != (lenread = loopread(&extrpipe, enew+newpos, ctrl[1])) )
    {
      free(enew); enew = NULL;
      return(DIFF_ERR_CORRUPT);
    }

    newpos+=ctrl[1];
    oldpos+=ctrl[2];
  }

  *New = enew;
  *NewSize = newsize;
  return(DIFF_ERR_OK); /* all ok */
}

/* ------------------------------------------------------------------------- */
