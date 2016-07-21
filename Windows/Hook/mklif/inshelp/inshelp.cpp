// inshelp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define _CRT_SECURE_NO_DEPRECATE

#include "../fssync/selfprot.cpp"

typedef enum _eThreeState
{
	_eTS_None = 0,
	_eTS_On = 1,
	_eTS_Off = 2
}eThreeState;

void
SkipDelimeter (
	wchar_t** ppwchCmdValue
	)
{
	if (L':' == *ppwchCmdValue[0])
		(*ppwchCmdValue)++;
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

int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
    int ret = 0;

    int argc;

	wchar_t* pwchParams = GetCommandLineW();
    wchar_t **argv = CommandLineToArgvW(pwchParams, &argc);

	eThreeState eActivateStatus = _eTS_None;
	bool bInstaller = false;
	bool bActivateFileProtection = false;
	bool bActivateRegistryProtection = false;
	bool bFidBox = true;

	wchar_t* pwchFileProtectionLocation = NULL;

	/*wchar_t dbgmsg[MAX_PATH];
	wsprintf( dbgmsg, L"parameters count %d\n", argc );
	OutputDebugString( dbgmsg );*/

	for (int cou = 1; cou < argc; cou++)
	{
		wchar_t* pwchOption = NULL;

		/*wsprintf( dbgmsg, L"parameter %d value %s\n", cou, argv[cou] );
		OutputDebugString( dbgmsg );*/

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
			
			default:
				break;
			}
			break;

		default:
			break;
		}
	}

	if (bActivateFileProtection && !pwchFileProtectionLocation) 
	{
		ret = 1; // activate without location

		//OutputDebugString( L"bActivateFileProtection && !pwchFileProtectionLocation\n" );
	}
	else
    {
	    PVOID pProContext = Protect (
		    bActivateFileProtection,
		    pwchFileProtectionLocation,
		    bActivateRegistryProtection,
		    bInstaller
		    );

	    if (pProContext)
        {
	        if (!bFidBox)
		        MKL_ReleaseDrvFile( pProContext );
        	
			switch (eActivateStatus)
			{
			case _eTS_On:
				MKL_ChangeClientActiveStatus( pProContext, TRUE );
				break;
			
			case _eTS_Off:
				MKL_ChangeClientActiveStatus( pProContext, FALSE );
				break;

			case _eTS_None:
			default:
				break;
			}

	        MKL_ClientUnregister( &pProContext );
        }
        else
		{
			//OutputDebugString( L"pProContext failed\n" );
			ret = 2;
		}
    }

    LocalFree(argv);

	return ret;
}
