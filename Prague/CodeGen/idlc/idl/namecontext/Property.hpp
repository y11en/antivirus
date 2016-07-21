#ifndef namecontext_Property_hpp
#define namecontext_Property_hpp

#include "ConstValue.hpp"

namespace idl {

class Property : public ConstValue {
  public:
		Property() : _optional(false) {}

  	typedef boost::intrusive_ptr<Property> Ptr;

		bool isOptional() const { return _optional; }
		void setOptional( bool val ) { _optional = val; }
  	
  private:
		bool _optional;
};

} //namespace idl

#endif //namecontext_Property_hpp
