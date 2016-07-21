/*
*/

#pragma warning( disable : 4786 )
#include "SplitTextBuffer.h"
#include <algorithm>
#include <Prague/prague.h>

extern hROOT g_root;

//////////////////////////////////////////////////////////////////////////
namespace TrafficProtocoller  {
namespace Util  {



//////////////////////////////////////////////////////////////////////////
bool SplitTextBuffer::PutRawData(const char_t* pData, size_t nDataSize)
{
	if (!pData || nDataSize <= 0)
		return true;

	// добавим данные к необработаным
	size_t lastSize = m_sIncompleteData.size();

	try
	{
		m_sIncompleteData.append(pData, nDataSize);
	}
	catch (const std::bad_alloc&)
	{
		PR_TRACE(( g_root, prtERROR, "stb\tSplitTextBuffer::PutRawData Out of memory"));
		return false;
	}

	// начинать искать паттерн надо с позиции (конец прошлых необработаных данных) - (длина паттерна) + 1
	// т.к. паттерн может прийти по частям

	size_t pos = 0;
	if (lastSize >= (m_sSplitPattern.size() + 1))
	{
		pos = lastSize - m_sSplitPattern.size() + 1;
	}
	
	string_t::iterator it			= m_sIncompleteData.begin() + pos;
	string_t::iterator it_end		= m_sIncompleteData.end();
	
	while (it != it_end)
	{
		string_t::iterator itf = std::search(it, it_end, m_sSplitPattern.begin(), m_sSplitPattern.end());
		
		if (itf != it_end)
		{
			try
			{
				string_t sCompleteData = m_sIncompleteData.substr(0, std::distance(m_sIncompleteData.begin(), itf));
				it		= m_sIncompleteData.erase(m_sIncompleteData.begin(), itf + m_sSplitPattern.size());
				it_end	= m_sIncompleteData.end();
				m_deqCompleteData.push_back(sCompleteData);
			}
			catch (const std::bad_alloc&)
			{
				PR_TRACE(( g_root, prtERROR, "stb\tSplitTextBuffer::PutRawData Out of memory"));
				return false;
			}
		}
		else
		{
			it = it_end;
		}
	}

	return true;
}

}  // namespace Util
}  // namespace TrafficProtocoller
