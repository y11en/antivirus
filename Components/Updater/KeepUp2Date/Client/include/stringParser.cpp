#include "stringParser.h"
#include "callback_iface.h"

std::vector<KLUPD::NoCaseString> KLUPD::StringParser::splitString(const NoCaseString &stringToSplitIn, const NoCaseString &delimiters)
{
    std::vector<NoCaseString> result;

    // using string implementation because MS VS 6 STL implementation
    //  can not insert() to string if iterator type differs
    const NoCaseString stringToSplit = stringToSplitIn;

    NoCaseString::const_iterator iterStart = stringToSplit.begin();
    while(true)
    {
        NoCaseString::const_iterator iterEnd = std::find_first_of(iterStart, stringToSplit.end(),
            delimiters.begin(), delimiters.end());

        // there is no interval constructor in STLPort
        NoCaseString stlPortWorkAround = stringToSplit.toWideChar() + std::distance(stringToSplit.begin(), iterStart);
        stlPortWorkAround.resize(std::distance(iterStart, iterEnd));

        if(!stlPortWorkAround.empty())
            result.push_back(stlPortWorkAround.toWideChar());

        if(iterEnd == stringToSplit.end())
            return result;

        // +1 is to skip delimiter
        iterStart = iterEnd + 1;
    }
}

void KLUPD::StringParser::performSubstitutions(const ExpandOrder &expandOrder, Path &pathInOut,
                                               CallbackInterface &callbacks, const PathSubstitutionMap &pathSubstitutions)
{
    const wchar_t patternDelimiter = L'%';

    Path path = pathInOut;

    size_t startOffsetAgainstCycling = 0;

    while(true)
    {
        NoCaseString::iterator iterStart = std::find(path.m_value.begin() + startOffsetAgainstCycling, path.m_value.end(), patternDelimiter);
        if(iterStart == path.m_value.end())
            break;

        NoCaseString::iterator iterEnd = std::find(iterStart + 1, path.m_value.end(), patternDelimiter);
        // the start '%' symbol has no terminating '%' symbol
        if(iterEnd == path.m_value.end())
            break;

        // include trailing '%'
        ++iterEnd;


        // there is no interval constructor in STLPort
        Path stlPortWorkAround = path.m_value.toWideChar() + std::distance(path.m_value.begin(), iterStart);
        stlPortWorkAround.m_value.resize(std::distance(iterStart, iterEnd));

        const Path environmentValue = getEnvironmentValue(expandOrder, callbacks, stlPortWorkAround.toWideChar(), pathSubstitutions);

        // to avoid cycling
        startOffsetAgainstCycling = std::distance(path.m_value.begin(), iterStart) + environmentValue.size();

        path.m_value.replace(iterStart, iterEnd, environmentValue.toWideChar());
    }

    pathInOut = path;
}

void KLUPD::StringParser::clearSubstitutions(Path &pathInOut, const wchar_t patternDelimiter)
{
    // using string implementation because MS VS 6 STL implementation
    //  can not insert() to string if iterator type differs
    NoCaseString path = pathInOut.toWideChar();

    size_t startOffsetAgainstCycling = 0;

    while(true)
    {
        NoCaseString::iterator iterStart = std::find(path.begin() + startOffsetAgainstCycling, path.end(), patternDelimiter);
        if(iterStart == path.end())
            break;

        NoCaseString::iterator iterEnd = std::find(iterStart + 1, path.end(), patternDelimiter);
        // the start '%' symbol has no terminating '%' symbol
        if(iterEnd == path.end())
            break;

        // include trailing '%'
        ++iterEnd;


        // clear substitution
		NoCaseString empty;
        path.replace(iterStart, iterEnd, empty);
    }

    pathInOut = path;
}

void KLUPD::StringParser::canonizePath(const ExpandOrder &expandOrder, Path &path, CallbackInterface &callbacks)
{
    performSubstitutions(expandOrder, path, callbacks, callbacks.updaterConfiguration().m_pathSubstitutionMap);
    path.correctPathDelimiters();
}

KLUPD::NoCaseString KLUPD::StringParser::getEnvironmentValue(const ExpandOrder &expandOrder, CallbackInterface &callbacks,
                                                const NoCaseString &environmentVariableNameIn, const PathSubstitutionMap &pathSubstitutions)
{
    /// get variable from user specified storage
    PathSubstitutionMap::const_iterator findIter = std::find_if(pathSubstitutions.begin(),
        pathSubstitutions.end(), PathSubstitutionFinderByKey(environmentVariableNameIn));

    if(findIter != pathSubstitutions.end())
        return findIter->second;


    /// removing '%' decorations
    if(environmentVariableNameIn.size() < 2)
        return NoCaseString();

    // there is no interval constructor in STLPort
    NoCaseString stlPortWorkAround = environmentVariableNameIn.toWideChar() + 1;
    stlPortWorkAround.resize(stlPortWorkAround.size() - 1);


    /// expand environment from product
    NoCaseString result;
    if(callbacks.expandEnvironmentString(stlPortWorkAround, result, expandOrder))
        return result;

    /// get system environment variable
    if(unicodeFileOperationsSupported())
    {
        // both _WIN32 and WSTRING_SUPPORTED macros are always defined in case unicodeFileOperationsSupported() returns true
        #if defined(WIN32) && defined(WSTRING_SUPPORTED)
            const wchar_t *resultVariable = _wgetenv(stlPortWorkAround.toWideChar());
            if(resultVariable)
                return resultVariable;
        #endif
    }
    else
    {
        const char *resultVariable = getenv(stlPortWorkAround.toAscii().c_str());
        if(resultVariable)
            return asciiToWideChar(resultVariable).c_str();
    }


    // expand failed
    return environmentVariableNameIn;
}
