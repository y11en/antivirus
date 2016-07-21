#include "Context.hpp"
#include "MessageClass.hpp"

namespace idl {

#pragma warning(suppress : 4355)
MessageClass::MessageClass() : _this(this, null_deleter()) {
}

MessageClass::Weak MessageClass::getWeak() {
	return _this;
}

} //namespace idl
