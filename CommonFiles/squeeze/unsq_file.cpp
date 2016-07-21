#include "CRC32.h"
#include "sq_u.h"
#include "sqhdr.h"
#include "squeeze.h"
#include <vector>
#include <string>
#include <errno.h>

using namespace std;
using namespace Squeeze;

void ReadFile(FILE* f, vector<Squeeze::BYTE>& file)
{
	Squeeze::BYTE buf[1024*16];
	int nReaded;
	while ((nReaded = fread(buf, 1, sizeof(buf), f)) > 0)
		file.insert(file.end(), buf, buf + nReaded);
}

int Squeeze::unsqueeze(const char* pszInFile, vector<CFile*>& files, TTracer* ptracer)
{
	FILE* fi = fopen(pszInFile, "rb");
	if (!fi)
	{
		if (ptracer) ptracer("Error: failed to open file \"%s\" (%s)\n", pszInFile, strerror(errno));
		return 1;
	}
	
    CFile file;
    file.filename = pszInFile;
	ReadFile(fi, file.body);
	
	fclose(fi);

    return unsqueeze(file, files, ptracer);
}

