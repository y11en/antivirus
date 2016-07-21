// GetFileSectors.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"


int _tmain(int argc, _TCHAR* argv[])
{
	
// 	argv[1] = L"d:\\myfile.txt";
// 	argc = 2;
	if (argc < 2)
	{
		printf("usage example: c:\\myfile.txt \n");
		return -1;
	}
	
	HANDLE hDevHandle=INVALID_HANDLE_VALUE;
	INT res=-1;
	PRETRIEVAL_POINTERS_BUFFER  prpb=NULL;
	
	LARGE_INTEGER file_size;
	
	DWORD SectorsPerCluster;
	DWORD BytesPerSector;
	DWORD NumberOfFreeClusters;
	DWORD TotalNumberOfClusters;	

	WCHAR fs_path[4]; //путь в виде "C:\\"
	memset(fs_path, 0, 4*sizeof(WCHAR) );
	memcpy( fs_path, argv[1], 3*sizeof(WCHAR) );
		
	if (!GetDiskFreeSpaceW(
						fs_path, 
						&SectorsPerCluster, 
						&BytesPerSector, 
						&NumberOfFreeClusters, 
						&TotalNumberOfClusters
						)
		)
	{
		printf("GetDiskFreeSpaceW error\n");
		return -1;
	}

	hDevHandle = CreateFileW(
		argv[1],//fs_path,
		GENERIC_READ /*| GENERIC_WRITE*/,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL  | FILE_FLAG_NO_BUFFERING,
		NULL
		);

	if (hDevHandle==INVALID_HANDLE_VALUE)
	{
		printf("CreateFileW error\n");
		return -1;
	}
	if (GetFileSizeEx(hDevHandle,&file_size)<0) 
	{
		printf("GetFileSizeEx error\n");
		return -1;
	}

	DWORD file_cl_cnt; 
	DWORD cluster_size=SectorsPerCluster*BytesPerSector;
	if (file_size.QuadPart< cluster_size || file_size.QuadPart%cluster_size>0)
	{
		file_cl_cnt=(DWORD)(file_size.QuadPart/cluster_size+1);
	}
	else
	{
		file_cl_cnt=(DWORD)(file_size.QuadPart/cluster_size);
	}

	STARTING_VCN_INPUT_BUFFER start_vcn;
	start_vcn.StartingVcn.QuadPart=file_cl_cnt-1;

	prpb=(PRETRIEVAL_POINTERS_BUFFER )new char[sizeof(RETRIEVAL_POINTERS_BUFFER )+sizeof(LARGE_INTEGER)*2*10];
	DWORD nRead;
	if (!DeviceIoControl(hDevHandle,
		FSCTL_GET_RETRIEVAL_POINTERS,
		&start_vcn,
		sizeof(STARTING_VCN_INPUT_BUFFER),
		prpb, 
		sizeof(RETRIEVAL_POINTERS_BUFFER )+sizeof(LARGE_INTEGER)*2*10,
		&nRead,
		NULL))
	{
		//очень маленький файл находиться в MFT
		DWORD err = GetLastError();
		if (err == ERROR_HANDLE_EOF )
			printf("DeviceIoControl error: file is too small\n");
		else
			printf("DeviceIoControl error: GetLastError = %d \n", err);
		
	}
	else
	{
		printf("DeviceIoControl success\n");
	}
	
	CloseHandle(hDevHandle);
	hDevHandle=NULL;
	
	delete [] prpb;

	return 0;
}

