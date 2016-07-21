#ifndef ImportStartNode_hpp
#define ImportStartNode_hpp

#include "NodeInterface.hpp"

namespace idl { namespace parser {

class ImportStartNode : public NodeBase<ImportStartNode> {
  public:
  	ImportStartNode(const Context& context, const idl::scaner::String& file);

  	const idl::scaner::String& getFileName() const;

  private:
  	idl::scaner::String	_filename;
};

}} //namespace idl::parser

#endif //ImportStartNode_hpp
