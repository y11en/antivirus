#include "CRC32.h"
#include "sq_s.h"
#include "sqhdr.h"
#include "squeeze.h"
#include <vector>
#include <string>
#include <errno.h>
#include <stdarg.h>

using namespace std;
using namespace Squeeze;

void ReadFile(FILE* f, vector<Squeeze::BYTE>& file);

void printf_tracer(const char *str...)
{
    printf("Error: ");

    va_list list;
    va_start(list, str);
    vprintf(str, list);
    va_end(list);

    printf("\n");
}

int Squeeze::unsqueeze(const char* pszInFile, const char* pszFolder /*= NULL*/, const char* pszName /*= NULL*/)
{
	vector<CFile*> files;
	int res = unsqueeze(pszInFile, files, &printf_tracer);
	if (res != 0)
		return res;

	string folder;
	if (pszFolder && *pszFolder)
	{
		folder = pszFolder;
        char c = *folder.rbegin();
		if (*folder.rbegin() != '\\' && *folder.rbegin() != '/')
			folder += '\\';
	}

	for (int i = 0; i < files.size(); ++i)
	{
		CFile* pFile = files[i];
        if ( 0 == pFile->filename.size() )
            continue;
		string name = pFile->filename;
        if (pszName && *pszName)
        {
            name = pszName;
            string::size_type nExtPos = pFile->filename.rfind('.');
            if ( std::string::npos == nExtPos )
                nExtPos = 0;
            name.append(pFile->filename, nExtPos, pFile->filename.size()-1 );
        }
		string path = folder + name;
		FILE* fo = fopen(path.c_str(), "wb");
		if (!fo)
		{
			printf("Error: failed to open file \"%s\" (%s)\n", path.c_str(), strerror(errno));
			return 1;
		}
		
		if (fwrite(&pFile->body.front(), pFile->body.size(), 1, fo) != 1)
		{
			fclose(fo);
			printf("Error: failed to save file \"%s\" (%s)\n", path.c_str(), strerror(errno));
			return 1;
		}
		
		fclose(fo);

		delete pFile;
	}

	return 0;
}

int Squeeze::squeeze(const vector<string>& files, const char* pszOutFile)
{
	vector<Squeeze::BYTE> file;

	for (int i = 0; i < files.size(); ++i)
	{
		string filename = files[i];
		string::size_type sepPos = filename.rfind('\\');
		if (sepPos != string::npos)
			filename.erase(0, sepPos + 1);
		sepPos = filename.rfind('/');
		if (sepPos != string::npos)
			filename.erase(0, sepPos + 1);
		
		FILE* fi = fopen(files[i].c_str(), "rb");
		if (!fi)
		{
			printf("Error: failed to open file \"%s\" (%s)\n", files[i].c_str(), strerror(errno));
			return 1;
		}
		
		vector<Squeeze::BYTE> body;
		ReadFile(fi, body);
		
		fclose(fi);

		CSqueezeFile fl;
		fl.dwSignature = dwFileSignature;
		fl.dwFileSize = body.size();
		fl.dwFilenameSize = filename.size();

		file.insert(file.end(), (Squeeze::BYTE*)(&fl), (Squeeze::BYTE*)(&fl + 1));
		file.insert(file.end(), (Squeeze::BYTE*)filename.c_str(), (Squeeze::BYTE*)(filename.c_str() + filename.size()));
		file.insert(file.end(), body.begin(), body.end());
	}

	for (vector<Squeeze::BYTE>::iterator it = file.begin(), end = file.end(); it != end; ++it)
		*it ^= 0xB3;
	
	CSqueezeHeader header;
	header.dwSignature = dwSqueezeHeaderSignature;
	header.dwVersion = dwSqueezeCurrentVersion;
	header.dwHeaderSize = sizeof(header);
	header.dwOriginalSize = file.size();
	header.dwCompressedSize = 0;

	FILE* fo = fopen(pszOutFile, "w+b");
	if (!fo)
	{
		printf("Error: failed to open file \"%s\" (%s)\n", pszOutFile, strerror(errno));
		return 1;
	}

	if (fwrite(&header, sizeof(header), 1, fo) != 1)
	{
		fclose(fo);
		printf("Error: failed to save file \"%s\" (%s)\n", pszOutFile, strerror(errno));
		return 1;
	}

	header.dwCompressedSize = squeeze(&file.front(), fo, file.size());
	if (!file.empty() && header.dwCompressedSize == 0)
	{
		fclose(fo);
		printf("Error: failed to save file \"%s\" (%s)\n", pszOutFile, strerror(errno));
		return 1;
	}

	fseek(fo, sizeof(CSqueezeHeader), SEEK_SET);

	vector<Squeeze::BYTE> compressedFile;
	ReadFile(fo, compressedFile);
	header.dwCrc = KLUTIL::crc32(0, &compressedFile.front(), compressedFile.size());

	fseek(fo, 0, SEEK_SET);

	if (fwrite(&header, sizeof(header), 1, fo) != 1)
	{
		fclose(fo);
		printf("Error: failed to save file \"%s\" (%s)\n", pszOutFile, strerror(errno));
		return 1;
	}

	fclose(fo);
	
	return 0;
}
