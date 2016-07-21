#include "idl/Context.hpp"
#include "NameContext.hpp"

namespace idl {

using parser::NodeInterface;

NameContext::NameContext() {
}


NameContext::~NameContext() {
}

const NameContext::Children& NameContext::getChildren() const {
	return _children;
}
		
const parser::Context& NameContext::getDeclarationContext() const {
	return _declarationContext;
}

parser::NodeInterface::Ptr NameContext::getDeclarationPoint() {
	return _declarationPoint.lock().get();
}

void NameContext::setDeclaration(NodeInterface::Ptr declarationPoint) {
	_declarationContext = declarationPoint->getContext();
	_declarationPoint = declarationPoint->getWeak();
}

void NameContext::addChild(Ptr child) {
	_children.push_back(child);
}

const NamePath&	NameContext::getNamePath() const {
	return _path;
}

void NameContext::setNamePath( const NamePath& parentPath, const Name& name, const Name& alt ) {
	_path = parentPath;
	_path.push_back(name);
	_alt = alt;
}
	
const Name&	NameContext::getRealName() const {
	if ( _alt.empty() )
		return getCoreName();
	return _alt;
}

const Name&	NameContext::getCoreName() const {
	ASSERT(_path.size() > 0);
	return _path.last();
}

NameContext::Ptr NameContext::find( const Name& name, const Name& alt ) const {
	Children::const_iterator i = _children.begin();
	Children::const_iterator end = _children.end();
	for (; i != end; ++i) {
		NameContext::Ptr nCtx = *i;
		if ( nCtx->checkName(name,alt) )
			return nCtx;
	}
	return NULL;
}

const Name& NameContext::getAltName() const {
	return _alt;
}

// ---
bool NameContext::checkName( const Name& name, const Name& alt ) const {
	const Name& c_name = getCoreName();
	if ( (c_name == name) || (c_name == alt) )
		return true;
	if ( !alt.empty() ) {
		const Name& a_name = getAltName();
		if ( (a_name == name) || (a_name == alt) )
			return true;
	}
	return false;
}

} //namespace idl
