#include <windows.h>
#include "WinFileStorageR.h"

namespace KasperskyLicensing{


//-----------------------------------------------------------------------------

WinFileStorageR::WinFileStorageR(const char_t* path, const char_t* pattern) 
	: impl(path, pattern)
{
}

//-----------------------------------------------------------------------------

AutoPointer<IObjectImage> WinFileStorageR::LoadObject(const char_t* name) const
{
	return impl.LoadObject(name);
}

//-----------------------------------------------------------------------------

IStorageR::NameEnumerator WinFileStorageR::GetObjectNames() const
{
	return impl.GetObjectNames();
}

//-----------------------------------------------------------------------------

}