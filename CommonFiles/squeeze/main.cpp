#include <stdio.h>
#include "squeeze.h"

using namespace std;
using namespace Squeeze;

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printf("Squeeze tool, version %i\n", dwSqueezeCurrentVersion);
		printf("Usage: %s <src-files> <dst-file>\n", argv[0]);
		printf("       %s -u <src-file>\n", argv[0]);
		return 1;
	}

	bool bUnsqu = strcmp(argv[1], "-u") == 0;
	if (bUnsqu)
		return unsqueeze(argv[2]);

	vector<string> inFiles;
	for (int i = 1; i < argc - 1; ++i)
		inFiles.push_back(argv[i]);

	return squeeze(inFiles, argv[argc - 1]);
}

