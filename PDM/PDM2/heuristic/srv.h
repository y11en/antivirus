#ifndef __SRV_FUNC_H
#define __SRV_FUNC_H

#include "../eventmgr/include/types.h"
#include "../eventmgr/include/envhelper.h"
#include "../eventmgr/include/cpath.h"
#include "../eventmgr/include/pathlist.h"

//tcstring   FileGetFullPath(tcstring path);
bool srvGetFileFormat(cEnvironmentHelper* pEnvironmentHelper, cFile& file);
bool srvIsFileExecutable(cEnvironmentHelper* pEnvironmentHelper, cFile& file);
bool srvIsImageCopy(cEnvironmentHelper* pEnvironmentHelper, cFile& src_file, cFile& dst_file);
size_t srvParseCmdLine(cEnvironmentHelper* pEnvironmentHelper, tstring sCmdLine, tcstring sDefaultDir, tcstring* params, size_t max_params_count, bool bInterpreteur = false);
bool srvComparePath(cEnvironmentHelper* pEnvironmentHelper, tcstring sPath1, tcstring sPath2, bool bCompareImages = false);
bool srvIsWinSystemFile( cEnvironmentHelper* pEnvironmentHelper, cFile &file, bool* pbSystemName = NULL,  bool* pbSystemPath = NULL);

#endif // __SRV_FUNC_H
