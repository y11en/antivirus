#include "stdafx.h"
#include "BWFind.h"
#include <boost/regex/v3/regex.hpp>
#include <string>

static bool ConvertBwRule2RegExp(cStringObj &rule)
{
	if(rule.empty())
		return false;

	tWCHAR *pResBuffer = new tWCHAR[rule.size() * 11]; // на каждый символ не больше 11
	tWCHAR *pDst = pResBuffer;
	if(!pResBuffer)
		return false;

	const tWCHAR *pSrc = rule.data();
	tWCHAR wc, pwc = 0;
	while(wc = *pSrc++)
	{
		switch(wc)
		{
		case L'.':
		case L'[':
		case L']':
		case L'^':
		case L'-':
		case L'+':
		case L'(':
		case L')':
		case L'{':
		case L'}':
		case L'$':
		case L'|':
		case L'!':
			*pDst++ = L'\\';
			*pDst++ = wc;
			break;
		case L'?':
			if(pwc != L'\\')
			{
				*pDst++ = L'[';
				*pDst++ = L'[';
				*pDst++ = L':';
				*pDst++ = L'p';
				*pDst++ = L'r';
				*pDst++ = L'i';
				*pDst++ = L'n';
				*pDst++ = L't';
				*pDst++ = L':';
				*pDst++ = L']';
				*pDst++ = L']';
			}
			else
				*pDst++ = L'?';
			break;
		case L'*':
			if(pwc != L'\\')
				*pDst++ = L'.';
			*pDst++ = L'*';
			break;
		case L'\\':
			if(*pSrc == L'?' || *pSrc == L'*')
			{
				*pDst++ = wc;
			}
			else
			{
				*pDst++ = L'\\';
				*pDst++ = wc;
			}
			break;
		case L' ':
			*pDst++ = L'\\';
			*pDst++ = L's';
			*pDst++ = L'+';
			break;
		default:
			*pDst++ = wc;
		}
		pwc = wc;
	}

	*pDst++ = 0;

	cStringObj result(pResBuffer);
	delete []pResBuffer;

	if(rule[0] == '*')
	{
		result.erase(0, 2);
	}
	else
	{
		cStringObj s("^");
		s += result;
		result = s;
	}

	if(rule[rule.length() - 1] == '*')
	{
		result.erase(result.length() - 2, 2);
	}
	else
	{
		result += "$";
	}

	rule = result;

	return true;
}

tDWORD BWFind::AsBwListFindPattern(
	const cAsBwListRules &patterns,
	const wchar_t *text,
	tDWORD dwMaxLevel,
	std::set<tSIZE_T> &found_indices)
{
	found_indices.clear();

	if(!wcslen(text))
		return 0;

	const tDWORD maxAllowedElems = 1; // оказалось, это быстрее всего

	tDWORD dwLevel = 0;

	tSIZE_T dwPatternSize = patterns.size(), current_idx = 0;
	tSIZE_T ind_table[maxAllowedElems];
	while(current_idx < dwPatternSize)
	{
		cStringObj search_pattern;
		tSIZE_T i;
		for(i = 0; i < maxAllowedElems && current_idx < dwPatternSize; ++current_idx)
		{
			const cAsBwListRule &rule = patterns[current_idx];
			if(rule.m_bEnabled)
			{
				cStringObj pattern = rule.m_sRule;
				if(ConvertBwRule2RegExp(pattern) && !pattern.empty())
				{
					if(!search_pattern.empty())
						search_pattern += "|";
					search_pattern += "(";
					search_pattern += pattern;
					search_pattern += ")";
					ind_table[i++] = current_idx;
				}
			}
		}
		if(!search_pattern.empty())
		{
			try
			{
				PR_TRACE((0, prtNOT_IMPORTANT, "BWFind: searching using pattern '%ls'", search_pattern.data()));
				const boost::wregex regexp(search_pattern.data(), boost::regex::perl | boost::regex::icase);
				const wchar_t *p = text;
				boost::wcmatch what; 
				unsigned flags = boost::match_default;
				while(boost::regex_search(p, what, regexp, flags))
				{
					bool bFound = false;
					tSIZE_T idx, size;
					for(idx = 1, size = what.size(); idx < size; ++idx)
					{
						const wchar_t *s, *e;
						if((s = what[idx].first) && (e = what[idx].second) && e > s)
						{
							bFound = true;
							idx--;
							break;
						}
					}
					if(bFound)
					{
						idx = ind_table[idx];
						if(idx < dwPatternSize)
						{
							if(found_indices.find(idx) == found_indices.end())
							{
								found_indices.insert(idx);
								const cAsBwListRule &foundrule = patterns[idx];
								dwLevel += foundrule.m_dwLevel;
								PR_TRACE((0, prtNOTIFY, "BWFind: (%d, '%ls', %d)", idx, foundrule.m_sRule.data(), foundrule.m_dwLevel));
								if(dwLevel >= dwMaxLevel)
									return dwLevel;
							}
						}
						else
						{
							PR_TRACE((0, prtIMPORTANT, "BWFind: wrong index %d", idx));
							PR_ASSERT(0);
						}
					}
					else
					{
						PR_ASSERT(0);
						PR_TRACE((0, prtIMPORTANT, "BWFind: not found"));
					}
					p = what[0].first + 1;
					flags |= boost::match_prev_avail; 
					flags |= boost::match_not_bob; 	
				}
			}
			catch(std::exception &e)
			{
				PR_TRACE((0, prtERROR, "Exception '%s' during regexp match", e.what()));
			}
		}
	}

	return dwLevel;
}

tDWORD BWFind::AsBwListFindPattern(
						   const cAsBwListRules &patterns,
						   const char *text,
						   tDWORD dwMaxLevel,
						   std::set<tSIZE_T> &found_indices)
{
	found_indices.clear();
	wchar_t *pwText = NULL;
	int nReqSize;
	if((nReqSize = MultiByteToWideChar(CP_ACP, 0, text, -1, pwText, 0)) == 0)
		return 0;
	pwText = new wchar_t[nReqSize];
	if(!pwText)
		return 0;
	tDWORD dwLevel = 0;
	if(MultiByteToWideChar(CP_ACP, 0, text, -1, pwText, nReqSize))
		dwLevel = BWFind::AsBwListFindPattern(patterns, pwText, dwMaxLevel, found_indices);
	delete []pwText;
	return dwLevel;
}