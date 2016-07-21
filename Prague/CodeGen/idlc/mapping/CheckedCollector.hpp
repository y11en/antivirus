#ifndef mapping_CheckedCollector_hpp
#define mapping_CheckedCollector_hpp

#include "Collector.hpp"

namespace idl {

class CheckedCollector : public Collector {
  public:
	virtual void constDecl(const Attributes& , ConstValue::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void typedefDecl(const Attributes& , Typedef::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void structDecl(const Attributes& , Struct::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void enumDecl(const Attributes& , Enum::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void interfaceOpen(const Attributes& , Interface::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void interfaceClose(const Attributes& , Interface::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void methodDecl(const Attributes& , Method::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void externMethodDecl(const Attributes& , ExternMethod::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void componentOpen(const Attributes& , Component::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void componentClose(const Attributes& , Component::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void implementationOpen(const Attributes& , Implementation::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void implementationClose(const Attributes& , Implementation::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void sysMethodDecl(const Attributes& , NameContext::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void propertyDecl(const Attributes& , Property::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void propertyPluginDecl(const Attributes& , Property::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void propertyPluginAddDecl(const Attributes& , Property::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void messageDecl(const Attributes& , Message::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void messageClassDecl(const Attributes& , MessageClass::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void messageClassExtendDecl(const Attributes& , MessageClassExtend::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
	
	virtual void errorDecl(const Attributes& , NameContext::Ptr ) {
		ASSERT(false && "must not be reachable");
	}

	virtual void forwardIFace( NameContext::Ptr context ) {
		ASSERT(false && "must not be reachable");
	}

	virtual void forwardStruct( NameContext::Ptr context ) {
		ASSERT(false && "must not be reachable");
	}

	virtual void typecast( NameContext::Ptr context ) {
		ASSERT(false && "must not be reachable");
	}

	virtual void externalTypeDecl(ExternalType::Ptr ) {
		ASSERT(false && "must not be reachable");
	}
		        
  	virtual void importFile(const std::string& ) {
		ASSERT(false && "must not be reachable");
  	}
  	  
  private:
};

}

#endif //mapping_CheckedCollector_hpp
