#ifndef Lexeme_hpp
#define Lexeme_hpp

#include "types.hpp"
#include "grammar.hpp"

#include "Context.hpp"
//#include "LexemeValue.hpp"

#include <boost/shared_ptr.hpp>

namespace idl { namespace scaner {

class LexemeValue;

typedef idl::parser::Context Context;

class Lexeme {
  public:
	struct Ids {
  		#include "LexemeIds.hpp" // in order to protect namespace
	};
	typedef Ids::Id Id;

	typedef LexemeValue Value;

  public:
  	Lexeme();

  	const Value&	getValue() const;
  	const Context&	getContext() const;
  	Id				getId() const;
  	  
	void 			make(const Context& context, Id id);
	void 			makeString(const Context& context, Id id, const String& value);
	void 			makeFloat(const Context& context, Id id, Float value);
	void 			makeInt(const Context& context, Id id, Int value);
	
  private:		
  	Lexeme(const Lexeme&);
  	void operator=(const Lexeme&);

	boost::shared_ptr<Value>	_value;
};

}} // namespace idl::scaner

#endif //Lexeme_hpp
