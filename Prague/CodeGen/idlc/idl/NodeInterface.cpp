#include "Context.hpp"

#include "NodeInterface.hpp"

#include "namecontext/Type.hpp"
#include "namecontext/Value.hpp"
#include "namecontext/NameContext.hpp"

namespace idl { namespace parser {

NodeInterface::Weak NodeInterface::getWeak() {
	return _thisWeak;
}

#pragma warning(suppress : 4355)
NodeInterface::NodeInterface() : _thisWeak(this, null_deleter()) {
}
  
NodeDescriptor::NodeDescriptor() {
}
  
NodeDescriptor::~NodeDescriptor() {
}

bool NodeDescriptor::isValue() const { 
	return dynamic_cast<Value*>(get_pointer(_nameContext)) != NULL; 
}

bool NodeDescriptor::isType() const {
	return dynamic_cast<Type*>(get_pointer(_nameContext)) != NULL; 
}

Type::Ptr NodeDescriptor::getType() const {
	ASSERT(isType());
	return static_cast<Type*>(get_pointer(_nameContext));
}

Value::Ptr NodeDescriptor::getValue() const {
	ASSERT(isValue());
	return static_cast<Value*>(get_pointer(_nameContext));
}

NameContext::Ptr NodeDescriptor::getNameContext() const {
	return _nameContext;
}

void NodeDescriptor::setNameContext(NameContext::Ptr nameContext) {
	_nameContext = nameContext;
}

}} //namespace idl::parser
