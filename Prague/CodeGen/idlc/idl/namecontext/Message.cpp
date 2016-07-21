#include "Context.hpp"
#include "Message.hpp"

namespace idl {

void Message::setMessageClass(MessageClass::Ptr messageClass) {
	ASSERT(messageClass);
	_messageClass = messageClass->getWeak();
}

MessageClass::Ptr Message::getMessageClass() const {
	return _messageClass.lock().get();
}
  	  	  
} //namespace idl
