#ifndef Tuple_hpp
#define Tuple_hpp

#include "NodeInterface.hpp"

#include <vector>

namespace idl { namespace parser {

class Context;

class Tuple : public NodeBase<Tuple> {
  public:
  	Tuple(const Context& context = Context());
  	
  	void prepend(NodeInterface::Ptr node);

	void removeNode(size_t i);
	void replaceNode(size_t i, NodeInterface::Ptr node);
	NodeInterface::Ptr getNode(size_t i);
  	
	size_t getSize() const;

  	Tuple::Ptr transformTuple(Transformer& transformer);
  	
	virtual NodeInterface::Ptr transform(Transformer& transformer);

	static Tuple::Ptr replaceNode(const Tuple& tuple, size_t i, NodeInterface::Ptr);
	static Tuple::Ptr removeNode(const Tuple& tuple, size_t i);
		
  private:
  	virtual bool visitChildern(Visitor& visitor);
  	
  	typedef std::vector<NodeInterface::Ptr> Nodes;
  	
  	Nodes	_nodes;
};

}} //namespace idl::parser

#endif //Tuple_hpp
