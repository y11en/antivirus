/*
*/

#if !defined(__KL_DSKMWRAP_TEST_FILEUTILS_H)
#define __KL_DSKMWRAP_TEST_FILEUTILS_H

#include <string>

namespace KL  {
namespace DskmLibWrapper  {
namespace Test  {

class FolderGuard
{
public:
	FolderGuard(const char* path, bool create_if_not_exists);
	~FolderGuard();
private:
	std::string m_cur;
};

bool ReadDataFromFile(const char* file_name, int pos, char* pData, size_t nDataSize);
bool WriteDataToFile(const char* file_name, int pos, const char* pData, size_t nDataSize);
bool CreateFileAndWriteData(const char* file_name, const char* pData, size_t nDataSize);
bool DeleteFile(const char* path);

}  // namespace Test
}  // namespace DskmLibWrapper
}  // namespace KL

#endif  // !defined(__KL_DSKMWRAP_TEST_FILEUTILS_H)
