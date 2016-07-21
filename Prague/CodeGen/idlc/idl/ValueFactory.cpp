#include "ValueFactory.hpp"

#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/range.hpp>
#include <boost/checked_delete.hpp>

namespace idl { namespace parser {

ValueFactory& ValueFactory::instance() {
	static ValueFactory _factory;
	return _factory;
}

Value* ValueFactory::createValue() {
	_values.push_back(new Value);
	return _values.back();
}

Context* ValueFactory::createContext() {
	_contexts.push_back(new Context);
	return _contexts.back();
}

void 	ValueFactory::clear() {
	using namespace boost::lambda;
	std::for_each(_values.begin(), _values.end(), delete_ptr());
	std::for_each(_contexts.begin(), _contexts.end(), delete_ptr());
}

ValueFactory::~ValueFactory() {
	clear();
}


}} //namespace idl::parser
