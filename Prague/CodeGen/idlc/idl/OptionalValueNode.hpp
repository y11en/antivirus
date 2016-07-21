#ifndef OptionalValueNode_hpp
#define OptionalValueNode_hpp

#include "grammar.hpp"
#include "NodeInterface.hpp"
#include "Tuple.hpp"

namespace idl { namespace parser {

class OptionalValueNode : public NodeBase<OptionalValueNode> {
  public:
  	OptionalValueNode(const Context& context);
  	OptionalValueNode(const Context& context, NodeInterface::Ptr value);

	bool 				hasValue() const;
	NodeInterface::Ptr	getValue() const;
	
  private:
  	virtual bool visitChildern(Visitor& visitor);
  	
  private:
  	NodeInterface::Ptr	_value;
};

}} //namespace idl::parser

#endif //OptionalValueNode_hpp
