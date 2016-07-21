#ifndef mapping_Splitter_hpp
#define mapping_Splitter_hpp

#include <vector>
#include "mapping/Collector.hpp"

namespace idl {

class Splitter : public Collector {
	typedef std::vector<Collector*> Collectors;
	Collectors	_collectors;

	template <class ContextPtr>
	void redirect(const Attributes& attrs, typename ContextPtr context, void (Collector::*call)(const Attributes&, typename ContextPtr));

  public:
	void addCollector(Collector* collector);
	void flushCollectors();
	
  public:
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
	virtual void externalTypeDecl(ExternalType::Ptr context);
  virtual void importFile(const std::string& file);
	virtual void forwardIFace( NameContext::Ptr context );
	virtual void forwardStruct( NameContext::Ptr context );
	virtual void typecast( NameContext::Ptr context );
};

} //namespace idl

#endif //mapping_Splitter_hpp
