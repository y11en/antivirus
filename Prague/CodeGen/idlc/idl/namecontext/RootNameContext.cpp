#include "idl/Context.hpp"
#include "RootNameContext.hpp"

#include "BasicType.hpp"
#include "Sequence.hpp"

using boost::dynamic_pointer_cast;

namespace idl {

RootNameContext::RootNameContext() {
	for (int i = 0; i < parser::BasicTypeNode::NumberOfTypes; ++i) {
		_basicTypes[i] = new BasicType((BasicType::TypeId)(i));
	}
	_sequence = new Sequence();
}

RefType::Ptr RootNameContext::addRefType(Type::Ptr type, int refCount, bool isConst) {
	if (RefType::Ptr refTypeAsArg = dynamic_pointer_cast<RefType>(type)) {
		
		type = refTypeAsArg->getType();
		refCount += refTypeAsArg->getCount();
		isConst |= refTypeAsArg->isConst();
	}

	ASSERT(!dynamic_pointer_cast<RefType>(type));

	RefKey key = RefKey(type->getWeak(), RefSubKey(refCount, isConst));
	RefMap::iterator i = _refMap.find(key);
	
	if (i != _refMap.end()) {
		return i->second;
	}
	
	RefType::Ptr refType = new RefType;
	refType->setType(type);
	refType->setCount(refCount);
	refType->setConst(isConst);
	
	_refMap.insert(RefMap::value_type(key, refType));
	return refType;
}

BasicType::Ptr RootNameContext::getBasicType(BasicType::TypeId typeId) const {
	return _basicTypes[typeId];
}

Sequence::Ptr RootNameContext::getSequence() const {
	return _sequence;
}

} //namespace idl
