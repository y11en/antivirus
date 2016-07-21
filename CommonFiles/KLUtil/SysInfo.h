#if !defined(AFX_SYSINFO_H__184E8A52_B366_47ED_A112_74BA33A86A1E__INCLUDED_)
#define AFX_SYSINFO_H__184E8A52_B366_47ED_A112_74BA33A86A1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "defs.h"

namespace KLUTIL
{

#define SYS_INFO_SEP _T("--------------------\n");

KLUTIL_API std::string GetSysInfo ();

} // namespace KLUTIL

#endif // !defined(AFX_SYSINFO_H__184E8A52_B366_47ED_A112_74BA33A86A1E__INCLUDED_)
