#include "Context.hpp"
#include "PluginMapping.hpp"
#include "CommonMapping.hpp"
#include "MappingTools.hpp"

#include "mapping/tools.hpp"
#include <boost/format.hpp>

namespace idl {

using namespace boost::io;
using boost::format;

PluginMapping::PluginMapping(Generator& generator) : Redirector(generator) {
}

Component::Ptr PluginMapping::getPluginContext() const {
	return _pluginContext;
}

void PluginMapping::externMethodDecl(const Attributes& attrs, ExternMethod::Ptr context) {
	_externMethods.push_back(ExternMethods::value_type(attrs, context));
	//_methods.push_back(Methods::value_type(attrs, context));
}

//std::string PluginMapping::genPluginFileName(const Attributes& /*attrs*/, Component::Ptr context) {
//	return to_lower_copy(context->getName());
//}

void PluginMapping::componentOpen(const Attributes& /*attrs*/, Component::Ptr context) {
	ASSERT(!_pluginContext);
	_pluginContext = context;
	
	_externMethods.clear();
	
	//pushCodeBlock("declaration.plugin." + context->getNamePath().str(), CodeBlock::CommonLang);
	setBlock("declaration.plugin." + context->getNamePath().str(), CodeBlock::CommonLang);
	
	add(format("//\tplugin %1% definition\n") % CommonMapping::cName(context));
	indent();
	
}

std::string genMethodArgsDeclaration(Method::Ptr method, const std::string& thisArg = "") {
	std::string args = thisArg;

	BOOST_FOREACH(Value::Weak arg, method->getArgs()) {
		extendArgsList(args, CommonMapping::cName(arg.lock()->getType()) + " "  + arg.lock()->getRealName());
	}
	
	if (method->isVarArg()) {
		extendArgsList(args, "...");
	}
	
	return args;
}

void PluginMapping::componentClose( const Attributes& attrs, Component::Ptr context ) {
	static format macroFmt("#define %1%_ID %2%\n");
	add(macroFmt % CommonMapping::macroName(context) % genId(attrs));
			
	std::string pluginMnemonic = CommonMapping::cName(context);
	pluginMnemonic += "_PLUGINID";

	static format idFmt("static const PluginId %1% = %2%_ID;\n");
	add(idFmt % pluginMnemonic % CommonMapping::macroName(context));
		
	format versionFmt("static const tVERSION %1%_Version = %2%;\n");
	add(versionFmt % CommonMapping::cName(context) % genVersion(attrs, context));
	//add(genVersion(attrs, context));
		
	const ExternMethods& methods = _externMethods;
	
	BOOST_FOREACH(const ExternMethods::value_type& method, methods) {
		static format line("#define %1%_ID %2%\n");
		
		add(line % CommonMapping::macroName(method.second) % genId(method.first));
	}
	
	unindent();
	add(format("//\tend of plugin %1% definition\n\n") % CommonMapping::cName(context));
	
	//popCodeBlock();
	//pushCodeBlock("declaration.pluginimpex." + context->getNamePath().str(), CodeBlock::CommonLang);
	setBlock("declaration.pluginimpex." + context->getNamePath().str(), CodeBlock::CommonLang);
	
	add( "#ifdef IMPEX_TABLE_CONTENT\n\n\n" );
	indent();
	
	BOOST_FOREACH(const ExternMethods::value_type& method, methods) {
		static format line("{ (tDATA)&%1%, %2%_ID, %3% },\n");
		
		add(line % method.second->getRealName() % CommonMapping::macroName(context) % genId(method.first));
	}
	
	unindent();

	add("\n\n#elif defined(IMPEX_EXPORT_LIB) || defined(IMPEX_IMPORT_LIB)\n\n\n");

	indent();
	add("#include <iface/impexhlp.h>\n\n\n");

	BOOST_FOREACH(const ExternMethods::value_type& method, methods) {
		static format line("IMPEX_DECL %1% IMPEX_FUNC(%2%)( %3% )IMPEX_INIT;\n");
		
		add(line 
				% CommonMapping::cName(method.second->getReturnType()) 
				% method.second->getRealName() 
				% genMethodArgsDeclaration(method.second));
	}
	unindent();
	add( "\n\n#endif\n" );
	
	//popCodeBlock();
	//pushCodeBlock("definition.plugin." + context->getNamePath().str(), CodeBlock::CommonLang);
	setBlock( "definition.plugin." + context->getNamePath().str(), CodeBlock::CommonLang );
	
	format srcFmt(
		/*"#include <prague.h>					\n"
		"#include <iface/i_root.h>              \n"
		"                                       \n"
		"#define  IMPEX_EXPORT_LIB              \n"
		"#include <%1%.h>       				\n"
		"                                       \n"
		"#define  IMPEX_TABLE_CONTENT           \n"
		"EXPORT_TABLE( export_table )           \n"
		"#include <%1%.h>       				\n"
		"EXPORT_TABLE_END                       \n"*/
		"                                       \n"
		"PR_MAKE_TRACE_FUNC;                    \n"
	);
	
	add( srcFmt /*% genPluginFileName(attrs, context)*/ );
	
	ASSERT(_pluginContext);
	_pluginContext = NULL;
	
	//popCodeBlock();
}

} //namespace idl
