#include "idl/Context.hpp"
#include "BasicType.hpp"

#include "idl/Keywords.hpp"

namespace idl {

using parser::BasicTypeNode;
using scaner::Keywords;
using scaner::Lexeme;

BasicType::BasicType(TypeId typeId) : _typeId(typeId) {
	static const Name names[] = {
		Keywords::getStr(Lexeme::Ids::S8),
		Keywords::getStr(Lexeme::Ids::S16),
		Keywords::getStr(Lexeme::Ids::S32),
		Keywords::getStr(Lexeme::Ids::S64),
		Keywords::getStr(Lexeme::Ids::U8),
		Keywords::getStr(Lexeme::Ids::U16),
		Keywords::getStr(Lexeme::Ids::U32),
		Keywords::getStr(Lexeme::Ids::U64),
		Keywords::getStr(Lexeme::Ids::Character),
		Keywords::getStr(Lexeme::Ids::Float),
		Keywords::getStr(Lexeme::Ids::Void),
	};

	setNamePath( NamePath(), names[_typeId], Name() );
}

size_t BasicType::getSize() const {
	return getSize(_typeId);
}

size_t BasicType::getSize(TypeId typeId) {
	ASSERT("depricated" && false);

	const size_t sizes[] = {
			1, 2, 4, 8,
			1, 2, 4, 8,
			4, 4, 0 };
			
	ASSERT(typeId*sizeof(size_t) < sizeof(sizes));
	return sizes[typeId];
}

BasicType::TypeId BasicType::getTypeId() const {
	return _typeId;
}

bool BasicType::isSigned(TypeId typeId) {
	switch (typeId) {
		case BasicTypeNode::SInt8 :
		case BasicTypeNode::SInt16:
		case BasicTypeNode::SInt32:
		case BasicTypeNode::SInt64:
		case BasicTypeNode::Float:
			return true;
		case BasicTypeNode::UInt8 :
		case BasicTypeNode::UInt16:
		case BasicTypeNode::UInt32:
		case BasicTypeNode::UInt64:
		case BasicTypeNode::Character:
		case BasicTypeNode::Void:
			return false;
		default:
		ASSERT(false);
	}
	return false;
}

bool BasicType::isUnsigned(TypeId typeId) {
	return !isSigned(typeId) && typeId != BasicTypeNode::Void;
}

bool BasicType::isInt(TypeId typeId) {
	switch (typeId) {
		case BasicTypeNode::SInt8 :
		case BasicTypeNode::SInt16:
		case BasicTypeNode::SInt32:
		case BasicTypeNode::SInt64:
		case BasicTypeNode::UInt8 :
		case BasicTypeNode::UInt16:
		case BasicTypeNode::UInt32:
		case BasicTypeNode::UInt64:
			ASSERT(!isFloat(typeId));
			return true;
		default:
			ASSERT(typeId == BasicTypeNode::Float || typeId == BasicTypeNode::Void);
	}
	return false;
}

bool BasicType::isFloat(TypeId typeId) {
	return typeId == BasicTypeNode::Float;
}

scaner::Int BasicType::getMask(TypeId typeId) {
	scaner::Int masks[] = {
		0x00				, // size 0
		0xff				, // size 1
		0xffff				, // size 2
		0xffffffff			, // size 4
		0xffffffffffffffffLL, // size 8
	};

	ASSERT(getSize(typeId)*sizeof(scaner::Int) < sizeof(masks));
	
	return masks[getSize(typeId)];
}

bool BasicType::isSigned() const {
	return isSigned(_typeId);
}

bool BasicType::isUnsigned() const {
	return isUnsigned(_typeId);
}

bool BasicType::isInt() const {
	return isInt(_typeId);
}

bool BasicType::isFloat() const {
	return isFloat(_typeId);
}
  	
} //namespace idl
