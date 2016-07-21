#include "Context.hpp"
#include "ImportEndNode.hpp"
#include "namecontext/NameContext.hpp"

namespace idl { namespace parser {

ImportEndNode::ImportEndNode(const Context& context, const idl::scaner::String& name) : NodeBase<ImportEndNode>(context), _filename(name) {
}

const idl::scaner::String& ImportEndNode::getFileName() const {
	return _filename;
}

}} //namespace idl::parser
