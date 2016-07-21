#ifndef WINFILESTORAGERW_H
#define WINFILESTORAGERW_H

#include <string>
#include <vector>
#include <LicensingInterface.h>

namespace KasperskyLicensing{

class WinFileStorageRW : public IStorageRW
{
public:

	explicit WinFileStorageRW(const char_t* path, const char_t* pattern);
	/**
	 * loads stored object by given name
	 */
	virtual AutoPointer<IObjectImage> LoadObject(const char_t* name) const;

	/**
	 * returns stored object names
	 */
	virtual NameEnumerator GetObjectNames() const;
	
	/**
	 * adds object to storage
	 */
	virtual void StoreObject(const IObjectImage& obj);

	/**
	 * removes object from storage by given name
	 */
	virtual void RemoveObject(const char_t* name);

private:

	// ensures that specified object exists
	static void EnsureObjectExists(const std::basic_string<char_t>& path, unsigned mask);

	// directory and pattern
	std::basic_string<char_t> dir, pat;
};

class ObjectImage : public IObjectImage
{
public:

	ObjectImage(const char_t* name, size_t size);

	/**
	 * returns stored object name
	 */
	virtual const char_t* GetObjectName() const;

	/**
	 * returns size of stored object image
	 */
	virtual unsigned int GetObjectSize() const;

	/**
	 * returns stored object image
	 */
	virtual const void* GetObjectImage() const;

private:

	std::vector<char> image;
	std::basic_string<char_t> obj_name;
};

}

#endif // WINFILESTORAGERW_H