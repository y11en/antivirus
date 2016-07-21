#include "Context.hpp"
#include "InterfaceMapping.hpp"
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
using boost::to_upper_copy;
using boost::to_lower_copy;
using boost::replace_all_copy;
using boost::replace_all;
using boost::format;

InterfaceMapping::InterfaceMapping(Generator& generator) : CommonMappingC(generator) {
	setBlock("declaration.interface", CodeBlock::CLang);
}

InterfaceMapping::~InterfaceMapping() {
}

void InterfaceMapping::interfaceOpen(const Attributes& attrs, Interface::Ptr context) {
	indent();
	add(format("//\tinterface %1% definition\n") % fullName(context));
	
	add((format("#define %1%_ID %2%\n") 
			% macroName(context)
			% genId(attrs)).str());
	add((format("static const InterfaceId %1%_IID = %2%_ID;\n") 
			% context->getRealName() 
			% macroName(context)).str());
	
	//_methods.clear();
	_errors.clear();
}

void InterfaceMapping::interfaceClose(const Attributes& attrs, Interface::Ptr context) {
	genInterfaceMethods(context);
	genErrors(true, genId(attrs));
	add(format("//\tend of interface %1% definition\n\n") % fullName(context));
	unindent();
}

void InterfaceMapping::constDecl(const Attributes& attrs, ConstValue::Ptr context) {
	static format constFormat("static const %1% %2% = %3%;\n");
	std::string cnst = 
		(constFormat 
			% fullName(context->getType()) 
			% shortName(context) 
			% genConstValue(context->getPerformer())).str();
	add( cnst );
}

void InterfaceMapping::methodDecl(const Attributes& /*attrs*/, Method::Ptr /*context*/) {
	//_methods.push_back(Methods::value_type(attrs, context));
}

void InterfaceMapping::propertyDecl(const Attributes& attrs, Property::Ptr context) {
	genPropertyDecl(attrs, context, false);
}

void InterfaceMapping::messageDecl(const Attributes& attrs, Message::Ptr context) {
	format fmt("static const MessageId %1% = %2%;\n");
	fmt = fmt 
			% shortName(context) 
			% genId(attrs);
	add(fmt);
}

void InterfaceMapping::messageClassDecl(const Attributes& attrs, MessageClass::Ptr context) {
	format fmt("static const MessageClassId %1% = %2%;\n");
	fmt = fmt 
			% shortName(context) 
			% genId(attrs);
	add(fmt);
}

void InterfaceMapping::messageClassExtendDecl(const Attributes& /*attrs*/, MessageClassExtend::Ptr /*context*/) {
}

void InterfaceMapping::errorDecl(const Attributes& attrs, NameContext::Ptr context) {
	_errors.push_back(Errors::value_type(attrs, context));
}

void InterfaceMapping::genInterfaceMethod(Interface::Ptr interface, Method::Ptr method, std::string& methodTypedef, std::string& methodVtbl, std::string& methodCall) const {
	// type name
	format typeName("fnp%1%");
	typeName = typeName % methodCName(interface, method);
	
	// method type
	format thisArg("%1% _this");
	std::string argsTypedef((thisArg % genInterfaceStructPtrName(interface)).str());
	std::string argsCallLeft("_this");
	std::string argsCallRight("(_this)");
				
	argsTypedef = genMethodArgsDeclaration(method, argsTypedef);
	argsCallLeft = genMethodArgsCall(method, argsCallLeft);
	argsCallRight = genMethodArgsCall(method, argsCallRight);
			
	std::string returnType(fullName(method->getReturnType()));
	
	format typedefFmt("typedef %1% (pr_call *%2%) (%3%);\n");
	typedefFmt = typedefFmt % returnType % typeName.str() % argsTypedef;
	
	std::string vtblRec(typeName.str() + "\t" + method->getRealName() + ";\n");
	format callRecFmt("#define CALL_%1%(%2%)\t((_this)->vtbl->%3%(%4%))\n");
	callRecFmt = callRecFmt % methodCName(interface, method) % argsCallLeft % method->getRealName() % argsCallRight;
	
	methodTypedef.append(typedefFmt.str());
	methodVtbl.append(vtblRec);
	methodCall.append(callRecFmt.str());
}

void InterfaceMapping::gatherSelfMethods(Interface::Ptr context, Methods& methods) const {
	BOOST_FOREACH(NameContext::Ptr child, context->getChildren()) {
		if(Method::Ptr method = dynamic_pointer_cast<Method>(child)) {
			methods.push_back(method);
		}
	}
}
	
void InterfaceMapping::gatherMethods(Interface::Ptr context, Methods& methods) const {
	//methods.clear();
	
	// base methods follow first
	BOOST_FOREACH(Interface::Ptr base, context->getBases()) {
		gatherMethods(base, methods);
	}
	gatherSelfMethods(context, methods);
}

std::string InterfaceMapping::genInterfaceStructPtrName(Interface::Ptr context) const {
	return "h" + cName(context);
}

std::string InterfaceMapping::genInterfaceVtblName(Interface::Ptr context) const {
	static format fmt("i%1%Vtbl");
	return (fmt % cName(context)).str();
}

void InterfaceMapping::genInterfaceMethods(Interface::Ptr context) {
	format hStruct(
		"struct %1%;\n"
		"typedef struct %1% %1%;\n"
		"\n"
		"typedef struct tag_%2% {\n"
		"	const %1%* vtbl;\n"
		"	const iSYSTEMVtbl* sys;\n"
		"} *%2%;\n"
	);
		

	hStruct = hStruct % genInterfaceVtblName(context) % genInterfaceStructPtrName(context);

	std::string	methodTypedef;
	std::string	methodVtbl;
	std::string	methodCall;

	Methods methods;
	gatherMethods(context, methods);
	
	BOOST_FOREACH(Method::Ptr method, methods) {
		genInterfaceMethod(context, method, methodTypedef, methodVtbl, methodCall);
	}

	if (methodVtbl.empty()) {
		methodVtbl = "void (*empty_struct_workaround)();\n";
	}
		
	static format vtblStruct(
			"struct %1% {\n"
			"%2%"
			"};\n");
	
	std::string data;
	data += hStruct.str();
	data += methodTypedef;
	data += (vtblStruct % genInterfaceVtblName(context) /*% shift(vtblBase)*/ % shift(methodVtbl)).str();
	data += methodCall;

	add(data);
}
	
void InterfaceMapping::genErrors(bool isInterface, const std::string& id) {
	std::string facility = isInterface ? "fac_INTERFACE_DECLARED" : "fac_IMPLEMENTATION_DECLARED";

	int errorId = 0;
	
	std::string data;
	
	BOOST_FOREACH(Errors::value_type& errorPair, _errors) {
		format fmt("static const tERROR %1% = PR_MAKE_ERR(%2% /*severity*/, %3% /*facility*/, %4% /*implementation id*/, %5% /*incremental code*/);\n");
		int severity = findAttr(errorPair.first, AttributeNode::Warning)->getInt() == 0 ? 1 : 0;
		fmt = fmt % shortName(errorPair.second) % severity % facility % id % errorId;
		data.append(fmt.str());
		errorId += 1;
	}
	add(data);
}

std::string InterfaceMapping::genMethodArgsDeclaration(Method::Ptr method, const std::string& thisArg) const {
	std::string args = thisArg;

	BOOST_FOREACH(Value::Weak arg, method->getArgs()) {
		extendArgsList(args, fullName(arg.lock()->getType()) + " "  + arg.lock()->getRealName());
	}
	
	if (method->isVarArg()) {
		extendArgsList(args, "...");
	}
	
	return args;
}
	
std::string InterfaceMapping::genMethodArgsCall(Method::Ptr method, const std::string& thisArg) const {
	std::string args = thisArg;

	BOOST_FOREACH(Value::Weak arg, method->getArgs()) {
		extendArgsList(args, arg.lock()->getRealName());
	}
	
	return args;
}

/*std::string InterfaceMapping::genConstValue(Performer::Ptr performer) const {
	ASSERT(performer);

	if (performer->type() == Performer::TypeString) {
		return (format("\"%1%\"") % performer->asString()).str();
	} else {
		ASSERT(performer->type() == Performer::TypeInt || performer->type() == Performer::TypeFloat);
		return (format("%1%") % performer->asInt()).str();
	}
}*/

std::string InterfaceMapping::genPropertyType(Property::Ptr context) const {
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
				ASSERT(false);
			default:
				ASSERT(false);
		}
	}
	
	return "pTYPE_BINARY";
}

void InterfaceMapping::genPropertyMacro(const Attributes& attrs, Property::Ptr context, bool local) {
	format fmt("#define %1% mPROPERTY_MAKE_%4%(%2%, %3%)\n");
	fmt = fmt % macroName(context) % genPropertyType(context) % genId(attrs) % (local ? "LOCAL" : "GLOBAL");
	add(fmt);
}

void InterfaceMapping::genPropertyDecl(const Attributes& attrs, Property::Ptr context, bool local) {
	genPropertyMacro(attrs, context, local);
	
	format fmt("static const PropertyId %1% = %2%;\n");
	fmt = fmt % shortName(context) % macroName(context);
	add(fmt);
}

} //namespace idl
