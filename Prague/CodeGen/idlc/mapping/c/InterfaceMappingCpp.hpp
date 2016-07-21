#ifndef mapping_InterfaceMappingCpp_hpp
#define mapping_InterfaceMappingCpp_hpp

#include "Context.hpp"
#include "namecontext/Property.hpp"
#include "mapping/Collector.hpp"

#include "CommonMapping.hpp"

#include <vector>

namespace idl {

class InterfaceMappingCpp : public CommonMappingCpp {
public:
	typedef std::vector<Method::Ptr> Methods;
protected:
	typedef std::vector<std::pair<Attributes, NameContext::Ptr> > Errors;

public:
	InterfaceMappingCpp(Generator& generator);
	virtual ~InterfaceMappingCpp();

public:
	virtual void interfaceOpen(const Attributes& attrs, Interface::Ptr context);
	virtual void interfaceClose(const Attributes& attrs, Interface::Ptr context);
	virtual void constDecl(const Attributes& attrs, ConstValue::Ptr context);
	virtual void methodDecl(const Attributes& /*attrs*/, Method::Ptr /*context*/);
	virtual void propertyDecl(const Attributes& attrs, Property::Ptr context);
	virtual void messageDecl(const Attributes& attrs, Message::Ptr context);
	virtual void messageClassDecl(const Attributes& attrs, MessageClass::Ptr context);
	virtual void messageClassExtendDecl(const Attributes& /*attrs*/, MessageClassExtend::Ptr /*context*/);
	virtual void errorDecl(const Attributes& attrs, NameContext::Ptr context);

protected:
	std::string genInterfaceDecl(const Attributes& attrs, Interface::Ptr context) const;
	std::string genIface2ImplDecl( const Attributes& attrs, Interface::Ptr context, std::string& impl ) const;
	std::string genInterfaceMethodCall(Interface::Ptr interface, Method::Ptr method) const;
	std::string genInterfaceMethodDecl(Interface::Ptr /*interface*/, Method::Ptr method) const;

	void gatherSelfMethods(Interface::Ptr context, Methods& methods) const;
	void gatherMethods(Interface::Ptr context, Methods& methods) const;

	std::string genInterfaceMethods(Interface::Ptr context) const;
	std::string genInterfaceMethodsCalls(Interface::Ptr context) const;
	std::string genError(const Errors::value_type& errorPair, bool isInterface, const std::string& id, int errorId) const;
	std::string genErrors(bool isInterface, const std::string& id) const;
	std::string genMethodArgsDeclaration(Method::Ptr method, const std::string& thisArg = "") const;
	std::string genMethodArgsCall(Method::Ptr method, const std::string& thisArg = "") const;
	//std::string genConstValue(Performer::Ptr performer);
	std::string genPropertyType(Property::Ptr context) const;
	std::string genPropertyId(const Attributes& attrs, Property::Ptr context, bool local) const;
	std::string genPropertyMacro(const Attributes& attrs, Property::Ptr context, bool local) const;
	std::string genPropertyDecl(const Attributes& attrs, Property::Ptr context, bool local) const;
	std::string genPropAccessors() const;

protected:
	typedef std::vector<Property::Ptr> IfaceProps;
	IfaceProps _ifaceProps;
	Errors     _errors;
};

} //namespace idl

#endif //mapping_InterfaceMapping_hpp
