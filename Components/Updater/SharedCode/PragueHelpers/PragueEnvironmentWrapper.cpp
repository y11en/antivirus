#include "PragueEnvironmentWrapper.h"

#include <Prague/iface/i_root.h>

extern hROOT g_root;

PRAGUE_HELPERS::PragueEnvironmentWrapper::EnvironmentCacheItem::EnvironmentCacheItem(const bool result, const KLUPD::NoCaseString &output)
: m_result(result),
  m_output(output)
{
}

bool PRAGUE_HELPERS::PragueEnvironmentWrapper::expandEnvironmentString(const KLUPD::NoCaseString &input,
                                                                       KLUPD::NoCaseString &output,
                                                                       const KLUPD::StringParser::ExpandOrder &expandOrder)
{
    EnvironmentCache::const_iterator cachedItem = m_environmentCache.find(input);
    if(cachedItem != m_environmentCache.end())
    {
        output = cachedItem->second.m_output;
        return cachedItem->second.m_result;
    }

    // append delimiters
    const KLUPD::NoCaseString inputWithDelimiters = KLUPD::NoCaseString(L"%") + input + L"%";

    // try to expand string by sending message to product
    cStringObj result(inputWithDelimiters.toWideChar());

	if(expandOrder == KLUPD::StringParser::bySystemFirst)
		g_root->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_SYSTEM_ONLY_STRING, (hOBJECT)cAutoString(result), 0, 0);

    if(PR_SUCC(g_root->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(result), 0, 0)))
    {
        output = result.data();
        m_environmentCache[input] = EnvironmentCacheItem(true, output);
        return true;
    }
    m_environmentCache[input] = EnvironmentCacheItem(false);
    return false;
}
