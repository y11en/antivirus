#include "Context.hpp"

#include "Typedef.hpp"
#include "ExternalType.hpp"
#include "RefType.hpp"
#include "RootNameContext.hpp"
#include "ErrorFactory.hpp"

namespace idl {

static bool validType(Type::Ptr type, bool check) {
	if (check) {
		return type != NULL;
	} else {
		if (type) {
			return true;
		} else {
			ASSERT(!type);
			// only error in name resolving may cause null types
			return !ErrorFactory::instance().getErrors().empty();
		}
	}
}

size_t Typedef::getSize() const {
	return getType()->getSize();
}

Type::Ptr reduceTypedef( Type::Ptr type, bool& isRef, bool& isConst, int& count, bool check ) {
	
	ASSERT( validType(type,check) );
	
	Typedef::Ptr typedefType = boost::dynamic_pointer_cast<Typedef>(type);
	if ( typedefType )
		return reduceTypedef(typedefType->getType(), isRef, isConst, count, check);

	ExternalType::Ptr externalType = boost::dynamic_pointer_cast<ExternalType>(type);
	if ( externalType ) {
		Type::Ptr ext = externalType->getType();
		if ( ext )
			return reduceTypedef( ext, isRef, isConst, count, check );
		return type;
	}

	RefType::Ptr refType = boost::dynamic_pointer_cast<RefType>(type);
	if ( refType ) {
		isRef = true;
		isConst |= refType->isConst();
		count += refType->getCount();
		Type::Ptr rtype = refType->getType();
		return reduceTypedef( rtype, isRef, isConst, count, check );
	}
	
	return type;
}

Type::Ptr reduceTypedef(RootNameContext::Ptr root, Type::Ptr type, bool check) {
	ASSERT(validType(type, check));
#if 1
	bool isRef = false, isConst = false;
	int count = 0;

	Type::Ptr reduced = reduceTypedef(type, isRef, isConst, count, check);
	
	ASSERT(!boost::dynamic_pointer_cast<Typedef>(reduced));
	ASSERT(!boost::dynamic_pointer_cast<RefType>(reduced));
	
	if (isRef) {
		return root->addRefType(reduced, count, isConst);
	} else {
		ASSERT(!isConst && count == 0);
		return reduced;
	}
#else
	Typedef::Ptr typedefType = boost::dynamic_pointer_cast<Typedef>(type);
	if (typedefType) {
		return typedefType->reduce();
	}
	return type;

#endif
}
		
} //namespace idl
		
