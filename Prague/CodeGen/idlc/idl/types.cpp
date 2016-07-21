#include "types.hpp"

#include <algorithm>
#include <boost/algorithm/string.hpp>

namespace idl {

std::string NamePath::str() const {
	if (_path.size() == 0) {
		return std::string();
	}

	Names::const_iterator i = _path.begin(), end = _path.end();
	
	std::string result = *i;

	++i;
		  		
	for (; i != end; ++i) {
		result += '.';
		result += *i;
	}
	return result;
}

NamePath NamePath::make(const std::string& str) {
	std::vector<std::string>	names;
    boost::split(names, str, boost::is_any_of("."), boost::token_compress_on);
    NamePath result;
    std::copy(names.begin(), names.end(), std::back_inserter(result._path));
    return result;
}

} //namespace idl