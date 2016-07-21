#ifndef namecontext_Typedef_hpp
#define namecontext_Typedef_hpp

#include <vector>

#include "Type.hpp"
#include "Value.hpp"

namespace idl {

class Typedef : public Type {
  public:
  	typedef boost::intrusive_ptr<Typedef>	Ptr;
  	virtual size_t getSize() const;
  	
  	/**
  		Return 'real' type behind typedef. For instance: 
  			typedef s16	Type1;
  			typedef Type1 Type2;
  			
  		for 'Type2' reduce() returns s16.
  		
  		@return 'real' type (not a typedef)
  	*/
	//Type::Ptr Typedef::reduce();
  private:
};

class RootNameContext;

extern Type::Ptr reduceTypedef( boost::intrusive_ptr<RootNameContext> root, Type::Ptr type, bool check = true);
extern Type::Ptr reduceTypedef( Type::Ptr type, bool& isRef, bool& isConst, int& count, bool check);

} //namespace idl

#endif //namecontext_Typedef_hpp
