/////////////////////////////////////////////////////////////////////////////////////
//
// Load Component DLL
//
/////////////////////////////////////////////////////////////////////////////////////

#include <AVPcmpid.h>

/////////////////////////////////////////////////////////////////////////////
// COMP_LoadDLL - Function loads DLL from specified folder or (if there is not DLL)
// from default folders and imports all functions
// --------------------------------------------------------------------------
// 
// return:		TRUE - if successfull, FALSE - otherwise.
BOOL COMP_LoadDLL(LPCTSTR pszDllPath);

/////////////////////////////////////////////////////////////////////////////
// COMP_FreeDLL - Function unloads DLL from memory
void COMP_FreeDLL();

//___________________________________________________________________________


/////////////////////////////////////////////////////////////////////////////
// DLL Exported functions
/////////////////////////////////////////////////////////////////////////////

extern COLLECT_ABOUT_INFO_H COMP_CollectAboutInfoH;
extern COLLECT_ABOUT_INFO_S COMP_CollectAboutInfoS;

//___________________________________________________________________________
