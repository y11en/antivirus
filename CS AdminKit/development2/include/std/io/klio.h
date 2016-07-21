/*KLCSAK_PUBLIC_HEADER*/
/*KLCSAK_BEGIN_PRIVATE*/
/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file klio.h
 * \authors Andrew Kazachkov, 
            Mikhail Karmazine, 
            Andrey Bryxin, 
            Damir Shiyafetdinov
 * \date 2002
 * \brief Работа с файлами
 *
 */
/*KLCSAK_END_PRIVATE*/
#ifndef KL_IO_H_68F05DB1_D23C_4d36_B9D6_69613A9352E5
#define KL_IO_H_68F05DB1_D23C_4d36_B9D6_69613A9352E5

#ifdef __unix__
#include <stdio.h>
#endif

#include "avp/avp_data.h"

#include "std/base/kldefs.h"
#include "std/base/klbase.h"
#include "std/thr/sync.h"

/*KLCSAK_BEGIN_PRIVATE*/
#include <string>
#include <vector>
/*KLCSAK_END_PRIVATE*/

namespace KLSTD{

#ifdef _WIN32
	const wchar_t c_szwPathDelimiters[]=L"\\/";
#else
 #ifdef N_PLAT_NLM
	const wchar_t c_szwPathDelimiters[]=L"\\/";
 #else
	const wchar_t c_szwPathDelimiters[]=L"/";
 #endif
#endif


	typedef enum{
		CF_OPEN_EXISTING=1,									//opens only if exists else fail
		CF_CREATE_NEW=2,									//creates only if not exists else fail
		CF_OPEN_ALWAYS=CF_OPEN_EXISTING|CF_CREATE_NEW,		//opens if exists and creates if not exists
		CF_CLEAR=4,
		CF_TRUNCATE_EXISTING=CF_OPEN_EXISTING|CF_CLEAR,		//truncate 
		CF_CREATE_ALWAYS=CF_OPEN_ALWAYS|CF_CLEAR			//creates if not exists and opens and clears if exists
	}CREATION_FLAGS;

	typedef enum{
		AF_READ=1,		// Read access
		AF_WRITE=2		// Write access
	}ACCESS_FLAGS;

	typedef enum{
		SF_READ=1,      // Sharing read
		SF_WRITE=2,     // Sharing write
	}SHARE_FLAGS;

	typedef enum{
		EF_RANDOM_ACCESS=1,             //Caching hint: file will be accessed randomly
		EF_SEQUENTIAL_SCAN=2,           //Caching hint: file will be accessed sequentially
		EF_TEMPORARY=4,                 //Caching hint: file is temporary and it will be deleted as soon as possible
        EF_DONT_WORRY_ABOUT_CLOSE=8,
        EF_DELETE_ON_CLOSE = 16,
        EF_WRITE_THROUGH = 32           //Caching hint: file will be written to disk directly
	}EXTRA_FLAGS;

	enum SEEK_TYPE{
		ST_SET,     //seek from beginning
		ST_CUR,     //seek from current position
		ST_END      //seek from the end of file
	};

    /*KLCSAK_BEGIN_PRIVATE*/

	class KLSTD_NOVTABLE FileMapping: public KLBase{
	public:
		virtual void*		GetPointer()=0;
		virtual AVP_qword	GetSize() const =0;
	};

	class KLSTD_NOVTABLE File: public KLBase{
	public:	
        /*!
          \brief	Reads the file. 

          \param	pBuffer     IN pointer to buffer to read
          \param	AVP_dword   IN number of bytes to read 
          \return	number of bytes read
        */
		virtual AVP_dword			Read(void* pBuffer, AVP_dword dwBuffer) =0;
        /*!
          \brief	Writes to the file. 

          \param	pBuffer     IN pointer to data to write
          \param	AVP_dword   IN size of data to write in bytes
          \return	number of bytes written
        */
		virtual AVP_dword			Write(const void* pBuffer, AVP_dword dwBuffer) =0;

        /*!
          \brief	Moves "file pointer" to the specified position

          \param	llOffset    IN offset
          \param	type        IN seek base
          \return	absolute position
        */
		virtual AVP_qword			Seek(AVP_longlong llOffset, SEEK_TYPE type) =0;

        /*!
          \brief	Rerurns file size

          \return	file size in bytes
        */
		virtual AVP_qword			GetSize() = 0;

        /*!
          \brief	Returns file name

          \return	file name
        */
		virtual const std::wstring	GetName()const = 0;

        /*!
          \brief	Maps file into memory

          \param	dwAccess    IN access type
          \param	qwOffset    IN mapping offset
          \param    dwSize      IN mapping size
        */
        virtual void                Map(AVP_dword       dwAccess,
										AVP_qword       qwOffset,
										AVP_dword       dwSize,
										FileMapping**   ppMapping) = 0;
        //! Not implemented. Do not use
		virtual void				Lock(AVP_qword qwOffset, AVP_qword qwBytes, long lTimeout=KLSTD_INFINITE) =0;
        //! Not implemented. Do not use
		virtual void				Unlock(AVP_qword qwOffset, AVP_qword qwBytes) =0;

        /*!
          \brief	Changes file size

          \param	qwNewSize   IN new size
        */
		virtual void				SetEOF(AVP_qword qwNewSize) =0;
        /*!
          \brief	Returns runtime file descriptor

          \return	runtime file descriptor
        */
		virtual int					GetFileID()=0;

        /*!
          \brief	Flushes file to the disk

          \param	bThrowError    IN if true throws exception in case of error occured
        */       
        virtual bool                Flush(bool bThrowError=false) = 0;		
	};
	
	class KLSTD_NOVTABLE StdioFile: public KLBase{
	public:	
		virtual bool				ReadString(wchar_t* szBuffer, const int nSize) = 0;
		virtual void				WriteString(const wchar_t* szBuffer) = 0;
		virtual void				Seek(AVP_long lOffset, SEEK_TYPE type) = 0;
		virtual long				GetSize() = 0;
		virtual const std::wstring	GetName()const  = 0;
		virtual int					GetFileID()=0;
	};

	KLCSC_DECL time_t GetFileCreateTime( const std::wstring &wstrFilePath );

    /*KLCSAK_END_PRIVATE*/
};

/*KLCSAK_BEGIN_PRIVATE*/

/*!
  \brief	Creates file object wrapper

  \param	wstrPath    IN file name
  \param	dwSharing   IN sharing mode: any of SHARE_FLAGS
  \param	dwCreation  IN creation flags: one of CREATION_FLAGS
  \param	dwAccess    IN access flags: one of ACCESS_FLAGS
  \param	dwFlags     IN extra flags (caching hinst): EXTRA_FLAGS
  \param	ppFile      OUT file object wrapper
  \param	lTimeout    IN timeout to wait if the file is blocked
*/
KLCSC_DECL  void KLSTD_CreateFile(
                const std::wstring& wstrPath,
                AVP_dword           dwSharing,
                AVP_dword           dwCreation,
                AVP_dword           dwAccess,
                AVP_dword           dwFlags,
                KLSTD::File**       ppFile,
                long                lTimeout=0);

KLCSC_DECL void KLSTD_CreateStdioFile(
				const std::wstring&	wstrPath,
				AVP_dword			dwSharing,
				AVP_dword			dwCreation,
				AVP_dword			dwAccess,
				KLSTD::StdioFile**	ppFile);


/*!
  \brief		Удаление файла. 
  \param		szwFileName - Имя файла, который необходимо удалить.
  \param		bThrowExcpt - Если false, функция не выбрасывает исключения.
  \return		Если файл успешно удален, возвращает true. Если файла не был найден, возвращает false.
			Если произошла другая ошибка, то, в зависимости от значения bThrowExcpt выбрасывает
			исключение или возвращает false.
*/
KLCSC_DECL bool KLSTD_Unlink(const wchar_t* szwFileName, bool bThrowExcpt=true);


/*!
  \brief	Удаление директории
  \param	szwDir - Имя директории, которую необходимо удалить.
  \param	bExcept - Если false, функция не выбрасывает исключения.
  \return	Если директория успешно удалена, возвращает true.
			Если директория не была найдена, возвращает false.
			Если произошла другая ошибка, то, в зависимости от значения bThrowExcpt выбрасывает
			исключение или возвращает false.
*/
KLCSC_DECL bool KLSTD_RemoveDirectory(const wchar_t* szwDir, bool bExcept);

/*!
  \brief	Создание директории. 
  \param	szwDir - Имя директории. Директория уровнем выше должна быть создана.
  \param	bExcept - Если false, функция не выбрасывает исключения.
  \return	Если директория успешно создана, возвращает true.
			Если произошла ошибка, то, в зависимости от значения bThrowExcpt 
            выбрасывает исключение или возвращает false.
*/
KLCSC_DECL bool KLSTD_CreateDirectory(const wchar_t* szwDir, bool bExcept);

/*!
  \brief	Создание директории включая все родительские директории.
  \param	szwDir - Имя директории. При необходимости создаёт всю ветвь иерархии.
*/
KLCSC_DECL void KLSTD_CreatePath(const wchar_t* szwFullPath);


/*!
  \brief	Deletes specified directory with all child files and directories

  \param	szwPath IN directory name to delete
  \return	false if directory doesn't exist, otherwise true
*/
KLCSC_DECL bool KLSTD_DeletePath(const wchar_t* szwPath);

KLCSC_DECL bool KLSTD_IfExists(const wchar_t* szwFileName);

KLCSC_DECL bool KLSTD_IsDirectory(const wchar_t* szwFileName);


/*!
  \brief	Renames (moves) an existing file or a directory, including its children

  \param	szwOldFileName  IN  existing name
  \param	szwNewFileName  IN  new name
*/
KLCSC_DECL void KLSTD_Rename(const wchar_t* szwOldFileName, const wchar_t* szwNewFileName);


/*!
  \brief	Копирует указанный файл

  \param	szwExistingFileName
  \param	szwNewFileName
  \param	bOverwrite
*/
KLCSC_DECL void KLSTD_CopyFile(
                               const wchar_t*   szwExistingFileName,
                               const wchar_t*   szwNewFileName,
                               bool             bOverwrite);

/*!
  \brief	Создаёт временный файл в директории для временных файлов, и возвравщает его полное имя.
	В случае ошибки выбрасывается исключение.

  \param	wstrTmpFileName [out] имя временного файла
*/
KLCSC_DECL void KLSTD_GetTempFile(std::wstring& wstrTmpFileName);

/*!
  \brief	Функция вызывает для переданного FILE* функцию fclose 
   и снимает с данного файла блокировку во внутренней системе
*/
KLCSC_DECL int KLSTD_FClose( FILE *stream );

KLCSC_DECL void KLSTD_SplitPath(
					const std::wstring&		wstrFullPath,
					std::wstring&			wstrDir,
					std::wstring&			wstrName,
					std::wstring&			wstrExt);

KLCSC_DECL void KLSTD_MakePath(					
					const std::wstring&		wstrDir,
					const std::wstring&		wstrName,
					const std::wstring&		wstrExt,
					std::wstring&			wstrFullPath);

KLCSC_DECL bool KLSTD_PathAppend(
					const std::wstring&		wstrPath,
					const std::wstring&		wstrMore,
                    std::wstring&           wstrResult,
                    bool bExcept );

KLCSC_DECL void KLSTD_GetFilesByMask(
                const std::wstring&         wstrMask,
                std::vector<std::wstring>&  vecNames);

KLCSC_DECL void KLSTD_GetSubdirsByMask(
                const std::wstring&         wstrMask,
                std::vector<std::wstring>&  vecNames);


namespace KLSTD
{

    inline std::wstring DoPathAppend(
					const std::wstring&		wstrPath,
					const std::wstring&		wstrMore)
    {
        std::wstring wstrResult;
        KLSTD_PathAppend(wstrPath, wstrMore, wstrResult, true);
        return wstrResult;
    };
};

/*KLCSAK_END_PRIVATE*/

#endif //KL_IO_H_68F05DB1_D23C_4d36_B9D6_69613A9352E5
