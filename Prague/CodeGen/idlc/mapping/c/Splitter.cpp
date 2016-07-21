#include "Context.hpp"

#include "Splitter.hpp"

namespace idl {

template <class ContextPtr>
void Splitter::redirect(const Attributes& attrs, typename ContextPtr context, void (Collector::*call)(const Attributes&, typename ContextPtr)) {
	ASSERT(!_collectors.empty());
	//BOOST_FOREACH(Collector* collector, _collectors) {
	Collectors::iterator curr = _collectors.begin();
	for( ; curr != _collectors.end(); ++curr ) {
		Collector* collector = *curr;
		(collector->*call)( attrs, context );
	}
}

void Splitter::addCollector(Collector* collector) {
	_collectors.push_back(collector);
}

void Splitter::flushCollectors() {
	_collectors.clear();
}

void Splitter::constDecl(const Attributes& attrs, ConstValue::Ptr context) {
	redirect(attrs, context, &Collector::constDecl);
}

void Splitter::typedefDecl(const Attributes& attrs, Typedef::Ptr context) {
	redirect(attrs, context, &Collector::typedefDecl);
}

void Splitter::structDecl(const Attributes& attrs, Struct::Ptr context) {
	redirect(attrs, context, &Collector::structDecl);
}

void Splitter::enumDecl(const Attributes& attrs, Enum::Ptr context) {
	redirect(attrs, context, &Collector::enumDecl);
}

void Splitter::interfaceOpen(const Attributes& attrs, Interface::Ptr context) {
	redirect(attrs, context, &Collector::interfaceOpen);
}

void Splitter::interfaceClose(const Attributes& attrs, Interface::Ptr context) {
	redirect(attrs, context, &Collector::interfaceClose);
}

void Splitter::methodDecl(const Attributes& attrs, Method::Ptr context) {
	redirect(attrs, context, &Collector::methodDecl);
}

void Splitter::externMethodDecl(const Attributes& attrs, ExternMethod::Ptr context) {
	redirect(attrs, context, &Collector::externMethodDecl);
}

void Splitter::componentOpen(const Attributes& attrs, Component::Ptr context) {
	redirect(attrs, context, &Collector::componentOpen);
}

void Splitter::componentClose(const Attributes& attrs, Component::Ptr context) {
	redirect(attrs, context, &Collector::componentClose);
}

void Splitter::implementationOpen(const Attributes& attrs, Implementation::Ptr context) {
	redirect(attrs, context, &Collector::implementationOpen);
}

void Splitter::implementationClose(const Attributes& attrs, Implementation::Ptr context) {
	redirect(attrs, context, &Collector::implementationClose);
}

void Splitter::sysMethodDecl(const Attributes& attrs, NameContext::Ptr context) {
	redirect(attrs, context, &Collector::sysMethodDecl);
}

void Splitter::propertyDecl(const Attributes& attrs, Property::Ptr context) {
	redirect(attrs, context, &Collector::propertyDecl);
}

void Splitter::propertyPluginDecl(const Attributes& attrs, Property::Ptr context) {
	redirect(attrs, context, &Collector::propertyPluginDecl);
}

void Splitter::propertyPluginAddDecl(const Attributes& attrs, Property::Ptr context) {
	redirect(attrs, context, &Collector::propertyPluginAddDecl);
}

void Splitter::messageDecl(const Attributes& attrs, Message::Ptr context) {
	redirect(attrs, context, &Collector::messageDecl);
}

void Splitter::messageClassDecl(const Attributes& attrs, MessageClass::Ptr context) {
	redirect(attrs, context, &Collector::messageClassDecl);
}

void Splitter::messageClassExtendDecl(const Attributes& attrs, MessageClassExtend::Ptr context) {
	redirect(attrs, context, &Collector::messageClassExtendDecl);
}

void Splitter::errorDecl(const Attributes& attrs, NameContext::Ptr context) {
	redirect(attrs, context, &Collector::errorDecl);
}

void Splitter::externalTypeDecl(ExternalType::Ptr context) {
	ASSERT(!_collectors.empty());
	BOOST_FOREACH(Collector* collector, _collectors) {
		collector->externalTypeDecl(context);
	}
}
	        
void Splitter::importFile(const std::string& file) {
	ASSERT(!_collectors.empty());
	BOOST_FOREACH(Collector* collector, _collectors) {
		collector->importFile(file);
	}
}

void Splitter::forwardIFace( NameContext::Ptr context ) {
	ASSERT(!_collectors.empty());
	BOOST_FOREACH(Collector* collector, _collectors) {
		collector->forwardIFace(context);
	}
}

void Splitter::forwardStruct( NameContext::Ptr context ) {
	ASSERT(!_collectors.empty());
	BOOST_FOREACH(Collector* collector, _collectors) {
		collector->forwardStruct(context);
	}
}

void Splitter::typecast( NameContext::Ptr context ) {
	ASSERT(!_collectors.empty());
	BOOST_FOREACH(Collector* collector, _collectors) {
		collector->typecast(context);
	}
}

} //namespace idl
