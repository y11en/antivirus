#ifndef namecontext_BasicType_hpp
#define namecontext_BasicType_hpp

#include "NameContext.hpp"
#include "Type.hpp"

#include "idl/BasicTypeNode.hpp"

namespace idl {

class BasicType : public Type {
  public:
  	typedef boost::intrusive_ptr<BasicType>	Ptr;
  	typedef parser::BasicTypeNode::TypeId TypeId;
  
  	BasicType(TypeId typeId);

  	virtual size_t getSize() const;
	TypeId getTypeId() const;

	bool isSigned() const;
	bool isUnsigned() const;
	bool isInt() const;
	bool isFloat() const;

	static size_t getSize(TypeId typeId);
	static scaner::Int getMask(TypeId typeId);
		  	  	
	static bool isSigned(TypeId typeId);
	static bool isUnsigned(TypeId typeId);
	static bool isInt(TypeId typeId);
	static bool isFloat(TypeId typeId);
	  	  	
  private:
  	TypeId _typeId;
};

} //namespace idl

#endif //namecontext_BasicType_hpp
