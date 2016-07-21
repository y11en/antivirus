#ifndef namecontext_GatherDeclaredNames_hpp
#define namecontext_GatherDeclaredNames_hpp

#include "RootNameContext.hpp"

namespace idl {

extern RootNameContext::Ptr gatherDeclaredNames(parser::NodeInterface::Ptr root);

} //namespace idl

#endif //namecontext_GatherDeclaredNames_hpp
