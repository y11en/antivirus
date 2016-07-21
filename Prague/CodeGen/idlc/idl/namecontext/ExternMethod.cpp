#include "Context.hpp"
#include "ExternMethod.hpp"

//#include <boost/crc.hpp>

namespace idl {
/*
static unsigned int getHash(const NamePath& path) {
	std::string name = path.str();
	
	boost::crc_32_type crc;
	
	crc.process_bytes(&name[0], name.size());
	
	return static_cast<unsigned int>(crc.checksum());
}

ExternMethod::Id ExternMethod::getId() const {
	return getHash(getNamePath());
}
*/
} //namespace idl
