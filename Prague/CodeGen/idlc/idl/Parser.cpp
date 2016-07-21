#include "Parser.hpp"

#include <iostream>

#include "Context.hpp"
#include "Scaner.hpp"
#include "Exception.hpp"
#include "Builder.hpp"

extern int yyparse (idl::parser::Parser& parser, idl::parser::Builder& builder, idl::scaner::Scaner& scaner);

namespace idl { namespace parser {

Parser::Parser() {
}

NodeInterface::Ptr Parser::parse(boost::shared_ptr<idl::scaner::Scaner> scaner) {
	_scaner = scaner;
	
	Builder builder;

	#if defined(YYDEBUG)
		extern int yydebug;
		yydebug = 1;	
	#endif

	int result = yyparse(*this, builder, *_scaner);
	_scaner.reset();
	
	return result == 0 ? builder.getRootNode() : NodeInterface::Ptr();
}

NodeInterface::Ptr Parser::parse(const std::string& file) {
	boost::shared_ptr<idl::scaner::Scaner> scaner = boost::shared_ptr<idl::scaner::Scaner>(new idl::scaner::Scaner());
	scaner->setSource(file);
	return parse(scaner);
}

NodeInterface::Ptr Parser::parse(const std::string& name, boost::shared_ptr<std::istream> stream) {
	boost::shared_ptr<idl::scaner::Scaner> scaner = boost::shared_ptr<idl::scaner::Scaner>(new idl::scaner::Scaner());
	scaner->setSource(name, stream);
	return parse(scaner);
}

void Parser::error(const Context& , const std::string& ) {
	//std::cerr << context << ": " << message << std::endl;
}


}} //namespace idl::parser
