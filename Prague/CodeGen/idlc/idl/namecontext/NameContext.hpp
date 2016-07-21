#ifndef namecontext_NameContext_hpp
#define namecontext_NameContext_hpp

#include <vector>
#include <boost/intrusive_ptr.hpp>

#include "idl/types.hpp"
#include "idl/NodeInterface.hpp"
#include "idl/NameNode.hpp"

namespace idl {

class NameContext : public RefCountBase {
public:
	typedef boost::intrusive_ptr<NameContext>	Ptr;
	typedef std::vector<Ptr>					Children;

	NameContext();
	virtual ~NameContext();

	const Children& getChildren() const;
	const parser::Context& getDeclarationContext() const;
	parser::NodeInterface::Ptr getDeclarationPoint();

	void setDeclaration(parser::NodeInterface::Ptr declarationPoint);

	virtual void    addChild(Ptr child);

	const NamePath&	getNamePath() const;

	void            setNamePath( const NamePath& parentPath, parser::NameNode::Ptr nameNode ) { setNamePath(parentPath,nameNode->getCoreName(),nameNode->getAltName()); }
	void            setNamePath( const NamePath& parentPath, const Name& name, const Name& alt );

	const Name&     getRealName() const;
	const Name&     getCoreName() const;
	const Name&     getAltName() const;
	bool            checkName( parser::NameNode::Ptr nameNode ) const;
	bool            checkName( NameContext::Ptr      ctx ) const;
	bool            checkName( const Name& name, const Name& alt ) const;

	Ptr             find( parser::NameNode::Ptr nn ) const { return find(nn->getCoreName(),nn->getAltName()); }
	Ptr             find( NameContext::Ptr ctx )     const { return find(ctx->getCoreName(),ctx->getAltName()); }
	virtual Ptr     find( const Name& name, const Name& alt ) const;

private:
	Children				_children;

	parser::Context             _declarationContext;
	parser::NodeInterface::Weak _declarationPoint;
	NamePath                    _path;
	Name                        _alt;
};

// ---
inline bool NameContext::checkName( parser::NameNode::Ptr nameNode ) const {
	return checkName( nameNode->getCoreName(), nameNode->getAltName() );
}

// ---
inline bool NameContext::checkName( NameContext::Ptr ctx ) const {
	return checkName( ctx->getCoreName(), ctx->getAltName() );
}

} //namespace idl

#endif //namecontext_NameContext_hpp
