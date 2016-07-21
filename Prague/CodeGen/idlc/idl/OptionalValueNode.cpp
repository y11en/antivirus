#include "Context.hpp"
#include "OptionalValueNode.hpp"
#include "Visitor.hpp"

namespace idl { namespace parser {

OptionalValueNode::OptionalValueNode(const Context& context) : NodeBase<OptionalValueNode>(context) {
}

OptionalValueNode::OptionalValueNode(const Context& context, NodeInterface::Ptr value) : NodeBase<OptionalValueNode>(context), _value(value) {
	ASSERT(_value);
}

bool OptionalValueNode::hasValue() const {
	return _value;
}

NodeInterface::Ptr OptionalValueNode::getValue() const {
	return _value;
}

bool OptionalValueNode::visitChildern(Visitor& visitor) {
	if (!hasValue()) {
		return true;
	}
	return _value->visit(visitor);
}
  	
}} //namespace idl::parser
