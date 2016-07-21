#include <map>
#include <vector>

#include <boost/cstdint.hpp>

#include "Collector.hpp"

namespace idl {

class Metadata : public Collector {
  public:
  virtual bool processImports() const { return true; }
	virtual void constDecl(const Attributes& attrs, ConstValue::Ptr context);
	virtual void typedefDecl(const Attributes& attrs, Typedef::Ptr context);
	virtual void structDecl(const Attributes& attrs, Struct::Ptr context);
	virtual void enumDecl(const Attributes& attrs, Enum::Ptr context);
	virtual void interfaceOpen(const Attributes& attrs, Interface::Ptr context);
	virtual void interfaceClose(const Attributes& attrs, Interface::Ptr context);
	virtual void methodDecl(const Attributes& attrs, Method::Ptr context);
	virtual void externMethodDecl(const Attributes& attrs, ExternMethod::Ptr context);
	virtual void componentOpen(const Attributes& attrs, Component::Ptr context);
	virtual void componentClose(const Attributes& attrs, Component::Ptr context);
	virtual void implementationOpen(const Attributes& attrs, Implementation::Ptr context);
	virtual void implementationClose(const Attributes& attrs, Implementation::Ptr context);
	virtual void sysMethodDecl(const Attributes& attrs, NameContext::Ptr context);
	virtual void propertyDecl(const Attributes& attrs, Property::Ptr context);
	virtual void propertyPluginDecl(const Attributes& attrs, Property::Ptr context);
	virtual void propertyPluginAddDecl(const Attributes& attrs, Property::Ptr context);
	virtual void messageDecl(const Attributes& attrs, Message::Ptr context);
	virtual void messageClassDecl(const Attributes& attrs, MessageClass::Ptr context);
	virtual void messageClassExtendDecl(const Attributes& attrs, MessageClassExtend::Ptr context);
	virtual void errorDecl(const Attributes& attrs, NameContext::Ptr context);
	virtual void forwardIFace( NameContext::Ptr context );
	virtual void forwardStruct( NameContext::Ptr context );
	virtual void typecast( NameContext::Ptr context );

	virtual void externalTypeDecl(ExternalType::Ptr context);
				
	virtual void importFile(const std::string& file);

  	void dump(std::vector<char>& data) const;
    
  private:
  	void saveAttrs(const Attributes& attrs, NameContext::Ptr context);
  
  	typedef boost::int8_t	SInt8;
  	typedef boost::uint32_t	UInt32;
  
#pragma pack(1)
	struct Header {
		SInt8 _prague[7];
		SInt8 _metadata[9];
		
		Header();
	};

#pragma pack(1)
  	struct PluginDesc {
  		UInt32	_size;
  		UInt32	_implementationNumber;
  		UInt32	_exportFuncsNumber;
  		UInt32	_autoload;
  		UInt32	_pluginId;
  		UInt32	_pluginCodepage;
  		
  		PluginDesc() : _size(sizeof(PluginDesc) - sizeof(_size)), _pluginId(0) {}
  	};
  	
#pragma pack(1)
  	struct ImplementationDesc {
  		UInt32	_size;
  		UInt32	_staticPropsNumber;
  		UInt32	_pluginId;
  		UInt32	_subtypeId;
  		UInt32	_pluginVersion;
  		UInt32	_implementationId;
  		UInt32	_pluginFlags;
  		UInt32	_compatibleInterfaceId;
  		UInt32	_interfaceId;
  		UInt32	_pluginCodepage;
  		
  		ImplementationDesc() : _size(sizeof(ImplementationDesc) - sizeof(_size)) {}
  	};
  	
    struct SharedPropDesc {
#pragma pack(1)
    	struct FixedPart {
			UInt32	_propId;
			UInt32	_size;
			
			FixedPart(UInt32 id) : _propId(id), _size(0) {}
    	}	_fixedPart;
#pragma pack()
    	std::vector<char*>	_value;
    	
    	SharedPropDesc(UInt32 id) : _fixedPart(id) {}
    };

    typedef std::map<NameContext::Ptr, Attributes>	AttrsMap;
    AttrsMap									_attrsMap;

    typedef std::vector<SharedPropDesc> 		SharedPropDescs;
    
  	struct ImplementationDescPair {  	
		ImplementationDesc	_implementationDesc;
		SharedPropDescs		_sharedProps;
  	};
  	
  	typedef	UInt32 ExportFuncDesc;
  	
    typedef std::vector<ImplementationDescPair> ImplementationDescs;
  	typedef std::vector<ExportFuncDesc> 		ExportFuncDescs;

	UInt32				_pluginVersion;
  	Header				_header;
  	PluginDesc			_plugin;
  	ExportFuncDescs		_exportFuncs;
  	ImplementationDescs	_implementations;
};

}
