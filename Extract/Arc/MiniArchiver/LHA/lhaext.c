#include "lhaext.h"

#include <Prague/prague.h>
#include <Prague/pr_oid.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_seqio.h>

#include <Extract/iface/i_uacall.h>
#include <Extract/plugin/p_lha.h>

#include <memory.h>
#include <string.h>

/* ------------------------------------------------------------------------- */

tERROR LHA_Rotate(tLHA_CTX* LhaCtx)
{
  if ( NULL == LhaCtx->IO.hSeqIn || NULL == LhaCtx->hParent )
    return(errOPERATION_CANCELED); /* not inited */

  if ( errOPERATION_CANCELED == CALL_SYS_SendMsg(LhaCtx->IO.hSeqIn,
    pmc_PROCESSING, pm_PROCESSING_YIELD, LhaCtx->hParent, 0 ,0) )
  {
    return(errOPERATION_CANCELED);
  }

  return(errOK);
}

/* ------------------------------------------------------------------------- */

tERROR LHA_InitFile(tIO_CTX* IoCtx, hSEQ_IO hSeqIn, hSEQ_IO hSeqOut)
{
  tERROR Error;

  if ( !IoCtx || !hSeqIn )
    return(errPARAMETER_INVALID);

  IoCtx->hSeqIn = hSeqIn;
  IoCtx->Buff   = IoCtx->Lha->RdBuf;
  IoCtx->Limit  = RD_BUFLEN;
  IoCtx->Rdt    = 0;
  IoCtx->BPos   = 0;
  IoCtx->REOF   = cFALSE;
  IoCtx->NoRdt  = cFALSE;

  IoCtx->hSeqOut = hSeqOut;
  IoCtx->WBuf    = IoCtx->Lha->WrBuf;
  IoCtx->WBufLen = WR_BUFLEN;
  IoCtx->WBufPos = 0;

  Error = CALL_SeqIO_Seek(hSeqIn, &IoCtx->FPos, 0, cSEEK_FORWARD);
  if ( !PR_SUCC(Error) )
    return(Error);

  return(errOK);
}

/* ------------------------------------------------------------------------- */

static tERROR LHA_UpdateInput(tIO_CTX* IoCtx)
{
  tERROR Error;
  tDWORD Result;
  tDWORD Cnt;

  IoCtx->Rdt = 0;
  if ( IoCtx->NoRdt )
  {
    /* disabled */
    IoCtx->REOF = cTRUE;
    return(errOBJECT_INVALID);
  }

  /* send yeld message */
  if ( PR_FAIL(Error = LHA_Rotate(IoCtx->Lha)) )
    return(Error);

  /* get current position */
  Error = CALL_SeqIO_Seek(IoCtx->hSeqIn, &IoCtx->FPos, 0, cSEEK_FORWARD);
  if ( !PR_SUCC(Error) )
    return(Error);

  /* read next block */
  Result = 0; Cnt = IoCtx->Limit;
  Error  = CALL_SeqIO_Read(IoCtx->hSeqIn, &Result, IoCtx->Buff, Cnt);
  if ( PR_FAIL(Error) && ((Error != errOUT_OF_OBJECT) || (!Result && Cnt)) )
    return(errOUT_OF_OBJECT);

  IoCtx->BPos = 0;
  IoCtx->Rdt  = Result;
  return(errOK);
}

/* ------------------------------------------------------------------------- */

static tBYTE LHA_GetByte(tIO_CTX* IoCtx)
{
  tERROR Error;

  if ( IoCtx->REOF )
    return(0); /* end of file */
  
  if ( IoCtx->BPos >= IoCtx->Rdt )
  {
    if ( errOK != (Error = LHA_UpdateInput(IoCtx)) )
      IoCtx->Lha->Error = Error; /* save error */
  }

  if ( !IoCtx->Rdt )
  {
    IoCtx->REOF = (cTRUE);
    return(0); /* no more input */
  }

  return(IoCtx->Buff[IoCtx->BPos++]);
}

/* ------------------------------------------------------------------------- */

tQWORD LHA_Seek(tIO_CTX* IoCtx, tQWORD Pos)
{
  tERROR Error;

  if ( Pos >= IoCtx->FPos && Pos < (IoCtx->FPos + IoCtx->Rdt) )
  {
    IoCtx->BPos = (tDWORD)(Pos - IoCtx->FPos);
  }
  else
  {
    IoCtx->Rdt  = 0;
    IoCtx->BPos = 0;
    Error = CALL_SeqIO_Seek(IoCtx->hSeqIn, &IoCtx->FPos, Pos, cSEEK_SET);
    if ( !PR_SUCC(Error) || IoCtx->FPos != Pos )
    {
      /* seek out of file */
      IoCtx->REOF = (cTRUE);
    }
  }

  return(IoCtx->FPos + IoCtx->BPos);
}

/* ------------------------------------------------------------------------- */

tQWORD LHA_Tell(tIO_CTX* IoCtx)
{
  return(IoCtx->BPos + IoCtx->FPos);
}

/* ------------------------------------------------------------------------- */

tDWORD LHA_ReadBlock(tIO_CTX* IoCtx, tVOID* Buff, tUINT Count)
{
  tBYTE* Buf;
  tDWORD Cnt;
  tDWORD Rdt;

  Rdt = 0;
  Buf = (tBYTE*)(Buff);
  while ( Count && !IoCtx->REOF )
  {
    if ( IoCtx->BPos >= IoCtx->Rdt )
      if ( errOK != LHA_UpdateInput(IoCtx) )
        break;

    if ( !(Cnt = Min(Count, (IoCtx->Rdt - IoCtx->BPos))) )
      break;
    memcpy(&Buf[0], &IoCtx->Buff[IoCtx->BPos], Cnt);

    IoCtx->BPos += Cnt;
    Count -= Cnt;
    Buf   += Cnt;
    Rdt   += Cnt;
  }

  return(Rdt);
}

/* ------------------------------------------------------------------------- */

static tERROR LHA_FlushWBuf(tIO_CTX* IoCtx)
{
  tERROR Error;
  tDWORD Wrt;

  Error = CALL_SeqIO_Write(IoCtx->hSeqOut, &Wrt, IoCtx->WBuf, IoCtx->WBufPos);
  if ( PR_FAIL(Error) )
  {
    IoCtx->Lha->Error = Error;
    return(Error);
  }

  IoCtx->WBufPos = 0;
  Error = CALL_SeqIO_Flush(IoCtx->hSeqOut);
  return(errOK);
}

/* ------------------------------------------------------------------------- */

static int LHA_PutByte(tIO_CTX* IoCtx, tBYTE b)
{
  if ( IoCtx->WBufPos >= IoCtx->WBufLen )
    if ( PR_FAIL(LHA_FlushWBuf(IoCtx)) )
      return(0);

  IoCtx->WBuf[IoCtx->WBufPos++] = b;
  return(1);
}

/* ------------------------------------------------------------------------- */

#define get_word(a)        lha_get_word(&a)
#define put_word(a,b)      lha_put_word(&a,b)
#define get_longword(a)    lha_get_longword(&a)
#define put_longword(a,b)  lha_put_longword(&a,b)

/* ------------------------------------------------------------------------- */

static tWORD lha_get_word(tBYTE** g_ptr)
{
  tBYTE* get_ptr = *(g_ptr);
  tUINT  b0, b1;

  b0 = get_byte();
  b1 = get_byte();

  *(g_ptr) = get_ptr;
  return (tWORD)((b1 << 8) + b0);
}

static tVOID lha_put_word(tBYTE** g_ptr, tWORD v)
{
  tBYTE* get_ptr = *(g_ptr);

  put_byte(v);
  put_byte(v >> 8);

  *(g_ptr) = get_ptr;
}

static tDWORD lha_get_longword(tBYTE** g_ptr)
{
  tUINT b0, b1, b2, b3;
  tBYTE* get_ptr = *(g_ptr);

  b0 = get_byte();
  b1 = get_byte();
  b2 = get_byte();
  b3 = get_byte();

  *(g_ptr) = get_ptr;
  return(tUINT)((b3 << 24) + (b2 << 16) + (b1 << 8) + b0);
}

static tVOID lha_put_longword(tBYTE** g_ptr, tDWORD v)
{
  tBYTE* get_ptr = *(g_ptr);

  put_byte(v);
  put_byte(v >> 8);
  put_byte(v >> 16);
  put_byte(v >> 24);

  *(g_ptr) = get_ptr;
}

/* ------------------------------------------------------------------------- */

/* directory conversion */
static tBYTE* convdelim(tBYTE* path, tBYTE delim)
{
  tBYTE* p;
  tBYTE  c;

  for ( p = path; (c = *p) != 0; p++ )
  {
    if ( c == '\\' || c == DELIM || c == DELIM2 ) 
    {
      *p = delim;
      path = p + 1;
    }
  }

  return(path);
}

/* ------------------------------------------------------------------------- */

static tBYTE calc_sum(tCHAR* p, tINT len)
{
  tINT sum;

  for (sum = 0; len; len--)
    sum += *p++;

  return(tBYTE)(sum & 0xff);
}

/* ------------------------------------------------------------------------- */

tERROR LHA_GetHeader(tIO_CTX* IO, tLZ_HEADER* hdr)
{
  tLHA_CTX*  LHA;
  tBYTE* ptr;
  tBYTE* data;
  tBYTE* dirname;
  tBYTE* get_ptr;
  tINT   header_size;
  tINT   name_length;
  tINT   extend_size;
  tINT   dir_length = 0;
  tINT   checksum;
  tINT   dmy;
  tINT   i;

  LHA  = IO->Lha;
  data = &LHA->TmBuf[0];
  dirname = &LHA->TmBuf[LZHEADER_STRAGE+16];
  hdr->hdr_parse_fail = 0;

  memset(hdr, 0, sizeof(tLZ_HEADER));
  if ( IO->REOF || !(header_size = LHA_GetByte(IO)) )
    return(errNOT_OK); /* done */

  if ( IO->NoRdt && header_size >= (tINT)(IO->Rdt - IO->BPos) )
    return(errNOT_OK); /* buffer overrun */

  if ( LHA_ReadBlock(IO, &data[I_HEADER_CHECKSUM], (header_size - 1)) !=
       (tDWORD)(header_size - 1) )
  {
    /* invalid header (LHarc file ?) */
    return(errNOT_OK);  /* finish */
  }

  setup_get(&data[I_HEADER_LEVEL]);
  hdr->header_level = get_byte();
  if ( hdr->header_level != 2 && 2 != LHA_ReadBlock(IO,&data[header_size],2) )
  {
    /* invalid header (LHarc file ?) */
    return(errNOT_OK); /* finish */
  }

  if ( hdr->header_level >= 3 )
  {
    /* unknown level header */
    return(errNOT_OK);
  }

  setup_get(&data[I_HEADER_CHECKSUM]);
  hdr->hdr_crc_len = header_size;
  checksum = get_byte();

  if ( hdr->header_level == 2 )
  {
    hdr->header_size = header_size + checksum * 256;
  }
  else
  {
    hdr->header_size = header_size;
  }

  memcpy(hdr->method, &data[I_METHOD], METHOD_TYPE_STRAGE);
  setup_get(&data[I_PACKED_SIZE]);
  hdr->packed_size   = get_longword(get_ptr);
  hdr->original_size = get_longword(get_ptr);
  hdr->last_modified_stamp = get_longword(get_ptr);
  hdr->attribute = get_byte();

  if ( (hdr->header_level = get_byte()) != 2 )
  {
    if ( calc_sum(&data[I_METHOD], header_size) != checksum )
    {
      /* header checksum error (LHarc file?) */
      /* return(errNOT_OK); */
      hdr->hdr_parse_fail = 1;
    }

    name_length = get_byte();
    hdr->name_length = name_length;
    for ( i = 0; i < name_length; i++ )
      hdr->name[i] = (tBYTE)get_byte();
    hdr->name[name_length] = '\0';
  }
  else
  {
    hdr->unix_last_modified_stamp = hdr->last_modified_stamp;
    name_length = 0;
  }

  /* defaults for other type (unused) */
  hdr->unix_mode = (UNIX_FILE_REGULAR | UNIX_RW_RW_RW);
  hdr->unix_gid  = 0;
  hdr->unix_uid  = 0;

  if ( hdr->header_level == 0 )
  {
    extend_size = header_size - name_length -22;
    if ( extend_size < 0 )
    {
      if ( extend_size == -2 )
      {
        hdr->extend_type = EXTEND_GENERIC;
        hdr->has_crc = (cFALSE);
      } 
      else
      {
        /* unkonwn header (lha file?)" */
        return(errNOT_OK);
      }
    }
    else
    {
      hdr->has_crc = (cTRUE);
      hdr->crc = get_word(get_ptr);
    }

    if ( extend_size >= 1 )
    {
      hdr->extend_type = get_byte();
      extend_size--;
    }
    if ( hdr->extend_type == EXTEND_UNIX )
    {
      if ( extend_size >= 11 )
      {
        hdr->minor_version = get_byte();
        hdr->unix_last_modified_stamp = (long)get_longword(get_ptr);
        hdr->unix_mode = get_word(get_ptr);
        hdr->unix_uid  = get_word(get_ptr);
        hdr->unix_gid  = get_word(get_ptr);
        extend_size -= 11;
      } 
      else
      {
        hdr->extend_type = EXTEND_GENERIC;
      }
    }

    while ( extend_size-- > 0 )
    {
      if ( get_ptr >= &data[LZHEADER_STRAGE] )
        return(errNOT_OK); /* overrun */

      dmy = get_byte();
    }

    if ( hdr->extend_type == EXTEND_UNIX )
      return(errOK);
  } 
  else if ( hdr->header_level == 1 )
  {
    hdr->has_crc = cTRUE;
    extend_size = header_size - name_length-25;
    hdr->crc    = get_word(get_ptr);
    hdr->extend_type = get_byte();
    
    while (extend_size-- > 0)
    {
      if ( get_ptr >= &data[LZHEADER_STRAGE] )
        return(errNOT_OK); /* overrun */

      dmy = get_byte();
    }
  } 
  else 
  { 
    /* level 2 */
    hdr->has_crc = cTRUE;
    hdr->crc = get_word(get_ptr);
    hdr->extend_type = get_byte();
  }   

  hdr->hdr_ext_len = 0;
  if ( hdr->header_level > 0 )
  {
    /* extend header */
    if (hdr->header_level != 2)
      setup_get(data + hdr->header_size);
    ptr = get_ptr;
    while ( (header_size = get_word(get_ptr)) != 0 )
    {
      if ( hdr->header_level != 2 )
      { 
        if ( (&data[LZHEADER_STRAGE] - get_ptr < header_size) || 
             LHA_ReadBlock(IO, get_ptr, header_size) < (tUINT)(header_size) )
        {
          /* invalid header (LHa file ?) */
          return(errNOT_OK);
        }
      }
      if ( (&data[LZHEADER_STRAGE] - get_ptr) < header_size )
      {
        /* too big header */
        return(errNOT_OK);
      }

      switch (get_byte()) 
      {
      case 0:
        /* header crc */
        setup_get(get_ptr + header_size - 3);
        break;
      case 1:
         /* filename */
        if (  header_size < 3 || (header_size - 3) >= MAX_LHA_NAME )
          return(errNOT_OK); /* buffer overrun */
        for ( i = 0; i < header_size - 3; i++ )
        {
          if ( get_ptr >= &data[LZHEADER_STRAGE-1] )
            return(errNOT_OK);
          
          hdr->name[i] = (tBYTE)get_byte();
        }
        hdr->name[header_size - 3] = '\0';
        name_length = header_size - 3;
        break;
      case 2:
         /* directory */
        if (  header_size < 3 || (header_size - 3) >= MAX_LHA_NAME )
          return(errNOT_OK); /* buffer overrun */
        for (i = 0; i < header_size - 3; i++)
          dirname[i] = (tBYTE)get_byte();
        dirname[header_size - 3] = '\0';
        convdelim(dirname, DELIM);
        dir_length = header_size - 3;
        break;
      case 0x40:
        /* MS-DOS attribute */
        if (hdr->extend_type == EXTEND_MSDOS ||
            hdr->extend_type == EXTEND_HUMAN ||
            hdr->extend_type == EXTEND_GENERIC)
          hdr->attribute = (tBYTE)get_word(get_ptr);
        break;
      case 0x50:
        /* UNIX permission */
        if (hdr->extend_type == EXTEND_UNIX)
          hdr->unix_mode = get_word(get_ptr);
        break;
      case 0x51:
        /* UNIX gid and uid */
        if (hdr->extend_type == EXTEND_UNIX) {
          hdr->unix_gid = get_word(get_ptr);
          hdr->unix_uid = get_word(get_ptr);
        }
        break;
      case 0x52:
        /* UNIX group name */
        setup_get(get_ptr + header_size - 3);
        break;
      case 0x53:
        /* UNIX user name */
        setup_get(get_ptr + header_size - 3);
        break;
      case 0x54:
        /* UNIX last modified time */
        if (hdr->extend_type == EXTEND_UNIX)
          hdr->unix_last_modified_stamp = (tLONG)get_longword(get_ptr);
        break;
      default:
         /* other headers */
        setup_get(get_ptr + header_size - 3);
        break;
      }
    }

    if (hdr->header_level != 2 && get_ptr - ptr != 2)
    {
      int len = _toi32(get_ptr - ptr - 2);

      hdr->packed_size -= len;
      hdr->hdr_ext_len += len;
      hdr->header_size += len;
    }
  }

  switch ( hdr->extend_type )
  {
  case EXTEND_MSDOS:
  case EXTEND_HUMAN:
    if (hdr->header_level == 2)
      hdr->unix_last_modified_stamp = hdr->last_modified_stamp;
    break;
  case EXTEND_OS68K:
  case EXTEND_XOSK:
  case EXTEND_UNIX:
    break;
  case EXTEND_MACOS:
    break;
  default:
    if (hdr->header_level == 2)
      hdr->unix_last_modified_stamp = hdr->last_modified_stamp;
  }

  if (dir_length) 
  {
    tINT NLen, DLen;

    NLen = _toi32(strlen((char*)hdr->name));
    DLen = _toi32(strlen((char*)dirname));

    memcpy(&dirname[DLen], hdr->name, NLen);
    memcpy(hdr->name, dirname, DLen + NLen);
    name_length += dir_length;
  }

  return(errOK);
}

/* ------------------------------------------------------------------------- */

#define init_getbits()  lha_init_getbits(Ctx)
#define getbits(n)      lha_getbits(Ctx, (kav_uint8)(n))
#define fillbuf(n)      lha_fillbuf(Ctx, (kav_uint8)(n))

CSTATIC void CATTRIB lha_fillbuf(tLHA_CTX* Ctx, kav_uint8 n)
{
  /* shift bitbuf n bits left, read n bits */
  while ( n > Ctx->Glob.bitcount )
  {
    n -= Ctx->Glob.bitcount;
    Ctx->Glob.bitbuf = (Ctx->Glob.bitbuf << Ctx->Glob.bitcount) + 
      (Ctx->Glob.subbitbuf >> (CHAR_BIT - Ctx->Glob.bitcount));

    if ( Ctx->Glob.compsize != 0 )
    {
      Ctx->Glob.compsize--;
      Ctx->Glob.subbitbuf = (kav_uint8)LHA_GetByte(&Ctx->IO);
    }
    else
      Ctx->Glob.subbitbuf = 0;

    Ctx->Glob.bitcount = CHAR_BIT;
  }
  
  Ctx->Glob.bitcount -= n;
  Ctx->Glob.bitbuf = (Ctx->Glob.bitbuf << n) + 
    (Ctx->Glob.subbitbuf >> (CHAR_BIT - n));
  Ctx->Glob.subbitbuf <<= n;
}

/* ------------------------------------------------------------------------- */

CSTATIC kav_uint16 CATTRIB lha_getbits(tLHA_CTX* Ctx, kav_uint8 n)
{
  kav_uint16 x;

  x = Ctx->Glob.bitbuf >> (2 * CHAR_BIT - n);
  fillbuf(n);
  return x;
}

CSTATIC CINLINE void CATTRIB lha_init_getbits(tLHA_CTX* Ctx)
{
  Ctx->Glob.bitbuf    = 0;
  Ctx->Glob.subbitbuf = 0;
  Ctx->Glob.bitcount  = 0;
  fillbuf(2 * CHAR_BIT);
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB make_table(
  tLHA_CTX*       Ctx,
  kav_sint16      nchar,
  kav_uint8       bitlen[],
  kav_sint16      tablebits,
  kav_uint16      table[],
  kav_uint        tbl_cnt)
{
  kav_uint16   count[17];  /* count of bitlen      */
  kav_uint16   weight[17]; /* 0x10000ul >> bitlen  */
  kav_uint16   start[17];  /* first code of bitlen */
  kav_uint16   total;
  kav_uint     i, l;
  kav_int      j, k, m, n, avail;
  kav_uint16*  p;

  avail = nchar;

  /* initialize */
  for (i = 1; i <= 16; i++) {
    count[i] = 0;
    weight[i] = 1 << (16 - i);
  }

  /* count */
  for (i = 0; i < (kav_uint)(nchar); i++)
  {
    if ( bitlen[i] >= (sizeof(count)/sizeof(count[0])) )
    {
      Ctx->Error = errOBJECT_INVALID;
      return;
    }
    count[bitlen[i]] += 1;
  }

  /* calculate first code */
  total = 0;
  for (i = 1; i <= 16; i++) {
    start[i] = total;
    total += weight[i] * count[i];
  }

  if ((total & 0xffff) != 0)
  {
    /* DEBUG_PRINT(("error in make_table(), bad table")); */
    Ctx->Error = errOBJECT_INVALID;
    return;
  }

  /* shift data for make table */
  m = 16 - tablebits;
  for (i = 1; i <= (unsigned int)(tablebits); i++) {
    start[i] >>= m;
    weight[i] >>= m;
  }

  /* initialize */
  j = start[tablebits + 1] >> m;
  k = 1 << tablebits;
  if (j != 0)
    for (i = j; i < (unsigned int)(k); i++)
      table[i] = 0;

  /* create table and tree */
  for (j = 0; j < nchar; j++) {
    k = bitlen[j];
    if (k == 0)
      continue;
    l = start[k] + weight[k];
    if (k <= tablebits) 
    {
      if ( l > tbl_cnt )
      {
        Ctx->Error = errOBJECT_INVALID;
        return;
      }

      /* code in table */
      for (i = start[k]; i < l; i++)
        table[i] = j;
    }
    else {
      /* code not in table */
      p = &table[(i = start[k]) >> m];
      i <<= tablebits;
      n = k - tablebits;
      /* make tree (n length) */
      while (--n >= 0) {
        if (*p == 0) {
          Ctx->Glob.right[avail] = Ctx->Glob.left[avail] = 0;
          *p = avail++;
        }
        if (i & 0x8000)
          p = &Ctx->Glob.right[*p];
        else
          p = &Ctx->Glob.left[*p];
        i <<= 1;
      }
      *p = j;
    }
    start[k] = l;
  }
}

/* ------------------------------------------------------------------------- */

#undef  NP
#undef  NP2

#define NP      (8 * 1024 / 64)
#define NP2     (NP * 2 - 1)

static const int fixed[2][16] = {
  { 3, 0x01, 0x04, 0x0c, 0x18, 0x30, 0 },             /* old compatible */
  { 2, 0x01, 0x01, 0x03, 0x06, 0x0D, 0x1F, 0x4E, 0 }  /* 8K buf         */
};

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB start_c_dyn(tLHA_CTX* Ctx)
{
  int i, j, f;

  Ctx->DHuf.n1 = (Ctx->Glob.n_max >= 
    (tUINT)(256+Ctx->Glob.maxmatch-THRESHOLD+1)) ? 
    (512) : (Ctx->Glob.n_max - 1);

  for ( i = 0; i < TREESIZE_C; i++ ) 
  {
    Ctx->Glob.stock[i] = i;
    Ctx->Glob.block[i] = 0;
  }
  for (i = 0, j = Ctx->Glob.n_max * 2 - 2; i < (int)Ctx->Glob.n_max; i++, j--)
  {
    Ctx->Glob.freq[j] = 1;
    Ctx->Glob.child[j] = ~i;
    Ctx->Glob.s_node[i] = j;
    Ctx->Glob.block[j] = 1;
  }

  Ctx->DHuf.avail = 2;
  Ctx->Glob.edge[1] = Ctx->Glob.n_max - 1;
  i = Ctx->Glob.n_max * 2 - 2;
  while (j >= 0) 
  {
    f = Ctx->Glob.freq[j] = Ctx->Glob.freq[i] + Ctx->Glob.freq[i-1];
    Ctx->Glob.child[j] = i;
    Ctx->Glob.parent[i] = Ctx->Glob.parent[i-1] = j;
    if (f == Ctx->Glob.freq[j + 1]) 
    {
      Ctx->Glob.edge[Ctx->Glob.block[j] = Ctx->Glob.block[j + 1]] = j;
    }
    else 
    {
      Ctx->Glob.edge[Ctx->Glob.block[j] = 
        Ctx->Glob.stock[Ctx->DHuf.avail++]] = j;
    }
    i -= 2;
    j--;
  }
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB ready_made(tLHA_CTX* Ctx, int method)
{
  int             i, j;
  unsigned int    code, weight;
  const int*      tbl;

  tbl = fixed[method];
  j = *tbl++;
  weight = 1 << (16 - j);
  code = 0;
  for (i = 0; i < (tINT)Ctx->SHuf.np; i++) {
    while (*tbl == i) {
      j++;
      tbl++;
      weight >>= 1;
    }
    Ctx->Glob.pt_len[i] = j;
    Ctx->Glob.pt_code[i] = code;
    code += weight;
  }
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB decode_start_fix(tLHA_CTX* Ctx)
{
  Ctx->Glob.n_max = 314;
  Ctx->Glob.maxmatch = 60;
  init_getbits();
  Ctx->SHuf.np = (1 << (12 - 6));
  start_c_dyn(Ctx);
  ready_made(Ctx, 0);
  make_table(Ctx, (kav_sint16)Ctx->SHuf.np, Ctx->Glob.pt_len, 8, Ctx->Glob.pt_table, 256);
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB reconst(tLHA_CTX* Ctx, tINT start, tINT end)
{
  int             i, j, k, l, b;
  unsigned int    f, g;

  for (i = j = start; i < end; i++)
  {
    if ((k = Ctx->Glob.child[i]) < 0) 
    {
      Ctx->Glob.freq[j] = (Ctx->Glob.freq[i] + 1) / 2;
      Ctx->Glob.child[j] = k;
      j++;
    }
    if (Ctx->Glob.edge[b = Ctx->Glob.block[i]] == i) 
    {
      Ctx->Glob.stock[--Ctx->DHuf.avail] = b;
    }
  }
  j--;
  i = end - 1;
  l = end - 2;
  while (i >= start) 
  {
    while (i >= l) 
    {
      Ctx->Glob.freq[i]  = Ctx->Glob.freq[j];
      Ctx->Glob.child[i] = Ctx->Glob.child[j];
      i--, j--;
    }
    f = Ctx->Glob.freq[l] + Ctx->Glob.freq[l + 1];
    for (k = start; f < Ctx->Glob.freq[k]; k++);
    while (j >= k) 
    {
      Ctx->Glob.freq[i]  = Ctx->Glob.freq[j];
      Ctx->Glob.child[i] = Ctx->Glob.child[j];
      i--, j--;
    }
    Ctx->Glob.freq[i] = f;
    Ctx->Glob.child[i] = l + 1;
    i--;
    l -= 2;
  }
  f = 0;
  for (i = start; i < end; i++) 
  {
    if ((j = Ctx->Glob.child[i]) < 0)
      Ctx->Glob.s_node[~j] = i;
    else
      Ctx->Glob.parent[j] = Ctx->Glob.parent[j - 1] = i;
    if ((g = Ctx->Glob.freq[i]) == f) {
      Ctx->Glob.block[i] = b;
    }
    else 
    {
      Ctx->Glob.edge[b = Ctx->Glob.block[i] = 
        Ctx->Glob.stock[Ctx->DHuf.avail++]] = i;
      f = g;
    }
  }
}

/* ------------------------------------------------------------------------- */

CSTATIC kav_int CATTRIB swap_inc(tLHA_CTX* Ctx, kav_int p)
{
  int b, q, r, s;

  b = Ctx->Glob.block[p];
  if ((q = Ctx->Glob.edge[b]) != p) 
  {
    /* swap for leader */
    r = Ctx->Glob.child[p];
    s = Ctx->Glob.child[q];
    Ctx->Glob.child[p] = s;
    Ctx->Glob.child[q] = r;
    if (r >= 0)
      Ctx->Glob.parent[r] = Ctx->Glob.parent[r - 1] = q;
    else
      Ctx->Glob.s_node[~r] = q;
    if (s >= 0)
      Ctx->Glob.parent[s] = Ctx->Glob.parent[s - 1] = p;
    else
      Ctx->Glob.s_node[~s] = p;
    p = q;
    goto Adjust;
  }
  else if (b == Ctx->Glob.block[p + 1]) {
Adjust:
    Ctx->Glob.edge[b]++;
    if (++Ctx->Glob.freq[p] == Ctx->Glob.freq[p - 1]) {
      Ctx->Glob.block[p] = Ctx->Glob.block[p - 1];
    }
    else
    {
      Ctx->Glob.edge[Ctx->Glob.block[p] = 
        Ctx->Glob.stock[Ctx->DHuf.avail++]] = p; /* create block */
    }
  }
  else if (++Ctx->Glob.freq[p] == Ctx->Glob.freq[p - 1])
  {
    Ctx->Glob.stock[--Ctx->DHuf.avail] = b; /* delete block */
    Ctx->Glob.block[p] = Ctx->Glob.block[p - 1];
  }
  return Ctx->Glob.parent[p];
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB update_c(tLHA_CTX* Ctx, int p)
{
  int q;

  if ( Ctx->Glob.freq[ROOT_C] == 0x8000 )
  {
    reconst(Ctx, 0, Ctx->Glob.n_max * 2 - 1);
  }
  Ctx->Glob.freq[ROOT_C]++;
  q = Ctx->Glob.s_node[p];
  do 
  {
    q = swap_inc(Ctx, q);
  } while (q != ROOT_C);
}

/* ------------------------------------------------------------------------- */

CSTATIC kav_uint16 CATTRIB decode_c_dyn(tLHA_CTX* Ctx)
{
  kav_sint16 buf, cnt;
  int   c;

  c = Ctx->Glob.child[ROOT_C];
  buf = Ctx->Glob.bitbuf;
  cnt = 0;
  do 
  {
    c = Ctx->Glob.child[c - (buf < 0)];
    buf <<= 1;
    if (++cnt == 16) 
    {
      fillbuf(16);
      buf = Ctx->Glob.bitbuf;
      cnt = 0;
    }
  } 
  while (c > 0);
  fillbuf((unsigned char)(cnt));
  c = ~c;
  update_c(Ctx, c);
  if (c == Ctx->DHuf.n1)
    c += getbits(8);
  return c;
}

/* ------------------------------------------------------------------------- */

CSTATIC kav_uint16 CATTRIB decode_p_st0(tLHA_CTX* Ctx)
{
  int i, j;

  j = Ctx->Glob.pt_table[Ctx->Glob.bitbuf >> 8];
  if ( j < (int)Ctx->SHuf.np ) 
  {
    fillbuf(Ctx->Glob.pt_len[j]);
  }
  else 
  {
    fillbuf(8);
    i = Ctx->Glob.bitbuf;
    do 
    {
      if ((short) i < 0)
        j = Ctx->Glob.right[j];
      else
        j = Ctx->Glob.left[j];
      i <<= 1;
    } 
    while (j >= (int)Ctx->SHuf.np);
    fillbuf(Ctx->Glob.pt_len[j] - 8);
  }
  return (j << 6) + getbits(6);
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB update_p(tLHA_CTX* Ctx, kav_int p)
{
  int             q;

  if (Ctx->DHuf.total_p == 0x8000)
  {
    reconst(Ctx, ROOT_P, Ctx->DHuf.most_p + 1);
    Ctx->DHuf.total_p = Ctx->Glob.freq[ROOT_P];
    Ctx->Glob.freq[ROOT_P] = 0xffff;
  }
  q = Ctx->Glob.s_node[p + N_CHAR];
  while (q != ROOT_P) 
  {
    q = swap_inc(Ctx, q);
  }
  Ctx->DHuf.total_p++;
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB make_new_node(tLHA_CTX* Ctx, tINT p)
{
  int             q, r;

  r = Ctx->DHuf.most_p + 1;
  q = r + 1;
  Ctx->Glob.s_node[~(Ctx->Glob.child[r] = 
    Ctx->Glob.child[Ctx->DHuf.most_p])] = r;
  Ctx->Glob.child[q] = ~(p + N_CHAR);
  Ctx->Glob.child[Ctx->DHuf.most_p] = q;
  Ctx->Glob.freq[r] = Ctx->Glob.freq[Ctx->DHuf.most_p];
  Ctx->Glob.freq[q] = 0;
  Ctx->Glob.block[r] = Ctx->Glob.block[Ctx->DHuf.most_p];
  if (Ctx->DHuf.most_p == ROOT_P) 
  {
    Ctx->Glob.freq[ROOT_P] = 0xffff;
    Ctx->Glob.edge[Ctx->Glob.block[ROOT_P]]++;
  }
  Ctx->Glob.parent[r] = Ctx->Glob.parent[q] = Ctx->DHuf.most_p;
  Ctx->Glob.edge[Ctx->Glob.block[q] = Ctx->Glob.stock[Ctx->DHuf.avail++]] =
    Ctx->Glob.s_node[p + N_CHAR] = Ctx->DHuf.most_p = q;

  update_p(Ctx, p);
}


/* ------------------------------------------------------------------------- */

CSTATIC kav_uint16 CATTRIB decode_p_dyn(tLHA_CTX* Ctx)
{
  kav_sint16  buf, cnt;
  int    c;

  while (Ctx->Glob.count > Ctx->DHuf.nextcount) 
  {
    make_new_node(Ctx, Ctx->DHuf.nextcount / 64);
    if ( (kav_sint32)(Ctx->DHuf.nextcount += 64) >= Ctx->DHuf.nn)
      Ctx->DHuf.nextcount = 0xffffffff;
  }
  c = Ctx->Glob.child[ROOT_P];
  buf = Ctx->Glob.bitbuf;
  cnt = 0;
  while (c > 0) 
  {
    c = Ctx->Glob.child[c - (buf < 0)];
    buf <<= 1;
    if (++cnt == 16) {
      fillbuf(16);
      buf = Ctx->Glob.bitbuf;
      cnt = 0;
    }
  }
  fillbuf(cnt);
  c = (~c) - N_CHAR;
  update_p(Ctx, c);

  return (c << 6) + getbits(6);
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB start_p_dyn(tLHA_CTX* Ctx)
{
  Ctx->Glob.freq[ROOT_P] = 1;
  Ctx->Glob.child[ROOT_P] = ~(N_CHAR);
  Ctx->Glob.s_node[N_CHAR] = ROOT_P;
  Ctx->Glob.edge[Ctx->Glob.block[ROOT_P] = 
    Ctx->Glob.stock[Ctx->DHuf.avail++]] = ROOT_P;
  Ctx->DHuf.most_p = ROOT_P;
  Ctx->DHuf.total_p = 0;
  Ctx->DHuf.nn = 1 << Ctx->Glob.dicbit;
  Ctx->DHuf.nextcount = 64;
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB decode_start_dyn(tLHA_CTX* Ctx)
{
  Ctx->Glob.n_max = 286;
  Ctx->Glob.maxmatch = MAXMATCH;
  init_getbits();
  start_c_dyn(Ctx);
  start_p_dyn(Ctx);
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB decode_start_st0(tLHA_CTX* Ctx)
{
  Ctx->Glob.n_max = 286;
  Ctx->Glob.maxmatch = MAXMATCH;
  init_getbits();
  Ctx->SHuf.np = 1 << (LZHUFF3_DICBIT - 6);
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB read_tree_c(tLHA_CTX* Ctx)
{
  /* read tree from file */
  int             i, c;

  i = 0;
  while (i < N1) {
    if (getbits(1))
      Ctx->Glob.c_len[i] = getbits(LENFIELD) + 1;
    else
      Ctx->Glob.c_len[i] = 0;
    if (++i == 3 && Ctx->Glob.c_len[0] == 1 && 
        Ctx->Glob.c_len[1] == 1 && 
        Ctx->Glob.c_len[2] == 1)
    {
      c = getbits(CBIT);
      for (i = 0; i < N1; i++)
        Ctx->Glob.c_len[i] = 0;
      for (i = 0; i < 4096; i++)
        Ctx->Glob.c_table[i] = c;
      return;
    }
  }
  make_table(Ctx, N1, Ctx->Glob.c_len, 12, Ctx->Glob.c_table, 4096);
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB read_tree_p(tLHA_CTX* Ctx)
{
  /* read tree from file */
  int             i, c;

  i = 0;
  while (i < NP) 
  {
    Ctx->Glob.pt_len[i] = (kav_uint8)getbits(LENFIELD);
    if (++i == 3 && Ctx->Glob.pt_len[0] == 1 && 
        Ctx->Glob.pt_len[1] == 1 && 
        Ctx->Glob.pt_len[2] == 1) 
    {
      c = getbits(MAX_DICBIT - 7);
      for (i = 0; i < NP; i++)
        Ctx->Glob.c_len[i] = 0;
      for (i = 0; i < 256; i++)
        Ctx->Glob.c_table[i] = c;
      return;
    }
  }
}

/* ------------------------------------------------------------------------- */

CSTATIC kav_uint16 CATTRIB decode_c_st0(tLHA_CTX* Ctx)
{
  int  i, j;

  if ( Ctx->SHuf.blocksize == 0 )
  { 
    /* read block head */
    Ctx->SHuf.blocksize = getbits(BUFBITS); /* read block blocksize */
    read_tree_c(Ctx);
    if (getbits(1)) 
    {
      read_tree_p(Ctx);
    }
    else 
    {
      ready_made(Ctx, 1);
    }
    make_table(Ctx, NP, Ctx->Glob.pt_len, 8, Ctx->Glob.pt_table, 256);
    if ( errOK != Ctx->Error )
      return(0);
  }
  Ctx->SHuf.blocksize--;
  j = Ctx->Glob.c_table[Ctx->Glob.bitbuf >> 4];
  if (j < N1)
    fillbuf(Ctx->Glob.c_len[j]);
  else {
    fillbuf(12);
    i = Ctx->Glob.bitbuf;
    do {
      if ((short) i < 0)
        j = Ctx->Glob.right[j];
      else
        j = Ctx->Glob.left[j];
      i <<= 1;
    } while (j >= N1);
    fillbuf(Ctx->Glob.c_len[j] - 12);
  }
  if (j == N1 - 1)
    j += getbits(EXTRABITS);
  return j;
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB read_pt_len(
  tLHA_CTX*       Ctx,
  kav_sint16      nn,
  kav_sint16      nbit,
  kav_sint16      i_special)
{
  int           i, c, n;

  n = getbits(nbit);
  if (n == 0) {
    c = getbits(nbit);
    for (i = 0; i < nn; i++)
      Ctx->Glob.pt_len[i] = 0;
    for (i = 0; i < 256; i++)
      Ctx->Glob.pt_table[i] = c;
  }
  else {
    i = 0;
    while (i < n) {
      c = Ctx->Glob.bitbuf >> (16 - 3);
      if (c == 7) 
      {
        kav_uint16  mask = 1 << (16 - 4);
        while (mask & Ctx->Glob.bitbuf) 
        {
          mask >>= 1;
          c++;
        }
      }
      fillbuf((c < 7) ? 3 : c - 3);
      Ctx->Glob.pt_len[i++] = c;
      if (i == i_special) {
        c = getbits(2);
        while (--c >= 0)
          Ctx->Glob.pt_len[i++] = 0;
      }
    }
    while (i < nn)
      Ctx->Glob.pt_len[i++] = 0;
    make_table(Ctx, nn, Ctx->Glob.pt_len, 8, Ctx->Glob.pt_table, 256);
    if ( errOK != Ctx->Error )
      return;
  }
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB read_c_len(tLHA_CTX* Ctx)
{
  kav_sint16 i, c, n;

  n = getbits(CBIT);
  if (n == 0) {
    c = getbits(CBIT);
    for (i = 0; i < NC; i++)
      Ctx->Glob.c_len[i] = 0;
    for (i = 0; i < 4096; i++)
      Ctx->Glob.c_table[i] = c;
  } else {
    i = 0;
    while (i < n) {
      c = Ctx->Glob.pt_table[Ctx->Glob.bitbuf >> (16 - 8)];
      if (c >= NT) {
        kav_uint16 mask = 1 << (16 - 9);
        do {
          if (Ctx->Glob.bitbuf & mask)
            c = Ctx->Glob.right[c];
          else
            c = Ctx->Glob.left[c];
          mask >>= 1;
        } while (c >= NT);
      }
      fillbuf(Ctx->Glob.pt_len[c]);
      if (c <= 2) {
        if (c == 0)
          c = 1;
        else if (c == 1)
          c = getbits(4) + 3;
        else
          c = getbits(CBIT) + 20;
        while (--c >= 0)
          Ctx->Glob.c_len[i++] = 0;
      }
      else
        Ctx->Glob.c_len[i++] = c - 2;
    }
    while (i < NC)
      Ctx->Glob.c_len[i++] = 0;
    make_table(Ctx, NC, Ctx->Glob.c_len, 12, Ctx->Glob.c_table, 4096);
    if ( errOK != Ctx->Error )
      return;
  }
}

/* ------------------------------------------------------------------------- */

CSTATIC kav_uint16 CATTRIB decode_c_st1(tLHA_CTX* Ctx)
{
  kav_uint16 j, mask;

  if ( Ctx->SHuf.blocksize == 0) 
  {
    Ctx->SHuf.blocksize = getbits(16);
    read_pt_len(Ctx, NT, TBIT, 3);
    read_c_len(Ctx);
    read_pt_len(Ctx, (kav_sint16)Ctx->SHuf.np, (kav_sint16)Ctx->SHuf.pbit, -1);
    if ( errOK != Ctx->Error )
      return(0);
  }
  Ctx->SHuf.blocksize--;
  j = Ctx->Glob.c_table[Ctx->Glob.bitbuf >> 4];
  if (j < NC)
    fillbuf(Ctx->Glob.c_len[j]);
  else {
    fillbuf(12);
    mask = 1 << (16 - 1);
    do {
      if (Ctx->Glob.bitbuf & mask)
        j = Ctx->Glob.right[j];
      else
        j = Ctx->Glob.left[j];
      mask >>= 1;
    } while (j >= NC);
    fillbuf(Ctx->Glob.c_len[j] - 12);
  }
  return j;
}

/* ------------------------------------------------------------------------- */

CSTATIC kav_uint16 CATTRIB decode_p_st1(tLHA_CTX* Ctx)
{
  kav_uint16 j, mask;

  j = Ctx->Glob.pt_table[Ctx->Glob.bitbuf >> (16 - 8)];
  if (j < Ctx->SHuf.np)
    fillbuf(Ctx->Glob.pt_len[j]);
  else {
    fillbuf(8);
    mask = 1 << (16 - 1);
    do {
      if (Ctx->Glob.bitbuf & mask)
        j = Ctx->Glob.right[j];
      else
        j = Ctx->Glob.left[j];
      mask >>= 1;
    } while (j >= Ctx->SHuf.np);
    fillbuf(Ctx->Glob.pt_len[j] - 8);
  }
  if (j != 0)
    j = (1 << (j - 1)) + getbits(j - 1);
  return j;
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB decode_start_st1(tLHA_CTX* Ctx)
{
  if (Ctx->Glob.dicbit <= 13)  
  {
    Ctx->SHuf.np = 14;
    Ctx->SHuf.pbit = 4;
  } else {
    if (Ctx->Glob.dicbit == 16)
    {
      Ctx->SHuf.np = 17; /* for -lh7- */
    } else {
      Ctx->SHuf.np = 16;
    }
    Ctx->SHuf.pbit = 5;
  }

  init_getbits();
  Ctx->SHuf.blocksize = 0;
}

/* ------------------------------------------------------------------------- */

CSTATIC kav_uint16 CATTRIB decode_c_lzs(tLHA_CTX* Ctx)
{
  if (getbits(1)) {
    return getbits(8);
  }
  else {
    Ctx->SHuf.matchpos = getbits(11);
    return getbits(4) + 0x100;
  }
}

/* ------------------------------------------------------------------------- */

CSTATIC kav_uint16 CATTRIB decode_p_lzs(tLHA_CTX* Ctx)
{
  return (Ctx->Glob.loc - Ctx->SHuf.matchpos - MAGIC0) & 0x7ff;
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB decode_start_lzs(tLHA_CTX* Ctx)
{
  init_getbits();
}

/* ------------------------------------------------------------------------- */

CSTATIC kav_uint16 CATTRIB decode_c_lz5(tLHA_CTX* Ctx)
{
  int             c;

  if (Ctx->SHuf.flagcnt == 0) 
  {
    Ctx->SHuf.flagcnt = 8;
    Ctx->SHuf.flag = LHA_GetByte(&Ctx->IO);
  }
  Ctx->SHuf.flagcnt--;
  c = LHA_GetByte(&Ctx->IO);
  if ((Ctx->SHuf.flag & 1) == 0) 
  {
    Ctx->SHuf.matchpos = c;
    c = LHA_GetByte(&Ctx->IO);
    Ctx->SHuf.matchpos += (c & 0xf0) << 4;
    c &= 0x0f;
    c += 0x100;
  }
  Ctx->SHuf.flag >>= 1;
  return c;
}

/* ------------------------------------------------------------------------- */

CSTATIC kav_uint16 CATTRIB decode_p_lz5(tLHA_CTX* Ctx)
{
  return (Ctx->Glob.loc - Ctx->SHuf.matchpos - MAGIC5) & 0xfff;
}

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB decode_start_lz5(tLHA_CTX* Ctx)
{
#if 0
  int             i;

  Ctx->SHuf.flagcnt = 0;
  for (i = 0; i < 256; i++)
    memset(&Ctx->Glob.text[i * 13 + 18], i, 13);
  for (i = 0; i < 256; i++)
    Ctx->Glob.text[256 * 13 + 18 + i] = i;
  for (i = 0; i < 256; i++)
    Ctx->Glob.text[256 * 13 + 256 + 18 + i] = 255 - i;
  memset(&Ctx->Glob.text[256 * 13 + 512 + 18], 0, 128);
  memset(&Ctx->Glob.text[256 * 13 + 512 + 128 + 18], ' ', 128 - 18);
#endif
}

/* ------------------------------------------------------------------------- */

/* crc-16 table */
static const kav_uint16 crctable[256] = {
  0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601,
  0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440, 0xCC01, 0x0CC0,
  0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40, 0x0A00, 0xCAC1, 0xCB81,
  0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841, 0xD801, 0x18C0, 0x1980, 0xD941,
  0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01,
  0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0,
  0x1680, 0xD641, 0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081,
  0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
  0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441, 0x3C00,
  0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0,
  0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840, 0x2800, 0xE8C1, 0xE981,
  0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41,
  0x2D00, 0xEDC1, 0xEC81, 0x2C40, 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700,
  0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0,
  0x2080, 0xE041, 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281,
  0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
  0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01,
  0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840, 0x7800, 0xB8C1,
  0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41, 0xBE01, 0x7EC0, 0x7F80,
  0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541,
  0x7700, 0xB7C1, 0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101,
  0x71C0, 0x7080, 0xB041, 0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0,
  0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481,
  0x5440, 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
  0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841, 0x8801,
  0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1,
  0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41, 0x4400, 0x84C1, 0x8581,
  0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0, 0x4380, 0x8341,
  0x4100, 0x81C1, 0x8081, 0x4040
};

CSTATIC kav_uint16 CATTRIB calccrc(tBYTE* p, tUINT n, kav_uint16 crc)
{
  while (n-- > 0)
    UPDATE_CRC(*p++);
  return(crc);
}

/* ------------------------------------------------------------------------- */

static const struct sDECODE_OPTION decode_define[] = {
  { decode_c_dyn, decode_p_st0, decode_start_fix }, /* lh1 */
  { decode_c_dyn, decode_p_dyn, decode_start_dyn }, /* lh2 */
  { decode_c_st0, decode_p_st0, decode_start_st0 }, /* lh3 */
  { decode_c_st1, decode_p_st1, decode_start_st1 }, /* lh4 */
  { decode_c_st1, decode_p_st1, decode_start_st1 }, /* lh5 */
  { decode_c_st1, decode_p_st1, decode_start_st1 }, /* lh6 */
  { decode_c_st1, decode_p_st1, decode_start_st1 }, /* lh7 */
  { decode_c_lzs, decode_p_lzs, decode_start_lzs }, /* lzs */
  { decode_c_lz5, decode_p_lz5, decode_start_lz5 }  /* lz5 */
};

/* ------------------------------------------------------------------------- */

CSTATIC void CATTRIB lha_decode(tLHA_CTX* Ctx, tINTERFACING* interf)
{
  tDECODE_OPTION decode_set;
  kav_uint32  i, j, k, c;
  kav_uint32  dicsiz1, offset;
  kav_uint8*  dtext;
  kav_uint    RotCnt;
  tERROR      Error;

  Ctx->Glob.dicbit   = interf->dicbit;
  Ctx->Glob.origsize = interf->original;
  Ctx->Glob.compsize = interf->packed;
  decode_set = decode_define[interf->method - 1];

  Ctx->Glob.crc = 0;
  Ctx->Glob.prev_char = -1;
  Ctx->Slide.dicsiz   = (1L << Ctx->Glob.dicbit);

  if ( Ctx->WndSize < Ctx->Slide.dicsiz )
  {
    tERROR Error;

    if ( NULL != Ctx->Window )
    {
      CALL_SYS_ObjHeapFree(Ctx->hParent, Ctx->Window);
      Ctx->Window = NULL;
    }
   
    Error = CALL_SYS_ObjHeapAlloc(Ctx->hParent, &Ctx->Window, Ctx->Slide.dicsiz+64);
    if ( PR_FAIL(Error) || NULL == Ctx->Window )
    {
      Ctx->Error = errNOT_ENOUGH_MEMORY;
      return;
    }
  }

  if ( NULL == (dtext = Ctx->Window) )
  {
    /* DEBUG_PRINT(("Can't alloc 0x%X for dtext", Ctx->Slide.dicsiz)); */
    return;
  }

  memset(&dtext[0], 0x20, Ctx->Slide.dicsiz);
  decode_set.decode_start(Ctx);
  if ( errOK != Ctx->Error )
    return;

  dicsiz1 = (Ctx->Slide.dicsiz - 1);
  offset = (interf->method == LARC_METHOD_NUM) ? 0x100 - 2 : 0x100 - 3;
  Ctx->Glob.count = 0;
  Ctx->Glob.loc = 0;

  if ( errOK != (Error = LHA_Rotate(Ctx)) )
  {
    Ctx->Error = errOPERATION_CANCELED;
    return; /* user stopped */
  }

  RotCnt = 0;
  while ((tINT)Ctx->Glob.count < Ctx->Glob.origsize) 
  {
    if ( !(RotCnt++ & 0xffff) && errOK != (Error = LHA_Rotate(Ctx)) )
    {
      Ctx->Error = errOPERATION_CANCELED;
      break; /* user stopped */
    }

    c = decode_set.decode_c(Ctx);
    if ( errOK != Ctx->Error )
      return;

    if ( c <= UCHAR_MAX ) 
    {
      dtext[Ctx->Glob.loc++] = c;
      if (Ctx->Glob.loc == (tUINT)Ctx->Slide.dicsiz)
      {
        tERROR Error;
        tDWORD Wrt;
         
        Error = CALL_SeqIO_Write(Ctx->hWriter, &Wrt, dtext, Ctx->Slide.dicsiz);
        if ( !PR_SUCC(Error) || Wrt != Ctx->Slide.dicsiz )
        {
          Ctx->Error = errNOT_OK;
          return;
        }
        Ctx->Glob.crc = calccrc(dtext, Ctx->Slide.dicsiz, Ctx->Glob.crc);
        Ctx->Glob.loc = 0;
      }
      Ctx->Glob.count++;
    }
    else 
    {
      j = c - offset;
      i = (Ctx->Glob.loc - decode_set.decode_p(Ctx) - 1) & dicsiz1;
      Ctx->Glob.count += j;
      for (k = 0; k < j; k++) 
      {
        c = dtext[(i + k) & dicsiz1];
        dtext[Ctx->Glob.loc++] = c;
        if (Ctx->Glob.loc == Ctx->Slide.dicsiz) 
        {
          tERROR Error;
          tDWORD Wrt;

          Error = CALL_SeqIO_Write(Ctx->hWriter, &Wrt, dtext, Ctx->Slide.dicsiz);
          if ( !PR_SUCC(Error) || Wrt != Ctx->Slide.dicsiz )
          {
            Ctx->Error = errNOT_OK;
            return;
          }
          Ctx->Glob.crc = calccrc(dtext, Ctx->Slide.dicsiz, Ctx->Glob.crc);
          Ctx->Glob.loc = 0;
        }
      }
    }
  }

  if (Ctx->Glob.loc != 0) 
  {
    tERROR Error;
    tDWORD Wrt;

    Error = CALL_SeqIO_Write(Ctx->hWriter, &Wrt, dtext, Ctx->Glob.loc);
    if ( !PR_SUCC(Error) || Wrt != Ctx->Glob.loc )
    {
      Ctx->Error = errNOT_OK;
      return;
    }
    Ctx->Glob.crc = calccrc(dtext, Ctx->Glob.loc, Ctx->Glob.crc);
  }

  interf->crc = Ctx->Glob.crc;
}

/* ------------------------------------------------------------------------- */

/* decompressor methods names */
static const char N_LZHUFF0_METHOD[] = LZHUFF0_METHOD;
static const char N_LZHUFF1_METHOD[] = LZHUFF1_METHOD;
static const char N_LZHUFF2_METHOD[] = LZHUFF2_METHOD;
static const char N_LZHUFF3_METHOD[] = LZHUFF3_METHOD;
static const char N_LZHUFF4_METHOD[] = LZHUFF4_METHOD;
static const char N_LZHUFF5_METHOD[] = LZHUFF5_METHOD;
static const char N_LZHUFF6_METHOD[] = LZHUFF6_METHOD;
static const char N_LZHUFF7_METHOD[] = LZHUFF7_METHOD;
static const char N_LARC_METHOD[]    = LARC_METHOD;
static const char N_LARC5_METHOD[]   = LARC5_METHOD;
static const char N_LARC4_METHOD[]   = LARC4_METHOD;
static const char N_LZHDIRS_METHOD[] = LZHDIRS_METHOD;

/* decompressor methods array */
static const char* const methods[] = {
  N_LZHUFF0_METHOD, N_LZHUFF1_METHOD, N_LZHUFF2_METHOD, N_LZHUFF3_METHOD,
  N_LZHUFF4_METHOD, N_LZHUFF5_METHOD, N_LZHUFF6_METHOD, N_LZHUFF7_METHOD,
  N_LARC_METHOD,    N_LARC5_METHOD,   N_LARC4_METHOD,   N_LZHDIRS_METHOD,
  NULL
};

/* ------------------------------------------------------------------------- */

#define COPYFILE_BUFLEN  TM_BUFLEN

CSTATIC tBOOL CATTRIB copyfile_ph(tIO_CTX* F1, tUINT Size, tUINT* Crc)
{
  kav_uint16 Cnt;
  kav_uint16 crc;
  tBYTE*     Buf;
  tDWORD     Wrt;
  tERROR     Error;

  crc = *(Crc);
  Buf = F1->Lha->TmBuf;
  while( Size > 0 )
  {
    Cnt = (kav_uint16)Min(Size, COPYFILE_BUFLEN);
    if ( Cnt != LHA_ReadBlock(F1, &Buf[0], Cnt) )
    {
      /* DEBUG_PRINT(("Can't read 0x%X of input stream", Cnt)); */
      F1->Lha->Error = errOUT_OF_OBJECT;
      return(cFALSE);
    }

    Error = CALL_SeqIO_Write(F1->Lha->hWriter, &Wrt, Buf, Cnt);
    if ( !PR_SUCC(Error) || Wrt != Cnt )
    {
      /* DEBUG_PRINT(("Can't write 0x%X to output stream", Cnt)); */
      F1->Lha->Error = errNOT_OK;
      return(cFALSE);
    }

    crc = calccrc(Buf, Cnt, crc);
    Size -= Cnt;
  }

  *(Crc) = crc;
  return(cTRUE);
}

/* ------------------------------------------------------------------------- */

CSTATIC kav_sint32 decode_lzhuf(tLHA_CTX* Ctx, kav_uint32 original_size,
                                kav_uint32 packed_size, kav_uint32 method)
{
  tINTERFACING interf;

  interf.method   = method;
  interf.dicbit   = 13;  /* method + 8; "-lh5-" */
  interf.infile   = &Ctx->IO;
  interf.original = original_size;
  interf.packed   = packed_size;
  interf.crc      = 0;

  switch (method) 
  {
  case LZHUFF0_METHOD_NUM:
  case LARC4_METHOD_NUM:
    copyfile_ph(&Ctx->IO, original_size, &interf.crc);
    break;
  case LARC_METHOD_NUM:       /* -lzs- */
    interf.dicbit = 11;
    lha_decode(Ctx, &interf);
    break;
  case LZHUFF1_METHOD_NUM:    /* -lh1- */
  case LZHUFF4_METHOD_NUM:    /* -lh4- */
  case LARC5_METHOD_NUM:      /* -lz5- */
    interf.dicbit = 12;
    lha_decode(Ctx, &interf);
    break;
  case LZHUFF6_METHOD_NUM:    /* -lh6- */
  case LZHUFF7_METHOD_NUM:    /* -lh7- */
    interf.dicbit = (method - LZHUFF6_METHOD_NUM) + 15;
  default:
    lha_decode(Ctx, &interf);
    break;
  }

  return(interf.crc);
}

/* ------------------------------------------------------------------------- */

tERROR LHA_ExtractOne(tLHA_CTX* Ctx, tLZ_HEADER* hdr)
{
  tINT crc;
  tINT method;

  for ( method = 0; ; method++ )
  {
    if ( methods[method] == NULL )
    {
      /* DEBUG_PRINT(("Unknown method skiped: %s", hdr->method)); */
      return(errOBJECT_INVALID);
    }

    if ( memcmp(hdr->method, methods[method], 5) == 0 )
      break;
  }

  hdr->crc_fail = 0;
  crc = decode_lzhuf(Ctx, hdr->original_size, hdr->packed_size, method);
  if ( errOK != Ctx->Error )
    return(Ctx->Error);

  if ( hdr->has_crc && crc != hdr->crc )
  {
    /* DEBUG_PRINT(("CRC error in %s", hdr->name)); */
    hdr->crc_fail = 1;
  }

  return(errOK); /* ok */
}

/* ------------------------------------------------------------------------- */

typedef unsigned char  uchar;   /*  8 bits or more */
typedef unsigned int   uint;    /* 16 bits or more */
typedef unsigned short ushort;  /* 16 bits or more */
typedef unsigned long  ulong;   /* 32 bits or more */
typedef short          node;

#undef  DICSIZ
#undef  PBIT
#undef  NPT
#undef  NP
#undef  NT

#define DICBIT    13    /* 12(-lh4-) or 13(-lh5-) */
#define DICSIZ   (1U << DICBIT)
#define MATCHBIT   8    /* bits for MAXMATCH - THRESHOLD */
#define MAXMATCH 256    /* formerly F (not more than UCHAR_MAX + 1) */
#define THRESHOLD  3    /* choose optimal value */
#define PERC_FLAG  0x8000U

#define NC (UCHAR_MAX + MAXMATCH + 2 - THRESHOLD)
#define CBIT 9        /* $\lfloor \log_2 NC \rfloor + 1$ */
#define CODE_BIT  16  /* codeword length */

#define NP (DICBIT + 1)
#define NT (CODE_BIT + 3)
#define PBIT 4  /* smallest integer such that (1U << PBIT) > NP */
#define TBIT 5  /* smallest integer such that (1U << TBIT) > NT */
#if NT > NP
  #define NPT NT
#else
  #define NPT NP
#endif

#define PERCOLATE  1
#define NIL        0
#define MAX_HASH_VAL (3 * DICSIZ + (DICSIZ / 512 + 1) * UCHAR_MAX)

typedef struct sLHA_ENCODER {
  uchar*  text;
  uchar*  childcount;
  node*   position;
  node*   parent;
  node*   prev;
  node*   next;
  uchar*  level;
  uchar*  buf;
  node    pos;
  node    matchpos;
  node    avail;
  ulong   origsize;
  ulong   compsize;
  uint    output_pos;
  uint    output_mask;
  uint    blocksize;
  uint    bufsiz;
  uint    bitbuf;
  uint    cpos;
  uint    subbitbuf;
  int     bitcount;
  int     remainder;
  int     matchlen;
  int     unpackable;
  ushort  left[2 * NC - 1];
  ushort  right[2 * NC - 1];
  uchar   c_len[NC];
  uchar   pt_len[NPT];
  ushort  c_freq[2 * NC - 1];
  ushort  c_table[4096];
  ushort  c_code[NC];
  ushort  p_freq[2 * NP - 1];
  ushort  pt_table[256];
  ushort  pt_code[NPT];
  ushort  t_freq[2 * NT - 1];

  /* mektree.c */
  int     depth;
  int     n, heapsize;
  short   heap[NC + 1];
  ushort* freq;
  ushort* sortptr;
  uchar*  len;
  ushort  len_cnt[17];

  tLHA_CTX* Lha;
} tLHA_ENCODER, *pLHA_ENCODER;

static int fread_crc(tLHA_CTX* Ctx, unsigned char* p, int n)
{
  n = (int)LHA_ReadBlock(&Ctx->IO, p, n);
  Ctx->Glob.crc = calccrc(p, n, Ctx->Glob.crc);
  return n;
}

static void* mem_malloc(tLHA_ENCODER* Ctx, int size)
{
  tERROR Error;
  tPTR   Mem;

  Mem = NULL;
  if ( PR_FAIL(Error = CALL_Heap_Alloc(Ctx->Lha->hHeap, &Mem, size)) )
    return(NULL);

  return(Mem);
}

static int allocate_memory(tLHA_ENCODER* Ctx)
{
  if ( Ctx->next != NULL)
    return(1); /* already */
  
  Ctx->text       = mem_malloc(Ctx, DICSIZ * 2 + MAXMATCH);
  Ctx->level      = mem_malloc(Ctx, (DICSIZ + UCHAR_MAX + 1) * sizeof(*Ctx->level));
  Ctx->childcount = mem_malloc(Ctx, (DICSIZ + UCHAR_MAX + 1) * sizeof(*Ctx->childcount));
  #if PERCOLATE
    Ctx->position = mem_malloc(Ctx, (DICSIZ + UCHAR_MAX + 1) * sizeof(*Ctx->position));
  #else
    Ctx->position = mem_malloc(Ctx, DICSIZ * sizeof(*Ctx->position));
  #endif
  Ctx->parent     = mem_malloc(Ctx, DICSIZ * 2 * sizeof(*Ctx->parent));
  Ctx->prev       = mem_malloc(Ctx, DICSIZ * 2 * sizeof(*Ctx->prev));
  Ctx->next       = mem_malloc(Ctx, (MAX_HASH_VAL + 1) * sizeof(*Ctx->next));

  if ( !Ctx->text || !Ctx->level || !Ctx->childcount || !Ctx->position )
    return(0);
  if ( !Ctx->parent || !Ctx->prev || !Ctx->next )
    return(0);

  return(1);
}

static void init_slide(tLHA_ENCODER* Ctx)
{
  node i;

  for (i = DICSIZ; i <= DICSIZ + UCHAR_MAX; i++) {
    Ctx->level[i] = 1;
    #if PERCOLATE
      Ctx->position[i] = NIL;  /* sentinel */
    #endif
  }
  for (i = DICSIZ; i < DICSIZ * 2; i++) Ctx->parent[i] = NIL;
  Ctx->avail = 1;
  for (i = 1; i < DICSIZ - 1; i++) Ctx->next[i] = i + 1;
  Ctx->next[DICSIZ - 1] = NIL;
  for (i = DICSIZ * 2; i <= MAX_HASH_VAL; i++) Ctx->next[i] = NIL;
}

#define HASH(p, c) ((p) + ((c) << (DICBIT - 9)) + DICSIZ * 2)

static node child(tLHA_ENCODER* Ctx, node q, uchar c)
  /* q's child for character c (NIL if not found) */
{
  node r;

  r = Ctx->next[HASH(q, c)];
  Ctx->parent[NIL] = q;  /* sentinel */
  while (Ctx->parent[r] != q) r = Ctx->next[r];
  return r;
}

static void makechild(tLHA_ENCODER* Ctx, node q, uchar c, node r)
  /* Let r be q's child for character c. */
{
  node h, t;

  h = (node)HASH(q, c);
  t = Ctx->next[h]; Ctx->next[h] = r; Ctx->next[r] = t;
  Ctx->prev[t] = r;  Ctx->prev[r] = h;
  Ctx->parent[r] = q;  Ctx->childcount[q]++;
}

static void split(tLHA_ENCODER* Ctx, node old)
{
  node nnew, t;

  nnew = Ctx->avail; Ctx->avail = Ctx->next[nnew]; Ctx->childcount[nnew] = 0;
  t = Ctx->prev[old]; Ctx->prev[nnew] = t; Ctx->next[t] = nnew;
  t = Ctx->next[old]; Ctx->next[nnew] = t; Ctx->prev[t] = nnew;
  Ctx->parent[nnew] = Ctx->parent[old];
  Ctx->level[nnew] = Ctx->matchlen;
  Ctx->position[nnew] = Ctx->pos;
  makechild(Ctx, nnew, Ctx->text[Ctx->matchpos + Ctx->matchlen], old);
  makechild(Ctx, nnew, Ctx->text[Ctx->pos + Ctx->matchlen], Ctx->pos);
}

static void insert_node(tLHA_ENCODER* Ctx)
{
  node  q, r, j, t;
  uchar c, *t1, *t2;

  if ( Ctx->matchlen >= 4 )
  {
    Ctx->matchlen--;
    r = (node)((Ctx->matchpos + 1) | DICSIZ);
    while ((q = Ctx->parent[r]) == NIL) r = Ctx->next[r];
    while (Ctx->level[q] >= Ctx->matchlen) {
      r = q;  q = Ctx->parent[q];
    }
    #if PERCOLATE
      t = q;
      while (Ctx->position[t] < 0) {
        Ctx->position[t] = Ctx->pos;  t = Ctx->parent[t];
      }
      if (t < DICSIZ) Ctx->position[t] = Ctx->pos | PERC_FLAG;
    #else
      t = q;
      while (t < DICSIZ) {
        Ctx->position[t] = Ctx->pos;  t = Ctx->parent[t];
      }
    #endif
  } else {
    q = (node)(Ctx->text[Ctx->pos] + DICSIZ);  c = Ctx->text[Ctx->pos + 1];
    if ((r = child(Ctx, q, c)) == NIL) {
      makechild(Ctx, q, c, Ctx->pos); Ctx->matchlen = 1;
      return;
    }
    Ctx->matchlen = 2;
  }
  for ( ; ; ) {
    if (r >= DICSIZ) {
      j = MAXMATCH; Ctx->matchpos = r;
    } else {
      j = Ctx->level[r];
      Ctx->matchpos = Ctx->position[r] & ~PERC_FLAG;
    }
    if (Ctx->matchpos >= Ctx->pos) Ctx->matchpos -= DICSIZ;
    t1 = &Ctx->text[Ctx->pos + Ctx->matchlen];
    t2 = &Ctx->text[Ctx->matchpos + Ctx->matchlen];
    while (Ctx->matchlen < j) {
      if (*t1 != *t2) { split(Ctx, r);  return;  }
      Ctx->matchlen++;  t1++;  t2++;
    }
    if (Ctx->matchlen >= MAXMATCH) break;
    Ctx->position[r] = Ctx->pos;
    q = r;
    if ((r = child(Ctx, q, *t1)) == NIL) {
      makechild(Ctx, q, *t1, Ctx->pos);  return;
    }
    Ctx->matchlen++;
  }
  t = Ctx->prev[r]; Ctx->prev[Ctx->pos] = t; Ctx->next[t] = Ctx->pos;
  t = Ctx->next[r]; Ctx->next[Ctx->pos] = t; Ctx->prev[t] = Ctx->pos;
  Ctx->parent[Ctx->pos] = q; Ctx->parent[r] = NIL;
  Ctx->next[r] = Ctx->pos;  /* special use of next[] */
}

static void delete_node(tLHA_ENCODER* Ctx)
{
  #if PERCOLATE
    node q, r, s, t, u;
  #else
    node r, s, t, u;
  #endif

  if (Ctx->parent[Ctx->pos] == NIL) return;
  r = Ctx->prev[Ctx->pos];  s = Ctx->next[Ctx->pos];
  Ctx->next[r] = s;  Ctx->prev[s] = r;
  r = Ctx->parent[Ctx->pos]; Ctx->parent[Ctx->pos] = NIL;
  if (r >= DICSIZ || --Ctx->childcount[r] > 1) return;
  #if PERCOLATE
    t = Ctx->position[r] & ~PERC_FLAG;
  #else
    t = Ctx->position[r];
  #endif
  if (t >= Ctx->pos) t -= DICSIZ;
  #if PERCOLATE
    s = t;  q = Ctx->parent[r];
    while ((u = Ctx->position[q]) & PERC_FLAG) {
      u &= ~PERC_FLAG;  if (u >= Ctx->pos) u -= DICSIZ;
      if (u > s) s = u;
      Ctx->position[q] = (node)(s | DICSIZ);  q = Ctx->parent[q];
    }
    if (q < DICSIZ) {
      if (u >= Ctx->pos) u -= DICSIZ;
      if (u > s) s = u;
      Ctx->position[q] = (node)(s | DICSIZ | PERC_FLAG);
    }
  #endif
  s = child(Ctx, r, Ctx->text[t + Ctx->level[r]]);
  t = Ctx->prev[s]; u = Ctx->next[s];
  Ctx->next[t] = u; Ctx->prev[u] = t;
  t = Ctx->prev[r]; Ctx->next[t] = s; Ctx->prev[s] = t;
  t = Ctx->next[r]; Ctx->prev[t] = s; Ctx->next[s] = t;
  Ctx->parent[s] = Ctx->parent[r]; Ctx->parent[r] = NIL;
  Ctx->next[r] = Ctx->avail; Ctx->avail = r;
}

static void mem_memmove(void* dst, void* src, unsigned int len)
{
  while( len-- )
  {
    *(char*)dst = *(char*)src;
    dst = (char*)dst + 1;
    src = (char*)src + 1;
  }
}

static void get_next_match(tLHA_ENCODER* Ctx)
{
  int n;

  Ctx->remainder--;
  if (++Ctx->pos == DICSIZ * 2) {
    mem_memmove(&Ctx->text[0], &Ctx->text[DICSIZ], DICSIZ + MAXMATCH);
    n = fread_crc(Ctx->Lha, &Ctx->text[DICSIZ + MAXMATCH], DICSIZ);
    Ctx->remainder += n; Ctx->pos = DICSIZ; /* putc('.', stderr); */
  }
  delete_node(Ctx);  insert_node(Ctx);
}

/* ----- io.h -------------------------------------------------------------- */

static void init_putbits(tLHA_ENCODER* Ctx)
{
  Ctx->bitcount = CHAR_BIT;
  Ctx->subbitbuf = 0;
}

/* Write rightmost n bits of x */
static void putbits(tLHA_ENCODER* Ctx, int n, uint x)
{
  unsigned char c;

  if (n < Ctx->bitcount) {
    Ctx->subbitbuf |= x << (Ctx->bitcount -= n);
  } else {
    if (Ctx->compsize < Ctx->origsize) {
      c = (unsigned char)(Ctx->subbitbuf | (x >> (n -= Ctx->bitcount)));
      if ( !LHA_PutByte(&Ctx->Lha->IO, c) )
      {
        Ctx->Lha->Error = errNOT_OK;
        return; /* can't write */
      }
      Ctx->compsize++;
    } else Ctx->unpackable = 1;
    if (n < CHAR_BIT) {
      Ctx->subbitbuf = x << (Ctx->bitcount = CHAR_BIT - n);
    } else {
      if (Ctx->compsize < Ctx->origsize) {
        c = (unsigned char)(x >> (n - CHAR_BIT));
        if ( !LHA_PutByte(&Ctx->Lha->IO, c) )
        {
          Ctx->Lha->Error = errNOT_OK;
          return; /* can't write */
        }
        Ctx->compsize++;
      } else Ctx->unpackable = 1;
      Ctx->subbitbuf = x << (Ctx->bitcount = 2 * CHAR_BIT - n);
    }
  }
}

/* ---------- maketree.c --------------------------------------------------- */

static void count_len(tLHA_ENCODER* Ctx, int i)  /* call with i = root */
{
  if (i < Ctx->n) Ctx->len_cnt[(Ctx->depth < 16) ? Ctx->depth : 16]++;
  else {
    Ctx->depth++;
    count_len(Ctx, Ctx->left[i]);
    count_len(Ctx, Ctx->right[i]);
    Ctx->depth--;
  }
}

static void make_len(tLHA_ENCODER* Ctx, int root)
{
  int i, k;
  uint cum;

  for (i = 0; i <= 16; i++) Ctx->len_cnt[i] = 0;
  count_len(Ctx, root);
  cum = 0;
  for (i = 16; i > 0; i--)
    cum += Ctx->len_cnt[i] << (16 - i);
  while (cum != (1U << 16)) {
    /* fprintf(stderr, "17"); */
    Ctx->len_cnt[16]--;
    for (i = 15; i > 0; i--) {
      if (Ctx->len_cnt[i] != 0) {
        Ctx->len_cnt[i]--;  Ctx->len_cnt[i+1] += 2;  break;
      }
    }
    cum--;
  }
  for (i = 16; i > 0; i--) {
    k = Ctx->len_cnt[i];
    while (--k >= 0) Ctx->len[*Ctx->sortptr++] = i;
  }
}

/* priority queue; send i-th entry down heap */
static void downheap(tLHA_ENCODER* Ctx, int i)
{
  int j, k;

  k = Ctx->heap[i];
  while ((j = 2 * i) <= Ctx->heapsize) {
    if (j < Ctx->heapsize && Ctx->freq[Ctx->heap[j]] > Ctx->freq[Ctx->heap[j + 1]])
      j++;
    if (Ctx->freq[k] <= Ctx->freq[Ctx->heap[j]]) break;
    Ctx->heap[i] = Ctx->heap[j];  i = j;
  }
  Ctx->heap[i] = k;
}

static void make_code(tLHA_ENCODER* Ctx, int n, uchar len[], ushort code[])
{
  int    i;
  ushort start[18];

  start[1] = 0;
  for (i = 1; i <= 16; i++)
    start[i + 1] = (start[i] + Ctx->len_cnt[i]) << 1;
  for (i = 0; i < n; i++) code[i] = start[len[i]]++;
}

/* make tree, calculate len[], return root */
static int make_tree(tLHA_ENCODER* Ctx, int nparm, ushort freqparm[],
                      uchar lenparm[], ushort codeparm[])
{
  int i, j, k, avail;

  Ctx->n = nparm;  Ctx->freq = freqparm;  Ctx->len = lenparm;
  avail = Ctx->n;  Ctx->heapsize = 0;  Ctx->heap[1] = 0;
  for (i = 0; i < Ctx->n; i++) {
    Ctx->len[i] = 0;
    if (Ctx->freq[i]) Ctx->heap[++Ctx->heapsize] = i;
  }
  if (Ctx->heapsize < 2) {
    codeparm[Ctx->heap[1]] = 0;  return Ctx->heap[1];
  }
  for (i = Ctx->heapsize / 2; i >= 1; i--)
    downheap(Ctx, i);  /* make priority queue */
  Ctx->sortptr = codeparm;
  do {  /* while queue has at least two entries */
    i = Ctx->heap[1];  /* take out least-freq entry */
    if (i < Ctx->n) *Ctx->sortptr++ = i;
    Ctx->heap[1] = Ctx->heap[Ctx->heapsize--];
    downheap(Ctx, 1);
    j = Ctx->heap[1];  /* next least-freq entry */
    if (j < Ctx->n) *Ctx->sortptr++ = j;
    k = avail++;  /* generate new node */
    Ctx->freq[k] = Ctx->freq[i] + Ctx->freq[j];
    Ctx->heap[1] = k;  downheap(Ctx, 1);  /* put into queue */
    Ctx->left[k] = i;  Ctx->right[k] = j;
  } while (Ctx->heapsize > 1);
  Ctx->sortptr = codeparm;
  make_len(Ctx, k);
  make_code(Ctx, nparm, lenparm, codeparm);
  return k;  /* return root */
}

/* ----- huf.c ------------------------------------------------------------- */

static int huf_encode_start(tLHA_ENCODER* Ctx)
{
  int i;

  if (Ctx->bufsiz == 0)
  {
    Ctx->bufsiz = 16 * 1024U;
    if ( NULL == (Ctx->buf = mem_malloc(Ctx, Ctx->bufsiz)) )
      return(0);
  }
  Ctx->buf[0] = 0;
  for (i = 0; i < NC; i++) Ctx->c_freq[i] = 0;
  for (i = 0; i < NP; i++) Ctx->p_freq[i] = 0;
  Ctx->output_pos = Ctx->output_mask = 0;
  init_putbits(Ctx);
  return(1);
}

static void count_t_freq(tLHA_ENCODER* Ctx)
{
  int i, k, n, count;

  for (i = 0; i < NT; i++) Ctx->t_freq[i] = 0;
  n = NC;
  while (n > 0 && Ctx->c_len[n - 1] == 0) n--;
  i = 0;
  while (i < n) {
    k = Ctx->c_len[i++];
    if (k == 0) {
      count = 1;
      while (i < n && Ctx->c_len[i] == 0) {  i++;  count++;  }
      if (count <= 2) Ctx->t_freq[0] += count;
      else if (count <= 18) Ctx->t_freq[1]++;
      else if (count == 19) { Ctx->t_freq[0]++; Ctx->t_freq[1]++;  }
      else Ctx->t_freq[2]++;
    } else Ctx->t_freq[k + 2]++;
  }
}

static void write_pt_len(tLHA_ENCODER* Ctx, int n, int nbit, int i_special)
{
  int i, k;

  while (n > 0 && Ctx->pt_len[n - 1] == 0) n--;
  putbits(Ctx, nbit, n);
  i = 0;
  while (i < n) {
    k = Ctx->pt_len[i++];
    if (k <= 6) putbits(Ctx, 3, k);
    else putbits(Ctx, k - 3, (1U << (k - 3)) - 2);
    if (i == i_special) {
      while (i < 6 && Ctx->pt_len[i] == 0) i++;
      putbits(Ctx, 2, (i - 3) & 3);
    }
  }
}

static void write_c_len(tLHA_ENCODER* Ctx)
{
  int i, k, n, count;

  n = NC;
  while (n > 0 && Ctx->c_len[n - 1] == 0) n--;
  putbits(Ctx, CBIT, n);
  i = 0;
  while (i < n) {
    k = Ctx->c_len[i++];
    if (k == 0) {
      count = 1;
      while (i < n && Ctx->c_len[i] == 0) {  i++;  count++;  }
      if (count <= 2) {
        for (k = 0; k < count; k++)
          putbits(Ctx, Ctx->pt_len[0], Ctx->pt_code[0]);
      } else if (count <= 18) {
        putbits(Ctx, Ctx->pt_len[1], Ctx->pt_code[1]);
        putbits(Ctx, 4, count - 3);
      } else if (count == 19) {
        putbits(Ctx, Ctx->pt_len[0], Ctx->pt_code[0]);
        putbits(Ctx, Ctx->pt_len[1], Ctx->pt_code[1]);
        putbits(Ctx, 4, 15);
      } else {
        putbits(Ctx, Ctx->pt_len[2], Ctx->pt_code[2]);
        putbits(Ctx, CBIT, count - 20);
      }
    } else putbits(Ctx, Ctx->pt_len[k + 2], Ctx->pt_code[k + 2]);
  }
}

static void encode_c(tLHA_ENCODER* Ctx, int c)
{
  putbits(Ctx, Ctx->c_len[c], Ctx->c_code[c]);
}

static void encode_p(tLHA_ENCODER* Ctx, uint p)
{
  uint c, q;

  c = 0;  q = p;  while (q) {  q >>= 1;  c++;  }
  putbits(Ctx, Ctx->pt_len[c], Ctx->pt_code[c]);
  if (c > 1) putbits(Ctx, c - 1, p & (0xFFFFU >> (17 - c)));
}

static void send_block(tLHA_ENCODER* Ctx)
{
  uint i, k, flags, root, pos, size;

  root = make_tree(Ctx, NC, Ctx->c_freq, Ctx->c_len, Ctx->c_code);
  size = Ctx->c_freq[root];  putbits(Ctx, 16, size);
  if (root >= NC) {
    count_t_freq(Ctx);
    root = make_tree(Ctx, NT, Ctx->t_freq, Ctx->pt_len, Ctx->pt_code);
    if (root >= NT) {
      write_pt_len(Ctx, NT, TBIT, 3);
    } else {
      putbits(Ctx, TBIT, 0);
      putbits(Ctx, TBIT, root);
    }
    write_c_len(Ctx);
  } else {
    putbits(Ctx, TBIT, 0);
    putbits(Ctx, TBIT, 0);
    putbits(Ctx, CBIT, 0);
    putbits(Ctx, CBIT, root);
  }
  root = make_tree(Ctx, NP, Ctx->p_freq, Ctx->pt_len, Ctx->pt_code);
  if (root >= NP) {
    write_pt_len(Ctx, NP, PBIT, -1);
  } else {
    putbits(Ctx, PBIT, 0);
    putbits(Ctx, PBIT, root);
  }
  pos = 0;
  for (i = 0; i < size; i++) {
    if (i % CHAR_BIT == 0) flags = Ctx->buf[pos++];  else flags <<= 1;
    if (flags & (1U << (CHAR_BIT - 1))) {
      encode_c(Ctx, Ctx->buf[pos++] + (1U << CHAR_BIT));
      k = Ctx->buf[pos++] << CHAR_BIT;  k += Ctx->buf[pos++];
      encode_p(Ctx, k);
    } else encode_c(Ctx, Ctx->buf[pos++]);
    if (Ctx->unpackable) return;
  }
  for (i = 0; i < NC; i++) Ctx->c_freq[i] = 0;
  for (i = 0; i < NP; i++) Ctx->p_freq[i] = 0;
}

static void output(tLHA_ENCODER* Ctx, uint c, uint p)
{
  if ((Ctx->output_mask >>= 1) == 0) {
    Ctx->output_mask = 1U << (CHAR_BIT - 1);
    if (Ctx->output_pos >= Ctx->bufsiz - 3 * CHAR_BIT) {
      send_block(Ctx);
      if (Ctx->unpackable) return;
      Ctx->output_pos = 0;
    }
    Ctx->cpos = Ctx->output_pos++; Ctx->buf[Ctx->cpos] = 0;
  }
  Ctx->buf[Ctx->output_pos++] = (uchar)c;
  Ctx->c_freq[c]++;
  if (c >= (1U << CHAR_BIT)) {
    Ctx->buf[Ctx->cpos] |= Ctx->output_mask;
    Ctx->buf[Ctx->output_pos++] = (uchar)(p >> CHAR_BIT);
    Ctx->buf[Ctx->output_pos++] = (uchar) p;
    c = 0;  while (p) {  p >>= 1;  c++;  }
    Ctx->p_freq[c]++;
  }
}

static void huf_encode_end(tLHA_ENCODER* Ctx)
{
  if (!Ctx->unpackable) {
    send_block(Ctx);
    putbits(Ctx, CHAR_BIT - 1, 0);  /* flush remaining bits */
  }
}

static void encode(tLHA_ENCODER* Ctx)
{
  node lastmatchpos;
  int  lastmatchlen;

  allocate_memory(Ctx);  init_slide(Ctx);  huf_encode_start(Ctx);
  Ctx->remainder = fread_crc(Ctx->Lha, &Ctx->text[DICSIZ], DICSIZ + MAXMATCH);
  /* putc('.', stderr); */
  Ctx->matchlen = 0;
  Ctx->pos = DICSIZ;  insert_node(Ctx);
  if (Ctx->matchlen > Ctx->remainder) Ctx->matchlen = Ctx->remainder;
  while (Ctx->remainder > 0 && !Ctx->unpackable && Ctx->Lha->Error == errOK)
  {
    lastmatchlen = Ctx->matchlen;
    lastmatchpos = Ctx->matchpos;
    get_next_match(Ctx);
    if (Ctx->matchlen > Ctx->remainder) Ctx->matchlen = Ctx->remainder;
    if (Ctx->matchlen > lastmatchlen || lastmatchlen < THRESHOLD)
      output(Ctx, Ctx->text[Ctx->pos - 1], 0);
    else {
      output(Ctx, lastmatchlen + (UCHAR_MAX + 1 - THRESHOLD),
           (Ctx->pos - lastmatchpos - 2) & (DICSIZ - 1));
      while (--lastmatchlen > 0) get_next_match(Ctx);
      if (Ctx->matchlen > Ctx->remainder) Ctx->matchlen = Ctx->remainder;
    }
  }
  huf_encode_end(Ctx);
}

/* ------------------------------------------------------------------------- */

#if 0
#include <stdio.h>
static tERROR DumpSeqIO(hSEQ_IO hSrc, tCHAR* Name)
{
  tQWORD Size;
  tQWORD CurOfs;
  tBYTE  Buf[1024];
  tDWORD Cnt;
  FILE*  F;
  
  F = fopen(Name, "wb");
  CALL_SeqIO_Seek(hSrc, &CurOfs, 0, cSEEK_FORWARD);
  CALL_IO_GetSize(hSrc, &Size, IO_SIZE_TYPE_EXPLICIT);
  CALL_SeqIO_Seek(hSrc, 0, 0, cSEEK_SET);

  while ( Size )
  {
    Cnt = (tDWORD)Min(Size, sizeof(Buf));
    CALL_SeqIO_Read(hSrc, NULL, Buf, Cnt);
    fwrite(Buf, 1, Cnt, F);
    Size -= Cnt;
  }

  fclose(F);
  CALL_SeqIO_Seek(hSrc, NULL, CurOfs, cSEEK_SET);
  return(errOK);
}
#endif

/* ------------------------------------------------------------------------- */

static int copyfile(tLHA_CTX* Ctx, kav_uint32 Size)
{
  tERROR Error;
  tDWORD Cnt, Rdt, Wrt;
  tBYTE* Buf = Ctx->TmBuf;

  /* copy */
  while( Size )
  {
    Cnt = Min(Size, TM_BUFLEN);
    if ( PR_FAIL(Error = CALL_SeqIO_Read(Ctx->IO.hSeqIn, &Rdt, Buf, Cnt)) )
    {
      Ctx->Error = Error;
      return(0);
    }
    Ctx->Glob.crc = calccrc(Buf, Cnt, Ctx->Glob.crc);
    if ( PR_FAIL(Error = CALL_SeqIO_Write(Ctx->IO.hSeqOut, &Wrt, Buf, Cnt)) )
    {
      Ctx->Error = Error;
      return(0);
    }
    if ( Rdt != Cnt || Wrt != Cnt )
    {
      Ctx->Error = errOUT_OF_OBJECT;
      return(0);
    }
    Size -= Cnt;
  }

  CALL_SeqIO_Flush(Ctx->IO.hSeqOut);
  /* DumpSeqIO(Ctx->IO.hSeqIn,  "input.bin");  */
  /* DumpSeqIO(Ctx->IO.hSeqOut, "result.bin"); */
  return(1);
}

/* ------------------------------------------------------------------------- */

static int encode_lzhuf(tLHA_CTX* Ctx, tDWORD size, tDWORD* original_size_var,
  tDWORD* packed_size_var, tCHAR* hdr_method, int method, int* result)
{
  Ctx->Encoder->unpackable = 0;
  Ctx->Encoder->origsize = size;
  Ctx->Encoder->compsize = 0;
  Ctx->Glob.origsize = size;
  Ctx->Glob.compsize = 0;
  Ctx->Glob.crc = 0;

  if ( method > 0 )
  {
    encode(Ctx->Encoder);
    if ( Ctx->Encoder->unpackable )
    {
      *result = 1;
      return(0);
    }
    LHA_FlushWBuf(&Ctx->IO);
    *packed_size_var = Ctx->Encoder->compsize;
    *original_size_var = Ctx->Encoder->origsize;
  } 
  else
  {
    copyfile(Ctx, size);
    *packed_size_var = size;
    *original_size_var = size;
  }
  
  hdr_method[0] = '-';
  hdr_method[1] = 'l';
  hdr_method[2] = 'h';
  hdr_method[3] = '0' + method;
  hdr_method[4] = '-';

  *result = 0;
  return Ctx->Glob.crc;
}

/* ------------------------------------------------------------------------- */

tERROR CopySeqIOBlock(tLHA_CTX* Ctx, hSEQ_IO hSrc, hSEQ_IO hDst,
                      tQWORD SrcOfs, tQWORD DstOfs, tDWORD BlkSize)
{
  tERROR Error;
  tDWORD Cnt, Rdt, Wrt;
  tBYTE* Buf;

  /* seek files */
  if ( PR_FAIL(Error = CALL_SeqIO_Seek(hSrc, NULL, SrcOfs, cSEEK_SET)) )
    return(Error);
  if ( PR_FAIL(Error = CALL_SeqIO_Seek(hDst, NULL, DstOfs, cSEEK_SET)) )
    return(Error);

  /* copy */
  Buf = Ctx->TmBuf;
  while( BlkSize )
  {
    Cnt = Min(BlkSize, TM_BUFLEN);
    if ( PR_FAIL(Error = CALL_SeqIO_Read(hSrc, &Rdt, Buf, Cnt)) )
      return(Error);
    if ( PR_FAIL(Error = CALL_SeqIO_Write(hDst, &Wrt, Buf, Cnt)) )
      return(Error);
    if ( Rdt != Cnt || Wrt != Cnt )
      return(errOUT_OF_OBJECT);
    BlkSize -= Cnt;
  }

  return(errOK);
}

/* ------------------------------------------------------------------------- */

tERROR LHA_CompressOne(tLHA_CTX* Ctx, tFILE_NODE* FNode, hSEQ_IO hSeqSrc,
                       hSEQ_IO hSeqIn, hSEQ_IO hSeqOut, tQWORD OutOfs,
                       tDWORD* Written)
{
  tERROR  Error;
  tCHAR   Meth[5];
  tQWORD  QUSize;
  tDWORD  USize;
  tDWORD  PSize;
  tDWORD  Rdt;
  tBYTE*  Header;
  tUINT   HdrLen;
  tUINT   NamLen;
  tINT    crc;
  tINT    res;

  /* init compressor */
  if ( PR_FAIL(Error = LHA_InitFile(&Ctx->IO, hSeqIn, hSeqOut)) )
    goto loc_exit;

#if 0
  DumpSeqIO(hSeqIn, "healed.bin");
#endif

  /* alloc encoder context */
  if ( NULL == Ctx->Encoder )
  {
    if ( PR_FAIL(Error = CALL_Heap_Alloc(Ctx->hHeap, (tPTR)(&Ctx->Encoder),
      sizeof(tLHA_ENCODER) + 64)) )
    {
      goto loc_exit;
    }
    
    memset(Ctx->Encoder, 0, sizeof(tLHA_ENCODER));
    Ctx->Encoder->Lha = Ctx;
  }

  /* get input size */
  Ctx->Encoder->Lha->Error = errOK;
  Error = CALL_IO_GetSize(hSeqIn, &QUSize, IO_SIZE_TYPE_EXPLICIT);
  if ( PR_FAIL(Error) )
    goto loc_exit;

  /* calc block header size (header_level == 1) */
  NamLen = _toui32(strlen(FNode->LzHdr.name));
  HdrLen = (1+1+5+4+4+4+1+1+1+NamLen+2+1+2);
  if ( HdrLen >= TM_BUFLEN )
  {
    Error = errNOT_OK;
    goto loc_exit;
  }

  /* reserve header */
  memset(Ctx->TmBuf, 0, HdrLen);
  if ( PR_FAIL(Error = CALL_SeqIO_Seek(hSeqOut, NULL, OutOfs, cSEEK_SET)) )
    goto loc_exit;
  if ( PR_FAIL(Error = CALL_SeqIO_Write(hSeqOut, NULL, Ctx->TmBuf, HdrLen)) )
    goto loc_exit;

  /* pack stream */
  crc = encode_lzhuf(Ctx, (tDWORD)QUSize, &USize, &PSize, &Meth[0],
    LZHUFF5_METHOD_NUM, &res);

  if ( res == 1 ) /* can't pack - too small */
  {
    /* reseek io's */
    if ( PR_FAIL(Error = CALL_SeqIO_Seek(hSeqOut, NULL, OutOfs + HdrLen, cSEEK_SET)) )
      goto loc_exit;
    if ( PR_FAIL(Error = CALL_SeqIO_SetSize(hSeqOut, OutOfs + HdrLen)) )
      goto loc_exit;
    if ( PR_FAIL(Error = CALL_SeqIO_Seek(hSeqOut, NULL, OutOfs + HdrLen, cSEEK_SET)) )
      goto loc_exit;
    if ( PR_FAIL(Error = CALL_SeqIO_Seek(hSeqIn, NULL, 0, cSEEK_SET)) )
      goto loc_exit;

    /* reinit compressor */
    if ( PR_FAIL(Error = LHA_InitFile(&Ctx->IO, hSeqIn, hSeqOut)) )
      goto loc_exit;

    /* store file */
    crc = encode_lzhuf(Ctx, (tDWORD)QUSize, &USize, &PSize, &Meth[0],
      LZHUFF0_METHOD_NUM, &res);
  }

  /* check for internal errors */
  if ( Ctx->Encoder->Lha->Error != errOK )
    return(Ctx->Encoder->Lha->Error);

  /* build new header */
  Header = &Ctx->TmBuf[0];
  Header[0x00] = 25 + NamLen;
  memcpy(&Header[2], &Meth[0], 5);
  *(tDWORD*)(&Header[0x07]) = PSize;
  *(tDWORD*)(&Header[0x0B]) = USize;
  *(tDWORD*)(&Header[0x0F]) = FNode->LzHdr.last_modified_stamp;
  Header[0x13] = FNode->LzHdr.attribute;
  Header[0x14] = 1;
  Header[0x15] = NamLen;
  memcpy(&Header[0x16], &FNode->LzHdr.name[0], NamLen);
  *(tWORD*)(&Header[0x16+NamLen]) = crc;
  Header[0x18+NamLen] = 0x20;
  *(tWORD*)(&Header[0x19+NamLen]) = 0;
  Header[1] = calc_sum(&Ctx->TmBuf[I_METHOD], 25+NamLen-2);

  /* write new header */
  if ( PR_FAIL(Error = CALL_SeqIO_Seek(hSeqOut, NULL, OutOfs, cSEEK_SET)) )
    goto loc_exit;
  if ( PR_FAIL(Error = CALL_SeqIO_Write(hSeqOut, &Rdt, Ctx->TmBuf, HdrLen)) )
    goto loc_exit;

  Error = errOK;
loc_exit:
  if ( Written ) *Written = (tDWORD)(HdrLen + PSize);
  return(Error);
}

/* ------------------------------------------------------------------------- */
