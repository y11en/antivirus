#include "Context.hpp"
#include "GlobalMapping.hpp"
#include "MappingTools.hpp"

using boost::format;

namespace idl {

GlobalMapping::GlobalMapping(Generator& generator) : Redirector(generator), _cMapping(generator), _cppMapping(generator) {
	_cMapping.setBlock("declaration.interface", CodeBlock::CLang);
	_cppMapping.setBlock("declaration.interface", CodeBlock::CppLang);
	setBlock("declaration.interface", CodeBlock::CommonLang);
}

void GlobalMapping::start() {
	//pushCodeBlock("declaration.interface", CodeBlock::CLang);
}

void GlobalMapping::stop() {
	//popCodeBlock();
}

void GlobalMapping::constDecl(const Attributes& attrs, ConstValue::Ptr context) {
	_cMapping.constDecl(attrs, context);
	_cppMapping.constDecl(attrs, context);
}

void GlobalMapping::typedefDecl(const Attributes& attrs, Typedef::Ptr context) {
	_cMapping.typedefDecl(attrs, context);
	_cppMapping.typedefDecl(attrs, context);
}

void GlobalMapping::structDecl(const Attributes& attrs, Struct::Ptr context) {
	_cMapping.indent();
	_cMapping.structDecl(attrs, context);
	_cMapping.unindent();
	_cppMapping.indent();
	_cppMapping.structDecl(attrs, context);
	_cppMapping.unindent();
}

void GlobalMapping::enumDecl(const Attributes& attrs, Enum::Ptr context) {
	_cMapping.enumDecl(attrs, context);
	_cppMapping.enumDecl(attrs, context);
}

void GlobalMapping::forwardIFace( NameContext::Ptr context ) {
	_cMapping.forwardIFace(context);
	_cppMapping.forwardIFace(context);
}

void GlobalMapping::forwardStruct( NameContext::Ptr context ) {
	_cMapping.forwardStruct(context);
	_cppMapping.forwardStruct(context);
}

void GlobalMapping::typecast( NameContext::Ptr context ) {
	_cMapping.typecast(context);
	_cppMapping.typecast(context);
}

void GlobalMapping::externalTypeDecl(ExternalType::Ptr context) {
	format fmt("#include \"%1%\"\n");
	add(fmt % context->getFile());
}
	
void GlobalMapping::importFile(const std::string& file) {
	format fmt("#include \"%1%\"\n");
	std::string fname = makeInterfaceHeaderFile( file );
	add( fmt % fname );
}

} //namespace idl
