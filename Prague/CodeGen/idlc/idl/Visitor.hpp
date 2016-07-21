#ifndef Visitor_hpp
#define Visitor_hpp

#include <boost/intrusive_ptr.hpp>

#include "Operations.hpp"

namespace idl { namespace parser {

#define METHOD_FOR(Type)                    \
  	virtual bool visitBefore(Type*) {return true;}	\
  	virtual bool visitAfter(Type*) {return true;}

class IntNode;
class FloatNode;
class NameNode;
class ErrorNode;
class Tuple;
class StubNode;
class BasicTypeNode;
class InOutNode;
class ImportEndNode;
class ImportStartNode;
class AttributeNode;
class OptionalValueNode;
class StringNode;

template <Operations::Op id>
class OperationNode;
  	
class Visitor {
  public:
  	METHOD_FOR(IntNode)
  	METHOD_FOR(FloatNode)
  	METHOD_FOR(StringNode)
  	METHOD_FOR(NameNode)
  	METHOD_FOR(StubNode)
  	METHOD_FOR(ErrorNode)
  	METHOD_FOR(Tuple)
  	METHOD_FOR(BasicTypeNode)
  	METHOD_FOR(InOutNode)
  	METHOD_FOR(ImportEndNode)
  	METHOD_FOR(ImportStartNode)
  	METHOD_FOR(AttributeNode)
  	METHOD_FOR(OptionalValueNode)
	METHOD_FOR(OperationNode<Operations::Add>)
	METHOD_FOR(OperationNode<Operations::And>)
	//METHOD_FOR(OperationNode<Operations::Array>)
	METHOD_FOR(OperationNode<Operations::As>)
	METHOD_FOR(OperationNode<Operations::BitAnd>)
	METHOD_FOR(OperationNode<Operations::BitInv>)
	METHOD_FOR(OperationNode<Operations::BitOr>)
	METHOD_FOR(OperationNode<Operations::BitXor>)
	METHOD_FOR(OperationNode<Operations::Block>)
	METHOD_FOR(OperationNode<Operations::Component>)
	METHOD_FOR(OperationNode<Operations::ComponentId>)
	//METHOD_FOR(OperationNode<Operations::ComponentFacet>)
	//METHOD_FOR(OperationNode<Operations::ComponentFacetInterface>)
	//METHOD_FOR(OperationNode<Operations::ComponentFacetVersion>)
	METHOD_FOR(OperationNode<Operations::ComponentVendor>)
	METHOD_FOR(OperationNode<Operations::ComponentVersion>)
	//METHOD_FOR(OperationNode<Operations::ComponentImplements>)
	METHOD_FOR(OperationNode<Operations::ComponentRequiredInterface>)
	METHOD_FOR(OperationNode<Operations::ComponentRequires>)
	METHOD_FOR(OperationNode<Operations::Dot>)
	METHOD_FOR(OperationNode<Operations::Const>)
	METHOD_FOR(OperationNode<Operations::ConstType>)
	METHOD_FOR(OperationNode<Operations::DataStruct>)
	METHOD_FOR(OperationNode<Operations::Div>)
	METHOD_FOR(OperationNode<Operations::Enum>)
	METHOD_FOR(OperationNode<Operations::EnumEntry>)
	METHOD_FOR(OperationNode<Operations::Eq>)
	METHOD_FOR(OperationNode<Operations::ErrorDef>)
	METHOD_FOR(OperationNode<Operations::ExternMethod>)
	//METHOD_FOR(OperationNode<Operations::For>)
	METHOD_FOR(OperationNode<Operations::ForwardIFace>)
	METHOD_FOR(OperationNode<Operations::ForwardStruct>)
	METHOD_FOR(OperationNode<Operations::FunCall>)
	METHOD_FOR(OperationNode<Operations::Ge>)
	METHOD_FOR(OperationNode<Operations::Gt>)
	//METHOD_FOR(OperationNode<Operations::If>)
	METHOD_FOR(OperationNode<Operations::Implementation>)
	METHOD_FOR(OperationNode<Operations::ImportName>)
	METHOD_FOR(OperationNode<Operations::ImportNameAsType>)
	METHOD_FOR(OperationNode<Operations::Interface>)
	METHOD_FOR(OperationNode<Operations::InterfaceId>)
	METHOD_FOR(OperationNode<Operations::Le>)
	METHOD_FOR(OperationNode<Operations::Let>)
	METHOD_FOR(OperationNode<Operations::Lt>)
	METHOD_FOR(OperationNode<Operations::Message>)
	METHOD_FOR(OperationNode<Operations::MessageAdd>)
	METHOD_FOR(OperationNode<Operations::MessageClass>)
	METHOD_FOR(OperationNode<Operations::MessageClassExtend>)
	METHOD_FOR(OperationNode<Operations::Method>)
	METHOD_FOR(OperationNode<Operations::MethodArg>)
	METHOD_FOR(OperationNode<Operations::Mul>)
	METHOD_FOR(OperationNode<Operations::Namespace>)
	METHOD_FOR(OperationNode<Operations::Ne>)
	METHOD_FOR(OperationNode<Operations::Neg>)
	METHOD_FOR(OperationNode<Operations::Nop>)
	METHOD_FOR(OperationNode<Operations::Not>)
	METHOD_FOR(OperationNode<Operations::Or>)
	METHOD_FOR(OperationNode<Operations::Property>)
	METHOD_FOR(OperationNode<Operations::PropertyDecl>)
	METHOD_FOR(OperationNode<Operations::PropertyPlugin>)
	METHOD_FOR(OperationNode<Operations::PropertyPluginAdd>)
	METHOD_FOR(OperationNode<Operations::RefType>)
	//METHOD_FOR(OperationNode<Operations::Sequence>)
	METHOD_FOR(OperationNode<Operations::Shared>)
	METHOD_FOR(OperationNode<Operations::ShiftLeft>)
	METHOD_FOR(OperationNode<Operations::ShiftRight>)
	METHOD_FOR(OperationNode<Operations::String>)
	METHOD_FOR(OperationNode<Operations::Struct>)
	METHOD_FOR(OperationNode<Operations::StructEntry>)
	METHOD_FOR(OperationNode<Operations::Sub>)
	METHOD_FOR(OperationNode<Operations::SysMethod>)
	METHOD_FOR(OperationNode<Operations::Typedef>)
	METHOD_FOR(OperationNode<Operations::Typecast>)
	METHOD_FOR(OperationNode<Operations::VarArg>)
	METHOD_FOR(OperationNode<Operations::Vendor>)
	METHOD_FOR(OperationNode<Operations::VendorId>)
	//METHOD_FOR(OperationNode<Operations::While>)
};

}} //namespace idl::parser

#endif //Visitor_hpp
