#include "Context.hpp"
#include "FloatNode.hpp"
#include "namecontext/NameContext.hpp"

namespace idl { namespace parser {

FloatNode::FloatNode(const Context& context, idl::scaner::Float value) : NodeBase<FloatNode>(context), _value(value) {
}

idl::scaner::Float FloatNode::getValue() const {
	return _value;
}

}} //namespace idl::parser
