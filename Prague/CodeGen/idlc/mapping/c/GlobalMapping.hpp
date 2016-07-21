#ifndef mapping_GlobalMapping_hpp
#define mapping_GlobalMapping_hpp

#include "mapping/Collector.hpp"

#include "CommonMapping.hpp"

#include <vector>

#define PRAGUE_STD_HEADERS              \
				"#include <Prague/prague.h>\n"  \
				"#include <Prague/pr_vtbl.h>\n" \
				"#include <Prague/pr_idl.h>\n"

namespace idl {

class GlobalMapping : public Redirector {
  public:
  	GlobalMapping(Generator& generator);

  	void start();
  	void stop();
  	
  public:
	virtual void constDecl(const Attributes& /*attrs*/, ConstValue::Ptr context);
	virtual void typedefDecl(const Attributes& /*attrs*/, Typedef::Ptr context);
	virtual void structDecl(const Attributes& /*attrs*/, Struct::Ptr context);
	virtual void enumDecl(const Attributes& /*attrs*/, Enum::Ptr context);
	virtual void forwardIFace( NameContext::Ptr context );
	virtual void forwardStruct( NameContext::Ptr context );
	virtual void typecast( NameContext::Ptr context );
  
  
	virtual void externalTypeDecl(ExternalType::Ptr context);
	
  	virtual void importFile(const std::string& file);
  	
  private:
  	CommonMappingC		_cMapping;
  	CommonMappingCpp	_cppMapping;
};

} //namespace idl

#endif //mapping_GlobalMapping_hpp
