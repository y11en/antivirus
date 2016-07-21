#ifndef namecontext_RefType_hpp
#define namecontext_RefType_hpp

#include "Type.hpp"

namespace idl {

class RefType : public Type {
  public:
  	typedef boost::intrusive_ptr<RefType>	Ptr;

  	virtual size_t getSize() const;
  	
  	void setCount(int count);
  	void setConst(bool isConst);
  	
  	int		getCount() const;
  	bool	isConst() const;
  	
  private:
  	int		_count;
  	bool	_const;
};

} //namespace idl

#endif //namecontext_RefType_hpp
