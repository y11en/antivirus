#ifndef _METADATA_H_
#define _METADATA_H_

#include <Prague/prague.h>

#pragma pack(4)

struct StaticProperty {
	StaticProperty()
		: thePropertyID(0),
	     thePropertySize(0),
	     thePropertyValue(0){}
	StaticProperty(const StaticProperty& property)
		: thePropertyID(0),
	     thePropertySize(0),
	     thePropertyValue(0) {operator = (property);}
	~StaticProperty();
	StaticProperty& operator =(const StaticProperty& property);

	tDWORD thePropertyID;
	tDWORD thePropertySize;
	tPTR thePropertyValue;
};

struct InterfaceMetadata
{
	InterfaceMetadata()
		: theStaticPropertiesCount(0),
	     thePID(PID_NONE),
	     theSub(0),
	     theVersion(0),
	     theVID(VID_ANY),
	     theFlags(0),
	     theCompat(IID_NONE),
	     theIID(IID_NONE),
	     theCodePage(cCP_ANSI),
	     theStaticProperties(0){}

	InterfaceMetadata(const InterfaceMetadata& metadata)
		: theStaticPropertiesCount(0),
	     thePID(PID_NONE),
	     theSub(0),
	     theVersion(0),
	     theVID(VID_ANY),
	     theFlags(0),
	     theCompat(IID_NONE),
	     theIID(IID_NONE),
	     theCodePage(cCP_ANSI),
	     theStaticProperties(0){operator = (metadata);}

	~InterfaceMetadata();

	InterfaceMetadata& operator =(const InterfaceMetadata& metadata);

	tDWORD          theStaticPropertiesCount;
	tPID            thePID;
	tDWORD          theSub;
	tVERSION        theVersion;
	tVID            theVID;
	tDWORD          theFlags;
	tIID            theCompat;
	tIID            theIID;
	tCODEPAGE       theCodePage;
	// -- place new fields here
	// -- don't forget to add loading and saving of an every new field to functions
	// -- from metaloader.h and metasaver.h (signature of the functions see below)
	// -- bool MetaLoader::load(InterfaceMetadata& metadata)
	// -- bool MetaSaver::save(const InterfaceMetadata&)

	// -- end of place for new fields
	StaticProperty* theStaticProperties;
};

struct PluginMetadata
{
	PluginMetadata()
		: theInterfacesCount(0),
	     theExportsCount(0),
	     theAutoStart(cFALSE),
	     thePluginID(PID_NONE),
	     theCodePage(cCP_ANSI),
	     theExports(0),
	     theInterfaces(0){}

	PluginMetadata(const PluginMetadata& metadata)
		: theInterfacesCount(0),
	     theExportsCount(0),
	     theAutoStart(cFALSE),
	     thePluginID(PID_NONE),
	     theCodePage(cCP_ANSI),
	     theExports(0),
	     theInterfaces(0){operator = (metadata);}

	~PluginMetadata();
	
	PluginMetadata& operator =(const PluginMetadata& metadata);
	void clear();

	tDWORD          theInterfacesCount;
	tDWORD          theExportsCount;
	tBOOL           theAutoStart;
	tDWORD          thePluginID;
	tCODEPAGE       theCodePage;
	// -- place new fields here
	// -- don't forget to add loading and saving of an every new field to functions
	// -- from metaloader.h and metasaver.h (signature of the functions see below)
	// -- bool MetaLoader::loadFields(PluginMetadata& plugin)
	// -- bool MetaSaver::save(const PluginMetadata&)

	// -- end of place for new fields
	tDWORD* theExports;
	InterfaceMetadata* theInterfaces;
};

struct MetaInfo;

class PragueMetadata
{
 public:
	PragueMetadata()
		: metainfo(0),
		allocated(0) {}
	
	~PragueMetadata();	
	
	void clear();
	
	unsigned int count() const;
	unsigned int count(const PluginMetadata* const* metadata) const;
	const PluginMetadata* getMetadata(unsigned int index) const;
	const PluginMetadata* const* getMetadata(const char* filename) const;
	const char* getFileName(unsigned int index) const;
#if defined(_WIN32)
	const PluginMetadata* const* getMetadata(const wchar_t* filename) const;
	const wchar_t* getWideFileName(unsigned int index) const;
#endif // _WIN32	
	MetaInfo** metainfo;
	unsigned int allocated;
 private:
	PragueMetadata(const PragueMetadata&){}
	PragueMetadata& operator =(const PragueMetadata&) {return *this;}
};

#pragma pack()

int load(const char* filename, PluginMetadata& metadata); 
int load(const char* filename, PragueMetadata& metadata); 
int save(const char* filename, PragueMetadata& metadata); 
#if defined(_WIN32)
int load(const wchar_t* filename, PluginMetadata& metadata);
int load(const wchar_t* filename, PragueMetadata& metadata);
int save(const wchar_t* filename, PragueMetadata& metadata);
#endif // _WIN32


#endif //_METADATA_H_
