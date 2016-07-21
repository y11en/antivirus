#include "Context.hpp"
#include "Enum.hpp"

namespace idl {

size_t Enum::getSize() const {
	return 4;
}

void Enum::addChild(NameContext::Ptr child) {
	Type::addChild(child);

	if (Value::Ptr value = boost::dynamic_pointer_cast<Value>(child)) {
		addEntry(value);
	}
}

void Enum::addEntry(Value::Ptr value) {
	_entries.push_back(value->getWeak());
}

const Enum::Entries& Enum::getEntries() const {
	return _entries;
}

} //namespace idl
