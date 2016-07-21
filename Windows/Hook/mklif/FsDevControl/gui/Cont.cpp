#include "StdAfx.h"

#include "Cont.h"

#include "../../mklapi/mklapi.h"
#include "../../md5.h"
#include "../../inc/hips_tools.h"   // api для работы с драйвером klif.sys

CCont::CCont(void)
{

}

CCont::~CCont(void)
{

}

LONG gMemCounter = 0;

void* __cdecl pfMemAlloc (
						  PVOID pOpaquePtr,
						  size_t size,
						  ULONG tag )
{
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );
	if (ptr)
		InterlockedIncrement( & gMemCounter );

	return ptr;
};

void __cdecl pfMemFree (
						PVOID pOpaquePtr,
						void** pptr)
{
	if (!*pptr)
		return;

	HeapFree( GetProcessHeap(), 0, *pptr );
	*pptr = NULL;

	InterlockedDecrement( & gMemCounter );
};

HRESULT
CCont::Init()
{
	HRESULT hResult = S_OK;

	pClientContext=NULL;
	ApiVersion = 0;
	AppId = 0;

	hResult = MKL_ClientRegister (
		(PVOID*)&pClientContext, 
		AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		0
		);

	if (IS_ERROR( hResult ))
		return hResult;
	

	hResult = MKL_GetDriverApiVersion( pClientContext, &ApiVersion );
	if (IS_ERROR( hResult ))
		return hResult;;

	hResult = MKL_GetAppId( pClientContext, &AppId );

	return hResult;
}



void
CCont::AddAppl(PAPPL_ITEM pappl_item)
{
	
	APPL_RUL app_rul;
	
	memcpy ( &app_rul.apl_item, pappl_item, sizeof(APPL_ITEM) );
		
	app_rul.prul_array=new RUL_ARRAY;
	
	//FillRulArray((app_rul.prul_array));

	conteiner.push_back(app_rul);

}

void
CCont::DelAppl(int ApplNum)
{

	
	RUL_ARRAY *prul_array=GetRulArray(ApplNum);

	if (!prul_array)
		return;
	
	prul_array->erase(prul_array->begin(),prul_array->end());
	
	delete prul_array;
	

	int size=(int)conteiner.size();
	if ( size <= ApplNum ) return;

	int i=0;
	APPL_RUL_ARRAY::iterator iter=conteiner.begin();
	if (ApplNum>0)
		while (i<ApplNum)
		{
			iter++;
			i++;
		}

	conteiner.erase(iter);

}


void
CCont::ChangeApplItem(PAPPL_ITEM pappl_item, int ApplNum)
{

	int size=(int)conteiner.size();
	if ( size <= ApplNum ) return;
	
	int i=0;
	APPL_RUL_ARRAY::iterator iter=conteiner.begin();
	if (ApplNum>0)
	while (i<ApplNum)
	{
		iter++;
		i++;
	}
	
	wsprintf ((iter->apl_item).path,L"%s", pappl_item->path);
	//1 показывает что по данному знучку нажали и значение нужно поменять на противоположное
	if (pappl_item->path_ch==1)
	{
		(iter->apl_item).path_ch=(~((iter->apl_item).path_ch));
	}

	if (pappl_item->hash_ch==1)
	{
		(iter->apl_item).hash_ch=(~((iter->apl_item).hash_ch));
	}

	
	
}





void
CCont::FillRulArray(RUL_ARRAY *pra)
{
	HRESULT hResult = S_OK;

	{
		PVOID pDevices = NULL;
		ULONG NamesLen = 0;
		hResult = MKL_LLD_QueryNames( pClientContext, &pDevices, &NamesLen );

		//DebugBreak();
		if (SUCCEEDED( hResult ))
		{
			PWCHAR pwchDevice;
			ULONG EnumContext = 0;
			WCHAR wchVolumeName[260];
			while (SUCCEEDED( MKL_LLD_Enum( pDevices, NamesLen, &EnumContext, &pwchDevice ) ))
			{
				MKLIF_LLD_INFO Info;
				
				hResult = MKL_LLD_GetInfo( pClientContext, pwchDevice, &Info );

				if (SUCCEEDED( hResult ))
				{
					hResult=MKL_GetVolumeName (
						pClientContext,
						pwchDevice,
						wchVolumeName,
						250
						);
					if (SUCCEEDED( hResult ))
					{
						RUL_ITEM rul_item;

						int i=0;
						
						{
							wsprintfW(rul_item.symbolLink,L"%s (%s)",wchVolumeName, pwchDevice);
							wsprintfW (rul_item.deviceName,L"%s*",pwchDevice);
							rul_item.w_3st=0;
							rul_item.r_3st=0;
							rul_item.e_3st=0;

							pra->push_back(rul_item);

						
						}
					}
				}

				
			}

			pfMemFree( NULL, &pDevices );
		}
	}

}


RUL_ARRAY* CCont::GetRulArray(int ApplNum)
{
	int size=(int)conteiner.size();
	if ( size <= ApplNum ) return NULL;
	
	APPL_RUL_ARRAY::iterator iter = conteiner.begin();
	
	
	int i=0;
	
	if (ApplNum>0)
	while (i< ApplNum)
	{
		iter++;
		i++;
	}
	
	return iter->prul_array;
	
	
}


void
CCont::ChangeRulItem(PRUL_ITEM prul_item, int ApplNum, int RulNum)
{
	RUL_ARRAY *prul_array=GetRulArray(ApplNum);

	if (!prul_array)
		return;
	
	
	int size=(int)prul_array->size();
	if ( size <= RulNum ) return;

	int i=0;
	RUL_ARRAY::iterator iter=prul_array->begin();
	if (RulNum>0)
	while (i<RulNum && iter!=prul_array->end())
	{
		iter++;
		i++;
	}

	iter->w_3st = prul_item->w_3st;
	iter->r_3st = prul_item->r_3st;
	iter->e_3st = prul_item->e_3st;
	iter->logFlag = prul_item->logFlag;
	iter->blockID = prul_item->blockID;
}


BOOL
CCont::AddRulItem(PRUL_ITEM prul_item, int ApplNum)
{
	if ( ApplNum<0 )return FALSE;

	RUL_ARRAY *prul_array = GetRulArray( ApplNum );

	if (!prul_array)
		return FALSE;

	prul_array->push_back( *prul_item );
	
	return TRUE;
}



HRESULT CCont::ResetApplRulesFromDriver()
{
	
	return 0;
}

/*#define ALLOW	0x0 //00
#define ASK		0x1 //01
#define DENY	0x3	//11

#define WRITE_BIT_OFFSET	0x00 //00 00 11
#define READ_BIT_OFFSET		0x02 //00 11 00	
#define ENUM_BIT_OFFSET		0x04 //11 00 00
*/


HRESULT CCont::InsertApplRulesToDriver()
{
	HRESULT hResult = S_OK;

	md5_byte_t digest[16];
	RUL_ARRAY *prul_array;
	ULONG digest_size=sizeof(digest);

	APPL_RUL_ARRAY::iterator iter_ara;
	RUL_ARRAY::iterator	iter_ra;
	
	//DebugBreak();
	
	
	MKL_ResetClientRules(pClientContext);

	ULONG mask;
	LONGLONG RulID;
	ULONG RulID_size;
	WCHAR nativeAppPath[260];
	ULONG nativeAppPathSize;
	
	WCHAR nativeFilePath[260];
	ULONG nativeFilePathSize;



	for(iter_ara=conteiner.begin(); iter_ara!=conteiner.end(); ++iter_ara)
	{
		MKL_QueryFileHash(pClientContext,iter_ara->apl_item.path, digest,&digest_size);
		
		nativeAppPathSize=260*sizeof(WCHAR);
		memset(nativeAppPath,0,  nativeAppPathSize);

		hResult = MKL_QueryFileNativePath( pClientContext,iter_ara->apl_item.path,nativeAppPath, &nativeAppPathSize );
		if ( !SUCCEEDED(hResult) )
		{
			wsprintfW(nativeAppPath, L"%s", iter_ara->apl_item.path);
		}
		else
			memset((char*)nativeAppPath+nativeAppPathSize,0,  2);
		
		prul_array=iter_ara->prul_array;
		for(iter_ra=prul_array->begin(); iter_ra!=prul_array->end(); ++iter_ra)
		{
			nativeFilePathSize=260*sizeof(WCHAR);
			memset(nativeFilePath,0,  nativeFilePathSize);
			
			hResult = MKL_QueryFileNativePath( pClientContext,iter_ra->deviceName, nativeFilePath, &nativeFilePathSize );
			if ( !SUCCEEDED(hResult) )
			{
				wsprintfW( nativeFilePath, L"%s", iter_ra->deviceName );
			}
			else
				memset((char*)nativeFilePath+nativeFilePathSize,0,  2);

			//mask=WREToMask(iter_ra->w_3st,iter_ra->r_3st,iter_ra->e_3st);

			mask = HIPS_SET_BLOCK_POS (HIPS_FLAG_DENY, HIPS_LOG_ON, 0, HIPS_POS_WRITE, 0);
			char str[260];
			sprintf (str, "%d", mask);
			MessageBoxA (0, str, str, 0);

			RulID_size = sizeof(RulID);
			
			hResult = MKL_AddApplRule(
				pClientContext,
				nativeAppPath,
				nativeFilePath,
				iter_ra->ValueName,
				&digest,
				digest_size,
				mask,
				&RulID,
				iter_ra->blockID);	

			if (SUCCEEDED (hResult)) MessageBox (0, L"MKL_AddApplRule () succeeded", L"test", 0);

		}
	}

	hResult = MKL_ApplyRules (pClientContext);
	if (SUCCEEDED (hResult)) MessageBox (0, L"MKL_ApplyRules () succeeded", L"test", 0);

	MessageBox (0, nativeAppPath, nativeFilePath, 0);

	return S_OK;
}
