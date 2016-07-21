#ifndef MAP_WRAPPER_H_INCLUDED_D3D7EC8B_D23C_4e1a_9979_0E684B33777D
#define MAP_WRAPPER_H_INCLUDED_D3D7EC8B_D23C_4e1a_9979_0E684B33777D

#include "comdefs.h"

namespace KLUPD {

// map environment variables to path, to expand variable into path
 // Note: std::map and std::set MUST NOT be published, that is why std::vector is used
typedef std::vector<std::pair<NoCaseString, NoCaseString> > PathSubstitutionMap;
// this is helper class to find path substitution by key
struct KAVUPDCORE_API PathSubstitutionFinderByKey : public std::unary_function<std::pair<NoCaseString, NoCaseString>, bool>
{
    PathSubstitutionFinderByKey(const NoCaseString &key);
    result_type operator()(const argument_type &)const;

private:
    NoCaseString m_key;
};


class KAVUPDCORE_API MapStringVStringWrapper
{
public:
    void operator=(const MapStringVStringWrapper &);
    void Clear();

    // output map in human-readable form
    NoCaseString toString()const;
	
    long GetSize();

    void AddPair(const NoCaseString &key, const std::vector<NoCaseString> &value);
	void AddPairs(const std::vector<std::pair<NoCaseString /* key */, NoCaseString /* value */> > &);

    void remove(const NoCaseString &key);
	
	// find and return single element from collection
	// key [out] - search key
	// result [in] - result of search for given key. Vector always includes single value
    bool Find(const NoCaseString &key, std::vector<NoCaseString> &value)const;
	
    // Note: std::map and std::set MUST NOT be published, that is why std::vector is used
    typedef std::vector<std::pair<NoCaseString, std::vector<NoCaseString> > > Type;
    typedef std::vector<std::pair<NoCaseString, std::vector<NoCaseString> > >::const_iterator ConstIterator;
    typedef std::vector<std::pair<NoCaseString, std::vector<NoCaseString> > >::iterator Iterator;

    Type m_data;
};


}   // namespace KLUPD

#endif // MAP_WRAPPER_H_INCLUDED_D3D7EC8B_D23C_4e1a_9979_0E684B33777D
