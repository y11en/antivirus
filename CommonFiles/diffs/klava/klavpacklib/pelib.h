#ifndef _KLAVPACK_PELIB_H_
#define _KLAVPACK_PELIB_H_  "Simple PE parser/editor"

#ifdef _WIN32
# include <windows.h>    /* common pe structures only */
#else
# include <kl_pe.h>
# define IMAGE_NT_HEADERS 		KL_PE_NT_HEADERS
# define IMAGE_SECTION_HEADER 		KL_PE_SECTION_HEADER
# define IMAGE_DOS_HEADER 		KL_PE_DOS_HEADER
# define IMAGE_DOS_SIGNATURE 		KL_PE_DOS_SIGNATURE
# define IMAGE_NT_SIGNATURE 		KL_PE_NT_SIGNATURE
# define IMAGE_DIRECTORY_ENTRY_RESOURCE KL_PE_DIRECTORY_ENTRY_RESOURCE
# define IMAGE_DIRECTORY_ENTRY_BASERELOC KL_PE_DIRECTORY_ENTRY_BASERELOC
# define IMAGE_DIRECTORY_ENTRY_IMPORT	KL_PE_DIRECTORY_ENTRY_IMPORT
# define IMAGE_SCN_MEM_WRITE		KL_PE_SCN_MEM_WRITE
#endif
#include "section_writer.h"
#include "types.h"

/* ------------------------------------------------------------------------- */

class CPELibrary
{
public:
  CPELibrary();
 ~CPELibrary();

  tVOID  Reset();

  tBOOL  MapFromFileBuffer(const tVOID* Buffer, tUINT BufferSize);
  tBOOL  SaveMapToBuffer(tVOID* Buffer, tUINT BufferSize);
  tUINT  GetMapSize() { return(MapSize); }
  tBYTE* GetMap()     { return(Map); }
  tBOOL  NormalizeImageMap();
  tBOOL  OptimizePEHeader();
  tUINT  GetNewHeadSize();
  tUINT  GetNewFileSize();

  IMAGE_NT_HEADERS*      GetMapPEHeader();
  IMAGE_SECTION_HEADER*  GetMapPESections();
  tBYTE* GetOriginalPEHeader(tUINT* HdrSize, tUINT* HdrOffs);

  tBOOL  AddLoaderSection(CSectionWriter* ObjWrt);

  tBYTE* MemRealloc(tVOID* Data, tUINT Size) { return (tBYTE*)realloc(Data, Size); };
  tBYTE* MemAlloc(tUINT Size) { return (tBYTE*)malloc(Size); };
  tVOID  MemFree(tVOID* Data) { if ( Data ) free(Data); }

private:
  tBYTE* Map;
  tBYTE* Overlay;
  tBYTE* OriginalHeader;
  tUINT  OriginalHeaderSize;
  tUINT  OriginalHeaderOffs;
  tUINT  MapSize;
  tUINT  OverlaySize;

  tBOOL  CopyOriginalHeader();
};

/* ------------------------------------------------------------------------- */

#endif /* _KLAVPACK_PELIB_H_ */
