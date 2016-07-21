#ifndef idl_Keywords_hpp
#define idl_Keywords_hpp

#include "Lexeme.hpp"

#include <map>
#include <string>

namespace idl { namespace scaner {

class Keywords {
  public:
  	static Lexeme::Id			getId(const std::string& str);
  	static const std::string&	getStr(Lexeme::Id id);
  	
  	static bool					exists(const std::string& str);

  private:
  	Keywords();
  	
	void defineKeyword(const std::string& str, Lexeme::Id id);
	void defineKeywords();

  	static Keywords	_instance;
  	
  	typedef std::map<std::string, Lexeme::Id> Index;
  	typedef std::map<Lexeme::Id, std::string> ReverseIndex;
  	
  	Index			_index;
  	ReverseIndex	_reverseIndex;
};

}} //namespace idl::scaner

#endif //idl_Keywords_hpp
