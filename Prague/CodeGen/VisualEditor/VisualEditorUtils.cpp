// VisualEditorUtils.cpp : implementation file
//

#include "stdafx.h"
#include <avpcontrols/wascmess.h>
#include <serialize/kldtser.h>
#include <datatreeutils/dtutils.h>
#include <_avpio.h>
#include <stuff/cpointer.h>
#include <stuff/pathstr.h>
#include <stuff/layout.h>
#include <stuff/comutils.h>
#include <stuff/objectset.h>

#include "visualeditor.h"
#include "visualeditordlg.h"

#include "CodePages.h"

#include <pr_prop.h>
#include <pr_types.h>
#include <pr_err.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPArray<TypeID>					gTypes(0, 1, true);
CPArray<TypeID>					gPropTypes(0, 1, true);
CPArray<PredefinedProp>			gHardProps(0, 1, true);
CPArray<PredefinedProp>			gSoftProps(0, 1, true);
CPArray<TypeConst>				gTypeConst(0, 1, true);
HDATA							ghSMRootData = NULL;
CVisualEditorDlg*				gpMainDlg = NULL; 

CObjectSet<int>	gSeparatorLead( 1, L'-' );

// ---
int GetConvertBase( const TCHAR *pS ) {
	int nBase = -1;
	if ( pS && *pS ) {
		while ( isspace((int)(unsigned char)*pS) || *pS == L'-' || *pS == L'+' )
			++pS;
		if ( *pS == L'0' ) {
			if ( *(pS + 1) == L'x' || *(pS + 1) == L'X' )
				return 16;
			nBase = 8;
			while ( *pS ) {
				if ( *pS >= L'8' && *pS <= L'9' ) {
					if ( nBase < 10 )
						nBase = 10;
				}
				if ( (*pS >= L'a' && *pS <= L'f') || (*pS >= L'A' && *pS <= L'F') ) 
					nBase = 16;
				pS++;
			}
			return nBase;
		}
		nBase = 10;
		while ( *pS ) {
			if ( *pS >= L'0' && *pS <= L'9' ) {
				if ( nBase < 10 )
					nBase = 10;
			}
			if ( (*pS >= L'a' && *pS <= L'f') || (*pS >= L'A' && *pS <= L'F') ) 
				nBase = 16;
			pS++;
		}
	}
	return nBase;
}

// ---
int A2I( const TCHAR *pS ) {
  while ( isspace((int)(unsigned char)*pS) )
    ++pS;
	int nBase = ::GetConvertBase( pS );
	if ( nBase > 0 )
		return strtoul( pS, NULL, nBase ); // !!!Petrovitch strtol( pS, NULL, nBase );
	return 0;
}

// ---
static BOOL IsHexDigit( int c ) {
	if ( !isdigit(c) )
		return tolower(c) >= L'a' && tolower(c) <= L'f';
	else
		return true;
}

// ---
__int64  A2I64( const TCHAR *pS, TCHAR **ppLast, int nBase ) {
	__int64 iR = 0;
	if ( pS && *pS ) {
		while ( isspace((int)(unsigned char)*pS) )
			++pS;
		if ( nBase == 16 ) {
			while ( *pS == L'-' || *pS == L'+' )
				++pS;
			if ( *pS == L'0' && (*(pS + 1) == L'x' || *(pS + 1) == L'X') )
				pS += 2;
			while( ::IsHexDigit(*pS) ) {
				if ( !isdigit(*pS) )
					iR = 16 * iR + (tolower(*pS) - L'a' + 10);
				else
					iR = 16 * iR + (*pS - L'0');
				pS++;
			}
		}
		else {
			if ( nBase == 10 ) {
				int c;              /* current char */
				int sign;           /* if '-', then negative, otherwise positive */

				c = (int)(unsigned char)*pS;
				sign = c;           /* save sign indication */
				if (c == '-' || c == '+')
						c = (int)(unsigned char)*(++pS);    /* skip sign */

				while ( isdigit(c) ) {
					iR = 10 * iR + (c - L'0');     /* accumulate digit */
					c = (int)(unsigned char)*(++pS);    /* get next char */
				}

				if (sign == '-')
					iR = -iR;
			}
			else {
				if ( nBase == 8 ) {
					while ( *pS == L'-' || *pS == L'+' )
						++pS;
					pS++;
					while( isdigit(*pS) ) {
						iR = 8 * iR + (*pS - L'0');
						pS++;
					}
				}
			}
		}
	}
	if ( ppLast )
		*ppLast = (TCHAR *)pS;
	return iR;
}

// ---
BOOL CheckConvertable( const TCHAR *pString ) {
	const TCHAR *pChar = pString;
	while ( *pChar )	{
		if ( !((*pChar >= L'a' && *pChar <= L'f') || 
					 (*pChar >= L'A' && *pChar <= L'F') || 
					 (*pChar >= L'0' && *pChar <= L'9') ||
					 (*pChar == L'x' || *pChar == L'X') ||
					 (*pChar == L'+' || *pChar == L'-')) )
			return false;

		pChar++;
	}
	TCHAR *pLast;
	int nBase = GetConvertBase( pString );
	::A2I64( pString, &pLast, nBase );
	return (pLast - pString) == lstrlen(pString);
}


// ---
TCHAR *StrDuplicate( const TCHAR *pStr ) {
	return lstrcpy( new TCHAR[pStr ? (lstrlen(pStr) + 1) : 1], pStr ? pStr : "" );
}


// ---------------------------------------------------------------------------------------------------------
// ---
CIObjectContainer::CIObjectContainer() :
  CPArray<CIObject>(0)	{
}
	
// ---
CIObject *CIObjectContainer::Find( const TCHAR *pObjectName ) {
	if ( pObjectName && *pObjectName ) {
		for ( int i=0,c=Count(); i<c; i++ ) {
			if ( !lstrcmpi(pObjectName, (*this)[i]->m_rcObjectName) )
				return (*this)[i];
		}
	}
	return NULL;
}

// ---
CIObject *CIObjectContainer::Find( AVP_dword dwID ) {
	for ( int i=0,c=Count(); i<c; i++ ) {
		if ( (*this)[i]->m_dwObjectID == dwID )
			return (*this)[i];
	}
	return NULL;
}


// ---
CIObject *CIObjectContainer::Add( const TCHAR *pObjectName ) {
	if ( pObjectName && *pObjectName ) {
		CIObject *pObject = Find( pObjectName );
		if ( !pObject ) {
			pObject = new CIObject(pObjectName);
			CPArray<CIObject>::Add( pObject );
		}
		return pObject;
	}

	return NULL;
}

// ---
CIObject *CIObjectContainer::Update( const TCHAR *pOldObjectName, const TCHAR *pNewObjectName ) {
	if ( pOldObjectName && *pOldObjectName && pNewObjectName && *pNewObjectName ) {
		CIObject *pObject = Find( pOldObjectName );
		if ( pObject ) {
			pObject->m_dwRefCount = 0;
			pObject->m_rcObjectName = pNewObjectName;
		}
		else {
			pObject = new CIObject( pNewObjectName );
			CPArray<CIObject>::Add( pObject );
		}
		return pObject;
	}
	return NULL;
}

// ---
CIObject *CIObjectContainer::Update( AVP_dword dwOldID, AVP_dword dwNewID ) {
	CIObject *pObject = Find( dwOldID );
	if ( pObject ) {
		pObject->m_dwRefCount = 0;
		pObject->m_dwObjectID = dwNewID;
	}
	return pObject;
}

// ---
BOOL CIObjectContainer::IsIncludeNameUsed( CIObject *pIObject ) {
	for ( int i=0,c=Count(); i<c; i++ ) {
		if ( !lstrcmp((*this)[i]->m_rcIncludeName, pIObject->m_rcIncludeName) && (*this)[i]->m_dwRefCount )
			return true;
	}
	return false;
}

// ---
void SaveUsedObject( CIObject *pIObject, HDATA hRootData ) {
	if ( hRootData ) {
		BOOL bFound = false;
		HPROP hValueProp;
		if ( hValueProp = ::DATA_Find_Prop(hRootData, NULL, VE_PID_IF_INCLUDENAMES) ) {
			AVP_dword dwCount = ::PROP_Arr_Count( hValueProp );
			for ( AVP_dword i=0; i<dwCount && !bFound; i++ ) {
				CAPointer<TCHAR> pValue = ::GetPropArrayValueAsString( hValueProp, i, NULL );
				if ( !lstrcmp(pValue, pIObject->m_rcIncludeName) ) {

					if ( !(hValueProp = ::DATA_Find_Prop(hRootData, NULL, VE_PID_IF_INCLUDEUIDS)) ) {
						hValueProp = ::DATA_Add_Prop( hRootData, NULL, VE_PID_IF_INCLUDEUIDS, 0, 0 );
						if ( hValueProp )
							::SetPropArrayValue( hValueProp, PROP_ARR_LAST, LPCTSTR(pIObject->m_UID), NULL );
					}
					else
						::SetPropArrayValue( hValueProp, i, LPCTSTR(pIObject->m_UID), NULL );
					return;
				}
			}
		}
		else
			hValueProp = ::DATA_Add_Prop( hRootData, NULL, VE_PID_IF_INCLUDENAMES, 0, 0 );

		if ( hValueProp && !bFound ) {
			::SetPropArrayValue( hValueProp, PROP_ARR_LAST, pIObject->m_rcIncludeName, NULL );
			if ( !(hValueProp = ::DATA_Find_Prop(hRootData, NULL, VE_PID_IF_INCLUDEUIDS)) ) 
				hValueProp = ::DATA_Add_Prop( hRootData, NULL, VE_PID_IF_INCLUDEUIDS, 0, 0 );
			if ( hValueProp )
				::SetPropArrayValue( hValueProp, PROP_ARR_LAST, LPCTSTR(pIObject->m_UID), NULL );
		}
	}
}

// ---
void RemoveUsedObject( CIObject *pIObject, HDATA hRootData ) {
	if ( hRootData ) {
		HPROP hValueProp;
		if ( hValueProp = ::DATA_Find_Prop(hRootData, NULL, VE_PID_IF_INCLUDENAMES) ) {
			AVP_dword dwCount = ::PROP_Arr_Count( hValueProp );
			for ( AVP_dword i=0; i<dwCount; i++ ) {
				CAPointer<TCHAR> pValue = ::GetPropArrayValueAsString( hValueProp, i, NULL );
				if ( !lstrcmp(pValue, pIObject->m_rcIncludeName) ) {
					::PROP_Arr_Remove( hValueProp, i, 1 );
					return;
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------

// ---
char *GTypeID2TypeName( DWORD nType ) {
	for ( int i=0,c=gTypes.Count(); i<c; i++ )
		if ( gTypes[i]->m_nType == nType )
			return gTypes[i]->m_pName;
  return NULL;
}


// ---
DWORD GTypeName2TypeID( const char *pTypeName ) {
	for ( int i=0,c=gTypes.Count(); i<c; i++ ) {
		const char* name = gTypes[i]->m_pName;
		if ( !lstrcmpi(name, pTypeName) )
			return gTypes[i]->m_nType;
	}
  return tid_VOID;
}


// ---
char *PropTypeID2PropTypeName( DWORD nType ) {
	for ( int i=0,c=gPropTypes.Count(); i<c; i++ )
		if ( gPropTypes[i]->m_nType == nType )
			return gPropTypes[i]->m_pName;
  return NULL;
}


// ---
DWORD PropTypeName2PropTypeID( const char *pTypeName ) {
	for ( int i=0,c=gPropTypes.Count(); i<c; i++ )
		if ( !lstrcmpi(gPropTypes[i]->m_pName, pTypeName) )
			return gPropTypes[i]->m_nType;
  return tid_VOID;
}


// ---
DWORD PropName2PropID( const char *pPropName, BOOL bHard ) {
	CPArray<PredefinedProp> *pProps = bHard ? &gHardProps : &gSoftProps;
	for ( int i=0,c=pProps->Count(); i<c; i++ )
		if ( !lstrcmpi((*pProps)[i]->m_pName, pPropName) )
			return (*pProps)[i]->m_nID;
  return 0;
}

// ---
char *PropName2PropTypeName( const char *pPropName, BOOL bHard ) {
	CPArray<PredefinedProp> *pProps = bHard ? &gHardProps : &gSoftProps;
	for ( int i=0,c=pProps->Count(); i<c; i++ )
		if ( !lstrcmpi((*pProps)[i]->m_pName, pPropName) )
			return (*pProps)[i]->m_pTypeName;
  return NULL;
}

// ---
DWORD PropName2PropType( const char *pPropName, BOOL bHard ) {
	CPArray<PredefinedProp> *pProps = bHard ? &gHardProps : &gSoftProps;
	for ( int i=0,c=pProps->Count(); i<c; i++ )
		if ( !lstrcmpi((*pProps)[i]->m_pName, pPropName) )
			return (*pProps)[i]->m_nType;
  return 0;
}

// ---
AVP_dword MaxTypeID() {
  AVP_dword dwID = 0;
  for ( int i=0,c=gTypes.Count(); i<c; i++ ) {
    AVP_dword dwCurrID = gTypes[i]->m_nType;
		if ( dwCurrID > dwID )
      dwID = dwCurrID;
  }
  if ( dwID > 0 )
    dwID++;
  return dwID;
}


// ---
AVP_dword CreateDataId( HDATA hParentData ) {
#if 0
  return ::CreateUniqueId();
#else
	CSArray<UINT> rcIdArray;
  HDATA hData = ::DATA_Get_First( hParentData, NULL );
	while ( hData )	{
		AVP_dword nDataPID = ::DATA_Get_Id( hData, NULL ); 
		rcIdArray.Add( GET_AVP_PID_ID(nDataPID)	);
    hData = ::DATA_Get_Next( hData, NULL );
	}

	return ::GetMinUnusedValue( rcIdArray );
#endif
}

// ---
void DestroyDataId( AVP_dword nDataId ) {
#if 0
  ::DestroyUniqueId( nDataId );
#else
#endif
}

// ---
bool FindForNodeID( CTreeWnd &owner, HTREEITEM cur, int ind, void *par ) {
	CCTItemsArray &items = *((CCheckTree &)owner).GetItemsArray();
	CCheckTreeItem *item = items[ind];

	if ( !IS_EMPTY_DATA(item) &&
			 CPPD(item)->m_hProp &&
			 CPPD(item)->m_hProp != ::DATA_Find_Prop(CPPD(item)->m_hData, NULL, 0) &&
		   GET_AVP_PID_ID(::PROP_Get_Id(CPPD(item)->m_hProp)) == AVP_dword(par) ) 
		return true;

	return false;
}


// ---
bool FindForNodeType( CTreeWnd &owner, HTREEITEM cur, int ind, void *par ) {
	CCTItemsArray &items = *((CCheckTree &)owner).GetItemsArray();
	CCheckTreeItem *item = items[ind];

	if ( !IS_EMPTY_DATA(item) && 
		   GET_NODE_TYPE(item) == int(par) ) 
		return true;

	return false;
}


// ---
bool FindForNodeHDATA( CTreeWnd &owner, HTREEITEM cur, int ind, void *par ) {
	CCTItemsArray &items = *((CCheckTree &)owner).GetItemsArray();
	CCheckTreeItem *item = items[ind];

	if ( !IS_EMPTY_DATA(item) && 
		   CPPD(item)->m_hData == HDATA(par) ) 
		return true;

	return false;
}


// ---
BOOL IsTypeProlog( CControlTree &rcTree, int nItemInd, int nDescrType )	{
	CCTItemsArray &items = *rcTree.GetItemsArray();
	CCheckTreeItem *item = items[nItemInd];

	return ( nItemInd >= 0 && 
			!IS_EMPTY_DATA(item) && 
			 (CPPD(item)->m_nSpecialType  & ~sdt_Label) == nDescrType );
}

// ---
BOOL IsIDProlog( CControlTree &rcTree, int nItemInd, AVP_dword nID )	
{
	AVP_dword dwPropId = GetTreeNodePropId(rcTree, nItemInd);
	return dwPropId == nID;
}

// ---
AVP_dword GetTreeNodePropId(CControlTree &rcTree, int nItemInd)
{
	AVP_dword dwRetVal = AVP_dword(-1);

	if(nItemInd < 0)
		return dwRetVal;

	CCTItemsArray &items = *rcTree.GetItemsArray();
	CCheckTreeItem *item = items[nItemInd];

	if(	IS_EMPTY_DATA(item) || 
		NULL == CPPD(item)->m_hProp ||
		CPPD(item)->m_hProp == ::DATA_Find_Prop(CPPD(item)->m_hData, NULL, 0))
		return dwRetVal;

	dwRetVal = GET_AVP_PID_ID(::PROP_Get_Id(CPPD(item)->m_hProp));
	return dwRetVal;
}

// ---
void DeleteNodesData( CTreeWnd &owner, HTREEITEM cur, int ind, void *par ) {
	CCTItemsArray &items = *((CCheckTree &)owner).GetItemsArray();
	CCheckTreeItem *item = items[ind];
		// ”ничтожить идентификатор узла
	if ( !IS_EMPTY_DATA(item) &&
			 IS_NODE_LABEL(item) &&
		   CPPD(item)->m_hData ) {
		HDATA hData = CPPD(item)->m_hData;
		AVP_dword nDataPID = ::DATA_Get_Id( hData, NULL );
		::DestroyDataId( GET_AVP_PID_ID(nDataPID) );
	}
	delete CPPD(item);
}


// ---
void FullyDeleteItem( CControlTree &rcTree, int nItemInd, BOOL bRemoveData )	{
	CCTItemsArray &items = *rcTree.GetItemsArray();

	// ”далить данные в элементах поддерева
	rcTree.ForEach( ::DeleteNodesData, NULL, nItemInd );

	if ( nItemInd >= 0 ) {
		// ”ничтожить идентификатор узла
 		CCheckTreeItem *item = items[nItemInd];
		AVP_dword nDataPID = ::DATA_Get_Id( CPPD(item)->m_hData, NULL );
		::DestroyDataId( GET_AVP_PID_ID(nDataPID) );

		if ( bRemoveData )
			// ”ничтожить ветвь в дереве данных
			::DATA_Remove( CPPD(item)->m_hData, NULL );

		// ”ничтожить данные узла
		delete CPPD(item);
		item->SetData( NULL, false );
	}

	// ”ничтожить поддерево элементов
	rcTree.DeleteItemWithSubtree( nItemInd );
}

// ---
HDATA FindParentHData( CCheckTree *pTree, int nIndex ) {
	HDATA hParentData = NULL;
	int nParentInd = pTree->GetParentIndex( nIndex );

	if ( nParentInd >= 0 ) {
		CCTItemsArray &items = *pTree->GetItemsArray();
		CCheckTreeItem *item = items[nParentInd];
		hParentData = CPPD(item)->m_hData;
		if ( !hParentData )
			return FindParentHData( pTree, nParentInd );
	}
	return hParentData;
}

// ---

BOOL CheckNumeric( const TCHAR *pString ) 
{
	for (const TCHAR *pChar = pString; *pChar;  pChar++)	
	{
		if( 
			!(*pChar == L'X' || *pChar == L'x' || *pChar == L'-' ||
				(*pChar >= L'0' && *pChar <= L'9'))
					 
		)
			return false;
	}
	return true;
}

BOOL CheckAlphabetical( const TCHAR *pString ) 
{
	const TCHAR *pChar = pString;
	while ( *pChar )	{
		if ( !((*pChar >= L'a' && *pChar <= L'z') || 
					 (*pChar >= L'A' && *pChar <= L'Z') || 
					 (*pChar >= L'0' && *pChar <= L'9') ||
					 *pChar == L'_' ||
					 *pChar == L'-' ||
					 *pChar == L' ') )
			return false;

		pChar++;
	}
	return true;
}


// ---
BOOL CheckEditedValue( CWnd *pOwner, const TCHAR *pText, BOOL bDigital ) 
{
	if ( !::IsSeparator(pText) ) 
	{
		if ( !bDigital ) 
		{
			if ( !pText || !*pText || !::CheckAlphabetical(pText) ) 
			{
				::DlgMessageBox( pOwner, IDS_ERR_INCORRECTNAMEVALUE, MB_ICONSTOP | MB_OK );
				return false;
			}
		}
		else {
			//int nRawID = A2I( pText );
			//if ( !nRawID || nRawID < 0 ) 
			if(!CheckNumeric(pText))
			{
				::DlgMessageBox( pOwner, IDS_ERR_INCORRECTIDVALUE, MB_ICONSTOP | MB_OK );
				return false;
			}
		}
	}
	return true;
}

// ---
void CVisualEditorDlg::LoadTypesTable() {
	CAPointer<TCHAR> pStartup = ::GetStartupDirectory();
	CPathStr pFileName = pStartup;
	pFileName ^= "PTypeTbl.ptt";

	HDATA hRootData = NULL;
	if ( ::KLDT_DeserializeUsingSWM(pFileName, &hRootData) && hRootData ) {
    m_types.Init( hRootData, true );
    
    MAKE_ADDR1(nTypesAddr		 , TT_PID_ROOT);
		MAKE_ADDR1(nPropTypesAddr, PT_PID_ROOT);
		MAKE_ADDR1(nHardPropsAddr, HP_PID_ROOT);
		MAKE_ADDR1(nSoftPropsAddr, SP_PID_ROOT);
		MAKE_ADDR1(nTypeConstAddr, TC_PID_ROOT);

		HDATA hTypesData = ::DATA_Find( hRootData, nTypesAddr );
		if ( hTypesData ) {
			HPROP hNameProp		= ::DATA_Find_Prop( hTypesData, NULL, 0 );
			HPROP hTypeProp		= ::DATA_Find_Prop( hTypesData, NULL, TT_PID_TYPEID );
			HPROP hDTTypeProp	= ::DATA_Find_Prop( hTypesData, NULL, TT_PID_DTTYPEID );
			HPROP hSizeOfProp	= ::DATA_Find_Prop( hTypesData, NULL, TT_PID_SIZEOF );
			if ( hNameProp && hTypeProp && hDTTypeProp && hSizeOfProp ) {
				AVP_dword dwNameCount = ::PROP_Arr_Count( hNameProp );
				AVP_dword dwTypeCount = ::PROP_Arr_Count( hTypeProp );
				AVP_dword dwDTTypeCount = ::PROP_Arr_Count( hDTTypeProp );
				AVP_dword dwSizeOfCount = ::PROP_Arr_Count( hSizeOfProp );
				if ( dwNameCount == dwTypeCount && dwTypeCount == dwDTTypeCount && dwNameCount == dwSizeOfCount ) {
					for ( int i=0; i<(int)dwNameCount; i++ ) {
						TypeID *pType = new TypeID;
						pType->m_nType = ::GetPropArrayNumericValueAsDWord( hTypeProp, i );
						pType->m_nDTType = ::GetPropArrayNumericValueAsDWord( hDTTypeProp, i );
						pType->m_pName = ::GetPropArrayValueAsString( hNameProp, i, NULL );
						pType->m_nSizeOf = ::GetPropArrayNumericValueAsDWord( hSizeOfProp, i );
						gTypes.Add( pType );
					}
				}
			}
		}

		HDATA hPropTypesData = ::DATA_Find( hRootData, nPropTypesAddr );
		if ( hPropTypesData ) {
			HPROP hNameProp = ::DATA_Find_Prop( hPropTypesData, NULL, 0 );
			HPROP hTypeProp = ::DATA_Find_Prop( hPropTypesData, NULL, PT_PID_TYPEID );
			HPROP hDTTypeProp = ::DATA_Find_Prop( hPropTypesData, NULL, PT_PID_DTTYPEID );
			HPROP hSizeOfProp	= ::DATA_Find_Prop( hPropTypesData, NULL, PT_PID_SIZEOF );
			if ( hNameProp && hTypeProp && hDTTypeProp && hSizeOfProp ) {
				AVP_dword dwNameCount = ::PROP_Arr_Count( hNameProp );
				AVP_dword dwTypeCount = ::PROP_Arr_Count( hTypeProp );
				AVP_dword dwDTTypeCount = ::PROP_Arr_Count( hDTTypeProp );
				AVP_dword dwSizeOfCount = ::PROP_Arr_Count( hSizeOfProp );
				if ( dwNameCount == dwTypeCount && dwTypeCount == dwDTTypeCount && dwNameCount == dwSizeOfCount ) {
					for ( int i=0; i<(int)dwNameCount; i++ ) {
						TypeID *pType = new TypeID;
						pType->m_nType = ::GetPropArrayNumericValueAsDWord( hTypeProp, i );
						pType->m_nDTType = ::GetPropArrayNumericValueAsDWord( hDTTypeProp, i );
						pType->m_pName = ::GetPropArrayValueAsString( hNameProp, i, NULL );
						pType->m_nSizeOf = ::GetPropArrayNumericValueAsDWord( hSizeOfProp, i );
						gPropTypes.Add( pType );
					}
				}
			}
		}

		HDATA hHardPropsData = ::DATA_Find( hRootData, nHardPropsAddr );
		if ( hHardPropsData ) {
			HPROP hNameProp					= ::DATA_Find_Prop( hHardPropsData, NULL, 0 );
			HPROP hTypeProp					= ::DATA_Find_Prop( hHardPropsData, NULL, HP_PID_TYPEID );
			HPROP hSharedProp				= ::DATA_Find_Prop( hHardPropsData, NULL, HP_PID_SHOULDBESHARED );
			HPROP hDefValProp				= ::DATA_Find_Prop( hHardPropsData, NULL, HP_PID_SHAREDDEFVALUE );
      HPROP hShortComProp			= ::DATA_Find_Prop( hHardPropsData, NULL, HP_PID_SHORTCOMMENT );
      HPROP hLargeComProp			= ::DATA_Find_Prop( hHardPropsData, NULL, HP_PID_LARGECOMMENT );
      HPROP hBehaviorComProp	= ::DATA_Find_Prop( hHardPropsData, NULL, HP_PID_BEHAVIORCOMMENT );
			if ( hNameProp && hTypeProp && hSharedProp && hDefValProp && hShortComProp && hLargeComProp && hBehaviorComProp ) {
				AVP_dword dwNameCount					= ::PROP_Arr_Count( hNameProp );
				AVP_dword dwTypeCount					= ::PROP_Arr_Count( hTypeProp );
				AVP_dword dwSharedCount				= ::PROP_Arr_Count( hSharedProp );
				AVP_dword dwDefValCount				= ::PROP_Arr_Count( hDefValProp );
				AVP_dword dwShortComCount			= ::PROP_Arr_Count( hShortComProp );
				AVP_dword dwLargeComCount			= ::PROP_Arr_Count( hLargeComProp );
				AVP_dword dwBehaviorComCount	= ::PROP_Arr_Count( hBehaviorComProp );
				if ( dwNameCount == dwTypeCount && dwNameCount == dwSharedCount && dwNameCount == dwDefValCount ) {
					for ( int i=0; i<(int)dwNameCount; i++ ) {
						PredefinedProp *pProp = new PredefinedProp;
						pProp->m_pName						= ::GetPropArrayValueAsString( hNameProp, i, NULL );
						AVP_dword dwType					= ::GetPropArrayNumericValueAsDWord( hTypeProp, i );
						pProp->m_nID							= (dwType & pNUMBER_MASK);
						pProp->m_nType						= (dwType & pTYPE_MASK) >> pTYPE_SHIFT;
						pProp->m_pTypeName				= ::StrDuplicate(::PropTypeID2PropTypeName(pProp->m_nType) );
						pProp->m_bShared					= ::GetPropArrayNumericValueAsDWord( hSharedProp, i );
						pProp->m_pDefValue				= ::GetPropArrayValueAsString( hDefValProp, i, NULL );
						pProp->m_pShortComment		= ::GetPropArrayValueAsString( hShortComProp, i, NULL );
						pProp->m_pLargeComment		= ::GetPropArrayValueAsString( hLargeComProp, i, NULL );
						pProp->m_pBehaviorComment = ::GetPropArrayValueAsString( hBehaviorComProp, i, NULL );
						gHardProps.Add( pProp );
					}
				}
			}
		}

		HDATA hSoftPropsData = ::DATA_Find( hRootData, nSoftPropsAddr );
		if ( hSoftPropsData ) {
			HPROP hNameProp					= ::DATA_Find_Prop( hSoftPropsData, NULL, 0 );
			HPROP hTypeProp					= ::DATA_Find_Prop( hSoftPropsData, NULL, SP_PID_TYPEID );
			HPROP hSharedProp				= ::DATA_Find_Prop( hSoftPropsData, NULL, SP_PID_SHOULDBESHARED );
			HPROP hDefValProp				= ::DATA_Find_Prop( hSoftPropsData, NULL, SP_PID_SHAREDDEFVALUE );
      HPROP hShortComProp			= ::DATA_Find_Prop( hSoftPropsData, NULL, SP_PID_SHORTCOMMENT );
      HPROP hLargeComProp			= ::DATA_Find_Prop( hSoftPropsData, NULL, SP_PID_LARGECOMMENT );
      HPROP hBehaviorComProp	= ::DATA_Find_Prop( hSoftPropsData, NULL, SP_PID_BEHAVIORCOMMENT );
			if ( hNameProp && hTypeProp && hSharedProp && hDefValProp && hShortComProp && hLargeComProp && hBehaviorComProp ) {
				AVP_dword dwNameCount					= ::PROP_Arr_Count( hNameProp );
				AVP_dword dwTypeCount					= ::PROP_Arr_Count( hTypeProp );
				AVP_dword dwSharedCount				= ::PROP_Arr_Count( hSharedProp );
				AVP_dword dwDefValCount				= ::PROP_Arr_Count( hDefValProp );
				AVP_dword dwShortComCount			= ::PROP_Arr_Count( hShortComProp );
				AVP_dword dwLargeComCount			= ::PROP_Arr_Count( hLargeComProp );
				AVP_dword dwBehaviorComCount	= ::PROP_Arr_Count( hBehaviorComProp );
				if ( dwNameCount == dwTypeCount && dwNameCount == dwSharedCount && dwNameCount == dwDefValCount ) {
					for ( int i=0; i<(int)dwNameCount; i++ ) {
						PredefinedProp *pProp			= new PredefinedProp;
						pProp->m_pName						= ::GetPropArrayValueAsString( hNameProp, i, NULL );
						AVP_dword dwType					= ::GetPropArrayNumericValueAsDWord( hTypeProp, i );
						pProp->m_nID							= (dwType & pNUMBER_MASK);
						pProp->m_nType						= (dwType & pTYPE_MASK) >> pTYPE_SHIFT;
						pProp->m_pTypeName				= ::StrDuplicate( ::PropTypeID2PropTypeName(pProp->m_nType) );
						pProp->m_bShared					= ::GetPropArrayNumericValueAsDWord( hSharedProp, i );
						pProp->m_pDefValue				= ::GetPropArrayValueAsString( hDefValProp, i, NULL );
						pProp->m_pShortComment		= ::GetPropArrayValueAsString( hShortComProp, i, NULL );
						pProp->m_pLargeComment		= ::GetPropArrayValueAsString( hLargeComProp, i, NULL );
						pProp->m_pBehaviorComment = ::GetPropArrayValueAsString( hBehaviorComProp, i, NULL );
						gSoftProps.Add( pProp );
					}
				}
			}
		}

		HDATA hConstData = ::DATA_Find( hRootData, nTypeConstAddr );
		if ( hConstData ) {
			HDATA hCurrData = ::DATA_Get_First( hConstData, NULL );
			while ( hCurrData ) {
				TypeConst *pTypeConst = new TypeConst;
				pTypeConst->m_dwPragueType = ::GetPropNumericValueAsDWord(::DATA_Find_Prop(hCurrData, NULL, 0));
				HPROP hNameProp  = ::DATA_Find_Prop( hCurrData, NULL, TC_PID_CONSTANTNAME );
				HPROP hValueProp = ::DATA_Find_Prop( hCurrData, NULL, TC_PID_CONSTANTVALUE );
				if ( hNameProp && hValueProp ) {
					AVP_dword dwNameCount = ::PROP_Arr_Count( hNameProp );
					AVP_dword dwValueCount = ::PROP_Arr_Count( hValueProp );
					for ( int i=0; i<(int)dwNameCount; i++ ) {
						ConstValue *pConst = new ConstValue;
						pConst->m_pConstName = ::GetPropArrayValueAsString( hNameProp, i, NULL );
						pConst->m_pValue		 = ::GetPropArrayValueAsString( hValueProp, i, NULL );
						::PROP_Arr_Get_Items( hValueProp, i, &pConst->m_dwValue, sizeof(pConst->m_dwValue) );
						pTypeConst->m_Constants.Add( pConst );
					}
				}

				gTypeConst.Add( pTypeConst );

				hCurrData = ::DATA_Get_Next( hCurrData, NULL );
			}
		}

		MAKE_ADDR1( nAddr1, VE_PID_IF_SYSTEMMETHODS );
		ghSMRootData = ::DATA_Find( hRootData, nAddr1 );
		if ( ghSMRootData )
			::DATA_Detach( ghSMRootData, NULL );

		//::DATA_Remove( hRootData, NULL );
	}
}

// ---
char *GetPureValueString( HPROP hProp ) {
	if ( NUMERIC_TYPE(GET_AVP_PID_TYPE(::PROP_Get_Id(hProp))) )
		return ::GetPropValueAsString( hProp, "%#x" );
	else
		return ::GetPropValueAsString( hProp, NULL );
}


// ---
void GetPureInfoValueString( CTTMessageInfo *info, HPROP hProp ) {
  if ( hProp ) {
	  delete [] info->m_Value.m_pItemText;
	  info->m_Value.m_pItemText = ::GetPureValueString( hProp );
  }
}

// ---
void GetCodePageValueString( CTTMessageInfo *info, HPROP i_hProp ) {

	CCodePages oCP;
	CString strCPName = oCP.GetCodePageById(::GetPropNumericValueAsDWord( i_hProp ));
	TCHAR* szText = new TCHAR[strCPName.GetLength() + 1];
	lstrcpy(szText, strCPName);

	delete [] info->m_Value.m_pItemText;
	info->m_Value.m_pItemText = szText;
}

// ---
void GetSubtypeValueString( CTTMessageInfo *info, HPROP i_hProp ) {

	CAPointer<char> strPropValue = ::GetPureValueString( i_hProp );

	char* strValue = GetConstantNameOnValue(strPropValue);
	if(NULL == strValue)
		strValue = strPropValue.Relinquish();

	delete [] info->m_Value.m_pItemText;
	info->m_Value.m_pItemText = strValue;
}

// ---
/*
void GetSubtypeDisplayStringWithPrefix( CTTMessageInfo *info, HPROP hProp ) {

	CAPointer<char> strPropValue = ::GetPureValueString( hProp );

	char* strValue = GetConstantNameOnValue(strPropValue);
	if(NULL == strValue)
		strValue = strPropValue.Relinquish();

	CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_SUBTYPEID_PREFIX, strValue);
	delete strValue;
	delete [] info->m_Value.m_pItemText;
	info->m_Value.m_pItemText = pLabel.Relinquish();
}
	 
// ---
void GetCodePageValueDisplayString( CTTMessageInfo *info, DWORD dwPrefixID, HPROP i_hProp ) {
	
	CCodePages oCP;
	CString strCPName = oCP.GetCodePageById(::GetPropNumericValueAsDWord( i_hProp ));
	
	delete [] info->m_Value.m_pItemText;
	info->m_Value.m_pItemText = ::CreateDisplayLabelSTR( dwPrefixID, strCPName );
}
	   */

// ---
void GetValueDisplayString( CTTMessageInfo *info, DWORD dwPrefixID, HPROP hProp ) {
	CAPointer<TCHAR>	pValue;

	if ( NUMERIC_TYPE(GET_AVP_PID_TYPE(::PROP_Get_Id(hProp))) )
		pValue = ::GetPropValueAsString( hProp, "%#x" );
	else
		pValue = ::GetPropValueAsString( hProp, NULL );

	delete [] info->m_Value.m_pItemText;
	info->m_Value.m_pItemText = ::CreateDisplayLabelSTR( dwPrefixID, pValue );
}

// ---
static int GetTextSize( LPCTSTR pText, HDC hDC ) {
  return LOWORD(::GetTabbedTextExtent(hDC, pText, lstrlen(pText), 0, NULL));
}


// ---
void GetSeparatorDisplayString( CPathStr &pDispStr, LPCTSTR pValueStr ) {
	pDispStr += "<---------- ";
	pDispStr += pValueStr;
	pDispStr += " ---------->";
}

// ---
void GetLargeCommentValueDisplayString( CTTMessageInfo *info, DWORD dwPrefixID, HDATA hData, AVP_dword dwCommentID ) {
	CAPointer<TCHAR> pLabel = ::CreateLargeCommentLabel( hData, dwCommentID );

	delete [] info->m_Value.m_pItemText;
	info->m_Value.m_pItemText = ::CreateDisplayLabelSTR( dwPrefixID, pLabel );
}


// ---
static AVP_dword GetMethodResultType( HDATA hData ) {
	AVP_dword nType = avpt_nothing;
	
	HPROP hValueProp;
	if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFM_RESULTTYPE ) ) 
		::PROP_Get_Val( hValueProp, &nType, sizeof(nType) );

	return nType;
}


// ---
struct SetTextInfo {
	DWORD  m_dwSiblingID;
	int    m_nIndex;
	TCHAR  *m_pText;
	BOOL   m_bChecked;
};

// ---
static void SetChildText( CTreeWnd &owner, HTREEITEM cur, int ind, void *par ) {
	if ( ((SetTextInfo *)par)->m_nIndex != ind ) {
		CControlTree &rcTree = (CControlTree &)owner;
		if ( ::IsIDProlog(rcTree, ind, ((SetTextInfo *)par)->m_dwSiblingID) ) 
			rcTree.SetTreeItemText( ind, ((SetTextInfo *)par)->m_pText );
	}
}


// ---
void SetParentChildNodeText( CControlTree &rcTree, int nItemInd, int nParentType, DWORD dwSiblingID, char *pText )	{
	if ( nItemInd >= 0 ) {
		CCTItemsArray &items = *rcTree.GetItemsArray();
		int nParentInd = rcTree.GetParentIndex( nItemInd );
		while ( nParentInd >= 0 ) {
			CCheckTreeItem *item = items[nParentInd];
			if ( GET_NODE_TYPE(item) == nParentType ) {
				SetTextInfo rInfo;
				rInfo.m_dwSiblingID = dwSiblingID;
				rInfo.m_pText = pText;
				rInfo.m_nIndex = nItemInd;

				rcTree.ForEach( ::SetChildText, &rInfo, nParentInd );
				return;
			}
			nParentInd = rcTree.GetParentIndex( nParentInd );
		}
	}
}

// ---
struct FindChildInfo {
	DWORD  m_dwSiblingID;
	int    m_nIndex;
};

// ---
static bool FindChild( CTreeWnd &owner, HTREEITEM cur, int ind, void *par ) {
	if ( ((SetTextInfo *)par)->m_nIndex != ind ) {
		CControlTree &rcTree = (CControlTree &)owner;
		return !!::IsIDProlog(rcTree, ind, ((SetTextInfo *)par)->m_dwSiblingID);
	}
	return false;
}


// ---
int FindParentChildNodeIndex( CControlTree &rcTree, int nItemInd, int nParentType, DWORD dwSiblingID )	{
	if ( nItemInd >= 0 ) {
		CCTItemsArray &items = *rcTree.GetItemsArray();
		int nParentInd = rcTree.GetParentIndex( nItemInd );
		while ( nParentInd >= 0 ) {
			CCheckTreeItem *item = items[nParentInd];
			if ( GET_NODE_TYPE(item) == nParentType ) {
				FindChildInfo rInfo;
				rInfo.m_dwSiblingID = dwSiblingID;
				rInfo.m_nIndex = nItemInd;

				return rcTree.FirstThat( ::FindChild, &rInfo, nParentInd );
			}
			nParentInd = rcTree.GetParentIndex( nParentInd );
		}
	}
	return -1;
}

// ---
HDATA FindPropDataByName( HDATA hRootData, LPCTSTR pPropName ) {
	HDATA hFoundData = NULL;
	if ( hRootData ) {
		HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
		while ( hCurrData && !hFoundData ) {
			HPROP hValueProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IFP_PROPNAME );
			if ( hValueProp ) {
				CAPointer<TCHAR> pValue = ::GetPropValueAsString( hValueProp, NULL );
				if ( !lstrcmp(pValue, pPropName) )
					hFoundData = hCurrData;
			}
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
	}
	return hFoundData;
}


// ---
BOOL CheckHardPropExists( HDATA hPropData ) {
	HPROP hValueProp = ::DATA_Find_Prop( hPropData, NULL, VE_PID_IFP_PROPNAME );
	if ( hValueProp ) {
		CAPointer<TCHAR> pValue = ::GetPropValueAsString( hValueProp, NULL );
		for ( int i=0,c=gHardProps.Count(); i<c; i++ ) {
			PredefinedProp *pProp = gHardProps[i];
			if ( !lstrcmp(pValue, pProp->m_pName) )
				return true;
		}
	}
	return false;
}





// ---
void AddHeaderGuidProp( HDATA hData ) {
	GUID rGUID;
	if ( SUCCEEDED(::CoCreateGuid(&rGUID)) ) {
		OLECHAR *pWGuidStr;
		CHAR pGuidStr[40] = {0};
		if ( SUCCEEDED(::StringFromCLSID(rGUID, &pWGuidStr)) ) {
			::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, pWGuidStr, -1, pGuidStr, sizeof(pGuidStr), 0, 0 );
			::CoTaskMemFree( pWGuidStr );
			CString rGuid = pGuidStr;
			rGuid.MakeUpper();
			rGuid.Replace( L'-', L'_' );
			rGuid.Remove( L'{' );
			rGuid.Remove( L'}' );

			HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_HEADERGUID );
			if ( !hProp ) 
				::DATA_Add_Prop( hData, NULL, VE_PID_IF_HEADERGUID, AVP_dword(LPCSTR(rGuid)), 0 );
			else
				::PROP_Set_Val( hProp, AVP_dword(LPCSTR(rGuid)), 0 );
		}
	}
}

// ---
BOOL IsLinkedToFile( HDATA hData ) {
	return hData && 
		(!!::DATA_Find_Prop( hData, NULL, VE_PID_IF_IMPORTFILENAME ) ||
		 !!::DATA_Find_Prop( hData, NULL, VE_PID_IF_LINKEDBYINTERFACE ));
}

// ---
BOOL IsParentLinkedToFile( HDATA hData ) {
	while ( hData && !::DATA_Find_Prop(hData, NULL, VE_PID_IF_LASTINTERFACESLEVEL) )
		hData = ::DATA_Get_Dad( hData, NULL );
	return ::IsLinkedToFile( hData );
}

// ---
BOOL IsImported( HDATA hData ) {
	AVP_bool bImported = FALSE;
	HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_IMPORTED );
	if ( hProp )
		::PROP_Get_Val( hProp, &bImported, sizeof(bImported) );
	return !!bImported;
}

// ---
BOOL IsEllipsis( const char *pName ) {
	return !lstrcmp( pName, "..." );
}


// ---
BOOL IsSeparator( HDATA hData ) {
	return hData ? !!::DATA_Find_Prop( hData, NULL, VE_PID_SEPARATOR ) : FALSE;
}

// ---
BOOL IsSelected( HDATA hData ) {
	if ( hData ) {
		HPROP hSelectedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_SELECTED );
		if ( hSelectedProp )
			return !!::GetPropNumericValueAsDWord( hSelectedProp );
	}

	return FALSE;
}

// ---
BOOL IsSeparator( LPCTSTR pszNodeText ) {
	return (gSeparatorLead | *pszNodeText);
}


// ---
BOOL SetSeparator( HDATA hData, LPTSTR pszNodeText ) {
	if ( gSeparatorLead | *pszNodeText ) {
		lstrcpy( pszNodeText, pszNodeText + 1 );
		if ( !::DATA_Find_Prop(hData, NULL, VE_PID_SEPARATOR) )
			::DATA_Add_Prop( hData, NULL, VE_PID_SEPARATOR, 0, 0 );
		return TRUE;
	}
	return FALSE;
}

// ---
BOOL SetSeparator( HDATA hData, CString& pszNodeText ) {
	if ( gSeparatorLead | pszNodeText[0] ) {
		if ( !::DATA_Find_Prop(hData, NULL, VE_PID_SEPARATOR) )
			::DATA_Add_Prop( hData, NULL, VE_PID_SEPARATOR, 0, 0 );

		CString strText = 1 + (LPCTSTR)pszNodeText;
		pszNodeText.Format(_T("<---------- %s ---------->"), strText);
		return TRUE;
	}
	return FALSE;
}

// ---
AVP_dword GetPropDTTypeByPragueType( HDATA hData ) {
	HPROP hTypeProp;
	if ( hTypeProp = ::DATA_Find_Prop(hData, NULL, VE_PID_IFP_TYPE) ) {
		AVP_dword dwPragueType = ::GetPropNumericValueAsDWord(hTypeProp);
		for ( int i=0,c=gPropTypes.Count(); i<c; i++ ) {
			if ( gPropTypes[i]->m_nType == dwPragueType ) {
				return gPropTypes[i]->m_nDTType;
			}
		}
	}
	return avpt_nothing;
}


// ---
static int GetSizeOfType( AVP_dword dwCheckType ) {
	for ( int i=0,c=gTypes.Count(); i<c; i++ )
		if ( gTypes[i]->m_nType == dwCheckType )
			return int(gTypes[i]->m_nSizeOf);
	return -1;
}

// ---
static BOOL CheckConstantAcceptable( AVP_dword dwCheckType, const char *pszCheckValue, AVP_dword dwMainType ) {
	int dwCheckSize = ::GetSizeOfType( dwCheckType );
	int dwMainSize = ::GetSizeOfType( dwMainType );
	if ( dwCheckSize > 0 && dwMainSize > 0 ) {
		if ( dwCheckSize <= dwMainSize )
			return true;

		if ( pszCheckValue ) {
			int nBase = ::GetConvertBase( pszCheckValue );
			char *pEndPtr;
			AVP_qword nValue = ::A2I64( pszCheckValue, &pEndPtr, nBase );
			nValue >>= (dwMainSize * CHAR_BIT);

			return !nValue;
		}
	}
	return false;
}

// --- 
void CollectCodePages(CStringList &rContList)
{
	CCodePages oCP;

	CString strCPName = oCP.GetFirstCodePage();

	rContList.RemoveAll();
	while(!strCPName.IsEmpty())
	{
		rContList.AddTail(strCPName);
		strCPName = oCP.GetNextCodePage(); 
	}
} 

// --- 
void CollectConstantNames( HDATA hRootData, CStringList &pContList, DWORD dwPragueType ) {
	for ( int i=0,c=gTypeConst.Count(); i<c; i++ ) {
		TypeConst *pTypeConst = gTypeConst[i];
		if ( pTypeConst->m_dwPragueType == dwPragueType ) {
			for ( int j=0,n=pTypeConst->m_Constants.Count(); j<n; j++ ) {
				pContList.AddTail( pTypeConst->m_Constants[j]->m_pConstName );
			}
		}
		else {
			for ( int j=0,n=pTypeConst->m_Constants.Count(); j<n; j++ ) {
				if ( ::CheckConstantAcceptable(pTypeConst->m_dwPragueType, pTypeConst->m_Constants[j]->m_pValue, dwPragueType) )
					pContList.AddTail( pTypeConst->m_Constants[j]->m_pConstName );
			}
		}
	}

	MAKE_ADDR1( nConstAddr, VE_PID_IF_CONSTANTS );
	HDATA hCurrIntData = ::DATA_Get_First( hRootData, NULL );
	while ( hCurrIntData ) {
		HDATA hConstRootData = ::DATA_Find( hCurrIntData, nConstAddr );
		if ( hConstRootData ) {
			HDATA hCurrConstData = ::DATA_Get_First( hConstRootData, NULL );
			while ( hCurrConstData ) {
				HPROP hValueProp = ::DATA_Find_Prop( hCurrConstData, NULL, VE_PID_IFC_VALUE );
				if ( hValueProp ) {
					CAPointer<TCHAR> pValue = ::GetPropValueAsString( hValueProp, NULL );
					if ( ::CheckConvertable(pValue) ) {
						HPROP hTypeProp = ::DATA_Find_Prop( hCurrConstData, NULL, VE_PID_IFC_BASETYPE );
						if ( hTypeProp ) {
							AVP_dword dwCurrType = ::GetPropNumericValueAsDWord( hTypeProp );
							if ( dwCurrType == dwPragueType )	{
								HPROP hNameProp = ::DATA_Find_Prop( hCurrConstData, NULL, VE_PID_IFC_CONSTANTNAME );
								if ( hNameProp ) {
									CAPointer<TCHAR> pName = ::GetPropValueAsString( hNameProp, NULL );
									pContList.AddTail( pName );
								}
							}
							else {
								if ( ::CheckConstantAcceptable(dwCurrType, pValue, dwPragueType) ) {
									HPROP hNameProp = ::DATA_Find_Prop( hCurrConstData, NULL, VE_PID_IFC_CONSTANTNAME );
									if ( hNameProp ) {
										CAPointer<TCHAR> pName = ::GetPropValueAsString( hNameProp, NULL );
										pContList.AddTail( pName );
									}
								}
							}
						}
					}
				}
				hCurrConstData = ::DATA_Get_Next( hCurrConstData, NULL );
			}
		}
		hCurrIntData = ::DATA_Get_Next( hCurrIntData, NULL );
	}
}

//--- 
void	AddGlobalConst(AVP_dword dwType, LPCSTR szName, LPCSTR szValue )
{
	TypeConst *pTypeConst = NULL;
	for (int i=0, c=gTypeConst.Count(); i<c; i++ ){
		pTypeConst = gTypeConst[i];
	
		if(dwType != pTypeConst->m_dwPragueType)
		{
			pTypeConst = NULL;
			continue;
		}
		else
			break;
	}

	if(NULL == pTypeConst){
		pTypeConst = new TypeConst;
		pTypeConst->m_dwPragueType = dwType;
		gTypeConst.Add(pTypeConst);
	}

	ConstValue *pConst = new ConstValue;
	pConst->m_pConstName = (TCHAR*) szName;
	pConst->m_pValue	 = (TCHAR*) szValue;

	pTypeConst->m_Constants.Add(pConst);
}

//--- 
char *GetConstantNameOnValue(const char *pConstValue ) {
	AVP_dword nID = A2I( pConstValue );
	for ( int i=0,c=gTypeConst.Count(); i<c; i++ ) {
		for ( int j=0,n=gTypeConst[i]->m_Constants.Count(); j<n; j++ ) {
			AVP_dword nConstID = A2I( gTypeConst[i]->m_Constants[j]->m_pValue);

			if ( nID == nConstID ) 
				return ::StrDuplicate( gTypeConst[i]->m_Constants[j]->m_pConstName );
			
		}
	}
	return NULL;
}

//--- 
char *GetConstantValueAsString( HDATA hRootData, const char *pConstName ) {
	for ( int i=0,c=gTypeConst.Count(); i<c; i++ ) {
		for ( int j=0,n=gTypeConst[i]->m_Constants.Count(); j<n; j++ ) {
			if ( !lstrcmp(gTypeConst[i]->m_Constants[j]->m_pConstName, pConstName) ) {
				return ::StrDuplicate( gTypeConst[i]->m_Constants[j]->m_pValue );
			}
		}
	}

	MAKE_ADDR1( nConstAddr, VE_PID_IF_CONSTANTS );
	HDATA hCurrIntData = ::DATA_Get_First( hRootData, NULL );
	while ( hCurrIntData ) {
		HDATA hConstRootData = ::DATA_Find( hCurrIntData, nConstAddr );
		if ( hConstRootData ) {
			HDATA hCurrConstData = ::DATA_Get_First( hConstRootData, NULL );
			while ( hCurrConstData ) {
				HPROP hNameProp = ::DATA_Find_Prop( hCurrConstData, NULL, VE_PID_IFC_CONSTANTNAME );
				if ( hNameProp ) {
					CAPointer<TCHAR> pName = ::GetPropValueAsString( hNameProp, NULL );
					if ( !lstrcmp(pName, pConstName) ) {
						HPROP hValueProp = ::DATA_Find_Prop( hCurrConstData, NULL, VE_PID_IFC_VALUE );
						if ( hValueProp ) {
							return ::GetPropValueAsString( hValueProp, NULL );
						}
					}
				}
				hCurrConstData = ::DATA_Get_Next( hCurrConstData, NULL );
			}
		}
		hCurrIntData = ::DATA_Get_Next( hCurrIntData, NULL );
	}
	return NULL;
}

// ---
void SetItemEditType( HPROP hProp, CControlTreeItem *item ) {
	CAPointer <TCHAR> pValue;
	AVP_dword nType = GET_AVP_PID_TYPE(::PROP_Get_Id(hProp));
	switch ( nType ) {
		case avpt_nothing :
			item->SetSimpleStringAlias();
			break;
		case avpt_date :
			item->SetDateAlias();
			break;
		case avpt_time :
			item->SetTimeAlias();
			break;
		case avpt_datetime :
			item->SetDateTimeAlias();
			break;
		default :
			item->SetSimpleEditAlias();
			break;
	}
}


// ---
void SetNodeUID( HDATA hData, BOOL bDirectSet/* = FALSE*/ ) {
	if ( hData ) {
		HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_UNIQUEID );
		if ( !hProp || bDirectSet ) {
			GUID rcGuid;
			TCHAR sGuid[40];
			memset( &rcGuid, 0, sizeof(rcGuid) );
			if ( SUCCEEDED(::CoCreateGuid(&rcGuid)) && SUCCEEDED(::StringFromGUID(rcGuid, sGuid, sizeof(sGuid))) ) {
				if ( !hProp )
					::DATA_Add_Prop( hData, NULL, VE_PID_UNIQUEID, AVP_dword(sGuid), 0 );
				else
					::PROP_Set_Val( hProp, AVP_dword(sGuid), 0 );
			}
		}
	}
}

// ---
BOOL GetNodeUID( HDATA hData, CNodeUID &pvUID ) {
	if ( hData ) {
		HPROP hUIDProp = ::DATA_Find_Prop( hData, NULL, VE_PID_UNIQUEID );
		if ( hUIDProp ) {
			::PROP_Get_Val( hUIDProp, &pvUID, sizeof(CNodeUID) );
			return TRUE;
		}
	}
	return FALSE;
}

// ---
BOOL CompareNodesUID( CNodeUID &pvFUID, CNodeUID &pvSUID ) {
	GUID rcFGuid;
	GUID rcSGuid;
	if ( SUCCEEDED(::GUIDFromString(pvFUID, rcFGuid)) && SUCCEEDED(::GUIDFromString(pvSUID, rcSGuid)) )
		return ::IsEqualGUID( rcFGuid, rcSGuid );

	return FALSE;
}

// ---
BOOL SetDataPropWithCheck( HDATA hData, AVP_dword dwPID, AVP_dword dwValue ) {
	BOOL bIsSet = FALSE;
	HPROP hProp = ::DATA_Find_Prop( hData, NULL, dwPID );
	if ( !hProp ) {
		::DATA_Add_Prop( hData, NULL, dwPID, AVP_dword(dwValue), 0 );
		bIsSet = TRUE;
	}
	else {
		if ( dwValue != ::GetPropNumericValueAsDWord(hProp) ) {
			::PROP_Set_Val( hProp, AVP_dword(dwValue), 0 );
			bIsSet = TRUE;
		}
	}

	return bIsSet;
}

// ---
BOOL SetDataPropWithCheck( HDATA hData, AVP_dword dwPID, LPCTSTR pValue ) {
	BOOL bIsSet = FALSE;
	HPROP hProp = ::DATA_Find_Prop( hData, NULL, dwPID );
	if ( !hProp ) {
		::DATA_Add_Prop( hData, NULL, dwPID, AVP_dword(pValue), 0 );
		bIsSet = TRUE;
	}
	else {
		CAPointer<char> pOldValue = ::GetPropValueAsString( hProp, NULL );
		if ( lstrcmp(pValue, pOldValue) ) {
			::PROP_Set_Val( hProp, AVP_dword(pValue), 0 );
			bIsSet = TRUE;
		}
	}

	return bIsSet;
}

// ---
BOOL SetDataPropArrayWithCheck( HPROP hProp, int nPos, LPCTSTR pValue ) {
	BOOL bIsSet = FALSE;
  AVP_dword dwCount = ::PROP_Arr_Count( hProp );
  if ( nPos >= int(dwCount) )
    return FALSE;
	CAPointer<char> pOldValue = ::GetPropArrayValueAsString( hProp, nPos, NULL );
	if ( lstrcmp(pValue, pOldValue) ) {
		::SetPropArrayValue( hProp, nPos, pValue, NULL );
		bIsSet = TRUE;
	}

	return bIsSet;
}

// ---
ItType WType2IType( int iwType ) {
	switch ( iwType ) {
		case wmt_IDefinition :
			return itPrototype;
		case wmt_IImplementation :
			return itImplementation;
	}
	return itAny;
}

// ---
BOOL IsAlreadyAttached( HDATA hDataToCheck, HDATA hRootData ) {
	if ( hDataToCheck && hRootData ) {
		CNodeUID rcUIDToCheck;
		if ( ::GetNodeUID(hDataToCheck, rcUIDToCheck) ) {
			HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
			while ( hCurrData ) {
				CNodeUID rcCurrUID;
				if ( ::GetNodeUID(hCurrData, rcCurrUID) && ::CompareNodesUID(rcUIDToCheck, rcCurrUID) ) 
					return TRUE;
				hCurrData = ::DATA_Get_Next( hCurrData, NULL );
			}
		}
	}
	return FALSE;
}


// ---
HDATA FindFirstImportedData( HDATA hRootData ) {
	HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
	while ( hCurrData ) {
		if ( ::IsLinkedToFile(hCurrData) )
			return hCurrData;
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
	return NULL;
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::ExchangeIncludes( const char *pOldIncludeName, const char *pNewIncludeName ) 
{
	HDATA hCurrData = ::DATA_Get_First( m_hTree, NULL );
	while ( hCurrData ) 
	{
		BOOL bFound = false;
		HPROP hValueProp;
		if ( hValueProp = ::DATA_Find_Prop(hCurrData, NULL, VE_PID_IF_INCLUDENAMES) ) 
		{
			AVP_dword dwCount = ::PROP_Arr_Count( hValueProp );
			BOOL bFound = false;
			for ( AVP_dword i=0; i<dwCount && !bFound; i++ ) 
			{
				CAPointer<TCHAR> pValue = ::GetPropArrayValueAsString( hValueProp, i, NULL );
				if ( !lstrcmp(pValue, pOldIncludeName) ) 
				{
					::SetPropArrayValue( hValueProp, i, pNewIncludeName, NULL );
					bFound = true;
				}
			}
			if ( !bFound )
				::SetPropArrayValue( hValueProp, PROP_ARR_LAST, pNewIncludeName, NULL );
		}
		else 
		{
			hValueProp = ::DATA_Add_Prop( hCurrData, NULL, VE_PID_IF_INCLUDENAMES, 0, 0 );
			::SetPropArrayValue( hValueProp, PROP_ARR_LAST, pNewIncludeName, NULL );
		}

		if ( m_hCCTree == hCurrData )
			ReloadInterfaceCCTree( hCurrData );

		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::SetObjectIncludeName( HDATA hData, const char *pIncludeName, BOOL bUpdate) 
{
	CAPointer<char> pObjectName;
	HPROP hValueProp;
	if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_OBJECTTYPE ) ) 
		pObjectName = ::GetPropValueAsString( hValueProp, NULL );
	else 
		pObjectName  = ::CreateObjectTypeLabel( hData );

	CIObject *pObject = m_IOTable.Find( pObjectName );
	if ( pObject ) 
	{
		if ( bUpdate )
			ExchangeIncludes( pObject->m_rcIncludeName, pIncludeName );

		pObject->m_rcIncludeName = pIncludeName;
	}
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::ProcessObjectTable( HDATA hData, AVP_dword dwRootPID, AVP_dword dwNamePID ) 
{
	MAKE_ADDR1(nAddr1, dwRootPID);
	HDATA hRootData = ::DATA_Find( hData, nAddr1 ); 
	if ( hRootData ) 
	{
		HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
		while ( hCurrData ) 
		{
			HPROP hProp = ::DATA_Find_Prop( hCurrData, NULL, dwNamePID );
			CAPointer<char> pValue;
			if ( hProp ) 
			{
				pValue = ::GetPropValueAsString( hProp, NULL );
				CIObject *pObject = m_IOTable.Add( pValue );
				if ( pObject ) {
					AVP_dword dwID = ::DATA_Get_Id( hCurrData, NULL );
					pObject->m_dwObjectID = dwID;
					if ( hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_INCLUDEFILE ) ) 
					{
						pValue = ::GetPropValueAsString( hProp, NULL );
						pObject->m_rcIncludeName = pValue;
					}
					::GetNodeUID( hData, pObject->m_UID );
				}
			}

			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
	}
}

// ------------------------------------------------------------------------------------------
static AVP_dword gTypePIDTable[] = {
	VE_PID_IFP_TYPENAME,
	VE_PID_IFM_RESULTTYPENAME,
	VE_PID_IFMP_TYPENAME,
	VE_PID_IFT_BASETYPENAME,
	VE_PID_IFC_BASETYPENAME,
	VE_PID_IFC_CONSTANTNAME,
};

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::LoadObjectsTable( HDATA hRootData ) 
{
	HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
	while ( hCurrData ) 
	{
		int gTypePIDTableCount = sizeof(gTypePIDTable) / sizeof(gTypePIDTable[0]);
		for ( int i=0; i<gTypePIDTableCount; i++ ) 
		{
			HPROP hProp = ::DATA_Find_Prop( hCurrData, NULL, gTypePIDTable[i] );
			if ( hProp ) 
			{
				CAPointer<char> pValue = ::GetPropValueAsString( hProp, NULL );
				SetObjectIsUsed( pValue );
			}
		}

		LoadObjectsTable( hCurrData );
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
}

// ---
void CVisualEditorDlg::PreparePredefinedPropComments( PredefinedProp *pProp, HDATA hData ) 
{
	HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_SHORTCOMMENT );
	if ( !hProp )
		::DATA_Add_Prop( hData, NULL, VE_PID_SHORTCOMMENT, AVP_dword(LPCTSTR(pProp->m_pShortComment)), 0 );
	else 
	{
		CAPointer<TCHAR> pValue = ::GetPropValueAsString( hProp, NULL );
		if ( !pValue || !*pValue )
			::PROP_Set_Val( hProp, AVP_dword(LPCTSTR(pProp->m_pShortComment)), 0 );
	}
	
	hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_LARGECOMMENT );
	if ( !hProp )
		::DATA_Add_Prop( hData, NULL, VE_PID_LARGECOMMENT, AVP_dword(LPCTSTR(pProp->m_pLargeComment)), 0 );
	else 
	{
		CAPointer<TCHAR> pValue = ::GetPropValueAsString( hProp, NULL );
		if ( !pValue || !*pValue )
			::PROP_Set_Val( hProp, AVP_dword(LPCTSTR(pProp->m_pLargeComment)), 0 );
	}
	
	hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_BEHAVIORCOMMENT );
	if ( !hProp )
		::DATA_Add_Prop( hData, NULL, VE_PID_BEHAVIORCOMMENT, AVP_dword(LPCTSTR(pProp->m_pBehaviorComment)), 0 );
	else 
	{
		CAPointer<TCHAR> pValue = ::GetPropValueAsString( hProp, NULL );
		if ( !pValue || !*pValue )
			::PROP_Set_Val( hProp, AVP_dword(LPCTSTR(pProp->m_pBehaviorComment)), 0 );
	}
}


// ---
void CVisualEditorDlg::PreparePredefinedProp( CPArray<PredefinedProp> &rcProps, const TCHAR *pName, HDATA hData ) 
{
	for ( int i=0,c=rcProps.Count(); i<c; i++ ) 
	{
		PredefinedProp *pProp = rcProps[i];

		if ( !lstrcmp(pProp->m_pName, pName) ) 
		{
			AVP_dword dwScope = pProp->m_bShared ? VE_IFP_SHARED : VE_IFP_LOCAL;
			HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFP_SCOPE );
			if ( !hProp )
				::DATA_Add_Prop( hData, NULL, VE_PID_IFP_SCOPE, dwScope, 0 );
			else
				::PROP_Set_Val( hProp, dwScope, 0 );

			hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFPSS_VARIABLEDEFVALUE );
			if ( !hProp )
				::DATA_Add_Prop( hData, NULL, VE_PID_IFPSS_VARIABLEDEFVALUE, AVP_dword(LPCTSTR(pProp->m_pDefValue)), 0 );
			else
				::PROP_Set_Val( hProp, AVP_dword(LPCTSTR(pProp->m_pDefValue)), 0 );

			SetPropTypifiedDefValue( hData );

			PreparePredefinedPropComments( pProp, hData );

			break;
		}
	}
}

// ------------------------------------------------------------------------------------------
