#ifndef ErrorNode_hpp
#define ErrorNode_hpp

#include <string>
#include "Context.hpp"
#include "NodeInterface.hpp"

namespace idl { namespace parser {

class ErrorNode : public NodeBase<ErrorNode> {
  public:
  	ErrorNode(const Context& context, const std::string& message);
  	  	
 private:
 	std::string	_message;
};

}} //namespace idl::parser

#endif //ErrorNode_hpp