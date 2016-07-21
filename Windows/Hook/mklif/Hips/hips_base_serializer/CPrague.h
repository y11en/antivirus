#include <prague.h>

#include <pr_loadr.h>
#include <iface\i_root.h>
//#include <iface\i_pfindr.h>
//#include <iface\i_ifenum.h>
#include <iface\i_string.h>
#include <iface\i_os.h>

class _CPrague
{
public:
	void*               m_hLoader;
	hROOT               m_hRoot;
	tDWORD              m_dwInitFlags;
	
	_CPrague();
	_CPrague(tCHAR* pchLoadPath);
	~_CPrague();
	tBOOL               LoadPrague();
	tBOOL               LoadPrague(tCHAR* pchLoadPath);
	void                UnloadPrague();

private:
	void                Init();
	tCHAR*              m_pchLoadPath;
	PragueLoadFuncEx    m_plf;
	PragueUnloadFunc    m_puf;
};
