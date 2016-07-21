#ifndef ValueFactory_hpp
#define ValueFactory_hpp

//#include "LexemeValue.hpp"
#include "Context.hpp"

#include <vector>
#include <boost/shared_ptr.hpp>

#include "NodeInterface.hpp"


namespace idl { namespace scaner {
	class LexemeValue;
}}

namespace idl { namespace parser {

struct Value {
	typedef boost::shared_ptr<idl::scaner::LexemeValue> LexemeValue;
	Context             _context;
	LexemeValue         _lexeme;
	NodeInterface::Ptr  _node;

/*	friend class ValueFactory;
	
  private:
  	Value() {}
  	Value(const Value&);
  	void operator=(const Value&);
  	~Value() {}*/
};

class ValueFactory {
public:
	static ValueFactory& instance();

	Value*    createValue();
	Context*  createContext();
	void      clear();
	~ValueFactory();

private:
	std::vector<Value*>   _values;
	std::vector<Context*> _contexts;
};

}} //namespace idl::parser

#endif //ValueFactory_hpp
