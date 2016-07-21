//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifndef _PVE_DT_UTILS
#define _PVE_DT_UTILS

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

HDATA	GetPluginStaticInterfaceRoot(HDATA i_hDataPlugin, BOOL& o_bBeenAdded);
BOOL	IsPluginStaticInterfaceRoot(HDATA i_hDataInterface);

BOOL	IsCommentPropId(AVP_dword i_dwPropId);

BOOL	IsNativeData(HDATA i_hData);
BOOL	IsNativeNode(HDATA i_hData);
void	CheckNativeProp(HDATA i_hData, void* i_pData);

BOOL	IsBoolPropSet(HDATA i_hData, AVP_dword i_dwPropId);
BOOL	CheckPropSet(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal);
BOOL	SetDwordProp(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal);
AVP_dword GetDwordProp(HDATA i_hData, AVP_dword i_dwPropId);

struct CNodeUID;

HPROP	FindPropByID( HDATA hData, AVP_word dwID );

HDATA FindEntityByUID( HDATA hRootData, CNodeUID &pFindUID, BOOL i_bShouldBeNative ); 
HDATA FindDataByUID( HDATA hRootData, CNodeUID &pFindUID ); 

typedef void (*pfnForEachRoutine)(HDATA,void*);
void ForEach(HDATA i_hRoot, pfnForEachRoutine i_pfnForEachRoutine, void* pRotineData);

typedef void (*pfnForEachSyncRoutine)(HDATA,HDATA,void*);
void ForEachSync(HDATA i_hRoot1, HDATA i_hRoot2, 
	pfnForEachSyncRoutine i_pfnForEachSyncRoutine, void* pSyncRotineData);

void DataAttachWithNextDataId(HDATA i_hParent, HDATA i_hAttached, AVP_dword i_uiFlags=DATA_IF_ROOT_INCLUDE);

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#endif //_PVE_DT_UTILS