#ifndef ImportEndNode_hpp
#define ImportEndNode_hpp

#include "NodeInterface.hpp"

namespace idl { namespace parser {

class ImportEndNode : public NodeBase<ImportEndNode> {
  public:
  	ImportEndNode(const Context& context, const idl::scaner::String& file);

  	const idl::scaner::String& getFileName() const;

  private:
  	idl::scaner::String	_filename;
};

}} //namespace idl::parser

#endif //ImportEndNode_hpp
