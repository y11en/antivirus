#include "Context.hpp"

#include "IntNode.hpp"

namespace idl { namespace parser {

IntNode::IntNode(const Context& context, idl::scaner::Int value) : NodeBase<IntNode>(context), _value(value) {
}

idl::scaner::Int IntNode::getValue() const {
	return _value;
}

}} //namespace idl::parser
