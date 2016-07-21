#ifndef __DLLCHECK_H__
#define __DLLCHECK_H__

////////////////////////////////////////////////////////////////////
//
//  DLLCHECK.H
//  Check DLL version utility function definitions
//  AVP	general application software
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////


// Check version of given DLLs
// pDescr					- pointer to two-dimension string array 
//									Each its line must have format "<Dll name>","<version>"
//									Exmpl : { {"Comctrl32.dll", "4.72.3110.9"}, {"Mfc42.dll","6.00.8447.0"}	}
// nDescrCount		- count of lines "pDescr" has.
// nCmpOperation  -	type of compare operation
//                  -1  - presented version must be less or equal than given
//                   0  - presented version must be equal to given
//                   1  - presented version must be more or equal than given
//
// Returned result -   -1 in case of success
//                     zero based index of "pDescr" line didn't fulfil condition
int CheckDllVersion( TCHAR *pDescr[][2], int nDescrCount, int nCmpOperation );

// Check version of standard DLL used by AVP with MessageBox showing
// Checked DLLs
/*
#ifdef _DEBUG
	{ "Mfc42d.dll", "6.00.8267.0" },
	{ "Msvcrtd.dll", "6.00.8267.0" },
	{ "Comctl32.dll", "4.70.0000.0" },
	{ "Mfco42d.dll", "6.00.8267.0" },
	{ "Atl.dll", "3.00.8449.0" },
	{ "Olepro32.dll",	"5.00.4275.0" },
	{ "Oleaut32.dll",	"2.40.4275.0" },
#else
	{ "Mfc42.dll", "6.00.8267.0" },
	{ "Msvcrt.dll", "6.00.8337.0" },
	{ "Comctl32.dll", "4.70.0000.0" },
	{ "Atl.dll", "3.00.8449.0" },
	{ "Olepro32.dll",	"5.00.4275.0" }
	{ "Oleaut32.dll",	"2.40.4275.0" }
#endif
*/
// Returned result - 1 - on success
//                   0 - in case of at least one DLL didn't fulfil condition
//                       (and show MessageBox)
BOOL CheckStandardAvpDLLWithMsg( BOOL bMessage = TRUE );

#endif