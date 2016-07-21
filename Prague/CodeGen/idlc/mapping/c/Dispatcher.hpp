#ifndef mapping_Dispatcher_hpp
#define mapping_Dispatcher_hpp

#include <stack>

#include "Splitter.hpp"

#include "GlobalMapping.hpp"
                        
#include "PluginMapping.hpp"
                        
#include "InterfaceMapping.hpp"
#include "InterfaceMappingCpp.hpp"
                        
#include "ImplementationMapping.hpp"
#include "ImplementationMappingCpp.hpp"

namespace idl {

class Generator;

class Dispatcher : public Splitter {
  public:	
	Dispatcher(Generator& generator);

	void start();
	void stop();

  private:
	Splitter* splitter();
	
	void pushSplitter(Splitter* splitter);
	void popSplitter();

	virtual void interfaceOpen(const Attributes& attrs, Interface::Ptr context);
	virtual void interfaceClose(const Attributes& attrs, Interface::Ptr context);
	virtual void componentOpen(const Attributes& attrs, Component::Ptr context);
	virtual void componentClose(const Attributes& attrs, Component::Ptr context);
	virtual void implementationOpen(const Attributes& attrs, Implementation::Ptr context);
	virtual void implementationClose(const Attributes& attrs, Implementation::Ptr context);
	
  private:
	Splitter	_globalSplitter;
	Splitter	_pluginSplitter;
	Splitter	_interfaceSplitter;
	Splitter	_implementationSplitter;
	
	GlobalMapping				_globalMapping;
	PluginMapping				_pluginMapping;
	InterfaceMapping			_interfaceMapping;
	InterfaceMappingCpp			_interfaceMappingCpp;
	ImplementationMapping		_implementationMapping;
	ImplementationMappingCpp	_implementationMappingCpp;

	std::stack<Splitter*>		_splitterStack;
};

} //namespace idl

#endif //mapping_Dispatcher_hpp
