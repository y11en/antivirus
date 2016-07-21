#ifndef PAGECFG_H
#define PAGECFG_H

//////////////////////////////////////////////////////////////////////////////

#include <CCClient\Policy.h>

//////////////////////////////////////////////////////////////////////////////

#define PROPERTYID_TASK_SETTINGS		0xFFFFFFAA
#define PROPERTYID_DATA							0xFFFFFFAA

//////////////////////////////////////////////////////////////////////////////
// Page cfg functions

//////////////////////////////////////////////////////////////////////////
// CfgPageGetPageCount - Returns Pages count available for this call type
//
// DWORD dwType			Call type (see General)
// return				Pages count
typedef int (*tfnCfgPageGetPageCount)(DWORD dwType);

/////////////////////////////////////////////////////////////////////////
// CfgPageCreatePage - Creates page by spacified call type and returns it HWND
//
// HWND hWndParent		Parent window handle
// DWORD n				Page number
// DWORD dwType			Call type (see General)
// return				Created Page Handle (HWND)
typedef HWND (*tfnCfgPageCreatePage)(HWND hWndParent, DWORD n, DWORD dwType);

//////////////////////////////////////////////////////////////////////////
// CfgPageDeletePage - Deletes spacified page
//
// HWND hPage			Page to delete
// return				Reserved - Must be 0
typedef int (*tfnCfgPageDeletePage)(HWND hPage);

//////////////////////////////////////////////////////////////////////////
// CfgPageGetDataFromPages - Gets data from all pages or from hPage only to previously
// allocated memory pointed by lpPolicy. It Calls twice
// 1. lpPolicy = NULL - simple returns size of data policy
// 2. lpPolicy = allocated destinaton buffer - Writes data policy to buffer pointed by lpPolicy
// 
// LPPOLICY lpPolicy	Points to buffer to write data policy. If NULL function simple returns
//						size of data policy
// HWND* lphPage		Array of Pages to get data from (see General)
// return				Size of data policy or 0 if error occured
typedef int (*tfnCfgPageGetDataFromPages)(LPPOLICY lpPolicy, HWND* lphPage);

//////////////////////////////////////////////////////////////////////////
// CfgPageSetDataToPages - Sets data to specified page or pages
//
// LPPOLICY lpPolicy	Points to data policy
// HWND* lphPage		Array of Pages to Set data to (see General)
// return				0 - if no errors, otherwise - 1
typedef int (*tfnCfgPageSetDataToPages)(LPPOLICY lpPolicy, HWND* lphPage);

//////////////////////////////////////////////////////////////////////////
// CfgPageGetHelpInfo - Gets information required to call help information for this page
//
// char* pszHelpFile	Pointer to buffer that receives Help file name
// DWORD* pdwHelpID		Pointer to DWORD buffer that receives Help ID
// HWND hPage			Handle of page
// return				0 - if no errors, otherwise - 1
typedef int (*tfnCfgPageGetHelpInfo)(char* pszHelpFile, DWORD* pdwHelpID, HWND hPage);

/////////////////////////////////////////////////////////////////////////
// CfgPageGetDefaultPolicy - Returns default policy for specified page type
//
// DWORD n				Page number
// DWORD dwType			Call type (see General)
// LPPOLICY pPolicy		Buffer to receive policy data 
//						(if NULL - function simple counts size of policy, otherwize stores data to pPolicy)
// return				Size of data policy or 0 if error occured
typedef DWORD (*tfnCfgPageGetDefaultPolicy)(DWORD n, DWORD dwType, LPPOLICY pPolicy);

typedef int (*tfnCfgPageGetPagesDataToObject)(HWND* phPage);
typedef int (*tfnCfgPageSetPagesDataFromObject)(LPVOID pObject, HWND* phPage);

#endif