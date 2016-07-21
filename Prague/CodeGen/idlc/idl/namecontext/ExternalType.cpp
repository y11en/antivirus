#include "Context.hpp"
#include "ExternalType.hpp"

namespace idl {

size_t ExternalType::getSize() const {
	ASSERT(false && "not yet supported");
	return 0;
}

void ExternalType::setFile(const std::string& file) {
	_file = file;
}

const std::string& ExternalType::getFile() const {
	return _file;
}

} //namespace idl
