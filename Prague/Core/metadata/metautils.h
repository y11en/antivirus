#if !defined (_METAUTILS_H_)
#define _METAUTILS_H_

#include "metadata.h"

  int load ( PluginMetadata& aMetadata, const char* aFileName ); 
  int save ( PluginMetadata& aMetadata, const char* aFileName, bool aNeedSwap = false );       
#endif // _METAUTILS_H_
