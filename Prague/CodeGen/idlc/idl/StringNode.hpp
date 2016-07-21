#ifndef StringNode_hpp
#define StringNode_hpp

#include "NodeInterface.hpp"

namespace idl { namespace parser {

class StringNode : public NodeBase<StringNode> {
  public:
  	StringNode(const Context& context, const idl::scaner::String& value);

  	const idl::scaner::String& getValue() const;

  private:
	idl::scaner::String _value;
};

}} //namespace idl::parser

#endif //StringNode_hpp
