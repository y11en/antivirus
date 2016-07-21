#include <vector>
#include <string>

namespace Squeeze
{
    typedef unsigned char BYTE;
    typedef unsigned long DWORD;

    const DWORD dwSqueezeCurrentVersion = 1;

    struct CFile
    {
	    std::string filename;
	    std::vector<BYTE> body;
    };

    typedef void TTracer(const char *str...);

    int unsqueeze(CFile& file, std::vector<CFile*>& files, TTracer* ptracer = NULL);
    int unsqueeze(const char* pszInFile, std::vector<CFile*>& files, TTracer* ptracer = NULL);

    int unsqueeze(const char* pszInFile, const char* pszFolder = NULL, const char* pszName = NULL);
    int squeeze(const std::vector<std::string>& files, const char* pszOutFile);
}
