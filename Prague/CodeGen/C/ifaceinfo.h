// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// IBaseInfo wrapper class

class IBaseInfo : public COleDispatchDriver
{
public:
	IBaseInfo() {}		// Calls COleDispatchDriver default constructor
	IBaseInfo(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IBaseInfo(const IBaseInfo& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	unsigned long GetDWORDAttr(unsigned long aid);
	CString GetStrAttr(unsigned long aid);
};
/////////////////////////////////////////////////////////////////////////////
// IFaceInfo wrapper class

class IFaceInfo : public COleDispatchDriver
{
public:
	IFaceInfo() {}		// Calls COleDispatchDriver default constructor
	IFaceInfo(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IFaceInfo(const IFaceInfo& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	// method 'InitByData' not emitted because of invalid return type or parameter type
};
