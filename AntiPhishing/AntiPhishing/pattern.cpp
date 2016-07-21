#include "pattern.h"


__inline bool MatchOkayW(unsigned short* pattern)
{

	if (*pattern == '*')
		pattern++;
	if (*pattern)
		return false;
	return true;
}

bool MatchWithPatternW(unsigned short* Pattern, unsigned short* Name, bool CaseSensivity, bool bPatternWithAsterics)
{
	unsigned short ch;
	unsigned short pc;

	if (!Pattern || !Name)
		return false;
	
	if(*Pattern=='*')
	{
		do Pattern++; while (*Pattern=='*');

		while(*Name && *Pattern)
		{
			pc = *Pattern;
			ch = *Name;
			if (CaseSensivity == false)
			{
				if (ch >= 'A' && ch <= 'Z')
					ch |= 0x20;
				if (pc >= 'A' && pc <= 'Z')
					pc |= 0x20;
			}
			if ((ch == '*') || (pc == ch) || (pc == '?'))
			{
				if(MatchWithPatternW(Pattern+1,Name+1, CaseSensivity, bPatternWithAsterics)) 
					return true;
			}
			
			Name++;
		}
		return MatchOkayW(Pattern);
	} 
	
	while(*Name && *Pattern != '*')
	{
		pc = *Pattern;
		ch = *Name;
		if (CaseSensivity == false)
		{
			if (ch >= 'A' && ch <= 'Z')
				ch |= 0x20;
			if (pc >= 'A' && pc <= 'Z')
				pc |= 0x20;
		}
		if(pc == ch || pc == '?')
		{
			Pattern++;
			Name++;
		} 
		else
		{
			if (pc == 0 && bPatternWithAsterics)
			{
				if (ch == '\\' || ch == '/')
					return true;
			}
			return false;
		}

	}
	if(*Name)
		return MatchWithPatternW(Pattern,Name, CaseSensivity, bPatternWithAsterics);
	
	return MatchOkayW(Pattern);
}


__inline bool MatchOkayA(char* pattern)
{
	if (*pattern == '*')
		pattern++;
	if (*pattern)
		return false;
	return true;
}

bool MatchWithPatternA(char* Pattern, char* Name, bool CaseSensivity, bool bPatternWithAsterics)
{
	char ch;
	char pc;

	if (!Pattern || !Name)
		return false;
	
	if(*Pattern=='*')
	{
		Pattern++;
		while(*Name && *Pattern)
		{
			pc = *Pattern;
			ch = *Name;
			if (CaseSensivity == false)
			{
				if (ch >= 'A' && ch <= 'Z')
					ch |= 0x20;
				if (pc >= 'A' && pc <= 'Z')
					pc |= 0x20;
			}
			if ((ch == '*') || (pc == ch) || (pc == '?'))
			{
				if(MatchWithPatternA(Pattern+1,Name+1, CaseSensivity, bPatternWithAsterics)) 
					return true;
			}
			
			Name++;
		}
		return MatchOkayA(Pattern);
	} 
	
	while(*Name && *Pattern != '*')
	{
		pc = *Pattern;
		ch = *Name;
		if (CaseSensivity == false)
		{
			if (ch >= 'A' && ch <= 'Z')
				ch |= 0x20;
			if (pc >= 'A' && pc <= 'Z')
				pc |= 0x20;
		}
		if(pc == ch || pc == '?')
		{
			Pattern++;
			Name++;
		} 
		else
		{
			if (pc == 0 && bPatternWithAsterics)
			{
				if (ch == '\\' || ch == '/')
					return true;
			}
			return false;
		}

	}
	if(*Name)
		return MatchWithPatternA(Pattern,Name, CaseSensivity, bPatternWithAsterics);
	
	return MatchOkayA(Pattern);
}

