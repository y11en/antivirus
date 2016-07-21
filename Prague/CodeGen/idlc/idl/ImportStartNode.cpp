#include "Context.hpp"
#include "ImportStartNode.hpp"
#include "namecontext/NameContext.hpp"

namespace idl { namespace parser {

ImportStartNode::ImportStartNode(const Context& context, const idl::scaner::String& name) : NodeBase<ImportStartNode>(context), _filename(name) {
}

const idl::scaner::String& ImportStartNode::getFileName() const {
	return _filename;
}

}} //namespace idl::parser
