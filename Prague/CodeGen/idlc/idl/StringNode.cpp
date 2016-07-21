#include "Context.hpp"
#include "StringNode.hpp"

namespace idl { namespace parser {

StringNode::StringNode(const Context& context, const idl::scaner::String& value) : NodeBase<StringNode>(context), _value(value) {
}

const idl::scaner::String& StringNode::getValue() const {
	return _value;
}

}} //namespace idl::parser
