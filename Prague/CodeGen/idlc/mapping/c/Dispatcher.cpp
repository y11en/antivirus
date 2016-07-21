#include "Context.hpp"

#include "Dispatcher.hpp"
#include "Generator.hpp"

namespace idl {

Dispatcher::Dispatcher(Generator& generator) :
		_globalMapping(generator),           
								  
		_pluginMapping(generator),           
								  
		_interfaceMapping(generator),        
		_interfaceMappingCpp(generator),     
								  
		_implementationMapping(generator),   
		_implementationMappingCpp(generator) {
								  
	/*_globalMapping 				= new GlobalMapping(generator);
	_globalMappingCpp 			= new GlobalMappingCpp(generator);

	_pluginMapping 				= new PluginMappng(generator);
	
	_interfaceMapping 			= new InterfaceMapping(generator);
	_interfaceMappingCpp 		= new InterfaceMappingCpp(generator);
			
	_implementationMapping 		= new ImplementationMapping(generator);
	_implementationMappingCpp 	= new ImplementationMappingCpp(generator);*/
	
	_globalSplitter.addCollector(&_globalMapping);
	
	_pluginSplitter.addCollector(&_pluginMapping);
	
	_interfaceSplitter.addCollector(&_interfaceMapping);
	_interfaceSplitter.addCollector(&_interfaceMappingCpp);
	
	_implementationSplitter.addCollector(&_implementationMapping);
	_implementationSplitter.addCollector(&_implementationMappingCpp);
}

void Dispatcher::start() {
	ASSERT(_splitterStack.empty());
	pushSplitter(&_globalSplitter);
	_globalMapping.start();
}

void Dispatcher::stop() {
	_globalMapping.stop();
	popSplitter();
	ASSERT(_splitterStack.empty());
}

Splitter* Dispatcher::splitter() {
	return _splitterStack.top();
}

void Dispatcher::pushSplitter(Splitter* s) {
	_splitterStack.push(s);
	
	flushCollectors();
	addCollector(splitter());
}

void Dispatcher::popSplitter() {
	ASSERT(!_splitterStack.empty());
	_splitterStack.pop();

	flushCollectors();
	if (!_splitterStack.empty()) {
		addCollector(splitter());
	}
}

void Dispatcher::interfaceOpen(const Attributes& attrs, Interface::Ptr context) {
	pushSplitter(&_interfaceSplitter);
	splitter()->interfaceOpen(attrs, context);
}

void Dispatcher::interfaceClose(const Attributes& attrs, Interface::Ptr context) {
	splitter()->interfaceClose(attrs, context);
	popSplitter();
}

void Dispatcher::componentOpen(const Attributes& attrs, Component::Ptr context) {
	pushSplitter(&_pluginSplitter);
	splitter()->componentOpen(attrs, context);
}

void Dispatcher::componentClose(const Attributes& attrs, Component::Ptr context) {
	splitter()->componentClose(attrs, context);
	popSplitter();
}

void Dispatcher::implementationOpen(const Attributes& attrs, Implementation::Ptr context) {
	ASSERT(splitter() == &_pluginSplitter);

	Component::Ptr pluginContext = _pluginMapping.getPluginContext();
	
	ASSERT(pluginContext);
	
	_implementationMapping.setPluginContext(pluginContext);
	_implementationMappingCpp.setPluginContext(pluginContext);
	
	pushSplitter(&_implementationSplitter);
	splitter()->implementationOpen(attrs, context);
}

void Dispatcher::implementationClose(const Attributes& attrs, Implementation::Ptr context) {
	splitter()->implementationClose(attrs, context);
	_implementationMapping.setPluginContext(NULL);
	_implementationMappingCpp.setPluginContext(NULL);
	popSplitter();
	ASSERT(splitter() == &_pluginSplitter);
}
	
} //namespace idl
