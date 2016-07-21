#include "LexemeValue.hpp"

namespace idl { namespace scaner {

const Context& LexemeValue::getContext() const {
	return _context;
}

Lexeme::Id LexemeValue::getId() const {
	return _id;
}
		
const String& LexemeValue::getString() const {
	return _string;
}

Float LexemeValue::getFloat() const {
	return _float;
}

Int LexemeValue::getInt() const {
	return _int;
}

LexemeValue::LexemeValue() {
}

LexemeValue::LexemeValue(const Context& context, Id id) : _context(context), _id(id) {
}

LexemeValue::LexemeValue(const Context& context, Id id, const String& value) : _context(context), _id(id), _string(value) {
}

LexemeValue::LexemeValue(const Context& context, Id id, Float value) : _context(context), _id(id), _float(value) {
}

LexemeValue::LexemeValue(const Context& context, Id id, Int value) : _context(context), _id(id), _int(value) {
}


}} // namespace idl::scaner
