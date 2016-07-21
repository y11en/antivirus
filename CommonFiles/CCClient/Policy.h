//////////////////////////////////////////////////////////////////////
// Policy.h Header file. 

#ifndef POLICY_H
#define POLICY_H

#include <tchar.h>

//////////////////////////////////////////////////////////////////////
// Predefined policy IDs (1 - 999) 
#define PID_TMP				0		// Temporary policy
#define PID_RETCODE			1		// Policy with operation return code

//////////////////////////////////////////////////////////////////////
// Predefined property IDs (0 - 999)
#define PROPID_DONE			0		// Policy terminatng property
#define PROPID_RETCODE		1		// Property with operation return code
#define PROPID_DATA			2		// Property with operation data

#pragma warning (disable : 4200)
#pragma pack(push, MyAlign, 1)

////////////////////////////////////////////////////////////////////////////////
// Policy's ID
union POLICY_ID {
	DWORD dwID;
	struct {
		WORD wProdID, wPolicyID;
	};
	struct {
		BYTE bProdCode, bProdFlags;
	};
};

////////////////////////////////////////////////////////////////////////////////
// Policy's Header
typedef struct POLICY
{
	POLICY_ID ID;	// Policy ID
	DWORD Mask;		// Reserved
	DWORD Size;		// Aggregate size of policy (properties and policy's private data (includes policy terminatng property))
	DWORD Count;	// Number of policy's properties
	CHAR Data[];	// Pointer to policy's data
} *LPPOLICY, *PPOLICY;

////////////////////////////////////////////////////////////////////////////////
// Property's Header
typedef struct PROPERTY
{
	DWORD ID;		// Property ID
	DWORD Size;		// Only size of property's data (without private data)
	CHAR Data[];	// Pointer to property's data
} *LPPROPERTY, *PPROPERTY;

#define SIZEOFSTR(pszString)	((_tcslen(pszString) + 1) * sizeof(TCHAR))

////////////////////////////////////////////////////////////////////////////////
// Sizes of policy's elements

#define SIZEOF_PROPERTY					(sizeof(PROPERTY))
#define SIZEOF_PROPERTY_DWORD			(SIZEOF_PROPERTY + sizeof(DWORD))		// Size of DWORD property
#define SIZEOF_PROPERTY_DWORD_N(n)		(SIZEOF_PROPERTY_DWORD * (n))			// Size of 'n' DWORD property
#define SIZEOF_PROPERTY_BIN(dwDataSize)	(SIZEOF_PROPERTY + dwDataSize)			// Size of Binary property with dwDataSize size
__inline DWORD SIZEOF_PROPERTY_STR(LPCTSTR pszString)							// Size of string property
{
	return (pszString?(SIZEOF_PROPERTY + SIZEOFSTR(pszString)):0);
}	

#define SIZEOF_POLICY					(sizeof(POLICY))						// Size of policy's header
#define SIZEOF_POLICY_EMPTY				(SIZEOF_POLICY + SIZEOF_PROPERTY_DWORD)	// Size of empty policy (header and terminatng property)

////////////////////////////////////////////////////////////////////////////////
// Private structs

// policy creation struct
typedef struct CREATE_POLICY
{
	BOOL bRealloc;
	LPPOLICY lpPolicy;
	PCHAR lpCurPos;
} *HPOLICY;

#pragma pack(pop, MyAlign)

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
void AddPolicy(HPOLICY hPolicy, LPPOLICY pPolicy, BOOL bFreePolicy = FALSE);

/////////////////////////////////////////////////////////////////////////
// MergePolicy
// Merges source policy with destination policy (with reallocating of destination policy's memory)
// ----------------------------------------------------------------------
// LPPOLICY *ppDestPolicy	- Pointer to pointer to destinition policy
// LPPOLICY pSourcePolicy	- Policy to add
// BOOL bFreePolicy			- If TRUE source policy will be deallocated on exit
//
// Return : Polinter to destination policy if successful else NULL
LPPOLICY MergePolicy(LPPOLICY *ppDestPolicy, LPPOLICY pSourcePolicy, BOOL bFreePolicy = FALSE);

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
LPPROPERTY CreateProperty(DWORD dwPropID, PVOID pData, DWORD dwSize, BOOL bFreeBuffer = FALSE);

/////////////////////////////////////////////////////////////////////////
// AddProperty
// Reallocates memory previously created policy and adds property
// ----------------------------------------------------------------------
// LPPOLICY *ppPolicy	- Pointer to pointer to destination policy
// LPPROPERTY pProperty	- Property to add
// BOOL bFreeBuffer		- If true source property's memory block will be deallocated on exit
//
// Return : TRUE if successfull otherwise FALSE
BOOL AddProperty(LPPOLICY *ppPolicy, LPPROPERTY pProperty, BOOL bFreeBuffer = FALSE);

/////////////////////////////////////////////////////////////////////////
// RemovePropertyByID
// Removes first or all properties with specified ID from policy and shrinks policy's memory block
// ----------------------------------------------------------------------
// LPPOLICY pPolicy		- Pointer to destination policy
// DWORD dwPropID		- ID of property(ies) to remove
// BOOL bRemoveAll		- If true finction removes all properties with specified ID
//
// Return : TRUE if successfull otherwise FALSE
BOOL RemovePropertyByID(LPPOLICY pPolicy, DWORD dwPropID, BOOL bRemoveAll = FALSE);

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
HPOLICY CreatePolicy(DWORD dwID, DWORD dwMask, DWORD dwSize);

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
HPOLICY CreatePolicyEx(DWORD dwID, DWORD dwMask, LPPOLICY pPolicy);

/////////////////////////////////////////////////////////////////////////
// CreatePropertyStr
// Adds string property to opened policy
// ----------------------------------------------------------------------
// HPOLICY hPolicy		- Policy handle
// DWORD dwPropID		- Property's ID
// LPCTSTR pszString	- ASCIIZ String to store in property
// BOOL bFreeBuffer		- If true source string memory block will be deallocated on exit
void CreatePropertyStr(HPOLICY hPolicy, DWORD PropID, LPCTSTR pszString, BOOL bFreeBuffer = FALSE);

/////////////////////////////////////////////////////////////////////////
// CreatePropertyBin
// Adds binary data property to opened policy
// ----------------------------------------------------------------------
// HPOLICY hPolicy		- Policy handle
// DWORD dwPropID		- Property's ID
// const void *pData			- Pointer to binary data to store in property
// DWORD dwSize			- Size of binary data
// BOOL bFreeBuffer		- If true source memory block will be deallocated on exit
void CreatePropertyBin(HPOLICY hPolicy, DWORD dwPropID, const void *pData, DWORD dwSize, BOOL bFreeBuffer = FALSE);

/////////////////////////////////////////////////////////////////////////
// CreatePropertyDWORD
// Adds DWORD property to opened policy
// ----------------------------------------------------------------------
// HPOLICY hPolicy		- Policy handle
// DWORD dwPropID		- Property's ID
// DWORD dwData			- DWORD data to store in property
void CreatePropertyDWORD(HPOLICY hPolicy, DWORD PropID, DWORD dwData);

/////////////////////////////////////////////////////////////////////////
// ClosePolicy
// Closes hPolicy handle
// ----------------------------------------------------------------------
// HPOLICY hPolicy		- Policy handle
// LPDWORD pdwDataSize	- Returns total size of properties in policy
//
// Return: Pointer to created policy if successfull otherwise NULL.
LPPOLICY ClosePolicy(HPOLICY hPolicy, LPDWORD pdwDataSize);

/////////////////////////////////////////////////////////////////////////
// CancelPolicy Cancels policy creation and closes hPolicy handle
// ----------------------------------------------------------------------
// HPOLICY hPolicy		- Policy handle
void CancelPolicy(HPOLICY hPolicy);

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
LPPROPERTY GetFirstProperty(LPPOLICY pPolicy);

/////////////////////////////////////////////////////////////////////////
// GetNextProperty
// Returns pointer to next property after property pointed by pStartProperty
// ----------------------------------------------------------------------
// LPPROPERTY pStartProperty - Points to property to start search
//
// Return: Pointer to property if successfull otherwise NULL.
LPPROPERTY GetNextProperty(LPPROPERTY pStartProperty);

/////////////////////////////////////////////////////////////////////////
// GetPropertyByID
// Returns pointer to first property with specified ID in policy
// ----------------------------------------------------------------------
// LPOLICY pPolicy		- Pointer to policy
// DWORD dwID			- ID of required property
//
// Return: Pointer to first property with spacified ID in policy if successfull otherwise NULL.
LPPROPERTY GetPropertyByID(LPPOLICY pPolicy, DWORD dwID);

/////////////////////////////////////////////////////////////////////////
// GetNextPropertyByID
// Returns pointer to next property with specified ID after property pointed by pStartProperty
// ----------------------------------------------------------------------
// LPPROPERTY pStartProperty - Points to property to start search
//
// Return: Pointer to property if successfull otherwise NULL.
LPPROPERTY GetNextPropertyByID(LPPROPERTY pStartProperty, DWORD ID);

/////////////////////////////////////////////////////////////////////////
// SavePolicyToFile
// Saves policy 'as is' to file
// ----------------------------------------------------------------------
// LPCTSTR pszFileName		- file name
// LPPOLICY pPolicy			- policy 
// BOOL bFree				- free source policy
//
// Return: TRUE if success/FALSE - otherwise
BOOL SavePolicyToFile(LPCTSTR pszFileName, LPPOLICY pPolicy, BOOL bFree);

/////////////////////////////////////////////////////////////////////////
// LoadPolicyFromFile
// Loads policy 'as is' from file
// ----------------------------------------------------------------------
// LPCTSTR pszFileName		- file name
// LPPOLICY pPolicy			- pointer to pointer to policy 
//
// Return: TRUE if success/FALSE - otherwise
BOOL LoadPolicyFromFile(LPCTSTR pszFileName, LPPOLICY* ppPolicy);

/////////////////////////////////////////////////////////////////////////
// Policy creation macros

#define POLICY_BEGIN(dwPolicyID, dwSize)  { \
	HPOLICY hPolicy = CreatePolicy(dwPolicyID, 0, dwSize); \
	if (hPolicy) { \

#define POLICY_BEGIN_EX(dwPolicyID, pPolicy)  { \
	HPOLICY hPolicy = CreatePolicyEx(dwPolicyID, 0, (LPPOLICY)pPolicy); \
	if (hPolicy) { \
	
__inline void POLICY_AddPropStr(HPOLICY hPolicy, DWORD dwPropID, LPCTSTR pszString) 
{
	if (pszString) CreatePropertyBin(hPolicy, dwPropID, (PVOID)pszString, SIZEOFSTR(pszString), FALSE);
}

#define ADD_PROP_STR(dwPropID, pszString)					POLICY_AddPropStr(hPolicy, dwPropID, pszString);

__inline void POLICY_AddPropDWORD(HPOLICY hPolicy, DWORD dwPropID, DWORD dwData)
{
	CreatePropertyBin(hPolicy, dwPropID, (PVOID)&dwData, sizeof(DWORD), FALSE);
}
#define ADD_PROP_DWORD(dwPropID, dwData)					POLICY_AddPropDWORD(hPolicy, dwPropID, dwData);

#define ADD_PROP_BIN(dwPropID, pData, dwSize, bFreeBuffer)	CreatePropertyBin(hPolicy, dwPropID, (PVOID)pData, dwSize, bFreeBuffer);
#define POLICY_END(pResultPolicy)							pResultPolicy = ClosePolicy(hPolicy, NULL); }}
#define POLICY_END_EX(pResultPolicy, pdwSize)				pResultPolicy = ClosePolicy(hPolicy, pdwSize); }}

/////////////////////////////////////////////////////////////////////////
// Compatibility defines

#define PROP_SIZE			sizeof(PROPERTY)					// Size of property's header
#define PROP_SIZE_DWORD		(PROP_SIZE + 4)						// Size of property with stored DWORD data
#define sizeofStrProp		SIZEOF_PROPERTY_STR					// Size of string property size
#define POLICY_SIZE			sizeof(POLICY)						// Size of policy's header
#define POLICY_SIZE_EX		(POLICY_SIZE + PROP_SIZE_DWORD)		// Size of policy's private data (header and policy terminatng property)

#endif