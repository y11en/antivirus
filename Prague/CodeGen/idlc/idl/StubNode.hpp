#ifndef StubNode_hpp
#define StubNode_hpp

#include "NodeInterface.hpp"

namespace idl { namespace parser {

class Context;

class StubNode : public NodeBase<StubNode> {
  public:
  	StubNode(const Context& context) : NodeBase<StubNode>(context) {
  	}
};

}} //namespace idl::parser

#endif //StubNode_hpp
