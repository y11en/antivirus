#include <windows.h>
#include "WinFileStorageRW.h"

namespace KasperskyLicensing{

//-----------------------------------------------------------------------------

class NameEnumeratorImp : public IStorageRW::NameEnumerator::Implement
{
public:
	explicit NameEnumeratorImp(const std::basic_string<char_t>& pattern);
	~NameEnumeratorImp();
	virtual IStorageRW::NameEnumerator::Implement* Clone() const;
	virtual IStorageRW::NameEnumerator::ValueType Item() const;
	virtual bool IsDone() const;
	virtual void Next();
private:
	std::basic_string<char_t> pattern;
	WIN32_FIND_DATA data;
	HANDLE hFind;
};

//-----------------------------------------------------------------------------
	
NameEnumeratorImp::NameEnumeratorImp(const std::basic_string<char_t>& p) : pattern(p)
{
	hFind = FindFirstFile(pattern.c_str(), &data);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		DWORD dwError = GetLastError();
		// no files with such name
		if (dwError == ERROR_FILE_NOT_FOUND)
			hFind = 0;
		else
			throw LicensingError(ILicensingError::LICERR_STORAGE_READ, dwError);
	}
}

//-----------------------------------------------------------------------------

IStorageRW::NameEnumerator::Implement* NameEnumeratorImp::Clone() const
{
	return new NameEnumeratorImp(pattern);
}

//-----------------------------------------------------------------------------

IStorageRW::NameEnumerator::ValueType NameEnumeratorImp::Item() const
{
	return data.cFileName;
}

//-----------------------------------------------------------------------------

bool NameEnumeratorImp::IsDone() const
{
	return !hFind;
}


//-----------------------------------------------------------------------------

NameEnumeratorImp::~NameEnumeratorImp()
{
	if (hFind)
		FindClose(hFind);
}

//-----------------------------------------------------------------------------

void NameEnumeratorImp::Next()
{
	if (!FindNextFile(hFind, &data))
	{
		DWORD dwError = GetLastError();

		if (dwError == ERROR_NO_MORE_FILES)
		{
			FindClose(hFind);
			hFind = 0;
		}
		else
			throw LicensingError(ILicensingError::LICERR_STORAGE_READ, dwError);
	}
}

//-----------------------------------------------------------------------------

WinFileStorageRW::WinFileStorageRW(const char_t* path, const char_t* pattern) 
	: dir(path)
	, pat(pattern)
{

	DWORD dwAttr = GetFileAttributes(path);
	// check for object existence
	if (dwAttr == INVALID_FILE_ATTRIBUTES)
	{
		DWORD dwError = GetLastError();

		switch (dwError)
		{
		case ERROR_FILE_NOT_FOUND:
			throw LicensingError(ILicensingError::LICERR_NOT_FOUND);
			break;
		default:
			throw LicensingError(ILicensingError::LICERR_STORAGE_READ, dwError);
			break;
		}
	}
	else if (!(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
		throw LicensingError(ILicensingError::LICERR_NOT_FOUND);

	// check for trailing slash
	if(dir.rfind('\\'))
		dir += "\\";
}

//-----------------------------------------------------------------------------

AutoPointer<IObjectImage> WinFileStorageRW::LoadObject(const char_t* name) const
{
	std::basic_string<char_t> fname = dir + name;

	DWORD dwAttr = GetFileAttributes(fname.c_str());
	// check for object existence
	if (dwAttr == INVALID_FILE_ATTRIBUTES)
	{
		DWORD dwError = GetLastError();

		switch (dwError)
		{
		case ERROR_FILE_NOT_FOUND:
			throw LicensingError(ILicensingError::LICERR_NOT_FOUND);
			break;
		default:
			throw LicensingError(ILicensingError::LICERR_STORAGE_READ, dwError);
			break;
		}
	}
	// ensure that entity is a file (not a directory)
	else if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		throw LicensingError(ILicensingError::LICERR_NOT_FOUND);

	HANDLE hFile = CreateFile(
		fname.c_str(), 
		GENERIC_READ, 
		0, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		0
	);
	// file cannot be opened
	if (!hFile)
		throw LicensingError(ILicensingError::LICERR_STORAGE_READ, GetLastError());
	
	DWORD dwSize = GetFileSize(hFile, NULL);
	// incorrect file size
	if (dwSize == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);
		throw LicensingError(ILicensingError::LICERR_STORAGE_READ, GetLastError());
	}
	// create image of given size
	AutoPointer<ObjectImage> aImage(new ObjectImage(name, dwSize));
	// store data to image
	if (!ReadFile(hFile, const_cast<void*>(aImage->GetObjectImage()), dwSize, &dwSize, NULL))
	{
		CloseHandle(hFile);
		throw LicensingError(ILicensingError::LICERR_STORAGE_READ, GetLastError());
	}

	CloseHandle(hFile);

	return aImage.Release();
}


//-----------------------------------------------------------------------------

IStorageRW::NameEnumerator WinFileStorageRW::GetObjectNames() const
{
	return new NameEnumeratorImp(dir + pat);
}

//-----------------------------------------------------------------------------

void WinFileStorageRW::StoreObject(const IObjectImage& obj)
{
	std::basic_string<char_t> fname = dir + obj.GetObjectName();

	HANDLE hFile = CreateFile(
		fname.c_str(), 
		GENERIC_WRITE, 
		0, 
		NULL, 
		CREATE_NEW, 
		FILE_ATTRIBUTE_NORMAL, 
		0
	);
	// file cannot be opened
	if (hFile == INVALID_HANDLE_VALUE)
		throw LicensingError(ILicensingError::LICERR_STORAGE_WRITE, GetLastError());

	DWORD dwWritten = 0;
	if (!WriteFile(hFile, obj.GetObjectImage(), obj.GetObjectSize(), &dwWritten, 0))
	{
		CloseHandle(hFile);
		throw LicensingError(ILicensingError::LICERR_STORAGE_WRITE, GetLastError());
	}

	CloseHandle(hFile);
}

//-----------------------------------------------------------------------------

void WinFileStorageRW::RemoveObject(const char_t* name)
{
	std::basic_string<char_t> fname = dir + name;

	DWORD dwAttr = GetFileAttributes(fname.c_str());
	// check for object existence
	if (dwAttr == INVALID_FILE_ATTRIBUTES)
	{
		DWORD dwError = GetLastError();

		switch (dwError)
		{
		case ERROR_FILE_NOT_FOUND:
			throw LicensingError(ILicensingError::LICERR_NOT_FOUND);
			break;
		default:
			throw LicensingError(ILicensingError::LICERR_STORAGE_WRITE, dwError);
			break;
		}
	}
	// ensure that entity is a file (not a directory)
	else if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		throw LicensingError(ILicensingError::LICERR_NOT_FOUND);


	if(!DeleteFile(fname.c_str()))
		throw LicensingError(ILicensingError::LICERR_STORAGE_WRITE, GetLastError());
}

//-----------------------------------------------------------------------------

ObjectImage::ObjectImage(const char_t* name, size_t size) 
	: image(size, 0)
	, obj_name(name)
{
}

//-----------------------------------------------------------------------------

const char_t* ObjectImage::GetObjectName() const
{
	return obj_name.c_str();
}

//-----------------------------------------------------------------------------

unsigned int ObjectImage::GetObjectSize() const
{
	return image.size();
}

//-----------------------------------------------------------------------------

const void* ObjectImage::GetObjectImage() const
{
	return &image[0];
}
//-----------------------------------------------------------------------------


}