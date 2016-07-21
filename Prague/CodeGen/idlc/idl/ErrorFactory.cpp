#include "ErrorFactory.hpp"

#include "Context.hpp"
#include "namecontext/NameContext.hpp"

#include "boost/format.hpp"
#include <iostream>

namespace idl {

using parser::Context;
using boost::format;

const std::string ErrorFactory::TheErrorFormats[] = {
	"failed to open file `%1%'",                         "FailedToOpenFile",
	"failed to find include file `%1%'",                 "FailedToFindIncludeFile",
	"fatal scaner error; line: %1%, reason: %2%",        "FatalScanerError",
	"%1%",                                               "ParseError",
	"name `%1%' already defined \n\tsee %2%",            "NameAlreadyDefined",
	"name `%1%' not found within context `%2%'",         "NameNotFound",
	"type expected",                                     "TypeExpected",
	"value expected",                                    "ValueExpected",
	"int value expected",                                "IntValueExpected",
	"interface name expected",                           "InterfaceExpected",
	"vendor name expected",                              "VendorExpected",
	"constant must be of basic type",                    "ConstMustBeOfBasicType",
	"unknown constant name `%1%'",                       "UnknownConstName",
	"argument of operation in constant expression requires basic type (got `%1%' instead)"    , "OperationRequiresArgsOfBasicType",
	"argument of operation in constant expression requires integer type (got `%1%' instead)"	, "OperationRequiresArgsOfIntType",
	"`%2%'-declaration duplicated (see %1%)"                                                  , "DeclarationDuplicated",
	"mandatory declaration `%1%' not found"                                                   , "MandatoryDeclarationNotFound",
	"malformed `%1%' - underscores are not allowed in identifiers"                            , "UnderscoresAreNotAllowed",
	"property `%1%' not found within context `%2%'"                                           , "PropertyNotFound",
	"msg class `%1%' not found within context `%2%'"                                          , "MessageClassNotFound",
	"type of property `%1%' differs from interface declaration (see %2%)"                     , "PropertyTypeMismatch",

	"attribute definition `%1%' duplicated (see %2%)",                  "AttributeDuplicated",
	"required attribute `%1%' not found",                               "RequiredAttributeNotFound",
	"attribute `%1%' is not allowed in this context",                   "AttributeNotAllowed",
	
	"void type is not allowed in this context",                         "VoidNotAllowed",
	
	"shared property is allowed to have getter or setter (see %1%)",    "SharedPropertyHasGetterOrSetter",
	"setter specified for read only property (see %1%)",                "ReadOnlyPropertyHasSetter",
	"read only property has no accessor",                               "ReadOnlyPropertyHasNoAccessor",
	"getter specified for write only property (see %1%)",               "WriteOnlyPropertyHasGetter",
	"write only property has no accessor",                              "WriteOnlyPropertyHasNoAccessor",
	"access attribute can't be empty",                                  "EmptyPropertyAccessList",
	"property accessors are overspecified",                             "PropertyAccessorsOverspecified",

	"shared property `%1%' must be assigned with a value",              "SharedPropertyHasNoValue",
	"local property `%1%' can't be assigned with a value",              "LocalPropertyHasValue",
	
	"const value specified for `%1%' has incompatible type",            "ConstValueTypeMismatched",
	
	"name `%1%' must be declared before first usage (see %2%)",         "NameMustBeDeclaredBeforeUsage",
	"const value specified for `%1%' of reference type must be zero",   "ConstValueForPtrMustBeZero",

	"forward declaration on unknown name `%1%'",                        "ForwardDeclarationOnUnknownName",
	"`%1%' is not an interface or struct (see `%2%'); only interfaces or structs may have forward declarations",    "onlyInterfaceOrStructMayHaveForwardDeclarations"
	"required property `%1%' not implemented in `%2%'",                 "RequiredPropNotImplemented"
	"implementaition '%1%' must be based on single interface",          "ImplementaitionMustBeBasedOnSingleInterface"
};

ErrorFactory& ErrorFactory::instance() {
	static ErrorFactory factory;
	return factory;
}


//void error(const format& msg) {
//	std::cout << msg << std::endl;
//}

// ---
std::string _context_str( const Context& ctx ) {
	std::string msg;
	int         fl = ctx.getFirstLine();
	int         fc = ctx.getFirstColon();
	int         ll = ctx.getLastLine();
	int         lc = ctx.getLastColon();

	char buff[80];
	if ( fl == ll )
		sprintf( buff, "%d.%d-%d", fl, fc, lc );
	else
		sprintf( buff, "%d-%d.%d-%d", fl, fc, ll, lc );
	return ctx.getSource() + "(" + buff + ")";
}

// ---
ErrorFactory::Error::Error( ErrorType type ) 
	: _type(type), _src(), _fl(0), _fc(0), _ll(0), _lc(0), _msg(ErrorFactory::TheErrorFormats[2*type]) {
}

// ---
ErrorFactory::Error::Error( ErrorType type, const parser::Context& c ) 
	: _type(type), 
		_src(c.getSource()), _fl(c.getFirstLine()), _fc(c.getFirstColon()), _ll(c.getLastLine()), _lc(c.getLastColon()), 
		_msg() {
}


// ---
ErrorFactory::Error::Error( ErrorType type, const parser::Context& c, bool ) 
	: _type(type), 
		_src(c.getSource()), _fl(c.getFirstLine()), _fc(c.getFirstColon()), _ll(c.getLastLine()), _lc(c.getLastColon()), 
		_msg( ErrorFactory::TheErrorFormats[2*type] ) {
}


// ---
ErrorFactory::Error::~Error() {
}


// ---
ErrorFactory::~ErrorFactory() {
}


// ---
void ErrorFactory::Error::out( std::ostream& stm ) const {
	std::string text;
	if ( _fl ) {
		char buff[80];
		if ( _fl == _ll )
			sprintf( buff, "%d.%d-%d", _fl, _fc, _lc );
		else
			sprintf( buff, "%d-%d.%d-%d", _fl, _fc, _ll, _lc );
		stm << _src << "(" << buff << "): ";
	}
	stm << "error(" << ErrorFactory::TheErrorFormats[2*_type+1] << "): " << _msg << std::endl;
}


// ---
bool ErrorFactory::out() {
	const ErrorFactory::Errors& errors = instance().getErrors();

	if ( !errors.empty() ) {
		ErrorFactory::Errors::const_iterator i = errors.begin(), end = errors.end();
		for (; i != end; ++i) {
			//std::cout << "error: " << format(i->second) << std::endl;
			//error(format(i->second));
			i->out( std::cout );
		}
		return false;
	}
	return true;
}


void ErrorFactory::clear() {
	_errors.clear();
}

const ErrorFactory::Errors& ErrorFactory::getErrors() const {
	return _errors;
}

void ErrorFactory::failedToOpenFile(const std::string& file) {
	Error err = FailedToOpenFile;
	err.msg( (err.fmt() % file).str() );
	_errors.push_back( err );
}

void ErrorFactory::failedToFindIncludeFile(const std::string& file) {
	Error err = FailedToFindIncludeFile;
	err.msg( (err.fmt() % file).str() );
	_errors.push_back( err );
}

void ErrorFactory::fatalScanerError( int line, const std::string& text ) {
	Error err = FatalScanerError;
	err.msg( (err.fmt() % line % text).str() );
	_errors.push_back( err );
}

void ErrorFactory::parseError(const Context& ctx, const std::string& description) {
	Error err( ParseError, ctx );
	err.msg( (err.fmt() % description).str() );
	_errors.push_back( err );
}
  	
void ErrorFactory::nameAlreadyDefined(const Context& ctx, const Name& name, const Context& whereFound) {
	Error err( NameAlreadyDefined, ctx );
	err.msg( (err.fmt() % name % _context_str(whereFound)).str() );
	_errors.push_back( err );
}

void ErrorFactory::nameNotFound(const Context& ctx, const Name& name, NameContext::Ptr nameContext) {
	Error err( NameNotFound, ctx );
	std::string nCtx = nameContext->getNamePath().str();
	if ( nCtx.empty() )
		nCtx = "root";
	err.msg( (err.fmt() % name % nCtx).str() );
	_errors.push_back( err );
}

void ErrorFactory::typeExpected(const Context& ctx) {
	_errors.push_back( Error(TypeExpected,ctx,false) );
}

void ErrorFactory::valueExpected(const Context& ctx) {
	_errors.push_back( Error(ValueExpected,ctx,false) );
}

void ErrorFactory::intValueExpected(const parser::Context& ctx) {
	_errors.push_back( Error(IntValueExpected,ctx,false) );
}

void ErrorFactory::interfaceExpected(const parser::Context& ctx) {
	_errors.push_back( Error(InterfaceExpected,ctx,false) );
}

void ErrorFactory::vendorExpected(const parser::Context& ctx) {
	_errors.push_back( Error(VendorExpected,ctx,false) );
}

void ErrorFactory::constMustBeOfBasicType(const parser::Context& ctx) {
	_errors.push_back( Error(ConstMustBeOfBasicType,ctx,false) );
}

void ErrorFactory::unknownConstName(const parser::Context& ctx, const NamePath& namePath) {
	Error err( UnknownConstName, ctx );
	err.msg( (err.fmt() % namePath.str()).str() );
	_errors.push_back( err );
}

void ErrorFactory::operationRequiresArgsOfBasicType(const parser::Context& ctx, const NamePath& namePath) {
	Error err( OperationRequiresArgsOfBasicType, ctx );
	err.msg( (err.fmt() % namePath.str()).str() );
	_errors.push_back( err );
}

void ErrorFactory::operationRequiresArgsOfIntType(const parser::Context& ctx, const NamePath& namePath) {
	Error err( OperationRequiresArgsOfIntType, ctx );
	err.msg( (err.fmt() % namePath.str()).str() );
	_errors.push_back( err );
}

void ErrorFactory::declarationDuplicated(const parser::Context& ctx, const parser::Context& firstFound, const std::string& declarationName) {
	Error err( DeclarationDuplicated, ctx );
	err.msg( (err.fmt() % firstFound.shortContextInfo() % declarationName).str() );
	_errors.push_back( err );
}

void ErrorFactory::mandatoryDeclarationNotFound(const parser::Context& ctx, const std::string& declarationName) {
	Error err( MandatoryDeclarationNotFound, ctx );
	err.msg( (err.fmt() % declarationName).str() );
	_errors.push_back( err );
}

void ErrorFactory::underscoresAreNotAllowed(const parser::Context& ctx, const std::string& identifier) {
	Error err( UnderscoresAreNotAllowed, ctx );
	err.msg( (err.fmt() % identifier).str() );
	_errors.push_back( err );
}

void ErrorFactory::propertyNotFound(const parser::Context& ctx, const Name& name, NameContextPtr nameContext) {
	Error err( PropertyNotFound, ctx );
	err.msg( (err.fmt() % name % nameContext->getNamePath().str()).str() );
	_errors.push_back( err );
}

void ErrorFactory::messageClassNotFound(const parser::Context& ctx, const Name& name, NameContextPtr nameContext) {
	Error err( MessageClassNotFound, ctx );
	err.msg( (err.fmt() % name % nameContext->getNamePath().str()).str() );
	_errors.push_back( err );
}

void ErrorFactory::propertyTypeMismatch(const parser::Context& ctx, const Name& name, const parser::Context& declContext) {
	Error err( PropertyTypeMismatch, ctx );
	err.msg( (err.fmt() % name % declContext).str() );
	_errors.push_back( err );
}

void ErrorFactory::attributeDuplicated(const parser::Context& ctx, const Name& name, const parser::Context& declContext) {
	Error err( AttributeDuplicated, ctx );
	err.msg( (err.fmt() % name % declContext).str() );
	_errors.push_back( err );
}

void ErrorFactory::requiredAttributeNotFound(const parser::Context& ctx, const Name& identifier) {
	Error err( RequiredAttributeNotFound, ctx );
	err.msg( (err.fmt() % identifier).str() );
	_errors.push_back( err );
}

void ErrorFactory::attributeNotAllowed(const parser::Context& ctx, const Name& identifier) {
	Error err( AttributeNotAllowed, ctx );
	err.msg( (err.fmt() % identifier).str() );
	_errors.push_back( err );
}

void ErrorFactory::voidNotAllowed(const parser::Context& ctx) {
	Error err( VoidNotAllowed, ctx );
	err.msg( (err.fmt()).str() );
	_errors.push_back( err );
}

void ErrorFactory::sharedPropertyHasGetterOrSetter(const parser::Context& ctx, const parser::Context& whereFound) {
	Error err( SharedPropertyHasGetterOrSetter, ctx );
	err.msg( (err.fmt() % whereFound).str() );
	_errors.push_back( err );
}

void ErrorFactory::readOnlyPropertyHasSetter(const parser::Context& ctx, const parser::Context& whereFound) {
	Error err( ReadOnlyPropertyHasSetter, ctx );
	err.msg( (err.fmt() % whereFound).str() );
	_errors.push_back( err );
}

void ErrorFactory::readOnlyPropertyHasNoAccessor(const parser::Context& ctx) {
	Error err( ReadOnlyPropertyHasNoAccessor, ctx );
	err.msg( (err.fmt()).str() );
	_errors.push_back( err );
}

void ErrorFactory::writeOnlyPropertyHasGetter(const parser::Context& ctx, const parser::Context& whereFound) {
	Error err( WriteOnlyPropertyHasGetter, ctx );
	err.msg( (err.fmt() % whereFound).str() );
	_errors.push_back( err );
}

void ErrorFactory::writeOnlyPropertyHasNoAccessor(const parser::Context& ctx) {
	Error err( WriteOnlyPropertyHasNoAccessor, ctx );
	err.msg( (err.fmt()).str() );
	_errors.push_back( err );
}

void ErrorFactory::emptyPropertyAccessList(const parser::Context& ctx) {
	Error err( EmptyPropertyAccessList, ctx );
	err.msg( (err.fmt()).str() );
	_errors.push_back( err );
}

void ErrorFactory::propertyAccessorsOverspecified(const parser::Context& ctx) {
	Error err( PropertyAccessorsOverspecified, ctx );
	err.msg( (err.fmt()).str() );
	_errors.push_back( err );
}

void ErrorFactory::sharedPropertyHasNoValue(const parser::Context& ctx, const NamePath& namePath) {
	Error err( SharedPropertyHasNoValue, ctx );
	err.msg( (err.fmt() % namePath.str()).str() );
	_errors.push_back( err );
}

void ErrorFactory::localPropertyHasValue(const parser::Context& ctx, const NamePath& namePath) {
	Error err( LocalPropertyHasValue, ctx );
	err.msg( (err.fmt() % namePath.str()).str() );
	_errors.push_back( err );
}

void ErrorFactory::constValueTypeMismatched(const parser::Context& ctx, const NamePath& namePath) {
	Error err( ConstValueTypeMismatched, ctx );
	err.msg( (err.fmt() % namePath.str()).str() );
	_errors.push_back( err );
}

void ErrorFactory::nameMustBeDeclaredBeforeUsage(const parser::Context& ctx, const Name& name, const parser::Context& declContext) {
	Error err( NameMustBeDeclaredBeforeUsage, ctx );
	err.msg( (err.fmt() % name % declContext).str() );
	_errors.push_back( err );
}

void ErrorFactory::constValueForPtrMustBeZero(const parser::Context& ctx, const NamePath& namePath) {
	Error err( ConstValueForPtrMustBeZero, ctx );
	err.msg( (err.fmt() % namePath.str()).str() );
	_errors.push_back( err );
}

void ErrorFactory::forwardDeclarationOnUnknownName(const parser::Context& ctx, const Name& name) {
	Error err( ForwardDeclarationOnUnknownName, ctx );
	err.msg( (err.fmt() % name).str() );
	_errors.push_back( err );
}

void ErrorFactory::onlyInterfaceOrStructMayHaveForwardDeclarations(const parser::Context& ctx, const Name& name, const parser::Context& declContext ) {
	Error err( OnlyInterfaceOrStructMayHaveForwardDeclarations, ctx );
	boost::format fmt = err.fmt();
	std::string str = (fmt % name % declContext).str();
	err.msg( (err.fmt() % name % declContext).str() );
	_errors.push_back( err );
}

void ErrorFactory::requiredPropNotImplemented(const parser::Context& ctx, const NamePath& propName, const NamePath& implementation) {
	Error err( RequiredPropNotImplemented, ctx );
	err.msg( (err.fmt() % propName.str() % implementation.str()).str() );
	_errors.push_back( err );
}

void ErrorFactory::implementaitionMustBeBasedOnSingleInterface( const parser::Context& ctx, const Name& impName ) {
	Error err( ImplementaitionMustBeBasedOnSingleInterface, ctx );
	err.msg( (err.fmt() % impName).str() );
	_errors.push_back( err );
}


} //namespace idl
