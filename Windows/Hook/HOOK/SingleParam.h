// сколько нужно выделить для FilterEvent. 
// _Count_ - число параметров
// _ExtraBuffersSize_ - размер буферов, в которых передаются данные.
#define PARAM_SIZE(_Count_, _ExtraBuffersSize_ ) \
sizeof(FILTER_EVENT_PARAM) + __SID_LENGTH + \
(_Count_) * ( sizeof (SINGLE_PARAM)	+ sizeof ( ULONG ) ) + _ExtraBuffersSize_

#define INITIALIZE_PARAM(_pParam, _EventType, _AppName, _ModuleID)  { \
pParam->HookID		= _ModuleID;				\
pParam->FunctionMj	= _EventType;				\
pParam->FunctionMi	= 0;						\
pParam->ProcessingType = PreProcessing;			\
pParam->ParamsCount = 0;						\
strcpy (pParam->ProcName, _AppName);			\
pParam->ProcessID	= 0;						\
pParam->ThreadID	= 1;						\
pParam->UnsafeValue = 0;						\
												\
pSingleParam = (PSINGLE_PARAM)pParam->Params;	\
pSingleParam->ParamSize = __SID_LENGTH;			\
pSingleParam->ParamValue[0] = 0;				\
pSingleParam->ParamID = _PARAM_OBJECT_SID;		\
pParam->ParamsCount++;							\
pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;					\
}

#define ADD_ULONG_PARAM(pSingleParam, pParam, _ParamID, _Value)		{				\
PVOID	pPointer;																	\
pPointer	 = (BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize;\
pSingleParam = (PSINGLE_PARAM)pPointer;												\
pSingleParam->ParamSize = sizeof(ULONG);											\
*(ULONG*)pSingleParam->ParamValue = _Value;											\
pSingleParam->ParamID = _ParamID;													\
pParam->ParamsCount++;																\
pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;					\
}

#define ADD_USHORT_PARAM(pSingleParam, pParam, _ParamID, _Value)		{				\
	PVOID	pPointer;																	\
	pPointer	 = (BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize;\
	pSingleParam = (PSINGLE_PARAM)pPointer;												\
	pSingleParam->ParamSize = sizeof(USHORT);											\
	*(USHORT*)pSingleParam->ParamValue = _Value;										\
	pSingleParam->ParamID = _ParamID;													\
	pParam->ParamsCount++;																\
	pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;					\
}

#define ADD_UCHAR_PARAM(pSingleParam, pParam, _ParamID, _Value)		{				\
	PVOID	pPointer;																	\
	pPointer	 = (BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize;\
	pSingleParam = (PSINGLE_PARAM)pPointer;												\
	pSingleParam->ParamSize = sizeof(UCHAR);											\
	*(UCHAR*)pSingleParam->ParamValue = _Value;										\
	pSingleParam->ParamID = _ParamID;													\
	pParam->ParamsCount++;																\
	pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;					\
}

#define COPY_BINARY_PARAM(pSingleParam, pParam, _ParamID, _pData, _DataSize)	{			\
	PVOID	pPointer;																	\
	pPointer	 = (BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize;\
	pSingleParam = (PSINGLE_PARAM)pPointer;												\
	pSingleParam->ParamSize = _DataSize;												\
	RtlCopyMemory (pSingleParam->ParamValue, _pData, _DataSize);										\
	pSingleParam->ParamID = _ParamID;													\
	pParam->ParamsCount++;																\
	pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;					\
}

#define ADD_BINARY_PARAM(pSingleParam, pParam, _ParamID, _pData, _DataSize)	{			\
	PVOID	pPointer;																	\
	pPointer	 = (BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize;\
	pSingleParam = (PSINGLE_PARAM)pPointer;												\
	pSingleParam->ParamSize = _DataSize;												\
	*(PVOID*)pSingleParam->ParamValue = _pData;										\
	pSingleParam->ParamID = _ParamID;													\
	pParam->ParamsCount++;																\
	pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_POINTER;					\
}

#define ADD_BINARY_FORMAT(pSingleParam, pParam, _ParamID, _DataSize)	{		\
	PVOID	pPointer;																	\
	pPointer	 = (BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize;\
	pSingleParam = (PSINGLE_PARAM)pPointer;												\
	pSingleParam->ParamSize = _DataSize;												\
	pSingleParam->ParamID = _ParamID;													\
	pParam->ParamsCount++;																\
	pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;					\
}

//---------------------------------------------------------------------------------------------
// changed for NET-KAH team
#define START_TDI_FILTERS(_Event_, _FilterType_, _PID_, _ProcessName_) 						\
ULONG ReqDataSize = sizeof(FILTER_EVENT_PARAM);									\
PSINGLE_PARAM pSingleParam;														\
PFILTER_EVENT_PARAM pParam;														\
BYTE *RequestData;																\
ULONG	BytesRet = 0;															\
ReqDataSize += 9*sizeof (SINGLE_PARAM) + sizeof(UCHAR) + sizeof(ULONG) + sizeof(USHORT) + \
sizeof(ULONG) + sizeof(USHORT) + sizeof(ULONG) + HASH_SIZE + USERAPPNAMELEN + USERAPPNAMELEN + __SID_LENGTH;	\
RequestData = (BYTE *)ExAllocatePoolWithTag(NonPagedPool, ReqDataSize, '*NeG');			\
pParam = (PFILTER_EVENT_PARAM)RequestData;										\
if ( pParam == NULL )															\
{																				\
	Verdict = Verdict_Discard;													\
}																				\
else																			\
{																				\
    RtlZeroMemory (pParam, ReqDataSize);                                        \
	INITIALIZE_PARAM(pParam, (_Event_), (_ProcessName_), (_FilterType_) );	\
	pParam->ProcessID = (_PID_);												\
	ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );
	

		
	
#define END_TDI_FILTERS			\
	ExFreePool(RequestData);	\
}


#define START_NDIS_FILTERS(_Event_, _FilterType_, _ProcessName_)									\
	ULONG ReqDataSize;																\
	PSINGLE_PARAM pSingleParam;														\
	PFILTER_EVENT_PARAM pParam;														\
	BYTE *RequestData;																\
	ULONG	BytesRet = 0;															\
	ReqDataSize = sizeof(FILTER_EVENT_PARAM);										\
	ReqDataSize += 9 * sizeof (SINGLE_PARAM) +  \
        sizeof(UCHAR) + \
        sizeof(UCHAR) + \
        sizeof(ULONG) + \
        sizeof(USHORT) + \
	    sizeof(ULONG) + \
        sizeof(USHORT) + \
        sizeof ( ULONG ) + __SID_LENGTH + \
        9 * sizeof ( SINGLE_PARAM ) + 9 * sizeof ( ULONG ); /* на самом деле не ULONG, но так меньше писать.*/ \
	RequestData = (BYTE *)ExAllocatePoolWithTag(NonPagedPool, ReqDataSize, '*NeG');			\
	pParam = (PFILTER_EVENT_PARAM)RequestData;										\
if ( pParam == NULL )																\
{																					\
	Verdict = Verdict_Discard;														\
}																					\
else																				\
{																					\
	INITIALIZE_PARAM(pParam, (_Event_), (_ProcessName_),(_FilterType_) );		\
	pParam->ProcessingType = PreProcessing;                                     \
    pParam->UnsafeValue     = 1;


#define END_NDIS_FILTERS		\
	ExFreePool(RequestData);	\
}