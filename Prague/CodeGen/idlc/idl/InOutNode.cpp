#include "Context.hpp"
#include "InOutNode.hpp"
#include "namecontext/NameContext.hpp"

namespace idl { namespace parser {

InOutNode::InOutNode(const Context& context, bool in) : NodeBase<InOutNode>(context), _in(in) {
}

bool InOutNode::isIn() const {
	return _in;
}

bool InOutNode::isOut() const {
	return !_in;
}

}} //namespace idl::parser
