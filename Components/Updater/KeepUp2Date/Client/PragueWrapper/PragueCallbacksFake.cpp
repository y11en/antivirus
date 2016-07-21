#include <sstream>
#include "PragueCallbacksFake.h"

///////////////////////////////////////////////////////////////////////////////
PragueCallbacksFake::PragueCallbacksFake()
    : m_config(0, 0, KLUPD::NoCaseString())
{
	m_config.UpdaterListFlags.update_bases = true;
	m_config.UpdaterListFlags.apply_urgent_updates = true;
	m_config.UpdaterListFlags.automatically_apply_available_updates = true;
}

///////////////////////////////////////////////////////////////////////////////
KLUPD::UpdaterConfiguration& PragueCallbacksFake::updaterConfiguration()
{
	return m_config;
}

///////////////////////////////////////////////////////////////////////////////
bool PragueCallbacksFake::checkFilesToDownload(KLUPD::FileVector&, const KLUPD::NoCaseString&, const bool)
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////
KLUPD::FileInfo PragueCallbacksFake::getPrimaryIndex(const bool retranslationMode)
{
	const wchar_t* PRIMARY_INDEX_FILE_NAME = L"u0607g.xml";
	const wchar_t* INDEX_RELATIVE_URL_PATH = L"index/6";

	KLUPD::FileInfo primIndex(PRIMARY_INDEX_FILE_NAME, 
		INDEX_RELATIVE_URL_PATH, KLUPD::FileInfo::index, true);
	primIndex.m_localPath = productFolder(retranslationMode);

	return primIndex;
}

///////////////////////////////////////////////////////////////////////////////
KLUPD::FileInfo PragueCallbacksFake::getUpdaterConfigurationXml()
{
	return KLUPD::FileInfo();
}

///////////////////////////////////////////////////////////////////////////////
KLUPD::FileInfo PragueCallbacksFake::getSitesFileXml()
{
	return KLUPD::FileInfo();
}

///////////////////////////////////////////////////////////////////////////////
bool PragueCallbacksFake::expandEnvironmentString(  const KLUPD::NoCaseString& input, 
                                                    KLUPD::NoCaseString& output, 
                                                    const KLUPD::StringParser::ExpandOrder& expandOrder )
{
    return m_pragueEnvironmentWrapper.expandEnvironmentString(input, output, expandOrder);
}
