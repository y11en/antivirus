// inshelpo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <shellapi.h>

#include "../hook/dofw_src.cpp"

void
SkipDelimeter (
	wchar_t** ppwchCmdValue
	)
{
	if (L':' == *ppwchCmdValue[0])
		(*ppwchCmdValue)++;
}

void
GetCmdValue (
	wchar_t* pwchCmdValue,
	bool* pbValue
	)
{
	SkipDelimeter( &pwchCmdValue );

	wchar_t wchtmp[0x1000];
	lstrcpy(wchtmp, pwchCmdValue);
	
	CharLower( wchtmp );

	if (!lstrcmp( wchtmp, L"on" ))
		*pbValue = true;
	else
		*pbValue = false;
}

void
GetCmdValue (
	wchar_t* pwchCmdValue,
	wchar_t** pwchValue
	)
{
	SkipDelimeter( &pwchCmdValue );

	*pwchValue = pwchCmdValue;
}

void
GetCmdValueState (
	wchar_t* pwchCmdValue,
	eThreeState* peState
	)
{
	SkipDelimeter( &pwchCmdValue );

	wchar_t wchtmp[0x1000];
	lstrcpy(wchtmp, pwchCmdValue);
	
	CharLower( wchtmp );

	if (!lstrcmp( wchtmp, L"on" ))
		*peState = _eTS_On;
	else if (!lstrcmp( wchtmp, L"off" ))
		*peState = _eTS_Off;
}

int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
    int ret = 0;

    int argc;
    wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	eThreeState eActivateStatus = _eTS_None;
	bool bInstaller = false;
	bool bActivateFileProtection = false;
	bool bActivateRegistryProtection = false;
	bool bFidBox = true;
	bool bActivateSecurityProtection = false;
	wchar_t* pwchFileProtectionLocation = NULL;

	for (int cou = 1; cou < argc; cou++)
	{
		wchar_t* pwchOption = NULL;

		switch(argv[cou][0])
		{
		case L'\\':
		case L'/':
			pwchOption = argv[cou];
			pwchOption++;
			switch (pwchOption[0])
			{
			case L'A':
			case L'a':
				GetCmdValueState( ++pwchOption, &eActivateStatus );
				break;

			case L'B':
			case L'b':
				GetCmdValue( ++pwchOption, &bFidBox );
				break;

			case L'I':
			case L'i':
				bInstaller = true;
				break;

			case L'L':
			case L'l':
				GetCmdValue( ++pwchOption, &pwchFileProtectionLocation );
				break;

			case L'R':
			case L'r':
				GetCmdValue( ++pwchOption, &bActivateRegistryProtection );
				break;

			case L'F':
			case L'f':
				GetCmdValue( ++pwchOption, &bActivateFileProtection );
				break;
			
			case L'S':
			case L's':
				GetCmdValue( ++pwchOption, &bActivateSecurityProtection );
				break;

			default:
				break;
			}
			break;

		default:
			break;
		}
	}

	if (bActivateFileProtection && !pwchFileProtectionLocation) 
		ret = 1; // activate without location
	else
    {
	    if (bInstaller)
		    GoProc(0, 0);

	    // activate file or registry protection
	    if (pwchFileProtectionLocation)
	    {
		    int len = lstrlen( pwchFileProtectionLocation );
		    if (L'\\' == pwchFileProtectionLocation[len - 1])
			    pwchFileProtectionLocation[len - 1] = 0;

	    }

	    Protect (
			eActivateStatus,
			bActivateFileProtection,
			pwchFileProtectionLocation,
			bActivateRegistryProtection,
			bActivateSecurityProtection
			);
    }

    if (!bFidBox)
	    DisableFBImp();

    LocalFree(argv);

	return ret;
}

