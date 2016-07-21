#ifndef namecontext_RootNameContext_hpp
#define namecontext_RootNameContext_hpp

#include <map>

#include "BasicType.hpp"
#include "Sequence.hpp"
#include "RefType.hpp"

namespace idl {

class RootNameContext : public NameContext {
  public:
  	typedef boost::intrusive_ptr<RootNameContext>	Ptr;
  	
  	RootNameContext();
  	BasicType::Ptr	getBasicType(BasicType::TypeId typeId) const;
	Sequence::Ptr	getSequence() const;
  	
	RefType::Ptr	addRefType(Type::Ptr type, int refCount, bool isConst);
	
  private:
  	typedef std::pair <int, bool> RefSubKey;
  	typedef std::pair < Type::Weak, RefSubKey > RefKey;
  	typedef std::map<RefKey, RefType::Ptr> RefMap;
  	
  
  	RefMap			_refMap;
  	
  	Sequence::Ptr	_sequence;
  	
  	BasicType::Ptr	_basicTypes[parser::BasicTypeNode::NumberOfTypes];
};

} //namespace idl

#endif //namecontext_RootNameContext_hpp
