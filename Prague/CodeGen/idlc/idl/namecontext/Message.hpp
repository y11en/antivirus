#ifndef namecontext_Message_hpp
#define namecontext_Message_hpp

#include "MessageClass.hpp"

namespace idl {

class Message : public NameContext {
  public:
  	typedef boost::intrusive_ptr<Message>	Ptr;

  	void setMessageClass(MessageClass::Ptr messageClass);
  	MessageClass::Ptr getMessageClass() const;
  	  	  
  private:
  	MessageClass::Weak	_messageClass;
};

} //namespace idl

#endif //namecontext_Message_hpp
