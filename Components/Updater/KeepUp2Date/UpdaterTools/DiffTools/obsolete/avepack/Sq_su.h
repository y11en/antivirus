#ifndef _SQ_SU_H_
#define _SQ_SU_H_

#ifdef __cplusplus
extern "C" {
#endif

DWORD squeeze( SMemFile *inf, SMemFile *outf, DWORD length, BYTE xor_byte );
DWORD unsqu (BYTE *i_buf,BYTE *o_buf);


#ifdef __cplusplus
}
#endif

#endif /*_SQ_SU_H_*/