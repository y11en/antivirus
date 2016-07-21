#include <stdlib.h>
#include <memory.h>
#include "../shared/types.h"
#include "ucl.h"

// ----------------------------------------------------------------------------

tVOID swd_initdict(tUCL_SWD* s, tBYTE* dict, tUINT dict_len)
{
    s->dict = s->dict_end = NULL;
    s->dict_len = 0;

    if (!dict || dict_len <= 0)
        return;
    if (dict_len > s->n)
    {
        dict += dict_len - s->n;
        dict_len = s->n;
    }

    s->dict = dict;
    s->dict_len = dict_len;
    s->dict_end = dict + dict_len;
    memcpy(s->b,dict,dict_len);
    s->ip = dict_len;
}

// ----------------------------------------------------------------------------

tVOID swd_insertdict(tUCL_SWD *s, tUINT node, tUINT len)
{
    ucl_uint key;

    s->node_count = s->n - len;
    s->first_rp = node;

    while (len-- > 0)
    {
        key = HEAD3(s->b,node);
        s->succ3[node] = s_head3(s,key);
        s->head3[key] = SWD_UINT(node);
        s->best3[node] = SWD_UINT(s->f + 1);
        s->llen3[key]++;

        key = HEAD2(s->b,node);
        s->head2[key] = SWD_UINT(node);

        node++;
    }
}

// ----------------------------------------------------------------------------

tINT swd_init(tUCL_SWD* s, tBYTE* dict, tUINT dict_len)
{
    ucl_uint i = 0;
    int c = 0;

    if (s->n == 0)
        s->n = SWD_N;
    if (s->f == 0)
        s->f = SWD_F;
    s->threshold = SWD_THRESHOLD;
    if (s->n > SWD_N || s->f > SWD_F)
        return UCL_E_INVALID_ARGUMENT;

    s->b = (unsigned char *) calloc(s->n + s->f + s->f + 16, 1);
    s->head3 = (swd_uint *)  calloc(SWD_HSIZE + 16, sizeof(*s->head3));
    s->succ3 = (swd_uint *)  calloc(s->n + s->f + 16, sizeof(*s->succ3));
    s->best3 = (swd_uint *)  calloc(s->n + s->f + 16, sizeof(*s->best3));
    s->llen3 = (swd_uint *)  calloc(SWD_HSIZE + 16, sizeof(*s->llen3));
    if (!s->b || !s->head3  || !s->succ3 || !s->best3 || !s->llen3)
        return(UCL_E_OUT_OF_MEMORY);

    s->head2 = (swd_uint *) calloc(UCL_UINT32_C(65536), sizeof(*s->head2));
    if (!s->head2)
        return(UCL_E_OUT_OF_MEMORY);

    // defaults
    s->max_chain = SWD_MAX_CHAIN;
    s->nice_length = s->f;
    s->use_best_off = 0;
    s->lazy_insert = 0;

    s->b_size = s->n + s->f;
    if (s->b_size + s->f >= SWD_UINT_MAX)
        return UCL_E_ERROR;
    s->b_wrap = s->b + s->b_size;
    s->node_count = s->n;

    memset(s->llen3, 0, sizeof(s->llen3[0]) * SWD_HSIZE);
    memset(s->head2, 0xff, sizeof(s->head2[0]) * UCL_UINT32_C(65536));

    s->ip = 0;
    swd_initdict(s,dict,dict_len);
    s->bp = s->ip;
    s->first_rp = s->ip;

    s->look = (ucl_uint) (s->c->in_end - s->c->ip);
    if (s->look > 0)
    {
        if (s->look > s->f)
            s->look = s->f;
        memcpy(&s->b[s->ip],s->c->ip,s->look);
        s->c->ip += s->look;
        s->ip += s->look;
    }

    if (s->ip == s->b_size)
        s->ip = 0;

    if (s->look >= 2 && s->dict_len > 0)
        swd_insertdict(s,0,s->dict_len);

    s->rp = s->first_rp;
    if (s->rp >= s->node_count)
        s->rp -= s->node_count;
    else
        s->rp += s->b_size - s->node_count;

    if (s->look < 3)
        memset(&s->b[s->bp+s->look],0,3);

    return UCL_E_OK;
}

// ----------------------------------------------------------------------------

tVOID swd_exit(tUCL_SWD* s)
{
    free(s->head2); s->head2 = NULL;
    free(s->llen3); s->llen3 = NULL;
    free(s->best3); s->best3 = NULL;
    free(s->succ3); s->succ3 = NULL;
    free(s->head3); s->head3 = NULL;
    free(s->b); s->b = NULL;
}

// ----------------------------------------------------------------------------

#define swd_pos2off(s,pos) \
    (s->bp > (pos) ? s->bp - (pos) : s->b_size - ((pos) - s->bp))

// ----------------------------------------------------------------------------

tVOID swd_getbyte(tUCL_SWD *s)
{
    int c;

    if ((c = getbyte(*(s->c))) < 0)
    {
        if (s->look > 0)
            --s->look;
    }
    else
    {
        s->b[s->ip] = UCL_BYTE(c);
        if (s->ip < s->f)
            s->b_wrap[s->ip] = UCL_BYTE(c);
    }
    if (++s->ip == s->b_size)
        s->ip = 0;
    if (++s->bp == s->b_size)
        s->bp = 0;
    if (++s->rp == s->b_size)
        s->rp = 0;
}

// ----------------------------------------------------------------------------
// remove node from lists

tVOID swd_remove_node(tUCL_SWD *s, tUINT node)
{
    if (s->node_count == 0)
    {
        ucl_uint key;

        key = HEAD3(s->b,node);
        --s->llen3[key];

        key = HEAD2(s->b,node);
        if ((ucl_uint) s->head2[key] == node)
            s->head2[key] = NIL2;
    }
    else
        --s->node_count;
}


// ----------------------------------------------------------------------------

tVOID swd_accept(tUCL_SWD *s, ucl_uint n)
{
    if (n > 0) do
    {
        ucl_uint key;

        swd_remove_node(s,s->rp);

        // add bp into HEAD3
        key = HEAD3(s->b,s->bp);
        s->succ3[s->bp] = s_head3(s,key);
        s->head3[key] = SWD_UINT(s->bp);
        s->best3[s->bp] = SWD_UINT(s->f + 1);
        s->llen3[key]++;

#ifdef HEAD2
        // add bp into HEAD2
        key = HEAD2(s->b,s->bp);
        s->head2[key] = SWD_UINT(s->bp);
#endif

        swd_getbyte(s);
    } while (--n > 0);
}

// ----------------------------------------------------------------------------

static tVOID swd_search(tUCL_SWD *s, ucl_uint node, ucl_uint cnt)
{
    const unsigned char *p1;
    const unsigned char *p2;
    const unsigned char *px;

    ucl_uint m_len = s->m_len;
    const unsigned char * b  = s->b;
    const unsigned char * bp = s->b + s->bp;
    const unsigned char * bx = s->b + s->bp + s->look;
    unsigned char scan_end1;

    scan_end1 = bp[m_len - 1];
    for ( ; cnt-- > 0; node = s->succ3[node])
    {
        p1 = bp;
        p2 = b + node;
        px = bx;


        if (
            p2[m_len - 1] == scan_end1 &&
            p2[m_len] == p1[m_len] &&
            p2[0] == p1[0] &&
            p2[1] == p1[1])
        {
            ucl_uint i;

            p1 += 2; p2 += 2;
            do {} while (++p1 < px && *p1 == *++p2);
            i = p1 - bp;

            if (i > m_len)
            {
                s->m_len = m_len = i;
                s->m_pos = node;
                if (m_len == s->look)
                    return;
                if (m_len >= s->nice_length)
                    return;
                if (m_len > (ucl_uint) s->best3[node])
                    return;
                scan_end1 = bp[m_len - 1];
            }
        }
    }
}

// ----------------------------------------------------------------------------

#ifdef HEAD2

static ucl_bool swd_search2(tUCL_SWD *s)
{
    ucl_uint key;


    key = s->head2[ HEAD2(s->b,s->bp) ];
    if (key == NIL2)
        return 0;

    if (s->m_len < 2)
    {
        s->m_len = 2;
        s->m_pos = key;
    }
    return 1;
}

#endif

// ----------------------------------------------------------------------------

tVOID swd_findbest(tUCL_SWD *s)
{
    ucl_uint key;
    ucl_uint cnt, node;
    ucl_uint len;


    /* get current head, add bp into HEAD3 */
    key = HEAD3(s->b,s->bp);
    node = s->succ3[s->bp] = s_head3(s,key);
    cnt = s->llen3[key]++;
    if (cnt > s->max_chain && s->max_chain > 0)
        cnt = s->max_chain;
    s->head3[key] = SWD_UINT(s->bp);

    s->b_char = s->b[s->bp];
    len = s->m_len;
    if (s->m_len >= s->look)
    {
        if (s->look == 0)
            s->b_char = -1;
        s->m_off = 0;
        s->best3[s->bp] = SWD_UINT(s->f + 1);
    }
    else
    {
      if (swd_search2(s))
            if (s->look >= 3)
                swd_search(s,node,cnt);
        if (s->m_len > len)
            s->m_off = swd_pos2off(s,s->m_pos);
        s->best3[s->bp] = SWD_UINT(s->m_len);
    }

    swd_remove_node(s,s->rp);

    // add bp into HEAD2
    key = HEAD2(s->b,s->bp);
    s->head2[key] = SWD_UINT(s->bp);
}



