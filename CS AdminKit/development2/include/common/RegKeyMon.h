/*!
 * (C) 2007 Kaspersky Lab
 *
 * \file	RegKeyMon.h
 * \author	Eugene Rogozhnikov
 * \date	09.11.2007 15:18:35
 * \brief	Класс-обертка метода RegNotifyChangeKeyValue для отслеживания изменений ключа в реестре
			Использование:

			CRegKeyMon rgMon( HKEY_LOCAL_MACHINE, L"bla-bla-bla" );
			rgMon.StartMonitoring();

			WaitForSingleObject(rgMon, 1000);
 *
 */

#ifndef __REGKEYMON_H__
#define __REGKEYMON_H__

namespace KLSTD
{
	class CRegKeyMon
	{
	public:
		CRegKeyMon( HKEY hKeyParent, LPCWSTR lpszKeyName );
		~CRegKeyMon();

		void StartMonitoring( BOOL bWatchSubtree, DWORD dwNotifyFilter );
		operator HANDLE() const; 

	private:
		KLSTD::KlRegKey m_key;
		HANDLE hMonEvent;
		HKEY m_hKeyParent;
		std::wstring m_wstrKeyName;
	};

	inline CRegKeyMon::CRegKeyMon( HKEY hKeyParent, LPCWSTR lpszKeyName ) :
		m_hKeyParent( hKeyParent ),
		m_wstrKeyName( lpszKeyName )
	{
		hMonEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		if( hMonEvent == NULL ) KLSTD_THROW_LASTERROR();
	}

	inline CRegKeyMon::~CRegKeyMon()
	{
		if( hMonEvent ) CloseHandle( hMonEvent );
	} 

	inline CRegKeyMon::operator HANDLE() const
	{
		return hMonEvent;
	}

	inline void CRegKeyMon::StartMonitoring( BOOL bWatchSubtree, DWORD dwNotifyFilter )
	{
		m_key.Close();
		KLSTD_RGCHK(m_key.Open( m_hKeyParent, m_wstrKeyName.c_str(), KEY_NOTIFY ));
		if(!ResetEvent( hMonEvent )) KLSTD_THROW_LASTERROR();
		KLSTD_RGCHK(RegNotifyChangeKeyValue( m_key, bWatchSubtree, dwNotifyFilter, hMonEvent, TRUE ));
	}
}; // namespace KLSTD


#endif //__REGKEYMON_H__