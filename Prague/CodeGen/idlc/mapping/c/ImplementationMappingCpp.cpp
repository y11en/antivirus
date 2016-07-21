#include "Context.hpp"
#include "ImplementationMappingCpp.hpp"
#include "MappingTools.hpp"

#include "namecontext/RefType.hpp"
#include "namecontext/BasicType.hpp"
#include "namecontext/RootNameContext.hpp"

#include "mapping/tools.hpp"

#include "Generator.hpp"

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace idl {

using parser::AttributeNode;
using parser::BasicTypeNode;

using boost::dynamic_pointer_cast;
using boost::format;

ImplementationMappingCpp::ImplementationMappingCpp(Generator& generator) : InterfaceMappingCpp(generator) {
}

void ImplementationMappingCpp::setPluginContext(Component::Ptr pluginContext) {
	_pluginContext = pluginContext;
}

void ImplementationMappingCpp::implementationOpen(const Attributes& attrs, Implementation::Ptr context) {
	//pushCodeBlock("declaration.implementation." + context->getNamePath().str(), CodeBlock::CppLang);
	setBlock("declaration.implementation." + context->getNamePath().str(), CodeBlock::CppLang);
	
	_errors.clear();
	_pluginProperties.clear();
	_sysMethods.clear();

	add( genIface2ImplDecl(attrs,context,_pluginContext->getNamePath().str()) );
	indent();
}

std::string ImplementationMappingCpp::genDataFields(Implementation::Ptr context) const {
	std::string data;
	std::vector< std::pair<Name, Property::Ptr> > properties;
	BOOST_FOREACH(const PluginProperties::value_type& property, _pluginProperties) {
		if (findAttr(property.first, AttributeNode::Shared)->getInt()) {
			continue;
		}
		
		if (AttributeNode::Ptr member_ref = findAttr(property.first, AttributeNode::Member_Ref, false)) {
			static format entry("%1% %2%;\n");
			
			data += (entry % fullName(property.second->getType()) % member_ref->getString()).str();
		}
	}

	return data;	
}

size_t ImplementationMappingCpp::hasSysMethod( const Name& name ) const {
	SysMethods::const_iterator i = _sysMethods.begin();
	for( ; i != _sysMethods.end(); ++i ) {
		NameContext::Ptr method = *i;
		if ( name == method->getRealName() )
			return true;
	}
	return false;
}

std::string ImplementationMappingCpp::genVirtualMethodsTable(Implementation::Ptr context) const {
	format table(
			"tDATA %1%::e_vtbl[] = {\\\n"
			"%2%"
			"};\\\n");
	
	
	std::string data;
	Methods methods;
	gatherMethods(context, methods);
	
	BOOST_FOREACH(Method::Ptr method, methods) {
		format recordFmt("CAST_FROM_MEMBER_FN(Scope::%1%),\\\n");
		data += (recordFmt % shortName(method)).str();
	}

	return (table % shortName(context) % shift(data)).str();
}
	
std::string ImplementationMappingCpp::genSysMethodsTable(Implementation::Ptr context) const {
	static const std::string names[] = {
			"Recognize",
			"ObjectNew",
			"ObjectInit",
			"ObjectInitDone",
			"ObjectCheck",
			"ObjectPreClose",
			"ObjectClose",
			"ChildNew",
			"ChildInitDone",
			"ChildClose",
			"MsgReceive",
			"IFaceTransfer",
			""
	};

	static format structFmt(
//		"static iINTERNAL i_%1%_vtbl = {\n%2%};\n"
		"iINTERNAL %1%::i_vtbl = {\\\n"
		"%2%"
		"};\\\n"
	);
	
	std::string data;
	
	format recordNull("(tIntFn%1%)(NULL),\\\n");
	format recordNew("(tIntFn%1%)(Scope::%1%),\\\n");
	format recordName("(tIntFn%1%)CAST_FROM_MEMBER_FN(Scope::%1%),\\\n");
	
	for (size_t i = 0; !names[i].empty(); ++i) {
		if (names[i] == "ObjectNew") {
			data += shift((recordNew % names[i]).str());
		} else if (hasSysMethod(names[i])) {
			data += shift((recordName % names[i]).str());
		} else {
			data += shift((recordNull % names[i]).str());
		}
	}
		
	return (structFmt % shortName(context) % data).str();
}

std::string ImplementationMappingCpp::genImplementationRegisterExtern(Implementation::Ptr context) const {
	format fmt("extern tERROR pr_call %1%_Register(hROOT root);\n");
	return (fmt % cName(context)).str();
}


// ---
std::string ImplementationMappingCpp::genImplementationRegister(Implementation::Ptr context) const {
	format fmt(
		"tERROR pr_call %1%_Register(hROOT root) {\n"
		"	return %2%::Register(root);\n"
		"}\n");
	return (fmt % context->getRealName() % fullName(context)).str();
}



// ---
std::string ImplementationMappingCpp::genImplementationRegisterInterface(Implementation::Ptr context, Interface::Ptr iface) const {
	static format fmt(
		"{\n"
		"	error = CALL_Root_RegisterIFace(\n"
		"		root,                                // root object\n"
		"\n"
		"		%2%::IID,                     // interface identifier\n"
		"		PID,                                 // plugin identifier\n"
		"		0 /*ImplID*/,                        // implementation identifier\n"
		"\n"
		"		%3%_Version,                         // plugin version\n"
		"		VID /*Version*/,                     // vendor id (must be: implementation version)\n"
		"\n"
		"		&i_vtbl,                             // internal(kernel called) function table\n"
		"		(sizeof(i_vtbl)/sizeof(tPTR)),       // internal function table size\n"
		"		&e_vtbl,                             // external function table\n"
		"		(sizeof(e_vtbl)/sizeof(tPTR)),       // external function table size\n"
		"		%4%_PropTable,                       // property table\n"
		"		mPROPERTY_TABLE_SIZE(%4%_PropTable), // property table size\n"
		"		ActualObjectSize-sizeof(cObjImpl),   // memory size\n"
		"		0                                    // interface flags\n"
		"	);\n"
		"\n"
		"	#ifdef _PRAGUE_TRACE_\n"
		"		if ( PR_FAIL(error) )\n"
		"			PR_TRACE( (root,prtDANGER,\"%1%(%2%) failed to register [%terr]\",error) );\n"
		"	#endif // _PRAGUE_TRACE_\n"
		"	if ( PR_FAIL(error) )\n"
		"		return error;\n"
		"}\n"
	);

	/*
	1 - fullName of implementation
	2 - fullName of interface
	3 - cName of plugin
	4 - cName of implementation
	*/
	std::string iface_name = fullName(iface);
	fmt % fullName(context) % iface_name % cName(_pluginContext) % cName(context);

	return fmt.str();
}

std::string ImplementationMappingCpp::genImplementationRegisterInterfaces(Implementation::Ptr context) const {
	ASSERT(context->getBases().size() == 1 && "implementation must inherit exactly one interface");
	Interface::Ptr iface = context;
	
	std::string data;
	do {
		ASSERT(iface->getBases().size() <= 1 && "interface may inherit no more than one interface");
		
		iface = iface->getBases().front();
		data += genImplementationRegisterInterface(context, iface);
	} while(!iface->getBases().empty());
	
	return shift(data);
}

std::string ImplementationMappingCpp::genImplementationRegisterBody(Implementation::Ptr context) const {
	static format fmt(
		//"tERROR pr_call %1%_Register(hROOT root) {\n"
		"	tERROR error;\n"
		"\n"
		"	PR_TRACE_A0( root, \"Enter %1%::Register method\" );\n"
		"\n"
		"%2%"
		"	PR_TRACE_A1( root, \"Leave %1%::Register method, ret %terr\", error );\n"
		"	return error;\n"
		//"}\n"
	);
	ASSERT(context->getBases().size() == 1 && "implementation must inherit exactly one interface");
	fmt 
		% fullName(context)
		% genImplementationRegisterInterfaces(context);
	
	return fmt.str();
}

bool ImplementationMappingCpp::isHString(Property::Ptr property) const {
	return false;
}

std::string ImplementationMappingCpp::genAccessorPtr(const Attributes& attrs, AttributeNode::AttributeId id) const {
	AttributeNode::Ptr accessor = findAttr(attrs, id, false);
	
	if (accessor) {
		format fmt("CAST_FROM_MEMBER_FN(%1%Redirect)");
		return (fmt % accessor->getString()).str();
	}
	
	return "NULL";
}

std::string ImplementationMappingCpp::genGetterPtr(const Attributes& attrs) const {
	return genAccessorPtr(attrs, AttributeNode::Getter);
}

std::string ImplementationMappingCpp::genSetterPtr(const Attributes& attrs) const {
	return genAccessorPtr(attrs, AttributeNode::Setter);
}

std::string ImplementationMappingCpp::genPropertyValue(Property::Ptr property) const {
	ASSERT(property->getPerformer());
	return genConstValue(property->getPerformer());
}

std::string ImplementationMappingCpp::genPropertyTable(const Attributes& attrs, Implementation::Ptr context) const {
	static format tableFmt(
		"mPROPERTY_TABLE(%1%_PropTable)		\n"
		"%2%"
		"mPROPERTY_TABLE_END(%1%_PropTable)	\n"
	);

	std::string data;
			
	AttributeNode::Ptr unknownPropGetter = findAttr(attrs, AttributeNode::UnknownPropGetter, false);
	AttributeNode::Ptr unknownPropSetter = findAttr(attrs, AttributeNode::UnknownPropSetter, false);

	if (unknownPropGetter || unknownPropSetter) {
		data += (format("mLAST_CALL_PROPERTY(%1%, %2%)\n")
			% genAccessorPtr(attrs, AttributeNode::UnknownPropGetter)
			% genAccessorPtr(attrs, AttributeNode::UnknownPropSetter)).str();
	}
		
	std::vector< std::pair<Name, Property::Ptr> > properties;
	BOOST_FOREACH(const PluginProperties::value_type& property, _pluginProperties) {
		format propFmt("PROPERTY_MAKE(%1%, %4%, %2%, %3%)\n");
		
		std::string cname = fullName(context);
		std::string pName = fullPropNameSimple(property.second);
		std::string pGet  = genGetterPtr(property.first);
		std::string pSet  = genSetterPtr(property.first);
		propFmt = propFmt % pName % pGet % pSet;
		
		if (findAttr(property.first, AttributeNode::Shared)->getInt()) {
			//if (isCharPtr(property.second)) {
			if (property.second->getPerformer()->type() == Performer::TypeString) {
				static format sharedFmt("%1%, sizeof(%1%), cPROP_BUFFER_SHARED_PTR");
				propFmt = propFmt % (sharedFmt % genPropertyValue(property.second)).str();
			} else {
				// integer value
				ASSERT(property.second->getPerformer()->type() == Performer::TypeInt);
				static format sharedFmt("%1%, sizeof(%2%), cPROP_BUFFER_SHARED");
				propFmt = propFmt % (sharedFmt % genPropertyValue(property.second) % fullName(property.second->getType())).str();
			}
		} else {
			AttributeNode::Ptr access = findAttr( property.first, AttributeNode::Access );
			AttributeNode::Ptr member_ref = findAttr( property.first, AttributeNode::Member_Ref, false );
			
			if ( member_ref ) {
				std::string rights = "0";
				if (AccessTools::hasRead(access)) {
					rights += " | cPROP_BUFFER_READ";
				}
				if (AccessTools::hasWrite(access)) {
					rights += " | cPROP_BUFFER_WRITE";
				}
				if (AccessTools::hasWriteOnInit(access)) {
					rights += " | cPROP_WRITABLE_ON_INIT | cPROP_BUFFER_WRITE";
				}
				
				static format fmt("offsetof(%1%, %2%) - sizeof(cObjImpl), sizeof(((%1%*)0)->%2%), %3%");
				std::string mname = member_ref->getString();
				std::string etc   = (fmt % cname % mname % rights).str();
				propFmt = propFmt % etc;
			} else {
				propFmt = propFmt % "0, 0, 0";
			}
		}
		
		data += propFmt.str();
	}
	
	return (tableFmt % cName(context) % shift(data)).str();
}

std::string ImplementationMappingCpp::genSysMethods(Implementation::Ptr context) const {
	std::string data;	
	static format fmt("virtual tERROR pr_call %1%();\n");
	BOOST_FOREACH(NameContext::Ptr method, _sysMethods) {
		data += (fmt % shortName(method)).str();
	}
	return data;
}

std::string ImplementationMappingCpp::genAccessors(const Attributes& attrs, Implementation::Ptr context) const {
	std::set<std::string>	accessors;

	if (AttributeNode::Ptr unknownPropGetter = findAttr(attrs, AttributeNode::UnknownPropGetter, false)) {
		accessors.insert(unknownPropGetter->getString());
	}
		
	if (AttributeNode::Ptr unknownPropSetter = findAttr(attrs, AttributeNode::UnknownPropSetter, false)) {
		accessors.insert(unknownPropSetter->getString());
	}
		
	BOOST_FOREACH(const PluginProperties::value_type& property, _pluginProperties) {
		if (AttributeNode::Ptr getter = findAttr(property.first, AttributeNode::Getter, false)) {
			accessors.insert(getter->getString());
		}
		if (AttributeNode::Ptr setter = findAttr(property.first, AttributeNode::Setter, false)) {
			accessors.insert(setter->getString());
		}
	}

	std::string data;	
	static format fmt(
		"tERROR pr_call %1%Redirect(tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) { return this->%1%(out_size, prop, buffer, size); }\n"
		"virtual tERROR pr_call %1%(tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) = 0;\n"
	);
	BOOST_FOREACH(const std::string& accessor, accessors) {
		data += (fmt % accessor).str();
	}
	return data;
}

void ImplementationMappingCpp::implementationClose(const Attributes& attrs, Implementation::Ptr context) {
	format versionFmt("static const tVERSION Version = %1%;\n");
	add(versionFmt % genVersion(attrs, context));
	//add(genVersion(attrs, context));
	add(genErrors(true, genId(attrs)));
	add(genInterfaceMethods(context));
	
	unindent();
	add("};\n\n");

	format cObjFmt(
		"struct pr_abstract %1% : public %3%, public iObj {\n\n"
		//"	typedef %1% Base;\n"
		"	OBJ_IMPL( %1% );\n\n"
		"	static iINTERNAL i_vtbl;\n"
		"	static tDATA e_vtbl[%2%];\n\n"
		"	inline void *__cdecl operator new(size_t, void *_)	{ return _; }\n"
		"	inline void __cdecl operator delete(void *, void *) {return; }\n\n"
		"	static const size_t ActualObjectSize;\n\n"
		"	static tERROR pr_call ObjectNew( hOBJECT object, tBOOL construct );\n"
		"	static tERROR pr_call Register(hROOT root);\n\n");

	size_t numOfMethods;
	{
		Methods methods;
		gatherMethods(context, methods);
		numOfMethods = methods.size();
	}
		
	add(cObjFmt % fullName(context) % numOfMethods % context->getRealName());
	indent();
	add( genSysMethods(context) );
	add ( "\n" );
	add( genAccessors(attrs,context) );
	add ( "\n" );
	add( genDataFields(context) );
	unindent();
	add("};\n\n\n");
	
	format vtblDefineFmt(
		"#define CPP_VTBL_%1%_DEFINE(Scope) \\\n"
		"	CPP_DEFINE_IMPLEMENTATION(Scope)\\\n"
		"%2%%3%\n");

	add(vtblDefineFmt
			% boost::to_upper_copy(cName(context))
			% shift(genSysMethodsTable(context))
			% shift(genVirtualMethodsTable(context)));
			
	add(genImplementationRegisterExtern(context));
			
	//popCodeBlock();
	
	//pushCodeBlock("definition.implementation." + context->getNamePath().str(), CodeBlock::CppLang);
	setBlock("definition.implementation." + context->getNamePath().str(), CodeBlock::CppLang);

	format registerFmt("tERROR pr_call %1%::Register(hROOT root) {\n");
	add(registerFmt % fullName(context));
	indent();
	add(genPropertyTable(attrs, context));
	unindent();
	add(genImplementationRegisterBody(context));
	add("};\n\n");
	
	add(genImplementationRegister(context));
	
	//popCodeBlock();
}

void ImplementationMappingCpp::sysMethodDecl(const Attributes& /*attrs*/, NameContext::Ptr context) {
	_sysMethods.push_back(context);
}

void ImplementationMappingCpp::propertyPluginDecl(const Attributes& attrs, Property::Ptr context) {
	_pluginProperties.push_back(PluginProperties::value_type(attrs, context));
}

void ImplementationMappingCpp::propertyPluginAddDecl(const Attributes& attrs, Property::Ptr context) {
	add(genPropertyDecl(attrs, context, true));
	_pluginProperties.push_back(PluginProperties::value_type(attrs, context));
}

} // namespace idl

