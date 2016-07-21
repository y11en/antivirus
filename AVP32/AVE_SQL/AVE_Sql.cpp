// AVE_Sql.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "AVE_Sql.h"

#include "Sql\_Sql.h"

#include "EvCache.h"

#include <stdlib.h>

// -----------------------------------------------------------------------------------------

CSql g_Sql;
CHAR SqlCmd_[10000];

CSql g_SqlFields;
CHAR SqlFieldsCmd[10000];

CSql g_SqlRecords;
PCHAR pSqlRecordsFilter = NULL;

void* (* _MM_Alloc)(unsigned int) = 0;
void  (* _MM_Free)(void*) = 0;

typedef struct _FD
{
	PVOID pData;
	SIZE_T Size;
}FD, *PFD;

void Bind(DWORD UserValue, SWORD ParamID, SWORD* pLocalType, SWORD* pSqlType, SDWORD* pValueSize)
{
	PFD pfd = (PFD) UserValue;

	*pLocalType = SQL_C_BINARY;
	*pSqlType = SQL_LONGVARBINARY;
	*pValueSize = pfd->Size;
}

void PutData(DWORD UserValue, SWORD ParamID, PVOID* Buffer, SDWORD* pBufferSize)
{
	PFD pfd = (PFD) UserValue;
	
	*Buffer = pfd->pData;
	*pBufferSize = pfd->Size;
}

void PrepareString(CHAR* szInStr, CHAR* szOutStr)
{
	int cou = 0;
	int cou2 = 0;
	while (szInStr[cou] != '\0')
	{
		szOutStr[cou2] = szInStr[cou];
		if (szOutStr[cou2] == '\'')
		{
			cou2++;
			szOutStr[cou2] = '\'';
		}
		cou2++;
		cou++;
	}
	szOutStr[cou2] = szInStr[cou];
}
// -----------------------------------------------------------------------------------------


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

// -----------------------------------------------------------------------------------------
int _MM_Compare(void* p1, void* p2, int len)
{
	int ret = *(DWORD*)p1 == *(DWORD*)p2 ? 0 : 1;
	return ret;
}

void _MM_FreeItemValue(void* item, int len, void* userdata)
{
	_MM_Free(userdata);
}

typedef struct _tAVC_FIELD
{
	DWORD m_FieldID;
	DWORD m_FieldSize;
	PVOID m_pFieldValue;
}AVC_FIELD, *PAVC_FIELD;

typedef struct _tCACHED_ITEM
{
	PHashTree	m_pHash;
	DWORD			RecordIDH;
	DWORD			RecordIDL;
}CACHED_ITEM, *PCACHED_ITEM;

CACHED_ITEM g_CachedItem;

// -----------------------------------------------------------------------------------------


AVE_SQL_API BOOL InitDB(void* (*pfMemAlloc)(unsigned int), void (*pfMemFree)(void*))
{
	BOOL bRet = TRUE;
	_MM_Alloc = pfMemAlloc;
	_MM_Free = pfMemFree;
	
	bRet |= g_Sql.Trusted_Connect("VirLibDB", "Ave_Sql", 0);
	bRet |= g_SqlFields.Trusted_Connect("VirLibDB", "Ave_Sql", 0);
	bRet |= g_SqlRecords.Trusted_Connect("VirLibDB", "Ave_Sql", 0);

	_Hash_Init_Library(_MM_Alloc, _MM_Free, _MM_Compare, _MM_FreeItemValue);

	g_CachedItem.m_pHash = _Hash_InitNew(4);
	g_CachedItem.RecordIDH = 0;
	g_CachedItem.RecordIDL = 0;

	return bRet;
}

AVE_SQL_API void DoneDB(void)
{
	_Hash_Done(g_CachedItem.m_pHash, 1);
	
	g_CachedItem.m_pHash = NULL;
	g_CachedItem.RecordIDH = 0;
	g_CachedItem.RecordIDL = 0;

	if (g_Sql.IsConnected())
	{
		g_Sql.DropData();
		g_Sql.DropConnect();
	}

	if (g_SqlFields.IsConnected())
	{
		g_SqlFields.DropData();
		g_SqlFields.DropConnect();
	}
	
	if (g_SqlRecords.IsConnected())
	{
		g_SqlRecords.DropData();
		g_SqlRecords.DropConnect();
	}

	if (pSqlRecordsFilter != NULL)
	{
		_MM_Free(pSqlRecordsFilter);
		pSqlRecordsFilter = NULL;
	}
}

AVE_SQL_API BOOL ClearDatabase(void)
{
	lstrcpy(SqlCmd_, "delete from Uni_Data delete from Uni_Records delete from Uni_Fields");
	return g_Sql.DoSqlQueryIn(SqlCmd_);
}

AVE_SQL_API BOOL AddRecord(DWORD RecordIDH, DWORD RecordIDL)
{
	BOOL bRet = FALSE;
	wsprintf(SqlCmd_, "insert into Uni_Records (RecordIDH, RecordIDL) values (%d, %d)", RecordIDH, RecordIDL);

	bRet = g_Sql.DoSqlQueryIn(SqlCmd_);
	if (bRet != 1)	bRet = 0;
	
	return bRet;
}

BOOL SetFieldInternal(DWORD RecordIDH, DWORD RecordIDL, DWORD dwFieldID, PVOID pData, DWORD dwSize, BOOL* pbSetMethod)
{
	BOOL bRet = FALSE;
	FD fd;

	fd.pData = pData;
	fd.Size = dwSize;

	int ParamCount = 0;
	char Method[32];

	if (pbSetMethod == NULL)
		lstrcpy(Method, "NULL");
	else
		wsprintf(Method, "%d", *pbSetMethod);


	DWORD dwFieldType = -1;

	switch (dwFieldID & FIELD_TYPE_MASK)
	{
	case FIELD_TYPE_DWORD:
		dwFieldType = 0;
		wsprintf(SqlCmd_, "execute sp_UniData_SetField %d, %d, %d, %d, %d, NULL, NULL, %s", RecordIDH, RecordIDL, dwFieldType, dwFieldID & FIELD_ID_MASK, *(DWORD*) pData, Method);
		break;
	case FIELD_TYPE_STRING:
		{
			CHAR pszPrep[8001];
			dwFieldType = 1;
			PrepareString((CHAR*) pData, pszPrep);
			wsprintf(SqlCmd_, "execute sp_UniData_SetField %d, %d, %d, %d, NULL, '%s', NULL, %s", RecordIDH, RecordIDL, dwFieldType, dwFieldID & FIELD_ID_MASK, pszPrep, Method);
		}
		break;
	case FIELD_TYPE_BINARY:
		ParamCount = 1;
		dwFieldType = 2;
		wsprintf(SqlCmd_, "execute sp_UniData_SetField %d, %d, %d, %d, NULL, NULL, ?, %s", RecordIDH, RecordIDL, dwFieldType, dwFieldID & FIELD_ID_MASK, Method);
		break;
	}
		
	if (dwFieldType == -1)
		return bRet;
	
	if (ParamCount > 0)
		bRet = g_Sql.DoBindQuery(SqlCmd_, ParamCount, Bind, PutData, (DWORD) &fd);
	else
		bRet = g_Sql.DoSqlQuery(SqlCmd_);
	
	if (bRet) g_Sql.DropData();
	if (bRet != 1)	bRet = 0;
	
	return bRet;
}

AVE_SQL_API BOOL SetField(DWORD RecordIDH, DWORD RecordIDL, DWORD dwFieldID, PVOID* pData, DWORD dwSize)
{
	return SetFieldInternal(RecordIDH, RecordIDL, dwFieldID, pData, dwSize, NULL);
}

AVE_SQL_API BOOL UpdateField(DWORD RecordIDH, DWORD RecordIDL, DWORD dwFieldID, PVOID pData, DWORD dwSize)
{
	BOOL bMethod = 1;
	return SetFieldInternal(RecordIDH, RecordIDL, dwFieldID, pData, dwSize, &bMethod);
}

AVE_SQL_API BOOL InsertField(DWORD RecordIDH, DWORD RecordIDL, DWORD dwFieldID, PVOID pData, DWORD dwSize)
{
	BOOL bMethod = 0;
	return SetFieldInternal(RecordIDH, RecordIDL, dwFieldID, pData, dwSize, &bMethod);
}

AVE_SQL_API BOOL GetField(DWORD RecordIDH, DWORD RecordIDL, DWORD dwFieldID, PVOID* pData, DWORD* pDataSize)
{
	BOOL bRet = FALSE;

	if ((pData == NULL) || (pDataSize == NULL))
		return bRet;
	
	*pData = NULL;
	*pDataSize = 0;

	DWORD dwFieldType = -1;
	
	switch (dwFieldID & FIELD_TYPE_MASK)
	{
	case FIELD_TYPE_DWORD:
		dwFieldType = 0;
		wsprintf(SqlCmd_, "select FieldDataDW from Uni_Data where RecordIDH = %d and RecordIDL = %d and FieldID = %d", RecordIDH, RecordIDL, dwFieldID & FIELD_ID_MASK);
		break;
	case FIELD_TYPE_STRING:
		dwFieldType = 1;
		wsprintf(SqlCmd_, "select FieldDataStr from Uni_Data where RecordIDH = %d and RecordIDL = %d and FieldID = %d", RecordIDH, RecordIDL, dwFieldID & FIELD_ID_MASK);
		break;
	case FIELD_TYPE_BINARY:
		dwFieldType = 2;
		wsprintf(SqlCmd_, "select FieldData from Uni_Data where RecordIDH = %d and RecordIDL = %d and FieldID = %d", RecordIDH, RecordIDL, dwFieldID & FIELD_ID_MASK);
		break;
	}
	
	if (dwFieldType == -1)
		return bRet;

	if (g_Sql.DoSqlQuery(SqlCmd_))
	{
		if (g_Sql.NextData())
		{
			SFieldValue dbVal;
			g_Sql.GetFiledValue(&dbVal, 0);
			if (dbVal.vt != SFieldValue::VT_NULL)
			{
				*pData = _MM_Alloc(dbVal.FieldSize);
				if (*pData != NULL)
				{
					bRet = TRUE;
					*pDataSize = dbVal.FieldSize;
					CopyMemory(*pData, dbVal.Value.pcVal, dbVal.FieldSize);
				}
			}
		}

		g_Sql.DropData();
	}

	return bRet;
}

// -----------------------------------------------------------------------------------------
static int GetConvertBase( const char *pS ) {
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

static unsigned long A2UI( const char *pS, char **pEndPtr ) {
	int nBase;
	if ( pEndPtr )
		*pEndPtr = (char *)pS;
	while ( isspace((int)(unsigned char)*pS) )
		++pS;
	nBase = GetConvertBase( pS );
	if ( nBase > 0 )
		return strtoul( pS, pEndPtr, nBase );
	return 0;
}
// -----------------------------------------------------------------------------------------

AVE_SQL_API BOOL DeleteRecord(DWORD RecordIDH, DWORD RecordIDL)
{
	BOOL bRet;

	wsprintf(SqlCmd_, "delete from Uni_Data where RecordIDH = %d and RecordIDL = %d delete from Uni_Records where RecordIDH = %d and RecordIDL = %d", RecordIDH, RecordIDL, RecordIDH, RecordIDL);
	bRet = g_Sql.DoSqlQueryIn(SqlCmd_);
	if (bRet != 1)	bRet = 0;

	return bRet;
}

AVE_SQL_API BOOL SetFilter(CHAR* szFilterString)
{
	CHAR ParamID[32];
	CHAR strParamID[32];
	PCHAR pNext;
	long FieldID;
	CHAR ch[2];
	ch[1] = 0;
	if (pSqlRecordsFilter != NULL)
	{
		_MM_Free(pSqlRecordsFilter);
		pSqlRecordsFilter = NULL;
	}
		
	if ((szFilterString == NULL) || (lstrlen(szFilterString) == 0))
	{
		return TRUE;
	}
	
	pSqlRecordsFilter = (PCHAR) _MM_Alloc(lstrlen(szFilterString) + 4096);
	if (pSqlRecordsFilter == NULL)
		return FALSE;

	int cou = 0;

	lstrcpy(pSqlRecordsFilter, "select Idx, RecordIDH, RecordIDL from v_Uni_RD where ");

	while (lstrlen(szFilterString) > cou)
	{
		int coud = 0;
		lstrcpy(ParamID, "");
		
		if (szFilterString[cou] == '{')
		{
			cou++;
			while (szFilterString[cou] != '}')
				ParamID[coud++]  = szFilterString[cou++];

			ParamID[coud] = 0;
			FieldID = A2UI(ParamID, &pNext);
			
			wsprintf(strParamID, "%d", FieldID & FIELD_ID_MASK);
			
			lstrcat(pSqlRecordsFilter, " FieldID=");
			lstrcat(pSqlRecordsFilter, strParamID);

			if (szFilterString[cou + 1] != 0)
			{
				if ((szFilterString[cou + 1] == '=')
					|| (szFilterString[cou + 1] == '!')
					|| (szFilterString[cou + 1] == '>')
					|| (szFilterString[cou + 1] == '<')
					|| (szFilterString[cou + 1] == '&')
					|| (szFilterString[cou + 1] == '|')
					|| (szFilterString[cou + 1] == '^'))
				{
					lstrcat(pSqlRecordsFilter, " AND ");
					switch (FieldID & FIELD_TYPE_MASK)
					{
					case FIELD_TYPE_DWORD:
						lstrcat(pSqlRecordsFilter, "FieldDataDW");
						break;
					case FIELD_TYPE_STRING:
						lstrcat(pSqlRecordsFilter, "FieldDataStr");
						break;
					case FIELD_TYPE_BINARY:
						lstrcat(pSqlRecordsFilter, "FieldData");
						break;
					}
				}
			}
		}
		else
		{
			ch[0] = szFilterString[cou];
			lstrcat(pSqlRecordsFilter, ch);
		}
		cou++;
	}
	
	lstrcat(pSqlRecordsFilter, " FOR BROWSE");
#ifdef _DEBUG
	OutputDebugString(pSqlRecordsFilter);
	OutputDebugString("\n");
	int len = lstrlen(pSqlRecordsFilter);
#endif

	return TRUE;
}

AVE_SQL_API DWORD EnumRecords(BOOL bFirst, DWORD* RecordIDH, DWORD* RecordIDL)
{
	PCHAR pReq;
	if ((RecordIDH == NULL) || (RecordIDL == NULL))
		return 0;
	
	if (bFirst == TRUE)
	{
		g_SqlRecords.DropData();
		if (pSqlRecordsFilter != NULL)
			pReq = pSqlRecordsFilter;
		else
			pReq = "select Idx, RecordIDH, RecordIDL from Uni_Records FOR BROWSE";

		if (g_SqlRecords.DoSqlQuery(pReq) != 1)
		{
			g_SqlRecords.DropData();
			return 0;
		}
	}

	if (g_SqlRecords.NextData() == FALSE)
		return 0;

	SFieldValue ArrVal[3];
	g_SqlRecords.GetFiledValue(&ArrVal[0], 0);
	g_SqlRecords.GetFiledValue(&ArrVal[1], 1);
	g_SqlRecords.GetFiledValue(&ArrVal[2], 2);

	if ((ArrVal[0].vt == SFieldValue::VT_NULL) || (ArrVal[1].vt == SFieldValue::VT_NULL) || (ArrVal[2].vt == SFieldValue::VT_NULL))
		return 0;

	*RecordIDH = *ArrVal[1].Value.plVal;
	*RecordIDL = *ArrVal[2].Value.plVal;
	
	return *ArrVal[0].Value.plVal;
}

AVE_SQL_API BOOL GetFieldsCount(DWORD RecordIDH, DWORD RecordIDL, DWORD* pCount)
{
	BOOL bRet;
	wsprintf(SqlCmd_, "select count(*) from Uni_Data where RecordIDH = %d and RecordIDL = %d", RecordIDH, RecordIDL);
	
	bRet = g_Sql.DoSqlQuery(SqlCmd_);
	if (bRet)
	{
		bRet = FALSE;
		if (g_Sql.NextData())
		{
			SFieldValue dbVal;
			g_Sql.GetFiledValue(&dbVal, 0);
			if (dbVal.vt == SFieldValue::VT_I4)
			{
				*pCount = *dbVal.Value.plVal;
				bRet = TRUE;
			}
		}
		g_Sql.DropData();
	}
	
	return bRet;
}

AVE_SQL_API BOOL EnumFields(DWORD RecordIDH, DWORD RecordIDL, BOOL bFirst, DWORD* pFieldID)
{
	if (pFieldID == NULL)
		return FALSE;
	
	if (bFirst == TRUE)
	{
		g_SqlFields.DropData();
		wsprintf(SqlFieldsCmd, "select FieldID from Uni_Data where RecordIDH = %d and RecordIDL = %d group by FieldID FOR BROWSE", RecordIDH, RecordIDL);
		
		if (g_SqlFields.DoSqlQuery(SqlFieldsCmd) != 1)
		{
			g_SqlFields.DropData();
			return FALSE;
		}
	}
	
	if (g_SqlFields.NextData() == FALSE)
		return FALSE;
	
	SFieldValue dbVal;
	g_SqlFields.GetFiledValue(&dbVal, 0);
	
	if (dbVal.vt == SFieldValue::VT_NULL)
		return FALSE;
	
	*pFieldID = *dbVal.Value.plVal;
	
	return TRUE;
}

AVE_SQL_API BOOL IsRecord(DWORD RecordIDH, DWORD RecordIDL)
{
	BOOL bRet;
	wsprintf(SqlCmd_, "select count(*) from Uni_Records where RecordIDH = %d and RecordIDL = %d", RecordIDH, RecordIDL);
	
	bRet = g_Sql.DoSqlQuery(SqlCmd_);
	if (bRet)
	{
		bRet = FALSE;
		if (g_Sql.NextData())
		{
			SFieldValue dbVal;
			g_Sql.GetFiledValue(&dbVal, 0);
			if (dbVal.vt == SFieldValue::VT_I4)
				if (*dbVal.Value.plVal > 0)
					bRet = TRUE;
		}
		g_Sql.DropData();
	}
	
	return bRet;
}

AVE_SQL_API BOOL IsField(DWORD RecordIDH, DWORD RecordIDL, DWORD dwFieldID)
{
	BOOL bRet = FALSE;

	switch (dwFieldID & FIELD_TYPE_MASK)
	{
	case FIELD_TYPE_DWORD:
		wsprintf(SqlCmd_, "select count(*) from Uni_Data where RecordIDH = %d and RecordIDL = %d and FieldID = %d and FieldDataDW is not NULL", RecordIDH, RecordIDL, dwFieldID & FIELD_ID_MASK);
		break;
	case FIELD_TYPE_STRING:
		wsprintf(SqlCmd_, "select count(*) from Uni_Data where RecordIDH = %d and RecordIDL = %d and FieldID = %d and FieldDataStr is not NULL", RecordIDH, RecordIDL, dwFieldID & FIELD_ID_MASK);
		break;
	case FIELD_TYPE_BINARY:
		wsprintf(SqlCmd_, "select count(*) from Uni_Data where RecordIDH = %d and RecordIDL = %d and FieldID = %d and FieldData is not NULL", RecordIDH, RecordIDL, dwFieldID & FIELD_ID_MASK);
		break;
	default:
		return FALSE;
	}
	
	bRet = g_Sql.DoSqlQuery(SqlCmd_);
	if (bRet)
	{
		bRet = FALSE;
		if (g_Sql.NextData())
		{
			SFieldValue dbVal;
			g_Sql.GetFiledValue(&dbVal, 0);
			if (dbVal.vt == SFieldValue::VT_I4)
				if (*dbVal.Value.plVal > 0)
					bRet = TRUE;
		}
		g_Sql.DropData();
	}
	
	return bRet;
}

AVE_SQL_API BOOL GetFieldCached(DWORD RecordIDH, DWORD RecordIDL, DWORD dwFieldID, PVOID* pData, DWORD* pDataSize, BOOL DoNotAlloc)
{
	BOOL bRet = FALSE;
	
	if ((pData == NULL) || (pDataSize == NULL))
		return bRet;
	
	*pData = NULL;
	*pDataSize = 0;

	PAVC_FIELD pAvc;

	if ((g_CachedItem.RecordIDH != RecordIDH) || (g_CachedItem.RecordIDL != RecordIDL))
	{
		_Hash_Free_AllItems(g_CachedItem.m_pHash, 1);

		wsprintf(SqlCmd_, "select FieldID, FieldDataDW, FieldDataStr, FieldData from Uni_Data where RecordIDH = %d and RecordIDL = %d", RecordIDH, RecordIDL);
		
		if (g_Sql.DoSqlQuery(SqlCmd_))
		{
			SFieldValue ArrVal[4];

			DWORD FieldType[3] = {FIELD_TYPE_DWORD, FIELD_TYPE_STRING, FIELD_TYPE_BINARY};

			g_CachedItem.RecordIDH = RecordIDH;
			g_CachedItem.RecordIDL = RecordIDL;

			while (g_Sql.NextData())
			{
				for (int cou = 0; cou < sizeof(ArrVal)/sizeof(ArrVal[0]); cou++)
				{
					g_Sql.GetFiledValue(&ArrVal[cou], cou);

					if (cou != 0)
					{
						if (ArrVal[cou].vt != SFieldValue::VT_NULL)
						{
							pAvc = (PAVC_FIELD) _MM_Alloc(ArrVal[cou].FieldSize + sizeof (AVC_FIELD));
							if (pAvc != NULL)
							{
								pAvc->m_pFieldValue = (BYTE*) pAvc + sizeof(AVC_FIELD);
								CopyMemory(pAvc->m_pFieldValue, ArrVal[cou].Value.pbVal, ArrVal[cou].FieldSize);
								pAvc->m_FieldSize = ArrVal[cou].FieldSize;

								pAvc->m_FieldID = FieldType[cou - 1] | (*ArrVal[0].Value.plVal);
								_Hash_AddItem(g_CachedItem.m_pHash, &pAvc->m_FieldID, sizeof(pAvc->m_FieldID), pAvc);
							}
						}
					}
				}
			}
			
			g_Sql.DropData();
		}
	}

	PHashTreeItem pItem = _Hash_Find(g_CachedItem.m_pHash, &dwFieldID, sizeof(dwFieldID));
	if (pItem != NULL)
	{
		pAvc = (PAVC_FIELD) pItem->m_pUserData;
		if (pAvc != NULL)
		{
			*pDataSize = pAvc->m_FieldSize;
			
			if (DoNotAlloc)
				*pData = pAvc->m_pFieldValue;
			else
			{
				*pData = _MM_Alloc(pAvc->m_FieldSize);
				if(*pData != NULL)
					CopyMemory(*pData, pAvc->m_pFieldValue, pAvc->m_FieldSize);
			}
			if (*pData != NULL)
				bRet = TRUE;
			else 
				*pDataSize = 0;
		}

	}

	return bRet;
}