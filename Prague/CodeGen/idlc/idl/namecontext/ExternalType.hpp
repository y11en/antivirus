#ifndef namecontext_ExternalType_hpp
#define namecontext_ExternalType_hpp

#include "Type.hpp"

namespace idl {

class ExternalType : public Type {
  public:
  	typedef boost::intrusive_ptr<ExternalType>	Ptr;
  	virtual size_t getSize() const;

  	void setFile(const std::string& file);
  	const std::string& getFile() const;
  	
  private:
  	std::string		_file;
};

} //namespace idl

#endif //namecontext_ExternalType_hpp
