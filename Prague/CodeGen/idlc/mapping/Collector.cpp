#include "Context.hpp"
#include "Collector.hpp"

#include "IntNode.hpp"
#include "NameNode.hpp"
#include "OperationNode.hpp"
#include "ImportStartNode.hpp"
#include "ImportEndNode.hpp"

#include "namecontext/Value.hpp"
#include "namecontext/Type.hpp"
#include "namecontext/Enum.hpp"
#include "namecontext/Struct.hpp"
#include "namecontext/BasicType.hpp"
#include "namecontext/Typedef.hpp"
#include "namecontext/SequenceType.hpp"
#include "idl/namecontext/ConstValue.hpp"

#include "ArgsLayout.hpp"
#include "InOutNode.hpp"

#include "namecontext/traits.hpp"

#include <set>

namespace idl {

class CollectorNodeVisitor : public parser::Visitor, boost::noncopyable {
	Collector& 	_collector;

	int			_inImport;
	
	//std::set<SequenceType::Ptr>	_sequences;

	bool notInImport() const {
		return _inImport == 0;
	}
		
  public:
  	CollectorNodeVisitor(Collector& collector) : _collector(collector), /*_order(0),*/ _inImport(0) {
  	}


	virtual bool visitBefore(parser::ImportStartNode* node) {
		if (!_collector.processImports()) {
			if (notInImport()) {
				_collector.importFile(node->getFileName());
			}
			_inImport += 1;
		}
		return true;
	}
	
	virtual bool visitBefore(parser::ImportEndNode* /*node*/) {
		if (!_collector.processImports()) {
			ASSERT(!notInImport());
			_inImport -= 1;
		}
		return true;
	}
	
	/*virtual bool visitBefore(parser::OperationNode<parser::Operations::Sequence>* node) {
		if (!_inImport) {
			SequenceType::Ptr type = boost::dynamic_pointer_cast<SequenceType>(node->getDescriptor().getType());
			ASSERT(type);
			if (_sequences.insert(type).second) {
				_collector.sequence(type);
			}
		}
		return false;
	}*/
	
	virtual bool visitBefore(parser::OperationNode<parser::Operations::Const>* node) {
		if (notInImport()) {
			const idl::Collector::Attributes& attr = copyAttrs(OpTools::attrs(node));
			idl::ConstValue::Ptr val = OpTools::context(node);
			_collector.constDecl( attr, val );
		}
		return false;
	}
	
	virtual bool visitBefore(parser::OperationNode<parser::Operations::Typedef>* node) {
		if (notInImport()) {
			_collector.typedefDecl(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return false;
	}
	
	virtual bool visitBefore(parser::OperationNode<parser::Operations::Typecast>* node) {
		if (notInImport()) {
			_collector.typecast( OpTools::context(node) );
		}
		return false;
	}
	
	virtual bool visitBefore(parser::OperationNode<parser::Operations::Struct>* node) {
		if (notInImport()) {
			_collector.structDecl(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Enum>* node) {
		if (notInImport()) {
			_collector.enumDecl(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Interface>* node) {
		if (notInImport()) {
			_collector.interfaceOpen(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitAfter(parser::OperationNode<parser::Operations::Interface>* node) {
		if (notInImport()) {
			_collector.interfaceClose(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::ForwardIFace>* node) {
		if (notInImport()) {
			_collector.forwardIFace(OpTools::context(node));
		}
		return false;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::ForwardStruct>* node) {
		if (notInImport()) {
			_collector.forwardStruct(OpTools::context(node));
		}
		return false;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Method>* node) {
		if (notInImport()) {
			_collector.methodDecl(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::ExternMethod>* node) {
		if (notInImport()) {
			_collector.externMethodDecl(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Component>* node) {
		if (notInImport()) {
			_collector.componentOpen(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitAfter(parser::OperationNode<parser::Operations::Component>* node) {
		if (notInImport()) {
			_collector.componentClose(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Implementation>* node) {
		if (notInImport()) {
			_collector.implementationOpen(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitAfter(parser::OperationNode<parser::Operations::Implementation>* node) {
		if (notInImport()) {
			_collector.implementationClose(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::SysMethod>* node) {
		if (notInImport()) {
			_collector.sysMethodDecl(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Property>* node) {
		if (notInImport()) {
			_collector.propertyDecl(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::PropertyPlugin>* node) {
		if (notInImport()) {
			_collector.propertyPluginDecl(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::PropertyPluginAdd>* node) {
		if (notInImport()) {
			_collector.propertyPluginAddDecl(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Message>* node) {
		if (notInImport()) {
			_collector.messageDecl(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::MessageClass>* node) {
		if (notInImport()) {
			_collector.messageClassDecl(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::MessageClassExtend>* node) {
		if (notInImport()) {
			_collector.messageClassExtendDecl(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::ErrorDef>* node) {
		if (notInImport()) {
			_collector.errorDecl(copyAttrs(OpTools::attrs(node)), OpTools::context(node));
		}
		return true;
	}
	
	virtual bool visitBefore(parser::OperationNode<parser::Operations::ImportName>* node) {
		if (notInImport()) {
			_collector.externalTypeDecl(OpTools::context(node));
		}
		return true;
	}
	
	virtual bool visitBefore(parser::OperationNode<parser::Operations::ImportNameAsType>* node) {
		if (notInImport()) {
			_collector.externalTypeDecl(OpTools::context(node));
		}
		return true;
	}
	
  private:
  	const Collector::Attributes& copyAttrs(parser::Tuple::Ptr from) {
  		_attrsCache.resize(0);
  		
  		for (size_t i = 0; i < from->getSize(); ++i) {
  			parser::AttributeNode::Ptr attr = boost::dynamic_pointer_cast<parser::AttributeNode>(from->getNode(i));
  			ASSERT(attr);
  			_attrsCache.push_back(attr);
  		}
  		return _attrsCache;
  	}
  	
  	Collector::Attributes	_attrsCache;
};

void collect(parser::NodeInterface::Ptr rootNode, Collector& collector) {
	CollectorNodeVisitor visitor(collector);
	rootNode->visit(visitor);
}

} //namespace idl
