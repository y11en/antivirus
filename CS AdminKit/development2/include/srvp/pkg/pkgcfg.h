/*!
 * (C) 2006 "Kaspersky Lab"
 *
 * \file srvp/pkg/pkgcfg.h
 * \author Андрей Лащенков
 * \date 14:57 27.04.2006
 * \brief Интерфейс для чтения/записи файлов настроек инсталляционных пакетов.
 *
 */

#ifndef _KLPKG_PKGCFG_H__
#define _KLPKG_PKGCFG_H__

namespace KLPKG {

    class KLSTD_NOVTABLE PackageConfigurator : public KLSTD::KLBaseQI {
    public:
			/*!
		  \brief Получить бинарный образ спец. файла настроек пакета.
       
			\param wszFileRelativePath [in]	-	относительный путь к файлу; NULL для файла, указанного в .kpd
			\param ppData [out] результат	-	бинарный образ файла настроек.
		*/
		virtual void ReadPkgCfgFile(
			wchar_t* wszFileRelativePath,
			KLSTD::MemoryChunk** ppChunk) = 0;

		/*!
		  \brief Заменить бинарный образ спец. файла настроек пакета.
       
			\param wszFileRelativePath [in]	-	относительный путь к файлу; NULL для файла, указанного в .kpd
			\param pData [in] результат		-	бинарный образ файла настроек.
		*/
		virtual void WritePkgCfgFile(
			wchar_t* wszFileRelativePath,
			KLSTD::MemoryChunk* pChunk) = 0;

	};

	class KLSTD_NOVTABLE PackageConfigurator2 : public PackageConfigurator {
    public:
		/*!
		  \brief получить информацию о лицензионном ключе в пакете 
			\param wstrKeyFileName [out] - имя файла ключа
			\param ppMemoryChunk [out] - тело ключа
		*/
		virtual void GetLicenseKey(
			std::wstring& wstrKeyFileName,
			KLSTD::MemoryChunk** ppMemoryChunk) = 0;

		/*!
		  \brief добавить лицензионный ключ в пакет
			\param wstrKeyFileName [in] - имя файла ключа
			\param ppMemoryChunk [in] - тело ключа
		*/
		virtual void SetLicenseKey(
			const std::wstring& wstrKeyFileName,
			KLSTD::MemoryChunk* pMemoryChunk,
			bool bRemoveExisting) = 0;

	};
}

#endif // _KLPKG_PKGCFG_H__
