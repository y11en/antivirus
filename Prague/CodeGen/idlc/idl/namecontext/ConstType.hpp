#ifndef namecontext_ConstType_hpp
#define namecontext_ConstType_hpp

#include "Type.hpp"

namespace idl {

class ConstType : public Type {
  public:
  	typedef boost::intrusive_ptr<ConstType>	Ptr;
  	//virtual size_t getSize() const;

  private:
};

} //namespace idl

#endif //namecontext_ConstType_hpp
