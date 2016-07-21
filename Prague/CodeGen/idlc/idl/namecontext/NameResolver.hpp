#ifndef namecontext_NameResolver_hpp
#define namecontext_NameResolver_hpp

#include "RootNameContext.hpp"

namespace idl {

extern void resolveNames(RootNameContext::Ptr contexts, parser::NodeInterface::Ptr root);

} //namespace idl

#endif //namecontext_NameResolver_hpp
