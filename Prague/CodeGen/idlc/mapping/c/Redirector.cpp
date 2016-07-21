#include "Context.hpp"

#include "Redirector.hpp"
#include "Generator.hpp"
#include "MappingTools.hpp"

namespace idl {

Redirector::Redirector(Generator& generator) : _generator(generator) { 
}

void Redirector::setBlock(const CodeBlock::Name& blockName, CodeBlock::Language lang) {
	_blockName = blockName;
	_lang = lang;
}

/*void Redirector::pushCodeBlock(const std::string& name, CodeBlock::Language lang) {
	_generator.pushCodeBlock(name, lang);
}

void Redirector::popCodeBlock() {
	_generator.popCodeBlock();
}*/

void Redirector::add(const std::string& data) {
	ASSERT(!_blockName.empty());
	_generator.add(_blockName, _lang, data);
}

void Redirector::add(const boost::format& fmt) {
	add(fmt.str());
}

void Redirector::indent() {
	_generator.indent(_blockName, _lang);
}

void Redirector::unindent() {
	_generator.unindent(_blockName, _lang);
}

} // namespace idl
