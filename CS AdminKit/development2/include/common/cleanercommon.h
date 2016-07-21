/*!
 * (C) 2006 Kaspersky Lab
 * 
 * \file	cleanercommon.h
 * \author	Eugene Rogozhnikov
 * \date	06.06.2006 16:27:34
 * \brief	Общие константы, используемые подсистемой cleaner.	
 * 
 */

#ifndef __CLEANERCOMMON_H__
#define __CLEANERCOMMON_H__	  

#ifdef _WIN32

#define KLCLEANER_DLL_FILE_NAME				L"cleanapi.dll"

// Относительный путь к ini-файлам для библиотеки cleaner
#define KLCLEANER_INI_PATH					L"Cleaner"

// Имя секции main в ini-файле для cleaner
#define KLCLEANER_INI_MAIN_SECTION			L"main"

// Имя ключа в ini-файле для cleaner, где хранится имя продукта
#define KLCLEANER_INI_PRODUCT_NAME			L"name"

// Имя ключа в ini-файле для cleaner, где хранится тип ini-файла
#define KLCLEANER_INI_TYPE			L"type"

// Имя ключа в ini-файле для cleaner, где хранится ключ в реестре для обнаружения установки продукта
#define KLCLEANER_DETECT_REGISTRY			L"detect-registry"

// Имя ключа в ini-файле для cleaner, где хранится где хранится ProductCode продукта, ставящегося с помощью msi
#define KLCLEANER_DETECT_MSI			L"detect-msi"

namespace KLCLEANER {

	// Типы ini-файлов
	const wchar_t INI_TYPE_UNINSTALL[] = L"uninstall";
	const wchar_t INI_TYPE_DETECTONLY[] = L"detect-only";

	struct CIniFileInfo
	{
		std::wstring wstrProductName;
		std::wstring wstrType;
		std::wstring wstrFileName;
		std::wstring wstrDetectRegistry;
		std::wstring wstrDetectMsi;
	};

}; // namespace KLCLEAN

#endif

#endif //__CLEANERCOMMON_H__