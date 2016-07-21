#include "Context.hpp"

#include "Struct.hpp"

namespace idl {

void Struct::addChild(NameContext::Ptr child) {
	Type::addChild(child);

	if (Value::Ptr value = boost::dynamic_pointer_cast<Value>(child)) {
		addEntry(value);
	}
}

size_t Struct::getSize() const {
	size_t size = 0;

	Entries::const_iterator i = _entries.begin(), end = _entries.end();
	for (; i != end; ++i) {
		size += i->lock().get()->getType()->getSize();
	}

	return size;
}

void Struct::addEntry(Value::Ptr value) {
	_entries.push_back(value->getWeak());
}

const Struct::Entries& Struct::getEntries() const {
	return _entries;
}

} //namespace idl
