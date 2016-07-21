// FWRules.h: interface for the CAHFWAppRule2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FWRULES_H__F419D1D6_6B4A_4610_B4E5_51406C849024__INCLUDED_)
#define AFX_FWRULES_H__F419D1D6_6B4A_4610_B4E5_51406C849024__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <prague.h>
#include <pr_cpp.h>
#include <structs/s_gui.h>
//#include <list>

/*class CFwPresets;
extern CFwPresets g_FwPresets;

//////////////////////////////////////////////////////////////////////////
// CFwPresets

class CFwPresets : public cFwAppRulePresets
{
public:
	CFwPresets() : m_bInited(false) {}

	bool                 Get();
	void                 Clear();

	void                 GetGeneralPresets(cVector<cFwAppRulePreset *> &Presets, bool bAll = true);
	void                 GetPresetsByApp(cStringObj &sAppName, cVector<cFwAppRulePreset *> &Presets);
	void                 GetPresetsByActivity(cAHFWAppEventReport *pAppEvent, cVector<cFwAppRulePreset *> &Presets);
	cFwAppRulePreset *   GetPreset(int idx) { return (size_t)idx < size() ? &at(idx) : NULL; }
	
	bool                 CheckPresetActivity(cFwAppRulePreset *pPreset, cAHFWAppEventReport *pAppEvent);
	bool                 CheckPorts(cVector<cFWPort> &aPorts, tWORD nPort);

	static bool          LoadPresets(const tCHAR* strFile, cFwAppRulePresets &aPresets);
	static void          ExportRule(tString &strBuff, cSerializable *pRule);

	static LPCSTR        Localize(CRootItem *pRoot, LPCSTR strPresetId, LPCSTR strPrefix = "fwpn");
	static LPCWSTR       LocalizeW(CRootItem *pRoot, LPCSTR strPresetId, LPCSTR strPrefix = "fwpn");

protected:
	bool                 m_bInited;
};*/

//////////////////////////////////////////////////////////////////////////
// CFwRuleUtils

class CFwRuleUtils
{
public:
// 	static void          App2AppEx(cFwAppRules &aRules, cFwAppRulesEx &aRulesEx);
// 	static void          AppEx2App(cFwAppRulesEx &aRulesEx, cFwAppRules &aRules);
// 	static cFwAppRule *  FindRule(cFwAppRules &aRules, cStringObj &sAppName, tBYTE *pHash, cStringObj &sCmdLine, tBOOL bUseCmdLine, tBOOL bAddIfNotExist = cFALSE);
// 	static void          ApplyPreset(cFwAppRule &AppRule, cFwAppRulePreset *pPreset, tBOOL bBlocking);
// 	static void          MakeKnownIpRule(cFwAppRule &AppRule, const cIP& RemoteAddress, tBOOL bBlocking);
// 
// 	static int           CompareIP(const cIP& nIP1, const cIP& nIP2);
// 	static int           CompareAddrs(cFWAddress &Addr1, cFWAddress &Addr2);
// 	static int           ComparePorts(cFWPort &Port1, cFWPort &Port2);
// 	
// 	static bool          IsIp(LPCTSTR strIp, tBYTE Version = 0);
// 	static cIP           Str2Ip(LPCTSTR strIp, tBYTE Version = 0);
	static tString       Ip2Str(const cIP& Ip, tBOOL bMask = cFALSE );
	
// 	static tString       Address2Str(const cFWAddress &Addr);
// 	static tString       Port2Str(const cFWPort &Port);
// 	static tString       GetAddrListStr(const cVector<cFWAddress> &Addrs);
// 	static tString       GetPortListStr(const cVector<cFWPort> &Ports);
	static bool          ParsePortListStr(LPCTSTR sPorts, cFwPorts &aPorts);

// 	static bool          CheckRule(cAHFWAppEventReport &AppEvent, cFwAppRule &AppRule);
// 	static bool          CheckRule(cAHFWAppEventReport &AppEvent, cFwBaseRule &AppRule);
};


/*

//-----------------------------------------------------------------------------

void CheckRuleContainment(cAHFWBaseRuleEx *pRule, cVector<CAHFWPortRule> *pPortRules, cVector<CAHFWAppRule> *pAppRules);

//-----------------------------------------------------------------------------

// класс CRuleContainer содержит список многомерных пр€моугольников, определ€ющих правило,
// а также базовые функции работы с ним
class CRuleContainer
{
public:
	enum 
	{
		rciDirection = 0,
		rciProto = 1,
		rciRemoteAddr = 2,
		rciLocalAddr = 3,
		rciRemotePort = 4,
		rciLocalPort = 5,
		rciIcmpCode = 6,
		rciDimCount = 7
	};

	// это многомерный пр€моугольик, определ€емый начальной и конечной точкой
	// по каждой координате
	// в нем определена функци€ вычитани€ из одного пр€моугольника другого,
	// результатом которой €вл€етс€ список таких же пр€моугольников
	struct CRuleContainerItem
	{
		CRuleContainerItem()
		{
			memset ( this, 0, sizeof ( *this ) );

			// установим конечные точки
			m_Vector[rciDirection][1] = 1;
			m_Vector[rciProto][1] = 0xFF;
			m_Vector[rciRemoteAddr][1] = 0xFFFFFFFF;
			m_Vector[rciLocalAddr][1] = 0xFFFFFFFF;
			m_Vector[rciRemotePort][1] = 0xFFFF;
			m_Vector[rciLocalPort][1] = 0xFFFF;
			m_Vector[rciIcmpCode][1] = 0xFFFF;
		}

		DWORD m_Vector[rciDimCount][2];	// первый индекс - измерение
										// второй индекс - начальна€ / конечна€ точка
	};
	typedef std::list<CRuleContainerItem> CRuleContainerItemList;
	
public:
	CRuleContainer(cSerializable *pRule);

	bool            Subtract(const CRuleContainer & Container); // true, если Container имеет пересечение с *this
	bool            Intersect(const CRuleContainer & Container);
	
	bool            IsEmpty() const {return m_Items.empty();}
	cSerializable * GetRule() const {return m_pRule;}

protected:
	void            CreateFromPacketRule(cAHFWPortRuleEx *pRule);
	void            CreateFromAppRule(cAHFWAppRuleEx *pRule);
	void            CreateFromRuleElement(
		CRuleContainerItem &Item,
		cVector<cFWAddress> *pRemAddrs,
		cVector<cFWAddress> *pLocAddrs,
		cVector<cFWPort> *pRemPorts,
		cVector<cFWPort> *pLocPorts);

	cSerializable *                    m_pRule;
	tString                            m_strAppName;   // "" (пусто) дл€ всех

	CRuleContainerItemList             m_Items;
};

typedef std::list<CRuleContainer> CRuleContainerList;
*/

#endif // !defined(AFX_FWRULES_H__F419D1D6_6B4A_4610_B4E5_51406C849024__INCLUDED_)
