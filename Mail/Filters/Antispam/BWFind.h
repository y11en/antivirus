#ifndef __BWFIND_H__
#define __BWFIND_H__

#include <Prague/prague.h>
#include <Mail/structs/s_antispam.h>
#include <set>

namespace BWFind
{
	tDWORD AsBwListFindPattern(
				const cAsBwListRules &patterns,
				const wchar_t *text,
				tDWORD dwMaxLevel,
				std::set<tSIZE_T> &found_indices);
	tDWORD AsBwListFindPattern(
		const cAsBwListRules &patterns,
		const char *text,
		tDWORD dwMaxLevel,
		std::set<tSIZE_T> &found_indices);

} // namespace BWFind

#endif