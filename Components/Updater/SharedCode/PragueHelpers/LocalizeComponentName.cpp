#include "LocalizeComponentName.h"

#include "../../KeepUp2Date/Client/helper/stdinc.h"
#include "../../KeepUp2Date/Client/include/cfg_updater.h"
#include "../../KeepUp2Date/Client/helper/local_file.h"

#include <Prague/iface/e_loader.h>


PRAGUE_HELPERS::ComponentLocalization::ComponentLocalization(const tDWORD index, const tDWORD parent,
            const cStrObj &componentName, const cStrObj &componentIdentificator, const cStrObj &componentComment)
 : m_index(index),
   m_parent(parent),
   m_componentName(componentName),
   m_componentIdentificator(componentIdentificator),
   m_componentComment(componentComment)
{
}


bool PRAGUE_HELPERS::getLocalizedComponentFromString(const cStrObj &p_str, ComponentLocalization &filter)
{
    cStrObj strTemp = p_str;

    tDWORD dwPos = strTemp.find_first_of(COMPONENT_DELIMITER);
    if(dwPos == cSTRING_EMPTY_LENGTH)
        return false;

    filter.m_parent = pr_strtol(static_cast<const tCHAR *>(strTemp.c_str(cCP_ANSI)), 0, 10);

    strTemp = strTemp.substr(++dwPos);

    dwPos = strTemp.find_first_of(COMPONENT_DELIMITER);
    if(dwPos == cSTRING_EMPTY_LENGTH)
        return false;

    filter.m_componentIdentificator = strTemp.substr(0, dwPos);

    strTemp = strTemp.substr(++dwPos);

    dwPos = strTemp.find_first_of(COMPONENT_DELIMITER);
    if(dwPos == cSTRING_EMPTY_LENGTH)
        return false;

    filter.m_componentName = strTemp.substr(0, dwPos);

    strTemp = strTemp.substr(++dwPos);

    dwPos = strTemp.find_first_of(COMPONENT_DELIMITER);
    if(dwPos == cSTRING_EMPTY_LENGTH)
        dwPos = strTemp.length();

    filter.m_componentComment = strTemp.substr(0, dwPos);
    return true;
}

bool PRAGUE_HELPERS::getLocalizedComponentsInformationFromFile(const KLUPD::NoCaseString &componentLocalizationFileName,
                                                               Components &components, KLUPD::Log *pLog)
{
    std::vector<unsigned char> buffer;
    KLUPD::LocalFile componentLocalizationFile(componentLocalizationFileName);
    const size_t fileSize = componentLocalizationFile.size();
    if(!fileSize)
        return false;

    const KLUPD::CoreError readLocalizationFileResult = componentLocalizationFile.read(buffer);
    if(!KLUPD::isSuccess(readLocalizationFileResult))
    {
        TRACE_MESSAGE3("Failed read localization file '%S', result '%S'",
            componentLocalizationFileName.toWideChar(), KLUPD::toString(readLocalizationFileResult).toWideChar());
        return false;
    }


	cStrBuff buffWC;

#ifdef _WIN32
	const tDWORD dwLength = ::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char *>(&buffer[0]), fileSize, reinterpret_cast<tWCHAR *>(buffWC.get(2 * fileSize, false)), 2 * fileSize);
	if(!dwLength)
    {
        TRACE_MESSAGE2("Failed to convert UTF8 component localization file '%S'",
            componentLocalizationFileName.toWideChar());
		return false;
    }
#else
    mbstate_t state;
    memset(&state, 0, sizeof(state));
    
    const char *pSrc = reinterpret_cast<const char *>(&buffer[0]);
    const tDWORD dwLength = mbsrtowcs(reinterpret_cast<tWCHAR *>(buffWC.get(2 * (tDWORD)fileSize, false)), 
        &pSrc, fileSize, &state);
    if(dwLength == static_cast<size_t>(-1))
    {
        TRACE_MESSAGE2("Failed to convert UTF8 component localization file '%S'", componentLocalizationFileName.toWideChar());
        return false;
    }
#endif

	cStrObj strFile;
	strFile.assign(buffWC, cCP_UNICODE, dwLength * 2);

    tDWORD dwIndex = 1;
    while(strFile.length() != 0)
    {
        cStrObj strTemp;

        tDWORD dwPos = strFile.find_first_of(COMPONENT_DELIMITER);
        if(dwPos == cSTRING_EMPTY_LENGTH)
        {
            strTemp = strFile;
            strFile = "";
        }
        else
        {
            strTemp = strFile.substr(0, dwPos);

            dwPos = strFile.find_first_not_of(COMPONENT_DELIMITER, dwPos);
            if(dwPos == cSTRING_EMPTY_LENGTH)
                strFile = "";
            else
                strFile = strFile.substr(dwPos);
        }

        ComponentLocalization filter;
        if(getLocalizedComponentFromString(strTemp, filter))
        {
            filter.m_index = dwIndex;
            components.push_back(filter);
        }

        ++dwIndex;
    }

    return true;
}
