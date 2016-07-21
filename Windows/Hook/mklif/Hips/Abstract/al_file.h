#ifndef _AL_FILE_H_4541231843434_215846
#define _AL_FILE_H_4541231843434_215846

#include "al_base.h"

typedef __int32 int32;
typedef __int64 int64;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

#ifndef SEEK_SET
#define SEEK_SET        0               /* seek to an absolute position */
#define SEEK_CUR        1               /* seek relative to current position */
#define SEEK_END        2               /* seek relative to end of file */
#endif  /* ifndef SEEK_SET */

AL_DECLARE_INTERFACE(alFile) AL_INTERFACE_BASED(alBase, base)
AL_DECLARE_INTERFACE_BEGIN
	AL_DECLARE_METHOD4(bool, SeekRead, uint64 nPos, void* pData, size_t nSize, size_t* pnBytesRead);
	AL_DECLARE_METHOD4(bool, SeekWrite, uint64 nPos, const void* pData, size_t nSize, size_t* pnBytesWritten);
	AL_DECLARE_METHOD3(bool, Read, void* pData, size_t nSize, size_t* pnBytesRead);
	AL_DECLARE_METHOD3(bool, Write, const void* pData, size_t nSize, size_t* pnBytesWritten);
	AL_DECLARE_METHOD3(bool, Seek, int64 nSeek, int nSeekMethod, uint64* pnNewPos);
	AL_DECLARE_METHOD1(bool, GetSize, uint64* pnSize);
	AL_DECLARE_METHOD1(bool, SetSize, uint64 nSize);
	AL_DECLARE_METHOD0(bool, Flush);
	AL_DECLARE_METHOD0(bool, Close);
AL_DECLARE_INTERFACE_END

#endif //_AL_FILE_H_4541231843434_215846
