#ifndef namecontext_SequenceType_hpp
#define namecontext_SequenceType_hpp

#include "Type.hpp"

namespace idl {

class SequenceType : public Type {
  public:
  	typedef boost::intrusive_ptr<SequenceType>	Ptr;

	void setType(Type::Ptr type, size_t size);
	//Type::Ptr getType() const;

	size_t getNumberOfItems() const;

	virtual size_t getSize() const;
	
  private:
  	size_t		_size;
	//Type::Weak	_type;
};

} //namespace idl

#endif //namecontext_SequenceType_hpp
