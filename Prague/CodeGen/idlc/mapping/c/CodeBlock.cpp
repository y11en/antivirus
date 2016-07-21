#include "Context.hpp"

#include "CodeBlock.hpp"

#include "MappingTools.hpp"

namespace idl {

CodeBlock::CodeBlock(const std::string& name, Language lang) : _name(name), _lang(lang) {
}

CodeBlock::Language CodeBlock::lang() const {
	return _lang;
}

const std::string& CodeBlock::name() const {
	return _name;
}

void CodeBlock::add(const std::string& code) {
	_data += shift(code, _indent);
}

const std::string&	CodeBlock::data() const {
	return _data;
}

void CodeBlock::indent() {
	_indent += '\t';
}

void CodeBlock::unindent() {
	ASSERT(!_indent.empty());
	_indent = _indent.substr(0, _indent.length() - 1);
}

} //namespace idl
