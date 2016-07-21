#include "Context.hpp"
#include "RefType.hpp"

namespace idl {

size_t RefType::getSize() const {
	ASSERT(false && "not yet supported");
	return 0;
}

void RefType::setCount(int count) {
	_count = count;
}

void RefType::setConst(bool isConst) {
	_const = isConst;
}

int RefType::getCount() const {
	return _count;
}

bool RefType::isConst() const {
	return _const;
}

} //namespace idl

