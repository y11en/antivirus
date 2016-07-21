#ifndef _KLUTIL_STDFILE_H__INCLUDED_
#define _KLUTIL_STDFILE_H__INCLUDED_

#include <stdio.h>
#include <errno.h>
#include <tchar.h>
#include <wtypes.h>
#include "../except.h"
#include <string>

namespace KLUTIL
{
	
	//////////////////////////////////////////////////////////////////////////
	// CStdFile
	//////////////////////////////////////////////////////////////////////////
	
	//! Файловое исключение.
	DEFINE_LIBID(EID_StdFileLib, 0xCE84, 0x4AC5, 0x8C24, 0xDD6E);
	typedef CLibExcept<EID_StdFileLib> CFileException;
	
#define THROW_FILEEX() THROW(CFileException, errno, (GetErrHead() + strerror(errno)).c_str() ) 
	
	//! Обертка вокруг стандартного файла.
	class CStdFile
	{
		FILE* m_f;
		std::string m_szFilename;
		std::string GetErrHead() const
		{
			return "Файл \"" + m_szFilename + "\": ";    
		}
	public:
		
		CStdFile(): m_f(NULL) {}
		
		//! Конструктор.
		//!   \throw CFileException в случае если не удалось открыть файл.
		CStdFile(
			LPCTSTR pszFname,	//!< [in] Путь к файлу.
			LPCTSTR pszAccess	//!< [in] Режим доступа (см. fopen).
			): m_f(NULL) { Open(pszFname, pszAccess); }
		
		~CStdFile() { if (m_f) fclose(m_f); }
		
		//! Открывет файл.
		//!   \return \b false в случае если не удалось открыть файл. Код ошибки \b errno.
		bool TryOpen(
			LPCTSTR pszFname,	//!< [in] Путь к файлу.
			LPCTSTR pszAccess	//!< [in] Режим доступа (см. fopen).
			) throw()
		{
			Close();
			m_szFilename = pszFname;
			m_f = _tfopen(pszFname, pszAccess);
			return m_f != NULL;
		}
		
		//! Открывет файл.
		//!   \throw CFileException в случае если не удалось открыть файл.
		void Open(
			LPCTSTR pszFname,	//!< [in] Путь к файлу.
			LPCTSTR pszAccess	//!< [in] Режим доступа (см. fopen).
			)
		{
			if (!TryOpen(pszFname, pszAccess))
				THROW_FILEEX();
		}
		
		//! Закрывает файл.
		void Close() throw()
		{
			if (m_f)
			{
				fclose(m_f);
				m_f = NULL;
			}
		}
		
		operator FILE* () throw() { return m_f; }
		
		//@{
		//! Считывает текстовую строку.
		//!   \return \b false, если достигнут конец файла.
		//!   \throw CFileException в случае ошибки.
		bool ReadLine(
			char* pBuf,		//!< [out] Буфер для строки.
			size_t nBufSize	//!< [in] Размер буфера \e pBuf в байтах.
			)
		{
			if (fgets(pBuf, static_cast<int>(nBufSize), m_f) != NULL)
				return true;
			if (feof(m_f))
				return false;
			THROW_FILEEX();
		}
		
		bool ReadLine(
			std::string& line	//!< [out] Cтрока.
			)
		{
			line.swap(std::string()); // clear() missing workaround
			char buf[1024];
			if (!ReadLine(buf, sizeof(buf)))
				return false;
			do
			{
				line += buf;
				if (*line.rbegin() == '\n')
				{
					line.resize( line.size()-1 );
					return true;
				}
			} while(ReadLine(buf, sizeof(buf)));
			return true;
		}
		//@}
		
		//@{
		//! Записывает текстовую строку.
		//!   \throw CFileException в случае ошибки.
		void WriteLine(
			const char* pText	//!< [in] Выводимая строка.
			)
		{
			if (fputs(pText, m_f) == EOF)
				THROW_FILEEX();
		}
		
		void WriteLine(
			const std::string& text	//!< [in] Выводимая строка.
			)
		{
			WriteLine(text.c_str());
		}
		//@}
		
		//! Считывает блок данных.
		//!   \return Количество полностью считанных элементов.
		//!   \throw CFileException в случае ошибки.
		size_t Read(
			void* pBuf,		//!< [out] Буфер для данных.
			size_t nItemSize,	//!< [in] Размер элемента в байтах.
			size_t nItemCount	//!< [in] Максимальное количество элементов для чтения.
			)
		{
			size_t res = fread(pBuf, nItemSize, nItemCount, m_f);
			if (nItemCount<=res || feof(m_f))
				return res;
			THROW_FILEEX();
		}
		
		//! Записывает блок данных.
		//!   \throw CFileException в случае ошибки.
		void Write(
			const void* pBuf,	//!< [in] Указатель на данные.
			size_t nItemSize,	//!< [in] Размер элемента в байтах.
			size_t nItemCount	//!< [in] Максимальное количество элементов для записи.
			)
		{
			if (fwrite(pBuf, nItemSize, nItemCount, m_f) != nItemCount)
				THROW_FILEEX();
		}
		
		//! Проверят достигнут ли конец файла.
		//!   \return \b true - конец файла был достигнут.
		bool Eof() throw() { return feof(m_f) != 0; }
		
		//! Считывает один символ.
		//!   \throw CFileException в случае ошибки.
		char Get()
		{
			int res = fgetc(m_f);
			if (res != EOF || feof(m_f))
				return res;
			THROW_FILEEX();
		}
		
		//! Записывает один символ.
		//!   \throw CFileException в случае ошибки.
		void Put(
			char c	//!< [in] Записываемый символ.
			)
		{
			if (fputc(c, m_f) == EOF)
				THROW_FILEEX();
		}
		
		//! Устанавливает текущую позицию.
		//!   \throw CFileException в случае ошибки.
		void Seek(
			long nOffest,		//!< [in] Количество байтов от точки отсчёта \e nOrigin.
			int nOrigin = SEEK_SET	//!< [in] Точка отсчёта.\n
			//!< \b SEEK_CUR От текущей позиции.\n
			//!< \b SEEK_END От конца файла.\n
			//!< \b SEEK_SET От начала файла.
			)
		{
			if (fseek(m_f, nOffest, nOrigin) != 0)
				THROW_FILEEX();
		}
		
		//! Возвращает текущую позицию.
		//!   \return Текущая позиция.
		//!   \throw CFileException в случае ошибки.
		long Tell()
		{
			long res = ftell(m_f);
			if (res != -1L)
				return res;
			THROW_FILEEX();
		}
		
		//! Устанавливает текущую позицию на начало.
		void Rewind() throw()
		{
			rewind(m_f);
		}
};

} // namespace KLUTIL

#endif // _KLUTIL_STDFILE_H__INCLUDED_