#ifndef FloatNode_hpp
#define FloatNode_hpp

#include "NodeInterface.hpp"

namespace idl { namespace parser {

class FloatNode : public NodeBase<FloatNode> {
  public:
  	FloatNode(const Context& context, idl::scaner::Float value);

  	idl::scaner::Float getValue() const;

  private:
	idl::scaner::Float _value;
};

}} //namespace idl::parser

#endif //FloatNode_hpp
