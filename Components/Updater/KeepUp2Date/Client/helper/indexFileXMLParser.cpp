#include "indexFileXMLParser.h"
#include "updaterStaff.h"


KLUPD::IndexFileXMLParser::IndexFileXMLParser(const NoCaseString& strFilename,
                                              FileVector &files,
                                              Signature6Checker &signature6Checker,
                                              const Path &parentRelativeURLPath,
                                              const bool retranslationMode,
                                              CallbackInterface &callbacks,
                                              Log *log)
 : m_strFilename(strFilename),
   m_files(files),
   m_currentFileStartIndex(0),
   m_currentFileEndIndex(0),
   m_signature6Checker(signature6Checker),
   m_parentRelativeURLPath(parentRelativeURLPath),
   m_skipSectionLevel(0),
   m_retranslationMode(retranslationMode),
   m_callbacks(callbacks),
   pLog(log)
{
}

KLUPD::NoCaseString KLUPD::IndexFileXMLParser::getTagFromLocalFile(const Tag &tag,
                                                      const Path &fullFileName,
                                                      CallbackInterface &callbacks,
                                                      Log *pLog)
{
    // read file
    std::vector<unsigned char> buffer;
    const CoreError readFileResult = (LocalFile(fullFileName, pLog)).read(buffer);
    if(!isSuccess(readFileResult))
    {
        TRACE_MESSAGE3("Failed to get date from XML index file '%S', failed to read file, result '%S'",
            fullFileName.toWideChar(), toString(readFileResult).toWideChar());
        return NoCaseString();
    }
    if(buffer.empty())
    {
        TRACE_MESSAGE3("Failed to get date from XML index file '%S', file is corrupted, file size is %d",
            fullFileName.toWideChar(), buffer.size());
        return NoCaseString();
    }

    // parse file
    FileVector fileList;
    Signature6Checker dskmWrapper(pLog);
    Path relativeURLPath;
    const bool retranslationModeNoMatter = false;
    IndexFileXMLParser xmler(fullFileName.getFileNameFromPath().m_value, fileList, dskmWrapper, relativeURLPath, retranslationModeNoMatter, callbacks, pLog);

    XmlReader xmlReader(reinterpret_cast<const char *>(&buffer[0]), buffer.size());
    if(!xmler.parseXMLRecursively(xmlReader, 0))
    {
        TRACE_MESSAGE2("Failed to get date from XML index file '%S', parse error", fullFileName.toWideChar());
        return NoCaseString();
    }

    if(tag == updateDate)
        return xmler.m_dateFromFile;
    if(tag == blackDate)
        return xmler.m_blackDateFromFile;
    return NoCaseString();
}

std::vector<KLUPD::UpdatedSettings> KLUPD::IndexFileXMLParser::updatedSettings()const
{
    return m_updatedSettings;
}

bool KLUPD::IndexFileXMLParser::parseXMLRecursively(XmlReader &xmlReader, int levelGuard)
{
    if(++levelGuard > 20)
    {
        TRACE_MESSAGE("Parse error: too deep index file XML structure");
		return false;
    }

    try
	{
        XmlElement elt;
        int tagParseResult = LAX::XML_OK;
        while((tagParseResult = xmlReader.readElement(elt, 0, 0)) == LAX::XML_OK)
        {
            std::string tag;
            elt.getTagName(tag);
            XmlAttrMap attrs;
            elt.readAttrs(attrs);

            onStartElement(asciiToWideChar(tag), attrs);
            if(elt.hasBody() && !parseXMLRecursively(xmlReader, levelGuard))
                return false;

            onEndElement();
            tagParseResult = xmlReader.endElement(elt);
            if(tagParseResult != LAX::XML_OK)
                break;
        }
        if(tagParseResult < 0)
        {
            TRACE_MESSAGE2("Index XML file parse error: '%d'", tagParseResult);
            return false;
        }
        return true;
    }
    catch(LAX::XmlException &xmlException)
    {
        TRACE_MESSAGE2("Index file XML parse exception: '%s'", xmlException.what());
    }
    catch(const std::exception &error)
    {
        TRACE_MESSAGE2("Index file parse exception: '%s'", error.what());
    }
    return false;
}

void KLUPD::IndexFileXMLParser::onStartElement(const NoCaseString &name, const XmlAttrMap &attributes)
{
    if(!m_currentSectionName.empty() && m_currentSectionName != name)
	{
        m_sectionHistory.push(m_currentSectionName);
        m_currentSectionName = name;
    }
	else if(m_currentSectionName.empty())
		m_currentSectionName = name;
	
	if(m_skipSectionLevel)
	{
		// some unknown section
		++m_skipSectionLevel;
	}
	else if(m_currentSectionName == section_UpdateFiles)
		parseUpdateSection(attributes);
    else if(m_currentSectionName == section_UpdateSettings)
        parseSettingsSection(attributes);
	else if((m_currentSectionName== section_FileDescription)
        || (m_currentSectionName == section_Registry))
    {
		parseFileOrRegistrySection(attributes);
    }
	else if(m_currentSectionName == section_LocLang)
		parseLanguageSection(attributes);
	else if(m_currentSectionName == section_FromAppVersion)
		parseFromAppVersionSection(attributes);
	else if(m_currentSectionName == section_OS)
		parseOsSection(attributes);
	else
	{
		// skip unknown section
		++m_skipSectionLevel;
	}
}

void KLUPD::IndexFileXMLParser::onEndElement()
{
    if(!m_sectionHistory.empty())
    {
        m_currentSectionName = m_sectionHistory.top();
        m_sectionHistory.pop();
		
        // Decrease unknown sections level counter
        if(m_skipSectionLevel)
            --m_skipSectionLevel;
    }
}

void KLUPD::IndexFileXMLParser::parseUpdateSection(const XmlAttrMap &attributes)
{
    const NoCaseString dateFromFile = asciiToWideChar(attributes.find(keyw_UpdDate, ""));
    if(dateFromFile.checkDateFormat())
        m_dateFromFile = dateFromFile;
    else
        TRACE_MESSAGE2("Failed to parse index XML for date, invalid format string '%S'", dateFromFile.toWideChar());

    const NoCaseString blackDateFromFile = asciiToWideChar(attributes.find(keyw_BlackListDate, ""));
    if(blackDateFromFile.checkDateFormat())
        m_blackDateFromFile = blackDateFromFile;
    else if(!blackDateFromFile.empty())
        TRACE_MESSAGE2("Failed to parse index XML for black date, invalid format string '%S'", blackDateFromFile.toWideChar());

    std::string strObjectType = attributes.find(keyw_ObjectType, "");
    if( !strObjectType.empty() )
        m_signature6Checker.SetObjectType(m_strFilename.toAscii().c_str(), strObjectType.c_str());
}

void KLUPD::IndexFileXMLParser::parseSettingsSection(const XmlAttrMap &attributes)
{
    UpdatedSettings variable;

    for(unsigned attributeIndex = 0; attributeIndex < attributes.size(); ++attributeIndex)
    {
        if(attributes[attributeIndex].name == keyw_Name)
            variable.m_name.fromAscii(attributes[attributeIndex].value);
        else if(attributes[attributeIndex].name == keyw_Type)
            variable.m_type.fromAscii(attributes[attributeIndex].value);
        else if(attributes[attributeIndex].name == keyw_Value)
            variable.m_value.fromAscii(attributes[attributeIndex].value);
    }

    if(variable.m_name.empty())
    {
        const std::string error = std::string("'UpdateSettings' variable name is not specified: '")
            + variable.m_name.toAscii() + "', while parsing XML index file";
        throw std::invalid_argument(error);
    }
    if(variable.m_value.empty())
    {
        const std::string error = std::string("'UpdateSettings' value is not specified: '")
            + variable.m_name.toAscii() + "', while parsing XML index file";
        throw std::invalid_argument(error);
    }

    m_updatedSettings.push_back(variable);
}

void KLUPD::IndexFileXMLParser::parseFileOrRegistrySection(const XmlAttrMap &attributes)
{
    FileInfo fileInfo;
    // by default files are in the same folder that index
	fileInfo.m_relativeURLPath = m_parentRelativeURLPath;


    NoCaseString substituteEnvironment;
    for(unsigned k = 0; k < attributes.size(); ++k)
	{
        const NoCaseString name = asciiToWideChar(attributes[k].name);
        const NoCaseString value = asciiToWideChar(attributes[k].value);

        if((name == keyw_ApplicationID) || (name == keyw_Build))
        {
            fileInfo.m_applicationFilter = parseBuild(value);
            fileInfo.m_applicationFilter.obsoleteApplicationTagUsed(name == keyw_ApplicationID);
        }
		else if(name == keyw_ComponentID)
            fileInfo.m_componentIdSet = StringParser::splitString(value, COMPONENT_DELIMITER);
		else if(name == keyw_UpdateType)
            fileInfo.m_type = FileInfo::fromStringInIndexFormat(value);
		else if(name == keyw_UpdateObligation)
            fileInfo.m_obligatoryEntry = FileInfo::fromStringObligatoryInIndexFormat(value);
		else if(name == keyw_Filename)
            fileInfo.m_filename = value;
        else if(name == keyw_ServerFolder)
            fileInfo.m_relativeURLPath = m_parentRelativeURLPath + value;
		else if(name == keyw_RelativeSrvPath)
            fileInfo.m_relativeURLPath = value;
		else if(name == keyw_LocalPath)
            fileInfo.m_originalLocalPath = fileInfo.m_localPath = value;
		else if(name == keyw_FileSign)
            fileInfo.m_signature5 = value;
		else if(name == kayw_MD5)
            fileInfo.m_md5 = CBase64::decodeBuffer(value.toAscii().c_str());
		else if(name == keyw_FileSize)
            value.toLong(fileInfo.m_size);
		else if(name == keyw_FileDate)
        {
            if(value.checkDateFormat())
                fileInfo.m_dateFromIndex = value;
            else
                throw std::invalid_argument(std::string("Invalid date format '") + value.toAscii() + "'");
        }
		else if(name == keyw_Comment)
            fileInfo.m_comment = value;
		else if(name == keyw_ToAppVersion)
            fileInfo.m_toAppVersion = value;
        else if(name == SS_KEY_LocalizationSet6)
            fileInfo.m_localizationSet6 = value;
        else if(name == SS_KEY_LocalizationSetAK)
            fileInfo.m_localizationSetAK = value;
        else if(name == keyw_SubstituteEnvironment)
            substituteEnvironment = value;
        else if(name == keyw_FixName)
            fileInfo.m_fixName = value;
        else if(name == keyw_BlackListSolvePeriodDays)
            value.toLong(fileInfo.m_solvePeriodDays);
        else if(name == keyw_ReplaceMode)
            fileInfo.m_transactionInformation.m_replaceMode = FileInfo::TransactionInformation::fromString(value);
        else if(name == keyw_RegistryBody)
            fileInfo.m_inlinedContent = CBase64::decodeBuffer(value.toAscii().c_str());
    }

    // local path for this entry has not been obtained, using default
    if(fileInfo.m_originalLocalPath.empty())
        fileInfo.m_originalLocalPath = fileInfo.m_localPath = Path(L"%") + SS_KEY_BaseFolder + L"%";


    // file name is obligatory XML tag    
    if(fileInfo.m_filename.empty())
        throw std::invalid_argument("File name is not specified while parsing XML file section");

    FileVector files = expandFiles(substituteEnvironment, fileInfo);
    for(FileVector::iterator fileIter = files.begin(); fileIter != files.end(); ++fileIter)
    {
        StringParser::canonizePath(StringParser::byProductFirst, fileIter->m_localPath, m_callbacks);
	    StringParser::canonizePath(StringParser::byProductFirst, fileIter->m_relativeURLPath, m_callbacks);
    }


    //////////////////////////////////////////////////////////////////////////
    ////// adding file to set

    if(fileInfo.m_type == FileInfo::registry || fileInfo.m_type == FileInfo::key)
        m_signature6Checker.addDataForSignature6Check(fileInfo.m_inlinedContent);

    m_files.insert(m_files.end(), files.begin(), files.end());
    m_currentFileStartIndex = m_files.size() - files.size();
    m_currentFileEndIndex = m_files.size();
}

KLUPD::FileInfo::ApplicationFilter KLUPD::IndexFileXMLParser::parseBuild(const NoCaseString &applicationStringToParse)const
{
    FileInfo::ApplicationFilter result;
    const std::vector<NoCaseString> ranges = StringParser::splitString(applicationStringToParse, L";");
    for(std::vector<NoCaseString>::const_iterator rangeIter = ranges.begin(); rangeIter != ranges.end(); ++rangeIter)
        result.parseAndAddRange(*rangeIter);

    return result;
}

void KLUPD::IndexFileXMLParser::parseLanguageSection(const XmlAttrMap &attributes)
{
    for(size_t fileIndex = m_currentFileStartIndex; fileIndex < m_currentFileEndIndex; ++fileIndex)
    {
        const NoCaseString language = asciiToWideChar(attributes.find(keyw_Lang, ""));
        m_files[fileIndex].m_language.push_back(language);
    }
}

void KLUPD::IndexFileXMLParser::parseFromAppVersionSection(const XmlAttrMap &attributes)
{
    for(size_t fileIndex = m_currentFileStartIndex; fileIndex < m_currentFileEndIndex; ++fileIndex)
    {
        const NoCaseString fromApplicationVersion = asciiToWideChar(attributes.find(keyw_Version, ""));
        m_files[fileIndex].m_fromAppVersion.push_back(fromApplicationVersion);
    }
}

void KLUPD::IndexFileXMLParser::parseOsSection(const XmlAttrMap &attributes)
{
    FileInfo::OS os;
    os.m_name.fromAscii(attributes.find(keyw_OsName, ""));
    os.m_version.fromAscii(attributes.find(section_OsVer, ""));

    for(size_t fileIndex = m_currentFileStartIndex; fileIndex < m_currentFileEndIndex; ++fileIndex)
        m_files[fileIndex].m_OSes.push_back(os);
}

KLUPD::FileVector KLUPD::IndexFileXMLParser::expandItem(const NoCaseString &variableName,
                                                        const std::vector<NoCaseString> &variableValues, const FileInfo &fileIn)const
{
    FileVector result;
    for(std::vector<NoCaseString>::const_iterator variableValuesIter = variableValues.begin(); variableValuesIter != variableValues.end(); ++variableValuesIter)
    {
        // copy for current iteration
        FileInfo file = fileIn;

        PathSubstitutionMap pathSubstitution;
        pathSubstitution.push_back(std::make_pair(NoCaseString(L"%") + variableName + L"%", *variableValuesIter));
        StringParser::performSubstitutions(StringParser::byProductFirst, file.m_localPath, m_callbacks, pathSubstitution);
	    StringParser::performSubstitutions(StringParser::byProductFirst, file.m_relativeURLPath, m_callbacks, pathSubstitution);

        for(ComponentIdentefiers::iterator componentIter = file.m_componentIdSet.begin(); componentIter != file.m_componentIdSet.end(); ++componentIter)
        {
            Path path = *componentIter;
    	    StringParser::performSubstitutions(StringParser::byProductFirst, path, m_callbacks, pathSubstitution);
            *componentIter = path.toWideChar();
        }

//TODO	    StringParser::performSubstitutions(StringParser::byProductFirst, file.m_applicationID, pathSubstitution);

//TODO        for(std::vector<NoCaseString>::iterator languageIter = file.m_language.begin(); languageIter != file.m_language.end(); ++languageIter)
//TODO            StringParser::performSubstitutions(StringParser::byProductFirst, *languageIter, m_callbacks, pathSubstitution);

//TODO            StringParser::performSubstitutions(StringParser::byProductFirst, file.m_localizationSet6, m_callbacks, pathSubstitution);
//TODO            StringParser::performSubstitutions(StringParser::byProductFirst, file.m_localizationSetAK, m_callbacks, pathSubstitution);
        
        result.push_back(file);
    }

    return result;
}

KLUPD::FileVector KLUPD::IndexFileXMLParser::expandVariableSubstitution(const NoCaseString &variableName,
                                                                        const std::vector<NoCaseString> &variableValuesIn, const FileVector &files)const
{
    FileVector result;
    for(FileVector::const_iterator fileIter = files.begin(); fileIter != files.end(); ++fileIter)
    {
        std::vector<NoCaseString> variableValues;

        if(m_retranslationMode)
            variableValues = variableValuesIn;
        // in update mode single most appropriate item should be selected
        else
        {
            // first element is used by default,
             // in case application does not prefer any specific value,
             // or application provided valued does not match allowed values set
            const NoCaseString defaultValue = variableValuesIn[0];

            PathSubstitutionMap::const_iterator findPath = std::find_if(m_callbacks.updaterConfiguration().m_pathSubstitutionMap.begin(), m_callbacks.updaterConfiguration().m_pathSubstitutionMap.end(),
                PathSubstitutionFinderByKey(NoCaseString(L"%") + variableName + L"%"));

            const NoCaseString applicationProvidedPreferedValue = (findPath == m_callbacks.updaterConfiguration().m_pathSubstitutionMap.end())
                ? defaultValue : findPath->second;

            // using default in case application-provided value does not present in set of available values
            if(std::find(variableValuesIn.begin(), variableValuesIn.end(), applicationProvidedPreferedValue) == variableValuesIn.end())
                variableValues.push_back(defaultValue);
            else
                variableValues.push_back(applicationProvidedPreferedValue);
        }

        FileVector items = expandItem(variableName, variableValues, *fileIter);
        result.insert(result.end(), items.begin(), items.end());
    }
    return result;
}

KLUPD::FileVector KLUPD::IndexFileXMLParser::expandFiles(const NoCaseString &substituteEnvironment, const FileInfo &file)const
{
    FileVector result;
    result.push_back(file);

    // there can be several variables in string
    const std::vector<NoCaseString> variablesToParse = StringParser::splitString(substituteEnvironment, L";");

    for(std::vector<NoCaseString>::const_iterator variablesToParseIter = variablesToParse.begin(); variablesToParseIter != variablesToParse.end(); ++variablesToParseIter)
    {
        // each variable consists of name and value "Localization=[en,ru,de,fr]"
        const std::vector<NoCaseString> variablesNameAndValue = StringParser::splitString(*variablesToParseIter, L"=");

        // the only single '=' delimiter may presents
        if(variablesNameAndValue.size() != 2)
            continue;

        // the '[' and ']' symbols must present
        if(variablesNameAndValue[1].size() < 2)
            continue;
        // cut the '[' and ']' symbols
        NoCaseString variablesValue = variablesNameAndValue[1].toWideChar() + 1;
        variablesValue.resize(variablesValue.size() - 1);

        result = expandVariableSubstitution(variablesNameAndValue[0], StringParser::splitString(variablesValue, L","), result);
    }

    return result;
}
