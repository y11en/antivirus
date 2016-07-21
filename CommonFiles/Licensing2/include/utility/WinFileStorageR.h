#ifndef WINFILESTORAGER_H
#define WINFILESTORAGER_H

#include <LicensingInterface.h>
#include "WinFileStorageRW.h"

namespace KasperskyLicensing{


class WinFileStorageR : public IStorageR
{
public:

	explicit WinFileStorageR(const char_t* path, const char_t* pattern);
	/**
	 * loads stored object by given name
	 */
	virtual AutoPointer<IObjectImage> LoadObject(const char_t* name) const;

	/**
	 * returns stored object names
	 */
	virtual NameEnumerator GetObjectNames() const;
	
private:

	WinFileStorageRW impl;
};

}

#endif // WINFILESTORAGER_H