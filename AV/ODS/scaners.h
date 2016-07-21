// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- task.cpp
// -------------------------------------------

#include "process.h"

// -------------------------------------------

class CFileScanner : virtual public CObjectScanner
{
public:
	virtual void Process(bool bPreProcess, bool bIsOriginalObject);

protected:
	void CheckOpenError(tERROR error);
};

// -------------------------------------------

class CFolderScanner : public CFileScanner
{
public:
	virtual void Process(bool bPreProcess, bool bIsOriginalObject);

protected:
	void ProcessFolder(hObjPtr hFilePtr, cStrObj& strPathFrom);

private:
	cStrObj      m_strMask;
};

// -------------------------------------------

class CSectorsScanner : virtual public CObjectScanner
{
public:
	virtual void Process(bool bPreProcess, bool bIsOriginalObject);

protected:
	tERROR ProcessPhysicalDisks();

private:
	cStringObjVector m_vecDrives;
};

// -------------------------------------------

class CBTDisk : virtual public CObjectScanner
{
public:
	virtual void Process(bool bPreProcess, bool bIsOriginalObject);
};

// -------------------------------------------

class CDriveScanner : public CSectorsScanner, public CFolderScanner, public CBTDisk
{
public:
	virtual void Process(bool bPreProcess, bool bIsOriginalObject);
	bool ReinitCheck(tDWORD nMask);
};

// -------------------------------------------

class CDrivesScanner : public CDriveScanner
{
public:
	virtual void Process(bool bPreProcess, bool bIsOriginalObject);
};

// -------------------------------------------

class CMailSystemScanner : virtual public CObjectScanner
{
public:
	virtual void Process(bool bPreProcess, bool bIsOriginalObject);

private:
	tERROR ProcessMailObjects(hObjPtr oe);
	tERROR ProcessMailSystem(tPID pid);
};

// -------------------------------------------

class CStartUpObjectsScanner : public CFileScanner
{
public:
	virtual void Process(bool bPreProcess, bool bIsOriginalObject);

protected:
	tERROR ProcessFile(tWCHAR* strName, tDWORD nProgress);
	tERROR ProcessHost(tWCHAR* strName, tDWORD nAction);

	static tERROR pr_call _MsgReceive(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT recieve_point, tPTR pbuff, tDWORD* blen);

private:
	cStartUpEnum2* m_hStartupEnum;
	tDWORD         m_nInitObjCount;
	tPROPID        m_nCounterProp;
};

// -------------------------------------------

class CSystemRestoreScanner : public CFolderScanner
{
public:
	virtual void Process(bool bPreProcess, bool bIsOriginalObject);
};

// -------------------------------------------

class CMemoryScanner : public CFileScanner
{
public:
	virtual void Process(bool bPreProcess, bool bIsOriginalObject);

private:
	tERROR ProcessMemoryModule(hObjPtr oe);
	tERROR ProcessMemoryFile(hObjPtr oe);
	tERROR ProcessDOSMemory();

private:
	tDWORD	m_nModuleNumber;
};

// -------------------------------------------

class CMyComputerScanner :
	public CMemoryScanner, 
	public CStartUpObjectsScanner, 
	public CMailSystemScanner, 
	public CDrivesScanner
{
public:
	virtual void Process(bool bPreProcess, bool bIsOriginalObject);
};

// -------------------------------------------

class CQuarantineScanner : virtual public CObjectScanner
{
public:
	virtual void Process(bool bPreProcess, bool bIsOriginalObject);
};

// -------------------------------------------
