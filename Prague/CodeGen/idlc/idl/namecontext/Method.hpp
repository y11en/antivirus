#ifndef namecontext_Method_hpp
#define namecontext_Method_hpp

#include "Value.hpp"

namespace idl {

class Method : public NameContext {
  public:
  	Method();
  
  	typedef boost::intrusive_ptr<Method> Ptr;
  	
	typedef std::vector<Value::Weak> Args;

	virtual void addChild(NameContext::Ptr child);
	
	void setReturnType(Type::Ptr type);

	bool isVarArg() const;
	void setVarArg();
	
	Type::Ptr	getReturnType() const;
  	const Args& getArgs() const;
  	
  private:
  	void addArg(Value::Ptr value);

  	bool		_varArg;  	
  	Args		_args;
  	Type::Weak	_returnType;
  	
  private:
};

} //namespace idl

#endif //namecontext_Method_hpp
