BOOL ScanDir(char* szPath,
			 int ProcessFile(char* szPath,char* szFileName),
			 BOOL bScanSubfolders);
BOOL ScanDir2(LPSTR szPath, LPSTR szFileMask,
			 int ProcessFile(LPSTR szPath,LPSTR szFileName),
			 BOOL bScanSubfolders);
int ProcessFileScan(LPSTR szPath,LPSTR szFileName);
int ProcessFileTest(LPSTR szPath,LPSTR szFileName);
int ProcessFileScanAsAVP(LPSTR szPath,LPSTR szFileName);
int ProcessFileDir(LPSTR szPath,LPSTR szFileName);

