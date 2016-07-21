#ifndef OperationNode_hpp
#define OperationNode_hpp

#include "NodeInterface.hpp"
#include "Tuple.hpp"

namespace idl { namespace parser {

template <Operations::Op id>
class OperationNode : public NodeBase<OperationNode<id> > {
  public:
  	static const Operations::Op Id = id;
  
		OperationNode(const Context& context, Tuple::Ptr tuple) : Base(context), _tuple(tuple) {
		}
  	
  	Tuple::Ptr getTuple() const { return _tuple; }

  	NodeInterface::Ptr getArgNode(int i) const {
  		return _tuple->getNode(i);
  	}
  	
  	NodeInterface::Ptr replaceArg(int i, NodeInterface::Ptr arg) const {
  		return new OperationNode(getContext(), Tuple::replaceNode(*getTuple(), i, arg));
  	}

  	template <class T>
  	void getArg(int i, typename T& arg) {
  		NodeInterface::Ptr node = getArgNode(i);
			T::element_type* dyn = dynamic_cast<typename T::element_type*>(get_pointer(node));
  		ASSERT( dyn != 0 );
  		arg = T(static_cast<typename T::element_type*>(get_pointer(node)));
  	}
  	
  private:
  	typedef NodeBase<OperationNode<id> > Base;
  
  	virtual bool visitChildern(Visitor& visitor) { return _tuple->visit(visitor); }

  private:
  	Tuple::Ptr	_tuple;
};

typedef OperationNode<Operations::Dot> 			DotNode;
typedef OperationNode<Operations::Namespace> 	NamespaceNode;
typedef OperationNode<Operations::Interface> 	InterfaceNode;
	
//template <Operations::Op id>
//OperationNode<id>::OperationNode(const Context& context, Tuple::Ptr tuple) : Base(context), _tuple(tuple) {
//}

}} //namespace idl::parser

#endif //OperationNode_hpp
