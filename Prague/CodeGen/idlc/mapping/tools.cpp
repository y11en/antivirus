#include "Context.hpp"

#include "tools.hpp"
#include "Collector.hpp"

#include "Collector.hpp"
#include "ErrorFactory.hpp"
#include "ValueFactory.hpp"
#include "Lexeme.hpp"
#include "Scaner.hpp"
#include "Parser.hpp"
#include "ErrorNode.hpp"

#include "namecontext/GatherDeclaredNames.hpp"
#include "namecontext/NameResolver.hpp"
#include "namecontext/CheckAttrs.hpp"

#include <fstream>
//#include <iostream>
//#include <boost/format.hpp>

namespace idl {

using namespace parser;

//using boost::format;

RootNameContext::Ptr theTypeRoot;


// ---
NodeInterface::Ptr compileIDL(const std::string& idlFile, const std::vector<std::string>& incPaths, bool disableDefaultImports, Collector& collector) {
	ErrorFactory::instance().clear();

	NodeInterface::Ptr root;
	
	try {
		boost::shared_ptr<idl::scaner::Scaner> scaner = boost::shared_ptr<idl::scaner::Scaner>(new idl::scaner::Scaner());
		if (!disableDefaultImports) {
			scaner->addForceImport("stdidl.idl");
		}
		scaner->setSource(idlFile);
		scaner->setIncludePaths(incPaths);
			
		Parser parser;
		root = parser.parse(scaner);
		
		if (root) {
			theTypeRoot = gatherDeclaredNames(root);
			resolveNames(theTypeRoot, root);
		}

		if (ErrorFactory::instance().getErrors().empty()) {
			idl::checkAttrs(theTypeRoot, root);
		}
	} catch(Exception&) {
	}
	
	if ( !ErrorFactory::out() )
		return 0;

	collect( root, collector );
	return root;
}

} //namespace idl
