#ifndef _KLAVPACK_SECTION_WRITER_H_
#define _KLAVPACK_SECTION_WRITER_H_    "Simple IO wrapper for loader section"

#include <stdlib.h>
#include <memory.h>
#include "types.h"

/* ------------------------------------------------------------------------- */

class CSectionWriter
{
public:
  CSectionWriter(tUINT StartRVA = 0);
 ~CSectionWriter();

  tUINT  WriteData(tVOID* Buffer, tUINT BufferSize);
  tBOOL  AlignOffset(tUINT Alignment);

  tVOID  SetStartRVA(tUINT StartRVA)     { SectionRVA = StartRVA; }
  tUINT  GetCurrentRVA()    { return(CurrentOffset + SectionRVA); }
  tUINT  GetCurrentOffset() { return(CurrentOffset); }
  tBYTE* GetDataBuffer()    { return(Data); }
  tUINT  GetDataSize()      { return(CurrentOffset); }

private:
  tBYTE* Data;
  tUINT  DataBufferSize;
  tUINT  CurrentOffset;
  tUINT  SectionRVA;

  tBOOL  EnsureSize(tUINT Size);
};

/* ------------------------------------------------------------------------- */

#endif /* _KLAVPACK_SECTION_WRITER_H_ */
