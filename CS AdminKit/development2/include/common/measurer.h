/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	measurer.h
 * \author	Andrew Kazachkov
 * \date	07.03.2003 14:01:52
 * \brief	
 * 
 */
/*KLCSAK_PUBLIC_HEADER*/

#ifndef __MEASURER_H__KMn0zoPhs2qi5bcuOq_Tr1
#define __MEASURER_H__KMn0zoPhs2qi5bcuOq_Tr1

#include "std/time/kltime.h"
#include "std/trc/trace.h"


/*KLCSAK_BEGIN_PRIVATE*/

namespace KLDBG
{ 
    struct measure_times
    {
        size_t          m_nVersion;
        unsigned long   m_clkStart;
        AVP_qword       m_qwUserTime, 
                        m_qwKernelTime;        
    };
};

KLCSC_DECL bool KLDBG_StartMeasure(
                        const wchar_t*          szwModule, 
                        const wchar_t*          szwMessage, 
                        long                    nTraceLevel, 
                        KLDBG::measure_times&   mt);

KLCSC_DECL void KLDBG_EndMeasure(
                        const wchar_t*          szwModule, 
                        const wchar_t*          szwMessage, 
                        long                    nTraceLevel, 
                        KLDBG::measure_times&   mt);

namespace KLDBG
{    
    class CDbgTimeMeasurer2
    {
    public:
        CDbgTimeMeasurer2(const wchar_t* szwModule, const wchar_t* szwMessage, long nTraceLevel)
            :   m_szwMessage(szwMessage)
            ,   m_szwModule(szwModule)
            ,   m_nTraceLevel(nTraceLevel)
            ,   m_bResult(false)
    {
        m_oTimes.m_nVersion = 1;
        m_bResult = KLDBG_StartMeasure(szwModule, szwMessage, nTraceLevel, m_oTimes);
    };

    virtual ~CDbgTimeMeasurer2()
    {
        if(m_bResult)
            KLDBG_EndMeasure(m_szwModule, m_szwMessage, m_nTraceLevel, m_oTimes);
    };
    private:
        measure_times   m_oTimes;
        const wchar_t   *m_szwMessage, *m_szwModule;
        long			m_nTraceLevel;
        bool            m_bResult;
    };

#define KL_TMEASURE_BEGIN(_msg, _level) \
 { \
   KLDBG::CDbgTimeMeasurer2 dbgmsr(KLCS_MODULENAME, _msg, _level);

#define KL_TMEASURE_END() };

};

/*KLCSAK_END_PRIVATE*/

//KLCSAK_BEGIN_PRIVATE
/*
//KLCSAK_END_PRIVATE

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

//KLCSAK_BEGIN_PRIVATE
*/
//KLCSAK_END_PRIVATE


#endif //__MEASURER_H__KMn0zoPhs2qi5bcuOq_Tr1

// Local Variables:
// mode: C++
// End:
