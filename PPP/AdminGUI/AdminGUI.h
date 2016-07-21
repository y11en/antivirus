// AdminGUI.h
//

#pragma once

#include "ListsView.h"
#include "SettingsDlgs.h"
#include <RootSink.h>
#include <plugin\p_win32_reg.h>

//////////////////////////////////////////////////////////////////////////
// CRootSinkAdm

class CRootSinkAdm : public CKav6RootSink
{
public:
	CRootSinkAdm() : m_pCrHelp(NULL){}

	typedef CKav6RootSink TBase;

protected:
	bool      OnGetHelpId(tWORD &nHelpId);
	
	bool      IsSerializableHolder(const cSerDescriptor* pDesc, const cSerDescriptorField * pFld, LPCSTR strType, CustomHolderData ** ppHolderData);
	bool      ResolveSerializable(bool bToStruct, const cSerDescriptorField* pFld, LPCSTR strType, CustomHolderData * pHolderData, cSerializable* pStruct, tString& strResult, CBindProps* pProps);

	cCryptoHelper* GetCryptoHelper(bool& bSimple);

private:
	cCryptoHelper* m_pCrHelp;

protected:
	typedef std::pair<tWORD, tWORD> tHelpPair;
	typedef std::vector<tHelpPair> tHelpPairs;

	tHelpPairs     m_mapCtxHelpIds;
};

//////////////////////////////////////////////////////////////////////////
// CLockButtonHolder

#define HOLDERID_FIELDS                     "fld"
#define HOLDERID_LEVEL_FIELDS               "levfld"
#define FIELDID_LEVEL                       "level"

struct CLockButtonHolder : public CustomHolderData, public CSectionBindingCb
{
	CLockButtonHolder(bool bLockLevel = false) : m_pRoot(NULL), m_pBinding(NULL), m_pProfile(NULL), m_pProfilesList(NULL), m_bLockLevel(false) {}
	~CLockButtonHolder() { if( m_pBinding ) m_pBinding->Destroy(); }

	void   Init(CItemPropVals& pParams, CRootItem * pRoot);
	void   Resolve(bool bToStruct, cSerializable * pStruct, tString& strResult);
	void   Bind();
	void   Apply(bool bToStruct, tString& strResult);
	void   AddSource(cSerializable * pStruct, LPCSTR strSect = NULL);

	bool   OnAddDataSection(LPCSTR strSect);

	struct MaskItem
	{
		MaskItem() : m_pStr(0) {}
		
		tPTR     m_pStr;
		tString  m_strStructFldPath;
		cBitMask m_nMask;
	};
	
	CRootItem *              m_pRoot;
	CSectionBinding *        m_pBinding;
	std::vector<std::string> m_Fields;
	std::vector<MaskItem>    m_Masks;
	CProfile *               m_pProfile;
	CProfile *               m_pProfilesList;
	bool                     m_bLockLevel;
	MaskItem                 m_LevelMaskItem;
};