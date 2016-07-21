#include "Context.hpp"

#include "Sequence.hpp"

namespace idl {

size_t SequenceType::getNumberOfItems() const {
	return _size;
}

void SequenceType::setType(Type::Ptr type, size_t size) {
	NamePath path;
	path.push_back("sequence");
	//path.insert(path.end(), type->getNamePath().begin(), type->getNamePath().end());
	setNamePath( path, type->getNamePath().str(), Name() );

	Type::setType(type);	
	//_type = type->getWeak();
	_size = size;
}

//Type::Ptr SequenceType::getType() const {
//	return _type.lock().get();
//}

size_t SequenceType::getSize() const {
	const size_t headerSize = 4; /// where the actual sequence length saved

	return headerSize + _size*getType()->getSize();
}

/*const Struct::Entries& Struct::getEntries() const {
	return _entries;
}*/

} //namespace idl
