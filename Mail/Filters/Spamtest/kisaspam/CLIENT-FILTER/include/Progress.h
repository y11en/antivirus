#ifndef  _PROGRESS_H
#define  _PROGRESS_H

/*!
 * (C) 2003-2005 Ashmanov and Partners JSC
 *
 * \file 
 * \author A&P
 * \date 2003
 * \brief Time consuming operation progress dialog. Abstract class
*/

#ifndef  _PROGRESS_RH
#include "progress.rh"
#endif

#ifndef  _CRITSECT_H
#include "critsect.h"
#endif

//! Abstract progress dialog
class Progress
{
public:
    Progress();
    virtual ~Progress();

    virtual void SetHeader(LPCTSTR /*lpszHeader*/, int /*idIcon*/=0) {}      //!< Set dialog box header string 
    virtual void SetStatic(int /*id*/, LPCTSTR /*lpszText*/) {}//!< Set static text

    virtual void SetTotal(unsigned long total);     //!< Set progress control range
    virtual void IncreaseTotal(unsigned long delta);//!< Modify progress control range 
    virtual void DecreaseTotal(unsigned long delta);//!< Modify progress control range 
    virtual void SetPosition(unsigned long current);     //!< Set progress control state
    virtual void IncreasePosition(unsigned long delta);     //!< Change progress control state

    virtual void Abort();
    virtual bool IsAborted() const = 0;
    virtual HWND GetParent() const { return 0; }
    virtual HWND GetWindow() const { return 0; }

    virtual void ShowPosition() = 0;

    typedef void (*AbortFunc)(unsigned long param);

    virtual void SetAbortFunc(AbortFunc abort, unsigned long param);
protected:
    CriticalSection m_sect;
    AbortFunc m_abort; 
    unsigned long m_abortParam;
    

    unsigned long m_total;
    unsigned long m_current;
};



#endif // _PROGRESS_H



