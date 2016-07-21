#ifndef Builder_hpp
#define Builder_hpp

#include "types.hpp"

#include "NodeInterface.hpp"
#include "BasicTypeNode.hpp"
#include "Tuple.hpp"
#include "OperationNode.hpp"

#include <string>
#include <boost/shared_ptr.hpp>

namespace idl { namespace parser {

class Context;

class Builder {
  public:
  	void				setRootNode(const Context& context, NodeInterface::Ptr root);
  	NodeInterface::Ptr	getRootNode();
  
  	void registerSyntaxError(const Context& context, const std::string& message);
  
  	Tuple::Ptr 			createTuple();
  	NodeInterface::Ptr 	createError(const Context& context, const std::string& message);
  	NodeInterface::Ptr 	createBasicType(const Context& context, BasicTypeNode::TypeId typeId);
  	NodeInterface::Ptr 	createInt(const Context& context, idl::scaner::Int value);
  	NodeInterface::Ptr 	createFloat(const Context& context, idl::scaner::Float value);
  	NodeInterface::Ptr 	createString(const Context& context, const idl::scaner::String& value);
  	NodeInterface::Ptr 	createName(const Context& context, const idl::scaner::String& value);
  	NodeInterface::Ptr 	addName( const Context& context, const idl::scaner::String& value );
  	NodeInterface::Ptr 	createImportStart(const Context& context, const idl::scaner::String& value);
  	NodeInterface::Ptr 	createImportEnd(const Context& context, const idl::scaner::String& value);
  	
  	NodeInterface::Ptr 	createMethodArgQualifier(const Context& context, bool isIn);
  	
  	NodeInterface::Ptr 	createAttribute(const Context& context, int id, const idl::scaner::String& value);
  	NodeInterface::Ptr 	createAttribute(const Context& context, int id, const idl::scaner::Int& value);
  	NodeInterface::Ptr 	createAttribute(const Context& context, int id, Tuple::Ptr value);
  	
  	NodeInterface::Ptr 	createOptionalValue(const Context& context);
  	NodeInterface::Ptr 	createOptionalValue(const Context& context, NodeInterface::Ptr value);
  	
  	template <Operations::Op id>
  	NodeInterface::Ptr 	create(const Context& context, Tuple::Ptr tuple);
  	
  private:
  	struct SyntaxError;
  	
  	NodeInterface::Ptr				_root;
  	boost::shared_ptr<SyntaxError> 	_registeredSyntaxError;
};

template <Operations::Op id>
NodeInterface::Ptr Builder::create(const Context& context, Tuple::Ptr tuple) {
	return NodeInterface::Ptr(new OperationNode<id>(context, tuple));
}

}} //namespace idl::parser

#endif //Builder_hpp
