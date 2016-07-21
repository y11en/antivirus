#include "../eventmgr/include/cregkey.h"
#include "p2p.h"

static const tcstring g_DefaultPeer2PeerPaths[] = {
	_T("\\APPLEJUICE\\INCOMING\\"),
	_T("\\BEARSHARE\\SHARED\\"),
	_T("\\EDONKEY2000\\INCOMING\\"),
	_T("\\EMULE\\INCOMING\\"),
	_T("\\GNUCLEUS\\DOWNLOADS\\"),
	_T("\\GROKSTER\\MY GROKSTER\\"),
	_T("\\ICQ\\SHARED FILES\\"),
	_T("\\ICQ\\SHARED FOLDER\\"),
	//_T("\\KAZAA LITE K++\\MY SHARED FOLDER\\"),
	//_T("\\KAZAA LITE\\MY SHARED FOLDERS\\"),
	//_T("\\KAZAA LITE\\MY SHARED FOLDER\\"),
	//_T("\\KAZAA\\MY SHARED FOLDER\\"),
	//_T("\\KMD\\MY SHARED FOLDER\\"),
	_T("\\LIMEWIRE\\SHARED\\"),
	_T("\\MIRC\\DOWNLOAD\\"),
	//_T("\\MORPHEUS\\MY SHARED FOLDER\\"),
	_T("\\OVERNET\\INCOMING\\"),
	_T("\\RAPIGATOR\\SHARE\\"),
	_T("\\SHAREAZA\\DOWNLOADS\\"),
	_T("\\TESLA\\FILES\\"),
	//_T("\\WINMX\\MY SHARED FOLDER\\"),
	_T("\\WINMX\\SHARED\\"),
	_T("\\XOLOX\\DOWNLOADS\\"),
	// generic
	_T("\\MY DOWNLOADS\\"),
	_T("\\MY SHARED FOLDER\\"),
	_T("\\MY SHARED FOLDERS\\"),
};

tcstring p2p_keys1[] = {
	_T("\\KAZAA\\"),
	_T("\\IMESH\\CLIENT\\"),
	_T("\\GROKSTER\\"),
};

tcstring p2p_keys2[] = {
	_T("CLOUDLOAD"),
	_T("LOCALCONTENT"),
	_T("TRANSFER"),
};


cPeer2PeerPaths::cPeer2PeerPaths()
{
	for (int i=0; i<countof(g_DefaultPeer2PeerPaths); i++)
		m_list.add(g_DefaultPeer2PeerPaths[i]);
}

cPeer2PeerPaths::~cPeer2PeerPaths()
{
}

bool cPeer2PeerPaths::SearchPeer2PeerPaths(cEnvironmentHelper* pEnvironmentHelper)
{
	bool bResult = false;
	int i;
	for (i=0; i<countof(p2p_keys1); i++)
	{
		tchar tstrRegPath[0x100];
		tRegistryKey hKeyRoot;
		tRegistryKey hKeyLocal;
		for (int j=0; j<countof(p2p_keys2); j++)
		{
			tstrcpy(tstrRegPath, _T("SOFTWARE"));
			tstrcat(tstrRegPath, p2p_keys1[i]);
			tstrcat(tstrRegPath, p2p_keys2[j]);
			for (int k=0; k<2; k++)
			{
				if (k==0)
					hKeyRoot = crHKEY_LOCAL_MACHINE;
				else
					hKeyRoot = crHKEY_CURRENT_USER;
				if (pEnvironmentHelper->Reg_OpenKey(hKeyRoot, tstrRegPath, &hKeyLocal))
				{
					for (uint32_t v=0; v<0x100; v++)
					{
						tstring tstrValueName;
						uint32_t type;
						
						if (!pEnvironmentHelper->Reg_EnumValue(hKeyLocal, v, &tstrValueName, &type))
							break;
						if (type == crREG_SZ || type == crREG_EXPAND_SZ || type == crREG_MULTI_SZ)
						{
							tstring data = pEnvironmentHelper->Reg_QueryValueStr(hKeyLocal, tstrValueName);
							if (data)
							{
								cPath path(pEnvironmentHelper, data);
								if (AddPeer2PeerPath(path))
									bResult = true;
							}
						}
					}
					pEnvironmentHelper->Reg_CloseKey(hKeyLocal);
				}
			}
		}
	}
	return bResult;
}

bool cPeer2PeerPaths::IsP2PKey(tcstring pwchKeyName, tcstring pwchValueName)
{
	tstring KeyNameUp = tstrdup(pwchKeyName);

	tstrupr(KeyNameUp);
	tcstring p = tstrstr((tcstring)KeyNameUp, _T("\\SOFTWARE\\"));
	if (!p) {
		tstrfree(KeyNameUp);
		return false;
	}
	p+=9;
	for (int i=0; i<countof(p2p_keys1); i++)
	{
		size_t len = tstrlen(p2p_keys1[i]);
		if (0 == tstrncmp(p, p2p_keys1[i], len))
		{
			p += len;
			for (int j=0; j<countof(p2p_keys2); j++)
			{
				if (0 == tstrcmp(p, p2p_keys2[j])) {
					tstrfree(KeyNameUp);
					return true;
				}
			}
			break;
		}
	}
	tstrfree(KeyNameUp);
	return false;
}

bool cPeer2PeerPaths::AddPeer2PeerPath(cPath& path)
{
	cAutoLock lock(m_sync);
	return m_list.add(path.getFull());
}

bool cPeer2PeerPaths::IsPeer2PeerPath(cPath& path)
{
	cAutoLock lock(m_sync);
	return m_list.containSubstrForPath(path.getFull());
}
