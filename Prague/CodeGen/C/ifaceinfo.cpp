// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "stdafx.h"
#include "ifaceinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// IBaseInfo properties

/////////////////////////////////////////////////////////////////////////////
// IBaseInfo operations

unsigned long IBaseInfo::GetDWORDAttr(unsigned long aid)
{
	unsigned long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		aid);
	return result;
}

CString IBaseInfo::GetStrAttr(unsigned long aid)
{
	CString result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x2, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms,
		aid);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IFaceInfo properties

/////////////////////////////////////////////////////////////////////////////
// IFaceInfo operations
