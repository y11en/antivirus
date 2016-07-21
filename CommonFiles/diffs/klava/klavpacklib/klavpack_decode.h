#ifndef _KLAVPACK_DECODE_H_
#define _KLAVPACK_DECODE_H_  "Windows DLL Decoder for KLAVA plugins and databases"


#ifdef WIN32
    // 4251 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
    // 4786 'identifier' : identifier was truncated to 'number' characters in the debug information
    #pragma warning( disable : 4251 4786 )
#endif

#include <vector>
#include "types.h"
#include "../../diffs.h"

/* ------------------------------------------------------------------------- */

extern bool KLAVPACK_API KlavPack_IsPacked(const std::vector<unsigned char> &SourceVector);

extern bool KLAVPACK_API KlavPack_DecodeVector(const std::vector<unsigned char> &SourceVector,
                           std::vector<unsigned char> &DestinationVector);

extern bool KLAVPACK_API KlavPack_KFB_IsPacked(const std::vector<unsigned char> &SourceVector);

extern bool KLAVPACK_API KlavPack_KFB_DecodeVector(const std::vector<unsigned char> &SourceVector,
                           std::vector<unsigned char> &DestinationVector);

/* ------------------------------------------------------------------------- */

#endif /* _KLAVPACK_DECODE_H_ */
