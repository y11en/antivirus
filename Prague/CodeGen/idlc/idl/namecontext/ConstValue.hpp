#ifndef namecontext_ConstValue_hpp
#define namecontext_ConstValue_hpp

#include "ConstEval.hpp"
#include "Value.hpp"

namespace idl {

class ConstValue : public Value {
  public:
  	typedef boost::intrusive_ptr<ConstValue> Ptr;

  	//ConstValue();
  	  	
  	void setPerformer(Performer::Ptr performer);
  	Performer::Ptr getPerformer() const;
  	
  private:
  	Performer::Ptr	_performer;
};

} //namespace idl

#endif //namecontext_ConstValue_hpp
