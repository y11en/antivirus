#ifndef NameNode_hpp
#define NameNode_hpp

#include "NodeInterface.hpp"

#include <vector>

namespace idl { namespace parser {

class NameNode : public NodeBase<NameNode> {
public:
	NameNode( const Context& context, const Name& name );

	const Name& getRealName() const;
	const Name& getCoreName() const;
	const Name& getAltName()  const;
	void        setAltName( const Name& name );

private:
	Name	_name;
	Name  _alt;
};

}} //namespace idl::parser

#endif //NameNode_hpp
