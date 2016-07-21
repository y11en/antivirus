#ifndef LexemeValue_hpp
#define LexemeValue_hpp

#include "types.hpp"
#include "Context.hpp"
#include "Lexeme.hpp"

namespace idl { namespace scaner {

class LexemeValue {
  public:
  	typedef idl::parser::Context  Context;
  	typedef Lexeme::Id            Id;
  	
  	const Context&  getContext() const;

  	Id             getId() const;

		const String&  getString() const;
		Float          getFloat() const;
		Int            getInt() const;

		LexemeValue();
		
		LexemeValue( const Context& context, Id id );
		LexemeValue( const Context& context, Id id, const String& value );
		LexemeValue( const Context& context, Id id, Float value );
		LexemeValue( const Context& context, Id id, Int value );
			
  private:
  	Context _context;
  	Id      _id;
  
  	String  _string;
  	Float   _float;
  	Int     _int;
};

}} // namespace idl::scaner

#endif //LexemeValue_hpp
