#include "../metadata.h"
#include "metasaver.h"

int save(const char* filename, PragueMetadata& metadata)
{
	MetaSaver<SaveValue> saver;
	if(!saver.open(filename))
		return 0;
	return saver.save(metadata);
}

#if defined(_WIN32)
int save(const wchar_t* filename, PragueMetadata& metadata)
{
	MetaSaver<SaveValue> saver;
	if(!saver.open(filename))
		return 0;
	return saver.save(metadata);
}
#endif // _WIN32
