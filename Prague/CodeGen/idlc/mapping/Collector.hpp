#ifndef mappign_Collector_hpp
#define mappign_Collector_hpp

#include <vector>
#include <string>

#include "idl/namecontext/traits.hpp"
#include "idl/AttributeNode.hpp"

namespace idl {

struct Performer;
typedef boost::shared_ptr<Performer> PerformerPtr;

class Collector {
  public:
  	typedef std::vector<idl::parser::AttributeNode::Ptr> Attributes;

	public:
		virtual bool processImports() const { return false; }

		virtual void constDecl(const Attributes& attrs, ConstValue::Ptr context) = 0;
		virtual void typedefDecl(const Attributes& attrs, Typedef::Ptr context) = 0;
		virtual void structDecl(const Attributes& attrs, Struct::Ptr context) = 0;
		virtual void enumDecl(const Attributes& attrs, Enum::Ptr context) = 0;
		virtual void interfaceOpen(const Attributes& attrs, Interface::Ptr context) = 0;
		virtual void interfaceClose(const Attributes& attrs, Interface::Ptr context) = 0;
		virtual void methodDecl(const Attributes& attrs, Method::Ptr context) = 0;
		virtual void externMethodDecl(const Attributes& attrs, ExternMethod::Ptr context) = 0;
		virtual void componentOpen(const Attributes& attrs, Component::Ptr context) = 0;
		virtual void componentClose(const Attributes& attrs, Component::Ptr context) = 0;
		virtual void implementationOpen(const Attributes& attrs, Implementation::Ptr context) = 0;
		virtual void implementationClose(const Attributes& attrs, Implementation::Ptr context) = 0;
		virtual void sysMethodDecl(const Attributes& attrs, NameContext::Ptr context) = 0;
		virtual void propertyDecl(const Attributes& attrs, Property::Ptr context) = 0;
		virtual void propertyPluginDecl(const Attributes& attrs, Property::Ptr context) = 0;
		virtual void propertyPluginAddDecl(const Attributes& attrs, Property::Ptr context) = 0;
		virtual void messageDecl(const Attributes& attrs, Message::Ptr context) = 0;
		virtual void messageClassDecl(const Attributes& attrs, MessageClass::Ptr context) = 0;
		virtual void messageClassExtendDecl(const Attributes& attrs, MessageClassExtend::Ptr context) = 0;
		virtual void errorDecl(const Attributes& attrs, NameContext::Ptr context) = 0;
		virtual void forwardIFace( NameContext::Ptr context ) = 0;
		virtual void forwardStruct( NameContext::Ptr context ) = 0;
		virtual void typecast( NameContext::Ptr context ) = 0;

		virtual void externalTypeDecl(ExternalType::Ptr context) = 0;

		virtual void importFile(const std::string& file) = 0;

		//void visit(CollectorVisitor& visitor) const;

private:
};

extern void collect(parser::NodeInterface::Ptr root, Collector& collector);

} //namespace idl

#endif //mappign_Collector_hpp
