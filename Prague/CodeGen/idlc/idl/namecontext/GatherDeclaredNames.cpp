#include <stack>
#include <vector>
#include <boost/intrusive_ptr.hpp>
#include <boost/format.hpp>

#include "idl/Context.hpp"
#include "GatherDeclaredNames.hpp"
#include "BasicType.hpp"
#include "RootNameContext.hpp"
#include "Value.hpp"
#include "idl/ErrorFactory.hpp"
#include "idl/NameNode.hpp"
#include "idl/OperationNode.hpp"

#include "Namespace.hpp"

#include "traits.hpp"

namespace idl {

using namespace parser;

using boost::dynamic_pointer_cast;

// collect all declarations
class DeclarationCollector : public Visitor {
	RootNameContext::Ptr	_root;
	
	typedef std::stack<NameContext::Ptr> Stack;
	Stack	_contextStack;

  public:
	DeclarationCollector() {
		_root = new RootNameContext();
		
		_contextStack.push(_root);
	}

	~DeclarationCollector() {
		ASSERT(_contextStack.size() == 1);
		_contextStack.pop();
	}
		
	RootNameContext::Ptr getRootNameContext() const {
		return _root;
	}

	/**
		Within Interface we scan all base interfaces (it concerns props, message classes, errors, methods, consts, enums, structs)
			
		
	
	
		Within ComponentInterface we scan all base interfaces (it concerns props, message classes, errors, methods, consts, enums, structs)
	
	
		Property:
			properties are not extended, so in interfaces a given property may be defined only once
				- so we must ensure that
			property are of Value type
			`prop' definition in plugin requires previously defined interface property
			`add prop' definition in plugin translates into pair `prop'-as-in-interface and `prop'-as-in-plugin
				- so we must ensure uniqueness of a 
	*/
	
	template <Operations::Op Op>
	NameContext::Ptr createContext(parser::OperationNode<Op>* declarationPoint) const {
		NameNode::Ptr nameNode = OpTools::nameNode( declarationPoint );
		ASSERT(nameNode);
		Name        name = nameNode->getCoreName();
		const Name& alt  = nameNode->getAltName();
		
		NameContext::Ptr topCtx  = _contextStack.top();
		NameContext::Ptr prevCtx = topCtx->find( name, alt );
		NameContext::Ptr newCtx  = new op_traits<Op>::Context;
		
		if ( prevCtx ) {
			if ( dynamic_pointer_cast<Namespace>(prevCtx) && dynamic_pointer_cast<Namespace>(newCtx) ) {
				// reopen of the same namespace is quite correct operation
				nameNode->getDescriptor().setNameContext( prevCtx );
				return prevCtx;
			}

			Typedef::Ptr otype = dynamic_pointer_cast<Typedef>( prevCtx );
			Typedef::Ptr ntype = dynamic_pointer_cast<Typedef>( newCtx );
			if ( otype && ntype ) {
				// check if the same typedef
				nameNode->getDescriptor().setNameContext( prevCtx );
				return prevCtx;
			}
			
			ErrorFactory::instance().nameAlreadyDefined( declarationPoint->getContext(), name, prevCtx->getDeclarationContext() );

			static int uniqueId = 0;		
			name = Name((boost::format("__already_defined_%1%_%2%") % ++uniqueId % name).str());
		}
		
		newCtx->setNamePath( topCtx->getNamePath(), name, alt );
		newCtx->setDeclaration( declarationPoint );
		
		nameNode->getDescriptor().setNameContext( newCtx );
		
		topCtx->addChild( newCtx );
		
		return newCtx;
	}
	
	template <parser::Operations::Op Op>
	void openNameDeclaration(parser::OperationNode<Op>* declarationPoint) {
		NameContext::Ptr context = createContext(declarationPoint);
		
		ASSERT(_contextStack.size() > 0);
		_contextStack.push(context);
	}
	
	void closeNameDeclaration() {
		ASSERT(_contextStack.size() > 0);
		_contextStack.pop();
	}
	
#define NAME_DECLARE(op)                                                        \
	virtual bool visitBefore(parser::OperationNode<Operations::op>* node) {       \
		openNameDeclaration(node);                                                  \
		return true;                                                                \
	}                                                                             \
	virtual bool visitAfter(parser::OperationNode<Operations::op>* ) {            \
		closeNameDeclaration();                                                     \
		return true;                                                                \
	}

	virtual bool visitBefore(parser::OperationNode<Operations::Component>* node) {
		NameContext::Ptr context = createContext(node);
		return true;                                                               
	}                                                                              
	virtual bool visitAfter(parser::OperationNode<Operations::Component>* ) {   
		return true;                                                               
	}
	
	NAME_DECLARE(Namespace)
	NAME_DECLARE(Interface)
	NAME_DECLARE(Implementation)
	NAME_DECLARE(ErrorDef)
	NAME_DECLARE(Typedef)
	NAME_DECLARE(Enum)
	NAME_DECLARE(Struct)
	NAME_DECLARE(EnumEntry)
	NAME_DECLARE(StructEntry)
	NAME_DECLARE(Const)
	NAME_DECLARE(Method)
	NAME_DECLARE(ExternMethod)
	NAME_DECLARE(MethodArg)
	NAME_DECLARE(SysMethod)
	NAME_DECLARE(MessageClass)
	NAME_DECLARE(MessageClassExtend)
	NAME_DECLARE(Message)
	NAME_DECLARE(Property)
	//NAME_DECLARE(PropertyPlugin)
	NAME_DECLARE(PropertyPluginAdd)
	NAME_DECLARE(ImportName)
	NAME_DECLARE(ImportNameAsType)
};

RootNameContext::Ptr gatherDeclaredNames(NodeInterface::Ptr root) {
	DeclarationCollector collector;
	root->visit(collector);
	return collector.getRootNameContext();
}

} // namespace idl
