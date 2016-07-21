/*!
 * (C) 2007 Kaspersky Lab
 * 
 * \file	patches.h
 * \author	Eugene Rogozhnikov
 * \date	08.05.2007 18:13:37
 * \brief	
 * 
 */

#ifndef __PATCHES_H__
#define __PATCHES_H__

namespace KLPTCH
{
	///////////////////////////////////////////////////////////////////////////////////////////

	#define KLNAG_TASK_NAGENT_PATCH  L"KLNAG_TASK_NAGENT_PATCH"

	////////////////////////////////////////////////////////////////////////////////////////////
	// Константы, используемые в файле rslt_lists.xml. Также используются при передаче
	// в SOAP-методах
	
	const wchar_t szUpdaterRecentRetranslatedFiles[] = L"RecentRetranlatedFiles";

	const wchar_t szUpdaterType[] = L"Type";
	const wchar_t szUpdaterApplication[] = L"Application";
	const wchar_t szUpdaterFileName[] = L"FileName";
	const wchar_t szUpdaterRelativeSrvPath[] = L"RelativeSrvPath";
	const wchar_t szUpdaterToAppVersion[] = L"ToAppVersion";
	const wchar_t szUpdaterFromAppVersion[] = L"FromAppVersion";
	const wchar_t szUpdaterComment[] = L"Comment";
	const wchar_t szUpdaterDate[] = L"Date";

	const wchar_t szUpdaterTypePatchValue[] = L"Patch";

	// Используются для передачи параметров в soap-методах
	const wchar_t szCallResult[] = L"CallResult";
	const wchar_t szAppVersion[] = L"ApplicationVersion";

	//////////////////////////////////////////////////////////////////////////////////////

	struct CPatchInfo
	{
		std::wstring wstrApplication;
		std::wstring wstrFileName;
		std::wstring wstrRelativeSrvPath;
		std::wstring wstrToAppVersion;
		std::wstring wstrFromAppVersion; // Набор версий, с которых происходит обновление, разделенный ';'
		std::wstring wstrComment;
		time_t tDateTime;
	};
	inline bool operator==( const CPatchInfo& p1, const CPatchInfo& p2 )
	{
		return ( p1.wstrFileName == p2.wstrFileName );
	}
}; // namespace KLPTCH

#endif //__PATCHES_H__