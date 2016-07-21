#ifndef mapping_InterfaceMapping_hpp
#define mapping_InterfaceMapping_hpp

#include "Context.hpp"
#include "mapping/Collector.hpp"

#include "CommonMapping.hpp"

#include <vector>

namespace idl {

class InterfaceMapping : public CommonMappingC {
  public:
  	typedef std::vector<Method::Ptr> Methods;
  public:
  	InterfaceMapping(Generator& generator);
  	virtual ~InterfaceMapping();

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

  private:
  public:
	void genInterfaceMethod(Interface::Ptr interface, Method::Ptr method, std::string& methodTypedef, std::string& methodVtbl, std::string& methodCall) const;
	void gatherSelfMethods(Interface::Ptr context, Methods& methods) const;
	void gatherMethods(Interface::Ptr context, Methods& methods) const;
	void genInterfaceMethods(Interface::Ptr context);
	void genErrors(bool isInterface, const std::string& id);
	std::string genMethodArgsDeclaration(Method::Ptr method, const std::string& thisArg = "") const;
	std::string genMethodArgsCall(Method::Ptr method, const std::string& thisArg = "") const;
	//std::string genConstValue(Performer::Ptr performer) const;
	std::string genPropertyType(Property::Ptr context) const;
	void genPropertyMacro(const Attributes& attrs, Property::Ptr context, bool local);
	void genPropertyDecl(const Attributes& attrs, Property::Ptr context, bool local);
	std::string genInterfaceStructPtrName(Interface::Ptr context) const;
	std::string genInterfaceVtblName(Interface::Ptr context) const;
	
  private:
  public:
  	typedef std::vector<std::pair<Attributes, NameContext::Ptr> > Errors;
  	
  	Errors _errors;
};

} //namespace idl

#endif //mapping_InterfaceMapping_hpp
