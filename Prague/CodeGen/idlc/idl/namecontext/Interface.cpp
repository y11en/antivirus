#include "idl/Context.hpp"
#include "Interface.hpp"

namespace idl {

Interface::Interface() {
	//_interfaceId = 0;
}

size_t Interface::getSize() const {
	ASSERT(false && "not yet supported");
	return 0;
}

void Interface::addBase(Interface::Ptr base) {
	ASSERT(base);
	_bases.push_back(base);
}

const Interface::Bases& Interface::getBases() const {
	return _bases;
}

NameContext::Ptr Interface::findWithinBases( const Name& name, const Name& alt ) const {
	//BOOST_FOREACH(Interface::Ptr i, _bases) {
	Bases::const_iterator i = _bases.begin();
	for( ; i != _bases.end(); ++i ) {
		Interface::Ptr   iface = *i;
		NameContext::Ptr result = iface->find( name, alt ); 
		
		if ( result )
			return result;

		result = iface->findWithinBases( name, alt );
		if ( result )
			return result;
	}
	
	return NULL;
}
  	
/*Interface::InterfaceId Interface::getInterfaceId() const {
	return _interfaceId;
}

void Interface::setInterfaceId(InterfaceId interfaceId) {
	_interfaceId = interfaceId;
}*/

} //namespace idl
