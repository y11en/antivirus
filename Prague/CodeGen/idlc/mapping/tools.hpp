#ifndef mapping_tools_hpp
#define mapping_tools_hpp

#include <string>
#include <vector>

#include <boost/intrusive_ptr.hpp>
#include "NodeInterface.hpp"

namespace idl {

class Collector;

class RootNameContext;

extern boost::intrusive_ptr<RootNameContext> theTypeRoot;


extern parser::NodeInterface::Ptr compileIDL(const std::string& idlFile, const std::vector<std::string>& incPaths, bool disableDefaultImports, Collector& collector);

extern std::string makeProxyHeaderFile(const std::string idlFile);
extern std::string makeProxySourceFile(const std::string idlFile);
extern std::string makeSkeletonHeaderFile(const std::string idlFile);
extern std::string makeSkeletonSourceFile(const std::string idlFile);
extern std::string makeInterfaceHeaderFile(const std::string idlFile);
extern std::string makeInterfaceSourceFile(const std::string idlFile);

} //namespace idl

#endif //mapping_tools_hpp
