#if !defined(AFX_UPDATESCHEDULE_H__05266BA3_14BA_4F1B_AEF1_302F547C3F6E__INCLUDED_)
#define AFX_UPDATESCHEDULE_H__05266BA3_14BA_4F1B_AEF1_302F547C3F6E__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "../helper/stdinc.h"

namespace KLUPD {

struct KAVUPDCORE_API UpdateSchedule
{
    typedef size_t Minutes;
    
    UpdateSchedule(const Minutes &timeoutOnSuccess = 2 * 60, const Minutes &timeoutOnFailure = 20);
    
    // loads schedule information from XML
    void fromXML(const XmlAttrMap &);
    
    Minutes m_timeoutOnSuccess;
    Minutes m_timeoutOnFailure;
    
private:
    // advisory information for product when next update is to be launched in case success update operation
    static const KLUPD::NoCaseString s_attributeTimeoutOnSuccess;
    // advisory information for product when next update is to be launched in case update failure
    static const KLUPD::NoCaseString s_attributeTimeoutOnFailure;
};


}   // namespace KLUPD

#endif // !defined(AFX_UPDATESCHEDULE_H__05266BA3_14BA_4F1B_AEF1_302F547C3F6E__INCLUDED_)
