/*
*/

#if !defined(__KL_TRAFFICPROTOCOLLER_STRINGUTIL_H)
#define __KL_TRAFFICPROTOCOLLER_STRINGUTIL_H

#include <string>
#include <locale>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////
namespace TrafficProtocoller  {
namespace Util  {

//////////////////////////////////////////////////////////////////////////
inline bool EqualNoCase(const std::string& s, const char* psz)
{ 
	return (_stricmp(s.c_str(), psz) == 0); 
}

//////////////////////////////////////////////////////////////////////////
inline bool EqualNoCase(char ch1, char ch2)
{ 
	return (std::toupper(ch1, std::locale::classic()) 
				== std::toupper(ch2, std::locale::classic()));
}

class CSymbolCaseInsensitiveCmp
{
public:
	CSymbolCaseInsensitiveCmp():m_loc(std::locale::classic()){}
	bool operator()(char ch1, char ch2)
	{
		return std::toupper(ch1, m_loc) == std::toupper(ch2, m_loc);
	}
private:
	std::locale m_loc;
};

inline bool FindNoCase(const std::string& sSource, const std::string& sToFind)
{
	std::string::const_iterator it = std::search(
			sSource.begin(), sSource.end(),
			sToFind.begin(), sToFind.end(),
			CSymbolCaseInsensitiveCmp());

	return it != sSource.end();
}

}  // namespace Util
}  // namespace TrafficProtocoller

#endif  // !defined(__KL_TRAFFICPROTOCOLLER_STRINGUTIL_H)
