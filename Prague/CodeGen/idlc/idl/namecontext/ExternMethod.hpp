#ifndef namecontext_ExternMethod_hpp
#define namecontext_ExternMethod_hpp

#include "Method.hpp"

namespace idl {

class ExternMethod : public Method {
  public:
  	typedef boost::intrusive_ptr<ExternMethod> Ptr;
  	
  	//typedef unsigned int Id;
  	
  	//Id getId() const;
  	
  private:
};

} //namespace idl

#endif //namecontext_ExternMethod_hpp
