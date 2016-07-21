#include "Context.hpp"
#include "Generator.hpp"
#include "GlobalMapping.hpp"
#include "MappingTools.hpp"
#include "mapping/tools.hpp"

#include "namecontext/RefType.hpp"
#include "namecontext/BasicType.hpp"
#include "namecontext/RootNameContext.hpp"
#include "BasicTypeNode.hpp"
#include <boost/algorithm/string.hpp>

namespace idl {
				
using boost::format;

void includeFile(OutputContext& context, const std::string& file) {
	context._data += (format("#include \"%1%\"\n") % file).str();
}

void includeSysFile(OutputContext& context, const std::string& file) {
	context._data += (format("#include <%1%>\n") % file).str();
}

void wrapHeader( OutputContext& context, const std::string& pfx ) {
	std::string file = basename(context._file);
	
	std::string l_pfx;
	if ( !pfx.empty() )
		l_pfx = pfx + "_";
	static boost::format headerFormat(
			"#ifndef idl_%1%%2%_h\n"
			"#define idl_%1%%2%_h\n"
			"\n%3%\n"
			"#endif //idl_%1%%2%_h\n");
			
	context._data = (headerFormat % l_pfx % file.substr(0, file.find_first_of('.')) % context._data).str();
}

Generator::Generator() {
}

Generator::~Generator() {
}

/*void Generator::pushCodeBlock(const std::string& name, CodeBlock::Language lang) {
	CodeBlock::Ptr block;

	CodeBlocks::iterator i = _codeBlocks.find(name);
	if (i == _codeBlocks.end()) {
		block = CodeBlock::Ptr(new CodeBlock(name, lang));
		_codeBlocks[name] = block;
	} else {
		block = i->second;
		ASSERT(block);
		ASSERT(block->name() == name);
	}

	ASSERT(block);
	ASSERT(block->name() == name);
	
	_blockStack.push(block);
}

void Generator::popCodeBlock() {
	ASSERT(!_blockStack.empty());
	_blockStack.pop();
}

CodeBlock& Generator::currBlock() const {
	return *_blockStack.top();
}*/

void Generator::add(const CodeBlock::Name& name, CodeBlock::Language lang, std::string code) {
	block(name, lang)->add(code);
}

void Generator::indent(const CodeBlock::Name& name, CodeBlock::Language lang) {
	block(name, lang)->indent();
}

void Generator::unindent(const CodeBlock::Name& name, CodeBlock::Language lang) {
	block(name, lang)->unindent();
}

const OutputContexts& Generator::getOutputContexts() const {
	return _outputContexts;
}

CodeBlock::Ptr Generator::block(const CodeBlock::Name& name, CodeBlock::Language lang) {
	CodeBlock::Ptr codeBlock = findBlock(name, lang);
	
	if (!codeBlock) {
		codeBlock = CodeBlock::Ptr(new CodeBlock(name, lang));
		_codeBlocks.push_back(codeBlock);
	}
	
	return codeBlock;
}

CodeBlock::Ptr Generator::findBlock(const CodeBlock::Name& name, CodeBlock::Language lang) const {
	BOOST_FOREACH(CodeBlock::Ptr codeBlock, _codeBlocks) {
		if (codeBlock->name() == name && codeBlock->lang() == lang) {
			return codeBlock;
		}
	}
	return CodeBlock::Ptr();
}


int Generator::countBlocks(const std::string& pattern, CodeBlock::Language lang) const {
	int count = 0;
	BOOST_FOREACH(CodeBlock::Ptr block, _codeBlocks) {
		const std::string& bname = block->name();
		if ( boost::starts_with(bname,pattern) ) {
			CodeBlock::Language blang = block->lang();
			count += (blang == lang);
		}
	}
	return count;
}

void Generator::matchBlocks(const std::string& pattern, CodeBlock::Language lang, std::vector<std::string>& names) const {
	BOOST_FOREACH(CodeBlock::Ptr block, _codeBlocks) {
		if (boost::starts_with(block->name(), pattern) && (block->lang() == lang) != 0) {
			names.push_back(block->name().substr(pattern.size()));
		}
	}
}


// ---
void Generator::consolidate(const std::string& pattern, CodeBlock::Language lang, std::string& data) const {
	std::vector<std::string> names;
	matchBlocks(pattern, lang, names);
	
	BOOST_FOREACH(const std::string& name, names) {
		CodeBlock::Ptr cb = findBlock( pattern + name, lang );
		ASSERT( cb );
		data += "\n\n";
		//data += "\n// cb.beg(" + cb->langName() + ") \"" + cb->name() + "\"\n\n";
		data += cb->data();
		//data += "\n// cb.end(" + cb->langName() + ") \"" + cb->name() + "\"\n\n";
		data += "\n\n";
	}
}


// ---
void Generator::gen(const std::string& basename) {
	// collect code blocks into files
	
	// header file (Ah) <idl-name>.h
	//		--> std headers
	//		1- declaration.interface
	//		2- declaration.plugin.*
	//	1+2 - wrapped as a header
	//		3- declaration.pluginimpex.*
	
	// source file (Ac) <idl-name>.c
	//		--> #define IMPEX_EXPORT_LIB
	//		--> #include "Ah"
	//		--> #define IMPEX_TABLE_CONTENT
	//		--> #include "Ah"
	//		1- definition.plugin.*
	//		

	// header file (Bh) <impl-name>.h
	//		--> #include "Ah"
	//		1- declaration.implementation.*
	//		--> wrapped as a header
	
	// source file (Bc) <impl-name>.c
	//		--> #include "Bh"
	//		--> #include <iface/i_root.h>                  	
	//		1- definition.implementation.*

	std::vector<std::string> names;
	
	int howManyCppPlagins = countBlocks( "declaration.plugin.", CodeBlock::CppLang );
	ASSERT(howManyCppPlagins <= 1);
	
	int commonPlugins = countBlocks( "declaration.plugin.", CodeBlock::CommonLang );
	int commonImpex = countBlocks( "declaration.pluginimpex.", CodeBlock::CommonLang );

	std::string header;
	OutputContext::ftype ftype;
	if ( commonImpex || commonPlugins ) {
		header = "p_";
		ftype = OutputContext::plugin;
	}
	else {
		header = "i_";
		ftype = OutputContext::iface;
	}
	header += basename + ".h";

	OutputContext ifaceH( ftype, header );

	ifaceH._data += PRAGUE_STD_HEADERS;
	consolidate("declaration.interface", CodeBlock::CommonLang, ifaceH._data);

	int cIfaceC   = countBlocks( "declaration.interface", CodeBlock::CLang );
	int cIfaceCpp	= countBlocks( "declaration.interface", CodeBlock::CppLang );
	if ( cIfaceCpp ) {
		ifaceH._data += "#if defined(__cplusplus) && !defined(_USE_VTBL)\n\n";
		consolidate("declaration.interface", CodeBlock::CppLang, ifaceH._data);
	}
	if ( cIfaceC ) {
		if ( cIfaceCpp )
			ifaceH._data += "#else  // !defined(__cplusplus) || defined(_USE_VTBL)\n\n";
		else
			ifaceH._data += "#if !defined(__cplusplus) || defined(_USE_VTBL)\n\n";
		consolidate("declaration.interface", CodeBlock::CLang, ifaceH._data);
	}
	if ( cIfaceC || cIfaceCpp )
		ifaceH._data += "#endif  // __cplusplus & _USE_VTBL\n\n";

	consolidate("declaration.plugin.", CodeBlock::CommonLang, ifaceH._data);
	wrapHeader( ifaceH, std::string("") );
	ifaceH._data += "\n\n\n";
	consolidate("declaration.pluginimpex.", CodeBlock::CommonLang, ifaceH._data);
	_outputContexts.push_back(ifaceH);
	
	names.clear();
	matchBlocks("declaration.plugin.", CodeBlock::CommonLang, names);
	if (!names.empty()) {
		OutputContext ifaceC( OutputContext::src, basename + ".c" );
		
		ifaceC._data += "#define IMPEX_EXPORT_LIB\n";
		includeFile( ifaceC, ifaceH._file );
		
		ifaceC._data += "#define IMPEX_TABLE_CONTENT\n";
		ifaceC._data += "EXPORT_TABLE( export_table )\n";
		includeFile( ifaceC, ifaceH._file );
		ifaceC._data += "EXPORT_TABLE_END\n";
		
		includeSysFile( ifaceC, "Prague/iface/i_root.h" );
		consolidate("definition.plugin.", CodeBlock::CommonLang, ifaceC._data);
		
		_outputContexts.push_back(ifaceC);

		names.clear();	
		matchBlocks("declaration.implementation.", CodeBlock::CLang, names);
		
		//BOOST_FOREACH(const std::string& name, names) {
		std::vector<std::string>::iterator iname = names.begin();
		for( ; iname != names.end(); ++iname ) {
			const std::string& name = *iname;
			OutputContext implH( OutputContext::src, name + ".h" );
			includeFile( implH, ifaceH._file );
			consolidate("declaration.implementation." + name, CodeBlock::CommonLang, implH._data);
			//implH._data += "#if !defined(__cplusplus) || defined(_USE_VTBL)\n";
			//consolidate("declaration.implementation." + name, CodeBlock::CLang, implH._data);
			//implH._data += "#else\n";
			consolidate("declaration.implementation." + name, CodeBlock::CppLang, implH._data);
			//implH._data += "#endif\n";
			wrapHeader( implH, std::string("impl") );
			_outputContexts.push_back(implH);
			
			/*
			OutputContext implC( OutputContext::src, name + ".c" );
			includeFile(implC, implH._file);
			includeSysFile(implC, "Prague/iface/i_root.h");
			consolidate("definition.implementation." + name, CodeBlock::CLang, implC._data);
			_outputContexts.push_back(implC);
			*/
			
			OutputContext implCpp( OutputContext::src, name + ".cpp" );
			includeFile(implCpp, implH._file);
			includeSysFile(implCpp, "Prague/iface/i_root.h");
			consolidate("definition.implementation." + name, CodeBlock::CppLang, implCpp._data);
			_outputContexts.push_back(implCpp);
		}
	}
}

} //namespace idl

