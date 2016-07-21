////////////////////////////////////////////////////////////////////
//
//  DtPtrnME.c
//  Merge and extract data tree and pattern
//  AVP generic purpose stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <string.h>

#define __MEGRE_FUNC__
#include "datatreeutils/dtutils.h"

#include "avpprpid.h"
#include "AVPPort.h"

extern void* (* DTUAllocator)(AVP_uint);
extern void  (* DTULiberator)(void*);

DTUT_PROC AVP_bool DTUT_PARAM DTUT_ExchangePropValueEx( HPROP hFromProp, HPROP hToProp );
	

// ---
static HDATA SearchForNodeById( HDATA hData, AVP_dword nFindID ) {
	HDATA hCurrData;
	for ( hCurrData=DATA_Get_First(hData, NULL); hCurrData; hCurrData = DATA_Get_Next(hCurrData, NULL) ) {
		AVP_dword nPID = DATA_Get_Id(hCurrData, 0);
		AVP_dword nID = GET_AVP_PID_ID( nPID );
		if ( nID == nFindID )
			return hCurrData;
	}
	return NULL;
}

// ---
static int CheckSubTreedArray( HDATA hNewData, HDATA hCurrData ) {
	// Обработка массивов с подветвями
	AVP_dword nPID = DATA_Get_Id( hNewData, 0 );
	if ( GET_AVP_PID_ARR(nPID) ) {
		AVP_dword nCount = PROP_Arr_Count( DATA_Find_Prop(hNewData, 0, 0) );
		if ( nCount ) {
			HDATA hPatternTree = 0;
			// Искать дерево-шаблон для NewValue
			int i;
			for (	i=nCount; i>=0; i-- ) {
				hPatternTree = SearchForNodeById( hCurrData, i );
				if ( hPatternTree ) {
					AVP_byte  nTEDType = PP_TED_NONEDITABLE;
					if ( DATA_Find_Prop( hPatternTree, NULL, PP_PID_EDIT_TYPE ) ) 
						DATA_Get_Val( hPatternTree, NULL, PP_PID_EDIT_TYPE, &nTEDType, sizeof(nTEDType) );

					if ( nTEDType == PP_TED_NOTVISIBLE ) 
						break;
				}
			}

			if ( hPatternTree ) {
				// Если шаблон найден и он невидимый - это массив с поддеревьями
				if ( i != (int)nCount ) {
					// У самой ветви установить идентификатор = nCount - это будет ветвь для NewValue
					DATA_Replace_ID( hPatternTree, 0, (AVP_word)nCount );
				}
				for ( i=0; i<(int)nCount; i++ ) {
					// Искать дерево элемента
					HDATA hItemTree = SearchForNodeById( hCurrData, i );
					if ( hItemTree ) {
						// Если найдено - проверить правильность
						AVP_byte  nTEDType = PP_TED_NONEDITABLE;
						if ( DATA_Find_Prop( hItemTree, NULL, PP_PID_EDIT_TYPE ) ) 
							DATA_Get_Val( hItemTree, NULL, PP_PID_EDIT_TYPE, &nTEDType, sizeof(nTEDType) );

						if ( nTEDType != PP_TED_NOTVISIBLE ) 
							hItemTree = 0;
					}

					if ( !hItemTree ) {
						// Если не найден ...
						// Дублировать поддерево-шаблон и добавлять к шаблону
						// Идентификатор заменять на индекс
						HDATA hData = DATA_Copy( 0, 0, hPatternTree, DATA_IF_ROOT_INCLUDE );
						DATA_Replace_ID( hData, 0, (AVP_word)i );
						DATA_Attach( hCurrData, 0, hData, 0 );
					}
				}
			}
		}
		return 1;
	}
	return 0;
}


// ---
static void RemoveUnnecessaryTrees( HDATA hBaseData, HDATA hMergedData ) {
	// Выровнять количество подветвей в шаблоне по оригиналу
	HDATA hCurrMergedData = DATA_Get_First( hMergedData, NULL );
	HDATA hCurrBaseData = DATA_Get_First( hBaseData, NULL );
	if ( hCurrMergedData ) {
		// Если в оригинале они вообще есть...
		while ( hCurrBaseData && hCurrMergedData ) {
			// Сканировать шаблон и приравнивать идентификаторы узлов
			AVP_dword nPID = DATA_Get_Id(hCurrMergedData, 0);
			AVP_dword nID = GET_AVP_PID_ID( nPID );
			DATA_Replace_ID( hCurrBaseData, 0, (AVP_word)nID );
			hCurrBaseData = DATA_Get_Next( hCurrBaseData, NULL );
			hCurrMergedData = DATA_Get_Next( hCurrMergedData, NULL );
		}
		// Остальные узлы в шаблоне уничтожить
		while ( hCurrBaseData ) {
			HDATA hData = DATA_Get_Next( hCurrBaseData, NULL );
			DATA_Remove( hCurrBaseData, 0 );
			hCurrBaseData = hData;
		}
	}
	else {
		if ( hCurrBaseData ) {
			// Если нет - оставить один 
			DATA_Replace_ID( hCurrBaseData, 0, (AVP_word)0 );
			while ( hCurrBaseData ) {
				HDATA hData = DATA_Get_Next( hCurrBaseData, NULL );
				DATA_Remove( hCurrBaseData, 0 );
				hCurrBaseData = hData;
			}
		}
	}
}


// ---
static void ScanTreeForSubTreedArrays( HDATA hData ) {
	hData = DATA_Get_First( hData, 0 );
	while ( hData ) {
		// Обработка массивов с подветвями
		CheckSubTreedArray( hData, hData );
		ScanTreeForSubTreedArrays( hData );
		hData = DATA_Get_Next( hData, 0 );
	}
}


// ---
static AVP_dword *FindLastAddressID( AVP_dword *pdwSeq ) {
	if ( pdwSeq && *pdwSeq ) {
		for ( ; *pdwSeq; pdwSeq++ ) {
			if ( *(pdwSeq + 1) == 0 )
				return pdwSeq;
		}
	}
	return NULL;
}

// ---
static AVP_dword PrepareExchangePID( AVP_dword dwPID ) {
	AVP_TYPE dwType = avpt_nothing;
	switch( GET_AVP_PID_TYPE(dwPID) ) {
		case avpt_str :
			dwType = avpt_wstr;
			break;
		case avpt_wstr :
			dwType = avpt_str;
			break;
		case avpt_char :
			dwType = avpt_wchar;
			break;
		case avpt_wchar :
			dwType = avpt_char;
			break;
	}
	return MAKE_AVP_PID(GET_AVP_PID_ID(dwPID), GET_AVP_PID_APP(dwPID), dwType, GET_AVP_PID_ARR(dwPID) );
}


// ---
static HDATA FindDataToMerge( HDATA hMergedData, AVP_dword *pSeq ) {
	HDATA hFindData = DATA_Find( hMergedData, pSeq );
	if ( !hFindData ) {
		AVP_dword *pdwLast = FindLastAddressID( pSeq );
		if ( pdwLast ) {
			*pdwLast = PrepareExchangePID( *pdwLast );
			hFindData = DATA_Find( hMergedData, pSeq );
		}
	}
	return hFindData;
}


// ---
static HPROP FindPropToMerge( HDATA hMergedData, AVP_dword nPID ) {
	HPROP hFindProp = NULL;
	if ( !(hFindProp = DATA_Find_Prop(hMergedData, 0, nPID)) ){
		nPID = PrepareExchangePID( nPID );
		hFindProp = DATA_Find_Prop( hMergedData, 0, nPID );
	}
	return hFindProp;
}


// ---
static void MergeNodeContents( HDATA hCurrData, HDATA hMergedData, HDATA *hResultData ) {
//  HDATA hNewData = DATA_Copy( hCurrData, 0, *hResultData, DATA_IF_ROOT_INCLUDE | DATA_IF_DOESNOT_GO_DOWN );
	// For working around error
  HDATA hNewData = DATA_Copy( *hResultData, 0, hCurrData, DATA_IF_ROOT_INCLUDE | DATA_IF_DOESNOT_GO_DOWN );
	AVP_dword *pSeq = 0;
	HDATA hFindData;

	*hResultData = hNewData;

  DATA_Make_Sequence( hNewData, 0, 0, &pSeq );
	hFindData = FindDataToMerge( hMergedData, pSeq );

	if ( hFindData ) {
		HPROP hCurrProp;
		HPROP hNewHeadProp = DATA_Find_Prop(hNewData, 0, 0);
		HPROP hFindHeadProp = DATA_Find_Prop(hFindData, 0, 0);

		DTUT_ExchangePropValueEx( hFindHeadProp, hNewHeadProp );

		hCurrProp = DATA_Get_First_Prop( hFindData, NULL );
		while ( hCurrProp ) {
			if ( hFindHeadProp != hCurrProp ) {
				AVP_dword nPID = PROP_Get_Id( hCurrProp );
				HPROP hProp = FindPropToMerge( hNewData, nPID );
				if ( !hProp )	
					hProp = DATA_Add_Prop( hNewData, 0, nPID, 0, 0 );
				if ( hProp )
					DTUT_ExchangePropValueEx( hCurrProp, hProp );
			}
			hCurrProp = PROP_Get_Next( hCurrProp );
		}
	}

	// Обработка массивов с подветвями
	// Создать необходимое количество ветвей
	if ( CheckSubTreedArray(hNewData, hCurrData) && hFindData )
		// Убрать ненужные ветви
		RemoveUnnecessaryTrees( hCurrData, hFindData );

	DATA_Remove_Sequence( pSeq );
}

// ---
static void MergeSubTreeNodes( HDATA hCurrData, HDATA hMergedData, HDATA hResult ) {
	hCurrData = DATA_Get_First( hCurrData, 0 );

	while ( hCurrData ) {
		HDATA hNewData = hResult;
		MergeNodeContents( hCurrData, hMergedData, &hNewData );
		MergeSubTreeNodes( hCurrData, hMergedData, hNewData );
		hCurrData = DATA_Get_Next( hCurrData, 0 );
	}
}


// ---
static DATA_PROC AVP_bool DATA_PARAM  DATA_Merge( HDATA hInData1, AVP_dword* pAddr, HDATA hInData2, AVP_dword* pAddr2, HDATA* phResultData, AVP_dword flags, AVP_Merge_Param* param ) {
	// Native Property.lib method
//  *phResultData = DATA_Copy( hInData1, 0, 0, DATA_IF_ROOT_INCLUDE | DATA_IF_DOESNOT_GO_DOWN );

//  *phResultData = DATA_Copy( 0, 0, hInData1, DATA_IF_ROOT_INCLUDE | DATA_IF_DOESNOT_GO_DOWN );
//	MergeSubTreeNodes( hInData2, hInData1, *phResultData );

	// Это сделано для того, чтобы выполнить обмен PROPs прямо на корневом узле
	HDATA hNewData = 0;
	MergeNodeContents( hInData1, hInData2, &hNewData );
	*phResultData = hNewData;
	MergeSubTreeNodes( hInData2, hInData1, *phResultData );

	return 1;
}


// ---
// Merge data tree and pattern
// hTreeData    - HDATA of tree to be merged
// hPatternData - HDATA of pattern tree to use for werging
// phResultData - pointer to result HDATA
// Return	value - 1 on success, 0 on error occurs
DTUT_PROC AVP_bool  DTUT_PARAM DTUT_MergeDataTreeAndPattern( HDATA hTreeData, HDATA hPatternData, HDATA* phResultData ) {
	if ( phResultData )
		*phResultData = 0;

	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, "Library is not initialised" );
		return 0;
	}

	if ( hPatternData ) {
		if ( hTreeData ) {
			// Если есть дерево значений, то объединяем его с шаблоном
			if ( DATA_Merge(hTreeData, 0, hPatternData, 0, phResultData, DATA_IF_ROOT_EXCLUDE, 0) ) 
				return 1;
		}
		else {
			// Если нет дерева значений, то создаем копию шаблона
			AVP_dword nSSize;
			char *pBuffer;
			AVP_Serialize_Data rcSData;

			memset( &rcSData, 0, sizeof(rcSData) );

			DATA_Serialize( (HDATA)hPatternData, 0, &rcSData, 0, 0, &nSSize );

			pBuffer =  DTUAllocator(sizeof(BYTE) * nSSize);

			DATA_Serialize( hPatternData, 0, &rcSData, pBuffer, nSSize, &nSSize );

			*phResultData = DATA_Deserialize( &rcSData, pBuffer, nSSize, &nSSize );

			DTUT_ExchangePropValue( DATA_Find_Prop(hPatternData, 0, 0), DATA_Find_Prop(*phResultData, 0, 0) );

			ScanTreeForSubTreedArrays( *phResultData );

			DTULiberator( pBuffer );

			return !!*phResultData;
		}
	}
	return 0;
}

// ---
static HDATA CopyDataTreeValuesOnly( HDATA hInData, AVP_dword *aPropInclude ) {
	AVP_dword nSSize;
	char *pBuffer;
	HDATA hOutData;

	AVP_Serialize_Data rcSData;

	memset( &rcSData, 0, sizeof(rcSData) );
	rcSData.props_include = aPropInclude;

	DATA_Serialize( hInData, 0, &rcSData, 0, 0, &nSSize );

	pBuffer =  DTUAllocator(sizeof(BYTE) * nSSize);

	DATA_Serialize( hInData, 0, &rcSData, pBuffer, nSSize, &nSSize );

	hOutData = DATA_Deserialize( &rcSData, pBuffer, nSSize, &nSSize );

	DTULiberator( pBuffer );

	return hOutData;
}

// Extract only datas needed for working from data tree
// hTreeData    - HDATA of source tree
// phResultData - pointer to result HDATA
// pPropInclude	- pointer to property IDs array to be included to result tree
// Return	value - 1 on success, 0 on error occurs
DTUT_PROC AVP_bool  DTUT_PARAM DTUT_ExtractValuesDataTree( HDATA hTreeData, HDATA *phResultData, AVP_dword * pPropInclude ) {
	if ( phResultData )
		*phResultData = 0;

	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, "Library is not initialised" );
		return 0;
	}

	if ( hTreeData ) {
		AVP_dword *aPropInclude;
		if ( pPropInclude ) {
			// Если массив дали - посчитать количество элементов
			long *pPointer = pPropInclude; 
			int nCount = 0;
			for ( ; *pPointer; pPointer++,nCount++ );
			// Создать новый массив	+ AVP_PID_VALUE + PP_PID_VALUEASINDEX + PP_PID_UNIQUEID
			aPropInclude = DTUAllocator( sizeof(AVP_dword) *(nCount + 3 + 1) );
			memcpy( aPropInclude, pPropInclude, nCount * sizeof(AVP_dword) );
			aPropInclude[nCount + 0] = AVP_PID_VALUE;
			aPropInclude[nCount + 1] = PP_PID_VALUEASINDEX;
			aPropInclude[nCount + 2] = PP_PID_UNIQUEID;
			aPropInclude[nCount + 3] = 0;
		}
		else {
			aPropInclude = DTUAllocator( sizeof(AVP_dword) *(3 + 1) );
			aPropInclude[0] = AVP_PID_VALUE;
			aPropInclude[1] = PP_PID_VALUEASINDEX;
			aPropInclude[2] = PP_PID_UNIQUEID;
			aPropInclude[3] = 0;
		}

		*phResultData = CopyDataTreeValuesOnly( hTreeData, aPropInclude );

		DTULiberator( aPropInclude );

		return 1;
	}

	return 0;
}
