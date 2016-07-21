#if !defined(__METASAVER_H__included__)
#define __METASAVER_H__included__

#include <string.h>
#include <wchar.h>

#include "metaloader.h"

#define KL_MAX_WORDSIZE 64
#include <kl_byteswap.h>

#if defined(_WIN32)
#include <windows.h>
struct Win32File{
	Win32File() : handle(INVALID_HANDLE_VALUE), total(0) {}
	~Win32File() 
	{
		CloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
	}
	
	bool open(const char* filename)
	{
		handle = CreateFileA(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		return handle != INVALID_HANDLE_VALUE;
	}
	     
	bool open(const wchar_t* filename)
	{
		handle = CreateFileW(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		return handle != INVALID_HANDLE_VALUE;
	}
	
	bool write(const void* buffer, unsigned int size, MD5_CTX* ctx = 0)
	{
		DWORD count = 0;	
		const unsigned char* b = reinterpret_cast<const unsigned char*>(buffer);
		while(count != size){
			DWORD written = 0;
			if(!WriteFile(handle, b + count, size - count, &written, 0))
				return false;
			count += written;
		}
		if(ctx)
			MD5Update(ctx, reinterpret_cast<const unsigned char*>(buffer), size);
		total += size;
		return true;
	}
	

	HANDLE handle;
	unsigned int total;
};

typedef Win32File FileImp;
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct UnixFile{
	UnixFile () : fd(-1), total(0) {}
	~UnixFile() 
	{
		::close(fd);
		fd = -1;
	}

	bool open(const char* filename)
	{
		fd = ::open(filename, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
		return fd != -1;
	}
	
	bool write(const void* buffer, unsigned int size, MD5_CTX* ctx = 0)
	{
		if(fd == -1)
			return false;
		unsigned int count = 0;
		const unsigned char* b = reinterpret_cast<const unsigned char*>(buffer);
		while(count != size){
			int written = 0;
			if((written = ::write(fd, b + count, size - count)) < 0)
				return false;
			count += written;
		}
		if(ctx)
			MD5Update(ctx, b, size);
		total += size;
		return true;
		
	}

	int fd;
	unsigned int total;
};
typedef UnixFile FileImp;
#endif

struct SaveValue: public FileImp {
	template<typename T>
	bool save(const T& value, MD5_CTX* ctx = 0)
	{
		return FileImp::write(&value, sizeof(value), ctx);
	}
};

struct SaveBEValue: public FileImp {
	template<typename T>
	bool save(const T& value, MD5_CTX* ctx = 0)
	{
		T v;
		switch(sizeof(T)){
		case 2:	v = static_cast<T>(kl_bswap_16_be(value)); break;
		case 4: v = static_cast<T>(kl_bswap_32_be(value)); break;
#if !defined(__LP64__)
		case 8: v = static_cast<T>(kl_bswap_64_be(value)); break;
#endif // __LP64__
		default: return false;
		}
		return write(&v, sizeof(v), ctx);
	}
};


template<typename Swap>
struct MetaSaver : public Swap{
	using Swap::write;
	bool saveCache(const PragueMetadata& metadata)
	{
		if(!metadata.metainfo)
			return false;
		if(!write(MetadataSignaturePart1, sizeof(MetadataSignaturePart1)) ||
		   !write(MetadataSignaturePart4, sizeof(MetadataSignaturePart4)) ||
		   !save(metadata))
			return false;
		return true;
	}

	bool saveMulti(const PragueMetadata& metadata)
	{
		if(!metadata.metainfo)
			return false;
		unsigned char checksum[16] = {0};
		MD5_CTX ctx;
		MD5Init(&ctx);
		for(unsigned int i = 0; metadata.metainfo[i] != 0; ++i)
			for(unsigned int j = 0; metadata.metainfo[i]->metas[j] != 0; ++j)
				if(!save(*metadata.metainfo[i]->metas[j], &ctx))
					return false;
		MD5Final(checksum, &ctx);
		if(!write(checksum, sizeof(checksum)))
			return false;
		unsigned int count = Swap::total;
		if(!write(MetadataSignaturePart1, sizeof(MetadataSignaturePart1)) ||
		   !write(MetadataSignaturePart3, sizeof(MetadataSignaturePart3)) ||
		   !Swap::save(count))
			return false;
		return true;
	}

	bool saveOne(const PragueMetadata& metadata)
	{
		if(!metadata.metainfo)
			return false;
		unsigned char checksum[16] = {0};
		MD5_CTX ctx;
		MD5Init(&ctx);
		if(!write(MetadataSignaturePart1, sizeof(MetadataSignaturePart1), &ctx) ||
		   !write(MetadataSignaturePart2, sizeof(MetadataSignaturePart2), &ctx) ||
		   !save(*metadata.getMetadata(static_cast<unsigned int>(0)), &ctx))
			return false;
		MD5Final(checksum, &ctx);
		if(!write(checksum, sizeof(checksum)))
			return false;
		return true;
	}

	bool save(const PragueMetadata& metadata)
	{
		if(!metadata.metainfo)
			return false;
		for(unsigned int i= 0; metadata.metainfo[i] !=0; ++i)
			if(!save(*metadata.metainfo[i]))
				return false;
		return true;
			  
	}

	bool save(const MetaInfo& metainfo)
	{
		int s = (metainfo.filename) ? strlen(metainfo.filename) : wcslen(metainfo.widefilename) * sizeof(wchar_t);
		if(metainfo.widefilename)
			s = -s;
		if(!Swap::save(s) ||
		   !write(metainfo.storage, s))
			return false;
		TimeStamp timestamp;
#if defined(_WIN32)
		if(metainfo.widefilename){
			if(!getTimeStamp(metainfo.widefilename, timestamp))
				return false;
		}
		else
#endif //_WIN32
			if(!getTimeStamp(metainfo.filename, timestamp))
				return false;
		if(!write(&timestamp, sizeof(timestamp)))
			return false;
		s = 0;
		for(unsigned int i = 0; metainfo.metas[i] != 0; ++i)
			s += size(*metainfo.metas[i]);
		if(!Swap::save(s))
			return false;
		for(unsigned int i = 0; metainfo.metas[i] != 0; ++i)
			if(!save(*metainfo.metas[i]))
				return false;
		return true;
	}

	bool save(const PluginMetadata& metadata, MD5_CTX* ctx = 0)
	{
		unsigned int s = sizeof(PluginMetadata) - sizeof(metadata.theExports) - sizeof(metadata.theInterfaces);
		if(!Swap::save(s, ctx) ||
		   !Swap::save(metadata.theInterfacesCount, ctx) ||
		   !Swap::save(metadata.theExportsCount, ctx) ||
		   !Swap::save(metadata.theAutoStart, ctx) ||
		   !Swap::save(metadata.thePluginID, ctx) ||
		   !Swap::save(metadata.theCodePage, ctx)
		   //-- add saving of a new field here 
		   //!Swap::save(metadata.<new field>, ctx)
		   //--
		   )
			return false;
		for(unsigned int i = 0; i < metadata.theExportsCount; ++i)
			if(!Swap::save(metadata.theExports[i], ctx))
				return false;
		for(unsigned int i = 0; i < metadata.theInterfacesCount; ++i)
			if(!save(metadata.theInterfaces[i], ctx))
				return false;
		return true;
	}

	bool save(const InterfaceMetadata& metadata, MD5_CTX* ctx = 0)
	{
		unsigned int s = sizeof(InterfaceMetadata) - sizeof(metadata.theStaticProperties);
		if(!Swap::save(s, ctx) ||
		   !Swap::save(metadata.theStaticPropertiesCount, ctx) ||
		   !Swap::save(metadata.thePID, ctx) ||
		   !Swap::save(metadata.theSub, ctx) ||
		   !Swap::save(metadata.theVersion, ctx) ||
		   !Swap::save(metadata.theVID, ctx) ||
		   !Swap::save(metadata.theFlags, ctx) ||
		   !Swap::save(metadata.theCompat, ctx) ||
		   !Swap::save(metadata.theIID, ctx) ||
		   !Swap::save(metadata.theCodePage, ctx)
		   //-- add saving of a	new field here 
		   //!Swap::save(metadata.<new field>, ctx)
		   //--
		   )
			return false;
		for(unsigned int i = 0; i < metadata.theStaticPropertiesCount; ++i)
			if(!save(metadata.theStaticProperties[i], ctx))
				return false;
		return true;
	}

	bool save(const StaticProperty& property, MD5_CTX* ctx = 0)
	{
		if(!Swap::save(property.thePropertyID, ctx) ||
		   !Swap::save(property.thePropertySize, ctx))
			return false;
		unsigned int type = (property.thePropertyID & pTYPE_MASK) >> 20;
		switch(type){
		case tid_WCHAR :
		case tid_WSTRING :
			//#warning "Consideration is needed!"    
			break;

		case tid_CHAR:
		case tid_BYTE:
		case tid_SBYTE:
		case tid_STRING:    
		case tid_BINARY:
			return write(property.thePropertyValue, property.thePropertySize, ctx);
		default :
			switch(property.thePropertySize){
			case 2: return Swap::save(*(reinterpret_cast<tSHORT*>(property.thePropertyValue)), ctx);
			case 4: return Swap::save(*(reinterpret_cast<tDWORD*>(property.thePropertyValue)), ctx);
			case 8: return Swap::save(*(reinterpret_cast<tLONGLONG*>(property.thePropertyValue)), ctx);
			}
		}
		return true;
	}

	unsigned int size(const PluginMetadata& metadata)
	{
		unsigned int s =  
			sizeof(tDWORD) + sizeof(PluginMetadata) - 
			sizeof(metadata.theExports) - sizeof(metadata.theInterfaces) + 
			sizeof(tDWORD) * metadata.theExportsCount;
		for(unsigned int i = 0; i < metadata.theInterfacesCount; i++) 
			s += size(metadata.theInterfaces[i]);
		return s;

	}

	unsigned int size(const InterfaceMetadata& metadata)
	{
		unsigned int s = sizeof(tDWORD) + sizeof(InterfaceMetadata) - sizeof(metadata.theStaticProperties);
		for(unsigned int i = 0; i < metadata.theStaticPropertiesCount; i++)
			s += sizeof(StaticProperty) - sizeof (metadata.theStaticProperties[i].thePropertyValue) + 
				metadata.theStaticProperties[i].thePropertySize;
		return s;
	}
};


#endif // __METASAVER_H__included__
