#ifndef namecontext_Component_hpp
#define namecontext_Component_hpp

#include "NameContext.hpp"

namespace idl {

class Component : public NameContext {
  public:
  	typedef boost::intrusive_ptr<Component>	Ptr;
  	typedef scaner::Int	ComponentId;
  	
  	//ComponentId getComponentId() const;
  	//void setComponentId(ComponentId id);
  	  
  private:	
  	//ComponentId	_componentId;
};

} //namespace idl

#endif //namecontext_Component_hpp
