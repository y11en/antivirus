#ifndef Parser_hpp
#define Parser_hpp

#include <string>
#include <istream>

#include <boost/shared_ptr.hpp>

#include "NodeInterface.hpp"

namespace idl { namespace scaner {
	class Scaner;
}}

namespace idl { namespace parser {

class Context;

class Parser {
  public:
  	Parser();
  
	NodeInterface::Ptr parse(boost::shared_ptr<idl::scaner::Scaner> scaner);
	NodeInterface::Ptr parse(const std::string& file);
	NodeInterface::Ptr parse(const std::string& name, boost::shared_ptr<std::istream> stream);
  	void error(const Context& context, const std::string& message);
  	
   private:
   	boost::shared_ptr<idl::scaner::Scaner>	_scaner;
};


}} //namespace idl::parser

#endif //Parser_hpp
