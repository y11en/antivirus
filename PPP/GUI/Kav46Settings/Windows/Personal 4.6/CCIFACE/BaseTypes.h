#ifndef BASE_TYPES
#define BASE_TYPES

#include "defs.h"
#include "../../../CommonFiles/CCClient/Policy.h"

#include <string>
#include <vector>

//******************************************************************************
// class CBase (base class)
//******************************************************************************
class CCIFACEEXPORT CBase
{
// Construction
public:
	DWORD m_dwID;
	DWORD m_dwType;

	CBase();
	CBase(LPCSTR sName, DWORD dwType);
	virtual ~CBase ();

	bool Assign(CBase* pObject);

	int StoreToFile(LPCSTR sFileName) const;
	int LoadFromFile(LPCTSTR pszFilePath);
	virtual int Store(LPPOLICY* ppPolicy) const;
	virtual int Load(LPPOLICY pPolicy);
	
	//! неэффективное сравнение через сравнение сериализованных буферов
	bool IsEqual(const CBase &that) const;

	LPCSTR GetFullName() const;
	LPCSTR GetName() const;
	void SetName(LPCSTR szName);

	const std::string &GetNameStdStr () const;

protected:
	std::string m_sName;
};

////////////////////////////////////////////////////////////////////////////////////////
// class CContainer

template class CCIFACEEXPORT std::vector<CBase *>;

class CCIFACEEXPORT CContainer : public CBase
{
public:
	CContainer();
	virtual ~CContainer();

	int GetSize() const;

	int Add(CBase *pObject);
	void InsertAt(int nIndex, CBase *pObject);
	
	BOOL RemoveByName(LPCTSTR pszName);
	int RemoveAt(int i, BOOL bFreeItem = TRUE);
	void RemoveAll();
	
	CBase* GetItem(int i) const;
	CBase* GetItemByName(LPCTSTR pszName) const;

	CBase& operator[](int n);

	virtual int Store(LPPOLICY* ppPolicy) const;
	virtual int Load(LPPOLICY pPolicy);

	virtual CBase* NewItem(PVOID pData);
	
	CContainer &operator=(const CContainer &that);

private:
	std::vector<CBase *> m_Store;
};

#endif