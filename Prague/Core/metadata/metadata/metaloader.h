#if !defined(__METALOADER_H_included__)
#define __METALOADER_H_included__

#include <string.h>
#include <wchar.h>
#include <kl_byteswap.h>
#include <klsys/filemapping/wholefilemapping.h>
#include <klsys/memorymanager/guardedmm.h>
#include <klsys/guards/guard.h>

#include "md5.h"

#include "../metadata.h"

#if defined (_WIN32)
#include <windows.h>
typedef FILETIME TimeStamp; 
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
typedef time_t TimeStamp;
#endif // _WIN32

#define CHUNK 100

typedef KLSysNS::GuardedMM<KLSysNS::Guard> MetadataMemoryManager;
extern MetadataMemoryManager memorymanager;

using KLSysNS::WholeFileMapping;

const char MetadataSignaturePart4[] = "metacache";
const char MetadataSignaturePart3[] = "metadatas";
const char MetadataSignaturePart2[] = "metadata";
const char MetadataSignaturePart1[] = "Prague";

struct MetaInfo {
	MetaInfo()
		: storage(0),
	     filename(0),
	     widefilename(0), 
	     metas (0),
	     allocated(0) {memset(&timestamp, 0, sizeof(TimeStamp));}
	
	~MetaInfo();

	void* storage;
	char* filename;
	wchar_t* widefilename;
	PluginMetadata** metas;
	unsigned int allocated;
	TimeStamp timestamp;
};

void* allocate(size_t size);
void* reallocate(void* address, size_t size);
void deallocate(void* address);
bool allocateStaticProperties(InterfaceMetadata& metadata);
bool allocateExports(PluginMetadata& plugin);
bool allocateInterfaces(PluginMetadata& plugin);
bool getTimeStamp(const char* filename, TimeStamp& timestamp);
bool checkTimeStamp(const char* filename, const TimeStamp& timestamp);
const MetaInfo* getMetaInfo(const char* filename, MetaInfo** metainfo);
#if defined(_WIN32)
bool getTimeStamp(const wchar_t* filename, TimeStamp& timestamp);
bool checkTimeStamp(const wchar_t* filename, const TimeStamp& timestamp);
const MetaInfo* getMetaInfo(const wchar_t* filename, MetaInfo** metainfo);

#endif // _WIN32

struct GetValue{
	template<typename T>
	void get(const void* pointer, T& value)
	{
		memmove(&value, pointer, sizeof(T));
	}
};

struct GetLEValue{
	template<typename T>
	void get(const void* pointer, T& value)
	{
		memmove(&value, pointer, sizeof(T));
		switch(sizeof(T)){
		case 2:	value = static_cast<T>(kl_bswap_16_le(value)); break;
		case 4: value = static_cast<T>(kl_bswap_32_le(value)); break;
#if !defined(__LP64__)
		case 8: value = static_cast<T>(kl_bswap_64_le(value)); break;
#endif // __LP64__
		}
	}
};


template<typename Swap>
struct MetaLoader: public Swap{
	MetaLoader()
		: start(0),
	     end(0),
	     position(0) {}

	bool outOfRange(const unsigned char* address) const;
	bool outOfRange(const unsigned char* address, const unsigned char* s, const unsigned char* e) const;
	bool load(StaticProperty& property);
	bool load(InterfaceMetadata& metadata);
	bool loadExports(PluginMetadata& plugin);
	bool check_md5(unsigned char* checkstart);
	bool loadInterfaces(PluginMetadata& plugin);
	bool loadFields(PluginMetadata& plugin);
	bool load(unsigned char* map, unsigned int mappedsize, PluginMetadata& metadata);
	bool load(PluginMetadata& metadata, bool full);
	MetaInfo* load(unsigned char* map, unsigned int mappedsize);
	bool load(MetaInfo& metainfo, bool full);
	bool loadMulti(MetaInfo& metainfo);
	bool loadCache(unsigned char* map, unsigned int mappedsize, PragueMetadata& metadata);
	bool addMetaInfo(PragueMetadata& metadata, MetaInfo* metainfo);
	bool load(const char* filename, PragueMetadata& metadata);
#if defined(_WIN32)
	bool load(const wchar_t* filename, PragueMetadata& metadata);
#endif //_WIN32
	template<class T>
	bool getValue(T& value)
	{
		if(outOfRange(position + sizeof(T)))
			return false;
		Swap::get(position, value);
		position += sizeof(T);
		return true;
	}

	template<class T>
	bool getValue(T& value, const unsigned char* s, const unsigned char* e, unsigned char*& pos)
	{
		if(outOfRange(position + sizeof(T), s, e))
			return false;
		Swap::get(pos, value);
		pos += sizeof(T);
		return true;
	}

	unsigned char* start;
	unsigned char* end;
	unsigned char* position;
};

// MetaLoader

template<typename Swap>
bool MetaLoader<Swap>::outOfRange(const unsigned char* address) const 
{
	if((address < start) || (address > end))
		return true;
	return false;
}

template<typename Swap>
bool MetaLoader<Swap>::outOfRange(const unsigned char* address, const unsigned char* s, const unsigned char* e) const 
{
	if((address < s) || (address > e))
		return true;
	return false;
}



template<typename Swap>
bool MetaLoader<Swap>::load(StaticProperty& property)
{
	if(!getValue(property.thePropertyID)||
	   !getValue(property.thePropertySize))
		return false;
	if(outOfRange(position + property.thePropertySize))
		return false;
	property.thePropertyValue = allocate(property.thePropertySize);
	if(!property.thePropertyValue)
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
		memmove(property.thePropertyValue, position, property.thePropertySize);
		break;
	default :
		switch(property.thePropertySize){
		case 2: return getValue(*(reinterpret_cast<tSHORT*>(property.thePropertyValue)));
		case 4: return getValue(*(reinterpret_cast<tDWORD*>(property.thePropertyValue)));
		case 8: return getValue(*(reinterpret_cast<tLONGLONG*>(property.thePropertyValue)));
		}
	}
	position += property.thePropertySize;
	return true;
}

template<typename Swap>
bool MetaLoader<Swap>::load(InterfaceMetadata& metadata)
{
	unsigned int size;
	if(!getValue(size))
		return false;
	unsigned int neededsize = (size >= (sizeof(InterfaceMetadata)) - sizeof(metadata.theStaticProperties)) ? 
		sizeof(InterfaceMetadata) - sizeof(metadata.theStaticProperties) : 
		size;
	if(outOfRange(position + neededsize))
		return false;  
	unsigned char* pos = position;
	getValue(metadata.theStaticPropertiesCount, position, position + neededsize, pos);
	getValue(metadata.thePID, position, position + neededsize, pos);
	getValue(metadata.theSub, position, position + neededsize, pos);
	getValue(metadata.theVersion, position, position + neededsize, pos);
	getValue(metadata.theVID, position, position + neededsize, pos);
	getValue(metadata.theFlags, position, position + neededsize, pos);
	getValue(metadata.theCompat, position, position + neededsize, pos);
	getValue(metadata.theIID, position, position + neededsize, pos);
	getValue(metadata.theCodePage, position, position + neededsize, pos);
	//-- add getting of a new field here 
	// getValue(metadata.<new field>, position, position + neededsize, pos);	
	//--
	position += size;
	if(!metadata.theStaticPropertiesCount)
		return true;
	if(!allocateStaticProperties(metadata))
		return false;
	for(unsigned int i = 0; i < metadata.theStaticPropertiesCount; i++)
		if(!load(metadata.theStaticProperties[i]))
			return false; 
	return true;
}

template<typename Swap>
bool MetaLoader<Swap>::check_md5(unsigned char* checkstart)
{
	unsigned char checksum[16] = {0};
	if(outOfRange(position + sizeof(checksum)))
		return false;
	MD5_CTX ctx;
	MD5Init(&ctx);
	MD5Update(&ctx, checkstart, (unsigned)(position - checkstart));
	MD5Final(checksum, &ctx);
	if(memcmp(checksum, position, sizeof(checksum)))
		return false;
	position += sizeof(checksum);
	return true;
}

template<typename Swap>
bool MetaLoader<Swap>::loadExports(PluginMetadata& plugin)
{
	if(!plugin.theExportsCount)
		return true;
	unsigned int size = plugin.theExportsCount * sizeof(unsigned int);
	if(outOfRange(position + size))
		return false;
	// allocating memory blocks 
	if(!allocateExports(plugin))
		return false;
	// reading the export and the interfaces
	for(unsigned int i = 0; i < plugin.theExportsCount; ++i)
		getValue(plugin.theExports[i]);
	return true;
}

template<typename Swap>
bool MetaLoader<Swap>::loadInterfaces(PluginMetadata& plugin)
{
	if(!plugin.theInterfacesCount)
		return true;
	if(!allocateInterfaces(plugin))
		return false;
	for(unsigned int i = 0; i < plugin.theInterfacesCount; i++)
		if(!load(plugin.theInterfaces[i]))
			return false;
	return true;
}

template<typename Swap>
bool MetaLoader<Swap>::loadFields(PluginMetadata& plugin)
{
	unsigned int size;
	if(!getValue(size))
		return false;
	unsigned int neededsize = (size >=(sizeof(PluginMetadata)- sizeof(plugin.theExports)- sizeof(plugin.theInterfaces)))? 
		sizeof(PluginMetadata)-  sizeof(plugin.theExports)- sizeof(plugin.theInterfaces): 
		size;
	if(outOfRange(position + neededsize))
		return false;  
	unsigned char* pos = position;
	getValue(plugin.theInterfacesCount, position, position + neededsize, pos);
	getValue(plugin.theExportsCount, position, position + neededsize, pos);
	getValue(plugin.theAutoStart, position, position + neededsize, pos);
	getValue(plugin.thePluginID, position, position + neededsize, pos);
	getValue(plugin.theCodePage, position, position + neededsize, pos);
	//-- add getting of a new field here 
	//getValue(plugin.<new field>, position, position + neededsize, pos);
	//--
	position += size;
	return true;
}

template<typename Swap>
bool MetaLoader<Swap>::load(unsigned char* map, unsigned int mappedsize, PluginMetadata& metadata)
{
	const unsigned int offset = 0xFF;
	end = map + mappedsize; 
	position = end; 
	bool found = false;

	// searching a metadata marker
	do {
		position -= offset;
		if(position < map)
			position = map;
    
		unsigned char* pos = position;
		do {
			pos = reinterpret_cast <unsigned char*>(memchr(pos, MetadataSignaturePart1 [ 0 ], 
								       offset - (pos - position))); 
			
			if(pos &&
			   ((pos + sizeof(MetadataSignaturePart1) + sizeof(MetadataSignaturePart2)) < end) &&
			   !memcmp(pos, MetadataSignaturePart1, sizeof(MetadataSignaturePart1)) &&
			   !memcmp(pos + sizeof(MetadataSignaturePart1), MetadataSignaturePart2, sizeof(MetadataSignaturePart2))){
				position = pos;
				found = true;
				break;
			}

			if(pos)
				++pos;
		}
		while(pos);
	} 
	while((position > map) && !found);
  
	if(!found)
		return false;

	return load(metadata, true);
}

template<typename Swap>
bool MetaLoader<Swap>::load(PluginMetadata& metadata, bool full)
{ 
	start = position;

	if(full)
		position += sizeof(MetadataSignaturePart1)+ sizeof(MetadataSignaturePart2);		
	if(!loadFields(metadata) || !loadExports(metadata) || !loadInterfaces(metadata)){
		metadata.clear();
		return false;
	}
	if(full)
		return check_md5(start);
	return true;
}

template<typename Swap>
MetaInfo* MetaLoader<Swap>::load(unsigned char* map, unsigned int mappedsize)
{
	end = map + mappedsize; 
	const unsigned int offset = 0xFF;
	position = end; 
	bool found = false;
	bool multi = false;

	// searching a metadata marker
	do {
		position -= offset;
		if(position < map)
			position = map;
    
		unsigned char* pos = position;
		do {
			pos = reinterpret_cast <unsigned char*>(memchr(pos, MetadataSignaturePart1[0], 
								       offset - (pos - position))); 
			if(pos &&
			   ((pos + sizeof(MetadataSignaturePart1) + sizeof(MetadataSignaturePart3)) < end) &&
			   !memcmp(pos, 
				   MetadataSignaturePart1, 
				   sizeof(MetadataSignaturePart1))){
				if(!memcmp(pos + sizeof(MetadataSignaturePart1), 
					   MetadataSignaturePart3, 
					   sizeof(MetadataSignaturePart3))){
					position = pos;
					found = true;
					multi = true;
					break;
				}
				if(!memcmp(pos + sizeof(MetadataSignaturePart1), 
					   MetadataSignaturePart2, 
					   sizeof(MetadataSignaturePart2))){
					position = pos;
					found = true;
					break;
				}
			}
			if(pos)
				++pos;
		}
		while(pos);
	} 
	while((position > map) && !found);
  
	if(!found)
		return 0;

	void* tmp = allocate(sizeof(MetaInfo));
	if(!tmp)
		return 0;
	MetaInfo* metainfo = new(tmp) MetaInfo;
	
	bool loaded = (multi) ? loadMulti(*metainfo) : load(*metainfo, true);
	if(!loaded){
		metainfo->MetaInfo::~MetaInfo();
		deallocate(metainfo);
		return 0;
	}
	return metainfo;
}

template<typename Swap>
bool MetaLoader<Swap>::load(MetaInfo& metainfo, bool full)
{
	void* tmp = allocate(sizeof(PluginMetadata));
	if(!tmp)
		return false;
	PluginMetadata* meta = new(tmp) PluginMetadata;
	if(!load(*meta, full)){
		meta->PluginMetadata::~PluginMetadata();
		deallocate(meta);
		return false;
	}
	unsigned int i = 0;
	if(metainfo.metas)
		while(metainfo.metas[i] && (i < metainfo.allocated)) i++;
	if(i == metainfo.allocated){
		PluginMetadata** tmp = 
			reinterpret_cast<PluginMetadata**>(reallocate(metainfo.metas,
								      (metainfo.allocated + 2) *
								      sizeof(PluginMetadata*)));
		
		if(!tmp){
			meta->PluginMetadata::~PluginMetadata();
			deallocate(meta);
			return false;
		}
		i = metainfo.allocated;
		metainfo.allocated += 1;
		metainfo.metas = tmp;
	}
	metainfo.metas[i] = meta;
	return true;
}

template<typename Swap>
bool MetaLoader<Swap>::loadMulti(MetaInfo& metainfo)
{
	unsigned char* endpos = position;
	position += sizeof(MetadataSignaturePart1) + sizeof(MetadataSignaturePart3);
	
 	unsigned int size;
	if(!getValue(size))
 		return false;
 	position = endpos - size;
	endpos -= 16; //exclude checksum
	unsigned char* startpos = position;
	while(position != endpos){
		if(!load(metainfo, false))
			return false;
	}
	return check_md5(startpos);
}

template<typename Swap>
bool MetaLoader<Swap>::loadCache(unsigned char* map, unsigned int mappedsize, PragueMetadata& metadata)
{
	end = map + mappedsize; 
	// if metacache
	if(((map + sizeof(MetadataSignaturePart1) + sizeof(MetadataSignaturePart4)) >= end) ||
	   memcmp(map, MetadataSignaturePart1, sizeof(MetadataSignaturePart1)) ||
	   memcmp(map + sizeof(MetadataSignaturePart1), MetadataSignaturePart4, sizeof(MetadataSignaturePart4)))
		return false;
	
	metadata.clear();
	start = position = map + sizeof(MetadataSignaturePart1) + sizeof(MetadataSignaturePart4);
	unsigned char* endpos = map + mappedsize;
	while(position < endpos){
		end = endpos;
		void* tmp = allocate(sizeof(MetaInfo));
		if(!tmp)
			return false;
		MetaInfo* metainfo = new(tmp) MetaInfo;
		int size;
		if(!getValue(size))
			return false;
		bool unicode = false;
		if(size < 0){
#if defined(_WIN32)
			unicode = true;
			size = -size;
#else
			metainfo->MetaInfo::~MetaInfo();
			deallocate(metainfo);
			return false;
#endif		
		}
		if(outOfRange(position + size))
			return false;
		metainfo->storage = allocate((size + 1) * ((unicode) ? sizeof(wchar_t) : sizeof(char)));
		if(!metainfo->storage){
			metainfo->MetaInfo::~MetaInfo();
			deallocate(metainfo);
			return false;
		}
		memmove(metainfo->storage, position, size);
		if(unicode)
			metainfo->widefilename = reinterpret_cast<wchar_t*>(metainfo->storage);
		else
			metainfo->filename = reinterpret_cast<char*>(metainfo->storage);
		position += size;

		unsigned int metasize;
		if(!getValue(metainfo->timestamp) || 
		   !getValue(metasize)){
			metainfo->MetaInfo::~MetaInfo();			
			deallocate(metainfo);
			return false;
		}
		bool actual = false;
#if defined(_WIN32)
		if(unicode)
			actual = checkTimeStamp(metainfo->widefilename, metainfo->timestamp);
		else
#endif
			actual = checkTimeStamp(metainfo->filename, metainfo->timestamp);
		start = position;
		end = position + metasize;
		if(actual){
			while(position != end)
				if(!load(*metainfo, false))
					break;
			if(position == end){
				addMetaInfo(metadata, metainfo);
				continue;
			}
		}
		position = end;

#if defined(_WIN32)
		if(unicode)
			::load(metainfo->widefilename, metadata);
		else
#endif		
			::load(metainfo->filename, metadata);
		metainfo->MetaInfo::~MetaInfo();			
		deallocate(metainfo);

	}
	return true;
}

template<typename Swap>
bool MetaLoader<Swap>::addMetaInfo(PragueMetadata& metadata, MetaInfo* metainfo)
{
	unsigned int i = 0;
	if(metadata.metainfo)
		while(metadata.metainfo[i] && (i < metadata.allocated)) i++;
	if(i == metadata.allocated){
		MetaInfo** tmp = 
			reinterpret_cast<MetaInfo**>(reallocate(metadata.metainfo,
								(metadata.allocated + CHUNK + 1) * sizeof(MetaInfo*)));
		if(!tmp)
			return false;
		i = metadata.allocated;
		metadata.allocated += CHUNK;
		metadata.metainfo = tmp;
	}
	metadata.metainfo[i] = metainfo;
	return true;
}

template<typename Swap>
bool MetaLoader<Swap>::load(const char* filename, PragueMetadata& metadata)
{
	if(!filename)
		return 0;
	// fail, if metadata from this file already exists and was not modified
	const MetaInfo* mi = getMetaInfo(filename, metadata.metainfo);
	if(mi && checkTimeStamp(filename, mi->timestamp))
		return 0;
	WholeFileMapping::kl_size_t mappedsize = 0;
	WholeFileMapping filemapping;
	unsigned char* map = reinterpret_cast<unsigned char*>(filemapping.map(filename, KL_FILEMAP_READ, &mappedsize));
	if(!map || (map == reinterpret_cast<unsigned char*>(-1)))
		return false;
	if(loadCache(map, (unsigned)mappedsize, metadata))
		return true;
	MetaInfo* metainfo = load(map, (unsigned)mappedsize);
	if(!metainfo)
		return false;
	unsigned int size = strlen(filename);
	metainfo->storage = allocate(size + sizeof(char));
	if(!metainfo->storage || !addMetaInfo(metadata, metainfo)){
		metainfo->MetaInfo::~MetaInfo();
		deallocate(metainfo);
		return false;
	}
	memmove(metainfo->storage, filename, size);
	metainfo->filename = reinterpret_cast<char*>(metainfo->storage);
	getTimeStamp(metainfo->filename, metainfo->timestamp);
	return true;
}

#if defined(_WIN32)
template<typename Swap>
bool MetaLoader<Swap>::load(const wchar_t* filename, PragueMetadata& metadata)
{
	if(!filename)
		return 0;
	// fail, if metadata from this file already exists and was not modified
	const MetaInfo* mi = getMetaInfo(filename, metadata.metainfo);
	if(mi && checkTimeStamp(filename, mi->timestamp))
		return 0;
	WholeFileMapping::kl_size_t mappedsize = 0;
	WholeFileMapping filemapping;
	unsigned char* map = reinterpret_cast<unsigned char*>(filemapping.map(filename, KL_FILEMAP_READ, &mappedsize));
	if(!map || (map == reinterpret_cast<unsigned char*>(-1)))
		return false;
	if(loadCache(map, (unsigned)mappedsize, metadata))
		return true;
	MetaInfo* metainfo = load(map, (unsigned)mappedsize);
	if(!metainfo)
		return false;
	unsigned int size = wcslen(filename);
	metainfo->storage = allocate(size + sizeof(char));
	if(!metainfo->storage || !addMetaInfo(metadata, metainfo)){
		metainfo->MetaInfo::~MetaInfo();
		deallocate(metainfo);
		return false;
	}
	memmove(metainfo->storage, filename, size);
	metainfo->widefilename = reinterpret_cast<wchar_t*>(metainfo->storage);
	getTimeStamp(metainfo->widefilename, metainfo->timestamp);
	return true;
}
#endif //_WIN32
#endif // __METALOADER_H_included__
