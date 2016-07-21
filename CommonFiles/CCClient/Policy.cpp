#include <windows.h>
#include <malloc.h>
#include <assert.h>
#include "policy.h"

/////////////////////////////////////////////////////////////////////////
//
// Functions for merging policies
//
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// AddPolicy
// Adds all source policy properties (without terminatng property) to opened policy
// ----------------------------------------------------------------------
// HPOLICY hPolicy		- Destinition policy handle
// LPPOLICY pPolicy		- Policy to add
// BOOL bFreePolicy		- If TRUE source policy will be deallocated on exit
void AddPolicy(HPOLICY hPolicy, LPPOLICY pPolicy, BOOL bFreePolicy)
{
	if (pPolicy)
	{
		DWORD dwSize = pPolicy->Size - POLICY_SIZE_EX;
		if (hPolicy->bRealloc) 
		{
			int n = (BYTE*)hPolicy->lpCurPos - (BYTE*)hPolicy->lpPolicy;
			hPolicy->lpPolicy = (LPPOLICY)realloc(hPolicy->lpPolicy, n + dwSize);
			hPolicy->lpCurPos = (char*)hPolicy->lpPolicy + n;
		}

		memcpy(hPolicy->lpCurPos, pPolicy->Data, dwSize);
		hPolicy->lpCurPos += dwSize;
		hPolicy->lpPolicy->Count += pPolicy->Count - 1;

		if (bFreePolicy) free((LPVOID)pPolicy);
	}
}

/////////////////////////////////////////////////////////////////////////
// MergePolicy
// Merges source policy with destination policy (with reallocating of destination policy's memory)
// ----------------------------------------------------------------------
// LPPOLICY *ppDestPolicy	- Pointer to pointer to destinition policy
// LPPOLICY pSourcePolicy	- Policy to add
// BOOL bFreePolicy			- If TRUE source policy will be deallocated on exit
//
// Return : Polinter to destination policy if successful else NULL
LPPOLICY MergePolicy(LPPOLICY *ppDestPolicy, LPPOLICY pSourcePolicy, BOOL bFreePolicy)
{
	if (pSourcePolicy)
	{
		DWORD n = pSourcePolicy->Size - POLICY_SIZE_EX;

		*ppDestPolicy= (LPPOLICY)realloc(*ppDestPolicy,(*ppDestPolicy)->Size + n);
		LPPROPERTY p = GetPropertyByID(*ppDestPolicy, PROPID_DONE);
		if (p)
		{
			memcpy(p, pSourcePolicy->Data,n + PROP_SIZE_DWORD);
			(*ppDestPolicy)->Count += pSourcePolicy->Count - 1;
			(*ppDestPolicy)->Size += n;

			if (bFreePolicy) free((LPVOID)pSourcePolicy);
		}
	}

	return *ppDestPolicy;
}

/////////////////////////////////////////////////////////////////////////
//
// Functions for adding / removing properties from policy
//
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// CreateProperty
// Creates property with binary data end returns pointer to created property
// ----------------------------------------------------------------------
// DWORD dwPropID		- Property's ID
// PVOID pData			- Pointer to binary data to store in property
// DWORD dwSize			- Size of binary data
// BOOL bFreeBuffer		- If true source memory block will be deallocated on exit
LPPROPERTY CreateProperty(DWORD dwPropID, PVOID pData, DWORD dwSize, BOOL bFreeBuffer)
{
	LPPROPERTY p = NULL;

	if (pData)
	{
		p = (LPPROPERTY)malloc(dwSize + PROP_SIZE);
		if (p)
		{
			p->ID = dwPropID;
			p->Size = dwSize;
			memcpy(p->Data, pData, dwSize);

			if (bFreeBuffer) free(pData);
		}
	}

	return p;
}

/////////////////////////////////////////////////////////////////////////
// AddProperty
// Reallocates memory previously created policy and adds property
// ----------------------------------------------------------------------
// LPPOLICY *ppPolicy	- Pointer to pointer to destination policy
// LPPROPERTY pProperty	- Property to add
// BOOL bFreeBuffer		- If true source property's memory block will be deallocated on exit
//
// Return : TRUE if successfull otherwise FALSE
BOOL AddProperty(LPPOLICY *ppPolicy, LPPROPERTY pProperty, BOOL bFreeBuffer)
{
	BOOL rc =  FALSE;

	if (pProperty)
	{
		if ((*ppPolicy) && ((*ppPolicy)->Size >= POLICY_SIZE_EX))
		{
			LPPROPERTY	p = GetFirstProperty(*ppPolicy), p1;
			if (p)
			{
				DWORD n = pProperty->Size + PROP_SIZE;
				*ppPolicy = (LPPOLICY)realloc(*ppPolicy, (*ppPolicy)->Size + n);
				if (*ppPolicy)
				{
					p1 = GetPropertyByID(*ppPolicy, PROPID_DONE);
					if (p1)
					{
						memcpy((PBYTE)p1 + n, p1, PROP_SIZE_DWORD);
						memcpy(p1, pProperty, n);

						(*ppPolicy)->Count++;
						(*ppPolicy)->Size += n;
						rc = TRUE;
					}
				}
			}
		}
	
		if (bFreeBuffer) free((PVOID)pProperty);
	}

	return rc;
}

/////////////////////////////////////////////////////////////////////////
// RemovePropertyByID
// Removes first or all properties with specified ID from policy and shrinks policy's memory block
// ----------------------------------------------------------------------
// LPPOLICY pPolicy		- Pointer to destination policy
// DWORD dwPropID		- ID of property(ies) to remove
// BOOL bRemoveAll		- If true finction removes all properties with specified ID
//
// Return : TRUE if successfull otherwise FALSE
BOOL RemovePropertyByID(LPPOLICY pPolicy, DWORD dwPropID, BOOL bRemoveAll)
{
	BOOL rc = FALSE;

	if (pPolicy && (pPolicy->Size >= SIZEOF_POLICY_EMPTY))
	{
		DWORD n, dwSize;
		LPPROPERTY	p = GetFirstProperty(pPolicy), p1;

		do
		{
			p1 = GetNextProperty(p);
			if (!p1) return FALSE;

			if (p->ID == dwPropID)
			{
				dwSize = p->Size + PROP_SIZE;
				n = ((PBYTE)pPolicy + pPolicy->Size) - (PBYTE)p1;
				memcpy(p, p1, n);
				pPolicy->Count--;
				pPolicy->Size -= dwSize;
				if (!bRemoveAll) break;
			}
			else
				p = p1;

		} while(p1->ID != PROPID_DONE);

		if (_expand(pPolicy, pPolicy->Size)) rc = TRUE;
	}

	return rc;
}


/////////////////////////////////////////////////////////////////////////
//
// Functions for creating policy
//
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// CreatePolicy
// Create handle to compose policy
// ----------------------------------------------------------------------
// DWORD dwID			- Policy's ID
// DWORD dwMask			- Reserved 
// DWORD dwSize			- Total size of properties to store in policy. 
//						  If Size is 0 then policy's memory will be reallocated each time new property adds.
//
// Return: Policy handle if successfull otherwise NULL.
HPOLICY CreatePolicy(DWORD dwID, DWORD dwMask, DWORD dwSize)
{
	HPOLICY  hPolicy = NULL;

	LPPOLICY pPolicy = (LPPOLICY)malloc(dwSize + SIZEOF_POLICY_EMPTY + 4);
	if (pPolicy)
	{
		hPolicy = (HPOLICY)malloc(sizeof CREATE_POLICY);
		if (hPolicy)
		{
			hPolicy->bRealloc = (dwSize == 0);
			hPolicy->lpPolicy = pPolicy;
			hPolicy->lpPolicy->ID.dwID = dwID;
			hPolicy->lpPolicy->Mask = dwMask;
			hPolicy->lpPolicy->Count = 0;
			hPolicy->lpCurPos = hPolicy->lpPolicy->Data;
		}
	}

	return hPolicy;
}

/////////////////////////////////////////////////////////////////////////
// CreatePolicyEx
// Create handle to compose policy. Uses memory buffer pointed lpPolicy as policy's memory
// ----------------------------------------------------------------------
// DWORD dwID				- Policy's ID
// DWORD dwMask			- Reserved 
// LPPOLICY pPolicy	- Pointer to preallocated memory to store policy's properties 
//						  Memory block must be enought to store :
//								1. Policy's header - POLICY_SIZE
//								2. Policy's properties
//								3. Terminating property - PROP_SIZE_DWORD
//						  
// Return: Policy handle if successfull otherwise NULL.
HPOLICY CreatePolicyEx(DWORD dwID, DWORD dwMask, LPPOLICY pPolicy)
{
	HPOLICY hPolicy = NULL;

	if (pPolicy)
	{
		hPolicy = (HPOLICY)malloc(sizeof CREATE_POLICY);
		if (hPolicy)
		{
			hPolicy->lpPolicy = pPolicy;
			hPolicy->lpPolicy->ID.dwID = dwID;
			hPolicy->lpPolicy->Mask = dwMask;
			hPolicy->lpCurPos = hPolicy->lpPolicy->Data;
			hPolicy->lpPolicy->Count = 0;
			hPolicy->bRealloc = FALSE;
		}
	}

	return hPolicy;
}

/////////////////////////////////////////////////////////////////////////
// CreatePropertyStr
// Adds string property to opened policy
// ----------------------------------------------------------------------
// HPOLICY hPolicy		- Policy handle
// DWORD dwPropID		- Property's ID
// LPCTSTR pszString	- ASCIIZ String to store in property
// BOOL bFreeBuffer		- If true source string memory block will be deallocated on exit
void CreatePropertyStr(HPOLICY hPolicy, DWORD dwPropID, LPCTSTR pszString, BOOL bFreeBuffer)
{
	if (pszString) CreatePropertyBin(hPolicy, dwPropID, (PVOID)pszString, SIZEOFSTR(pszString), bFreeBuffer); // copy to dest with terminating '\0'
}

/////////////////////////////////////////////////////////////////////////
// CreatePropertyBin
// Adds binary data property to opened policy
// ----------------------------------------------------------------------
// HPOLICY hPolicy		- Policy handle
// DWORD dwPropID		- Property's ID
// PVOID pData			- Pointer to binary data to store in property
// DWORD dwSize			- Size of binary data
// BOOL bFreeBuffer		- If true source memory block will be deallocated on exit
void CreatePropertyBin(HPOLICY hPolicy, DWORD dwPropID, const void *pData, DWORD dwSize, BOOL bFreeBuffer)
{
	if (hPolicy && pData)
	{
		if (hPolicy->bRealloc) 
		{
			DWORD dwCurrentPolicySize = (PBYTE)hPolicy->lpCurPos - (PBYTE)hPolicy->lpPolicy;	// Current policy size
			hPolicy->lpPolicy = (LPPOLICY)realloc(hPolicy->lpPolicy, dwCurrentPolicySize + SIZEOF_PROPERTY_BIN(dwSize));
			hPolicy->lpCurPos = (PCHAR)hPolicy->lpPolicy + dwCurrentPolicySize;
		}
		assert(_msize(hPolicy->lpPolicy) >=	(PBYTE)hPolicy->lpCurPos - (PBYTE)hPolicy->lpPolicy + SIZEOF_PROPERTY_BIN(dwSize)); // Trying to save more data then was set in CreatePolicy dwSize

		memcpy(hPolicy->lpCurPos, &dwPropID, sizeof DWORD);
		hPolicy->lpCurPos += sizeof DWORD;
		memcpy(hPolicy->lpCurPos, &dwSize, sizeof DWORD);
		hPolicy->lpCurPos += sizeof DWORD;

		if (dwSize > 0)
		{
			memcpy(hPolicy->lpCurPos, pData, dwSize);
			hPolicy->lpCurPos += dwSize;
		}
		
		hPolicy->lpPolicy->Count++;

		if (bFreeBuffer) free((void *)pData);
	}
}

/////////////////////////////////////////////////////////////////////////
// CreatePropertyDWORD
// Adds DWORD property to opened policy
// ----------------------------------------------------------------------
// HPOLICY hPolicy		- Policy handle
// DWORD dwPropID		- Property's ID
// DWORD dwData			- DWORD data to store in property
void CreatePropertyDWORD(HPOLICY hPolicy, DWORD dwPropID, DWORD dwData)
{
	CreatePropertyBin(hPolicy, dwPropID, &dwData, sizeof DWORD);
}

/////////////////////////////////////////////////////////////////////////
// ClosePolicy
// Closes policy handle
// ----------------------------------------------------------------------
// HPOLICY hPolicy		- Policy handle
// LPDWORD pdwDataSize	- Returns total size of properties in policy
//
// Return: Pointer to created policy if successfull otherwise NULL.
LPPOLICY ClosePolicy(HPOLICY hPolicy, LPDWORD pdwDataSize)
{
	LPPOLICY pPolicy = NULL;

	if (hPolicy)
	{
		CreatePropertyDWORD(hPolicy, PROPID_DONE, 0);		// Terminal property
		pPolicy = hPolicy->lpPolicy;
		pPolicy->Size = hPolicy->lpCurPos - (PCHAR)pPolicy;
		if (pdwDataSize) *pdwDataSize = pPolicy->Size - SIZEOF_POLICY_EMPTY;

		hPolicy->lpPolicy = NULL;

		free(hPolicy);
	}

	return pPolicy;
}

/////////////////////////////////////////////////////////////////////////
// CancelPolicy Cancels policy creation and closes hPolicy handle
// ----------------------------------------------------------------------
// HPOLICY hPolicy		- Policy handle
void CancelPolicy(HPOLICY hPolicy)
{
	if (hPolicy)
		free(hPolicy);
}
/////////////////////////////////////////////////////////////////////////
//
// Functions for retrieving data from policy
//
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// GetFirstProperty
// Returns pointer to first property in policy
// ----------------------------------------------------------------------
// LPOLICY pPolicy		- Pointer to policy
//
// Return: Pointer to first property in policy if successfull otherwise NULL.
LPPROPERTY GetFirstProperty(LPPOLICY pPolicy)
{
	if (pPolicy)
		return (LPPROPERTY)pPolicy->Data;

	return NULL;
}

/////////////////////////////////////////////////////////////////////////
// GetNextProperty
// Returns pointer to next property after property pointed by pStartProperty
// ----------------------------------------------------------------------
// LPPROPERTY pStartProperty - Points to property to start search
//
// Return: Pointer to property if successfull otherwise NULL.
LPPROPERTY GetNextProperty(LPPROPERTY pStartProperty)
{
	if (pStartProperty->ID != PROPID_DONE) return (LPPROPERTY)&pStartProperty->Data[pStartProperty->Size];

	return NULL;
}

/////////////////////////////////////////////////////////////////////////
// GetPropertyByID
// Returns pointer to first property with specified ID in policy
// ----------------------------------------------------------------------
// LPOLICY pPolicy		- Pointer to policy
// DWORD dwID			- ID of required property
//
// Return: Pointer to first property with spacified ID in policy if successfull otherwise NULL.
LPPROPERTY GetPropertyByID(LPPOLICY pPolicy, DWORD dwID)
{
	if (pPolicy && (pPolicy->Size >= SIZEOF_POLICY_EMPTY))
	{
		DWORD dwSize = 0;
		LPPROPERTY pCurProp = (LPPROPERTY)pPolicy->Data;

		while (pCurProp && (dwSize < pPolicy->Size))
		{
			if (pCurProp->ID == dwID) return pCurProp;

			pCurProp = GetNextProperty(pCurProp);
			dwSize = (PBYTE)pCurProp - (PBYTE)pPolicy;
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////
// GetNextPropertyByID
// Returns pointer to next property with specified ID after property pointed by pStartProperty
// ----------------------------------------------------------------------
// LPPROPERTY pStartProperty - Points to property to start search
//
// Return: Pointer to property if successfull otherwise NULL.
LPPROPERTY GetNextPropertyByID(LPPROPERTY pStartProperty, DWORD dwID)
{
	while (pStartProperty)
	{
		pStartProperty = GetNextProperty(pStartProperty);
		if (!pStartProperty) return NULL;
		if (pStartProperty->ID == dwID) return pStartProperty;
	}

	return NULL;
}

BOOL SavePolicyToFile(LPCTSTR pszFileName, LPPOLICY pPolicy, BOOL bFree)
{
	BOOL rc = FALSE;

	if (pszFileName && *pszFileName && pPolicy)
	{
		HANDLE hDstFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (hDstFile != INVALID_HANDLE_VALUE)
		{
			DWORD n;
			rc = WriteFile(hDstFile, (LPCVOID)pPolicy, pPolicy->Size, &n, NULL);
			CloseHandle(hDstFile);
		}
	}

	if (pPolicy && bFree) free(pPolicy);

	return rc;
}

BOOL LoadPolicyFromFile(LPCTSTR pszFileName, LPPOLICY* ppPolicy)
{
	BOOL rc = FALSE;

	if (pszFileName && *pszFileName && ppPolicy)
	{
		HANDLE hSrcFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hSrcFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwSize = GetFileSize(hSrcFile, NULL), n;
			LPPOLICY pPolicy = (LPPOLICY)malloc(dwSize);
			if (pPolicy)
			{
				rc = ReadFile(hSrcFile, pPolicy, dwSize, &n, NULL);
				if (!rc) 
					free(pPolicy);
				else
					*ppPolicy = pPolicy;
			}
			CloseHandle(hSrcFile);
		}
	}

	return rc;
}