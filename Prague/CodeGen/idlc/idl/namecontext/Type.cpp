#include "idl/Context.hpp"
#include "Type.hpp"

namespace idl {

#pragma warning(suppress : 4355)
Type::Type() : _this(this, null_deleter()) {
}

/*size_t Type::getSize() const {
	return ReferenceSize;
}*/

Type::Weak Type::getWeak() {
	return _this;
}

void Type::setType(Type::Ptr type) {
	_type = type->getWeak();
}

Type::Ptr Type::getType() const {
	return _type.lock().get();
}

NameContext::Ptr Type::find( const Name& name, const Name& alt ) const {
	NameContext::Ptr result = NameContext::find( name, alt );
	
	if ( !result && _type.lock() ) {
		result = _type.lock()->find( name, alt );
	}
	
	return result;
}

} //namespace idl
