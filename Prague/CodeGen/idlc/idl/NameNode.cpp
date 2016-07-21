#include "Context.hpp"
#include "NameNode.hpp"

namespace idl { namespace parser {

NameNode::NameNode(const Context& context, const idl::scaner::String& name) : NodeBase<NameNode>(context), _name(name) {
}

const idl::scaner::String& NameNode::getRealName() const {
	if ( _alt.empty() )
		return _name;
	return _alt;
}

const idl::scaner::String& NameNode::getCoreName() const {
	return _name;
}

void NameNode::setAltName( const Name& name ) {
	_alt = name;
}


const Name& NameNode::getAltName() const {
	return _alt;
}


}} //namespace idl::parser
