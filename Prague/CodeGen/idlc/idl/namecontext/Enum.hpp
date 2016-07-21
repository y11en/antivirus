#ifndef namecontext_Enum_hpp
#define namecontext_Enum_hpp

#include <vector>

#include "Type.hpp"
#include "Value.hpp"

namespace idl {

class Enum : public Type {
  public:
  	typedef boost::intrusive_ptr<Enum>	Ptr;
	typedef std::vector<Value::Weak> 	Entries;
    
	virtual void addChild(NameContext::Ptr child);
  	virtual size_t getSize() const;
  	const Entries& getEntries() const;
  	
  private:
  	void addEntry(Value::Ptr value);
  	
  	Entries	_entries;
};

} //namespace idl

#endif //namecontext_Enum_hpp
