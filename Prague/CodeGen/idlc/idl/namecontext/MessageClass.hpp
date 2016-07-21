#ifndef namecontext_MessageClass_hpp
#define namecontext_MessageClass_hpp

#include "NameContext.hpp"

namespace idl {

class MessageClass : public NameContext {
  public:
  	typedef boost::intrusive_ptr<MessageClass>	Ptr;
  	typedef boost::weak_ptr<MessageClass> Weak;
  	
  	MessageClass();
  	
  	Weak getWeak();
  	  
  private:	
    boost::shared_ptr<MessageClass> _this;
};

} //namespace idl

#endif //namecontext_MessageClass_hpp
