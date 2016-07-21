#pragma align 1 
#pragma pack (1)

ULONG DiskType(PLIST pList);
ULONG ExistPath(char *Path,struct stat *pstatbuf);
int copyReg(const char *src_path,const char *dst_path);
  