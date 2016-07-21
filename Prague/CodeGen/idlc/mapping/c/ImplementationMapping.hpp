#ifndef mapping_ImplementationMapping_hpp
#define mapping_ImplementationMapping_hpp

#include "InterfaceMapping.hpp"

#include <vector>

namespace idl {

class ImplementationMapping : public InterfaceMapping {
  public:
  	ImplementationMapping(Generator& generator);

  	void setPluginContext(Component::Ptr pluginContext);
  	  
	virtual void implementationOpen(const Attributes& attrs, Implementation::Ptr context);
	virtual void implementationClose(const Attributes& attrs, Implementation::Ptr context);
	virtual void sysMethodDecl(const Attributes& /*attrs*/, NameContext::Ptr context);
	virtual void propertyPluginDecl(const Attributes& attrs, Property::Ptr context);
	virtual void propertyPluginAddDecl(const Attributes& attrs, Property::Ptr context);

  private:
	void genImplementationMethods(Implementation::Ptr context);
	std::string genDataStructName(Implementation::Ptr context) const;
	void genDataStruct(Implementation::Ptr context);
	size_t hasSysMethod(const Name& name) const;
	void genVtbl(Implementation::Ptr context);
	void genSysMethodsTable(Implementation::Ptr context);
	void genImplementationRegisterExtern(Implementation::Ptr context);
	std::string genImplementationRegisterInterface(Implementation::Ptr context, Interface::Ptr iface) const;
	std::string genImplementationRegisterInterfaces(Implementation::Ptr context) const;
	void genImplementationRegister(Implementation::Ptr context);
	bool isHString(Property::Ptr property) const;
	
	std::string genAccessorPtr(Implementation::Ptr context, const Attributes& attrs, parser::AttributeNode::AttributeId id) const;
	std::string genGetterPtr(Implementation::Ptr context, const Attributes& attrs) const;
	std::string genSetterPtr(Implementation::Ptr context, const Attributes& attrs) const;
	std::string genPropertyValue(Property::Ptr property) const;
	void genPropertyTable(const Attributes& attrs, Implementation::Ptr context);
	void genSysMethods(Implementation::Ptr context);
	void genAccessors(const Attributes& attrs, Implementation::Ptr context);
	std::string genObjectStructPtrName(Implementation::Ptr context) const;
	
  private:
  	typedef std::vector<std::pair<Attributes, Property::Ptr> > PluginProperties;
  	typedef std::vector<NameContext::Ptr> SysMethods;
  	
  	PluginProperties	_pluginProperties;
  	SysMethods			_sysMethods;
  	Component::Ptr 		_pluginContext;
};

} // namespace idl

#endif //mapping_ImplementationMapping_hpp

