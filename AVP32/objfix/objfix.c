#include <stdio.h>
#include <malloc.h>
#include <windows.h>

enum ERR {
	ERR_OK = 0, 
	ERR_BADARG, 
	ERR_FOPEN, 
	ERR_BADFILE, 
	ERR_MEM, 
	ERR_WRITE
};

static const BYTE Rdata[] = {'.', 'r', 'd', 'a', 't', 'a' };

static __inline void FixSecHdrs(IMAGE_SECTION_HEADER* pSecHdr, WORD SecNum)
{
	WORD I;
	BYTE SecName[IMAGE_SIZEOF_SHORT_NAME + 1];

	printf("\n   Sections:\n");
	SecName[IMAGE_SIZEOF_SHORT_NAME] = 0;
	for(I = 0; I < SecNum; I++)
	{
		memcpy(SecName, pSecHdr[I].Name, IMAGE_SIZEOF_SHORT_NAME);
		printf("   Name = %s, Characteristics = 0x%.8X", SecName, pSecHdr[I].Characteristics);
		
		if( memcmp(SecName, Rdata, sizeof(Rdata)) == 0 && 
			(pSecHdr[I].Characteristics & IMAGE_SCN_MEM_WRITE) == 0 )
		{
			pSecHdr[I].Characteristics |= IMAGE_SCN_CNT_CODE;
			printf(" - fixed");
		}

		printf("\n");

	}
	printf("\n");
}

static __inline int Fix(FILE* File)
{
	IMAGE_FILE_HEADER     FileHdr;
	IMAGE_SECTION_HEADER* pSecHdr;
	size_t                SecHdrsSize;
	
	/* read file header */
	printf(" + reading file header ... ");
	fseek(File, 0, SEEK_SET);
	if(fread(&FileHdr, 1, sizeof(FileHdr), File) != sizeof(FileHdr))
	{
		printf("error\n");
		return(ERR_BADFILE);
	}
	printf("ok\n");

	/* check machine type */
	printf(" + checking machine type ... ");
	if(FileHdr.Machine != IMAGE_FILE_MACHINE_I386)
	{
		printf("error\n");
		return(ERR_BADFILE);
	}
	printf("ok\n");

	printf("\n   NumberOfSections = %d\n\n", FileHdr.NumberOfSections);

	/* alloc memory for section headers */
	printf(" + allocating memory for section headers ... ");
	SecHdrsSize = FileHdr.NumberOfSections * sizeof(*pSecHdr);
	pSecHdr = malloc(SecHdrsSize);
	if(pSecHdr == NULL)
	{
		printf("error\n");
		return(ERR_MEM);
	}
	printf("ok\n");

	/* read section headers */
	printf(" + reading section headers ... ");
	if(fread(pSecHdr, 1, SecHdrsSize, File) != SecHdrsSize)
	{
		printf("error\n");
		free(pSecHdr);
		return(ERR_BADFILE);
	}
	printf("ok\n");

	FixSecHdrs(pSecHdr, FileHdr.NumberOfSections);

	/* write section headers */
	printf(" + writing section headers ... ");
	fseek(File, sizeof(FileHdr), SEEK_SET);
	if(fwrite(pSecHdr, 1, SecHdrsSize, File) != SecHdrsSize)
	{
		printf("error\n");
		free(pSecHdr);
		return(ERR_WRITE);
	}
	printf("ok\n");

	free(pSecHdr);

	return(ERR_OK);
}

int main(int Argc, char** Argv)
{
	FILE* File;
	int   Res;

	printf("COFF fixer by Alexander Liskin\nSet code flag in sections with read-only data\n");

	if(Argc != 2)
	{
		printf("\nUsage: objfix.exe [file]\n");
		return(ERR_BADARG);
	}

	/* open file */
	printf("\n + opening file ... ");
	File = fopen(Argv[1], "rb+");
	if(File == NULL)
	{
		printf("error\n");
		return(ERR_FOPEN);
	}
	printf("ok\n");

	/* fix file */
	Res = Fix(File);

	/* close file */
	fclose(File);

	if(Res == ERR_OK)
	{
		printf("\nFixed ok\n");
	}
	else
	{
		printf("\nFailed to fix\n");
	}

	return(Res);
}
