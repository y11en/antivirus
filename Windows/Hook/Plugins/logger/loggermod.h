#ifndef _IDS_MODULE_H_
#define _IDS_MODULE_H_

#include "klned\kl_plugmod.h"

#define MOD_VER	0x1

class CKl_LoggerModule : public CKl_PluginModule
{
public:
    CKl_LoggerModule();
    virtual ~CKl_LoggerModule();
};
#endif