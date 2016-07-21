#ifndef Transformer_hpp
#define Transformer_hpp

#include <boost/intrusive_ptr.hpp>

#include "Operations.hpp"

namespace idl { namespace parser {

#define METHOD_FOR_TRANSFORMER(Type)	virtual boost::intrusive_ptr<NodeInterface> transform(boost::intrusive_ptr<Type>);

class NodeInterface;

class IntNode;
class FloatNode;
class StringNode;
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

template <Operations::Op id>
class OperationNode;
  	
class Transformer {
  public:
  	METHOD_FOR_TRANSFORMER(IntNode)
  	METHOD_FOR_TRANSFORMER(FloatNode)
  	METHOD_FOR_TRANSFORMER(StringNode)
  	METHOD_FOR_TRANSFORMER(NameNode)
  	METHOD_FOR_TRANSFORMER(StubNode)
  	METHOD_FOR_TRANSFORMER(ErrorNode)
  	METHOD_FOR_TRANSFORMER(Tuple)
  	METHOD_FOR_TRANSFORMER(BasicTypeNode)
  	METHOD_FOR_TRANSFORMER(InOutNode)
  	METHOD_FOR_TRANSFORMER(ImportEndNode)
  	METHOD_FOR_TRANSFORMER(ImportStartNode)
  	METHOD_FOR_TRANSFORMER(AttributeNode)
  	METHOD_FOR_TRANSFORMER(OptionalValueNode)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Add>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::And>)
	//METHOD_FOR_TRANSFORMER(OperationNode<Operations::Array>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::As>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::BitAnd>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::BitInv>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::BitOr>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::BitXor>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Block>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Component>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::ComponentId>)
	//METHOD_FOR_TRANSFORMER(OperationNode<Operations::ComponentFacet>)
	//METHOD_FOR_TRANSFORMER(OperationNode<Operations::ComponentFacetInterface>)
	//METHOD_FOR_TRANSFORMER(OperationNode<Operations::ComponentFacetVersion>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::ComponentVendor>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::ComponentVersion>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::ComponentRequiredInterface>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::ComponentRequires>)
	//METHOD_FOR_TRANSFORMER(OperationNode<Operations::ComponentImplements>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Const>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::ConstType>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::DataStruct>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Dot>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Div>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Enum>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::EnumEntry>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Eq>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::ErrorDef>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::ExternMethod>)
	//METHOD_FOR_TRANSFORMER(OperationNode<Operations::For>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::ForwardIFace>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::ForwardStruct>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::FunCall>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Ge>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Gt>)
	//METHOD_FOR_TRANSFORMER(OperationNode<Operations::If>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Implementation>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::ImportName>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::ImportNameAsType>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Interface>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::InterfaceId>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Le>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Let>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Lt>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Message>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::MessageAdd>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::MessageClass>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::MessageClassExtend>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Method>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::MethodArg>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Mul>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Namespace>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Ne>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Neg>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Nop>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Not>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Or>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Property>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::PropertyDecl>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::PropertyPlugin>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::PropertyPluginAdd>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::RefType>)
	//METHOD_FOR_TRANSFORMER(OperationNode<Operations::Sequence>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Shared>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::ShiftLeft>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::ShiftRight>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::String>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Struct>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::StructEntry>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Sub>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::SysMethod>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Typedef>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Typecast>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::VarArg>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::Vendor>)
	METHOD_FOR_TRANSFORMER(OperationNode<Operations::VendorId>)
	//METHOD_FOR_TRANSFORMER(OperationNode<Operations::While>)
	
  protected:
	/*template <class T>
	T* copy(T* node) {
		T* copy = dynamic_cast<T*>(Transformer::transform(node));
		NodeInterface* newNode = Transformer::transform(node);
		ASSERT(dynamic_cast<T*>(newNode));
		return static_cast<T*>(newNode);
	}*/

  private:
	template <Operations::Op id>
	boost::intrusive_ptr<NodeInterface> transformOp(boost::intrusive_ptr<OperationNode<id> > node);
};

}} //namespace idl::parser

#endif //Transformer_hpp
