/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	measurer.h
 * \author	Andrew Kazachkov
 * \date	07.03.2003 14:01:52
 * \brief	
 * 
 */

#ifndef __MEASURER_H__KMn0zoPhs2qi5bcuOq_Tr1
#define __MEASURER_H__KMn0zoPhs2qi5bcuOq_Tr1

#include "std/time/kltime.h"
#include "std/trc/trace.h"




namespace KLDBG
{    
class CDbgTimeMeasurer
{
  public:

    CDbgTimeMeasurer(const wchar_t* szwModule, const wchar_t* szwMessage, long nTraceLevel) :
        m_szwMessage(szwMessage),
        m_szwModule(szwModule),
        m_nTraceLevel(nTraceLevel),
        m_clkStart(KLSTD::GetSysTickCount())
      {
        KLSTD::Trace(m_nTraceLevel, m_szwModule, L"Starting %ls...\n", m_szwMessage);
      }

    virtual ~CDbgTimeMeasurer()
      {
        unsigned long lDelta=KLSTD::GetSysTickCount()-m_clkStart;
        KLSTD::Trace(m_nTraceLevel, m_szwModule, L"...%ls took %d ms\n", m_szwMessage, lDelta);
      }

  private:
    unsigned long   m_clkStart;
    const wchar_t   *m_szwMessage, *m_szwModule;
    long			m_nTraceLevel;
};

#define KL_TMEASURE_BEGIN(_msg, _level) \
 { \
   KLDBG::CDbgTimeMeasurer dbgmsr(KLCS_MODULENAME, _msg, _level);

#define KL_TMEASURE_END() };

};



#endif //__MEASURER_H__KMn0zoPhs2qi5bcuOq_Tr1

// Local Variables:
// mode: C++
// End:
