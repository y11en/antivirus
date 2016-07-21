#include <string.h>

#include "metaloader.h"

MetadataMemoryManager memorymanager;

// StaticProperty 

StaticProperty::~StaticProperty()
{
	if(thePropertyValue){
		deallocate(thePropertyValue);
		thePropertyValue = 0;
		thePropertySize = 0;
	}
}

StaticProperty& StaticProperty::operator =(const StaticProperty& property)
{
	this->StaticProperty::~StaticProperty();
	thePropertyValue = allocate(property.thePropertySize);
	if(thePropertyValue){
		memmove(thePropertyValue, property.thePropertyValue, property.thePropertySize);
		thePropertyID = property.thePropertyID;
		thePropertySize = property.thePropertySize;
	}
	return *this;
}

// InterfaceMetadata

InterfaceMetadata::~InterfaceMetadata()
{
	if(theStaticProperties){
		for(unsigned int i = 0; i < theStaticPropertiesCount; i++)
			theStaticProperties [i].StaticProperty::~StaticProperty();
		deallocate(theStaticProperties);
		theStaticProperties = 0;
	}
	theStaticPropertiesCount = 0;
}

InterfaceMetadata& InterfaceMetadata::operator =(const InterfaceMetadata& metadata)
{
	this->InterfaceMetadata::~InterfaceMetadata();
	memmove(this, &metadata, sizeof(InterfaceMetadata) - sizeof(metadata.theStaticProperties));
	if(!theStaticPropertiesCount)
		return *this;
	if(!allocateStaticProperties(*this)){
		theStaticPropertiesCount = 0;
		return *this;
	}
	for(unsigned int i = 0; i < theStaticPropertiesCount; i++)
		theStaticProperties[i] = metadata.theStaticProperties[i];
	return *this;
}

// PluginMetadata

PluginMetadata::~PluginMetadata()
{
	clear();
}

void PluginMetadata::clear()
{
	if(theInterfaces){
		for(unsigned int i = 0; i < theInterfacesCount; i++)
			theInterfaces [ i ].InterfaceMetadata::~InterfaceMetadata();
		theInterfacesCount = 0;
		deallocate(theInterfaces);
		theInterfaces = 0;
	}
	if(theExports){
		deallocate(theExports);
		theExports = 0;
	}	
}

PluginMetadata& PluginMetadata::operator =(const PluginMetadata& metadata)
{
	clear();
	memmove(this, &metadata, sizeof(PluginMetadata)- sizeof(metadata.theExports)- sizeof(metadata.theInterfaces));
	if(theInterfacesCount)
		if(allocateInterfaces(*this))
			for(unsigned int i = 0; i < theInterfacesCount; i++)
				theInterfaces [ i ] = metadata.theInterfaces [ i ];
		else
			theInterfacesCount = 0;
	
	if(theExports)
		if(allocateExports(*this))
			for(unsigned int i = 0; i < theExportsCount; i++)
				theExports [ i ] = metadata.theExports [ i ];
		else
			theExportsCount = 0;
	return *this;
}

// Prague metadata

PragueMetadata::~PragueMetadata()
{
	clear();
}

void PragueMetadata::clear()
{
 	if(metainfo){
 		for(unsigned int i = 0; metainfo[i] != 0; ++i){
 			metainfo[i]->MetaInfo::~MetaInfo();
			deallocate(metainfo[i]);
		}
 		deallocate(metainfo);
 		metainfo = 0;
 	}
	allocated = 0;
}

unsigned int PragueMetadata::count() const
{
	if(!metainfo)
		return 0;
	unsigned int c = 0;
	for(unsigned int i = 0; metainfo[i] != 0; ++i)
		c += count(metainfo[i]->metas);
	return c;
}

unsigned int PragueMetadata::count(const PluginMetadata* const* metadata) const
{
	if(!metadata)
		return 0;
	unsigned int c = 0;
	for(unsigned int i = 0; metadata[i] != 0; ++i)
		++c;
	return c;
}

const PluginMetadata* PragueMetadata::getMetadata(unsigned int index) const
{
	if(!metainfo)
		return 0;
	unsigned int count = 0;
	for(unsigned int i = 0; metainfo[i] != 0; ++i)
		for(unsigned int j = 0; metainfo[i]->metas[j] != 0; ++j)
			if(count++ == index)
				return metainfo[i]->metas[j];
	return 0;

}

const PluginMetadata* const* PragueMetadata::getMetadata(const char* filename) const
{
	const MetaInfo* mi = getMetaInfo(filename, metainfo);
	if(!mi)
		return 0;
	return mi->metas;
}

const char* PragueMetadata::getFileName(unsigned int index) const
{
	if(!metainfo)
		return 0;
	unsigned int count = 0;
	for(unsigned int i = 0; metainfo[i] != 0; ++i)
		for(unsigned int j = 0; metainfo[i]->metas[j] != 0; ++j)
			if(count++ == index)
				return metainfo[i]->filename;
	return 0;
}

#if defined(_WIN32)
const PluginMetadata* const* PragueMetadata::getMetadata(const wchar_t* filename) const
{
	const MetaInfo* mi = getMetaInfo(filename, metainfo);
	if(!mi)
		return 0;
	return mi->metas;
}

const wchar_t* PragueMetadata::getWideFileName(unsigned int index) const
{
	if(!metainfo)
		return 0;
	unsigned int count = 0;
	for(unsigned int i = 0; metainfo[i] != 0; ++i)
		for(unsigned int j = 0; metainfo[i]->metas[j] != 0; ++j)
			if(count++ == index)
				return metainfo[i]->widefilename;
	return 0;
}
#endif // _WIN32	

// Common function

void* allocate(size_t size)
{
	return memorymanager.allocate(size);
}

void* reallocate(void* address, size_t size)
{
	return memorymanager.reallocate(address, size);
}

void deallocate(void* address)
{
	memorymanager.deallocate(address);
}


bool allocateStaticProperties(InterfaceMetadata& metadata)
{
	unsigned char* place = reinterpret_cast <unsigned char*>(allocate(sizeof(StaticProperty) * 
												    metadata.theStaticPropertiesCount));
	if(!place)
		return false;
	metadata.theStaticProperties = reinterpret_cast <StaticProperty*>(place);
	for(unsigned int i = 0; i < metadata.theStaticPropertiesCount; i++)
		new(place + i * sizeof(StaticProperty)) StaticProperty;
	return true;
}

bool allocateExports(PluginMetadata& plugin)
{
	tDWORD size = plugin.theExportsCount * sizeof(tDWORD);
	plugin.theExports = reinterpret_cast<tDWORD*>(allocate(size));
	if(!plugin.theExports)
		return false;
	return true;
}

bool allocateInterfaces(PluginMetadata& plugin)
{
	unsigned char* place = reinterpret_cast <unsigned char*>(allocate(sizeof(InterfaceMetadata) * 
									  plugin.theInterfacesCount));
	if(!place)
		return false;
	plugin.theInterfaces = reinterpret_cast <InterfaceMetadata*>(place);
	for(unsigned int i = 0; i < plugin.theInterfacesCount; i++)
		new(place + i * sizeof(InterfaceMetadata))InterfaceMetadata;
	return true;
}
