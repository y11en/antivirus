#if !defined(AFX_UNPACKER_H__B920BD07_F6E5_437C_8036_723EBF2DA29B__INCLUDED_)
#define AFX_UNPACKER_H__B920BD07_F6E5_437C_8036_723EBF2DA29B__INCLUDED_

bool UnpackOSFile (LPCWSTR szwPackedFilePath, LPCWSTR szwBaseFileToUnpack, LPCWSTR szwDestDirectory);
bool UnpackFile (LPCWSTR szwPackedFilePath, LPCWSTR szwFileToUnpack, LPCWSTR szwDestFile, bool bXor = FALSE);

bool UnpackIsFile(LPCWSTR szwPackedFilePath, LPCWSTR szwFileToUnpack);

bool UnpackFileToMemory(LPCWSTR szwPackedFilePath, LPCWSTR szwFileToUnpack, const char *& buffer, int & size);
void UnpackFreeMemory(const char *buffer);

std::wstring RemoveExtension (LPCWSTR string);
std::wstring RemoveExtension (const std::wstring &string);

#endif // !defined(AFX_UNPACKER_H__B920BD07_F6E5_437C_8036_723EBF2DA29B__INCLUDED_)
