#include "Context.hpp"
#include "Builder.hpp"
#include "StubNode.hpp"

#include "NodeInterface.hpp"
#include "Tuple.hpp"
#include "ErrorNode.hpp"
#include "NameNode.hpp"

#include "FloatNode.hpp"
#include "IntNode.hpp"
#include "StringNode.hpp"
#include "InOutNode.hpp"
#include "ImportEndNode.hpp"
#include "ImportStartNode.hpp"

#include "AttributeNode.hpp"
#include "OptionalValueNode.hpp"

#include "ErrorFactory.hpp"
#include "namecontext/NameContext.hpp"

#include <iostream>

namespace idl { namespace parser {

struct Builder::SyntaxError {
	Context		_context;
	std::string	_message;
};


void Builder::setRootNode(const Context& /*context*/, NodeInterface::Ptr root) {
	// GLR-parser sometimes causes double call on setRootNode when an error occurs
	if (!_root) {
		ASSERT(!_root);
		//std::cout << context << std::endl;
		_root = root;
		ASSERT(_root);
	}
}

NodeInterface::Ptr Builder::getRootNode() {
	ASSERT(_root);
	return _root;
}

void Builder::registerSyntaxError(const Context& context, const std::string& message) {
	ASSERT(!_registeredSyntaxError);
	
	_registeredSyntaxError = boost::shared_ptr<SyntaxError>(new SyntaxError);
	_registeredSyntaxError->_context = context;
	_registeredSyntaxError->_message = message;
	
	//ErrorFactory::instance().parseError(context, message);
}

Tuple::Ptr Builder::createTuple() {
	return Tuple::Ptr(new Tuple);
}

NodeInterface::Ptr Builder::createError(const Context& context, const std::string& message) {
	NodeInterface::Ptr error;
	
	//std::cout << ">: " << message << std::endl;
	
	if (_registeredSyntaxError) {
		//error = NodeInterface::Ptr(new ErrorNode(_registeredSyntaxError->_context, _registeredSyntaxError->_message));
		ErrorFactory::instance().parseError(_registeredSyntaxError->_context, message);
		error = NodeInterface::Ptr(new ErrorNode(_registeredSyntaxError->_context, message));
		_registeredSyntaxError.reset();
	} else {
		ErrorFactory::instance().parseError(context, message);
		error = NodeInterface::Ptr(new ErrorNode(context, message));
	}
	
	return error;
}

NodeInterface::Ptr 	Builder::createBasicType(const Context& context, BasicTypeNode::TypeId typeId) {
	return NodeInterface::Ptr(new BasicTypeNode(context, typeId));
}

NodeInterface::Ptr 	Builder::createInt(const Context& context, idl::scaner::Int value) {
	return NodeInterface::Ptr(new IntNode(context, value));
}
  	
NodeInterface::Ptr 	Builder::createFloat(const Context& context, idl::scaner::Float value) {
	return NodeInterface::Ptr(new FloatNode(context, value));
}

NodeInterface::Ptr 	Builder::createString(const Context& context, const idl::scaner::String& value) {
	return NodeInterface::Ptr(new StringNode(context, value));
}
  	
NodeInterface::Ptr 	Builder::createName(const Context& context, const idl::scaner::String& name) {
	return NodeInterface::Ptr(new NameNode(context, name));
}


// ---
NodeInterface::Ptr Builder::addName( const Context& context, const idl::scaner::String& name ) {
	return NodeInterface::Ptr(new NameNode(context, name));
}


NodeInterface::Ptr Builder::createMethodArgQualifier(const Context& context, bool isIn) {
	return NodeInterface::Ptr(new InOutNode(context, isIn));
}

NodeInterface::Ptr 	Builder::createImportStart(const Context& context, const idl::scaner::String& value) {
	return NodeInterface::Ptr(new ImportStartNode(context, value));
}

NodeInterface::Ptr 	Builder::createImportEnd(const Context& context, const idl::scaner::String& value) {
	return NodeInterface::Ptr(new ImportEndNode(context, value));
}


NodeInterface::Ptr 	Builder::createAttribute(const Context& context, int id, const idl::scaner::String& value) {
	return NodeInterface::Ptr(new AttributeNode(context, static_cast<AttributeNode::AttributeId>(id), value));
}

NodeInterface::Ptr 	Builder::createAttribute(const Context& context, int id, const idl::scaner::Int& value) {
	return NodeInterface::Ptr(new AttributeNode(context, static_cast<AttributeNode::AttributeId>(id), value));
}

NodeInterface::Ptr 	Builder::createAttribute(const Context& context, int id, Tuple::Ptr value) {
	return NodeInterface::Ptr(new AttributeNode(context, static_cast<AttributeNode::AttributeId>(id), value));
}

NodeInterface::Ptr 	Builder::createOptionalValue(const Context& context) {
	return NodeInterface::Ptr(new OptionalValueNode(context));
}

NodeInterface::Ptr 	Builder::createOptionalValue(const Context& context, NodeInterface::Ptr value) {
	return NodeInterface::Ptr(new OptionalValueNode(context, value));
}

//template <int id>
//NodeInterface::Ptr 	Builder::create(Tuple::Ptr tuple) {
//	return NodeInterface::Ptr();
//}

}} //namespace idl::parser

//#include "NameNode.hpp"
//#include "namecontext/traits.hpp"
#include "ValueFactory.hpp"
#include "idl_pfx.h"

struct SType {
	idl::parser::Value*	_value;
};

using boost::dynamic_pointer_cast;

namespace idl {

namespace scaner {
	bool checkPrefix( std::string& str, const char* pfx );
}

// ---
extern "C" SType checkIfaceName( SType nameNode ) {
	parser::NameNode::Ptr node = dynamic_pointer_cast<parser::NameNode>( nameNode._value->_node );
	if ( node ) {
		std::string name = node->getCoreName();
		if ( scaner::checkPrefix(name,IDL_IF_PFX) )
			node->setAltName( name );
	}
	return nameNode;
}

} // idl
