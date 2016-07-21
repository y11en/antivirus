#if defined (_MSC_VER)
#pragma warning (disable : 4786)
#endif

#if defined (_WIN32)

#if !defined (STRICT)
	#define STRICT
#endif

#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0500
#endif

#endif // defined (_WIN32)

//-----------------------------------------------------------------------------
// Includes

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../../../../include/platform_compat.h"
#include "../../../../include/secure_data.h"

#if defined (_WIN32)
#include <io.h>
#else

#if defined (__unix__)
#include <unistd.h>
#include <glob.h>
#include <limits.h>
#endif

#endif

//-----------------------------------------------------------------------------

#if defined (_MSC_VER)
#pragma warning (push, 4)
#endif

//-----------------------------------------------------------------------------

#if defined (UNICODE)
	#define out std::wcout
	#define outstream std::wostream
#else
	#define out std::cout
	#define outstream std::ostream
#endif

#if !defined (_WIN32)
	#define _tmain main
#endif

void ShowHelp ();
bool ReadFile (const _TCHAR* FileName, std::string* pFileImage);
bool DumpLicStorage (std::string& LicStorageImage);

int _tmain (int argc, _TCHAR **argv)
{

	if (argc < 2)
	{
		ShowHelp ();
		return (-1);
	}

	std::string	LicStorageImage;

	if (!ReadFile (argv[1], &LicStorageImage))
	{
		_tprintf (_T ("\nFailed to load file %s\n"), argv[1]);
		return (-1);
	}

	if (!DumpLicStorage (LicStorageImage))
	{
		return (-1);
	}

	return (0);

}

//-----------------------------------------------------------------------------

void ShowHelp ()
{
	_tprintf (_T ("Usage: DumpLicStorage.exe <license storage file name>"));
}

//-----------------------------------------------------------------------------

bool ReadFile (const _TCHAR* FileName, std::string* pFileImage)
{

	if (!FileName || !pFileImage)
	{
		return (false);
	}

	pFileImage->erase ();

	int file = _topen (FileName, O_RDONLY | O_BINARY);
	
	if (-1 == file) 
	{
		return (false);
	}
	
	long f_size = _lseek (file, 0, SEEK_END);
	
	
	if (-1 == f_size) 
	{
		_close (file);
		return (false);
	}
	
	char* s =  new char[(static_cast<unsigned int>(f_size))];
	
	if (!s) 
	{
		_close (file);
		return (false);
	}
	
	int size = static_cast<unsigned int> (f_size);
	
	_lseek (file, 0, SEEK_SET);
	
	int r = static_cast<int> (_read (file, s, size));
	
	_close (file);

	if (r)
	{
		pFileImage->assign (s, size);
	}

	delete [] s;
	
	return (r != 0);
	
}

//-----------------------------------------------------------------------------

void DumpKeyInfo (LicensingPolicy::SecureData::installed_key_info_t& ki);

bool DumpLicStorage (std::string& LicStorageImage)
{

	LicensingPolicy::SecureData::CSecureData	SD;
	HRESULT										hr;

	hr = SD.deserialize (LicStorageImage.c_str (), LicStorageImage.size ());
	if (FAILED (hr))
	{
		_tprintf (_T ("\nFailed to deserialize license storage. Error code is: %x\n"), hr);
		return (false);
	}

	LicensingPolicy::SecureData::installed_key_info_t	ActiveKeyInfo;
	LicensingPolicy::SecureData::installed_key_info_t	ReserveKeyInfo;
	LicensingPolicy::SecureData::installed_key_info_t	TrialKeyInfo;
	LicensingPolicy::date_t								LastKnownDate;

	hr = SD.getLastKnownDate (&LastKnownDate);

	if (FAILED (hr))
	{
		_tprintf (_T ("Failed to get last known date. Error code is: %x"), hr);
		return (false);
	}

	_tprintf (_T ("\nLast known date: %d.%d.%d\n"), LastKnownDate.m_day, 
		LastKnownDate.m_month, LastKnownDate.m_year);

	hr = SD.getActiveKeyInfo (&ActiveKeyInfo);
	if (FAILED (hr))
	{
		_tprintf (_T ("Failed to get active key info. Error code is: %x"), hr);
		return (false);
	}

	if (S_FALSE != hr)
	{
		_tprintf (_T ("Active key info:\n"));
		DumpKeyInfo (ActiveKeyInfo);
		_tprintf (_T ("\n"));
	}
	else
	{
		_tprintf (_T ("No active key is installed\n\n"));
	}

	hr = SD.getReserveKeyInfo (&ReserveKeyInfo);

	if (FAILED (hr))
	{
		_tprintf (_T ("Failed to get reserve key info. Error code is: %x"), hr);
		return (false);
	}

	if (S_FALSE != hr)
	{
		_tprintf (_T ("Reserve key info:\n"));
		DumpKeyInfo (ReserveKeyInfo);
		_tprintf (_T ("\n"));
	}
	else
	{
		_tprintf (_T ("No reserve key is installed\n\n"));
	}

	hr = SD.getTrialKeyInfo (&TrialKeyInfo);

	if (FAILED (hr))
	{
		_tprintf (_T ("Failed to get trial key info. Error code is: %x"), hr);
		return (false);
	}

	if (S_FALSE != hr)
	{
		_tprintf (_T ("Trial key info:\n"));
		DumpKeyInfo (TrialKeyInfo);
		_tprintf (_T ("\n"));
	}
	else
	{
		_tprintf (_T ("No trial key is installed\n"));
	}

	LicensingPolicy::date_t	TrialLimitDate;

	hr = SD.getTrialLimitDate (&TrialLimitDate);

	if (FAILED (hr))
	{
		_tprintf (_T ("Failed to get trial limit date. Error code is: %x\n"), hr);
		return (false);
	}

	if (! (!TrialLimitDate.m_day && !TrialLimitDate.m_month && !TrialLimitDate.m_year))
	{
		_tprintf (_T ("Trial limit date: %d.%d.%d\n"), TrialLimitDate.m_day, 
														TrialLimitDate.m_month,
														TrialLimitDate.m_year);
	}
	else
	{
		_tprintf (_T ("Trial limit date is unknown because no one trial key has been installed yet\n"));
	}

	LicensingPolicy::SecureData::installed_keys_list_t	KeysList;

	hr = SD.getKeysList (&KeysList);

	if (FAILED (hr))
	{
		_tprintf (_T ("\nFailed to get installed keys list. Error code is: %x"), hr);
		return (false);
	}

	if (KeysList.empty ())
	{
		_tprintf (_T ("\nNo key has been installed\n"));
		return (true);
	}

	_tprintf (_T ("\nTotal amount of keys have ever been installed is %d\n\n"), KeysList.size ());

	LicensingPolicy::SecureData::installed_keys_list_t::iterator	it (KeysList.begin ());
	LicensingPolicy::SecureData::installed_keys_list_t::iterator	end (KeysList.end ());

	for (; it != end; ++it)
	{
		if (it->second.serialNumber == ActiveKeyInfo.serialNumber)
		{
			_tprintf (_T ("\n(current active key)\n"));
		}
		else
		{
			if (it->second.serialNumber == ReserveKeyInfo.serialNumber)
			{
				_tprintf (_T ("\n(current reserve key)\n"));
			}
			else
			{
				if (it->second.serialNumber == TrialKeyInfo.serialNumber)
				{
					_tprintf (_T ("\n(current trial key)\n"));
				}
				else
				{
					_tprintf (_T ("\n\n"));
				}
			}
		}
		DumpKeyInfo (it->second);
	}

	
	return (true);
}

//-----------------------------------------------------------------------------

void DumpKeyInfo (LicensingPolicy::SecureData::installed_key_info_t& ki)
{
	_tprintf (_T ("\tKey file name\t\t:\t%s\n\tKey installation date\t:\t%d.%d.%d\n")
			  _T ("\tKey expiration date\t:\t%d.%d.%d\n\tKey serial number\t:\t%X\n")
			  _T ("\tUsed to be active key\t:\t%s\n\n"),
		ki.keyFileName.c_str (),
		ki.installDate.m_day, ki.installDate.m_month, ki.installDate.m_year,
		ki.expirationDate.m_day, ki.expirationDate.m_month, ki.expirationDate.m_year,
		ki.serialNumber.number.parts.keySerNum,
		(ki.usedToBeActiveKey ? _T ("true") : _T ("false"))
		);
}

//-----------------------------------------------------------------------------
