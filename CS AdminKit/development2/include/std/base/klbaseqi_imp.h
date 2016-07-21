/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	klbaseqi_imp.h
 * \author	Andrew Kazachkov
 * \date	15.10.2003 20:26:52
 * \brief	
 * 
 */

/*KLCSAK_PUBLIC_HEADER*/

#ifndef __KLBASEQI_IMP_H__
#define __KLBASEQI_IMP_H__

#define KLSTD_INTERAFCE_MAP_BEGIN(_main) \
    bool QueryInterface(KLSTD::KLSTD_IID iid, void** ppObject) \
    {   \
        KLSTD_CHKOUTPTR(ppObject);  \
        if(iid == KLSTD::KLBaseQI_IID || iid == KLSTD_IIDOF(_main))  \
        {   \
            *ppObject=static_cast<_main*>(this);   \
            static_cast<_main*>(this)->AddRef(); \
        }

#define KLSTD_INTERAFCE_MAP_ENTRY(_name)    \
        else if(iid == KLSTD_IIDOF(_name))  \
        {   \
            *ppObject=static_cast<_name*>(this);   \
            static_cast<_name*>(this)->AddRef(); \
        }

#define KLSTD_INTERAFCE_MAP_ENTRY_TEAR_OFF(_name, _creator)    \
        else if(iid == KLSTD_IIDOF(_name))  \
        {   _name* p=NULL;  \
            _creator(&p); \
            if(p)p->AddRef();   \
        }

#define KLSTD_INTERAFCE_MAP_ENTRY_CHILD(_name, _var)    \
        else if(iid == KLSTD_IIDOF(_name))  \
        {   \
            *ppObject=static_cast<_name*>(&_var); \
            static_cast<_name*>(&_var)->AddRef(); \
        }

#define KLSTD_INTERAFCE_MAP_END() \
        else    \
            *ppObject=NULL; \
        return ((*ppObject)!=NULL); \
    };

#define KLSTD_SINGLE_INTERAFCE_MAP(_name)   \
        KLSTD_INTERAFCE_MAP_BEGIN(_name)    \
        KLSTD_INTERAFCE_MAP_END()

#define KLSTD_INTERAFCE_MAP_REDIRECT(_ptr)  \
	virtual unsigned long AddRef()  \
    {   \
        return _ptr->AddRef();  \
    };  \
    \
	virtual unsigned long Release() \
    {   \
        return _ptr->Release(); \
    }   \
    \
    virtual bool QueryInterface(KLSTD::KLSTD_IID iid, void** ppObject) \
    {   \
        return _ptr->QueryInterface(iid, ppObject); \
    };

#endif //__KLBASEQI_IMP_H__
