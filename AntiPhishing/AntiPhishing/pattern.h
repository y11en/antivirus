#ifndef __PATTERN_MATCH_
#define __PATTERN_MATCH_

#ifdef __cplusplus
extern "C" {
#endif

bool MatchWithPatternW(unsigned short* Pattern, unsigned short* Name, bool CaseSensivity, bool bPatternWithAsterics);
bool MatchWithPatternA(char* Pattern, char* Name, bool CaseSensivity, bool bPatternWithAsterics);

#ifdef __cplusplus
} // extern "C"
#endif


#endif // __PATTERN_MATCH_