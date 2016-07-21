#include "Context.hpp"

#include "Sequence.hpp"

namespace idl {

Sequence::Sequence() {
	setNamePath( NamePath(), "sequence", Name() );
}

SequenceType::Ptr Sequence::add(Type::Ptr type, size_t size) {
	Key key(type->getWeak(), size);

	Params::iterator i = _params.find(key);

	if (i == _params.end()) {
		SequenceType::Ptr sequenceType = new SequenceType();
		sequenceType->setType(type, size);
		_params.insert(Params::value_type(key, sequenceType));
		return sequenceType;
	} else {
		return i->second;
	}
}


size_t Sequence::getSize() const {
	ASSERT(false);
	return 0;
}

/*const Struct::Entries& Struct::getEntries() const {
	return _entries;
}*/

} //namespace idl
