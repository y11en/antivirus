#ifndef _NET_PARSER_
#define _NET_PARSER_

#include "StdAfx.h"
#include "FilterEvent.h"

#define MAX_ENCAPSULATE_LEVEL	8
#define MAX_RECOGNIZE_STEPS		128

#define AUTO					0x00000000L

typedef enum tagPARSE_REPLY{
		error	= 0,
		ok		= 1,
		maybe	= 2,
		wrong	= 3
} PARSE_REPLY;

#pragma pack( push, 1)


typedef struct tagChunkInfo{
	PCHAR	pData;	/*IN - pointer to packet data*/
	ULONG	dwSize;	/*IN - actual chunk size*/
} ChunkInfo, *LPChunkInfo;


typedef struct tagParseShuttle{
	ChunkInfo			Frame;			// IN/OUT
	ChunkInfo			Buff;			// IN/OUT
	ULONG				dwError;		// OUT Error code: 0 - success
	ULONG				dwKey;			/* OUT - return encapsulator key for the next level*/
	ULONG				dwParamsCount;	/* OUT - Number of filled params*/
	LPNetInfoHeader		pIH;
	PFILTER_EVENT_PARAM pEventTransmit;
} ParseShuttle, *LPParseShuttle;


//return error code
typedef PARSE_REPLY (*ParseFunc)(LPParseShuttle ps);

typedef struct tagParseUnit {
	ULONG					dwKEY;
	ParseFunc				fParse;
	struct tagParseUnit*	pNextEncapsulate;
} ParseUnit, *LPParseUnit;


typedef struct tagTraversalState {
	ChunkInfo		Frame;
	ChunkInfo		Buff;
	LPParseUnit		Table;
	ULONG			dwIndex;
	ULONG			dwLookUpKey;
	PARSE_REPLY		LastRet; //error - state not valid
	ULONG			dwLastParamsCount;
}TraversalState, *LPTraversalState;

#pragma pack( pop )

#define MJ_ID(Type, kind)	( \
	(Type << 24) | (kind<<16) )


#define DEF(name)		PARSE_REPLY prot##name(LPParseShuttle ps);

#define BEGIN_PROT_PARSE(name, type)	PARSE_REPLY prot##name(LPParseShuttle ps) { \
	PSINGLE_PARAM pSingleParam	= NULL;												\
	type* pField				= (type*)ps->Frame.pData;							\
	ULONG count					= 0;												\
	PARSE_REPLY status			= maybe;											\
	Message(DL_INFO, DM_UNWIND, ("TRUE: Entering in %s...\n", #name));


#define ADD_VALUE(id, val)																		\
	if( ps->Buff.dwSize >  4 + sizeof(SINGLE_PARAM) )											\
	{																							\
		pSingleParam = (PSINGLE_PARAM)ps->Buff.pData;											\
		pSingleParam->ParamSize = 4;															\
		pSingleParam->ParamID	= id;															\
		*((unsigned long*)pSingleParam->ParamValue) = val;										\
		ps->Buff.pData  += ( sizeof(SINGLE_PARAM) +4 );											\
		ps->Buff.dwSize -= ( sizeof(SINGLE_PARAM) +4 );											\
		ps->dwParamsCount++;																	\
		Message(DL_INFO, DM_ADD_PARAM, ("TRUE: Add param:%#x, ID:%#.8x, size: 4\n",val, id));	\
	}																							\
	else																						\
	{																							\
		Message(DL_ERROR, DM_ADD_PARAM, ("TRUE: ERROR. Unsufficient buffer space\n"));			\
		return error;																			\
	}

#pragma warning( disable: 4244 ) 
#pragma warning( disable: 4242 )       
     
#ifdef __LITTLE_ENDIAN_BITFIELD

#define ADD_PARAM(id, field)	\
	if(ps->Buff.dwSize >  sizeof(pField->##field) + sizeof(SINGLE_PARAM)){ \
		pSingleParam = (PSINGLE_PARAM)ps->Buff.pData; \
		pSingleParam->ParamSize = sizeof(pField->##field); \
		pSingleParam->ParamID = id; \
		switch(sizeof(pField->##field)){ \
		case 1: *((unsigned char*)pSingleParam->ParamValue) = pField->##field; break; \
		case 2:	*((unsigned short*)pSingleParam->ParamValue) = (USHORT)((pField->##field)<<8)|((pField->##field)>>8); break; \
		case 4:	*((unsigned long*)pSingleParam->ParamValue) = (ULONG)((pField->##field)<<24)|(((pField->##field)<<8)&0x00FF0000)|(((pField->##field)>>8)&0x0000FF00)|(((pField->##field)>>24)&0x000000FF); break; \
/*		case 8:	*((unsigned __int64*)pSingleParam->ParamValue) = pField->##field; break; */\
		default:  \
			Message(DL_ERROR, DM_UNWIND, ("TRUE: ERROR. Cannot add param ID:%#.8x, size = %d\n", id, pSingleParam->ParamSize)); \
			return error; \
		} \
		ps->Buff.pData+=sizeof(SINGLE_PARAM)+pSingleParam->ParamSize; \
		ps->Buff.dwSize-=sizeof(SINGLE_PARAM)+pSingleParam->ParamSize; \
		ps->dwParamsCount++; \
		Message(DL_INFO, DM_ADD_PARAM, ("TRUE: Add param %#x, ID:%#.8x, size: %d\n",pField->##field, id, pSingleParam->ParamSize)); \
	}else{ \
		Message(DL_ERROR, DM_ADD_PARAM, ("TRUE: ERROR. Unsufficient buffer space\n")); \
		return error; \
	}

#define ADD_MKEY(field)			\
	switch(sizeof(pField->##field)){ \
		case 1: ps->dwKey = pField->##field; break; \
		case 2:	ps->dwKey = (USHORT)((pField->##field)<<8)|((pField->##field)>>8); break; \
		case 4:	ps->dwKey = (ULONG)((pField->##field)<<24)|(((pField->##field)<<8)&0x00FF0000)|(((pField->##field)>>8)&0x0000FF00)|(((pField->##field)>>24)&0x000000FF); break; \
/*		case 8:	*((unsigned __int64*)pSingleParam->ParamValue) = pField->##field; break; */\
		default:  \
			Message(DL_ERROR, DM_UNWIND, ("TRUE: Error. Cannot add MKEY(size = %d)\n", sizeof(pField->##field))); \
			return error; \
		} \
	Message(DL_FULL_INFO, DM_UNWIND, ("TRUE: Add key = %#x\n", ps->dwKey)); 


#define FIELD1(field)	pField->##field
#define FIELD2(field)	(USHORT)(((pField->##field)<<8)|((pField->##field)>>8))
#define FIELD4(field)	(ULONG)(((pField->##field)<<24)|(((pField->##field)<<8)&0x00FF0000)|(((pField->##field)>>8)&0x0000FF00)|(((pField->##field)>>24)&0x000000FF))


#elif defined(__BIG_ENDIAN_BITFIELD)

#define ADD_PARAM(id, field)	\
	if(ps->Buff.dwSize >  sizeof(pField->##field) + sizeof(SINGLE_PARAM)){ \
		pSingleParam = (PSINGLE_PARAM)ps->Buff.pData; \
		pSingleParam->ParamSize = sizeof(pField->##field); \
		pSingleParam->ParamID = id; \
		switch(sizeof(pField->##field)){ \
		case 1: *((unsigned char*)pSingleParam->ParamValue) = pField->##field; break; \
		case 2:	*((unsigned short*)pSingleParam->ParamValue) = pField->##field; break;\
		case 4:	*((unsigned long*)pSingleParam->ParamValue) = pField->##field; break;\
/*		case 8:	*((unsigned __int64*)pSingleParam->ParamValue) = pField->##field; break; */\
		default:  \
			Message(DL_ERROR, DM_UNWIND, ("TRUE: ERROR. Cannot add param ID:%#.8x, size = %d\n", id, pSingleParam->ParamSize)); \
			return error; \
		} \
		ps->Buff.pData+=sizeof(SINGLE_PARAM)+pSingleParam->ParamSize; \
		ps->Buff.dwSize-=sizeof(SINGLE_PARAM)+pSingleParam->ParamSize; \
		ps->dwParamsCount++; \
		Message(DL_INFO, DM_ADD_PARAM, ("TRUE: Add param %#x, ID:%#.8x, size: %d\n",pField->##field, id, pSingleParam->ParamSize)); \
	}else{ \
		Message(DL_ERROR, DM_ADD_PARAM, ("TRUE: ERROR. Unsufficient buffer space\n")); \
		return error; \
	}

#define ADD_MKEY(field)					ps->dwKey = pField->##field; \
		Message(DL_FULL_INFO, DM_UNWIND, ("TRUE: Add key = %#x\n", ps->dwKey)); 


#define FIELD1(field)	pField->##field
#define FIELD2(field)	pField->##field
#define FIELD4(field)	pField->##field

#endif //__LITTLE_ENDIAN_BITFIELD

#define ADD_PARAM_PTR(id, size, field)	\
	if(ps->Buff.dwSize >  size + sizeof(SINGLE_PARAM)){ \
		pSingleParam = (PSINGLE_PARAM)ps->Buff.pData; \
		pSingleParam->ParamSize = size; \
		pSingleParam->ParamID = id; \
		Message(DL_INFO, DM_ADD_PARAM, ("TRUE: Add ADD_PARAM_PTR ID:%#.8x, size: %d\n",id, size)); \
		for(count=0; count<size; count++) {\
				Message(DL_INFO, DM_ADD_PARAM, ("%.2X", (unsigned int*)((BYTE*)pSingleParam->ParamValue + count) )); \
				((unsigned char*)pSingleParam->ParamValue)[count] = ((unsigned char*)pField->##field)[count];} \
		ps->Buff.pData+=sizeof(SINGLE_PARAM)+pSingleParam->ParamSize; \
		ps->Buff.dwSize-=sizeof(SINGLE_PARAM)+pSingleParam->ParamSize; \
		ps->dwParamsCount++; \
	}else{ \
		Message(DL_ERROR, DM_UNWIND, ("TRUE: ERROR. Unsufficient buffer space\n")); \
		return error; \
	}

#define ADD_KEY(key)					ps->dwKey = key;	\
		Message(DL_FULL_INFO, DM_UNWIND, ("TRUE: Add key = %#x\n", ps->dwKey)); 


#define ADD_MKEY_PTR(field, size)	\
	if(size>4) return error; \
	switch(size){ \
		case 1: ps->dwKey = *((unsigned char*)pField->##field); break; \
		case 2:	ps->dwKey = *((unsigned short*)pField->##field); break; \
		case 3: for(count=0; count<size; count++) {\
				((unsigned char*)(&(ps->dwKey)))[count] = ((unsigned char*)pField->##field)[count];} break;\
		case 4:	ps->dwKey = *((unsigned long*)pField->##field); break; \
		default:  \
			Message(DL_ERROR, DM_UNWIND, ("TRUE: Error. Cannot add key(size = %d)\n", size)); \
			return error; \
		} \
	Message(DL_FULL_INFO, DM_UNWIND, ("TRUE: Add key = %#x, size = %d\n", ps->dwKey, size)); \

	

#define WRONG_IF_TRUE(exp)	if(exp){ return wrong;}
#define OK_IF_TRUE(exp)	if(exp){ status = ok;}
		
#define END_PROT_PARSE(size)	\
	ps->Frame.pData+=size; \
	ps->Frame.dwSize-=size; \
	Message(DL_INFO, DM_UNWIND, ("TRUE: End protocol parse, status : %d, offset: %d\n", status, size)); \
	return status; \
	}

#define	END_OF_PARSE(offset, ret)	 \
	ps->Frame.pData+=offset; \
	ps->Frame.dwSize-=offset; \
	Message(DL_INFO, DM_UNWIND, ("TRUE: End protocol parse, status : %d, offset: %d\n", ret, offset)); \
	return ret; 

#define MAX_EXCHANGE_AREA_SIZE		512

#endif //_NET_PARSER_