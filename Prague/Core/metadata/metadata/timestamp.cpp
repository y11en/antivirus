#include "metaloader.h"

#if defined(_WIN32)
bool getTimeStamp(const char* filename, TimeStamp& timestamp)
{
	HANDLE handle = CreateFileA(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(handle == INVALID_HANDLE_VALUE)
		return false;
	return GetFileTime(handle, NULL, NULL, &timestamp) != 0;
}

bool getTimeStamp(const wchar_t* filename, TimeStamp& timestamp)
{
	HANDLE handle = CreateFileW(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(handle == INVALID_HANDLE_VALUE)
		return false;
	return GetFileTime(handle, NULL, NULL, &timestamp) != 0;
}

bool checkTimeStamp(const char* filename, const TimeStamp& timestamp)
{
	TimeStamp ts;
	if(!getTimeStamp(filename, ts))
		return false;
	if(CompareFileTime(&ts, &timestamp))
		return false;
	return true;
}

bool checkTimeStamp(const wchar_t* filename, const TimeStamp& timestamp)
{
	TimeStamp ts;
	if(!getTimeStamp(filename, ts))
		return false;
	if(CompareFileTime(&ts, &timestamp))
		return false;
	return true;
}

#else // _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
bool getTimeStamp(const char* filename, TimeStamp& timestamp)
{
	struct stat st;
	if(stat(filename, &st))
		return false;
	timestamp = st.st_mtime;
	return true;
}

bool checkTimeStamp(const char* filename, const TimeStamp& timestamp)
{
	TimeStamp ts;
	if(!getTimeStamp(filename, ts))
		return false;
	if(ts != timestamp)
		return false;
	return true;
}

#endif // _WIN32
