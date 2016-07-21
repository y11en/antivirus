/*!
*		(C) 2002 Kaspersky Lab 
*		
*		\file	HostInfo.h
*		\brief	Интерфейс сбора информации о компьютере(версия OS, процессор, память...)
*		
*		\author Андрей Брыксин
*		\date	09.01.2003 13:44:43
*/		

#if !defined(AFX_HOSTINFO_H__BF743ECA_F55F_4EBE_924B_880A12277769__INCLUDED_)
#define AFX_HOSTINFO_H__BF743ECA_F55F_4EBE_924B_880A12277769__INCLUDED_

#include <std/base/klbase.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace KLHINF
{
// Имена параметров в возвращаемом контейнере
const wchar_t c_hinf_os_name[] = L"OS Name";											// STRING_T
const wchar_t c_hinf_os_version[] = L"OS Version";										// STRING_T
const wchar_t c_hinf_os_manufacturer[] = L"OS Manufacturer";							// STRING_T
const wchar_t c_hinf_os_directory[] = L"OS Directory";									// STRING_T
const wchar_t c_hinf_system_name[] = L"System Name";									// STRING_T
const wchar_t c_hinf_system_type[] = L"System Type";									// STRING_T

const wchar_t c_hinf_system_processor[] = L"Processor family";							// STRING_T
const wchar_t c_hinf_system_processor_number[] = L"Processor number";					// INT_T

const wchar_t c_hinf_system_mem_physical[] = L"Physical memory";						// INT_T
const wchar_t c_hinf_system_mem_physical_available[] = L"Available physical memory";	// INT_T
const wchar_t c_hinf_system_mem_virtual[] = L"Virtual memory";							// INT_T
const wchar_t c_hinf_system_mem_virtual_available[] = L"Available virtual memory";		// INT_T
const wchar_t c_hinf_system_mem_page_file_space[] = L"Page file space";					// INT_T

const wchar_t c_hinf_system_mem_physical_ll[] = L"Physical memory ll";					// LONG_T
const wchar_t c_hinf_system_mem_virtual_ll[] = L"Virtual memory ll";					// LONG_T
const wchar_t c_hinf_system_mem_page_file_space_ll[] = L"Page file space ll";			// LONG_T

const wchar_t c_hinf_country_code[] = L"Country code";									// STRING_T
const wchar_t c_hinf_time_zone[] = L"Time zone";										// STRING_T

// Имя секции в SettingStorage
const wchar_t KLHINF_SECTION[] = L"KLHINF";


/*!
* \brief Интерфейс реализует функциональность по сбору информации о компьютере(версия OS, процессор, память...)
*/
class HostInfo : public KLSTD::KLBase
{
public:
	//! \fn				: GetInfo
	//! \brief			: Возвращает информацию о компьютере(версия OS, процессор, память...)
	//												о компьютере(версия OS, процессор, память...)
	virtual void GetInfo() = 0;
};

};


//! \fn				: KLHINF_CreateHostInfo
//! \brief			: Создает экземпляр объекта HostInfo
//! \param          : ppHostInfo [out]		указатель на экземпляр объекта HostInfo
void KLHINF_CreateHostInfo(KLHINF::HostInfo** ppHostInfo);

#endif // !defined(AFX_HOSTINFO_H__BF743ECA_F55F_4EBE_924B_880A12277769__INCLUDED_)

