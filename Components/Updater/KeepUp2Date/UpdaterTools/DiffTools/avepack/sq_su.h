#ifndef _SQ_SU_H_
#define _SQ_SU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fakeoop.h"

unsigned long unsqu   (unsigned char* i_buf, unsigned char* o_buf);

unsigned long squeeze (SMemFile* inf, SMemFile* outf, unsigned long length,
                       unsigned char xor_byte);

#ifdef __cplusplus
}
#endif

#endif /* _SQ_SU_H_ */
