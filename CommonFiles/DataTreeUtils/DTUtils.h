////////////////////////////////////////////////////////////////////
//
//  DTUtils.h
//  Data tree manipulation function definition
//  AVP generic purpose stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#if !defined (__DTUTILS_H__)
#define __DTUTILS_H__

#include <avp_data.h>
#include <property/property.h>

#if defined (__unix__)
	#define  DTUT_PARAM 
#else
	#include <windows.h>
	#define		DTUT_PARAM  __cdecl
	#define		DTUT_REALISE_UNICODE
	#include <tchar.h>
#endif


#ifdef __cplusplus
#    define DTUT_PROC extern "C" 
#else
#    define DTUT_PROC
#endif


// Init library
// pfMemAlloc - pointer to memory-alloc function
// pfMemFree  - pointer to memory-free function
DTUT_PROC AVP_bool   DTUT_PARAM  DTUT_Init_Library( void* (*pfMemAlloc)(AVP_size_t), void (*pfMemFree)(void*) );

// Init library
// pfMemAlloc   - pointer to memory-alloc function
// pfMemFree    - pointer to memory-free function
// pfMemLock	  - pointer to inter-thread synchronization lock function
// pfMemUnlock	- pointer to inter-thread synchronization unlock function
// pContext     - pointer to inter-thread synchronization functions context
DTUT_PROC AVP_bool  DTUT_PARAM DTUT_Init_LibraryEx( void* (*pfMemAlloc)(AVP_size_t), 
																								    void (*pfMemFree)(void*),
																										void (*pfMemLock)(void *), 
																										void (*pfMemUnlock)(void *), 
																										void* pContext );
// Deinit library
DTUT_PROC AVP_bool   DTUT_PARAM  DTUT_Deinit_Library( AVP_bool bDeinitDATALib );

// ---
typedef enum  {	
	avpo_Different				= 0,
	avpo_TheSame					= avpo_Different	     + 1,
	avpo_Descendant				= avpo_TheSame		     + 1,
	avpo_FirstDescendant	= avpo_Descendant      + 1,
// Follow values are used by DTUT_CompareTwoAvpObjectsStr function
	avpo_Ancestor				  = avpo_FirstDescendant + 1,
	avpo_FirstAncestor	  = avpo_Ancestor        + 1,
}	CMPObjectsResult;

// Compare two AVP objects 
// hFirstObject  - HDATA handle of the first object
// hSecondObject - HDATA handle of the second object
// Return values -	avpo_Different				- objects are fully different
//									avpo_TheSame					- objects are equivalent
//									avpo_Descendant				- second object is descendant of first
//									avpo_FirstDescendant	- second object is the first descendant of first
DTUT_PROC AVP_word   DTUT_PARAM  DTUT_CompareTwoAvpObjects( HDATA hFirstObject, HDATA hSecondObject );
#if defined(DTUT_REALISE_UNICODE)
DTUT_PROC AVP_word   DTUT_PARAM  DTUT_CompareTwoAvpObjectsW( HDATA hFirstObject, HDATA hSecondObject );
#endif

// Compare two AVP objects by geven names 
// pszFirstObject  - name of the first object	 (empty string means "MyComputer" object)
// pszSecondObject - name of the second object  (empty string means "MyComputer" object)
// Return values   -	avpo_Different				- objects are fully different
//										avpo_TheSame					- objects are equivalent
//										avpo_Descendant				- second object is descendant of first
//										avpo_FirstDescendant	- second object is the first descendant of first
DTUT_PROC AVP_word   DTUT_PARAM  DTUT_CompareTwoAvpObjectsStr( const TCHAR *pszFirstObject, const TCHAR *pszSecondObject );
#if defined(DTUT_REALISE_UNICODE)
DTUT_PROC AVP_word   DTUT_PARAM  DTUT_CompareTwoAvpObjectsStrW( const wchar_t *pszFirstObject, const wchar_t *pszSecondObject );
#endif

// Merge data tree and pattern
// hTreeData    - HDATA of tree to be merged
// hPatternData - HDATA of pattern tree to use for werging
// phResultData - pointer to result HDATA
// Return	value - 1 on success, 0 on error occurs
DTUT_PROC AVP_bool   DTUT_PARAM  DTUT_MergeDataTreeAndPattern( HDATA hTreeData, HDATA hPatternData, HDATA* phResultData );

// Extract only datas needed for working from data tree
// hTreeData    - HDATA of source tree
// phResultData - pointer to result HDATA
// pPropInclude	- pointer to property IDs array to be included to result tree
// Return	value - 1 on success, 0 on error occurs
DTUT_PROC AVP_bool  DTUT_PARAM  DTUT_ExtractValuesDataTree( HDATA hTreeData, HDATA *phResultData, AVP_dword * pPropInclude );

// Exchange prop values
// hFromProp		- HPROP of value to be get from
// hToProp			- HPROP of value to be set to
// Return	value - 1 on success, 0 on error occurs
DTUT_PROC AVP_bool  DTUT_PARAM  DTUT_ExchangePropValue( HPROP hFromProp, HPROP hToProp );

// Exchange data props values
// hFromData		- HDATA to be get properties from
// hToData			- HDATA to be set properties to
// Return	value - 1 on success, 0 on error occurs
DTUT_PROC AVP_bool DTUT_PARAM DTUT_ExchangeDataPropValues( HDATA hFromData, HDATA hToData );

#define CDT_STRICT_CMP 0x80000000

// Compare two data trees
// hFirstData		- the first HDATA to compare
// hSecondData	- the second HDATA to compare
// dwFlags			- trees recursion flags (see Property.h)
//                Can be OR'ed with CDT_STRICT_CMP which means the strict comparison should be used.
//                In this case data tree nodes should have fully equal PIDs - i.e. IDs, AppIDs, 
//                types and array flags. Otherwise node IDs will be excluded from comparison.
// Return	value - 1 on trees are equal, 0 on any other cases (including errors)
DTUT_PROC AVP_bool DTUT_PARAM DTUT_CompareDataTrees( HDATA hFirstData, HDATA hSecondData, AVP_dword dwFlags );

// Prototype of string localisation call-back function for following functions
typedef void (*DTUT_LocalizeString)( AVP_dword dwStringID, TCHAR **pszSourceString );

// Convert Rules data tree to strings data tree
// hRulesData   - HDATA of rules tree to be converted
// hPatternData - HDATA of pattern tree to use for convert
// phResultData - pointer to result HDATA
// pfLocalize   - pointer to string localize function
// bShowUnselected - whether to show unselected rules
// Return	value - 1 on success, 0 on error occurs
DTUT_PROC AVP_bool DTUT_PARAM DTUT_ConvertRulesTreeToStringsTree( HDATA hRulesData, HDATA hPatternData, HDATA* phResultData, DTUT_LocalizeString pfLocalize, AVP_bool bShowUnselected );

// Convert known object Rules data tree to strings data tree
// hRulesData   - HDATA of rules tree to be converted
// hPatternData - HDATA of pattern tree to use for convert
// phResultData - pointer to result HDATA
// pfLocalize   - pointer to string localize function
// bShowUnselected - whether to show unselected rules
// Return	value - 1 on success, 0 on error occurs
DTUT_PROC AVP_bool DTUT_PARAM DTUT_ConvertKnownObjectRulesTreeToStringsTree( HDATA hRulesData, HDATA hPatternData, HDATA* phResultData, DTUT_LocalizeString pfLocalize, AVP_bool bShowUnselected );

// Convert Rules data tree to strings data tree
// hRulesData   - HDATA of rules tree to be converted
// hPatternData - HDATA of pattern tree to use for convert
// dwAppID			- identifier of application
// phResultData - pointer to result HDATA
// pfLocalize   - pointer to string localize function
// bShowUnselected - whether to show unselected rules
// Return	value - 1 on success, 0 on error occurs
DTUT_PROC AVP_bool DTUT_PARAM DTUT_ConvertRulesTreeToStandardReportHeaderStringsTree( HDATA hRulesData, HDATA hPatternData, AVP_dword dwAppID, HDATA* phResultData, DTUT_LocalizeString pfLocalize, AVP_bool bShowUnselected );

// Convert Rules data tree to INI strings data tree
// hRulesData   - HDATA of rules tree to be converted
// hPatternData - HDATA of pattern tree to use for convert
// phResultData - pointer to result HDATA
// Return	value - 1 on success, 0 on error occurs
DTUT_PROC AVP_bool DTUT_PARAM DTUT_ConvertRulesTreeToINIStringsTree( HDATA hRulesData, HDATA hPatternData, HDATA* phResultData, DTUT_LocalizeString pfLocalize );


// Find unique PID among first level children of hParentData
// hParentData  - HDATA of parent node to be scanned
// nSourcePID		- source PID for extracting APID, Type and Array
// Return value - Non 0 - found unique PID, 0 - in case of PID hasn't been found
DTUT_PROC AVP_dword DTUT_PARAM DTUT_FindUniquePID( HDATA hParentData, AVP_dword nSourcePID );


// Find tree node by unique id and get its value
// hStartData	 - HDATA of root node to be scanned
// nUniqueID   - value of unique id node looked for
// nDesiredPID - value of desired PID node looked for (0 - for trusted looking for)
// pValue      - pointer to buffer for value to be get
// nSize       - size of buffer
// Return value - HDATA of node was found, or 0 - in case of node was not found
DTUT_PROC HDATA DTUT_PARAM DTUT_GetDataByUniqueID( HDATA hStartData, AVP_dword nUniqueID, AVP_dword nDesiredPID, void *pValue, AVP_dword nSize );

// Find tree node by unique id and set its value
// hStartData	 - HDATA of root node to be scanned
// nUniqueID   - value of unique id node looked for
// nDesiredPID - value of desired PID node looked for (0 - for trusted looking for)
// nValue      - buffer with value to be set
// Return value - HDATA of node was found, or 0 - in case of node was not found
DTUT_PROC HDATA DTUT_PARAM DTUT_SetDataByUniqueID( HDATA hStartData, AVP_dword nUniqueID, AVP_dword nDesiredPID, AVP_dword nValue );

// Compare two AVP objects callback
// hFirstObject - HDATA handle of first object
// hSecondObject - HDATA handle of second object
// Return values -	avpo_Different				- objects are fully different
//									avpo_TheSame					- objects are equivalent
//									avpo_Descendant				- second object is descendant of first
//									avpo_FirstDescendant	- second object is the first descendant of first
typedef AVP_word  (*CompareTwoObjects)( HDATA hFirstObject, HDATA hSecondObject );

// Search for object rule
// hRuleTreeRoot			- HDATA of rule tree root node
// hObjectData				- HDATA of object to be used for search
// pCompareObjectFunc - pointer to compare objects function
//                      If NULL is given  DTUT_CompareTwoAvpObjects function will be used
// Return value  			- HDATA rule tree node were founded or 0 - in case of node not found
DTUT_PROC HDATA DTUT_PARAM DTUT_SearchForObjectRule( HDATA hRuleTreeRoot, HDATA hObjectData, CompareTwoObjects pCompareObjectFunc );
#if defined(DTUT_REALISE_UNICODE)
DTUT_PROC HDATA DTUT_PARAM DTUT_SearchForObjectRuleW( HDATA hRuleTreeRoot, HDATA hObjectData, CompareTwoObjects pCompareObjectFunc );
#endif

#define SOR_JUSTTHIS					0x0001
#define SOR_ANCESTOR					0x0002
#define SOR_FIRSTANCESTOR			0x0004
#define SOR_DESCENDANT				0x0008
#define SOR_FIRSTDESCENDANT		0x0010

#define SOR_ANYANCESTOR   (SOR_ANCESTOR | SOR_FIRSTANCESTOR)
#define SOR_ANYDESCENDANT (SOR_DESCENDANT | SOR_FIRSTDESCENDANT)

#define SOR_THISANDUPWARDS   (SOR_JUSTTHIS | SOR_ANYANCESTOR)
#define SOR_THISANDDOWNWARDS (SOR_JUSTTHIS | SOR_ANYDESCENDANT)

#define SOR_ANYRULE       (SOR_JUSTTHIS | SOR_ANYANCESTOR | SOR_ANYDESCENDANT)


// Compare two AVP objects by geven names callback
// pszFirstObject  - name of the first object
// pszSecondObject - name of the second object
// Return values -	avpo_Different				- objects are fully different
//									avpo_TheSame					- objects are equivalent
//									avpo_Descendant				- second object is descendant of first
//									avpo_FirstDescendant	- second object is the first descendant of first
typedef AVP_word  (*CompareTwoObjectsStr)( const TCHAR *pszFirstObject, const TCHAR *pszSecondObject );
#if defined(DTUT_REALISE_UNICODE)
typedef AVP_word  (*CompareTwoObjectsStrW)( const wchar_t *pszFirstObject, const wchar_t *pszSecondObject );
#endif

// Search for object rule
// hRuleTreeRoot			- HDATA of rule tree root node
// pszObjectName			- name of object to be used for search (empty string means "MyComputer" object)
// dwObjectType       - type of object - MyComputer, folder or file (see AVPNstID.h)
// pdwResult					- pointer to required and returned type of rule owner object (see SOR_XXX defines)
//                      If NULL is given the value SOR_THISANDUPWARDS will be used
// pCompareObjectFunc - pointer to compare objects function
//                      If NULL is given  DTUT_CompareTwoAvpObjectsStr function will be used
// Return value  			- HDATA rule tree node were founded or 0 - in case of node not found
DTUT_PROC HDATA DTUT_PARAM DTUT_SearchForObjectRuleEx( HDATA hRuleTreeRoot, 
																											 const TCHAR *pszObjectName, 
																											 DWORD  dwObjectType,
																											 DWORD *pdwResult, 
																											 CompareTwoObjectsStr pCompareObjectFunc );
#if defined(DTUT_REALISE_UNICODE)
DTUT_PROC HDATA DTUT_PARAM DTUT_SearchForObjectRuleExW( HDATA hRuleTreeRoot, 
																											  const wchar_t *pszObjectName, 
																											  DWORD  dwObjectType,
																											  DWORD *pdwResult, 
																											  CompareTwoObjectsStrW pCompareObjectFunc );
#endif

DTUT_PROC int				 DTUT_PARAM  DTUT_GetDataDatasCount( HDATA hData );
DTUT_PROC int				 DTUT_PARAM  DTUT_GetDataPropsCount( HDATA hData );

DTUT_PROC TCHAR			*DTUT_PARAM  DTUT_GetDataPropertyString( HDATA hData, AVP_dword nPID );

DTUT_PROC void			 DTUT_PARAM  DTUT_SetPropValue( HPROP hProp, const TCHAR *pStr, const TCHAR *pFormat );
DTUT_PROC void			 DTUT_PARAM  DTUT_SetPropArrayValue( HPROP hProp, int nPos, const TCHAR *pStr, const TCHAR *pFormat );
DTUT_PROC void			 DTUT_PARAM  DTUT_SetPropNumericValue( HPROP hProp, AVP_dword nValueValue );
DTUT_PROC void			 DTUT_PARAM  DTUT_SetPropArrayNumericValue( HPROP hProp, int nPos, AVP_dword nValueValue );

DTUT_PROC void			 DTUT_PARAM  DTUT_SetPropValueCC( HPROP hProp, const TCHAR *pStr, const TCHAR *pFormat );
DTUT_PROC void			 DTUT_PARAM  DTUT_SetPropArrayValueCC( HPROP hProp, int nPos, const TCHAR *pStr, const TCHAR *pFormat );

DTUT_PROC void			 DTUT_PARAM  DTUT_SetPropDateValue( HPROP hProp, const SYSTEMTIME *pSysTime );
DTUT_PROC void			 DTUT_PARAM  DTUT_SetPropDateArrayValue( HPROP hProp, int nPos, const SYSTEMTIME *pSysTime );

DTUT_PROC AVP_dword	 DTUT_PARAM  DTUT_GetPropNumericValueAsDWord( HPROP hProp );
DTUT_PROC AVP_dword	 DTUT_PARAM  DTUT_GetPropArrayNumericValueAsDWord( HPROP hProp, int nPos );

DTUT_PROC TCHAR			*DTUT_PARAM  DTUT_GetPropValueAsString( HPROP hProp, const TCHAR *pFormat );
DTUT_PROC TCHAR			*DTUT_PARAM  DTUT_GetPropArrayValueAsString( HPROP hProp, int nPos, const TCHAR *pFormat );

DTUT_PROC TCHAR			*DTUT_PARAM  DTUT_GetPropValueAsStringCC( HPROP hProp, const TCHAR *pFormat );
DTUT_PROC TCHAR			*DTUT_PARAM  DTUT_GetPropArrayValueAsStringCC( HPROP hProp, int nPos, const TCHAR *pFormat );

DTUT_PROC TCHAR			*DTUT_PARAM  DTUT_GetDataValueAsComboString( HDATA hData );
DTUT_PROC TCHAR			*DTUT_PARAM  DTUT_GetDataNumericValueAsComboString( HDATA hData );

DTUT_PROC AVP_bool	 DTUT_PARAM  DTUT_SetDataNumericValueAsComboString( HDATA hData, const TCHAR *pValueText );
DTUT_PROC AVP_bool	 DTUT_PARAM  DTUT_SetDataArrayNumericValueAsComboString( HDATA hData, int nPos, const TCHAR *pValueText );
DTUT_PROC TCHAR			*DTUT_PARAM  DTUT_GetDataArrayNumericValueAsComboString( HDATA hData, int nPos );
	
DTUT_PROC int				 DTUT_PARAM  DTUT_CompareValueInRangeI( HDATA hData, const TCHAR *pValueText, 
																													 AVP_dword nMinID, AVP_dword nMaxID, const TCHAR *pFormat );
DTUT_PROC int				 DTUT_PARAM  DTUT_CompareValueInRangeH( HDATA hData, const TCHAR *pValueText, 
																													 HPROP hMinProp, HPROP hMaxProp, const TCHAR *pFormat );
DTUT_PROC int				 DTUT_PARAM  DTUT_CompareDateValueInRangeI( HDATA hData, const SYSTEMTIME *pValueTime, 
																															 AVP_dword nMinID, AVP_dword nMaxID );
DTUT_PROC int				 DTUT_PARAM  DTUT_CompareDateValueInRangeH( HDATA hData, const SYSTEMTIME *pValueTime, 
																															 HPROP hMinProp, HPROP hMaxProp );

DTUT_PROC HDATA			 DTUT_PARAM  DTUT_CreateNodesSequence( HDATA hStartData, AVP_dword *pAddrSequence );


#if defined(DTUT_REALISE_UNICODE)
DTUT_PROC wchar_t		*DTUT_PARAM  DTUT_GetDataPropertyStringW( HDATA hData, AVP_dword nPID );

DTUT_PROC void			 DTUT_PARAM  DTUT_SetPropValueW( HPROP hProp, const wchar_t *pStr, const wchar_t *pFormat );
DTUT_PROC void			 DTUT_PARAM  DTUT_SetPropArrayValueW( HPROP hProp, int nPos, const wchar_t *pStr, const wchar_t *pFormat );

DTUT_PROC void			 DTUT_PARAM  DTUT_SetPropValueCCW( HPROP hProp, const wchar_t *pStr, const wchar_t *pFormat );
DTUT_PROC void			 DTUT_PARAM  DTUT_SetPropArrayValueCCW( HPROP hProp, int nPos, const wchar_t *pStr, const wchar_t *pFormat );

DTUT_PROC wchar_t		*DTUT_PARAM  DTUT_GetPropValueAsStringW( HPROP hProp, const wchar_t *pFormat );
DTUT_PROC wchar_t		*DTUT_PARAM  DTUT_GetPropArrayValueAsStringW( HPROP hProp, int nPos, const wchar_t *pFormat );
								 
DTUT_PROC wchar_t		*DTUT_PARAM  DTUT_GetPropValueAsStringCCW( HPROP hProp, const wchar_t *pFormat );
DTUT_PROC wchar_t		*DTUT_PARAM  DTUT_GetPropArrayValueAsStringCCW( HPROP hProp, int nPos, const wchar_t *pFormat );

DTUT_PROC wchar_t		*DTUT_PARAM  DTUT_GetDataValueAsComboStringW( HDATA hData );
DTUT_PROC wchar_t		*DTUT_PARAM  DTUT_GetDataNumericValueAsComboStringW( HDATA hData );

DTUT_PROC AVP_bool	 DTUT_PARAM  DTUT_SetDataNumericValueAsComboStringW( HDATA hData, const wchar_t *pValueText );
DTUT_PROC AVP_bool	 DTUT_PARAM  DTUT_SetDataArrayNumericValueAsComboStringW( HDATA hData, int nPos, const wchar_t *pValueText );
DTUT_PROC wchar_t		*DTUT_PARAM  DTUT_GetDataArrayNumericValueAsComboStringW( HDATA hData, int nPos );

DTUT_PROC int				 DTUT_PARAM  DTUT_CompareValueInRangeIW( HDATA hData, const wchar_t *pValueText, 
																													   AVP_dword nMinID, AVP_dword nMaxID, const wchar_t *pFormat );
DTUT_PROC int				 DTUT_PARAM  DTUT_CompareValueInRangeHW( HDATA hData, const wchar_t *pValueText, 
																													   HPROP hMinProp, HPROP hMaxProp, const wchar_t *pFormat );
#endif

#endif
