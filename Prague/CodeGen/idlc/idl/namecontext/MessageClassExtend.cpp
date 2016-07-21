#include "Context.hpp"
#include "MessageClassExtend.hpp"

namespace idl {

void MessageClassExtend::setMessageClass(MessageClass::Ptr messageClass) {
	ASSERT(messageClass);
	ASSERT(getCoreName() == messageClass->getCoreName());
	_messageClass = messageClass->getWeak();
}

MessageClass::Ptr MessageClassExtend::getMessageClass() const {
	return _messageClass.lock().get();
}
  	  	  
} //namespace idl
