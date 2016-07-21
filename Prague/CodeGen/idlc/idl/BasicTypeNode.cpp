#include "Context.hpp"
#include "BasicTypeNode.hpp"
#include "namecontext/NameContext.hpp"

namespace idl { namespace parser {

BasicTypeNode::BasicTypeNode(const Context& context, TypeId typeId) : NodeBase<BasicTypeNode>(context), _typeId(typeId){
}

BasicTypeNode::TypeId BasicTypeNode::getTypeId() const {
	return _typeId;
}
  
}} //namespace idl::parser
