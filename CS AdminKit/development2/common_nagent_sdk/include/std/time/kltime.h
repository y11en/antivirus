
#ifndef KL_TIME_H_6A0778F4_B132_45c7_AFEB_AC689542E38D
#define KL_TIME_H_6A0778F4_B132_45c7_AFEB_AC689542E38D


#include "std/base/klbase.h"
#include <time.h>

namespace KLSTD
{
#ifdef __unix__
	#define KLSTD_INVALID_TIME_VAUE		0xFFFFFFFF
#else
	#define KLSTD_INVALID_TIME_VAUE		0xFFFFFFFFFFFFFFFF	
#endif

    //! system time
    typedef AVP_qword   klstd_time_t;
    
    //! millisecond time
    typedef AVP_qword   klstd_mstime_t;
    
    //! invalid system time constant
    const klstd_time_t  c_tmInvalidSysTime = KLSTD_INVALID_TIME_VAUE;
    
    //! invalid millisecond time constant
    const klstd_mstime_t c_tmInvalidMsecTime = KLSTD_INVALID_TIME_VAUE;

    //! invalid time_t (depends on compiler version)
    const time_t c_invalid_time_t = -1;
};

/*!
  \brief	Returns precise UTC system time (with milliseconds)
  \param	secs [out]
  \param	ms [out]
*/
KLCSC_DECL  KLSTD::klstd_time_t KLSTD_GetSystemTime(KLSTD::klstd_time_t* secs = NULL, unsigned* ms = NULL);

/*!
  \brief	Returns millisecond UTC time
*/
KLCSC_DECL  KLSTD::klstd_mstime_t KLSTD_GetMsTime();

/*!
  \brief	Converts time into klstd_mstime_t
  \param	t   IN
  \param	ms  IN
  \return	klstd_mstime_t
*/
KLCSC_DECL  KLSTD::klstd_mstime_t KLSTD_Time2MsTime(
                    KLSTD::klstd_time_t     t, 
                    unsigned                ms = 0);

/*!
  \brief	Converts klstd_mstime_t into time
  \param	t   IN  milliseconds time
  \param	sec OUT  system time milliseconds 
  \param	ms  OUT system time milliseconds 
  \return	same as sec
*/
KLCSC_DECL  KLSTD::klstd_time_t KLSTD_MsTime2Time(
                            KLSTD::klstd_mstime_t  t, 
                            KLSTD::klstd_time_t*   sec = NULL, 
                            unsigned*       ms = NULL);



/*!
  \brief	Selects latest between two times
*/
KLCSC_DECL  time_t KLSTD_FASTCALL KLSTD_GetLatestDate(time_t x, time_t y);

namespace KLSTD
{
    struct system_time_t
    {
        system_time_t()
            :   m_tmTime(c_tmInvalidMsecTime)
        {;};
        
        explicit system_time_t(klstd_time_t s, unsigned ms = 0)
            :   m_tmTime(KLSTD_Time2MsTime(s , ms))
        {;};
        
        void Set(klstd_time_t s, unsigned ms = 0)
        {
            m_tmTime = KLSTD_Time2MsTime(s , ms);
        };
        
        klstd_time_t    GetSeconds() const
        {
            return KLSTD_MsTime2Time(m_tmTime);
        };
        
        unsigned    GetMilliseconds() const
        {
            unsigned msec = 0;
            KLSTD_MsTime2Time(m_tmTime, NULL, &msec);
            return msec;
        };

        void SetMsTime(klstd_mstime_t t)
        {
            m_tmTime = t;
        };

        klstd_mstime_t GetMsTime() const
        {
            return m_tmTime;
        };

        static system_time_t Now()
        {
            system_time_t tmNow;
            tmNow.SetMsTime(KLSTD_GetMsTime());
            return tmNow;
        };
    protected:
        klstd_mstime_t m_tmTime;
    };

	/*!
		\brief Возращает количество милисекунд прощедщее с момента запуска ОС
	*/
	KLCSC_DECL unsigned long GetSysTickCount();
    
	/*!
		\brief Calculates difference between two tick counts 
	*/   
    inline unsigned long KLSTD_FASTCALL DiffTickCount(unsigned long x1, unsigned long x2)
    {
        return (unsigned long)abs( (long)(x1 - x2) );
    };    

    typedef AVP_longlong kltick_t;
} // namespace KLSTD


/*!
  \brief	Returns number of milliseconds as 64-bit int since some 
            moment in the past.
  \return	number of milliseconds 
*/
KLCSC_DECL KLSTD::kltick_t KLSTD_GetSysTickCount();


/****************************************************************************
*
*                               Deprecated symbols
*
*****************************************************************************/

namespace KLSTD
{


#if defined(_MSC_VER) && _MSC_VER >= 1300
    //! !!! deprecated !!!
    typedef long        klstd_old_time_t;
#else
    //! !!! deprecated !!!
    typedef time_t      klstd_old_time_t;
    
#endif
};

/*!
  \brief	Переводит структуру tm, содержащую время UTC в time_t
            !!! deprecated !!!
  \param	IN tb время в формате структуры tm
  \return	время в формате time_t 
*/
KLCSC_DECL time_t KLSTD_mkgmtime(struct tm *tb);



/*!
  \brief	A thread-safe replacement for gmtime which is not thread-safe 
            (now is 10.10. 2006 !!!) in some implementations. 

  \param	timer   IN see description of gmtime
  \param	result  IN buffer to use
  \return	returns non-NULL pointer to buffer if success and NULL otherwise 
*/
KLCSC_DECL struct tm* KLSTD_gmtime(
                        const time_t*   timer, 
                        struct tm*      buffer); 


/*!
  \brief	Returns precise UTC time (with milliseconds)
            !!! deprecated !!!
  \param	secs [out]
  \param	ms [out]
*/
KLCSC_DECL  void KLSTD_GetPreciseTime(KLSTD::klstd_old_time_t& secs, long& ms);


namespace KLSTD
{


    //! !!! deprecated !!!
    const klstd_old_time_t c_tmInvalidTime = -1;

    

    // !!! deprecated !!!
    struct old_precise_time_t
    {
        old_precise_time_t(klstd_old_time_t s, long ms = 0)
            :   sec(s)
            ,   msec(ms)
        {;};
        
        old_precise_time_t()
            :   sec(c_tmInvalidTime)
            ,   msec(0)
        {;};
        
        static old_precise_time_t Now()
        {
            old_precise_time_t tmNow;
            KLSTD_GetPreciseTime(tmNow.sec, tmNow.msec);
            return tmNow;
        };

        operator klstd_old_time_t() const
        {
            return sec;
        };

        klstd_old_time_t    sec;
        long                msec;
    };


    // KLCSC_DECL unsigned long GetCurrentMilliseconds();
    //!\brief Возращает текущее количество милисекунд
    //  !!! deprecated !!!
    // this is inline here to avoid link problems with kltrace.dll,
    // from one side, and to have only one place for time-related
    // platform-dependent functions from other side.
    KLCSC_DECL unsigned long GetCurrentMilliseconds();
};


#endif //KL_TIME_H_6A0778F4_B132_45c7_AFEB_AC689542E38D
// Local Variables:
// mode: C++
// End:

