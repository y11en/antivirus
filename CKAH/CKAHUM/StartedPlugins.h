#if !defined(AFX_STARTEDPLUGINS_H__F0FB7134_E13D_4CD5_9E7B_271EF694523B__INCLUDED_)
#define AFX_STARTEDPLUGINS_H__F0FB7134_E13D_4CD5_9E7B_271EF694523B__INCLUDED_

#include "CKAHManifest.h"
#include <Stuff/thread.h>
#include <stuff/cpointer.h>

class StartedPluginsList : public std::list <CEnvelope<CCKAHPlugin> >, public CSync
{
public:
	using CSync::CLock;
};

namespace CKAHSTP
{
	bool IsPluginStarted (CCKAHPlugin::PluginType IfaceType);
	bool IsPluginResumed (CCKAHPlugin::PluginType IfaceType);

	typedef std::vector<CCKAHPlugin::PluginType> PluginTypeList;
	bool GetResumedPluginIfaceTypeList (PluginTypeList &list);
	bool MarkResumed (CCKAHPlugin::PluginType IfaceType, bool bResumed);
	int AddPluginToStarted (const CEnvelope<CCKAHPlugin> &Plugin);
}


#endif // !defined(AFX_STARTEDPLUGINS_H__F0FB7134_E13D_4CD5_9E7B_271EF694523B__INCLUDED_)
