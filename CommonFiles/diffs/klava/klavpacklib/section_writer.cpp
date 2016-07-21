#include <stdlib.h>
#include <memory.h>
#include "section_writer.h"
#include "types.h"

/* ------------------------------------------------------------------------- */

#ifndef ALIGN
# define ALIGN(X,Y)     (((X)+(Y)-1)&(~((Y)-1)))
#endif

#define SECTION_WRITER_BLOCK_SIZE  4096

/* ------------------------------------------------------------------------- */

CSectionWriter::CSectionWriter(tUINT StartRVA)
{
  Data = NULL;
  DataBufferSize = 0;
  CurrentOffset  = 0;
  SectionRVA = StartRVA;
}

CSectionWriter::~CSectionWriter()
{
  if ( Data )
    free(Data);
}

/* ------------------------------------------------------------------------- */

tUINT CSectionWriter::WriteData(tVOID* Buffer, tUINT BufferSize)
{
  if ( !EnsureSize(CurrentOffset + BufferSize) )
    return(0);

  memcpy(Data + CurrentOffset, Buffer, BufferSize);
  CurrentOffset += BufferSize;

  return(BufferSize);
}

/* ------------------------------------------------------------------------- */

tBOOL CSectionWriter::AlignOffset(tUINT Alignment)
{
  tUINT NewOfs;
  tUINT Len;

  NewOfs = ALIGN(CurrentOffset, Alignment);
  if ( !EnsureSize(NewOfs) )
    return(cFALSE);

  if ( 0 != (Len = NewOfs - CurrentOffset) )
    memset(Data + CurrentOffset, 0, Len);

  CurrentOffset = NewOfs;
  return(cTRUE);
}

/* ------------------------------------------------------------------------- */

tBOOL CSectionWriter::EnsureSize(tUINT Size)
{
  if ( !Data || Size > DataBufferSize )
  {
    Size = ALIGN(Size, SECTION_WRITER_BLOCK_SIZE);
    if ( NULL == (Data = (tBYTE*)realloc(Data, Size)) )
      return(cFALSE);

    DataBufferSize = Size;
  }

  return(cTRUE);
}