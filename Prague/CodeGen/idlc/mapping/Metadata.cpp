#include "Context.hpp"
#include "Metadata.hpp"

#include <vector>
#include <algorithm>

#include <boost/cstdint.hpp>

#include "md5.hpp"

static void md5Sign(std::vector<char>& data) {
	using namespace adobe;
	md5_t::digest_t digest = md5(&data[0], data.size());
	std::copy(digest.data(), digest.data() + digest.size(), std::back_inserter(data));
}

namespace idl {

using parser::AttributeNode;

template <class T>
void dumpStruct(std::vector<char>& data, const T& t) {
	const char* begin = reinterpret_cast<const char*>(&t);
	const char* end = begin + sizeof(T);
	std::copy(begin, end, std::back_inserter(data));
}
  	
parser::AttributeNode::Ptr findAttr(const Collector::Attributes& attrs, parser::AttributeNode::AttributeId id/*, bool ensure*/) {
	BOOST_FOREACH(parser::AttributeNode::Ptr attr, attrs) {
		if (attr->getId() == id) {
			return attr;
		}
	}
	//if (ensure) {
		ASSERT(false && "required attribute not found");
	//}
	return NULL;
}

static boost::uint32_t findId(const Collector::Attributes& attrs) {
	return static_cast<boost::uint32_t>(findAttr(attrs, AttributeNode::Id)->getInt());
}

Metadata::Header::Header() {
	static const char prague[] = "Prague";
	static const char metadata[] = "metadata";
	
	ASSERT(sizeof(_prague) == sizeof(prague));
	ASSERT(sizeof(_metadata) == sizeof(metadata));
	
	memcpy(_prague, prague, sizeof(prague));
	memcpy(_metadata, metadata, sizeof(metadata));
}

void Metadata::saveAttrs(const Attributes& attrs, NameContext::Ptr context) {
	ASSERT(_attrsMap.find(context) == _attrsMap.end());
	_attrsMap[context] = attrs;
}

  
void Metadata::constDecl(const Attributes& attrs, ConstValue::Ptr context) {
	//saveAttrs(attrs, context);
}

void Metadata::typedefDecl(const Attributes& attrs, Typedef::Ptr context) {
	//saveAttrs(attrs, context);
}

void Metadata::forwardIFace( NameContext::Ptr context ) {
	//saveAttrs(attrs, context);
}

void Metadata::forwardStruct( NameContext::Ptr context ) {
	//saveAttrs(attrs, context);
}

void Metadata::typecast( NameContext::Ptr context) {
	//saveAttrs(attrs, context);
}

void Metadata::structDecl(const Attributes& attrs, Struct::Ptr context) {
	//saveAttrs(attrs, context);
}

void Metadata::enumDecl(const Attributes& attrs, Enum::Ptr context) {
	//saveAttrs(attrs, context);
}

void Metadata::interfaceOpen(const Attributes& attrs, Interface::Ptr context) {
	saveAttrs(attrs, context);
}

void Metadata::interfaceClose(const Attributes& attrs, Interface::Ptr context) {
}

void Metadata::methodDecl(const Attributes& attrs, Method::Ptr context) {
	//saveAttrs(attrs, context);
}

void Metadata::externMethodDecl(const Attributes& attrs, ExternMethod::Ptr context) {
	saveAttrs(attrs, context);
	
	_plugin._exportFuncsNumber += 1;
	_exportFuncs.push_back(findId(attrs));
}

void Metadata::componentOpen(const Attributes& attrs, Component::Ptr context) {
	saveAttrs(attrs, context);
	
	_plugin._implementationNumber = 0;
	_plugin._exportFuncsNumber = 0;
	_plugin._autoload = (findAttr(attrs, AttributeNode::Autostart)->getInt() != 0);
	_plugin._pluginId = findId(attrs);
	_plugin._pluginCodepage = 0;
	
	_pluginVersion = static_cast<UInt32>(findAttr(attrs, AttributeNode::Version)->getInt());
}

void Metadata::componentClose(const Attributes& attrs, Component::Ptr context) {
}

void Metadata::implementationOpen(const Attributes& attrs, Implementation::Ptr context) {
	saveAttrs(attrs, context);
	
	_implementations.push_back(ImplementationDescPair());
	
	_implementations.back()._implementationDesc._staticPropsNumber = 0;
	_implementations.back()._implementationDesc._pluginId = _plugin._pluginId;
	_implementations.back()._implementationDesc._subtypeId = 0;
	_implementations.back()._implementationDesc._pluginVersion = _pluginVersion;
	_implementations.back()._implementationDesc._implementationId = findId(attrs);
	_implementations.back()._implementationDesc._pluginFlags = 0;
	_implementations.back()._implementationDesc._compatibleInterfaceId = 0;
	
	ASSERT(context->getBases().size() == 1);
	Interface::Ptr interface = context->getBases().front();
	ASSERT(interface->getBases().size() <= 1);
	if (interface->getBases().size() != 0) {
		Interface::Ptr compatible = interface->getBases().front();
		_implementations.back()._implementationDesc._compatibleInterfaceId = findId(_attrsMap[compatible]);
	}
	
	_implementations.back()._implementationDesc._interfaceId = findId(_attrsMap[interface]);
	_implementations.back()._implementationDesc._pluginCodepage = 0;
}

void Metadata::implementationClose(const Attributes& attrs, Implementation::Ptr context) {
	_plugin._implementationNumber += 1;
}

void Metadata::sysMethodDecl(const Attributes& attrs, NameContext::Ptr context) {
	saveAttrs(attrs, context);
}

void Metadata::propertyDecl(const Attributes& attrs, Property::Ptr context) {
	saveAttrs(attrs, context);
}

void Metadata::propertyPluginDecl(const Attributes& attrs, Property::Ptr context) {
	//saveAttrs(attrs, context);
	
	if (findAttr(attrs, AttributeNode::Shared)->getInt() != 0) {
		_implementations.back()._implementationDesc._staticPropsNumber += 1;
		_implementations.back()._sharedProps.push_back(SharedPropDesc(findId(_attrsMap[context])));
	}
}

void Metadata::propertyPluginAddDecl(const Attributes& attrs, Property::Ptr context) {
	saveAttrs(attrs, context);
	propertyPluginDecl(attrs, context);
}

void Metadata::messageDecl(const Attributes& attrs, Message::Ptr context) {
	//saveAttrs(attrs, context);
}

void Metadata::messageClassDecl(const Attributes& attrs, MessageClass::Ptr context) {
	//saveAttrs(attrs, context);
}

void Metadata::messageClassExtendDecl(const Attributes& attrs, MessageClassExtend::Ptr context) {
	//saveAttrs(attrs, context);
}

void Metadata::errorDecl(const Attributes& attrs, NameContext::Ptr context) {
	//saveAttrs(attrs, context);
}

void Metadata::externalTypeDecl(ExternalType::Ptr context) {
}
				
void Metadata::importFile(const std::string& file) {
}

void Metadata::dump(std::vector<char>& data) const {
	if (_plugin._pluginId == 0) {
		return;
	}

	dumpStruct(data, _header);
	dumpStruct(data, _plugin);
	
	ASSERT(_plugin._exportFuncsNumber == _exportFuncs.size());
	
	BOOST_FOREACH(const ExportFuncDesc& ef, _exportFuncs) {
		dumpStruct(data, ef);
	}

	ASSERT(_plugin._implementationNumber == _implementations.size());
	  		
	BOOST_FOREACH(const ImplementationDescPair& i, _implementations) {
		dumpStruct(data, i._implementationDesc);
		
		BOOST_FOREACH(const SharedPropDesc& sp, i._sharedProps) {
			ASSERT(sp._fixedPart._size == sp._value.size());
			ASSERT(sp._fixedPart._size == 0);
			dumpStruct(data, sp._fixedPart);
			//std::copy(sp._value.begin(), sp._value.end(), std::back_inserter(data));
		}
	}
	
	md5Sign(data);
}

}
