#ifndef namecontext_Interface_hpp
#define namecontext_Interface_hpp

#include "Type.hpp"

namespace idl {

	class Interface : public Type {
	public:
		typedef scaner::Int	InterfaceId;
		typedef boost::intrusive_ptr<Interface>	Ptr;
		typedef std::vector<Interface::Ptr> Bases;

		Interface();

		virtual size_t getSize() const;

		void addBase(Interface::Ptr bases);
		const Bases& getBases() const;

		NameContext::Ptr findWithinBases( const Name& name, const Name& alt ) const;

		//InterfaceId getInterfaceId() const;
		//void setInterfaceId(InterfaceId id);

	private:	
		//InterfaceId	_interfaceId;
		Bases			_bases;
	};

} //namespace idl

#endif //namecontext_Interface_hpp
