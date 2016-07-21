#include "Transformer.hpp"

#include <boost/intrusive_ptr.hpp>

#include "Context.hpp"

#include "Operations.hpp"

#include "NodeInterface.hpp"
#include "Tuple.hpp"
#include "ErrorNode.hpp"
#include "IntNode.hpp"
#include "FloatNode.hpp"
#include "StringNode.hpp"
#include "NameNode.hpp"
#include "OperationNode.hpp"
#include "StubNode.hpp"
#include "BasicTypeNode.hpp"
#include "InOutNode.hpp"
#include "ImportEndNode.hpp"
#include "ImportStartNode.hpp"
#include "AttributeNode.hpp"
#include "OptionalValueNode.hpp"

namespace idl { namespace parser {

#define IMPL_FOR_OP(Type)		boost::intrusive_ptr<NodeInterface> Transformer::transform(boost::intrusive_ptr<Type> node) { return transformOp(node); }
#define IMPL_FOR_LEAF(Type)		boost::intrusive_ptr<NodeInterface> Transformer::transform(boost::intrusive_ptr<Type> node) { return node; }

IMPL_FOR_OP(OperationNode<Operations::Add>)
IMPL_FOR_OP(OperationNode<Operations::And>)
//IMPL_FOR_OP(OperationNode<Operations::Array>)
IMPL_FOR_OP(OperationNode<Operations::As>)
IMPL_FOR_OP(OperationNode<Operations::BitAnd>)
IMPL_FOR_OP(OperationNode<Operations::BitInv>)
IMPL_FOR_OP(OperationNode<Operations::BitOr>)
IMPL_FOR_OP(OperationNode<Operations::BitXor>)
IMPL_FOR_OP(OperationNode<Operations::Block>)
IMPL_FOR_OP(OperationNode<Operations::Component>)
IMPL_FOR_OP(OperationNode<Operations::ComponentId>)
//IMPL_FOR_OP(OperationNode<Operations::ComponentFacet>)
//IMPL_FOR_OP(OperationNode<Operations::ComponentFacetInterface>)
//IMPL_FOR_OP(OperationNode<Operations::ComponentFacetVersion>)
IMPL_FOR_OP(OperationNode<Operations::ComponentVendor>)
IMPL_FOR_OP(OperationNode<Operations::ComponentVersion>)
//IMPL_FOR_OP(OperationNode<Operations::ComponentImplements>)
IMPL_FOR_OP(OperationNode<Operations::ComponentRequiredInterface>)
IMPL_FOR_OP(OperationNode<Operations::ComponentRequires>)
IMPL_FOR_OP(OperationNode<Operations::Const>)
IMPL_FOR_OP(OperationNode<Operations::ConstType>)
IMPL_FOR_OP(OperationNode<Operations::DataStruct>)
IMPL_FOR_OP(OperationNode<Operations::Dot>)
IMPL_FOR_OP(OperationNode<Operations::Div>)
IMPL_FOR_OP(OperationNode<Operations::Enum>)
IMPL_FOR_OP(OperationNode<Operations::EnumEntry>)
IMPL_FOR_OP(OperationNode<Operations::Eq>)
IMPL_FOR_OP(OperationNode<Operations::ErrorDef>)
IMPL_FOR_OP(OperationNode<Operations::ExternMethod>)
//IMPL_FOR_OP(OperationNode<Operations::For>)
IMPL_FOR_OP(OperationNode<Operations::ForwardIFace>)
IMPL_FOR_OP(OperationNode<Operations::ForwardStruct>)
IMPL_FOR_OP(OperationNode<Operations::FunCall>)
IMPL_FOR_OP(OperationNode<Operations::Ge>)
IMPL_FOR_OP(OperationNode<Operations::Gt>)
//IMPL_FOR_OP(OperationNode<Operations::If>)
IMPL_FOR_OP(OperationNode<Operations::Implementation>)
IMPL_FOR_OP(OperationNode<Operations::ImportName>)
IMPL_FOR_OP(OperationNode<Operations::ImportNameAsType>)
IMPL_FOR_OP(OperationNode<Operations::Interface>)
IMPL_FOR_OP(OperationNode<Operations::InterfaceId>)
IMPL_FOR_OP(OperationNode<Operations::Le>)
IMPL_FOR_OP(OperationNode<Operations::Let>)
IMPL_FOR_OP(OperationNode<Operations::Lt>)
IMPL_FOR_OP(OperationNode<Operations::Message>)
IMPL_FOR_OP(OperationNode<Operations::MessageAdd>)
IMPL_FOR_OP(OperationNode<Operations::MessageClass>)
IMPL_FOR_OP(OperationNode<Operations::MessageClassExtend>)
IMPL_FOR_OP(OperationNode<Operations::Method>)
IMPL_FOR_OP(OperationNode<Operations::MethodArg>)
IMPL_FOR_OP(OperationNode<Operations::Mul>)
IMPL_FOR_OP(OperationNode<Operations::Namespace>)
IMPL_FOR_OP(OperationNode<Operations::Ne>)
IMPL_FOR_OP(OperationNode<Operations::Neg>)
IMPL_FOR_OP(OperationNode<Operations::Nop>)
IMPL_FOR_OP(OperationNode<Operations::Not>)
IMPL_FOR_OP(OperationNode<Operations::Or>)
IMPL_FOR_OP(OperationNode<Operations::Property>)
IMPL_FOR_OP(OperationNode<Operations::PropertyDecl>)
IMPL_FOR_OP(OperationNode<Operations::PropertyPlugin>)
IMPL_FOR_OP(OperationNode<Operations::PropertyPluginAdd>)
IMPL_FOR_OP(OperationNode<Operations::RefType>)
//IMPL_FOR_OP(OperationNode<Operations::Sequence>)
IMPL_FOR_OP(OperationNode<Operations::Shared>)
IMPL_FOR_OP(OperationNode<Operations::ShiftLeft>)
IMPL_FOR_OP(OperationNode<Operations::ShiftRight>)
IMPL_FOR_OP(OperationNode<Operations::String>)
IMPL_FOR_OP(OperationNode<Operations::Struct>)
IMPL_FOR_OP(OperationNode<Operations::StructEntry>)
IMPL_FOR_OP(OperationNode<Operations::Sub>)
IMPL_FOR_OP(OperationNode<Operations::SysMethod>)
IMPL_FOR_OP(OperationNode<Operations::Typedef>)
IMPL_FOR_OP(OperationNode<Operations::Typecast>)
IMPL_FOR_OP(OperationNode<Operations::VarArg>)
IMPL_FOR_OP(OperationNode<Operations::Vendor>)
IMPL_FOR_OP(OperationNode<Operations::VendorId>)
//IMPL_FOR_OP(OperationNode<Operations::While>)

IMPL_FOR_LEAF(StubNode)
IMPL_FOR_LEAF(IntNode)
IMPL_FOR_LEAF(FloatNode)
IMPL_FOR_LEAF(StringNode)
IMPL_FOR_LEAF(NameNode)
IMPL_FOR_LEAF(ErrorNode)
IMPL_FOR_LEAF(BasicTypeNode)
IMPL_FOR_LEAF(InOutNode)
IMPL_FOR_LEAF(ImportEndNode)
IMPL_FOR_LEAF(ImportStartNode)
IMPL_FOR_LEAF(AttributeNode)
IMPL_FOR_LEAF(OptionalValueNode)

NodeInterface::Ptr Transformer::transform(Tuple::Ptr ) {
	ASSERT(!"it must not be called. Tuple::transform should be used instead");
	return 0;
}

template <Operations::Op id>
boost::intrusive_ptr<NodeInterface> Transformer::transformOp(typename boost::intrusive_ptr<OperationNode<id> > node) {
	Tuple::Ptr tuple = node->getTuple()->transformTuple(*this);
	
	if (tuple == node->getTuple())
		return node;
		
	return new OperationNode<id>(node->getContext(), tuple);
}

}} //namespace idl::parser
