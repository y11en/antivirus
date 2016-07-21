#ifndef namecontext_traits_hpp
#define namecontext_traits_hpp

#include "idl/ArgsLayout.hpp"
#include "idl/Tuple.hpp"
#include "idl/NameNode.hpp"
#include "idl/OptionalValueNode.hpp"
#include "Namespace.hpp"
#include "Interface.hpp"
#include "Component.hpp"
#include "Implementation.hpp"
#include "Message.hpp"
#include "MessageClass.hpp"
#include "MessageClassExtend.hpp"
#include "Property.hpp"
#include "Typedef.hpp"
#include "typecast.hpp"
#include "Struct.hpp"
#include "Enum.hpp"
#include "Method.hpp"
#include "ExternMethod.hpp"
#include "ExternalType.hpp"
#include "ConstValue.hpp"

namespace idl {

template <parser::Operations::Op op>
struct op_traits {};

template <parser::Operations::Op op>
struct op_traits_extend;

#define TYPICAL_TRAITS(op, context)                             \
	template <>                                                   \
	struct op_traits<parser::Operations::op> {                    \
		enum { Operation = parser::Operations::op };                \
		typedef ArgsLayout::op Layout;                              \
		typedef parser::OperationNode<parser::Operations::op> Node; \
		typedef context Context;                                    \
	};

TYPICAL_TRAITS(ForwardIFace, NameContext)
TYPICAL_TRAITS(ForwardStruct, NameContext)
TYPICAL_TRAITS(Interface, Interface)
TYPICAL_TRAITS(Component, Component)
TYPICAL_TRAITS(Implementation, Implementation)
TYPICAL_TRAITS(Enum, Enum)
TYPICAL_TRAITS(Struct, Struct)
TYPICAL_TRAITS(MessageClass, MessageClass)
TYPICAL_TRAITS(MessageClassExtend, MessageClassExtend)
TYPICAL_TRAITS(Namespace, Namespace)
TYPICAL_TRAITS(ErrorDef, NameContext)
TYPICAL_TRAITS(Typedef, Typedef)
TYPICAL_TRAITS(Typecast, Typecast)
TYPICAL_TRAITS(EnumEntry, Value)
TYPICAL_TRAITS(StructEntry, Value)
TYPICAL_TRAITS(Const, ConstValue)
TYPICAL_TRAITS(Method, Method)
TYPICAL_TRAITS(MethodArg, Value)
TYPICAL_TRAITS(ExternMethod, ExternMethod)
TYPICAL_TRAITS(SysMethod, NameContext)
TYPICAL_TRAITS(Message, Message)
TYPICAL_TRAITS(Property, Property)
TYPICAL_TRAITS(PropertyDecl, Property)
TYPICAL_TRAITS(PropertyPlugin, Property)
TYPICAL_TRAITS(PropertyPluginAdd, Property)
TYPICAL_TRAITS(ImportName, ExternalType)
TYPICAL_TRAITS(ImportNameAsType, ExternalType)

template <>
struct op_traits_extend<parser::Operations::PropertyPlugin> {
	typedef parser::OptionalValueNode ConstValueNode;
};

template <>
struct op_traits_extend<parser::Operations::PropertyPluginAdd> {
	typedef parser::OptionalValueNode ConstValueNode;
};

template <>
struct op_traits_extend<parser::Operations::Const> {
	typedef parser::NodeInterface ConstValueNode;
};

struct OpTools {
	template <parser::Operations::Op Op>
	static parser::Tuple::Ptr attrs(typename parser::OperationNode<Op>* node) {
		ASSERT(node != NULL);
		parser::Tuple::Ptr attrs;
		node->getArg(op_traits<Op>::Layout::Attrs, attrs);
		ASSERT(attrs);
		return attrs;
	}
	
	template <parser::Operations::Op Op>
	static parser::NameNode::Ptr nameNode(typename parser::OperationNode<Op>* node) {
		ASSERT(node != NULL);
		parser::NameNode::Ptr nameNode;
		node->getArg(op_traits<Op>::Layout::Name, nameNode);
		ASSERT(nameNode);
		return nameNode;
	}

	template <parser::Operations::Op Op>
	static parser::NodeInterface::Ptr body(typename parser::OperationNode<Op>* node) {
		ASSERT(node != NULL);
		parser::NodeInterface::Ptr bodyNode;
		node->getArg(op_traits<Op>::Layout::Body, bodyNode);
		ASSERT(bodyNode);
		return bodyNode;
	}
	
	template <parser::Operations::Op Op>
	static Type::Ptr type(typename parser::OperationNode<Op>* node) {
		ASSERT(node != NULL);
		parser::NodeInterface::Ptr typeNode;
		node->getArg(op_traits<Op>::Layout::Type, typeNode);
		ASSERT(typeNode);
		ASSERT(typeNode->getDescriptor().isType());
		Type::Ptr type = typeNode->getDescriptor().getType();
		ASSERT(type);
				
		return type;
	}
	
	template <parser::Operations::Op Op>
	static const Name& name(typename parser::OperationNode<Op>* node) {
		parser::NameNode::Ptr nameNode = OpTools::nameNode(node);
		return nameNode->getCoreName();
	}

	template <parser::Operations::Op Op>
	static typename op_traits<Op>::Context::Ptr context(typename parser::OperationNode<Op>* node) {
		parser::NameNode::Ptr nameNode = OpTools::nameNode(node);
		NameContext::Ptr nameContext = nameNode->getDescriptor().getNameContext();
		if ( !nameContext )
			return 0;
		//ASSERT(nameContext);
		typename op_traits<Op>::Context::Ptr context = boost::dynamic_pointer_cast<typename op_traits<Op>::Context>(nameContext);
		ASSERT(context);
		return context;
	}

	template <parser::Operations::Op Op>
	static typename op_traits_extend<Op>::ConstValueNode::Ptr value(typename parser::OperationNode<Op>* node) {
		ASSERT(node != NULL);
		op_traits_extend<Op>::ConstValueNode::Ptr valueNode;
		node->getArg(op_traits<Op>::Layout::Value, valueNode);
		ASSERT(valueNode);
		return valueNode;
	}
};
	
} // namespace idl

#endif namecontext_traits_hpp
