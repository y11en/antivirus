#include "updateSchedule.h"

#include "../helper/updaterStaff.h"

const KLUPD::NoCaseString KLUPD::UpdateSchedule::s_attributeTimeoutOnSuccess = L"SuccPeriod";
const KLUPD::NoCaseString KLUPD::UpdateSchedule::s_attributeTimeoutOnFailure = L"FailPeriod";

KLUPD::UpdateSchedule::UpdateSchedule(const Minutes &timeoutOnSuccess, const Minutes &timeoutOnFailure)
 : m_timeoutOnSuccess(timeoutOnSuccess),
   m_timeoutOnFailure(timeoutOnFailure)
{
}

void KLUPD::UpdateSchedule::fromXML(const XmlAttrMap &attributes)
{
    for(XmlAttrMap::AttrBag::const_iterator attributesIter = attributes.m_attrs.begin(); attributesIter != attributes.m_attrs.end(); ++attributesIter)
    {
        if(s_attributeTimeoutOnSuccess == asciiToWideChar(attributesIter->name))
            NoCaseString(asciiToWideChar(attributesIter->value)).toLong(m_timeoutOnSuccess);
        else if(s_attributeTimeoutOnFailure == asciiToWideChar(attributesIter->name.c_str()))
            NoCaseString(asciiToWideChar(attributesIter->value)).toLong(m_timeoutOnFailure);
    }
}
