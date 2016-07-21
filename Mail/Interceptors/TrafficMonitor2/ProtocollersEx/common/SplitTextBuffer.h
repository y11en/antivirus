/*
*/

#if !defined(__KL_SPLITTEXTBUFFER_H)
#define __KL_SPLITTEXTBUFFER_H

//////////////////////////////////////////////////////////////////////////
#include <deque>
#include <string>

//////////////////////////////////////////////////////////////////////////
namespace TrafficProtocoller  {
namespace Util  {

//////////////////////////////////////////////////////////////////////////
// class used to collect analysing data and split it into separate strings
//////////////////////////////////////////////////////////////////////////
class SplitTextBuffer
{
public:
	typedef char char_t;
	typedef std::basic_string<char_t> string_t;
public:
	SplitTextBuffer(const char_t* pszSplitPattern) : m_sSplitPattern(pszSplitPattern)  {}
	bool HasCompleteData() const				{ return !m_deqCompleteData.empty(); }
	const string_t& PeekCompleteData() const	{ return m_deqCompleteData.front(); }
	bool HasIncompleteData() const				{ return !m_sIncompleteData.empty(); }
	const string_t& PeekIncompleteData() const	{ return m_sIncompleteData; }
	void PopCompleteData()						{ m_deqCompleteData.pop_front(); }
	void ClearCompleteData()					{ m_deqCompleteData.clear(); }
	void ClearIncompleteData()					{ string_t().swap(m_sIncompleteData); }
	void ClearDataAll()							{ ClearCompleteData(); ClearIncompleteData(); }
	bool PutRawData(const char_t* pData, size_t nDataSize);
private:
	string_t m_sSplitPattern;
	std::deque<string_t> m_deqCompleteData;
	string_t m_sIncompleteData;
};

}
}

#endif // !defined(__KL_SPLITTEXTBUFFER_H)
