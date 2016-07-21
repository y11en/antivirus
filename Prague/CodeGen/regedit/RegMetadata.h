//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifndef _RegMetadata_h_included
#define _RegMetadata_h_included

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

namespace MD 
{
	BOOL IsTypeEditDecimal		(tTYPE_ID i_nType);
	BOOL IsTypeEditString		(tTYPE_ID i_nType);
	BOOL IsTypeEditBinary		(tTYPE_ID i_nType);

	BOOL IsTypeEqShort			(tTYPE_ID i_nType);	
	BOOL IsTypeEqInt			(tTYPE_ID i_nType);	
	BOOL IsTypeEqUINT			(tTYPE_ID i_nType);	
	BOOL IsTypeEqLong			(tTYPE_ID i_nType);	
	
	BOOL IsTypeEqBYTE			(tTYPE_ID i_nType);	
	BOOL IsType                 (tTYPE_ID i_nType);	
	BOOL IsTypeEqDWORD			(tTYPE_ID i_nType);	

	BOOL IsTypeUnicode			(tTYPE_ID i_nType);	


	void GetTypeImgAndTitle		(tTYPE_ID i_uiType, UINT& o_uiImg, CString& o_strTypeTitle);
	void GetDataStrRep			(tTYPE_ID i_uiType, void* i_pData, DWORD i_dwSize, CString& o_strDataRep);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
#endif