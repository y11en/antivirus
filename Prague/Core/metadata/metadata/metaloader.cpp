#include "metaloader.h"

// MetaInfo

MetaInfo::~MetaInfo()
{
	if(storage && (filename || widefilename)){
		deallocate(storage);
	}
	if(metas){
		for(unsigned int i = 0; metas[i] != 0; ++i){
			metas[i]->PluginMetadata::~PluginMetadata();
			deallocate(metas[i]);
		}
		deallocate(metas);
	}
	storage = 0;
	filename = 0;
	widefilename = 0;
	metas = 0;
	allocated = 0;
}

// loading metadata

int load(const char* filename, PluginMetadata& metadata)
{
	if(!filename)
		return 0;

	WholeFileMapping::kl_size_t mappedsize = 0;
	WholeFileMapping filemapping;
	unsigned char* map = reinterpret_cast<unsigned char*>(filemapping.map(filename, KL_FILEMAP_READ, &mappedsize));
	if(!map || (map == reinterpret_cast<unsigned char*>(-1)))
		return 0;
	MetaLoader<GetValue> loader;
	return loader.load(map, (unsigned)mappedsize, metadata) ? 1 : 0;
}

int load(const char* filename, PragueMetadata& metadata)
{
	MetaLoader<GetValue> loader;
	return loader.load(filename, metadata) ? 1 : 0;
}

const MetaInfo* getMetaInfo(const char* filename, MetaInfo** metainfo)
{		
	if(!metainfo)
		return 0;
	for(unsigned int i = 0; metainfo[i] != 0; ++i)
		if(metainfo[i]->filename && !strcmp(metainfo[i]->filename, filename))
			return metainfo[i];
	return 0;
}

#if defined (_WIN32)
int load(const wchar_t* filename, PluginMetadata& metadata)
{
	if(!filename)
		return 0;

	WholeFileMapping::kl_size_t mappedsize = 0;
	WholeFileMapping filemapping;
	unsigned char* map = reinterpret_cast<unsigned char*>(filemapping.map(filename, KL_FILEMAP_READ, &mappedsize));
	if(!map || (map == reinterpret_cast<unsigned char*>(-1)))
		return 0;
	MetaLoader<GetValue> loader;
	return loader.load(map, (unsigned)mappedsize, metadata) ? 1 : 0;
}

int load(const wchar_t* filename, PragueMetadata& metadata)
{
	MetaLoader<GetValue> loader;
	return loader.load(filename, metadata) ? 1 : 0;
}

const MetaInfo* getMetaInfo(const wchar_t* filename, MetaInfo** metainfo)
{		
	if(!metainfo)
		return 0;
	for(unsigned int i = 0; metainfo[i] != 0; ++i)
		if(metainfo[i]->widefilename && !wcscmp(metainfo[i]->widefilename, filename))
			return metainfo[i];
	return 0;
}
#endif // _WIN32
