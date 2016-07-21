// Filename.h: interface for the CFilename class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILENAME_H__EA312AC4_EEE1_4BB8_B4E0_65810D1D8F21__INCLUDED_)
#define AFX_FILENAME_H__EA312AC4_EEE1_4BB8_B4E0_65810D1D8F21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "defs.h"
#include "../Stuff/StdString.h"

namespace KLUTIL
{
	struct KLUTIL_API CFilename
	{
		CStdStringW drive;
		CStdStringW dir;
		CStdStringW fname;
		CStdStringW ext;

		CFilename();
		CFilename(const CStdStringW& path);
		void SplitPath(const CStdStringW& path);
		CStdStringW MakePath();
	};

} // namespace KLUTIL


#endif // !defined(AFX_FILENAME_H__EA312AC4_EEE1_4BB8_B4E0_65810D1D8F21__INCLUDED_)
