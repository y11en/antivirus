#ifndef mapping_MappingTools_hpp
#define mapping_MappingTools_hpp

#include "mapping/Collector.hpp"


extern std::string basename(const std::string& file);
extern std::string changeExt(const std::string& file, const std::string& ext);
extern std::string makeInterfaceHeaderFile( const std::string idlFile );

namespace idl {
				
extern std::string shift(const std::string& data, const std::string& tab = "\t");
extern parser::AttributeNode::Ptr findAttr(const Collector::Attributes& attrs, parser::AttributeNode::AttributeId id, bool ensure = true);
extern std::string& extendArgsList(std::string& list, const std::string& arg);
extern const std::string genId(const Collector::Attributes& attrs);
extern bool isCharPtr(Property::Ptr property);

extern std::string genVersion(const Collector::Attributes& attrs, NameContext::Ptr context);
extern std::string genConstValue(Performer::Ptr performer);
		
} // namespace idl

#endif //mapping_MappingTools_hpp
