#ifndef namecontext_MessageClassExtend_hpp
#define namecontext_MessageClassExtend_hpp

#include "MessageClass.hpp"

namespace idl {

class MessageClassExtend : public MessageClass {
  public:
  	typedef boost::intrusive_ptr<MessageClassExtend>	Ptr;

  	void setMessageClass(MessageClass::Ptr messageClass);
  	MessageClass::Ptr getMessageClass() const;
  	  	  
  private:
  	MessageClass::Weak	_messageClass;
};

} //namespace idl

#endif //namecontext_MessageClassExtend_hpp
