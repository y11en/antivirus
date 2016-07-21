#include "ErrorNode.hpp"
#include "Context.hpp"
#include "ErrorFactory.hpp"
#include "namecontext/NameContext.hpp"

#include <iostream>

namespace idl { namespace parser {

ErrorNode::ErrorNode(const Context& context, const std::string& message)
		: NodeBase<ErrorNode>(context), _message(message) {
	//ErrorFactory::instance().parseError(context, message);
	//std::cout << getContext() << ": " << _message << std::endl;
}

}} //namespace idl::parser
