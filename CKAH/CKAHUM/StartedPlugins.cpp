#include "stdafx.h"
#include "StartedPlugins.h"

// запущенные плагины
StartedPluginsList g_StartedPlugins;

bool CKAHSTP::IsPluginStarted (CCKAHPlugin::PluginType IfaceType)
{
	StartedPluginsList::CLock lock (&g_StartedPlugins);

	StartedPluginsList::const_iterator i;

	for (i = g_StartedPlugins.begin (); i != g_StartedPlugins.end (); ++i)
	{
		if ((*i)->GetIfaceType () == IfaceType)
			return true;
	}
	return false;
}

bool CKAHSTP::IsPluginResumed (CCKAHPlugin::PluginType IfaceType)
{
	StartedPluginsList::CLock lock (&g_StartedPlugins);

	StartedPluginsList::const_iterator i;

	for (i = g_StartedPlugins.begin (); i != g_StartedPlugins.end (); ++i)
	{
		if ((*i)->GetIfaceType () == IfaceType)
			return (*i)->IsResumed ();
	}
	return false;
}

bool CKAHSTP::MarkResumed (CCKAHPlugin::PluginType IfaceType, bool bResumed)
{
	StartedPluginsList::CLock lock (&g_StartedPlugins);

	StartedPluginsList::iterator i;

	for (i = g_StartedPlugins.begin (); i != g_StartedPlugins.end (); ++i)
	{
		if ((*i)->GetIfaceType () == IfaceType)
		{		
			(*i)->MarkResumed (bResumed);
			return true;
		}
	}
	return false;
}

int CKAHSTP::AddPluginToStarted (const CEnvelope<CCKAHPlugin> &Plugin)
{
	StartedPluginsList::CLock lock (&g_StartedPlugins);

	g_StartedPlugins.push_back (Plugin);
	return g_StartedPlugins.size () - 1;
}

bool CKAHSTP::GetResumedPluginIfaceTypeList (PluginTypeList &list)
{
	StartedPluginsList::CLock lock (&g_StartedPlugins);

	StartedPluginsList::const_iterator i;

	list.clear ();

	for (i = g_StartedPlugins.begin (); i != g_StartedPlugins.end (); ++i)
	{
		if ((*i)->IsResumed ())
			list.push_back ((*i)->GetIfaceType ());
	}

	return true;
}
