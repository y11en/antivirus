#include "Lexeme.hpp"
#include "LexemeValue.hpp"

#include "assert.hpp"

namespace idl { namespace scaner {

Lexeme::Lexeme() {
	//_value = boost::shared_ptr<Lexeme::Value>(new Lexeme::Value);
}

const Lexeme::Value& Lexeme::getValue() const {
	ASSERT(_value);
	return *_value;
}

Lexeme::Id Lexeme::getId() const {
	ASSERT(_value);
	return _value->getId();
}

const Context& Lexeme::getContext() const {
	ASSERT(_value);
	return _value->getContext();
}

void Lexeme::make(const Context& context, Id id) {
	_value = boost::shared_ptr<Lexeme::Value>(new Lexeme::Value(context, id));
}

void Lexeme::makeString(const Context& context, Id id, const String& value) {
	_value = boost::shared_ptr<Lexeme::Value>(new Lexeme::Value(context, id, value));
}

void Lexeme::makeFloat(const Context& context, Id id, Float value) {
	_value = boost::shared_ptr<Lexeme::Value>(new Lexeme::Value(context, id, value));
}

void Lexeme::makeInt(const Context& context, Id id, Int value) {
	_value = boost::shared_ptr<Lexeme::Value>(new Lexeme::Value(context, id, value));
}

}} // namespace idl::scaner
