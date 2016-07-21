#ifndef InOutNode_hpp
#define InOutNode_hpp

#include "NodeInterface.hpp"

namespace idl { namespace parser {

class InOutNode : public NodeBase<InOutNode> {
  public:
  	InOutNode(const Context& context, bool isIn);

  	bool isIn() const;
  	bool isOut() const;

  private:
	bool	_in;
};

}} //namespace idl::parser

#endif //InOutNode_hpp
