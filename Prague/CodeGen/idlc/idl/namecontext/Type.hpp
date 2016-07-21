#ifndef namecontext_Type_hpp
#define namecontext_Type_hpp

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "NameContext.hpp"

namespace idl {

class Type : public NameContext {
  public:
  	//static const int ReferenceSize = 4;
  
  	typedef boost::intrusive_ptr<Type>	Ptr;
  	typedef boost::weak_ptr<Type> Weak;
  	  	
  	Type();
  	Weak		getWeak();

  	virtual size_t getSize() const = 0;
  	  	
  	void setType(Type::Ptr type);
  
  	Type::Ptr	getType() const;
  	  	
	virtual NameContext::Ptr find( const Name& name, const Name& alt ) const;

  private:	
  	Weak					_type;
    boost::shared_ptr<Type> _this;
};

} //namespace idl

#endif //namecontext_Type_hpp
