#ifndef namecontext_Value_hpp
#define namecontext_Value_hpp

#include "NameContext.hpp"
#include "Type.hpp"

namespace idl {

class Value : public NameContext {
  public:
  	typedef boost::intrusive_ptr<Value>	Ptr;
  	typedef boost::weak_ptr<Value> Weak;

  	Value();
  	  	
  	void setType(Type::Ptr type);
  
  	Type::Ptr	getType() const;
  	Weak		getWeak();

	virtual NameContext::Ptr find( const Name& name, const Name& alt ) const;

  private:	
    boost::shared_ptr<Value>	_this;
  	Type::Weak					_type;
};

} //namespace idl

#endif //namecontext_Value_hpp
