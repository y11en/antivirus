#include "Context.hpp"
#include "ImplementationMapping.hpp"
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
using namespace boost::io;
using boost::to_upper_copy;
using boost::to_lower_copy;
using boost::replace_all_copy;
using boost::replace_all;
using boost::format;

ImplementationMapping::ImplementationMapping(Generator& generator) : InterfaceMapping(generator) {
}

void ImplementationMapping::setPluginContext(Component::Ptr pluginContext) {
	_pluginContext = pluginContext;
}

void ImplementationMapping::implementationOpen(const Attributes& attrs, Implementation::Ptr context) {
	setBlock("declaration.implementation." + context->getNamePath().str(), CodeBlock::CLang);
	//pushCodeBlock("declaration.implementation." + context->getNamePath().str(), CodeBlock::CLang);
	
	_errors.clear();
	_pluginProperties.clear();
	_sysMethods.clear();
	
	add((format("#define %1%_ID %2%\n") 
			% macroName(context)
			% genId(attrs)).str());
			
	std::string mnemonic = context->getRealName();
	mnemonic += "_IMPLID";

	add((
		format("static const ImplementationId %1% = %2%_ID;\n") 
			% mnemonic 
			% macroName(context)
		).str());
		
}

void ImplementationMapping::genImplementationMethods(Implementation::Ptr context) {
	Methods methods;
	gatherMethods(context, methods);
	
	BOOST_FOREACH(Method::Ptr method, methods) {
		format thisArg("%1% _this");
		std::string args((thisArg % genObjectStructPtrName(context)).str());
		args = genMethodArgsDeclaration(method, args);
		std::string returnType(fullName(method->getReturnType()));
		
		format externFmt("extern %1% pr_call %2% (%3%);\n");
		externFmt = externFmt % returnType % methodCName(context, method) % args;
		
		add(externFmt);
	}
}

std::string ImplementationMapping::genDataStructName(Implementation::Ptr context) const {
	return (format("t%1%_Data") % cName(context)).str();
}

void ImplementationMapping::genDataStruct(Implementation::Ptr context) {
	format tStruct(
		"typedef struct tag_%1% %1%;\n"
		"#define DATA_STRUCT_%2%_FIELDS \\\n"
		"%3%\n");
	
	std::string data;
	
	std::vector< std::pair<Name, Property::Ptr> > properties;
	BOOST_FOREACH(const PluginProperties::value_type& property, _pluginProperties) {
		if (findAttr(property.first, AttributeNode::Shared)->getInt()) {
			continue;
		}
		
		if (AttributeNode::Ptr member_ref = findAttr(property.first, AttributeNode::Member_Ref, false)) {
			static format entry("%1% %2%;\\\n");
			
			data += (entry % fullName(property.second->getType()) % member_ref->getString()).str();
		}
	}

	if (data.empty()) {
		data = "char empty_struct_workaround;\\\n";
	}
		
	tStruct = tStruct % genDataStructName(context) % cName(context) % shift(data);
	add(tStruct);
}

size_t ImplementationMapping::hasSysMethod(const Name& name) const {
	SysMethods::const_iterator i = _sysMethods.begin();
	for( ; i != _sysMethods.end(); ++i ) {
		NameContext::Ptr method = *i;
		if ( name == method->getRealName() )
			return true;
	}
	return false;
}

void ImplementationMapping::genVtbl(Implementation::Ptr context) {
	static format vtblFmt("static %3% e_%1%_vtbl = {\n%2%};\n");
	
	std::string data;
	
	Methods methods;
	gatherMethods(context, methods);
	
	BOOST_FOREACH(Method::Ptr method, methods) {
		static format recordFmt("(fnp%1%) %1%,\n");
		data += shift((recordFmt % methodCName(context, method)).str());
	}
	
	add(vtblFmt % cName(context) % data % genInterfaceVtblName(context));
}
	
void ImplementationMapping::genSysMethodsTable(Implementation::Ptr context) {
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
		"static iINTERNAL i_%1%_vtbl = {\n%2%};\n"
	);
	
	std::string data;
	
	format recordNull("(tIntFn%1%) NULL,\n");
	format recordName("(tIntFn%1%) %2%_%1%,\n");
	
	for (size_t i = 0; !names[i].empty(); ++i) {
		if (hasSysMethod(names[i])) {
			data += shift((recordName % names[i] % cName(context)).str());
		} else {
			data += shift((recordNull % names[i]).str());
		}
	}
		
	add(structFmt % cName(context) % data);
}

void ImplementationMapping::genImplementationRegisterExtern(Implementation::Ptr context) {
	static format fmt("extern tERROR pr_call %1%_Register(hROOT root);\n");
	add(fmt % cName(context));
}

std::string ImplementationMapping::genImplementationRegisterInterface(Implementation::Ptr context, Interface::Ptr iface) const {
	static format fmt(
			"{\n"
			"\n"
			"	error = CALL_Root_RegisterIFace(\n"
			"		root,                                // root object\n"
			"\n"
			"		%2%_IID,                             // interface identifier\n"
			"		%3%_PLUGINID,                        // plugin identifier\n"
			"		%4%_IMPLID,                          // implementation identifier\n"
			"\n"
			"		%5%_Version,                         // plugin version\n"
			"		%1%_Version,                         // implementation version\n"
			"\n"
			"		&i_%1%_vtbl,                         // internal(kernel called) function table\n"
			"		(sizeof(i_%1%_vtbl)/sizeof(tPTR)),   // internal function table size\n"
			"		&e_%1%_vtbl,                         // external function table\n"
			"		(sizeof(e_%1%_vtbl)/sizeof(tPTR)),   // external function table size\n"
			"		%1%_PropTable,                       // property table\n"
			"		mPROPERTY_TABLE_SIZE(%1%_PropTable), // property table size\n"
			"		sizeof(%6%),                         // memory size\n"
			"		0                                    // interface flags\n"
			"	);\n"
			"\n"
			"	#ifdef _PRAGUE_TRACE_\n"
			"		if ( PR_FAIL(error) )\n"
			"			PR_TRACE( (root,prtDANGER,\"%1%(%2%) failed to register [%terr]\",error) );\n"
			"	#endif // _PRAGUE_TRACE_\n"
			"\n"
			"	if ( PR_FAIL(error) )\n"
			"		return error;\n"
			"}\n"
	);
	
	ASSERT(iface);
	std::string interfaceName = iface->getRealName();
	std::string pluginName = shortName(_pluginContext);
	
	return (fmt 
			% cName(context)
			% interfaceName
			% pluginName
			% cName(context)
			% pluginName
			% genDataStructName(context)
			).str();
}

std::string ImplementationMapping::genImplementationRegisterInterfaces(Implementation::Ptr context) const {
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

				
void ImplementationMapping::genImplementationRegister(Implementation::Ptr context) {
	static format fmt(
			"tERROR pr_call %1%_Register(hROOT root) {																\n"
			"	tERROR error;																						\n"
			"	                                                                                                   	\n"
			"	PR_TRACE_A0( root, \"Enter %1%::Register method\" );                                          		\n"
			"	                                                                                                   	\n"
			"%2%"
			"	PR_TRACE_A1( root, \"Leave %1%::Register method, ret %terr\", error );                       		\n"
			"	return error;																						\n"
			"}	                                                                        							\n"
	);
	
	ASSERT(context->getBases().size() == 1);
	add((fmt 
		% cName(context)
		% genImplementationRegisterInterfaces(context)).str());
}

bool ImplementationMapping::isHString(Property::Ptr property) const {
	return false;
}

std::string ImplementationMapping::genAccessorPtr(Implementation::Ptr context, const Attributes& attrs, AttributeNode::AttributeId id) const {
	AttributeNode::Ptr accessor = findAttr(attrs, id, false);
	
	if (accessor) {
		return methodCName(context, accessor->getString());
	}
	
	return "NULL";
}

std::string ImplementationMapping::genGetterPtr(Implementation::Ptr context, const Attributes& attrs) const {
	return genAccessorPtr(context, attrs, AttributeNode::Getter);
}

std::string ImplementationMapping::genSetterPtr(Implementation::Ptr context, const Attributes& attrs) const {
	return genAccessorPtr(context, attrs, AttributeNode::Setter);
}

std::string ImplementationMapping::genPropertyValue(Property::Ptr property) const {
	ASSERT(property->getPerformer());
	return genConstValue(property->getPerformer());
}

void ImplementationMapping::genPropertyTable(const Attributes& attrs, Implementation::Ptr context) {
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
				% genAccessorPtr(context, attrs, AttributeNode::UnknownPropGetter)
				% genAccessorPtr(context, attrs, AttributeNode::UnknownPropSetter)).str();
	}
		
	std::vector< std::pair<Name, Property::Ptr> > properties;
	BOOST_FOREACH(const PluginProperties::value_type& property, _pluginProperties) {
		format propFmt("PROPERTY_MAKE(%1%, %4%, %2%, %3%)\n");
		
		propFmt = propFmt % macroName(property.second) % genGetterPtr(context, property.first) % genSetterPtr(context, property.first);
		
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
			AttributeNode::Ptr access = findAttr(property.first, AttributeNode::Access);
			
			if (AttributeNode::Ptr member_ref = findAttr(property.first, AttributeNode::Member_Ref, false)) {
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
				
				static format fmt("offsetof(%1%, %2%), sizeof(((%1%*)0)->%2%), %3%");
				propFmt = propFmt % (fmt % genDataStructName(context) % member_ref->getString() % rights).str();
			} else {
				propFmt = propFmt % "0, 0, 0";
			}
		}
		
		data += propFmt.str();
	}
	
	
	add(tableFmt % cName(context) % shift(data));
}

void ImplementationMapping::genSysMethods(Implementation::Ptr context) {
	format fmt("extern tERROR pr_call %1%(%2% _this );\n");
	BOOST_FOREACH(NameContext::Ptr method, _sysMethods) {
		add(fmt % cName(method) % genInterfaceStructPtrName(context));
	}
}

void ImplementationMapping::genAccessors(const Attributes& attrs, Implementation::Ptr context) {
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
	
	format fmt("extern tERROR pr_call %1%( %2% _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );\n");
	BOOST_FOREACH(const std::string& accessor, accessors) {
		add(fmt % methodCName(context, accessor) % genObjectStructPtrName(context));
	}
}

std::string ImplementationMapping::genObjectStructPtrName(Implementation::Ptr context) const {
	return "hi_" + cName(context);
}

void ImplementationMapping::implementationClose(const Attributes& attrs, Implementation::Ptr context) {
	format versionFmt("static const tVERSION %1%_Version = %2%;\n");
	add(versionFmt % context->getRealName() % genVersion(attrs, context));
		
	genErrors(true, genId(attrs));

	genInterfaceMethods(context);
	genSysMethods(context);
	
	genImplementationRegisterExtern(context);
			
	genDataStruct(context);
	
	static format objFmt(
		"typedef struct tag_%1% {	\n"
		"	const %2%* 	vtbl;\n"
		"	const iSYSTEMVTBL*  sys;\n"
		"	%3%*          data;\n"
		"} *%1%;\n"
	);
	add(objFmt % genObjectStructPtrName(context) % genInterfaceVtblName(context) % genDataStructName(context));
			
	genImplementationMethods(context);
	genAccessors(attrs, context);

	//popCodeBlock();
	
	//pushCodeBlock("definition.implementation." + context->getNamePath().str(), CodeBlock::CLang);
	setBlock("definition.implementation." + context->getNamePath().str(), CodeBlock::CLang);

	genSysMethodsTable(context);

	genVtbl(context);

	genPropertyTable(attrs, context);
					
	genImplementationRegister(context);
	//popCodeBlock();
}

void ImplementationMapping::sysMethodDecl(const Attributes& /*attrs*/, NameContext::Ptr context) {
	_sysMethods.push_back(context);
}

void ImplementationMapping::propertyPluginDecl(const Attributes& attrs, Property::Ptr context) {
	_pluginProperties.push_back(PluginProperties::value_type(attrs, context));
}

void ImplementationMapping::propertyPluginAddDecl(const Attributes& attrs, Property::Ptr context) {
	genPropertyDecl(attrs, context, true);
	_pluginProperties.push_back(PluginProperties::value_type(attrs, context));
}

} // namespace idl

