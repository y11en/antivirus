#ifndef IntNode_hpp
#define IntNode_hpp

#include "NodeInterface.hpp"

namespace idl { namespace parser {

class IntNode : public NodeBase<IntNode> {
  public:
  	IntNode(const Context& context, idl::scaner::Int value);

  	idl::scaner::Int getValue() const;

  private:
	idl::scaner::Int _value;
};

}} //namespace idl::parser

#endif //IntNode_hpp
