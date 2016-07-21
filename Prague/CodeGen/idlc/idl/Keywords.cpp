#include "Context.hpp"
#include "Keywords.hpp"

namespace idl { namespace scaner {

Lexeme::Id Keywords::getId(const std::string& str) {
	ASSERT(_instance._index.find(str) != _instance._index.end());
	Lexeme::Id id = _instance._index[str];
	ASSERT(_instance._reverseIndex.find(id) != _instance._reverseIndex.end());
	return id;
}

const std::string& Keywords::getStr(Lexeme::Id id) {
	ASSERT(_instance._reverseIndex.find(id) != _instance._reverseIndex.end());
	const std::string& str = _instance._reverseIndex[id];
	ASSERT(_instance._index.find(str) != _instance._index.end());
	return str;
}

bool Keywords::exists(const std::string& str) {
	return _instance._index.find(str) != _instance._index.end();
}
  	
Keywords Keywords::_instance;

Keywords::Keywords() {
	defineKeywords();
}

void Keywords::defineKeyword(const std::string& str, Lexeme::Id id) {
	ASSERT(_index.find(str) == _index.end());
	ASSERT(_reverseIndex.find(id) == _reverseIndex.end());
	
	_index[str] = id;
	_reverseIndex[id] = str;
}

void Keywords::defineKeywords() {
	ASSERT(_index.empty());
	ASSERT(_reverseIndex.empty());
	
	#define DEFINE_KEYWORD(str, id)		defineKeyword(str, Lexeme::Ids::id);
	#include <../keywords.h>

}

}} //namespace idl::scaner
