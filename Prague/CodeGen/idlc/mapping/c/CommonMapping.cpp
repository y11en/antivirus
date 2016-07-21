#include "Context.hpp"
#include "CommonMapping.hpp"
#include "MappingTools.hpp"
#include "mapping/tools.hpp"

#include "namecontext/RefType.hpp"
#include "namecontext/BasicType.hpp"
#include "namecontext/RootNameContext.hpp"
#include "BasicTypeNode.hpp"

#include <boost/algorithm/string.hpp>
#include <idl_pfx.h>
#include <Scaner.hpp>

namespace idl {

using boost::dynamic_pointer_cast;
using namespace boost::io;
using boost::to_upper_copy;
using boost::to_lower_copy;
using boost::replace_all_copy;
using boost::replace_all;
using boost::format;

CommonMappingC::CommonMappingC(Generator& generator) : CommonMapping(generator) {}

std::string CommonMappingC::fullNameSimple(NameContext::Ptr context) const {
	const NamePath& path = context->getNamePath();
	Interface::Ptr iface = dynamic_pointer_cast<Interface>( context );
	if ( iface ) {
		ASSERT(path.size() == 1);
		return "tag_h" + iface->getRealName();
	}
	return cName(path);
}

std::string CommonMappingC::shortNameSimple(NameContext::Ptr context) const {
	const NamePath& path = context->getNamePath();
	if (Interface::Ptr iface = dynamic_pointer_cast<Interface>(context)) {
		ASSERT(path.size() == 1);
		return "tag_h"+iface->getRealName();
	}
	return cName(path);
}

CommonMappingCpp::CommonMappingCpp(Generator& generator) : CommonMapping(generator) {}



// ---
std::string CommonMappingCpp::fullNameSimple(NameContext::Ptr context) const {
	const NamePath& path = context->getNamePath();
	Interface::Ptr iface = dynamic_pointer_cast<Interface>( context );
	if ( iface ) {
		ASSERT(path.size() == 1);
		std::string name = iface->getRealName();
		return name;
	}

	std::string fulln;
	int size = path.size();
	Type::Ptr type = dynamic_pointer_cast<Type>( context );
	for( int i=0; i<(size-1); ++i ) {
		std::string sect = path.get(i);
		if ( type ) {
			NameContext::Ptr ctx = theTypeRoot->find( sect, Name() );
			if ( ctx )
				sect = ctx->getRealName();
		}
		fulln += sect + "::";
	}
	fulln += path.last();
	return fulln;
}


std::string CommonMappingCpp::shortNameSimple(NameContext::Ptr context) const {
	const NamePath& path = context->getNamePath();
	Interface::Ptr iface = dynamic_pointer_cast<Interface>(context);
	if ( iface ) {
		ASSERT(path.size() == 1);
		std::string name = iface->getRealName();
		scaner::replPrefix( name, IDL_CL_PFX, IDL_IF_PFX );
		return name;
	}
	return path.last();
}

std::string CommonMappingCpp::fullPropNameSimple(NameContext::Ptr context) const {
	std::string full;
	const NamePath& path = context->getNamePath();
	int size = path.size();
	for( int i=0; i<(size-1); ++i ) {
		full += path.get( i );
		full += "::";
	}
	full += "IDLN(" + path.last() + ')';
	return full;
}

CommonMapping::CommonMapping(Generator& generator) : Redirector(generator) {}

std::string CommonMapping::cName(const NamePath& path) {
	std::string fullName = path.str();
	replace_all(fullName, ".", "_");
	return fullName;
}

std::string CommonMapping::name(NameContext::Ptr context, std::string (CommonMapping::*functor)(NameContext::Ptr context) const) const {
	if (RefType::Ptr ref = dynamic_pointer_cast<RefType>(context)) {
		std::string result;
		
		if (ref->isConst()) {
			result += "const ";
		}
		
		result += (this->*functor)(ref->getType());
		
		for (int i = 0; i < ref->getCount(); ++i) {
			result += '*';
		}
		
		return result;
	}
	
	return (this->*functor)(context);
}

std::string CommonMapping::fullName(NameContext::Ptr context) const {
	return name(context, &CommonMapping::fullNameSimple);
}

std::string CommonMapping::shortName(NameContext::Ptr context) const {
	return name(context, &CommonMapping::shortNameSimple);
}

std::string CommonMapping::cName(NameContext::Ptr context) {
	if (RefType::Ptr ref = dynamic_pointer_cast<RefType>(context)) {
		std::string result;
		
		if (ref->isConst()) {
			result += "const ";
		}
		
		result += cName(ref->getType()->getNamePath());
		
		for (int i = 0; i < ref->getCount(); ++i) {
			result += '*';
		}
		
		return result;
	}
	
	return cName(context->getNamePath());
}

std::string CommonMapping::macroName(NameContext::Ptr context) {
	return std::string("MACRO_") + to_upper_copy(cName(context));
}

/// generate name of method within interface (perhaps, derived one)
std::string CommonMapping::methodCName(Interface::Ptr interface, Method::Ptr method) {
	return methodCName(interface, method->getRealName());
}

std::string CommonMapping::methodCName(Interface::Ptr interface, const std::string& method) {
	NamePath path = interface->getNamePath();
	path.push_back(method);
	return cName(path);
}

//void CommonMapping::constDecl(const Attributes& attrs, ConstValue::Ptr context) {
//	static format constFormat("static const %1% %2% = %3%;\n");
//	std::string cnst = 
//		(constFormat 
//			% fullName(context->getType()) 
//			% shortName(context) 
//			% genConstValue(context->getPerformer())).str();
//	add( cnst );
//}

void CommonMappingC::typedefDecl(const Attributes& attrs, Typedef::Ptr context) {
	static format fmt("typedef %1% %2%;\n");
	add(fmt % fullName(context->getType()) % shortName(context));
}

void CommonMappingCpp::typedefDecl(const Attributes& attrs, Typedef::Ptr context) {
	static format fmt("typedef %1% %2%;\n");
	const NamePath& path = context->getNamePath();
	if ( 1 == path.size() )
		add( "#if defined(PR_IDL_TYPEDEF)\n\t" );
	add(fmt % /*fullName*/shortName(context->getType()) % shortName(context));
	if ( 1 == path.size() )
		add( "#endif // defined(PR_IDL_TYPEDEF)\n\n" );
}

void CommonMapping::structDecl(const Attributes& attrs, Struct::Ptr context) {
	add("#error \"export of structs not supported\"\n");
}

void CommonMapping::enumDecl(const Attributes& attrs, Enum::Ptr context) {
	add("#error \"export of enums not supported\"\n");
}


void CommonMappingC::forwardIFace( NameContext::Ptr context ) {
	static format fmt("typedef struct %1% %1%;\n");
	add(fmt % shortName(context));
}

void CommonMappingC::forwardStruct( NameContext::Ptr context ) {
	static format fmt("typedef struct %1% %1%;\n");
	add(fmt % shortName(context));
}

void CommonMappingCpp::forwardIFace( NameContext::Ptr context ) {
	static format fmt("struct %1%;\n");
	add(fmt % shortName(context));
}


void CommonMappingCpp::forwardStruct( NameContext::Ptr context ) {
	static format fmt("struct %1%;\n");
	add(fmt % shortName(context));
}


void CommonMappingC::typecast( NameContext::Ptr context ) {
}

void CommonMappingCpp::typecast( NameContext::Ptr context ) {
}


} //namespace idl
