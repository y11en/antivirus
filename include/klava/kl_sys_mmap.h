// kl_sys_mmap.h
//
// Helper classes for memory-mapped file support
//

#ifndef kl_sys_mmap_h_INCLUDED
#define kl_sys_mmap_h_INCLUDED

#include "../../CommonFiles/klsys/filemapping/filemapping.h"
#include "../../CommonFiles/klsys/filemapping/wholefilemapping.h"

using KLSysNS::FileMapping;
using KLSysNS::WholeFileMapping;

#define KLSYS_MMAP_ACCESS_HINT      KLSysNS::FileMapping::AccessHints

#define KLSYS_MMAP_HINT_DEFAULT     KLSysNS::FileMapping::ahNormalAccess
#define KLSYS_MMAP_HINT_SEQUENTIAL  KLSysNS::FileMapping::ahSequentialAccess
#define KLSYS_MMAP_HINT_RANDOM      KLSysNS::FileMapping::ahRandomAccess

#endif // kl_sys_mmap_h_INCLUDED

