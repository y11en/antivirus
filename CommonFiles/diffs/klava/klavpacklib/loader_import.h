#ifndef _KLAVPACK_LOADER_IMPORT_H_
#define _KLAVPACK_LOADER_IMPORT_H_  "Loader impor table generator"

#include "dlist.h"
#include "types.h"

/* ------------------------------------------------------------------------- */

class CLdrImport
{
public:
  CLdrImport();
 ~CLdrImport();

  tBOOL  GrabDataFromImage(tVOID* ImageMap);
  tBYTE* GetImportTable(tUINT NewImpRVA, tUINT* DataSize, tUINT* TblSize);
  tVOID  Reset();

private:
  CDynList ImpList;
  tBYTE* ImpData;
  tUINT  ImpTblSize;
  tUINT  ImpDataSize;

  tBOOL  ModulePresent(tCHAR* ModuleName);
};

/* ------------------------------------------------------------------------- */

#endif /* _KLAVPACK_LOADER_IMPORT_H_ */
