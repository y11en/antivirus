#include "fileInfo.h"

#include "base64.h"
#include "local_file.h"
#include "updaterStaff.h"


KLUPD::NoCaseString KLUPD::FileInfo::toString(const FileInfo::ChangeStatus &changeStatus)
{
    switch(changeStatus)
    {
    case modified:
        return SS_KEY_RecentStatusModified;
    case added:
        return SS_KEY_RecentStatusAdded;
    default:
        return SS_KEY_PostponedUnchanged;
    }
}
KLUPD::FileInfo::ChangeStatus KLUPD::FileInfo::stringToChangeStatus(const NoCaseString &statusString)
{
    if(statusString == SS_KEY_RecentStatusModified)
        return modified;
    if(statusString == SS_KEY_RecentStatusAdded)
        return added;

    // by default unchanged status
    return unchanged;
}

KLUPD::NoCaseString KLUPD::FileInfo::toString(const FileInfo::Type &updateType, const bool humanReadable)
{
    if(humanReadable)
    {
        switch(updateType)
        {
        case base:
            return L"base";
        case patch:
            return L"patch";
        case index:
            return L"index";
        case blackList:
            return L"black list";
        case registry:
            return L"signature 6 registry";
        case key:
            return L"signature 6 public key";
        default:
            std::ostringstream stream;
            stream.imbue(std::locale::classic());
            stream << "unexpected value (" << updateType << ")";
            return asciiToWideChar(stream.str());
        }
    }

    /// KCA format for settings storage
    switch(updateType)
    {
    case patch:
        return SS_KEY_Patch;

    case index:
        return SS_KEY_Desc;

    case blackList:
        return SS_KEY_Blst;

    default:
        return SS_KEY_Base;
    }
}

KLUPD::FileInfo::Type KLUPD::FileInfo::fromStringInIndexFormat(const NoCaseString &typeString)
{
    if((typeString == UpdateTypePatch)
        || (typeString == UpdateTypeExecutable))
    {
        return patch;
    }
    if(typeString == UpdateTypeDesc)
        return index;
    if(typeString == UpdateTypeBlst)
        return blackList;
    if(typeString == UpdateTypeSignature6Registry)
        return registry;
    if(typeString == UpdateTypeSignature6PublicKey)
        return key;

    // by default is base
    return base;
}

bool KLUPD::FileInfo::fromStringObligatoryInIndexFormat(const NoCaseString &inputString)
{
    // by default update entries are obligatory
    return inputString != UpdateObligOptional;
}


KLUPD::NoCaseString KLUPD::FileInfo::OS::toString()const
{
    // OS name
    NoCaseString result = m_name;
    // OS version
    if(!m_version.empty())
        result += NoCaseString(L":") + m_version;
    return result;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

KLUPD::FileInfo::ApplicationFilter::Range::Range(const BuildNumber &min, const BuildNumber &max)
 : m_min(min),
   m_max(max)
{
}
KLUPD::FileInfo::ApplicationFilter::BuildNumber KLUPD::FileInfo::ApplicationFilter::Range::minBuild()const
{
    return m_min;
}
KLUPD::FileInfo::ApplicationFilter::BuildNumber KLUPD::FileInfo::ApplicationFilter::Range::maxBuild()const
{
    return m_max;
}

bool KLUPD::FileInfo::ApplicationFilter::Range::intersects(const Range &range)const
{
    if((range.m_min <= m_min) && (m_min <= range.m_max))
        return true;

    if((m_min <= range.m_min) && (range.m_min <= m_max))
        return true;

    return false;
}

KLUPD::NoCaseString KLUPD::FileInfo::ApplicationFilter::Range::toString(const bool humanReadableFormat)const
{
    std::ostringstream stream;
    stream.imbue(std::locale::classic());

    if(m_min == m_max)
        stream << m_min;
    else
        stream << m_min << (humanReadableFormat ? "-" : ",") << m_max;
    return asciiToWideChar(stream.str());
}

bool KLUPD::FileInfo::ApplicationFilter::Range::fromString(const NoCaseString &stringToParse)
{
    if(stringToParse.empty())
        return false;

    // check if range is single number
    if(stringToParse.isNumber())
    {
        std::istringstream stream(stringToParse.toAscii());
        stream.imbue(std::locale::classic());
        stream >> m_min;
        m_max = m_min;
        return true;
    }

    // [min,max] interval
    std::vector<NoCaseString> interval = StringParser::splitString(stringToParse, L"-,");
    // assert there are 2 numbers
    if((interval.size() != 2) || !interval[0].isNumber() || !interval[1].isNumber())
        return false;

    std::istringstream streamMin(interval[0].toAscii());
    streamMin.imbue(std::locale::classic());
    streamMin >> m_min;

    std::istringstream streamMax(interval[1].toAscii());
    streamMax.imbue(std::locale::classic());
    streamMax >> m_max;

    return !streamMin.fail() && !streamMax.fail() && (m_min < m_max);
}

KLUPD::FileInfo::ApplicationFilter::ApplicationFilter()
 :m_obsoleteApplicationTagUsed(false)
{
}

std::vector<KLUPD::FileInfo::ApplicationFilter::Range> KLUPD::FileInfo::ApplicationFilter::numericRanges()const
{
    return m_numericRanges;
}
std::vector<KLUPD::NoCaseString> KLUPD::FileInfo::ApplicationFilter::stringBuilds()const
{
    return m_stringBuilds;
}

bool KLUPD::FileInfo::ApplicationFilter::obsoleteApplicationTagUsed()const
{
    return m_obsoleteApplicationTagUsed;
}

void KLUPD::FileInfo::ApplicationFilter::obsoleteApplicationTagUsed(const bool obsoleteApplicationTagUsedValue)
{
    m_obsoleteApplicationTagUsed = obsoleteApplicationTagUsedValue;
}

bool KLUPD::FileInfo::ApplicationFilter::matches(const ApplicationFilter &toCheck)const
{
    if(matchesToAll() || toCheck.matchesToAll())
        return true;

    // check string ranges
    if(std::find_first_of(m_stringBuilds.begin(), m_stringBuilds.end(),
        toCheck.m_stringBuilds.begin(), toCheck.m_stringBuilds.end()) != m_stringBuilds.end())
    {
        return true;
    }


    // check numeric ranges
    for(std::vector<Range>::const_iterator toCheckRangeIter = toCheck.m_numericRanges.begin(); toCheckRangeIter != toCheck.m_numericRanges.end(); ++toCheckRangeIter)
    {
        for(std::vector<Range>::const_iterator rangeIter = m_numericRanges.begin(); rangeIter != m_numericRanges.end(); ++rangeIter)
        {
            if(toCheckRangeIter->intersects(*rangeIter))
                return true;
        }
    }
    return false;
}
bool KLUPD::FileInfo::ApplicationFilter::matches(const NoCaseString &toCheck)const
{
    ApplicationFilter filter;
    filter.parseAndAddRange(toCheck);
    return matches(filter);
}
bool KLUPD::FileInfo::ApplicationFilter::matchesToAll()const
{
    return m_stringBuilds.empty() && m_numericRanges.empty();
}

void KLUPD::FileInfo::ApplicationFilter::parseAndAddRange(const NoCaseString &rangeToParseString)
{
    // numeric range
    Range range;
    if(range.fromString(rangeToParseString))
        m_numericRanges.push_back(range);
    // string range
    else if(!rangeToParseString.empty())
    {
        const std::vector<NoCaseString> stringBuilds = StringParser::splitString(rangeToParseString, L";");
        m_stringBuilds.insert(m_stringBuilds.end(), stringBuilds.begin(), stringBuilds.end());
    }
}

KLUPD::NoCaseString KLUPD::FileInfo::ApplicationFilter::toString()const
{
    NoCaseString result;

    if(m_obsoleteApplicationTagUsed)
        result += L"(obsolete format)";

    for(std::vector<NoCaseString>::const_iterator stringRangeIter = m_stringBuilds.begin(); stringRangeIter != m_stringBuilds.end(); ++stringRangeIter)
    {
        result += *stringRangeIter;
        if(stringRangeIter + 1 != m_stringBuilds.end())
            result += L";";
    }

    if(!result.empty() && !m_numericRanges.empty())
        result += L";";

    for(std::vector<Range>::const_iterator numericRangeIter = m_numericRanges.begin(); numericRangeIter != m_numericRanges.end(); ++numericRangeIter)
    {
        result += numericRangeIter->toString(true);
        if(numericRangeIter + 1 != m_numericRanges.end())
            result += L";";
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

KLUPD::FileInfo::TransactionInformation::ReplaceMode KLUPD::FileInfo::TransactionInformation::fromString(const NoCaseString &stringValue)
{
    if(stringValue == keyw_ReplaceModeSuffix)
        return suffix;
    return ordinary;
}
KLUPD::NoCaseString KLUPD::FileInfo::TransactionInformation::toString(const ReplaceMode &replaceMode)
{
    if(replaceMode == suffix)
        return keyw_ReplaceModeSuffix;
    return keyw_ReplaceModeOrdinary;
}

KLUPD::FileInfo::TransactionInformation::TransactionInformation(const ChangeStatus &changeStatus,
                                                                const ReplaceMode &replaceMode,
                                                                const Path &currentLocation,
                                                                const Path &newLocation)
  : m_changeStatus(changeStatus),
    m_replaceMode(replaceMode),
    m_currentLocation(currentLocation),
    m_newLocation(newLocation)
{
}
KLUPD::NoCaseString KLUPD::FileInfo::TransactionInformation::toString()const
{
    std::ostringstream stream;
    stream.imbue(std::locale::classic());

    stream << "current location '" << m_currentLocation.toAscii()
        << "', new location '" << m_newLocation.toAscii() << "'";
    if(m_replaceMode == suffix)
        stream << ", replace files using suffix";

    return asciiToWideChar(stream.str());
}


KLUPD::FileInfo::FileInfo(const Path &filename, const Path &relativeURLPath, const Type &type, const bool primaryIndex)
 : m_type(type),
   m_filename(filename),
   m_relativeURLPath(relativeURLPath),
   m_obligatoryEntry(true),
   m_size(static_cast<size_t>(-1)),
   m_solvePeriodDays(0),
   m_primaryIndex(primaryIndex)
{
    m_relativeURLPath.correctPathDelimiters();
}

KLUPD::NoCaseString KLUPD::FileInfo::toString(const bool withTransactionInformation)const
{
    std::ostringstream stream;
    stream.imbue(std::locale::classic());

    if(!m_inlinedContent.empty())
        stream << "inlined ";

    stream << toString(m_type).toAscii() << " '" << m_filename.toAscii()
        << "', relative URL '" << m_relativeURLPath.toAscii()
        << "', local path '" << m_localPath.toAscii()
        << "', original path '" << m_originalLocalPath.toAscii() << "'";

    stream << ", size ";
    if(m_size == static_cast<size_t>(-1))
        stream << "unknown";
    else
        stream << m_size;

    if(!m_applicationFilter.matchesToAll())
        stream << ", application filter '" << m_applicationFilter.toString().toAscii() << "'";
    if(!m_componentIdSet.empty())
        stream << ", components " << KLUPD::toString(m_componentIdSet).toAscii();

    if(!m_fromAppVersion.empty())
    {
        stream << ", application versions ";
        for(std::vector<NoCaseString>::const_iterator fromApplicaitonIter = m_fromAppVersion.begin();;)
        {
            stream << "'" << fromApplicaitonIter->toAscii() << "'";

            ++fromApplicaitonIter;
            if(fromApplicaitonIter == m_fromAppVersion.end())
                break;
            stream << " ";
        }
    }

    if(!m_toAppVersion.empty())
        stream << ", update to '" << m_toAppVersion.toAscii() << "'";

    if(!m_obligatoryEntry)
        stream << ", optional";
    
    if(!m_dateFromIndex.empty())
        stream << ", date '" << m_dateFromIndex.toAscii() << "'";

    if(!m_signature5.empty())
        stream << ", signature 5 '" << m_signature5.toAscii() << "'";

    if(!m_md5.empty())
    {
        std::vector<char> traceMD5 = CBase64::encodeBuffer(&m_md5[0], m_md5.size());
        traceMD5.push_back(0);
        stream << ", MD5 '" << &traceMD5[0] << "'";
    }

    if(!m_OSes.empty())
    {
        stream << ", OS:";
        for(std::vector<OS>::const_iterator osIter = m_OSes.begin(); osIter != m_OSes.end(); ++osIter)
            stream << " (" << osIter->toString().toAscii() << ")";
    }

    if(!m_language.empty())
    {
        stream << ", language(s):";
        for(std::vector<NoCaseString>::const_iterator languageIter = m_language.begin(); languageIter != m_language.end(); ++languageIter)
            stream << " '" << languageIter->toAscii() << "'";
    }


    if(!m_localizationSet6.empty())
        stream << ", localization set '" << m_localizationSet6.toAscii() << "'";

    if(!m_localizationSetAK.empty())
        stream << ", localization set AdminKit '" << m_localizationSetAK.toAscii() << "'";

    stream << ", status '" << FileInfo::toString(m_transactionInformation.m_changeStatus).toAscii() << "'";
    if(withTransactionInformation)
        stream << ",\n\t\t\ttransaction information: " << m_transactionInformation.toString().toAscii();

    if(!m_fixName.empty())
        stream << ", fix '" << m_fixName.toAscii() << "'";

    if(m_solvePeriodDays)
        stream << ", solve black list problem period " << m_solvePeriodDays << " days";

    return asciiToWideChar(stream.str());
}

bool KLUPD::FileInfo::isTheSame(const FileInfo &right, const bool retranslationMode)const
{
    if(m_filename != right.m_filename)
        return false;

    // in retranslation mode local path is ignored, and compared only in Update mode
    return retranslationMode
        ? (m_relativeURLPath == right.m_relativeURLPath)
        : (m_localPath == right.m_localPath);
}

bool KLUPD::FileInfo::isIndex()const
{
    return m_type == index;
}
bool KLUPD::FileInfo::isInlined()const
{
    return !m_inlinedContent.empty();
}
bool KLUPD::FileInfo::downloadNeeded()const
{
    return m_transactionInformation.m_changeStatus == FileInfo::added
        || m_transactionInformation.m_changeStatus == FileInfo::modified;
}
bool KLUPD::FileInfo::isBlackList()const
{
    return m_type == FileInfo::blackList;
}
const bool KLUPD::FileInfo::canBeDownloadedWithKlz()const
{
    // only index files can be downloaded with Klz compression mechanism
    return isIndex() && !isInlined();
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


KLUPD::FileVector::IsTheSamePredicate::IsTheSamePredicate(const FileInfo &etalon, const bool retranslationMode)
 : m_etalon(etalon),
   m_retranslationMode(retranslationMode)
{
}

bool KLUPD::FileVector::IsTheSamePredicate::operator()(const FileInfo &right)const
{
    return m_etalon.isTheSame(right, m_retranslationMode);
}


bool KLUPD::FileVector::theSameFilePresents(const FileInfo &etalon, const bool retranslationMode)const
{
    for(FileVector::const_iterator fileIter = begin(); fileIter != end(); ++fileIter)
    {
        if(fileIter->isTheSame(etalon, retranslationMode))
            return true;
    }
    return false;
}
bool KLUPD::FileVector::findTheSameFile(const FileInfo &etalon, FileInfo &result, const bool retranslationMode)const
{
    for(FileVector::const_iterator fileIter = begin(); fileIter != end(); ++fileIter)
    {
        if(fileIter->isTheSame(etalon, retranslationMode))
        {
            result = *fileIter;
            return true;
        }
    }
    return false;
}
void KLUPD::FileVector::insertNewOrUpdateTheSame(const FileInfo &sameElement, const FileInfo &newValue, bool &fileDuplicate, const bool retranslationMode)
{
    fileDuplicate = false;
    for(FileVector::iterator fileIter = begin(); fileIter != end(); ++fileIter)
    {
        if(fileIter->isTheSame(sameElement, retranslationMode))
        {
            *fileIter = newValue;
            fileDuplicate = true;
            return;
        }
    }
    push_back(newValue);
}

void KLUPD::FileVector::insertNewInTheBeginOfListOrUpdateTheSame(const FileInfo &sameElement, const FileInfo &newValue, bool &fileDuplicate, const bool retranslationMode)
{
    fileDuplicate = false;
    for(FileVector::iterator fileIter = begin(); fileIter != end(); ++fileIter)
    {
        if(fileIter->isTheSame(sameElement, retranslationMode))
        {
            *fileIter = newValue;
            fileDuplicate = true;
            return;
        }
    }
    insert(begin(), newValue);
}

bool KLUPD::FileVector::anyFileChanged()const
{
    for(const_iterator fileIter = begin(); fileIter != end(); ++fileIter)
    {
        if(fileIter->m_transactionInformation.m_changeStatus != FileInfo::unchanged)
            return true;
    }
    return false;
}

void KLUPD::FileVector::splitByComponents(FilesByComponent &filesByComponents, const bool filterEmptyTransactions)const
{
    for(FileVector::const_iterator fileIter = begin(); fileIter != end(); ++fileIter)
    {
        // component identifier is empty
        if(fileIter->m_componentIdSet.empty())
            filesByComponents[NoCaseString()].push_back(*fileIter);
        else
        {
            for(ComponentIdentefiers::const_iterator componentIter = fileIter->m_componentIdSet.begin(); componentIter != fileIter->m_componentIdSet.end(); ++componentIter)
                filesByComponents[*componentIter].push_back(*fileIter);
        }
    }

    // filter empty transactions if requested
    if(!filterEmptyTransactions)
        return;

    for(size_t filesByComponentsIndex = 0; filesByComponentsIndex < filesByComponents.size();)
    {
        FilesByComponent::iterator filesByComponentIter = filesByComponents.begin();
        std::advance(filesByComponentIter, filesByComponentsIndex);

        if(filesByComponentIter->second.anyFileChanged())
        {
            ++filesByComponentsIndex;
            continue;
        }

        filesByComponents.erase(filesByComponentIter);
    }
}
