//////////////////////////////////////////////////////////////////////
// Policy.h Header file. 

#ifndef POLICY_H
#define POLICY_H

#include <cstring>
#include <cwchar>

namespace KLPOL {

//////////////////////////////////////////////////////////////////////
// Predefined property IDs (0 - 999)
const long PROPID_DONE = 0;		// Policy terminatng property

#ifdef _MSC_VER
# pragma warning (disable : 4200)
# pragma pack(push, MyAlign, 1)
#endif

////////////////////////////////////////////////////////////////////////////////
// Policy's ID
typedef long POLICY_ID;

////////////////////////////////////////////////////////////////////////////////
// Policy's Header
typedef struct POLICY
{
	POLICY_ID ID;	// Policy ID
	long Size;		// Aggregate size of policy (properties and policy's private data (includes policy terminatng property))
	long Count;	// Number of policy's properties
	char Data[];	// Pointer to policy's data
} *LPPOLICY, *PPOLICY;

////////////////////////////////////////////////////////////////////////////////
// Property's Header
typedef struct PROPERTY
{
	long ID;		// Property ID
	long Size;		// Only size of property's data (without private data)
	char Data[];	// Pointer to property's data
} *LPPROPERTY, *PPROPERTY;

#define SIZEOFSTR(pszString)	((wcslen(pszString) + 1) * sizeof(wchar_t))

////////////////////////////////////////////////////////////////////////////////
// Sizes of policy's elements

#define SIZEOF_PROPERTY					(sizeof(PROPERTY))
#define SIZEOF_PROPERTY_DWORD			(SIZEOF_PROPERTY + sizeof(long))		// Size of long property
#define SIZEOF_PROPERTY_DWORD_N(n)		(SIZEOF_PROPERTY_DWORD * (n))			// Size of 'n' long property
#define SIZEOF_PROPERTY_BIN(nDataSize)	(SIZEOF_PROPERTY + nDataSize)			// Size of Binary property with nDataSize size
__inline long SIZEOF_PROPERTY_STR(const wchar_t* pszString)							// Size of string property
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
	bool bRealloc;
	LPPOLICY lpPolicy;
	char* lpCurPos;
} *HPOLICY;

#ifdef _MSC_VER
# pragma pack(pop, MyAlign)
#endif

/////////////////////////////////////////////////////////////////////////
//
// Functions for creating policy
//
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// CreatePolicy
// Create handle to compose policy
// ----------------------------------------------------------------------
// long nID			- Policy's ID
// long nSize			- Total size of properties to store in policy. 
//						  If Size is 0 then policy's memory will be reallocated each time new property adds.
//
// Return: Policy handle if successfull otherwise NULL.
HPOLICY CreatePolicy(long nID, long nSize);

/////////////////////////////////////////////////////////////////////////
// CreatePolicyEx
// Create handle to compose policy. Uses memory buffer pointed lpPolicy as policy's memory
// ----------------------------------------------------------------------
// long nID				- Policy's ID
// long nMask			- Reserved 
// LPPOLICY pPolicy	- Pointer to preallocated memory to store policy's properties 
//						  Memory block must be enought to store :
//								1. Policy's header - POLICY_SIZE
//								2. Policy's properties
//								3. Terminating property - PROP_SIZE_DWORD
//						  
// Return: Policy handle if successfull otherwise NULL.
HPOLICY CreatePolicyEx(long nID, LPPOLICY pPolicy);

/////////////////////////////////////////////////////////////////////////
// CreatePropertyStr
// Adds string property to opened policy
// ----------------------------------------------------------------------
// HPOLICY hPolicy		- Policy handle
// long nPropID		- Property's ID
// const wchar_t* pszString	- ASCIIZ String to store in property
// bool bFreeBuffer		- If true source string memory block will be deallocated on exit
void CreatePropertyStr(HPOLICY hPolicy, long PropID, const wchar_t* pszString, bool bFreeBuffer = false);

/////////////////////////////////////////////////////////////////////////
// CreatePropertyBin
// Adds binary data property to opened policy
// ----------------------------------------------------------------------
// HPOLICY hPolicy		- Policy handle
// long nPropID		- Property's ID
// void* pData			- Pointer to binary data to store in property
// long nSize			- Size of binary data
// bool bFreeBuffer		- If true source memory block will be deallocated on exit
void CreatePropertyBin(HPOLICY hPolicy, long nPropID, void* pData, long nSize, bool bFreeBuffer = false);

/////////////////////////////////////////////////////////////////////////
// CreatePropertyDWORD
// Adds long property to opened policy
// ----------------------------------------------------------------------
// HPOLICY hPolicy		- Policy handle
// long nPropID		- Property's ID
// long nData			- long data to store in property
void CreatePropertyDWORD(HPOLICY hPolicy, long PropID, long nData);

/////////////////////////////////////////////////////////////////////////
// ClosePolicy
// Closes hPolicy handle
// ----------------------------------------------------------------------
// HPOLICY hPolicy		- Policy handle
// LPDWORD pnDataSize	- Returns total size of properties in policy
//
// Return: Pointer to created policy if successfull otherwise NULL.
LPPOLICY ClosePolicy(HPOLICY hPolicy, long* pnDataSize);

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
// long nID			- ID of required property
//
// Return: Pointer to first property with spacified ID in policy if successfull otherwise NULL.
LPPROPERTY GetPropertyByID(LPPOLICY pPolicy, long nID);

/////////////////////////////////////////////////////////////////////////
// GetNextPropertyByID
// Returns pointer to next property with specified ID after property pointed by pStartProperty
// ----------------------------------------------------------------------
// LPPROPERTY pStartProperty - Points to property to start search
//
// Return: Pointer to property if successfull otherwise NULL.
LPPROPERTY GetNextPropertyByID(LPPROPERTY pStartProperty, long ID);

/////////////////////////////////////////////////////////////////////////
// SavePolicyToFile
// Saves policy 'as is' to file
// ----------------------------------------------------------------------
// const wchar_t* pszFileName		- file name
// LPPOLICY pPolicy			- policy 
// bool bFree				- free source policy
//
// Return: true if success/false - otherwise
bool SavePolicyToFile(const wchar_t* pszFileName, LPPOLICY pPolicy, bool bFree);

/////////////////////////////////////////////////////////////////////////
// LoadPolicyFromFile
// Loads policy 'as is' from file
// ----------------------------------------------------------------------
// const wchar_t* pszFileName		- file name
// LPPOLICY pPolicy			- pointer to pointer to policy 
//
// Return: true if success/false - otherwise
bool LoadPolicyFromFile(const wchar_t* pszFileName, LPPOLICY* ppPolicy);

/////////////////////////////////////////////////////////////////////////
// Policy creation macros

#define POLICY_BEGIN(nPolicyID, nSize)  { \
	HPOLICY hPolicy = CreatePolicy(nPolicyID, nSize); \
	if (hPolicy) { try {\

#define POLICY_BEGIN_EX(nPolicyID, pPolicy)  { \
	HPOLICY hPolicy = CreatePolicyEx(nPolicyID, (LPPOLICY)pPolicy); \
	if (hPolicy) { \
	
__inline void POLICY_AddPropStr(HPOLICY hPolicy, long nPropID, const wchar_t* pszString) 
{
	if (pszString) CreatePropertyBin(hPolicy, nPropID, (void*)pszString, SIZEOFSTR(pszString), false);
}

#define ADD_PROP_STR(nPropID, pszString)					POLICY_AddPropStr(hPolicy, nPropID, pszString);

__inline void POLICY_AddPropDWORD(HPOLICY hPolicy, long nPropID, long nData)
{
	CreatePropertyBin(hPolicy, nPropID, (void*)&nData, sizeof(long), false);
}
#define ADD_PROP_DWORD(nPropID, nData)					POLICY_AddPropDWORD(hPolicy, nPropID, nData);

#define ADD_PROP_BIN(nPropID, pData, nSize, bFreeBuffer)	CreatePropertyBin(hPolicy, nPropID, (void*)pData, nSize, bFreeBuffer);
#define POLICY_END(pResultPolicy) }\
catch(...){ \
	CancelPolicy(hPolicy); \
	throw; \
} \
pResultPolicy = ClosePolicy(hPolicy, NULL); }}

};

#endif // POLICY_H

// Local Variables:
// mode: C++
// End:
