#ifndef namecontext_Vendor_hpp
#define namecontext_Vendor_hpp

#include "NameContext.hpp"

namespace idl {

class Vendor : public NameContext {
  public:
  	typedef boost::intrusive_ptr<Vendor>	Ptr;
  	typedef scaner::Int	VendorId;
  	
  	//VendorId getVendorId() const;
  	//void setVendorId(VendorId id);
  	  
  private:	
  	//VendorId	_vendorId;
};

} //namespace idl

#endif //namecontext_Vendor_hpp
