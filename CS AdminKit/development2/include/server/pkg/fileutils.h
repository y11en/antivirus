/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PCK/Packages.h
 * \author Михаил Кармазин
 * \date 2002
 * \brief Интерфейсы и функции для работы с файлами и директориями (копирование и т.п.)
 *
 */

#ifndef _FILEUTILS_H__0852EE00_43FA_46E7_AC7F_82A22ECFCF7B_
#define _FILEUTILS_H__0852EE00_43FA_46E7_AC7F_82A22ECFCF7B_

#include <std/base/klbase.h>
#include <kca/prci/componentinstance.h>

namespace KLFU {
	/*! Перечисление с причинами вызова callback-функции при копировании фолдера:
		Progress - сообщение о пргрессе копирования.	
		CopyingFinished - копирование завершено.
		CopyFailed - копирование завершено неудачно
	 */
	enum ECopyCallbackReason { Progress, Counting, CopyFinished, CopyStopped, CopyFailed };


	/*!
		\brief Определение callback-функции, вызываемой при копировании фолдера:
		
		\param dwTotalPercent - процент скопрированных файлов
		\param eCopyCallbackReason - причина вызова callback'а (см.описание 
				для ECopyCallbackReason)
		\param pcszFileName - полный путь к обрабатавыемомоу в данный момоент файлу.
					Не NULL в случае Counting и Progress.
		\param pcszFailReason - строка с описанием ошибки, не-NULL только в случае
				eCopyCallbackReason == CopyFailed
	 */
	typedef void (*PCopyProgressRoutine)
		(
			AVP_dword dwTotalPercent,
			ECopyCallbackReason eCopyCallbackReason,
			const wchar_t* pcszFileName,
			const wchar_t* pcszFailReason,
			void* pParam
		);

	class KLSTD_NOVTABLE FolderAim : public KLSTD::KLBase {
    public:
	    /*!
			\brief Асинхронное копирование содержимого папки. Управление возвращается из 
					функции до завершения копирования. Само копирование происходит в
					отдельном потоке, при этом периодически из того же потока вызывается
					callback-функция с информацией о прогрессе выполнения. Сигналом конца
					копирования является вызов callback'а с параметром eCopyCallbackReason,
					равным либо CopyFinished, либо CopyStopped, либо CopyFailed.
					В случае CopyFailed передается также причина ошибки.

					Кроме того, в процессе копирование периодически проверяется значение
					bool-переменной bStopFlag (переданной по ссылке). Если bStopFlag
					становиться равным true, to процесс копирование прекращается, и вызывается
					callback-функция с eCopyCallbackReason == CopyStopped.

                    После возвращения управления из метода его можно немендленно вызывать
                    снова, не дожидаясь окончания копирование.

                    Деструктор данного объекта дожидается окончания всех запущенных с его 
                    помощью асинхронных процессов копирования (поэтому лучше убивать его,
                    когда копирование завершено, а иначе придется ждать).

			\param wstrFromPath - папка-источник
			\param wstrToPath - папка, в которую нужно копировать.
			\param bStopFlag - переданная по ссылке перменная, значение которой проверяеться
					в процессе копирования.
			\param pCopyProgressRoutine - callback-функция (см.выше описание).
					НЕ МОЖЕТ БЫТЬ NULL, так как ее финальный вызов с соответсвующим значением
					параметра eCopyCallbackReason сообщает о конце операции.
		*/
		virtual void CopyFolderAsynch(
				const std::wstring& wstrFromPath,
				const std::wstring& wstrToPath,
				int& bStopFlag,
				PCopyProgressRoutine pCopyProgressRoutine,
				void* pParam ) = 0;


		/*!
			\brief то же самое, что и пред. функция, за исключением того, что сообщения о
					прогрессе и конце операции передаються через метод Publish компонента,
					перданного через параметр pComponentInstance.
			
			\param wstrFromPath - папка-источник
			\param wstrToPath - папка, в которую нужно копировать.
			\param bStopFlag - переданная по ссылке перменная, значение которой проверяеться
					в процессе копирования.
			\param pComponentInstance - компонент, у которого вызывается метод Publish для
					сообшений о прогрессе и конце операции. Не может быть NULL, по тем же
					причинам, что и callback для пред.функции.
	    */
		virtual void CopyFolderAsynch(
				const std::wstring& wstrFromPath,
				const std::wstring& wstrToPath,
				int& bStopFlag,
				KLPRCI::ComponentInstance* pComponentInstance,
				void* pParam ) = 0;
        
		/*!
			\brief то же самое, что и CopyFolderAsynch, но работает синхронно 
                и  выбрасывет исключение в случае ошибки.
        */
		virtual void CopyFolder(
				const std::wstring& wstrFromPath,
				const std::wstring& wstrToPath,
				int& bStopFlag,
				PCopyProgressRoutine pCopyProgressRoutine,
				void* pParam ) = 0;

		/*!
			\brief то же самое, что и CopyFolderAsynch, но работает синхронно 
                и  выбрасывет исключение в случае ошибки.
        */
		virtual void CopyFolder(
				const std::wstring& wstrFromPath,
				const std::wstring& wstrToPath,
				int& bStopFlag,
				KLPRCI::ComponentInstance* pComponentInstance,
				void* pParam ) = 0;
        
		/*!
			\brief Возвращает размер папки. В случае ошибки выбрасывает исключение. */
        virtual AVP_longlong GetFolderSize( const std::wstring& wstrFolderPath ) = 0;

        virtual void DeleteFolderWithContent( const std::wstring& wstrFolder ) = 0;
		
        virtual void ClearReadOnlyAttributeForFilesInFolder( const std::wstring& wstrFolder ) = 0;
	};
}

KLCSSRV_DECL void KLFU_CreateFolderAim( KLFU::FolderAim** ppCopyFolder );

#endif _FILEUTILS_H__0852EE00_43FA_46E7_AC7F_82A22ECFCF7B_
