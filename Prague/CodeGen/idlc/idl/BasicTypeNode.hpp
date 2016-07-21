#ifndef BasicTypeNode_hpp
#define BasicTypeNode_hpp

#include "NodeInterface.hpp"

#include <vector>

namespace idl { namespace parser {

class BasicTypeNode : public NodeBase<BasicTypeNode> {
  public:
  	enum TypeId {
		SInt8 		= 0,
		SInt16		,
		SInt32      ,
		SInt64		,
		UInt8		,
		UInt16		,
		UInt32		,
		UInt64		,
		Character	,
		Float		,
		Void		,
		NumberOfTypes,
	};

  	BasicTypeNode(const Context& context, TypeId typeId);

	TypeId getTypeId() const;
  
  private:
  	TypeId	_typeId;
};

}} //namespace idl::parser

#endif //BasicTypeNode_hpp
