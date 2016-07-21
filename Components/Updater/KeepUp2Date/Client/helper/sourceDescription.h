#ifndef SOURCEDESCRIPTION_H_85C576CF_399D_49c3_AD67_7E3549F3CCF9
#define SOURCEDESCRIPTION_H_85C576CF_399D_49c3_AD67_7E3549F3CCF9

#if _MSC_VER > 1000
    #pragma once
#endif

#include "comdefs.h"
#include "path.h"
#include "../include/update_source.h"

namespace KLUPD {
    
struct KAVUPDCORE_API Source
{
    Source(const Path &url = Path(), const EUpdateSourceType & = UST_None, const bool useProxy = false,
        const Path &differenceFolder = L"diffs/", const EUpdateStructure & = UPS_DIRS);

    bool operator==(const Source &)const;
    
    // indicates whether non-directoried structure is allowed for the update source. For Kaspersky
    //  Laboratory sources only directoried structure is allowed to reduce load on infrastructure.
    bool onlyDirectoriedStructureAllowed()const;

    // transforms source address to user-readable string
    NoCaseString toString()const;

    Path m_url;
    EUpdateSourceType m_type;
    bool m_useProxy;
    Path m_differenceFolder;
    EUpdateStructure m_structure;
};

class KAVUPDCORE_API SourceList
{
public:
    SourceList();

    // append source to list if it is not already in list
    void addSource(const EUpdateSourceType &, const Path &URL, const bool useProxy, const Path &differenceFolder = L"diffs/");

    bool getCurrentElement(Source &)const;

    // return update structure of current element
    EUpdateStructure currentUpdateStructure()const;
    // set update structure of current element
    void currentUpdateStructure(const EUpdateStructure &);

    // make current source not to use proxy
    void disableProxyForCurrentSource();

    // returns false in case current source require date check of update
    // Note: date check on index files is only performed for Kaspersky Laboratory sources.
     // Date check is not performed if update from all other sources
    bool currentSourceIgnoreDates()const;

    // set current item to first source in list
    void moveFirst();
    /// set current item to next source in list
    void moveNext();

    // transforms source lost to user-readable table
    // delimiter [in] - each source is output on separate line and in the start line the delimeter is prepended
    NoCaseString toString(const char *delimiter = "\t")const;


    std::vector<Source> m_sources;

private:
    bool currentIndexInRange()const;

    size_t m_currentIndex;
};             

}   // namespace KLUPD

#endif  // SOURCEDESCRIPTION_H_85C576CF_399D_49c3_AD67_7E3549F3CCF9
