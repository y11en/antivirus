#include <stdlib.h>
#include <memory.h>
#include "../shared/types.h"
#include "ucl.h"

// ----------------------------------------------------------------------------

tINT init_match (tUCL_COMPRESS_CTX* c, tUCL_SWD* s, tBYTE* dict, 
                 tUINT dict_len, tUINT flags )
{
    int r;

    c->init = 1;
    s->c = c;
    c->last_m_len = c->last_m_off = 0;
    c->textsize = c->codesize = c->printcount = 0;
    c->lit_bytes = c->match_bytes = c->rep_bytes = 0;
    c->lazy = 0;

    r = swd_init(s,dict,dict_len);
    if (r != UCL_E_OK)
    {
        swd_exit(s);
        return r;
    }

    s->use_best_off = (flags & 1) ? 1 : 0;
    return UCL_E_OK;
}

// ----------------------------------------------------------------------------

tINT find_match (tUCL_COMPRESS_CTX* c, tUCL_SWD* s, tUINT this_len, tUINT skip)
{
    if (skip > 0)
    {
        swd_accept(s, this_len - skip);
        c->textsize += this_len - skip + 1;
    }
    else
    {
        c->textsize += this_len - skip;
    }

    s->m_len = THRESHOLD;
    swd_findbest(s);
    c->m_len = s->m_len;
    c->m_off = s->m_off;
    swd_getbyte(s);

    if (s->b_char < 0)
    {
        c->look = 0;
        c->m_len = 0;
//        swd_exit(s);
    }
    else
    {
        c->look = s->look + 1;
    }
    c->bp = c->ip - c->look;


    if (c->cb && c->textsize > c->printcount)
    {
        (*c->cb->CallBack)(c->textsize,c->codesize,3,c->cb->User);
        c->printcount += 1024;
    }

    return UCL_E_OK;
}


// ----------------------------------------------------------------------------
// bit buffer

tINT bbConfig(tUCL_COMPRESS_CTX* c, int endian, int bitsize)
{
    if (endian != -1)
    {
        if (endian != 0)
            return UCL_E_ERROR;
        c->bb_c_endian = endian;
    }
    if (bitsize != -1)
    {
        if (bitsize != 8 && bitsize != 16 && bitsize != 32)
            return UCL_E_ERROR;
        c->bb_c_s = bitsize;
        c->bb_c_s8 = bitsize / 8;
    }
    c->bb_b = 0; c->bb_k = 0;
    c->bb_p = NULL;
    c->bb_op = NULL;
    return UCL_E_OK;
}


tVOID bbWriteBits(tUCL_COMPRESS_CTX *c)
{
    ucl_byte *p = c->bb_p;
    ucl_uint32 b = c->bb_b;

    p[0] = UCL_BYTE(b >>  0);
    if (c->bb_c_s >= 16)
    {
        p[1] = UCL_BYTE(b >>  8);
        if (c->bb_c_s == 32)
        {
            p[2] = UCL_BYTE(b >> 16);
            p[3] = UCL_BYTE(b >> 24);
        }
    }
}


tVOID bbPutBit(tUCL_COMPRESS_CTX* c, unsigned int bit)
{
    if (c->bb_k < c->bb_c_s)
    {
        if (c->bb_k == 0)
        {
            c->bb_p = c->bb_op;
            c->bb_op += c->bb_c_s8;
        }

        c->bb_b = (c->bb_b << 1) + bit;
        c->bb_k++;
    }
    else
    {

        bbWriteBits(c);
        c->bb_p = c->bb_op;
        c->bb_op += c->bb_c_s8;
        c->bb_b = bit;
        c->bb_k = 1;
    }
}


tVOID bbPutByte(tUCL_COMPRESS_CTX *c, unsigned int b)
{
  *c->bb_op++ = UCL_BYTE(b);
}

tVOID bbFlushBits(tUCL_COMPRESS_CTX* c, unsigned int filler_bit)
{
    if (c->bb_k > 0)
    {
        while (c->bb_k != c->bb_c_s)
            bbPutBit(c, filler_bit);
        bbWriteBits(c);
        c->bb_k = 0;
    }
    c->bb_p = NULL;
}

