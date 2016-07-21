#ifndef Context_hpp
#define Context_hpp

#include <string>

namespace idl { namespace parser {

class Context {

public:
	Context();
	Context( const std::string& source, int firstLine, int firstColon, int lastLine, int lastColon, const std::string& text = "" );

	const std::string&	getSource() const;
	const std::string&  getText() const;
	int                 getFirstLine() const;
	int                 getFirstColon() const;
	int                 getLastLine() const;
	int                 getLastColon() const;

	std::string         fullContextInfo() const;
	std::string         mediumContextInfo() const;
	std::string         shortContextInfo() const;
	
	//static Context join(const Context& first, const Context& last);
		
  private:
  	std::string  _source;
  	std::string  _text;
  	int          _firstLine;
  	int          _lastLine;
  	int          _firstColon;
  	int          _lastColon;
};

extern std::ostream& operator << ( std::ostream& stream, const Context& context );

}} //namespace idl::parser

#endif //Context_hpp
