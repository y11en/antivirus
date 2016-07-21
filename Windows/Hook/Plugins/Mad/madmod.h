#ifndef _MAD_MODULE_H_
#define _MAD_MODULE_H_

#include "kl_plugmod.h"

#define MOD_VER	0x1

class CKl_MadModule : public CKl_PluginModule
{
public:
    CKl_MadModule();
    virtual ~CKl_MadModule();
};
#endif