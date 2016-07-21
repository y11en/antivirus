
#include <string>

int random(int nMin, int nMax);
int KLPAR_CompareFiles(const wchar_t* szwFile1, const wchar_t* szwFile2);
std::wstring genRandString(int nLen);
std::wstring genRandString(int nLen, int& summa);
std::wstring MakePerThreadFileName(const wchar_t* szwTemplate);
void AcquireRandomParams(int nLevels, int nVars, int nNodes, KLPAR::Params** ppParams, long* pSum);
void AddValue(
						KLPAR::Params* pParams,
						KLPAR::ValuesFactory* pFactory,
						std::wstring& strName,
						std::wstring& strVal);

// Local Variables:
// mode: C++
// End:
