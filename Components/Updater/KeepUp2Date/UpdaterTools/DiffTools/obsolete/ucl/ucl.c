#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "../shared/types.h"
#include "ucl.h"

// ----------------------------------------------------------------------------

static tVOID code_prefix_ss11(tUCL_COMPRESS_CTX* c, tUINT i)
{
  tUINT t;
  if (i >= 2)
  {
    t = 4;
    i += 2;
    do {
      t <<= 1;
    } while (i >= t);
    t >>= 1;
    do {
      t >>= 1;
      bbPutBit(c, (i & t) ? 1 : 0);
      bbPutBit(c, 0);
    } while (t > 2);
  }

  bbPutBit(c, (tUINT)i & 1);
  bbPutBit(c, 1);
}

// ----------------------------------------------------------------------------

static tVOID code_prefix_ss12(tUCL_COMPRESS_CTX* c, tUINT i)
{
  tUINT t;

  if (i >= 2)
  {
    t = 2;
    do {
      i -= t;
      t <<= 2;
    } while (i >= t);
    do {
      t >>= 1;
      bbPutBit(c, (i & t) ? 1 : 0);
      bbPutBit(c, 0);
      t >>= 1;
      bbPutBit(c, (i & t) ? 1 : 0);
    } while (t > 2);
  }

  bbPutBit(c, (tUINT)i & 1);
  bbPutBit(c, 1);
}

// ----------------------------------------------------------------------------

static tVOID code_match(tUCL_COMPRESS_CTX* c, tUINT m_len, tUINT m_off)
{
    tUINT m_low = 0;

    while (m_len > c->conf.max_match)
    {
        code_match(c, c->conf.max_match - 3, m_off);
        m_len -= c->conf.max_match - 3;
    }

    c->match_bytes += m_len;
    if (m_len > c->result[3])
        c->result[3] = m_len;
    if (m_off > c->result[1])
        c->result[1] = m_off;

    bbPutBit(c, 0);

    m_len = m_len - 1 - (m_off > M2_MAX_OFFSET);
    m_low = (m_len >= 4) ? 0u : (unsigned) m_len;
    if (m_off == c->last_m_off)
    {
      bbPutBit(c, 0);
      bbPutBit(c, 1);
      bbPutBit(c, m_low > 1);
      bbPutBit(c, m_low & 1);
    }
    else
    {
      code_prefix_ss12(c, 1 + ((m_off - 1) >> 7));
      bbPutByte(c, ((((tUINT)m_off - 1) & 0x7f) << 1) | ((m_low > 1) ? 0 : 1));
      bbPutBit(c, m_low & 1);
    }
    if (m_len >= 4)
        code_prefix_ss11(c, m_len - 4);

    c->last_m_off = m_off;
}

// ----------------------------------------------------------------------------

static tVOID code_run(tUCL_COMPRESS_CTX* c, tBYTE* ii, tUINT lit)
{
    if (lit == 0)
        return;
    c->lit_bytes += lit;
    if (lit > c->result[5])
        c->result[5] = lit;
    do {
        bbPutBit(c, 1);
        bbPutByte(c, *ii++);
    } while (--lit > 0);
}


// ----------------------------------------------------------------------------

static tINT len_of_coded_match(tUCL_COMPRESS_CTX*c, tUINT m_len, tUINT m_off)
{
    int b;
    if (m_len < 2 || (m_len == 2 && (m_off > M2_MAX_OFFSET))
        || m_off > c->conf.max_offset)
        return -1;

    m_len = m_len - 2 - (m_off > M2_MAX_OFFSET);

    if (m_off == c->last_m_off)
        b = 1 + 2;
    else
    {
        b = 1 + 9;
        m_off = (m_off - 1) >> 7;
        while (m_off > 0)
        {
            b += 3;
            m_off >>= 2;
        }
    }

    b += 2;
    if (m_len < 3)
        return b;
    m_len -= 3;

    do {
        b += 2;
        m_len >>= 1;
    } while (m_len > 0);

    return b;
}

// ----------------------------------------------------------------------------

// #define DO_FAST

// main public function
tINT UCL_Compress(tBYTE* in, tUINT in_len, tBYTE* out, tUINT* out_len,
                  tUINT Code, tUINT* Delta, tUCL_CALLBACK* cb)
{
    tBOOL DoCode;
    const int level = 10;
    const ucl_byte *ii;
    ucl_uint lit;
    ucl_uint m_len, m_off;
    tUCL_COMPRESS_CTX c_buffer;
    tUCL_COMPRESS_CTX* const c = &c_buffer;
    tUCL_SWD  the_swd;
    tUCL_SWD* const swd = &the_swd;
    ucl_uint result_buffer[16];
    int r;
    tUCL_COMPRESS_CFG  m_conf;
    tUCL_COMPRESS_CFG* const conf = &m_conf;
    tSWD_CONFIG  m_sc;
    tSWD_CONFIG* const sc = &m_sc;

#ifdef DO_FAST
    sc->try_lazy    = 2;
    sc->good_length = 64;
    sc->max_lazy    = 256;
    sc->nice_length = F;
    sc->max_chain   = 2048;
    sc->flags       = 1;
    sc->max_offset  = N;
#else
    sc->try_lazy    = 2;
    sc->good_length = 0x2000;
    sc->max_lazy    = 0x8000;
    sc->nice_length = F;
    sc->max_chain   = 0x10000;
    sc->flags       = 1;
    sc->max_offset  = N;
#endif
    
    DoCode = (0 == Code) ? (cTRUE) : (cFALSE);

    memset(&m_conf, 0xFF, sizeof(m_conf));
    memset(c, 0, sizeof(*c));
    c->ip = c->in = in;
    c->in_end = in + in_len;
    c->out = out;
    if (cb && cb->CallBack)
      c->cb = cb;
    c->result = result_buffer;
    memset(c->result, 0, 16*sizeof(*c->result));
    c->result[0] = c->result[2] = c->result[4] = UCL_UINT_MAX;
    memset(&c->conf, 0xff, sizeof(c->conf));
    if (conf)
      memcpy(&c->conf, conf, sizeof(c->conf));
    r = bbConfig(c, 0, 8);
    if (r == 0)
        r = bbConfig(c, c->conf.bb_endian, c->conf.bb_size);
    if (r != 0)
        return(UCL_E_INVALID_ARGUMENT);
    c->bb_op = out;

    ii = c->ip;             // point to start of literal run
    lit = 0;

    swd->f = F;
    swd->n = N;
    if (in_len >= 256 && in_len < swd->n)
      swd->n = in_len;
    else
      swd->n = N;

    r = init_match(c,swd,NULL,0,sc->flags);
    if (r != UCL_E_OK)
    {
      return r;
    }
    if (sc->max_chain > 0)
        swd->max_chain = sc->max_chain;
    if (sc->nice_length > 0)
        swd->nice_length = sc->nice_length;
    if (c->conf.max_match < swd->nice_length)
        swd->nice_length = c->conf.max_match;

    if (c->cb)
        (*c->cb->CallBack)(0,0,-1,c->cb->User);

    c->last_m_off = 1;
    r = find_match(c,swd,0,0);
    if (r != UCL_E_OK)
        return r;

    while (c->look > 0)
    {
        ucl_uint ahead;
        ucl_uint max_ahead;
        int l1, l2;

        c->codesize = c->bb_op - out;

        m_len = c->m_len;
        m_off = c->m_off;

        if (lit == 0)
            ii = c->bp;

        if (m_len < 2 || (m_len == 2 && (m_off > M2_MAX_OFFSET))
            || m_off > c->conf.max_offset)
        {
            // a literal
            lit++;
            swd->max_chain = sc->max_chain;
            r = find_match(c,swd,1,0);
            continue;
        }

        // shall we try a lazy match
        ahead = 0;
        if (sc->try_lazy <= 0 || m_len >= sc->max_lazy || 
            m_off == c->last_m_off)
        {
            // no
            l1 = 0;
            max_ahead = 0;
        }
        else
        {
            // yes, try a lazy match
            l1 = len_of_coded_match(c,m_len,m_off);
            max_ahead = UCL_MIN(sc->try_lazy, m_len - 1);
        }

        while (ahead < max_ahead && c->look > m_len)
        {
            if (m_len >= sc->good_length)
                swd->max_chain = sc->max_chain >> 2;
            else
                swd->max_chain = sc->max_chain;
            r = find_match(c,swd,1,0);
            ahead++;

            if (c->m_len < 2)
                continue;
            l2 = len_of_coded_match(c,c->m_len,c->m_off);
            if (l2 < 0)
                continue;
            if (l1 + (int)(ahead + c->m_len - m_len) * 5 > l2 + (int)(ahead) * 9)
            {
                c->lazy++;
                {
                    lit += ahead;
                }
                goto lazy_match_done;
            }
        }

        if ( cFALSE == DoCode && ii > &c->in[Code] )
        {
          DoCode = (cTRUE);
          if ( NULL != Delta )
            *Delta = (ii - in) - Code;
        }

        // 1 - code run
        if ( DoCode )
          code_run(c,(tBYTE*)ii,lit);
        lit = 0;

        // 2 - code match
        if ( DoCode )
          code_match(c,m_len,m_off);
        swd->max_chain = sc->max_chain;
        r = find_match(c,swd,m_len,1+ahead);
lazy_match_done:;
    }

    // store final run
    code_run(c,(tBYTE*)ii,lit);

    // EOF 
    bbPutBit(c, 0);
    code_prefix_ss12(c, UCL_UINT32_C(0x1000000));
    bbPutByte(c, 0xff);
    bbFlushBits(c, 0);

    c->codesize = c->bb_op - out;
    *out_len = c->bb_op - out;
    if (c->cb)
        (*c->cb->CallBack)(c->textsize,c->codesize,4,c->cb->User);

    swd_exit(swd);
    return(UCL_E_OK);
}

// ----------------------------------------------------------------------------

