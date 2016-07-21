#include "CRC32.h"
#include "sq_u.h"
#include "sqhdr.h"
#include "squeeze.h"
#include <vector>
#include <string>
#include <errno.h>

using namespace std;
using namespace Squeeze;


int Squeeze::unsqueeze(CFile& file, vector<CFile*>& files, TTracer* ptracer)
{
    if (file.body.size() < sizeof(CSqueezeHeader))
    {
		if (ptracer) ptracer("\"%s\" is not squeezed file\n", file.filename.c_str());
		return 1;
    }
       
	CSqueezeHeader& header = *(CSqueezeHeader *) &file.body.front();

	if (header.dwSignature != dwSqueezeHeaderSignature)
	{
		if (ptracer) ptracer("\"%s\" is not squeezed file\n", file.filename.c_str());
		return 1;
	}

	if (header.dwVersion != dwSqueezeCurrentVersion)
	{
		if (ptracer) ptracer("\"%s\" has been squeezed by not supported version of squeeze\n", file.filename.c_str());
		return 1;
	}

    BYTE * filefront = &file.body.front() + sizeof(CSqueezeHeader);
    DWORD  filesize = file.body.size() - sizeof(CSqueezeHeader);

	if (header.dwCompressedSize > filesize)
	{
		if (ptracer) ptracer("\"%s\" is broken\n", file.filename.c_str());
		return 1;
	}
	file.body.resize(header.dwCompressedSize + sizeof(CSqueezeHeader));
    filesize = header.dwCompressedSize;
	
	if (header.dwCrc != KLUTIL::crc32(0, filefront, filesize))
	{
		if (ptracer) ptracer("Error: CRC check failed\n");
		return 1;
	}

	vector<BYTE> origFile(header.dwOriginalSize);
	if (unsqu(filefront, &origFile.front()) != header.dwOriginalSize)
	{
		if (ptracer) ptracer("Error: fatal error\n");
		return 1;
	}

	vector<BYTE>::iterator it, end;
	for (it = origFile.begin(), end = origFile.end(); it != end; ++it)
		*it ^= 0xB3;

	for (it = origFile.begin(); it < origFile.end(); )
	{
		CSqueezeFile& fileHeader = (CSqueezeFile&)(*it);
		if (fileHeader.dwSignature != dwFileSignature)
		{
			if (ptracer) ptracer("\"%s\" is broken\n", file.filename.c_str());
			return 1;
		}
		it += sizeof(CSqueezeFile);
		string filename;
		filename.assign((char*)&(*it), fileHeader.dwFilenameSize);
		it += fileHeader.dwFilenameSize;
		CFile* pFile = new CFile();
		pFile->filename = filename;
		pFile->body.assign(it, it + fileHeader.dwFileSize);
		it += fileHeader.dwFileSize;
        files.push_back(pFile);
	}

	if (it != end)
	{
		if (ptracer) ptracer("\"%s\" is broken\n", file.filename.c_str());
		return 1;
	}
	
	return 0;
}
