#ifndef NodeInterface_hpp
#define NodeInterface_hpp

#include "types.hpp"
#include "Visitor.hpp"
#include "Transformer.hpp"

#include <boost/weak_ptr.hpp>

namespace idl { 

class NameContext;
class Value;
class Type;

namespace parser {

class Context;

class NodeDescriptor {
  public:
  	typedef boost::intrusive_ptr<NameContext>	NameContextPtr;
  	typedef boost::intrusive_ptr<Value> 		ValuePtr;
  	typedef boost::intrusive_ptr<Type> 			TypePtr;

  	NodeDescriptor();
  	~NodeDescriptor();
  	  
  	bool			isValue() const;
  	bool			isType() const;
  	
  	TypePtr			getType() const;
  	ValuePtr		getValue() const;
  	NameContextPtr  getNameContext() const;
  	  	
  	void			setNameContext(NameContextPtr nameContext);
  	
  private:
  	NameContextPtr	_nameContext;
};

class NodeInterface : public RefCountBase {
  public:
  	typedef boost::weak_ptr<NodeInterface> 		Weak;
  	typedef boost::intrusive_ptr<NodeInterface> Ptr;

		bool visit(Visitor& visitor) {
			bool result = true;
			if (visitBefore(visitor)) {
				result = visitChildern(visitor);
			}

			if (result) {
				result = visitAfter(visitor);
			}

			return result;
		}

  	//virtual NodeInterface* clone() const = 0;

  	Weak getWeak();
  	
  	virtual NodeDescriptor& getDescriptor() { return _descriptor; }
  	  	  	
  	virtual const Context& getContext() const = 0;
  	
  	virtual Ptr transform(Transformer& transformer) = 0;
  	  	  	
  	virtual ~NodeInterface() {}

  protected:
  	NodeInterface();
  	
  private:
  	virtual bool visitBefore(Visitor& visitor) = 0;	// whether to process children recursively
  	virtual bool visitAfter(Visitor& visitor) = 0;	// whether to continue visiting at all
  	virtual bool visitChildern(Visitor& ) { return true; }	// whether to continue visiting at all (must be inhereted from visitAfter)
  	
  private:
  	NodeDescriptor						_descriptor;
    boost::shared_ptr<NodeInterface>	_thisWeak;
};

template <class T>
class NodeBase : public NodeInterface {
  public:
  	typedef boost::intrusive_ptr<T> Ptr;
  	
  	virtual const Context& getContext() const {
  		return _context;
  	}

  	/*virtual NodeInterface* clone() const {
  		ASSERT(typeid(T) == typeid(*this));
  		return new T(static_cast<const T&>(*this));
  	}*/

  	virtual NodeInterface::Ptr transform(Transformer& transformer) {
  		ASSERT(typeid(T) == typeid(*this));
  		ASSERT(canBeLocallyWrapped());
  		return transformer.transform(Ptr(static_cast<T*>(this)));
  	}
  	  	  	
  protected:
  	NodeBase(const Context& context) {
  		_context = context;
  	}
  	
  private:
  	virtual bool visitBefore(Visitor& visitor) {
  		ASSERT(typeid(T) == typeid(*this));
  		return visitor.visitBefore(static_cast<T*>(this));
  	}

  	virtual bool visitAfter(Visitor& visitor) {
  		ASSERT(typeid(T) == typeid(*this));
  		return visitor.visitAfter(static_cast<T*>(this));
  	}
  	
  private:
  	Context		_context;
};

}} //namespace idl::parser


#endif //NodeInterface_hpp
