#include "Context.hpp"
#include "InterfaceMappingCpp.hpp"
#include "Generator.hpp"
#include "MappingTools.hpp"

#include "namecontext/RefType.hpp"
#include "namecontext/BasicType.hpp"
#include "namecontext/RootNameContext.hpp"

#include "mapping/tools.hpp"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace idl {

using parser::AttributeNode;
using parser::BasicTypeNode;

using boost::dynamic_pointer_cast;
using namespace boost::io;
using boost::format;

InterfaceMappingCpp::InterfaceMappingCpp(Generator& generator) : CommonMappingCpp(generator) {
	setBlock("declaration.interface", CodeBlock::CppLang);
}

InterfaceMappingCpp::~InterfaceMappingCpp() {
}


// ---
std::string InterfaceMappingCpp::genInterfaceDecl( const Attributes& attrs, Interface::Ptr context ) const {
	static format ifaceDeclFmt(
		"struct pr_abstract %1%%2% {\n"
		"	static const InterfaceId IID = %3%;\n"
	);

	std::string baseIface;
	if (!context->getBases().empty()) {
		ASSERT(context->getBases().size() == 1 && "multiple inheritance not supported");
		baseIface = " : public " + context->getBases().front()->getRealName();
	}
		
	std::string iface = context->getRealName();
	std::string decl = ( 
		ifaceDeclFmt 
			% iface
			% baseIface 
			% genId(attrs)).str();
	return decl;
}



// ---
std::string InterfaceMappingCpp::genIface2ImplDecl( const Attributes& attrs, Interface::Ptr context, std::string& impl ) const {
	format ifaceDeclFmt(
		"struct pr_abstract %1% %2% {\n"
		"	static const PluginId         PID    = %4%_PLUGINID;\n"
		"	static const ImplementationId ImplID = %3%;\n"
		"	static const VendorId         VID    = %3%;\n"
	);

	std::string baseIface;
	if (!context->getBases().empty()) {
		ASSERT(context->getBases().size() == 1 && "multiple inheritance not supported");
		baseIface = ": public " + context->getBases().front()->getRealName();
	}
		
	return (ifaceDeclFmt 
		% context->getRealName() 
		% baseIface 
		% genId(attrs)
		% impl).str();
}



void InterfaceMappingCpp::interfaceOpen(const Attributes& attrs, Interface::Ptr context) {
	add(format("\n\n//\tinterface %1% definition\n") % fullName(context));
	
	//_methods.clear();
	_errors.clear();

	add(genInterfaceDecl(attrs, context));
	indent();
}


static format propOrdAcc(
	"\n"
	"%1% pr_call get_%2%() { return get%3%(IDLN(%2%)); }\n"
	"tERROR pr_call set_%2%( %1% value ) { return set%3%(IDLN(%2%),value); }\n"
);
static format propStrAcc(
	"\n"
	"tERROR pr_call get_%1%( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(IDLN(%1%),buff,size,cp); }\n"
	"tERROR pr_call set_%1%( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(IDLN(%1%),buff,size,cp); }\n"
);
static format propBuffAcc(
	"\n"
	"tERROR pr_call get_%1%( tPTR buff, tDWORD size ) { return get(IDLN(%1%),buff,size); }\n"
	"tERROR pr_call set_%1%( tPTR buff, tDWORD size ) { return set(IDLN(%1%),buff,size); }\n"
);


// ---
static format& get_format( const std::string& type, unsigned& args, std::string& acc_type ) {
	if ( (type == "tSTRING") || (type == "tWSTRING") ) {
		args = 1, acc_type = "Str";
		return propStrAcc;
	}
	if ( type == "tBOOL" ) {
		args = 3, acc_type = "Bool";
		return propOrdAcc;
	}
	if ( (type == "tBYTE") || (type == "tCHAR") ) {
		args = 3, acc_type = "Byte";
		return propOrdAcc;
	}
	if ( type == "tWORD" ) {
		args = 3, acc_type = "Word";
		return propOrdAcc;
	}
	if ( type == "tUINT" ) {
		args = 3, acc_type = "UInt";
		return propOrdAcc;
	}
	if ( type == "tDWORD" ) {
		args = 3, acc_type = "DWord";
		return propOrdAcc;
	}
	if ( type == "tQWORD" ) {
		args = 3, acc_type = "QWord";
		return propOrdAcc;
	}
	if ( type == "hOBJECT" ) {
		args = 3, acc_type = "Obj";
		return propOrdAcc;
	}
	if ( type == "tPTR" ) {
		args = 3, acc_type = "Ptr";
		return propOrdAcc;
	}
	args = 1;
	acc_type.clear();
	return propBuffAcc;
}


// ---
std::string InterfaceMappingCpp::genPropAccessors() const {
	std::string res;
	for( unsigned i = 0; i < _ifaceProps.size(); ++i ) {
		std::string	  acc, str;
		unsigned      args = 0;
		Property::Ptr prop = _ifaceProps[i];
		idl::Name     tname = prop->getType()->getRealName();
		idl::Name     pname = prop->getRealName();
		format&       fmt = get_format( tname, args, acc );
		if ( args == 1 ) 
			str = (fmt % pname).str();
		else
			str = (fmt % tname % pname % acc).str();
		res += str;
	}
	return res;
}

void InterfaceMappingCpp::interfaceClose(const Attributes& attrs, Interface::Ptr ctx) {
	unindent();
	
	static format ifaceDeclFmt(
		"%1%"
		"%2%"
		"};\n\n\n"
		"#if !defined(PR_IDL_LEGACY)\n\n"
		"%5%\n\n"
		"#endif // !defined(PR_IDL_LEGACY)\n\n\n"
		"struct pr_abstract %3% : public %4%, public iObj {\n\n"
		"	OBJ_IMPL( %3% );\n"
		"%6%\n"
		"};\n\n"
		"%7%" // typedef if any
	);

	std::string typedef_str;
	std::string short_name    = shortName(ctx);
	std::string name          = ctx->getRealName();
	std::string errors        = genErrors( true, genId(attrs) );
	std::string methods       = genInterfaceMethods( ctx );
	std::string propAccessors = genPropAccessors();
	std::string method_calls  = genInterfaceMethodsCalls( ctx );

	_ifaceProps.clear();

	Attributes::const_iterator i = attrs.begin();
	for( ; i != attrs.end(); ++i ) {
		AttributeNode::Ptr attr = *i;
		AttributeNode::AttributeId id = attr->getId();
		if ( id == AttributeNode::C_Type_Name ) {
			//"#if defined(PR_IDL_TYPEDEF)\n"
			//"	typedef cIfaceName* hIFACENAME;\n"
			//"#endif\n\n"
			typedef_str =
				"#if defined(PR_IDL_TYPEDEF)\n"
				"	typedef " + short_name + "* " + attr->getString() +
				";\n"
				"#endif\n\n";
			break;
		}
	}

	add( ifaceDeclFmt 
		% shift(errors)
		% shift(methods)
		% short_name
		% name
		% shift(method_calls)
		% shift(propAccessors)
		% typedef_str
	);
	
	add( format("//\tend of interface %1% definition\n\n") % fullName(ctx) );
}

// ---
void InterfaceMappingCpp::constDecl(const Attributes& attrs, ConstValue::Ptr context) {
	static format constFormat("static const %1% IDLN(%2%) = %3%;\n");
	std::string cnst = 
		(constFormat 
			% fullName(context->getType()) 
			% shortName(context) 
			% genConstValue(context->getPerformer())).str();
	add( cnst );
}

void InterfaceMappingCpp::methodDecl(const Attributes& /*attrs*/, Method::Ptr /*context*/) {
	//_methods.push_back(Methods::value_type(attrs, context));
}

void InterfaceMappingCpp::propertyDecl(const Attributes& attrs, Property::Ptr context) {
	_ifaceProps.push_back( context );
	add( genPropertyDecl(attrs,context,false) );
}

void InterfaceMappingCpp::messageDecl(const Attributes& attrs, Message::Ptr context) {
	format fmt("static const MessageId %1%_%2% = %3%;\n");
	fmt = fmt 
		% shortName(context->getMessageClass()) 
		% shortName(context) 
		% genId(attrs);
	add(fmt);
}

void InterfaceMappingCpp::messageClassDecl(const Attributes& attrs, MessageClass::Ptr context) {
	format fmt("static const MessageClassId IDLN(%1%) = %2%;\n");
	fmt = fmt 
		% shortName(context) 
		% genId(attrs);
	add(fmt);
}

void InterfaceMappingCpp::messageClassExtendDecl(const Attributes& /*attrs*/, MessageClassExtend::Ptr /*context*/) {
}

void InterfaceMappingCpp::errorDecl(const Attributes& attrs, NameContext::Ptr context) {
	_errors.push_back(Errors::value_type(attrs, context));
}


std::string InterfaceMappingCpp::genInterfaceMethodCall(Interface::Ptr interface, Method::Ptr method) const {
	static format callRecFmt("#define CALL_%1%(%2%)\t((_this)->%3%(%4%))\n");
				
	std::string argsCallLeft = genMethodArgsCall(method, "_this");
	std::string argsCallRight = genMethodArgsCall(method);
	
	return (callRecFmt % methodCName(interface, method) % argsCallLeft % method->getRealName() % argsCallRight).str();
}

std::string InterfaceMappingCpp::genInterfaceMethodDecl(Interface::Ptr /*interface*/, Method::Ptr method) const {
	static format fmt("virtual %1% pr_call %2%(%3%) = 0;\n");
	return (fmt
		% fullName(method->getReturnType())
		% shortName(method)
		% genMethodArgsDeclaration(method)).str();
}

void InterfaceMappingCpp::gatherSelfMethods(Interface::Ptr context, Methods& methods) const {
	BOOST_FOREACH(NameContext::Ptr child, context->getChildren()) {
		if(Method::Ptr method = dynamic_pointer_cast<Method>(child)) {
			methods.push_back(method);
		}
	}
}
	
void InterfaceMappingCpp::gatherMethods(Interface::Ptr context, Methods& methods) const {
	//methods.clear();
	
	// base methods follow first
	BOOST_FOREACH(Interface::Ptr base, context->getBases()) {
		gatherMethods(base, methods);
	}
	gatherSelfMethods(context, methods);
}

std::string InterfaceMappingCpp::genInterfaceMethods(Interface::Ptr context) const {
	std::string	methodsVtbl;

	Methods methods;
	gatherMethods(context, methods);
	
	std::string	methodVtbl;
	
	BOOST_FOREACH(Method::Ptr method, methods) {
		methodsVtbl += genInterfaceMethodDecl(context, method);
	}
	
	return methodsVtbl;
}
	
std::string InterfaceMappingCpp::genInterfaceMethodsCalls(Interface::Ptr context) const {
	std::string	methodsCall;

	Methods methods;
	gatherMethods(context, methods);
	
	std::string	methodCall;
	
	BOOST_FOREACH(Method::Ptr method, methods) {
		methodsCall += genInterfaceMethodCall(context, method);
	}
	
	return methodsCall;
}
	
std::string InterfaceMappingCpp::genError(const Errors::value_type& errorPair, bool isInterface, const std::string& id, int errorId) const {
	static format fmt("static const tERROR IDLN(%1%) = PR_MAKE_ERR(%2% /*severity*/, %3% /*facility*/, %4% /*implementation id*/, %5% /*incremental code*/);\n");
	std::string facility = isInterface ? "fac_INTERFACE_DECLARED" : "fac_IMPLEMENTATION_DECLARED";
	int severity = findAttr(errorPair.first, AttributeNode::Warning)->getInt() == 0 ? 1 : 0;
	return (fmt % shortName(errorPair.second) % severity % facility % id % errorId).str();
}

std::string InterfaceMappingCpp::genErrors(bool isInterface, const std::string& id) const {
	int errorId = 0;
	std::string data;
	BOOST_FOREACH(const Errors::value_type& errorPair, _errors) {
		data += genError(errorPair, isInterface, id, errorId);
		errorId += 1;
	}
	return data;
}

std::string InterfaceMappingCpp::genMethodArgsDeclaration(Method::Ptr method, const std::string& thisArg) const {
	std::string arglist = thisArg;

	//BOOST_FOREACH(Value::Weak arg, method->getArgs()) {
	const Method::Args& args = method->getArgs();
	Method::Args::const_iterator i = args.begin();
	for( ; i != args.end(); ++i ) {
		Value::Weak arg = *i;
		Type::Ptr   type = arg.lock()->getType();
		std::string typen = fullName( type );
		std::string parn  = arg.lock()->getRealName();
		extendArgsList( arglist, typen + " " + parn );
	}
	
	if ( method->isVarArg() )
		extendArgsList( arglist, "..." );
	
	return arglist;
}
	
std::string InterfaceMappingCpp::genMethodArgsCall(Method::Ptr method, const std::string& thisArg) const {
	std::string args = thisArg;

	BOOST_FOREACH(Value::Weak arg, method->getArgs()) {
		extendArgsList(args, arg.lock()->getRealName());
	}
	
	return args;
}

/*std::string InterfaceMappingCpp::genConstValue(Performer::Ptr performer) const {
	ASSERT(performer);

	if (performer->type() == Performer::TypeString) {
		return (format("\"%1%\"") % performer->asString()).str();
	} else {
		ASSERT(performer->type() == Performer::TypeInt || performer->type() == Performer::TypeFloat);
		return (format("%1%") % performer->asInt()).str();
	}
}*/

std::string InterfaceMappingCpp::genPropertyType(Property::Ptr context) const {
	if (isCharPtr(context)) {
		return "pTYPE_STRING";
	}
	
	Type::Ptr type = reduceTypedef(theTypeRoot, context->getType());
	if (RefType::Ptr ref = dynamic_pointer_cast<RefType>(type)) {
		if (ref->getCount() > 0) {
			return "pTYPE_PTR";
		} else {
			type = type->getType(); // skip constness
		}
	}

	if (BasicType::Ptr basic = dynamic_pointer_cast<BasicType>(type)) {
		switch(basic->getTypeId()) {
			case BasicTypeNode::SInt8		:
				return "pTYPE_CHAR";
			case BasicTypeNode::SInt16		:
				return "pTYPE_SHORT";
			case BasicTypeNode::SInt32      :
				return "pTYPE_INT";
			case BasicTypeNode::SInt64		:
				return "pTYPE_LONGLONG";
			case BasicTypeNode::UInt8		:
				return "pTYPE_BYTE";
			case BasicTypeNode::UInt16		:
				return "pTYPE_WORD";
			case BasicTypeNode::UInt32		:
				return "pTYPE_DWORD";
			case BasicTypeNode::UInt64		:
				return "pTYPE_QWORD";
			case BasicTypeNode::Character	:
			case BasicTypeNode::Float		:
			case BasicTypeNode::Void		:
				ASSERT(false && "not implemented");
			default:
				ASSERT(false);
		}
	}
	
	return "pTYPE_BINARY";
}

std::string InterfaceMappingCpp::genPropertyId(const Attributes& attrs, Property::Ptr context, bool local) const {
	static format fmt("mPROPERTY_MAKE_%3%(%1%, %2%)");
	return (fmt % genPropertyType(context) % genId(attrs) % (local ? "LOCAL" : "GLOBAL")).str();
}

std::string InterfaceMappingCpp::genPropertyMacro(const Attributes& attrs, Property::Ptr context, bool local) const {
	static format fmt("#define %1% %2%\n");
	return (fmt % macroName(context) % genPropertyId(attrs, context, local)).str();
}

std::string InterfaceMappingCpp::genPropertyDecl(const Attributes& attrs, Property::Ptr context, bool local) const {
	static format fmt("static const PropertyId IDLN(%1%) = %2%;\n");
	return (fmt % shortName(context) % genPropertyId(attrs, context, local)).str();
}

} //namespace idl
