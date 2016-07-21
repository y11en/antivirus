#if !defined(AFX_STRINGPARSER_H__2457D7B4_060B_47A4_80A2_4FC7D6AEF8EF__INCLUDED_)
#define AFX_STRINGPARSER_H__2457D7B4_060B_47A4_80A2_4FC7D6AEF8EF__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif

#include "../helper/map_wrapper.h"
#include "../helper/path.h"

namespace KLUPD {

class CallbackInterface;

class KAVUPDCORE_API StringParser
{
public:
    enum ExpandOrder
    {
        byProductFirst,
        bySystemFirst
    };

    // split string into substring using delimiters
    static std::vector<NoCaseString> splitString(const NoCaseString &stringToSplit, const NoCaseString &delimiters);

    // expands string with path substitutions and application environment
    static void performSubstitutions(const ExpandOrder &, Path &, CallbackInterface &, const PathSubstitutionMap &);
    // replaces all substitutions with empty strings
    static void clearSubstitutions(Path &path, const wchar_t patternDelimiter);

   
    //// transform path to 'updater canonical form', and substitute variables
    static void canonizePath(const ExpandOrder &, Path &path, CallbackInterface &);

private:
    // searches environment variable into pathSubstitutions storage and into application current environment
    static NoCaseString getEnvironmentValue(const ExpandOrder &, CallbackInterface &,
        const NoCaseString &environmentVariableName, const PathSubstitutionMap &);
};

}   // namespace KLUPD


#endif // !defined(AFX_STRINGPARSER_H__2457D7B4_060B_47A4_80A2_4FC7D6AEF8EF__INCLUDED_)
