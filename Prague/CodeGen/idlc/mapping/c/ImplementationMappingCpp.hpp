#ifndef mapping_ImplementationMappingCpp_hpp
#define mapping_ImplementationMappingCpp_hpp

#include "InterfaceMappingCpp.hpp"

#include <vector>

namespace idl {

class ImplementationMappingCpp : public InterfaceMappingCpp {
  public:
  	ImplementationMappingCpp(Generator& generator);

  	void setPluginContext(Component::Ptr pluginContext);
  	  
	virtual void implementationOpen(const Attributes& attrs, Implementation::Ptr context);
	virtual void implementationClose(const Attributes& attrs, Implementation::Ptr context);
	virtual void sysMethodDecl(const Attributes& /*attrs*/, NameContext::Ptr context);
	virtual void propertyPluginDecl(const Attributes& attrs, Property::Ptr context);
	virtual void propertyPluginAddDecl(const Attributes& attrs, Property::Ptr context);

  private:
	std::string genDataFields(Implementation::Ptr context) const;
	size_t hasSysMethod(const Name& name) const;
	std::string genVirtualMethodsTable(Implementation::Ptr context) const;
	std::string genSysMethodsTable(Implementation::Ptr context) const;
	std::string genImplementationRegisterExtern(Implementation::Ptr context) const;
	std::string genImplementationRegister(Implementation::Ptr context) const;
	std::string genImplementationRegisterInterface(Implementation::Ptr context, Interface::Ptr iface) const;
	std::string genImplementationRegisterInterfaces(Implementation::Ptr context) const;
	std::string genImplementationRegisterBody(Implementation::Ptr context) const;
	bool isHString(Property::Ptr property) const;
	
	std::string genAccessorPtr(const Attributes& attrs, parser::AttributeNode::AttributeId id) const;
	std::string genGetterPtr(const Attributes& attrs) const;
	std::string genSetterPtr(const Attributes& attrs) const;
	std::string genPropertyValue(Property::Ptr property) const;
	std::string genPropertyTable(const Attributes& attrs, Implementation::Ptr context) const;
	std::string genSysMethods(Implementation::Ptr context) const;
	std::string genAccessors(const Attributes& attrs, Implementation::Ptr context) const;
  
  private:
  	typedef std::vector<std::pair<Attributes, Property::Ptr> > PluginProperties;
  	typedef std::vector<NameContext::Ptr> SysMethods;
  	
  	PluginProperties	_pluginProperties;
  	SysMethods			_sysMethods;
  	Component::Ptr 		_pluginContext;
};

} // namespace idl

#endif //mapping_ImplementationMapping_hpp

