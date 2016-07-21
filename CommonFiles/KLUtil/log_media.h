#ifndef __LOG_MEDIA_H
#define __LOG_MEDIA_H

#include "defs.h"
#include "log.h"
#include "inl/synchro.h"

namespace KLUTIL
{
	class KLUTIL_API CConsoleMedia: public ILogMedia
	{
	public:
		CConsoleMedia();
	protected:
		virtual void Write(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszDate, LPCTSTR pszTime, LPCTSTR pszThreadId, LPCTSTR pszThreadName, LPCTSTR pszModule, LPCTSTR pszMessage);
	private:
		CCritSec m_cs;
		HANDLE m_hConsole;
		CONSOLE_SCREEN_BUFFER_INFO m_info;
	};

	class KLUTIL_API CFileMedia: public ILogMedia
	{
	public:
		CFileMedia(LPCTSTR pszFilename, bool bAppend, bool bFlush);
		virtual ~CFileMedia();
		virtual bool IsWorking() const;
	protected:
		virtual void Write(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszDate, LPCTSTR pszTime, LPCTSTR pszThreadId, LPCTSTR pszThreadName, LPCTSTR pszModule, LPCTSTR pszMessage);
	private:
		bool m_bFlush;
		CFlHndl m_hFile;
		CHndl m_hMutex;
	};

	class KLUTIL_API CDebugMedia: public ILogMedia
	{
		DWORD m_dwParam;
	public:
		CDebugMedia(DWORD dwParam);
		virtual bool IsWorking() const;
	protected:
		virtual void Write(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszDate, LPCTSTR pszTime, LPCTSTR pszThreadId, LPCTSTR pszThreadName, LPCTSTR pszModule, LPCTSTR pszMessage);
	};

} // namespace KLUTIL

#endif