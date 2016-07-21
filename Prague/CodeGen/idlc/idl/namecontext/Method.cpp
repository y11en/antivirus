#include "Context.hpp"

#include "Method.hpp"

namespace idl {

Method::Method() : _varArg(false) {
}

bool Method::isVarArg() const {
	return _varArg;
}

void Method::setVarArg() {
	ASSERT(!_varArg);
	_varArg = true;
}

void Method::addChild(NameContext::Ptr child) {
	ASSERT(child);

	NameContext::addChild(child);

	if (Value::Ptr value = boost::dynamic_pointer_cast<Value>(child)) {
		addArg(value);
	}
}

void Method::addArg(Value::Ptr value) {
	ASSERT(value);
	_args.push_back(value->getWeak());
}

const Method::Args& Method::getArgs() const {
	return _args;
}

void Method::setReturnType(Type::Ptr type) {
	ASSERT(type);
	_returnType = type->getWeak();
	ASSERT(_returnType.lock().get());
}

Type::Ptr Method::getReturnType() const {
	ASSERT(_returnType.lock().get());
	return _returnType.lock().get();
}


} //namespace idl

