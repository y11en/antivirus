#if !defined(AFX_FORMATTIMESTRING_H__2D879857_65A2_47EE_BD9D_2008984BE64E__INCLUDED_)
#define AFX_FORMATTIMESTRING_H__2D879857_65A2_47EE_BD9D_2008984BE64E__INCLUDED_

#include "defs.h"
#include "../Stuff/StdString.h"
#include "SysTime.h"

namespace KLUTIL
{

KLUTIL_API int Get_n_DaysStringEx(const CSysTime& tEventData,
								  CStdString & szDays,
								  CStdString DateTimeString);

KLUTIL_API int GetHumanDaysBetween(const CSysTime& tEventPrev, const CSysTime& tEventNext);

KLUTIL_API int GetHumanDaysAgo(const CSysTime& tEventData);

KLUTIL_API CStdString GetHumanDaysAgoString(const CSysTime& tEventData,
											CStdString DateTimeString = _T(""));

KLUTIL_API CStdString GetHumanDaysIntervalString(const CSysTime& tEventData,
												 CStdString DateTimeString = _T(""));

} // namespace KLUTIL

#endif // !defined(AFX_FORMATTIMESTRING_H__2D879857_65A2_47EE_BD9D_2008984BE64E__INCLUDED_)

