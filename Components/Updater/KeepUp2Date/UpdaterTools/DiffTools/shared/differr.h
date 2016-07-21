#ifndef _DIFFERR_H_
#define _DIFFERR_H_  "Global error codes for diff finder"

// ----------------------------------------------------------------------------

#define DIFF_ERR_OK          (0)                 // no error here
#define DIFF_ERR_PARAM      (-1)                 // incompatible argument
#define DIFF_ERR_NOMEM      (-2)                 // not enought memory
#define DIFF_ERR_FILE       (-3)                 // io operation failed
#define DIFF_ERR_SIZE       (-4)                 // too big block
#define DIFF_ERR_OTHER      (-5)                 // internal engine error
#define DIFF_ERR_CORRUPT    (-6)                 // corrupted diff file
#define DIFF_ERR_VERSION    (-7)                 // incompatible version
#define DIFF_ERR_AVCPACK    (-8)                 // ave -> avc repack fail
#define DIFF_ERR_DATA       (-9)                 // incompatible base data
#define DIFF_ERR_SIGN       (-10)                // can't sign diff-file
#define DIFF_ERR_BAD16      (-11)                // corrupt 16-bit link
#define DIFF_ERR_PACK       (-12)                // some lzma pack error
#define DIFF_ERR_NAMEGEN    (-13)                // can't generate diff name
#define DIFF_ERR_BUFF       (-14)                // too small buffer

// ----------------------------------------------------------------------------

#endif // _DIFFERR_H_
