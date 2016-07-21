#include <stdlib.h>
#include <memory.h>
#include "../shared/types.h"
#include "../shared/diff.h"
#include "ucl.h"

// ----------------------------------------------------------------------------

//#define fail(x,r)   if (x) { *dst_len = olen; return r; }
#define fail(x,r)
#define getbit_8(bb, src, ilen) \
    (((bb = bb & 0x7f ? bb*2 : ((unsigned)src[ilen++]*2+1)) >> 8) & 1)
#define getbit(bb)      getbit_8(bb,src,ilen)

// ----------------------------------------------------------------------------

tINT  UCL_Decompress(tDIFF_HDR* Hdr, tBYTE* src, tUINT src_len, tBYTE* dst,
                     tUINT* dst_len)
{
    unsigned   bc = 0;
    ucl_uint32 bb = 0;
    ucl_uint ilen = 0, olen = 0, last_m_off = 1;
    const ucl_uint oend = *dst_len;
    unsigned char* r_ptr;
    unsigned char* r_max;
    unsigned char* r_base;

// ----
    memcpy(&dst[Hdr->BaseSize], &src[0], Hdr->Delta);
    r_base = r_ptr = &dst[Hdr->BaseSize+Hdr->Delta];
    r_max  = &dst[Hdr->BaseSize+Hdr->CodeSize];
    dst += Hdr->BaseSize;
    olen = Hdr->Delta;
    src += Hdr->Delta;
    src_len -= Hdr->Delta;
// ----

    for (;;)
    {
        ucl_uint m_off, m_len;
        while (getbit(bb))
        {
            fail(ilen >= src_len, UCL_E_INPUT_OVERRUN);
            fail(olen >= oend, UCL_E_OUTPUT_OVERRUN);
            dst[olen++] = src[ilen++];
        }
        m_off = 1;
        for (;;)
        {
            m_off = m_off*2 + getbit(bb);
            fail(ilen >= src_len, UCL_E_INPUT_OVERRUN);
            fail(m_off > UCL_UINT32_C(0xffffff) + 3, UCL_E_LOOKBEHIND_OVERRUN);
            if (getbit(bb)) break;
            m_off = (m_off-1)*2 + getbit(bb);
        }
        if (m_off == 2)
        {
            m_off = last_m_off;
            m_len = getbit(bb);
        }
        else
        {
            fail(ilen >= src_len, UCL_E_INPUT_OVERRUN);
            m_off = (m_off-3)*256 + src[ilen++];
            if (m_off == UCL_UINT32_C(0xffffffff))
                break;
            m_len = (m_off ^ UCL_UINT32_C(0xffffffff)) & 1;
            m_off >>= 1;
            last_m_off = ++m_off;
        }
        m_len = m_len*2 + getbit(bb);
        if (m_len == 0)
        {
            m_len++;
            do {
                m_len = m_len*2 + getbit(bb);
                fail(ilen >= src_len, UCL_E_INPUT_OVERRUN);
                fail(m_len >= oend, UCL_E_OUTPUT_OVERRUN);
            } while (!getbit(bb));
            m_len += 2;
        }
        m_len += (m_off > 0x500);
        fail(olen + m_len > oend, UCL_E_OUTPUT_OVERRUN);
        fail(m_off > olen, UCL_E_LOOKBEHIND_OVERRUN);
        {
            const ucl_byte *m_pos;
            m_pos = dst + olen - m_off;
            dst[olen++] = *m_pos++;
            do dst[olen++] = *m_pos++; while (--m_len > 0);
        }
    }
    *dst_len = olen;
    return ilen == src_len ? UCL_E_OK : (ilen < src_len ? UCL_E_INPUT_NOT_CONSUMED : UCL_E_INPUT_OVERRUN);
}

// ----------------------------------------------------------------------------
