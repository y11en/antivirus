////////////////////////////////////////////////////////////////////
//
//  SearRule.c
//  Search rule implementation
//  AVP generic purpose stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <string.h>
#include <tchar.h>
#include <datatreeutils/dtutils.h>
#include <avpnstid.h>
#include <AVPPort.h>

extern void* (* DTUAllocator)(AVP_size_t);
extern void  (* DTULiberator)(void*);

#define NWNS_ROOTNAME	L"<NWR>"

// Compare two AVP objects callback
// hFirstObject - HDATA handle of first object
// hSecondObject - HDATA handle of second object
// Return values -	avpo_Different				- objects are fully different
//									avpo_TheSame					- objects are equivalent
//									avpo_Descendant				- second object is descendant of first
//									avpo_FirstDescendant	- second object is the first descendant of first
// typedef AVP_word  (*CompareTwoObjects)( HDATA hFirstObject, HDATA hSecondObject );

// Search for object rule
// hRuleTreeRoot			- HDATA of rule tree root node
// hObjectData				- HDATA of object to be used for search
// pCompareObjectFunc - pointer to compare objects function 
// Return value  			- HDATA rule tree node were founded or 0 - in case of node not found
// ---
DTUT_PROC HDATA DTUT_PARAM DTUT_SearchForObjectRuleW( HDATA hRuleTreeRoot, HDATA hObjectData, CompareTwoObjects pCompareObjectFunc ) {
	if ( !hRuleTreeRoot || !hObjectData ) {
    _RPT0( _CRT_ASSERT, _T("DTUT_SearchForObjectRule Wrong parameters") );
		return 0;
	}
	else {
		HDATA hCurrData;
		HDATA hCandidateRule = 0;
		AVP_bool bStopSearch = 0;
		MAKE_ADDR1( nAddr, NS_PID_RULE_ROOT );

		if ( !pCompareObjectFunc )
			pCompareObjectFunc = DTUT_CompareTwoAvpObjectsW;

		hCurrData = DATA_Get_First( hRuleTreeRoot, 0 );
		while ( hCurrData && !bStopSearch ) {
			HDATA hRuleRoot = DATA_Find( hCurrData, nAddr );
			if ( hRuleRoot ) {
				AVP_dword nCMPResult = pCompareObjectFunc( hCurrData, hObjectData );
				switch ( nCMPResult ) {
					case avpo_Descendant :
					case avpo_FirstDescendant :
						hCandidateRule = hRuleRoot;
						break;
					case avpo_TheSame :
						hCandidateRule = hRuleRoot;
						bStopSearch = 1;
						break;
				}
			}
			hCurrData = DATA_Get_Next( hCurrData, 0 );
		}
		return hCandidateRule;
	}
}


// Compare two AVP objects by geven names callback
// pszFirstObject  - name of the first object
// pszSecondObject - name of the second object
// Return values -	avpo_Different				- objects are fully different
//									avpo_TheSame					- objects are equivalent
//									avpo_Descendant				- second object is descendant of first
//									avpo_FirstDescendant	- second object is the first descendant of first
//typedef AVP_word  (*CompareTwoObjectsStr)( const wchar_t *pszFirstObject, const wchar_t *pszSecondObject );

// Search for object rule
// hRuleTreeRoot			- HDATA of rule tree root node
// pszObjectName			- name of object to be used for search (empty string means "MyComputer" object)
// dwObjectType       - type of object - MyComputer, folder or file (see AVPNstID.h)
// pdwResult					- pointer to required and returned type of rule owner object (see SOR_XXX defines)
//                      If NULL is given the value SOR_THISANDUPWARDS will be used
// pCompareObjectFunc - pointer to compare objects function
//                      If NULL is given  DTUT_CompareTwoAvpObjectsStr function will be used
// Return value  			- HDATA rule tree node were founded or 0 - in case of node not found
DTUT_PROC HDATA DTUT_PARAM DTUT_SearchForObjectRuleExW( HDATA hRuleTreeRoot, 
																											  const wchar_t *pszObjectName, 
																											  DWORD  dwObjectType,
																											  DWORD *pdwResult, 
																											  CompareTwoObjectsStrW pCompareObjectFunc ) {
	if ( !hRuleTreeRoot || !pszObjectName ) {
    _RPT0( _CRT_ASSERT, _T("DTUT_SearchForObjectRuleEx Wrong parameters") );
		return 0;
	}
	else {
		if ( pdwResult && !*pdwResult ) {
			return 0;
		}
		else {
			HDATA    hCurrData;
			HDATA    hCandidateRule = 0;
			AVP_bool bStopSearch    = 0;
			DWORD    dwResult       = 0;
			DWORD    dwRequired     = pdwResult ? *pdwResult : SOR_THISANDUPWARDS;
			MAKE_ADDR1( nAddr, NS_PID_RULE_ROOT );

			if ( !pCompareObjectFunc )
				pCompareObjectFunc = DTUT_CompareTwoAvpObjectsStrW;

			hCurrData = DATA_Get_First( hRuleTreeRoot, 0 );
			while ( hCurrData && !bStopSearch ) {
				HDATA hRuleRoot = DATA_Find( hCurrData, nAddr );
				if ( hRuleRoot ) {
					AVP_dword nFirstObjectType = 0;
					wchar_t *pRuleObjectName = 0;
					AVP_dword nCMPResult;

					HPROP hFirstObjectTypeProp = DATA_Find_Prop( hCurrData, NULL, NS_PID_OBJECT_TYPE );
					if ( hFirstObjectTypeProp ) 
						PROP_Get_Val( hFirstObjectTypeProp, &nFirstObjectType, sizeof(nFirstObjectType) );

					if ( nFirstObjectType != NS_OBJECT_TYPE_MYCOMPUTER ) {
						if ( nFirstObjectType != NS_OBJECT_TYPE_NWROOT )
							pRuleObjectName  = DTUT_GetPropValueAsStringW( DATA_Find_Prop(hCurrData, NULL, 0), NULL );
						else
							pRuleObjectName  = wcscpy( (wchar_t *)DTUAllocator(sizeof(wchar_t) * (wcslen(NWNS_ROOTNAME) + 1)), NWNS_ROOTNAME );
					}
					else
						pRuleObjectName  = wcscpy( (wchar_t *)DTUAllocator(sizeof(wchar_t)), L"" );

					nCMPResult = pCompareObjectFunc( pRuleObjectName, pszObjectName );
					switch ( nCMPResult ) 
						case avpo_TheSame : {
							if ( dwRequired & SOR_JUSTTHIS ) {
								if ( dwObjectType == nFirstObjectType ) {
									hCandidateRule = hRuleRoot;
									dwResult = SOR_JUSTTHIS;
									bStopSearch = (dwRequired & SOR_JUSTTHIS) == SOR_JUSTTHIS;
								}
							}
							break;
						case avpo_FirstDescendant :
							if ( dwRequired & SOR_FIRSTANCESTOR ) {
								hCandidateRule = hRuleRoot;
								dwResult = SOR_FIRSTANCESTOR;
								bStopSearch = (dwRequired & SOR_ANYANCESTOR) == SOR_FIRSTANCESTOR;
							}
							break;
						case avpo_Descendant :
							if ( dwRequired & SOR_ANCESTOR ) {
								hCandidateRule = hRuleRoot;
								dwResult = SOR_ANCESTOR;
								bStopSearch = (dwRequired & SOR_ANYANCESTOR) == SOR_ANCESTOR;
							}
							break;
						case avpo_FirstAncestor :
							if ( dwRequired & SOR_FIRSTDESCENDANT ) {
								hCandidateRule = hRuleRoot;
								dwResult = SOR_FIRSTDESCENDANT;
								bStopSearch = (dwRequired & SOR_ANYDESCENDANT) == SOR_FIRSTDESCENDANT;
							}
							break;
						case avpo_Ancestor :
							if ( dwRequired & SOR_DESCENDANT ) {
								hCandidateRule = hRuleRoot;
								dwResult = SOR_DESCENDANT;
								bStopSearch = (dwRequired & SOR_ANYDESCENDANT) == SOR_DESCENDANT;
							}
							break;
					}
					DTULiberator( pRuleObjectName );
				}
				hCurrData = DATA_Get_Next( hCurrData, 0 );
			}
			if ( pdwResult )
				*pdwResult = dwResult;

			return hCandidateRule;
		}
	}
}
