

#include "build/general.h"

#include "std/time/kltime.h"
#include "std/thr/sync.h"
#include "common/measurer.h"

#include<time.h>

#ifdef _WIN32
    #include <std/win32/klos_win32v40.h>
#endif

#ifdef N_PLAT_NLM
#  include <nwsemaph.h>
#  include <nwthread.h>
#  include <nwtime.h>
#endif

#ifdef __unix
    #include <sys/times.h>
    #include <unistd.h>
#endif

#ifdef USE_BOOST_THREADS
	#include <boost/thread/thread.hpp>
	#include <boost/thread/mutex.hpp>
	#include <boost/thread/recursive_mutex.hpp>
	#include <boost/thread/condition.hpp>
	#include <boost/thread/xtime.hpp>
#endif

using namespace KLSTD;

#define KLCS_MODULENAME L"KLSTD"


KLCSC_DECL  KLSTD::klstd_time_t KLSTD_GetSystemTime(KLSTD::klstd_time_t* secs, unsigned* ms)
{
    time_t secs_ = c_invalid_time_t;
    long ms_ = 0; 
    KLSTD_GetPreciseTime(secs_, ms_);
    if(secs)
        *secs = (klstd_time_t)secs_;
    if(ms)
        *ms = (unsigned)ms_;
    return (klstd_time_t)secs_;
};

/*!
  \brief	Returns millisecond UTC time
*/
KLCSC_DECL  KLSTD::klstd_mstime_t KLSTD_GetMsTime()
{
    klstd_time_t    secs = c_tmInvalidSysTime;
    unsigned        ms = 0;
    KLSTD_GetSystemTime(&secs, &ms);
    return KLSTD_Time2MsTime(secs, ms);
};

KLCSC_DECL  klstd_mstime_t KLSTD_Time2MsTime(klstd_time_t t, unsigned ms)
{
    if( !t || c_tmInvalidSysTime == t )
        return c_tmInvalidMsecTime;
    return t*1000 + ms;
};

KLCSC_DECL  klstd_time_t KLSTD_MsTime2Time(klstd_mstime_t t, klstd_time_t* sec, unsigned* ms)
{
    klstd_time_t res;
    if( !t || c_tmInvalidMsecTime == t )
    {
        res = c_tmInvalidSysTime;
        if( ms )
            *ms = 0;
    }
    else
    {
        res = t/1000;
        if( ms )
            *ms = (unsigned)t%1000;
    };
    if( sec )
        *sec = res;
    return res;
};

namespace KLSTD
{

#ifdef __unix
    /* andkaz:  ugly implementation that uses current time. Ugly because time may change. 
                we use it inly in extraordinary cases
    */
    namespace
    {
        class CInitBoost_KLSTD_GetSysTickCount
        {
        public:
            CInitBoost_KLSTD_GetSysTickCount()
            {            
                boost::xtime_get(&m_xtStart, boost::TIME_UTC);
            };
        
            ~CInitBoost_KLSTD_GetSysTickCount()
            {
                ;
            };

            KLSTD::kltick_t GetTimeSinceStart()
            {
	            boost::xtime xt;
                boost::xtime_get(&xt, boost::TIME_UTC);
                return 24*3600*1000 + AVP_qword(ConvertTime(xt) - ConvertTime(m_xtStart));
            };
        protected:
            KLSTD::kltick_t ConvertTime(const boost::xtime& xt)
            {
	            AVP_longlong res = xt.nsec;
	            res /= 1000000; // ms
	            res += (static_cast<AVP_longlong>( xt.sec ) % 1000000) * 1000;
	            return res;
            };
        protected:
            boost::xtime m_xtStart;
        }g_Boost_GetSysTickCount;
    };

    const long c_lSC_CLK_TCK = sysconf(_SC_CLK_TCK);

    struct lSC_CLK_TCK_Base
    {
        lSC_CLK_TCK_Base()
        {
            struct tms tm;
            m_nBase = (unsigned long)times(&tm);
        };
        unsigned long m_nBase;
    }
    g_clkBase;

#endif

	unsigned long GetSysTickCountI()
    {
#ifdef _WIN32
        return ::GetTickCount();
#elif N_PLAT_NLM
        return GetCurrentTicks() * 55;
#elif __unix
        unsigned long ulResult = 0;
        if(-1L == c_lSC_CLK_TCK || 0L == c_lSC_CLK_TCK)
        {// we faild to acuire c_lSC_CLK_TCK from the kernel
            ulResult = (unsigned long)g_Boost_GetSysTickCount.GetTimeSinceStart();
        }
        else
        {
            struct tms tm;
            //ulResult = ((unsigned long)times(&tm))*1000/((unsigned long)c_lSC_CLK_TCK);
            ulResult = ((unsigned long)times(&tm)) - g_clkBase.m_nBase + (unsigned long)c_lSC_CLK_TCK;
        };
        return ulResult;
#endif
    }

    class CGetSysTickCountI
    {
    public:
        CGetSysTickCountI()
            :   m_nBase(GetSysTickCountI())
        {
#ifdef _WIN32
            bool bUpdated = false;
            CRegKey key;
            if( 0 == key.Open(
                    HKEY_LOCAL_MACHINE,
                    _T("SOFTWARE\\KasperskyLab\\Components\\34\\.core\\.independent\\Debug"),
                    KEY_READ))
            {
                DWORD dwCountBase = 0;
                key.QueryValue(dwCountBase, _T("CountBase"));
                if(dwCountBase)
                {
                    dwCountBase *= 1000;
                    m_nBase = GetSysTickCountI() - (ULONG_MAX - (unsigned long)dwCountBase);
                    bUpdated = true;
                };
            };
            if(!bUpdated)
                m_nBase -= 1000;
#endif
        }
        ~CGetSysTickCountI()
        {
            ;
        };
        inline unsigned long GetBase()
        {
            return m_nBase;
        };
    protected:
        unsigned long m_nBase;
    }
    g_oGetSysTickCount;

	unsigned long GetSysTickCount()
    {
#ifndef __unix
        return GetSysTickCountI() - g_oGetSysTickCount.GetBase();
#else
        if(-1L == c_lSC_CLK_TCK || 0L == c_lSC_CLK_TCK)
            return GetSysTickCountI();
        return (unsigned long)(AVP_qword)KLSTD_GetSysTickCount();
#endif
    };



    //!\brief Возращает текущее количество милисекунд
    // int GetCurrentMillisecondsString()
    unsigned long GetCurrentMilliseconds()
    {
    #ifdef USE_BOOST_THREADS
		    boost::xtime xt;
		    boost::xtime_get(&xt, boost::TIME_UTC);

		    return xt.nsec / 1000000;
    #else
	    #ifdef _WIN32
		    SYSTEMTIME tempSysTime;
		    GetLocalTime( &tempSysTime );
		    return tempSysTime.wMilliseconds;
	    #endif
	    #ifdef __unix
		    timespec t;
		    __impl::Thread::gettime( &t );
		    return t.tv_nsec / 1000000;
	    #endif
	    #ifdef N_PLAT_NLM
		    return 0;//time(0) * 1000;
	    #endif
    #endif
    }

} // namespace KLSTD


namespace KLSTD
{
    extern KLSTD::CAutoPtr<KLSTD::CriticalSection> g_pSysTickCountCS;
    unsigned long g_ulTickCountHi = 0;
    unsigned long g_ulTickCountLo = 0;
};

using namespace KLSTD;

KLCSC_DECL KLSTD::kltick_t KLSTD_GetSysTickCount()
{
    AVP_qword result = 0;
    {
        KLSTD_ASSERT(KLSTD::g_pSysTickCountCS);
        KLSTD::AutoCriticalSection acs(KLSTD::g_pSysTickCountCS);
        const unsigned long ulTickCountNewLo = KLSTD::GetSysTickCountI();
        if(ulTickCountNewLo < KLSTD::g_ulTickCountLo)
        {//overflow !!!
            ++KLSTD::g_ulTickCountHi;
        };
        KLSTD::g_ulTickCountLo = ulTickCountNewLo;
        result = (AVP_qword(KLSTD::g_ulTickCountHi) << 32) | AVP_qword(KLSTD::g_ulTickCountLo);
#ifdef __unix
        if(-1L != c_lSC_CLK_TCK && 0L != c_lSC_CLK_TCK)
            result = (result*1000)/((AVP_qword)(unsigned long)c_lSC_CLK_TCK);
#endif
    };    
    return (KLSTD::kltick_t)result;
};

namespace
{
    const unsigned c_uMaxTime = INT_MAX - (365*24*3600);
};

KLCSC_DECL  void KLSTD_GetPreciseTime(time_t& secs, long& ms)
{
#ifdef USE_BOOST_THREADS
	boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);

	secs = (long)xt.sec;
	ms = xt.nsec / 1000000;
#else
	#ifdef _WIN32
        ULARGE_INTEGER ul;
        GetSystemTimeAsFileTime((FILETIME*)&ul);
        AVP_qword temp = (ul.QuadPart - 116444736000000000)/10000;
        AVP_qword llSecs    =   temp/1000;
        if(llSecs > c_uMaxTime)
        {
            llSecs = c_uMaxTime;
            ms = 0;
        }
        else
        {
            secs = (time_t)llSecs;
            ms = (size_t)(temp % 1000);
        };
     #endif
	#ifdef N_PLAT_NLM
        secs = time(0);
        ms = 0;//secs*1000; 
     #endif        

	#ifdef _unix
		timespec t;
		__impl::Thread::gettime( &t );

		secs = t.tv_sec;
		ms = t.tv_nsec / 1000000;
	#endif
#endif
	KLSTD_ASSERT(ms >=0 && ms <= 999);
};

namespace{

	#define _DAY_SEC           (24L * 60L * 60L)    /* secs in a day */

	#define _YEAR_SEC          (365L * _DAY_SEC)    /* secs in a year */

	#define _FOUR_YEAR_SEC     (1461L * _DAY_SEC)   /* secs in a 4 year interval */

	#define _DEC_SEC           315532800L           /* secs in 1970-1979 */

	#define _BASE_YEAR         70L                  /* 1970 is the base year */

	#define _BASE_DOW          4                    /* 01-01-70 was a Thursday */

	#define _LEAP_YEAR_ADJUST  17L                  /* Leap years 1900 - 1970 */

	#define _MAX_YEAR          138L                 /* 2038 is the max year */

	static int _days[] = {
			-1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364
	};

	/*
	 * ChkAdd evaluates to TRUE if dest = src1 + src2 has overflowed
	 */
	#define ChkAdd(dest, src1, src2)   ( ((src1 >= 0L) && (src2 >= 0L) \
		&& (dest < 0L)) || ((src1 < 0L) && (src2 < 0L) && (dest >= 0L)) )

	/*
	 * ChkMul evaluates to TRUE if dest = src1 * src2 has overflowed
	 */
	#define ChkMul(dest, src1, src2)   ( src1 ? (dest/src1 != src2) : 0 )
};

/*!
  \brief	Converts a struct tm value (in UTC) to a time_t value, then updates the struct tm value.

  \return	If successful, mktime returns the specified calender time encoded as a time_t value. Otherwise, (time_t)(-1) is returned to indicate an error.
  \param	tb - pointer to a tm time structure to convert and normalize
*/
KLCSC_DECL time_t KLSTD_mkgmtime(struct tm *tb)
{
		time_t tmptm1, tmptm2, tmptm3;
		struct tm *tbtemp;

		/*
		 * First, make sure tm_year is reasonably close to being in range.
		 */
		if ( ((tmptm1 = tb->tm_year) < _BASE_YEAR - 1) || (tmptm1 > _MAX_YEAR
		  + 1) )
			goto err_mktime;


		/*
		 * Adjust month value so it is in the range 0 - 11.  This is because
		 * we don't know how many days are in months 12, 13, 14, etc.
		 */

		if ( (tb->tm_mon < 0) || (tb->tm_mon > 11) ) {

			/*
			 * no danger of overflow because the range check above.
			 */
			tmptm1 += (tb->tm_mon / 12);

			if ( (tb->tm_mon %= 12) < 0 ) {
				tb->tm_mon += 12;
				tmptm1--;
			}

			/*
			 * Make sure year count is still in range.
			 */
			if ( (tmptm1 < _BASE_YEAR - 1) || (tmptm1 > _MAX_YEAR + 1) )
				goto err_mktime;
		}

		/***** HERE: tmptm1 holds number of elapsed years *****/

		/*
		 * Calculate days elapsed minus one, in the given year, to the given
		 * month. Check for leap year and adjust if necessary.
		 */
		tmptm2 = _days[tb->tm_mon];
		if ( !(tmptm1 & 3) && (tb->tm_mon > 1) )
				tmptm2++;

		/*
		 * Calculate elapsed days since base date (midnight, 1/1/70, UTC)
		 *
		 *
		 * 365 days for each elapsed year since 1970, plus one more day for
		 * each elapsed leap year. no danger of overflow because of the range
		 * check (above) on tmptm1.
		 */
		tmptm3 = (tmptm1 - _BASE_YEAR) * 365L + ((tmptm1 - 1L) >> 2)
		  - _LEAP_YEAR_ADJUST;

		/*
		 * elapsed days to current month (still no possible overflow)
		 */
		tmptm3 += tmptm2;

		/*
		 * elapsed days to current date. overflow is now possible.
		 */
		tmptm1 = tmptm3 + (tmptm2 = (long)(tb->tm_mday));
		if ( ChkAdd(tmptm1, tmptm3, tmptm2) )
			goto err_mktime;

		/***** HERE: tmptm1 holds number of elapsed days *****/

		/*
		 * Calculate elapsed hours since base date
		 */
		tmptm2 = tmptm1 * 24L;
		if ( ChkMul(tmptm2, tmptm1, 24L) )
			goto err_mktime;

		tmptm1 = tmptm2 + (tmptm3 = (long)tb->tm_hour);
		if ( ChkAdd(tmptm1, tmptm2, tmptm3) )
			goto err_mktime;

		/***** HERE: tmptm1 holds number of elapsed hours *****/

		/*
		 * Calculate elapsed minutes since base date
		 */

		tmptm2 = tmptm1 * 60L;
		if ( ChkMul(tmptm2, tmptm1, 60L) )
			goto err_mktime;

		tmptm1 = tmptm2 + (tmptm3 = (long)tb->tm_min);
		if ( ChkAdd(tmptm1, tmptm2, tmptm3) )
			goto err_mktime;

		/***** HERE: tmptm1 holds number of elapsed minutes *****/

		/*
		 * Calculate elapsed seconds since base date
		 */

		tmptm2 = tmptm1 * 60L;
		if ( ChkMul(tmptm2, tmptm1, 60L) )
			goto err_mktime;

		tmptm1 = tmptm2 + (tmptm3 = (long)tb->tm_sec);
		if ( ChkAdd(tmptm1, tmptm2, tmptm3) )
			goto err_mktime;

		/***** HERE: tmptm1 holds number of elapsed seconds *****/
        struct tm tmbuffer;
#if defined(N_PLAT_NLM) 		// Novell Netware        
		struct tm fooTm;
		if ( (tbtemp = KLSTD_gmtime((const unsigned long *)&tmptm1, &fooTm)) == NULL )
//		if ( (tbtemp = KLSTD_gmtime(&tmptm1, &fooTm)) == NULL )
			goto err_mktime;
#else
		if ( (tbtemp = KLSTD_gmtime(&tmptm1, &tmbuffer)) == NULL )
			goto err_mktime;
#endif

		/***** HERE: tmptm1 holds number of elapsed seconds, adjusted *****/
		/*****       for local time if requested                      *****/

		*tb = *tbtemp;
		return (time_t)tmptm1;

err_mktime:
		/*
		 * All errors come to here
		 */
		return (time_t)(-1);
};

/*
    Т.н. "библиотека" glibc не устает нас радовать своими 
    заморочками. В ч. про функцию gmtime написано буквально 
    следующее: "The gmtime() function need not be reentrant. A 
    function that is not required to be reentrant is not required 
    to be thread-safe. ". 
    Вобщем, для переносимости следует использовать функцию 
    KLSTD_gmtime. 
*/
KLCSC_DECL struct tm* KLSTD_gmtime(const time_t*   timer, 
                        struct tm*      buffer)
{
    KLSTD_ASSERT(buffer);
    if(!buffer || !timer || KLSTD::c_tmInvalidTime == (*timer) )
        return NULL;
    struct tm* pResult = NULL;
#if defined(_WIN32)
    struct tm * pTm = gmtime(timer);
    if(pTm)
    {
        pResult = buffer;
        *buffer = *pTm;
    };
#elif defined(__unix) || defined(N_PLAT_NLM)
    pResult = gmtime_r(timer, buffer);
#else
    #error("Not implemeted")
#endif
    return pResult;
};

namespace
{
    const AVP_qword c_qwInvalidFileTime = KLSTD_INVALID_TIME_VAUE;
    
#ifdef _WIN32
    class CInfo_PerformanceFrequency
    {
    public:
        CInfo_PerformanceFrequency()
            :   m_qwFreq(0)
        {
            if(!QueryPerformanceFrequency((LARGE_INTEGER*)(void*)&m_qwFreq))
                m_qwFreq = 0;
        };
        AVP_qword   m_qwFreq;
    }g_oInfo_PerformanceFrequency;
#endif    
};

using namespace KLDBG;

KLCSC_DECL bool KLDBG_StartMeasure(
                        const wchar_t*  szwModule, 
                        const wchar_t*  szwMessage, 
                        long            nTraceLevel, 
                        measure_times&  mt)
{
    bool bResult = false;
    int nCurrentTraceLevel = 0;
    if(KLSTD::IsTraceStarted(&nCurrentTraceLevel) && nCurrentTraceLevel >= nTraceLevel)
    {
        //mt.m_clkStart = KLSTD::GetSysTickCount();

#ifdef _WIN32

        AVP_qword qwPerfCount = 0;
        if( !g_oInfo_PerformanceFrequency.m_qwFreq || 
            !QueryPerformanceCounter((LARGE_INTEGER*)(void*)&qwPerfCount))
        {
            qwPerfCount = 0;
        }
        else
            mt.m_clkStart = (unsigned long)qwPerfCount;

        //KLSTD_TRACE1(4, L"qwPerfCount = 0x%I64x\n", qwPerfCount);

        FILETIME ftCreationTime;
        FILETIME ftExitTime;
        //LPFILETIME ftKernelTime;
        //LPFILETIME ftUserTime;

        if(!GetThreadTimes(
                    GetCurrentThread(), 
                    &ftCreationTime, 
                    &ftExitTime, 
                    (LPFILETIME)&mt.m_qwKernelTime, 
                    (LPFILETIME)&mt.m_qwUserTime))
        {
            mt.m_qwUserTime = mt.m_qwKernelTime = c_qwInvalidFileTime;
        };

#else
		mt.m_qwUserTime = mt.m_qwKernelTime = c_qwInvalidFileTime;
#endif

        KLSTD::Trace(nTraceLevel, szwModule, L"Starting %ls...\n", szwMessage);
        bResult = true;
    };
    return bResult;
};

KLCSC_DECL void KLDBG_EndMeasure(
                        const wchar_t*  szwModule, 
                        const wchar_t*  szwMessage, 
                        long            nTraceLevel, 
                        measure_times&  mt)
{
    int nCurrentTraceLevel = 0;
    if(KLSTD::IsTraceStarted(&nCurrentTraceLevel) && nCurrentTraceLevel >= nTraceLevel)
    {
		AVP_qword qwKernelTime, qwUserTime;
		bool bFailure = false;

#ifdef _WIN32
        FILETIME ftCreationTime;
        FILETIME ftExitTime;        
        //LPFILETIME ftKernelTime;
        //LPFILETIME ftUserTime;

        AVP_qword qwPerfCount = 0;
        if( !g_oInfo_PerformanceFrequency.m_qwFreq || 
            !QueryPerformanceCounter((LARGE_INTEGER*)(void*)&qwPerfCount))
        {            
            qwPerfCount = 0;
        };

        //KLSTD_TRACE1(4, L"qwPerfCount = 0x%I64x\n", qwPerfCount);

        if(!GetThreadTimes(
                    GetCurrentThread(), 
                    &ftCreationTime, 
                    &ftExitTime, 
                    (LPFILETIME)&qwKernelTime, 
                    (LPFILETIME)&qwUserTime))
        {
            bFailure = true;
        };
#endif
        double lfDelta = 
            g_oInfo_PerformanceFrequency.m_qwFreq
                ?   ((1000.0*double(((unsigned long)qwPerfCount)-mt.m_clkStart))/double(AVP_longlong(g_oInfo_PerformanceFrequency.m_qwFreq)))
                :   (double)KLSTD::DiffTickCount(KLSTD::GetSysTickCount(), mt.m_clkStart);
        //unsigned long lDelta = KLSTD::DiffTickCount(KLSTD::GetSysTickCount(), mt.m_clkStart);
        if( bFailure || 
            c_qwInvalidFileTime == mt.m_qwUserTime || 
            c_qwInvalidFileTime == mt.m_qwKernelTime )
        {
            KLSTD::Trace(nTraceLevel, szwModule, L"...%ls took %lf ms\n", szwMessage, lfDelta);
        }
        else
        {
            long lDelta = long(lfDelta+0.5);
            KLSTD::Trace(   nTraceLevel, 
                            szwModule, 
                            L"...%ls took %lf ms (User: %ld ms, Kernel: %ld ms, FullLoad: %ld %%)\n", 
                            szwMessage, 
                            lfDelta,
                            long((qwUserTime - mt.m_qwUserTime)/10000),
                            long((qwKernelTime - mt.m_qwKernelTime)/10000),
                            lDelta
                                ?   (100*long((qwUserTime - mt.m_qwUserTime + 
                                        qwKernelTime - mt.m_qwKernelTime)/10000)/lDelta)
                                :   0
            );
        };        
    };
};

KLCSC_DECL  time_t KLSTD_FASTCALL KLSTD_GetLatestDate(time_t x, time_t y)
{
    if(x == -1 && y != -1)
        return y;
    if(x != -1 && y == -1)
        return x;
    return std::max(x, y);//but using difftime is more correct
};
