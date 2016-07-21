#ifndef namecontext_Struct_hpp
#define namecontext_Struct_hpp

#include <vector>

#include "Type.hpp"
#include "Value.hpp"

namespace idl {

class Struct : public Type {
  public:
  	typedef boost::intrusive_ptr<Struct>	Ptr;
	typedef std::vector<Value::Weak> 		Entries;
    
	virtual void addChild(NameContext::Ptr child);
  	virtual size_t getSize() const;
  	const Entries& getEntries() const;
  	
  private:
  	void addEntry(Value::Ptr value);
  	
  	Entries	_entries;
};

} //namespace idl

#endif //namecontext_Struct_hpp
