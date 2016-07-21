/*!
 * (C) 2007 Kaspersky Lab
 * 
 * \file	inventoryutils.h
 * \author	Eugene Rogozhnikov
 * \date	23.08.2007 18:48:00
 * \brief	
 * 
 */

#ifndef __INVENTORYUTILS_H__
#define __INVENTORYUTILS_H__

#ifdef _WIN32

#include <string>
#include <vector>

#include <std/win32/klos_win32v50.h>
#include <common\cleanercommon.h> 

namespace KLINV {

	std::wstring GetCleanerIniPath();

	void GetCleanerIniFileInfo(
		const std::wstring& wstrIniFilePath,
		std::vector<KLCLEANER::CIniFileInfo>& vectIniInfo );

}; // namespace KLINV

#endif // _WIN32
#endif //__INVENTORYUTILS_H__