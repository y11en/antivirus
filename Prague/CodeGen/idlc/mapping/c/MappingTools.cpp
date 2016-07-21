#include "Context.hpp"
#include "MappingTools.hpp"

#include "CommonMapping.hpp"

#include "namecontext/RootNameContext.hpp"

#include "mapping/tools.hpp"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

std::string basename(const std::string& file) {
	size_t slash = file.find_last_of("/\\");
	
	if (slash == std::string::npos) {
		return file;
	}
	
	return file.substr(slash + 1, file.length());
}

std::string changeExt(const std::string& file, const std::string& ext) {
	return file.substr(0, file.find_last_of('.')) + ext;
}

std::string makeInterfaceHeaderFile( const std::string idlFile ) {
	int slash = idlFile.find_last_of( "\\/" );
	int dot   = idlFile.find_last_of( '.' );
	int size;
	if ( dot < slash )
		dot = idlFile.length();
	if ( slash == std::string::npos )
		slash = 0;
	else
		++slash;
	size = dot - slash;
	std::string fname = idlFile.substr( slash, size );
	if ( !fname.compare("stdidl") || !fname.compare("types") )
		fname = "Prague/iface/i_" + fname + ".h";
	else
		fname = idlFile.substr( 0, slash ) + "i_" + fname + ".h";
	return fname;
}

namespace idl {
				
using parser::AttributeNode;
using parser::BasicTypeNode;
using boost::dynamic_pointer_cast;
using namespace boost::io;
using boost::to_upper_copy;
using boost::to_lower_copy;
using boost::replace_all_copy;
using boost::replace_all;
using boost::format;

std::string shift(const std::string& data, const std::string& tab) {
	if (data.empty()) {
		return data;
	}
	
	std::string result = tab;
	BOOST_FOREACH(char c, data.substr(0, data.length() - 1)) {
		result += c;
		if (c == '\n') {
			result += tab;
		}
	}
	result += data[data.length() - 1];
	return result;
}

AttributeNode::Ptr findAttr(const Collector::Attributes& attrs, AttributeNode::AttributeId id, bool ensure) {
	BOOST_FOREACH(AttributeNode::Ptr attr, attrs) {
		if (attr->getId() == id) {
			return attr;
		}
	}
	if (ensure) {
		ASSERT(false && "required attribute not found");
	}
	return NULL;
}
  
std::string& extendArgsList(std::string& list, const std::string& arg) {
	if (!list.empty()) {
		list += ", ";
	}
	list += arg;
	return list;
}

const std::string genId(const Collector::Attributes& attrs) {
	return (format("%1%") % group(std::hex, std::showbase, findAttr(attrs, AttributeNode::Id)->getInt())).str();
}

bool isCharPtr(Property::Ptr property) {
	Type::Ptr type = reduceTypedef(theTypeRoot, property->getType());
	
	if (RefType::Ptr ref = dynamic_pointer_cast<RefType>(type)) {
		if (BasicType::Ptr innerType = dynamic_pointer_cast<BasicType>(ref->getType())) {
			return /*ref->isConst() &&*/ innerType->getTypeId() == parser::BasicTypeNode::SInt8 && ref->getCount() == 1;
		}
	}
	
	return false;
}

std::string genVersion(const Collector::Attributes& attrs, NameContext::Ptr /*context*/) {
	AttributeNode::Ptr version = findAttr(attrs, AttributeNode::Version);
	ASSERT(version);
		
	return (format("%1%") % version->getInt()).str();
	
	//static format fmt("static const tVERSION %1%_VERSION = %2%;\n");
	
	//return (fmt % CommonMapping::shortName(context) % version->getInt()).str();
}

std::string mangleString(const std::string& text) {
	std::string result;
	BOOST_FOREACH(char c, text) {
		switch (c) {
			case '"': result += "\\\""; break;
			case '\\': result += "\\"; break;
			case '\t': result += "\\t"; break;
			case '\r': result += "\\r"; break;
			case '\n': result += "\\n"; break;
			default: result += c;
		}
	}
	return result;
}
 
std::string genConstValue(Performer::Ptr performer) {
	ASSERT(performer);

	if (performer->type() == Performer::TypeString) {
		static format fmt("\"%1%\"");
		return (fmt % mangleString(performer->asString())).str();
	} else {
		static format fmt("%1%");
		ASSERT(performer->type() == Performer::TypeInt || performer->type() == Performer::TypeFloat);
		return (format("%1%") % performer->asInt()).str();
	}
}
		
} // namespace idl
