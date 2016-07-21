#ifndef _SQ_SU_H_15A51B18_39B2_4343_BB40_D0BC677F1D7D
#define _SQ_SU_H_15A51B18_39B2_4343_BB40_D0BC677F1D7D

#ifdef WIN32

    // 4251 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
    // 4786 'identifier' : identifier was truncated to 'number' characters in the debug information
    #pragma warning( disable : 4251 4786 )
#endif

#include <vector>

unsigned int unsqu(const unsigned char *i_buf, unsigned char *o_buf);
unsigned int squeeze(const unsigned char *inf, std::vector<unsigned char> &outf, const unsigned int length);

#endif  // _SQ_SU_H_15A51B18_39B2_4343_BB40_D0BC677F1D7D
