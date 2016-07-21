#include "stdafx.h"
#include "common.h"
#include "ShellHelper.h"

namespace ShellHelper
{
	HICON GetShellIcon(SYSTEM_IMAGES nImageIndex)
	{
		ASSERT(nImageIndex >= SI_FIRST);
		ASSERT(nImageIndex <= SI_LAST);

		HICON hIcon = NULL;
		int nIconsNum = ExtractIconEx(_T("SHELL32.DLL"), nImageIndex, NULL, &hIcon, 1);
		if (nIconsNum == 1)
			return hIcon;

		return NULL;
	}

} // namespace ShellHelper