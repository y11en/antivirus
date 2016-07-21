#ifndef ErrorFactory_hpp
#define ErrorFactory_hpp

#include <string>
#include <vector>

#include <utility>
#include "boost/format.hpp"
#include "Context.hpp"

#include "types.hpp"

namespace idl { namespace parser {
class Context;
}} //namespace idl::parser

namespace idl {

class NameContext;

class ErrorFactory {
public:
	enum ErrorType {
		FailedToOpenFile		= 0,
		FailedToFindIncludeFile,
		FatalScanerError,
		ParseError,
		NameAlreadyDefined,
		NameNotFound,
		TypeExpected,
		ValueExpected,
		IntValueExpected,
		InterfaceExpected,
		VendorExpected,
		ConstMustBeOfBasicType,
		UnknownConstName,
		OperationRequiresArgsOfBasicType,
		OperationRequiresArgsOfIntType,
		DeclarationDuplicated,
		MandatoryDeclarationNotFound,
		UnderscoresAreNotAllowed,
		PropertyNotFound,
		MessageClassNotFound,
		PropertyTypeMismatch,
		AttributeDuplicated,
		RequiredAttributeNotFound,
		AttributeNotAllowed,

		VoidNotAllowed,

		SharedPropertyHasGetterOrSetter,
		ReadOnlyPropertyHasSetter,
		ReadOnlyPropertyHasNoAccessor,
		WriteOnlyPropertyHasGetter,
		WriteOnlyPropertyHasNoAccessor,
		EmptyPropertyAccessList,
		PropertyAccessorsOverspecified,

		SharedPropertyHasNoValue,
		LocalPropertyHasValue,

		ConstValueTypeMismatched,

		NameMustBeDeclaredBeforeUsage,

		ConstValueForPtrMustBeZero,
		ForwardDeclarationOnUnknownName,
		OnlyInterfaceOrStructMayHaveForwardDeclarations,
		RequiredPropNotImplemented,
		ImplementaitionMustBeBasedOnSingleInterface,
	};

	//typedef std::pair<ErrorType, std::string>	Pair;
	//typedef std::vector<Pair>					Errors;

	struct Error {
		ErrorType   _type;
		std::string _src;
		int         _fl, _fc, _ll, _lc;
		std::string _msg;

		Error( ErrorType type );
		Error( ErrorType type, const parser::Context& ctx );
		Error( ErrorType type, const parser::Context& ctx, bool );
		~Error();

		boost::format fmt() { return boost::format(ErrorFactory::TheErrorFormats[2*_type]); }
		void msg( std::string& msg ) { _msg = msg; }

		void out( std::ostream& o ) const;
	};

	typedef std::vector<Error> Errors;

	typedef boost::intrusive_ptr<NameContext> NameContextPtr;

public:

	~ErrorFactory();
	static ErrorFactory& instance();
	static bool out();

	void clear();

	const Errors&	getErrors() const;

	void failedToOpenFile(const std::string& file);
	void failedToFindIncludeFile(const std::string& file);
	void fatalScanerError(int line, const std::string& msg);
	void parseError(const parser::Context& context, const std::string& message);
	void nameAlreadyDefined(const parser::Context& context, const Name& name, const parser::Context& whereFound);
	void nameNotFound(const parser::Context& context, const Name& name, NameContextPtr nameContext);
	void typeExpected(const parser::Context& context);
	void valueExpected(const parser::Context& context);
	void intValueExpected(const parser::Context& context);
	void interfaceExpected(const parser::Context& context);
	void vendorExpected(const parser::Context& context);
	void constMustBeOfBasicType(const parser::Context& context);
	void unknownConstName(const parser::Context& context, const NamePath& namePath);
	void operationRequiresArgsOfBasicType(const parser::Context& context, const NamePath& namePath);
	void operationRequiresArgsOfIntType(const parser::Context& context, const NamePath& namePath);

	void declarationDuplicated(const parser::Context& firstFound, const parser::Context& current, const std::string& declarationName);
	void mandatoryDeclarationNotFound(const parser::Context& context, const std::string& declarationName);

	void underscoresAreNotAllowed(const parser::Context& context, const std::string& identifier);

	void propertyNotFound(const parser::Context& context, const Name& name, NameContextPtr nameContext);
	void messageClassNotFound(const parser::Context& context, const Name& name, NameContextPtr nameContext);
	void propertyTypeMismatch(const parser::Context& context, const Name& name, const parser::Context& declContext);

	void attributeDuplicated(const parser::Context& context, const Name& name, const parser::Context& declContext);
	void requiredAttributeNotFound(const parser::Context& context, const Name& identifier);
	void attributeNotAllowed(const parser::Context& context, const Name& identifier);

	void voidNotAllowed(const parser::Context& context);

	void sharedPropertyHasGetterOrSetter(const parser::Context& context, const parser::Context& whereFound);
	void readOnlyPropertyHasSetter(const parser::Context& context, const parser::Context& whereFound);
	void readOnlyPropertyHasNoAccessor(const parser::Context& context);
	void writeOnlyPropertyHasGetter(const parser::Context& context, const parser::Context& whereFound);
	void writeOnlyPropertyHasNoAccessor(const parser::Context& context);
	void emptyPropertyAccessList(const parser::Context& context);
	void propertyAccessorsOverspecified(const parser::Context& context);

	void sharedPropertyHasNoValue(const parser::Context& context, const NamePath& namePath);
	void localPropertyHasValue(const parser::Context& context, const NamePath& namePath);

	void constValueTypeMismatched(const parser::Context& context, const NamePath& namePath);

	void nameMustBeDeclaredBeforeUsage(const parser::Context& context, const Name& name, const parser::Context& declContext);

	void constValueForPtrMustBeZero(const parser::Context& context, const NamePath& namePath);
	void forwardDeclarationOnUnknownName(const parser::Context& context, const Name& name);
	void onlyInterfaceOrStructMayHaveForwardDeclarations(const parser::Context& context, const Name& name, const parser::Context& declContext);
	void requiredPropNotImplemented(const parser::Context& context, const NamePath& propName, const NamePath& implementation);
	void implementaitionMustBeBasedOnSingleInterface( const parser::Context& context, const Name& impName );

private:
	Errors	_errors;


	static const std::string TheErrorFormats[];
};

}

#endif //ErrorFactory_hpp
