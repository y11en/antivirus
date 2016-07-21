#ifndef namecontext_Sequence_hpp
#define namecontext_Sequence_hpp

#include <map>

#include "Type.hpp"
#include "SequenceType.hpp"

namespace idl {

class Sequence : public Type {
  public:
  	typedef boost::intrusive_ptr<Sequence>		Ptr;
	Sequence();
	    
	SequenceType::Ptr add(Type::Ptr type, size_t size);
	
  	virtual size_t getSize() const;
  	//const Entries& getEntries() const;
  	
  private:
  	typedef std::pair<Type::Weak, size_t>		Key;
	typedef std::map<Key, SequenceType::Ptr>	Params;

  	Params	_params;
};

} //namespace idl

#endif //namecontext_Sequence_hpp
