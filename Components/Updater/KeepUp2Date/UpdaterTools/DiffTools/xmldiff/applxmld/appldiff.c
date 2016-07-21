#include <stdio.h>

#include "xml_diff.h"

int __cdecl main(int argc, char* argv[])
{
	tFile sOldFile, sDiffFile, sOutFile, sTempFile1, sTempFile2;
	tHeap sHeap;
	bool bRes;

	InitHeapRtl(&sHeap);
	
	if (argc < 4)
	{
		printf("usage: %s <old_file> <diff_file> <out_file>" , argv[0]);
		return 1;
	}
	
	if (!InitFileStdioN(&sOldFile, argv[1], "rb"))
	{
		printf("Error: cannot open old_file ('%s')\n", argv[1]);
		return 2;
	}
	
	if (!GetXMLTimeStamp(&sOldFile))
	{
		printf("Error: cannot retrive XML timestamp (UpdateDate field) from '%s'\n", argv[1]);
		return 3;
	}

	if (!InitFileStdioN(&sDiffFile, argv[2], "rb"))
	{
		printf("Error: cannot open diff_file ('%s')\n", argv[2]);
		return 2;
	}	

	if (!InitFileStdioN(&sOutFile, argv[3], "wb+"))
	{
		printf("Error: cannot open out_file ('%s')\n", argv[3]);
		return 2;
	}	

	if (!InitFileMemio(&sTempFile1, &sHeap))
	{
		printf("Error: cannot init temporary file (1)\n");
		return 2;
	}	
	
	if (!InitFileMemio(&sTempFile2, &sHeap))
	{
		printf("Error: cannot init temporary file (1)\n");
		return 2;
	}	

	bRes = ApplyXmlDiffSet(&sOldFile, &sDiffFile, &sOutFile, &sTempFile1, &sTempFile2);

	sTempFile2.Close(&sTempFile2);
	sTempFile1.Close(&sTempFile2);
	sOutFile.Close(&sOutFile);
	sDiffFile.Close(&sDiffFile);
	sOldFile.Close(&sOldFile);

	if (!bRes)
	{
		printf("Error: cannot apply diff '%s' to file '%s'\n", argv[2], argv[1]);
		unlink(argv[3]);
	}
	
	return 0;
}
/*
tDiffList pDiffList;

old+diffset->temp
if (temp!=new)
{
	MakeDiff(old, new, diff);
	MakeDiff(temp, new, incdiff);
	if (incdiff < diff)
		AddDiff(pList, old_ver, incdiff);
}

base = new;
for (;;)
{
old = ...;
MakeDiff(old, new, fulldiff);

BuildBase(old, new, base);
MakeDiff(base, new, basediff);

ApplyDiff(old, diffset, test)
MakeDiff(test, new, incdiff);

if (fulldiff < basediff && fulldiff < incdiff)
{
	// use full diff
	if (diff)
		AddDiff2Set(diff);
	diff = fulldiff;
	diff.from_ver = diff.to_ver = old_ver;
} 
else if (incdiff < basediff)
{
	// use incdiff
	
}
else
{
	// use basediff
	diff = basediff;
	diff.to_ver = old_ver;
}
*/

