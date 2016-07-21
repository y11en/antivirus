#include "idl/Context.hpp"
#include "Value.hpp"

namespace idl {

#pragma warning(suppress : 4355)
Value::Value() : _this(this, null_deleter()) {
}

Value::Weak Value::getWeak() {
	return _this;
}

void Value::setType(Type::Ptr type) {
	_type = type->getWeak();
}

Type::Ptr Value::getType() const {
	return _type.lock().get();
}

NameContext::Ptr Value::find( const Name& name, const Name& alt ) const {
	return _type.lock()->find( name, alt );
}

} //namespace idl
