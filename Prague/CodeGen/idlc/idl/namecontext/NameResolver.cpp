#include <set>
#include <stack>
#include <vector>
#include <boost/intrusive_ptr.hpp>

#include <boost/noncopyable.hpp>

#include "idl/Context.hpp"

#include "NameResolver.hpp"
#include "BasicType.hpp"
#include "Value.hpp"
#include "ConstValue.hpp"
//#include "Sequence.hpp"
//#include "SequenceType.hpp"

#include "idl/ErrorFactory.hpp"
#include "idl/IntNode.hpp"
#include "idl/FloatNode.hpp"
#include "idl/StringNode.hpp"
#include "idl/NameNode.hpp"
#include "idl/OperationNode.hpp"
#include "RootNameContext.hpp"
#include "ConstEval.hpp"

#include "ExternalType.hpp"

#include "idl/ArgsLayout.hpp"
#include "traits.hpp"
#include <idl_pfx.h>
#include "scaner.hpp"

namespace idl {

using namespace parser;
using boost::dynamic_pointer_cast;

typedef std::set<NameContext::Ptr> ForwardDeclarations;

class ActualizedNames {
  public:
	void actualize( NameContext::Ptr context ) {
		std::set<NameContext::Ptr>::iterator found = _actualizedNames.find( context );
		if ( found == _actualizedNames.end() ) {
			_actualizedNames.insert( context );
			return;
		}

		NameContext::Ptr prev = *found;
		Typedef::Ptr p_tdef = dynamic_pointer_cast<Typedef>( prev );
		Typedef::Ptr n_tdef = dynamic_pointer_cast<Typedef>( context );
		if ( p_tdef && n_tdef ) {
			bool p_ref = false;
			bool p_const = false;
			int  p_count = 0;
			Type::Ptr p_t = reduceTypedef( p_tdef->getType(), p_ref, p_const, p_count, false );
			if ( p_t ) {
				bool n_ref = false;
				bool n_const = false;
				int  n_count = 0;
				Type::Ptr n_t = reduceTypedef( n_tdef->getType(), n_ref, n_const, n_count, false );
				if ( n_t ) {
					const Name& p_n = p_t->getAltName();
					const Name& n_n = n_t->getAltName();
					if ( (p_n == n_n) && (p_ref == n_ref) && (p_const == n_const) && (p_count == n_count) )
						return;
				}
			}
		}
		ErrorFactory::instance().nameAlreadyDefined( context->getDeclarationContext(), context->getNamePath().str(), prev->getDeclarationContext() );
	}

	bool isActual(NameContext::Ptr context) const {
		return _actualizedNames.find(context) != _actualizedNames.end();
	}
	
	bool requireActual(const Context& context, NameContext::Ptr nameContext) const {
		if ( !isActual(nameContext) ) {
			ErrorFactory::instance().nameMustBeDeclaredBeforeUsage(context, nameContext->getNamePath().str(), nameContext->getDeclarationContext());
			return false;
		}
		return true;
	}

  private:
	std::set<NameContext::Ptr>	_actualizedNames;
};

class NameContextStack {
public:
	~NameContextStack() {
		ASSERT(_contextStack.empty());
	}

	NameContext::Ptr top() const {
		return _contextStack.back();
	}

	NameContext::Ptr findOnStack( NameNode::Ptr nameNode ) const {
		return findOnStack( nameNode->getCoreName(), nameNode->getAltName() );
	}

	NameContext::Ptr findOnStack( const Name& name, const Name& alt ) const {
		Stack::const_reverse_iterator i = _contextStack.rbegin();
		Stack::const_reverse_iterator end = _contextStack.rend();

		NameContext::Ptr result;
		for (; i !=end; ++i) {
			NameContext::Ptr ctx = *i;
			result = ctx->find( name, alt );

			if ( result )
				break;

			Interface::Ptr iface = dynamic_pointer_cast<Interface>(*i);
			if ( iface )
				result = iface->findWithinBases( name, alt );

			if ( result )
				break;
		}

		return result;
	}

	//NameContext::Ptr findOnTop(const Name& name) const {
	//	ASSERT(!_contextStack.empty());
	//	return _contextStack.back()->find(name);
	//}

	void push(NameContext::Ptr context) {
		_contextStack.push_back(context);
	}

	void pop() {
		ASSERT(!_contextStack.empty());
		_contextStack.pop_back();
	}

private:
	typedef std::vector<NameContext::Ptr> Stack;
	Stack	_contextStack;
};

class ResolutionVisitor : public Visitor, boost::noncopyable {
	bool fail() {
		_result = false;
		return _result;
	}
		
	bool success(bool result = true) {
		_result &= result;
		return _result;
	}
		
  public:
  	ResolutionVisitor(RootNameContext::Ptr root, const NameContextStack& contextStack, const ActualizedNames& actualizedNames, const ForwardDeclarations& forwardIfaces, const ForwardDeclarations& forwardStructs, const EvaledConsts& consts) 
  			: _result(true), _withinRefType(0), _root(root), _contextStack(contextStack), _actualizedNames(actualizedNames), _forwardIfaces(forwardIfaces), _forwardStructs(forwardStructs), _consts(consts) {}

	void resetResult() {
		_result = true;
	}

	bool getResult() const {
		return _result;
	}
  			
	bool resolveGlobalName( NameNode::Ptr node ) const {
		NameContext::Ptr context;
		//std::string name = node->getCoreName();

		if ( _withinRefType > 0 ) {
			std::string alt = node->getAltName();

			//BOOST_FOREACH(NameContext::Ptr forwardContext, _forwardIfaces) {
			ForwardDeclarations::const_iterator i = _forwardIfaces.begin();
			for( ; i != _forwardIfaces.end(); ++i ) {
				NameContext::Ptr fwdCtx = *i;
				if ( fwdCtx->checkName(node) ) {
					context = fwdCtx;
					break;
				}
			}
			if ( !context ) {
				//BOOST_FOREACH(NameContext::Ptr forwardContext, _forwardStructs) {
				for ( i = _forwardStructs.begin(); i != _forwardStructs.end(); ++i ) {
					NameContext::Ptr fwdCtx = *i;
					if ( fwdCtx->checkName(node) ) {
						context = fwdCtx;
						break;
					}
				}
			}
		} 
			
		if ( !context ) {
			context = _contextStack.findOnStack( node );

  		if (context && !_actualizedNames.requireActual(node->getContext(), context))
      	return false;

  		if (!context) {
  			// register name as external type spec.
  			context = new ExternalType;
  			context->setNamePath( NamePath(), node );
  			context->setDeclaration(node);
  			
  			_root->addChild(context);
  			
  			node->getDescriptor().setNameContext(context);
  			
  			ErrorFactory::instance().nameNotFound(node->getContext(), node->getCoreName(), _contextStack.top());
  			return false;
  		}
		}
		
		node->getDescriptor().setNameContext(context);
		
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::RefType>* node) {
		NodeInterface::Ptr typeParam = node->getArgNode(ArgsLayout::RefType::Type);
		
		ASSERT(typeParam);
		
		_withinRefType += 1;
		typeParam->visit(*this);
		_withinRefType -= 1;

		if (!typeParam->getDescriptor().isType()) {
			ErrorFactory::instance().typeExpected(typeParam->getContext());
			fail();
		} 
		
		if (_result) {
			Type::Ptr type = typeParam->getDescriptor().getType();
			ASSERT(type);
			node->getDescriptor().setNameContext(_root->addRefType(type, 1, false));
		}
		
		return false;
	}
	
	virtual bool visitBefore(parser::OperationNode<parser::Operations::ConstType>* node) {
		NodeInterface::Ptr typeParam = node->getArgNode(ArgsLayout::ConstType::Type);
		
		ASSERT(typeParam);
		
		typeParam->visit(*this);
		if (!typeParam->getDescriptor().isType()) {
			ErrorFactory::instance().typeExpected(typeParam->getContext());
			fail();
		} 
		
		if (_result) {
			Type::Ptr type = typeParam->getDescriptor().getType();
			ASSERT(type);
			node->getDescriptor().setNameContext(_root->addRefType(type, 0, true));
		}
		
		return false;
	}
	
	virtual bool visitAfter(BasicTypeNode* /*node*/) {
		return _result;
	}
	
	virtual bool visitAfter(NameNode* /*node*/) {
		return _result;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Dot>* /*node*/) {
		return _result;
	}
	
	template <BasicTypeNode::TypeId typeId, class T>
	void resolveConstValueNode(T* node) {
		Value::Ptr value = new Value();
		value->setType(_root->getBasicType(typeId));
		node->getDescriptor().setNameContext(value);
	}
	
	virtual bool visitBefore(IntNode* node) {
		resolveConstValueNode<BasicTypeNode::SInt32>(node);
		return false;
	}
	
	virtual bool visitBefore(FloatNode* node) {
		resolveConstValueNode<BasicTypeNode::Float>(node);
		return false;
	}
	
	virtual bool visitBefore(StringNode* node) {
		Value::Ptr value = new Value();
		RefType::Ptr ref =_root->addRefType(_root->getBasicType(BasicTypeNode::SInt8), 1, true);
		value->setType(ref);
		node->getDescriptor().setNameContext(value);
		return false;
	}
	
	virtual bool visitBefore(BasicTypeNode* node) {
		node->getDescriptor().setNameContext(_root->getBasicType(node->getTypeId()));
		return false;
	}
	
	virtual bool visitBefore(NameNode* node) {
		success(resolveGlobalName(node));
		return false;
	}

	//virtual bool visitBefore(parser::OperationNode<parser::Operations::Sequence>* node) {
	//	NodeInterface::Ptr typeParam = node->getArgNode(ArgsLayout::Sequence::Type);
	//	NodeInterface::Ptr sizeParam = node->getArgNode(ArgsLayout::Sequence::Size);
	//	ASSERT(typeParam && sizeParam);
	//	
	//	typeParam->visit(*this);
	//	sizeParam->visit(*this);

	//	size_t size = 0;

	//	Performer::Ptr performer = evalConst(sizeParam, _consts);

	//	if (performer == NULL) {
	//		//ErrorFactory::instance().intValueExpected(typeParam->getContext());
	//		fail();
	//	} else {
	//		size = static_cast<size_t>(performer->asInt());
	//	}
	//			
	//	/*if (IntNode::Ptr intNode = dynamic_pointer_cast<IntNode>(sizeParam)) {
	//		size = intNode->getValue();
	//	} else {
	//		ErrorFactory::instance().intValueExpected(typeParam->getContext());
	//		fail();
	//	}*/

	//	if (!typeParam->getDescriptor().isType()) {
	//		ErrorFactory::instance().typeExpected(typeParam->getContext());
	//		fail();
	//	} 
	//	
	//	if (_result) {
	//		Type::Ptr type = typeParam->getDescriptor().getType();
	//		node->getDescriptor().setNameContext(_root->getSequence()->add(type, size));
	//	}
	//	
	//	return false;
	//}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Dot>* node) {
		NodeInterface::Ptr left;
		NameNode::Ptr right;
		
		if (node->getTuple()->getSize() == 1) {
			node->getArg(ArgsLayout::DotUnary::Right, right);
		} else {
			ASSERT(node->getTuple()->getSize() == 2);
			node->getArg(ArgsLayout::Dot::Left, left);
			node->getArg(ArgsLayout::Dot::Right, right);
		}
		
		NameContext::Ptr nameContext;
		
		if (!left) {
			success(resolveGlobalName(right));
		} else {
			left->visit(*this);
			NameContext::Ptr parentContext = left->getDescriptor().getNameContext();
			if (parentContext) {
				NameContext::Ptr nameContext = parentContext->find( right->getCoreName(), right->getAltName() );
				
				if (!nameContext) {
					ErrorFactory::instance().nameNotFound( right->getContext(), right->getCoreName(), parentContext );
					fail();
				} else if (!_actualizedNames.requireActual(node->getContext(), nameContext)) {
					nameContext = NULL;
					fail();
				}
				
				right->getDescriptor().setNameContext(nameContext);
			}
		}
		
		node->getDescriptor().setNameContext(right->getDescriptor().getNameContext());
		
		return false;
	}

	BasicType::Ptr ensureNonVoidBasicType(const Context& context, Type::Ptr type) {
		ASSERT(type);
		BasicType::Ptr t = dynamic_pointer_cast<BasicType>(reduceTypedef(_root, type, false));
		if (!t) {
			ErrorFactory::instance().operationRequiresArgsOfBasicType(context, type->getNamePath());
			fail();
		}
		else if (t->getTypeId() == BasicTypeNode::Void) {
			ErrorFactory::instance().voidNotAllowed(context);
			fail();
		}
		else {
			return t;
		}
		
		return _root->getBasicType(BasicTypeNode::SInt32);
	}
	
	BasicType::Ptr ensureIntType(const Context& context, Type::Ptr type) {
		ASSERT(type);
		BasicType::Ptr t = ensureNonVoidBasicType(context, type);
		ASSERT(t);
		
		if (!t->isInt()) {
			ErrorFactory::instance().operationRequiresArgsOfIntType(context, type->getNamePath());
			fail();
			t = _root->getBasicType(BasicTypeNode::SInt32);
		}
		
		ASSERT(t);
		return t;
	}
	
	BasicType::Ptr ensureIntFloatType(const Context& context, Type::Ptr type) {
		ASSERT(type);
		BasicType::Ptr t = ensureNonVoidBasicType(context, type);
		ASSERT(t);
		
		if (!t->isInt() && !t->isFloat()) {
			ErrorFactory::instance().operationRequiresArgsOfIntType(context, type->getNamePath());
			fail();
			t = _root->getBasicType(BasicTypeNode::Float);
		}
		
		ASSERT(t);
		return t;
	}
	
	template <Operations::Op id>
	Type::Ptr selectType(const Context& context, Type::Ptr arg) {
		ASSERT(arg);
		switch (id) {
			case parser::Operations::BitInv:
				arg = ensureIntType(context, arg);
				break;
			case parser::Operations::Neg:
				arg = ensureIntFloatType(context, arg);
				break;
			default:
				ASSERT(false && "unexpected operation");
		}

		ASSERT(arg);
		return arg;
	}
	
	template <Operations::Op id>
	Type::Ptr selectType(const Context& context, Type::Ptr left, Type::Ptr right) {
		ASSERT(left && right);
		
		BasicType::Ptr l = ensureNonVoidBasicType(context, left);
		BasicType::Ptr r = ensureNonVoidBasicType(context, right);

		ASSERT(l && r);

		switch (id) {
			case parser::Operations::BitAnd:
			case parser::Operations::BitInv:
			case parser::Operations::BitOr:
			case parser::Operations::BitXor:
			case parser::Operations::ShiftLeft:
			case parser::Operations::ShiftRight:
				l = ensureIntType(context, l);
				r = ensureIntType(context, r);
				break;
			case parser::Operations::Mul:
			case parser::Operations::Div:
			case parser::Operations::Sub:
			case parser::Operations::Add:
				if (l->isFloat() || r->isFloat()) {
					ensureIntFloatType(context, l);
					ensureIntFloatType(context, r);
					return _root->getBasicType(BasicTypeNode::Float);
				}
				break;
			default:
				ASSERT(false);
		}

		ASSERT(l->isInt() && r->isInt());
		
		bool signedType = l->isSigned() || r->isSigned();
		BasicTypeNode::TypeId typeId = signedType ? BasicTypeNode::SInt64 : BasicTypeNode::UInt64;
		
		return _root->getBasicType(typeId);
	}

	template <Operations::Op id>
	Type::Ptr retrieveArgType(parser::OperationNode<id>* node, int arg) {
		ASSERT(node);
		ASSERT(node->getArgNode(arg));
		Value::Ptr value = node->getArgNode(arg)->getDescriptor().getValue();
		if (!value) {
			ErrorFactory::instance().valueExpected(node->getArgNode(arg)->getContext());
			fail();
			return NULL;
		}
		
		Type::Ptr type = value->getType();

		ASSERT(type || !type && ErrorFactory::instance().getErrors().size() != 0);
		
		/*if (!type) {
			ErrorFactory::instance().typeExpected(node->getArgNode(arg)->getContext());
		}*/
			
		return type;
	}

	template <Operations::Op id>
	void assignValueToOp(parser::OperationNode<id>* node, Type::Ptr type) {
		Value::Ptr context = new Value;
				
		context->setType(type);
		context->setDeclaration(node);
		
		node->getDescriptor().setNameContext(context);
	}

	/*enum AllowedTypes {
		AllowedTypes_Int		,
		AllowedTypes_Float		,
		AllowedTypes_IntFloat	,
		AllowedTypes_String		,
		AllowedTypes_Ptr		,
	};

	bool checkOpArgType(type, allowedTypes) */
		
	template <Operations::Op id>
	bool visitBinaryOp(parser::OperationNode<id>* node) {
		Type::Ptr left = retrieveArgType(node, ArgsLayout::BinaryOp::First);
		Type::Ptr right = retrieveArgType(node, ArgsLayout::BinaryOp::Second);

		if (!left || !right) {
			return false;
		}

		/*if (!checkOpArgType(left, allowedTypes)) {
			ErrorFactory::instance().wrongTypeInConstExpression(node->getContext(), left->getNamePath());
			return false;
		}
		
		if (!checkOpArgType(right, allowedTypes)) {
			ErrorFactory::instance().wrongTypeInConstExpression(node->getContext(), right->getNamePath());
			return false;
		}*/

		assignValueToOp(node, selectType<id>(node->getContext(), left, right));
		
		return true;
	}
	
	template <Operations::Op id>
	bool visitUnaryOp(parser::OperationNode<id>* node) {
		Value::Ptr context = new Value;
		
		Type::Ptr argType = retrieveArgType(node, ArgsLayout::UnaryOp::Arg);

		if (!argType) {
			return false;
		}
				
		Type::Ptr type = ensureNonVoidBasicType(node->getContext(), argType);

		if (!type) {
			return false;
		}
		
		/*if (!checkOpArgType(type, allowedTypes)) {
			ErrorFactory::instance().wrongTypeInConstExpression(node->getContext(), left->getNamePath());
			return false;
		}*/
		
		assignValueToOp(node, selectType<id>(node->getContext(), type));

		return true;
	}

	virtual bool visitAfter(parser::OptionalValueNode* node) {
		ASSERT(node->hasValue());
		NodeInterface::Ptr value = node->getValue();
		ASSERT(value);
		ASSERT(_result ? value->getDescriptor().getNameContext() : true);
		node->getDescriptor().setNameContext(value->getDescriptor().getNameContext());
		return _result;
	}
	
	#define VISIT_BINARY_OP(op)                                                        \
			virtual bool visitAfter(parser::OperationNode<parser::Operations::op>* node) { \
				return success(visitBinaryOp(node));                                         \
			}
	
	#define VISIT_UNARY_OP(op)                                                         \
			virtual bool visitAfter(parser::OperationNode<parser::Operations::op>* node) { \
				return success(visitUnaryOp(node));                                          \
			}
	
	VISIT_BINARY_OP(Add)
	VISIT_BINARY_OP(Sub)
	VISIT_BINARY_OP(Mul)
	VISIT_BINARY_OP(Div)
		
	VISIT_BINARY_OP(BitAnd)
	VISIT_BINARY_OP(BitOr)
	VISIT_BINARY_OP(BitXor)
		
	VISIT_BINARY_OP(ShiftLeft)
	VISIT_BINARY_OP(ShiftRight)
		
	VISIT_UNARY_OP(BitInv)
	VISIT_UNARY_OP(Neg)
		
		/*And,
		Eq,
		FunCall,
		Ge,
		Gt,
		Le,
		Lt,
		Ne,
		Not,
		Or,*/
  private:
  	bool					_result;
	int						_withinRefType;
  	const EvaledConsts&		_consts;
	RootNameContext::Ptr	_root;
	const NameContextStack&	_contextStack;
	const ActualizedNames&	_actualizedNames;
	const ForwardDeclarations& _forwardIfaces;
	const ForwardDeclarations& _forwardStructs;
};

class NameResolver : public Visitor, boost::noncopyable {
	enum ExpectedDescriptor {
		TYPE_EXPECTED,
		VALUE_EXPECTED, 
		NAMECONTEXT_EXPECTED,
	};

	EvaledConsts            _consts;
	
	RootNameContext::Ptr    _root;
	NameContextStack        _contextStack;

	ResolutionVisitor       _resolutionVisitor;

	ActualizedNames         _actualizedNames;
	
	ForwardDeclarations     _forwardIfaces;
	ForwardDeclarations     _forwardStructs;
	bool                    _pushCtx;

	template <Operations::Op Op>
	void actualize(parser::OperationNode<Op>* declarationPoint) {
		_actualizedNames.actualize(OpTools::context(declarationPoint));
	}
				
  public:
	NameResolver(RootNameContext::Ptr root) : _resolutionVisitor(root, _contextStack, _actualizedNames, _forwardIfaces, _forwardStructs, _consts), _pushCtx(false) {
		_root = root;
		_contextStack.push(_root);
	}

	virtual ~NameResolver() {
		_contextStack.pop();
	}

	bool getResult() const {
		return _resolutionVisitor.getResult();
	}

	bool resolve(NodeInterface::Ptr node) {
		_resolutionVisitor.resetResult();
		node->visit(_resolutionVisitor);
		return _resolutionVisitor.getResult();
	}
	
	template <class T>
	bool resolve(T* holder, int arg, ExpectedDescriptor expected = NAMECONTEXT_EXPECTED) {
		NodeInterface::Ptr node = holder->getArgNode(arg);
		
		bool result = resolve(node);
		
		NodeDescriptor& desc = node->getDescriptor();

		if (result) {
			if (expected == TYPE_EXPECTED && !desc.isType()) {
				ErrorFactory::instance().typeExpected(node->getContext());
				result = false;
			} else if (expected == VALUE_EXPECTED && !desc.isValue()) {
				ErrorFactory::instance().valueExpected(node->getContext());
				result = false;
			}
		}

		return result;
	}

		
	template <class T>
	void assignTypeToValue(T* holder, int from, int to) {
		NodeInterface::Ptr fromNode = holder->getArgNode(from);
		NodeInterface::Ptr toNode = holder->getArgNode(to);
		
		ASSERT(fromNode->getDescriptor().isType());
		ASSERT(toNode->getDescriptor().isValue());
		
		toNode->getDescriptor().getValue()->setType(fromNode->getDescriptor().getType());
	}
	
	template <class T>
	void assignTypeToType(T* holder, int from, int to) {
		NodeInterface::Ptr fromNode = holder->getArgNode(from);
		NodeInterface::Ptr toNode = holder->getArgNode(to);
		
		ASSERT(fromNode->getDescriptor().isType());
		ASSERT(toNode->getDescriptor().isType());
		
		toNode->getDescriptor().getType()->setType(fromNode->getDescriptor().getType());
	}

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	virtual bool visitBefore(parser::OperationNode<parser::Operations::ForwardIFace>* node) {
		NameNode::Ptr nameNode = OpTools::nameNode( node );
		NameContext::Ptr context = _contextStack.findOnStack( nameNode );
		if ( !context ) {
			context = new Interface;
			context->setDeclaration( node );
			context->setNamePath( NamePath(), nameNode );
		}
		
		Interface::Ptr iface = dynamic_pointer_cast<Interface>( context );
		if ( iface )
			_forwardIfaces.insert( iface );
		else {
			ErrorFactory::instance().onlyInterfaceOrStructMayHaveForwardDeclarations(node->getContext(), nameNode->getCoreName(), context->getDeclarationContext());
			return false;
		}
		nameNode->getDescriptor().setNameContext( context );
		return false;
	}

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	virtual bool visitBefore(parser::OperationNode<parser::Operations::ForwardStruct>* node) {
		NameNode::Ptr nameNode = OpTools::nameNode( node );
		NameContext::Ptr context = _contextStack.findOnStack( nameNode );
		if ( !context ) {
			context = new Struct;
			context->setDeclaration( node );
			context->setNamePath( NamePath(), nameNode );
		}
		
		Struct::Ptr str = dynamic_pointer_cast<Struct>( context );
		if ( str )
			_forwardStructs.insert( str );
		else {
			ExternalType::Ptr extype = dynamic_pointer_cast<ExternalType>( context );
			if ( !extype ) {
				ErrorFactory::instance().onlyInterfaceOrStructMayHaveForwardDeclarations(node->getContext(), nameNode->getCoreName(), context->getDeclarationContext());
				return false;
			}
		}
		nameNode->getDescriptor().setNameContext( context );
		return false;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Interface>* node) {
		actualize(node);
				
		NameContext::Ptr ctx = resolveBaseInterfaces( node, false );
		if ( ctx ) {
			_pushCtx = true;
			_contextStack.push( ctx );
			
			NodeInterface::Ptr bodyNode = node->getArgNode(ArgsLayout::Interface::Body);
			ASSERT(bodyNode);
			bodyNode->visit(*this);
		}				
		return false; // disable default descending visiting
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Interface>* /*node*/) {
		if ( _pushCtx ) {
			_pushCtx = false;
			_contextStack.pop();
			return true;
		}
		return false;
	}
	
	virtual bool visitBefore( parser::OperationNode<parser::Operations::Implementation>* node ) {
		actualize(node);
				
		NameContext::Ptr ctx = resolveBaseInterfaces( node, true );
		if ( ctx ) {
			_pushCtx = true;
			_contextStack.push( ctx );
			
			NodeInterface::Ptr bodyNode = node->getArgNode( ArgsLayout::Implementation::Body );
			ASSERT(bodyNode);
			bodyNode->visit(*this);
		}				
		return false; // disable default descending visiting
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Implementation>* /*node*/) {
		if ( _pushCtx ) {
			_pushCtx = false;
			_contextStack.pop();
			return true;
		}
		return false;
	}
	

	// «автра в этом месте должна быть решена проблема расширени€ классов сообщений!!!
	struct MessageClassVisitor : public Visitor {
		MessageClassVisitor(MessageClass::Ptr messageClass) : _messageClass(messageClass) {}
		virtual bool visitBefore(parser::OperationNode<parser::Operations::Message>* node) {
			Message::Ptr message = OpTools::context(node);
			ASSERT(message);
			message->setMessageClass(_messageClass);
			
			return false;
		}
	  private:
	  	MessageClass::Ptr _messageClass;
	};
	
	/*struct MessageClassExtendVisitor : public Visitor {
		MessageClassVisitor(MessageClassExtend::Ptr messageClassExtend) : _messageClassExtend(messageClassExtend) {}
		virtual bool visitBefore(parser::OperationNode<parser::Operations::Message>* node) {
			NameNode::Ptr nameNode;
			node->getArg(ArgsLayout::Implementation::Name, nameNode);
			
			Message::Ptr message = dynamic_pointer_cast<Message>(nameNode->getDescriptor().getNameContext());
			ASSERT(message);

			// as it stands nothing prevents from extending message classes with the same name but within different interface


						
			message->setMessageClass(_messageClass);
			
			return false;
		}
	  private:
	  	MessageClassExtend::Ptr _messageClassExtend;
	};*/


	virtual bool visitBefore(parser::OperationNode<parser::Operations::MessageClass>* node) {
		//1. ensure that there is no named entity with the same name in inhereted interfaces
		//2. assign this type with all messages
		
		actualize(node);
				
		if (!ensureNameNotDeclaredInBases(node)) {
			return false;
		}
		
		// as it stands nothing prevents from extending message classes with the same name but within different interface
		MessageClass::Ptr messageClass = OpTools::context(node);
		MessageClassVisitor visitor(messageClass);
		OpTools::body(node)->visit(visitor);
		return false;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Property>* node) {
		actualize(node);
				
		ensureNameNotDeclaredInBases(node);
		
		bool typeResolved = resolve(node, ArgsLayout::Property::Type, TYPE_EXPECTED); // type
		
		if (typeResolved) {
			assignTypeToValue(node, ArgsLayout::Property::Type, ArgsLayout::Property::Name);
			ensureNonVoid(node);
			IntNode::Ptr optNode;
			node->getArg( ArgsLayout::Property::Optional, optNode );
			Property::Ptr propNameCtx = OpTools::context(node);
			propNameCtx->setOptional( optNode->getValue() == 1 );
		}
		
		return false;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::PropertyDecl>* node) {
		NameNode::Ptr nameNode = OpTools::nameNode( node );
		Property::Ptr prop = ensureNameDeclaredInBases<Property>( nameNode->getCoreName(), nameNode->getAltName() );
		
		if ( !prop ) {
			ErrorFactory::instance().propertyNotFound( node->getContext(), nameNode->getCoreName(), _contextStack.top() );
			return false;
		}
		//prop->setOptional( false );

		return false;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::PropertyPlugin>* node) {
		NameNode::Ptr nameNode = OpTools::nameNode( node );
		Property::Ptr property = ensureNameDeclaredInBases<Property>( nameNode->getCoreName(), nameNode->getAltName() );
		
		if (!property) {
			ErrorFactory::instance().propertyNotFound( node->getContext(), nameNode->getCoreName(), _contextStack.top() );
			return false;
		}
		
		if ( resolve(node,ArgsLayout::PropertyPlugin::Type,TYPE_EXPECTED) ) { // type
			// add check for equivalence of resolved type and property->getType
			OpTools::nameNode(node)->getDescriptor().setNameContext(property);
			
			NodeInterface::Ptr typeNode = node->getArgNode(ArgsLayout::PropertyPlugin::Type);
			Type::Ptr type = typeNode->getDescriptor().getType();
			
			if (type != property->getType()) {
				ErrorFactory::instance().propertyTypeMismatch(node->getContext(), OpTools::name(node), property->getDeclarationContext());
				return false;
			}
			
			// assignTypeToValue(node, ArgsLayout::PropertyPlugin::Type, ArgsLayout::PropertyPlugin::Name);
			if (OpTools::value(node)->hasValue()) {
				bool valueResolved = resolve(node, ArgsLayout::PropertyPlugin::Value, VALUE_EXPECTED); // expr
				if (valueResolved) {
					assignConst(node);
				}
			}
		}

		//OpTools::context(node)->setProperty(property);
		
		return false;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::PropertyPluginAdd>* node) {
		actualize(node);
				
		ensureNameNotDeclaredInBases(node);
		
		bool typeResolved = resolve(node, ArgsLayout::PropertyPluginAdd::Type, TYPE_EXPECTED); // type
		
		if (typeResolved) {
			assignTypeToValue(node, ArgsLayout::PropertyPluginAdd::Type, ArgsLayout::PropertyPluginAdd::Name);
		}
		
		ensureNonVoid(node);
		
		if (OpTools::value(node)->hasValue()) {
			bool valueResolved = resolve(node, ArgsLayout::PropertyPluginAdd::Value, VALUE_EXPECTED); // expr
			if (valueResolved) {
				assignConst(node);
			}
		}

		return false;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::ErrorDef>* node) {
		actualize(node);
				
		ensureNameNotDeclaredInBases(node);

		// here must be a default value processing
				
		return false;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Namespace>* node) {
		actualize(node);
				
		_contextStack.push(OpTools::context(node));
		return true;
	}
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Namespace>* ) {
		_contextStack.pop();
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Method>* node) {
		actualize(node);
				
		ensureNameNotDeclaredInBases(node);
		/*bool typeResolved =*/ resolve(node, ArgsLayout::Method::Type, TYPE_EXPECTED); // return type
		OpTools::context(node)->setReturnType(OpTools::type(node));
		_contextStack.push(OpTools::context(node));
		
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Method>* ) {
		_contextStack.pop();
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::VarArg>* ) {
		Method::Ptr method = dynamic_pointer_cast<Method>(_contextStack.top());
		ASSERT(method);
		method->setVarArg();
		return false;
	}
	
	virtual bool visitBefore(parser::OperationNode<parser::Operations::ExternMethod>* node) {
		actualize(node);
				
		/*bool typeResolved =*/ resolve(node, ArgsLayout::ExternMethod::Type, TYPE_EXPECTED); // return type
		OpTools::context(node)->setReturnType(OpTools::type(node));
		_contextStack.push(OpTools::context(node));
		return true;
	}
	virtual bool visitAfter(parser::OperationNode<parser::Operations::ExternMethod>* ) {
		_contextStack.pop();
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::SysMethod>* node) {
		actualize(node);
				
		ensureNameNotDeclaredInBases(node);

		// here must be check for correct value of SysMethod
				
		return false;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Struct>* node) {
		actualize(node);
				
		_contextStack.push(OpTools::context(node));
		return true;
	}
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Struct>* ) {
		_contextStack.pop();
		return true;
	}
		
	virtual bool visitBefore(parser::OperationNode<parser::Operations::Enum>* node) {
		actualize(node);
				
		_contextStack.push(OpTools::context(node));
		return true;
	}
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Enum>* ) {
		_contextStack.pop();
		return true;
	}
		
	virtual bool visitBefore(parser::OperationNode<parser::Operations::StructEntry>* node) {
		actualize(node);
				
		bool typeResolved = resolve(node, ArgsLayout::StructEntry::Type, TYPE_EXPECTED); // type
		
		if (typeResolved) {
			assignTypeToValue(node, ArgsLayout::StructEntry::Type, ArgsLayout::StructEntry::Name);
			ensureNonVoid(node);
		}
		
		return false;
	}
	
	virtual bool visitBefore(parser::OperationNode<parser::Operations::EnumEntry>* node) {
		actualize(node);
				
		NameNode::Ptr name = OpTools::nameNode(node);
		
		ASSERT(name);
		ASSERT(name->getDescriptor().isValue());
		
		Type::Ptr type = dynamic_cast<Type*>(get_pointer(_contextStack.top()));
		ASSERT(type);
		
		name->getDescriptor().getValue()->setType(type);
		
		return true;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Typedef>* node) {
		bool typeResolved = resolve(node, ArgsLayout::Typedef::Type, TYPE_EXPECTED); // type
		
		if (typeResolved) {
			assignTypeToType(node, ArgsLayout::Typedef::Type, ArgsLayout::Typedef::Name);
			//ensureNonVoid(node);
		}

		actualize(node);
				
		return false;
	}
		
	virtual bool visitBefore(parser::OperationNode<parser::Operations::Typecast>* node) {
		bool typeResolved = resolve( node, ArgsLayout::Typecast::Type, TYPE_EXPECTED ); // type
		
		if ( typeResolved ) {
			assignTypeToType( node, ArgsLayout::Typecast::Type, ArgsLayout::Typecast::Expr );
			//ensureNonVoid(node);
		}

		actualize(node);
				
		return false;
	}
		
	virtual bool visitBefore(parser::OperationNode<parser::Operations::MethodArg>* node) {
		actualize(node);
				
		bool typeResolved = resolve(node, ArgsLayout::MethodArg::Type, TYPE_EXPECTED); // type
		
		if (typeResolved) {
			assignTypeToValue(node, ArgsLayout::MethodArg::Type, ArgsLayout::MethodArg::Name);
			ensureNonVoid(node);
		}
		
		return false;
	}
			
	virtual bool visitBefore(parser::OperationNode<parser::Operations::MessageClassExtend>* node) {
		actualize(node);
				
		MessageClassExtend::Ptr messageClassExtend = OpTools::context(node);
		
		//1. ensure that there is no named entity with the same name in inhereted interfaces
		//2. assign this type with all messages

		MessageClass::Ptr messageClass = ensureNameDeclaredInBases<MessageClass>( messageClassExtend->getCoreName(), messageClassExtend->getAltName() );
				
		if (!messageClass) {
			ErrorFactory::instance().messageClassNotFound(node->getContext(), messageClassExtend->getCoreName(), _contextStack.top());
			return false;
		}
		
		messageClassExtend->setMessageClass(messageClass);
		
		MessageClassVisitor visitor(messageClassExtend);
		OpTools::body(node)->visit(visitor);
		return false;
	}

	bool isCharPtr(Type::Ptr type) const {
		Type::Ptr reducedType = reduceTypedef(_root, type, false);
		
		if (RefType::Ptr ref = dynamic_pointer_cast<RefType>(reducedType)) {
			if (BasicType::Ptr innerType = dynamic_pointer_cast<BasicType>(ref->getType())) {
				return /*ref->isConst() &&*/ innerType->getTypeId() == parser::BasicTypeNode::SInt8 && ref->getCount() == 1;
			}
		}
		
		return false;
	}
	
	bool isPtr(Type::Ptr type) const {
		Type::Ptr reducedType = reduceTypedef(_root, type, false);
		
		if (RefType::Ptr ref = dynamic_pointer_cast<RefType>(reducedType)) {
			return true;
		}
		
		return false;
	}
	
	template <Operations::Op id>	
	bool checkForAcceptableTypeForConst(parser::OperationNode<id>* node) {
		ensureNonVoid(node);
		
		Type::Ptr type = OpTools::context(node)->getType();
		
		if (isPtr(type)) {
			return true;
		}
		
		BasicType::Ptr basicType = dynamic_pointer_cast<BasicType>(reduceTypedef(_root, type, false));
		
		if (!basicType) {
			ErrorFactory::instance().constMustBeOfBasicType(node->getContext());
		}
		return type;
	}

	template <Operations::Op id>	
	bool assignConst(parser::OperationNode<id>* node) {
		if (checkForAcceptableTypeForConst(node)) {
			NodeInterface::Ptr valueNode = OpTools::value(node);
			ASSERT(valueNode);

			Performer::Ptr performer = evalConst(valueNode, _consts);

			if (performer) {
				ConstValue::Ptr constValue = OpTools::context(node);
				ASSERT(constValue);
				
				constValue->setPerformer(performer);

				if (isPtr(constValue->getType()) && performer->type() == Performer::TypeInt) {
					if (performer->asInt() != 0) {
						ErrorFactory::instance().constValueForPtrMustBeZero(node->getContext(), OpTools::context(node)->getNamePath());
					}
				} else if (isCharPtr(constValue->getType())) {
					if (performer->type() != Performer::TypeString) {
						ErrorFactory::instance().constValueTypeMismatched(node->getContext(), OpTools::context(node)->getNamePath());
					}
				} else {
					if (performer->type() != Performer::TypeInt && performer->type() != Performer::TypeFloat) {
						ErrorFactory::instance().constValueTypeMismatched(node->getContext(), OpTools::context(node)->getNamePath());
					}
				}
				
				return true;
			}
		}
		return false;
	}
	
	
	virtual bool visitBefore(parser::OperationNode<parser::Operations::Const>* node) {
		bool typeResolved = resolve(node, ArgsLayout::Const::Type, TYPE_EXPECTED); // type
		bool valueResolved = resolve(node, ArgsLayout::Const::Value, VALUE_EXPECTED); // expr
		
		if (typeResolved) {
			assignTypeToValue(node, ArgsLayout::Const::Type, ArgsLayout::Const::Name);

			if (valueResolved && assignConst(node)) {
				ASSERT(_consts.find(OpTools::context(node)->getNamePath()) == _consts.end());
				_consts[OpTools::context(node)->getNamePath()] = OpTools::context(node)->getPerformer();
			}
			
		}
		
		actualize(node);
				
		return false;
	}
		
	virtual bool visitBefore(parser::OperationNode<parser::Operations::ImportName>* node) {
		// as it stands nothing prevents from extending message classes with the same name but within different interface
		ExternalType::Ptr type = OpTools::context(node);
		StringNode::Ptr file;
		node->getArg(ArgsLayout::ImportName::File, file);
		
		type->setFile(file->getValue());
		
		actualize(node);
		
		return false;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::ImportNameAsType>* node) {
		// as it stands nothing prevents from extending message classes with the same name but within different interface
		ExternalType::Ptr type = OpTools::context(node);
		StringNode::Ptr file;
		node->getArg(ArgsLayout::ImportNameAsType::File, file);
		
		type->setFile(file->getValue());
		
		bool typeResolved = resolve(node, ArgsLayout::ImportNameAsType::Type, TYPE_EXPECTED); // type
		
		if (typeResolved) {
			assignTypeToType(node, ArgsLayout::ImportNameAsType::Type, ArgsLayout::ImportNameAsType::Name);
		}
		
		actualize(node);
		
		return false;
	}

	/// !!!!!!!!!!! tools
	/// Search is only performed in bases, NOT IN `interface' ITSELF!!!
	/*NameContext::Ptr findWithinInteface(Interface::Ptr interface, const Name& name) const {
		ASSERT(interface);

		const Interface::Bases& bases = interface->getBases();
			
		BOOST_FOREACH(Interface::Ptr i, bases) {
			NameContext::Ptr result = i->find(name); 
			
			if (result) {
				return result;
			}
			
			result = findWithinInteface(i, name);
			
			if (result) {
				return result;
			}
		}
		
		//ASSERT(!result);
		return NULL;
	}*/
	
	
	NameContext::Ptr findWithinInteface( const Name& name, const Name& alt ) const {
		Interface::Ptr iface = dynamic_pointer_cast<Interface>( _contextStack.top() );
		ASSERT( iface );
		return iface->findWithinBases( name, alt );
	}
	
	
	template <class RequiredNameContext>
	typename RequiredNameContext::Ptr ensureNameDeclaredInBases( const Name& name, const Name& alt ) const {
		NameContext::Ptr nameCtx = findWithinInteface( name, alt );
		
		if ( !nameCtx ) {
			//ErrorFactory::instance().nameNotFound(node.getContext(), node.getName(), _contextStack.top());
			//ErrorFactory::... name not found
			return NULL; ///???
		}
		
		RequiredNameContext::Ptr requiredCtx = dynamic_pointer_cast<RequiredNameContext>( nameCtx );
		
		if ( !requiredCtx ) {
			//ErrorFactory::... name not of required type
			return NULL; ///???
		}
		
		return requiredCtx;
	}
	
	template <Operations::Op Op>
	bool ensureNameNotDeclaredInBases(typename parser::OperationNode<Op>* node) {
	//bool ensureNameNotDeclaredInBases(const Name& name) const {
		NameNode::Ptr nameNode = OpTools::nameNode( node );
		NameContext::Ptr prevCtx = findWithinInteface( nameNode->getCoreName(), nameNode->getAltName() );
		
		if ( prevCtx ) {
			ErrorFactory::instance().nameAlreadyDefined( node->getContext(), nameNode->getCoreName(), prevCtx->getDeclarationContext() );
			return false;
		}
		
		return true;
	}
		
	// ---
	NameContext::Ptr resolveBaseInterfaces( NodeInterface::Ptr node, bool imp ) {

		Tuple::Ptr bases;
		NameNode::Ptr nameNode;
		if ( imp ) {
			OperationNode<Operations::Implementation>::Ptr cnode = dynamic_pointer_cast<parser::OperationNode<parser::Operations::Implementation> >( node );
			ASSERT( cnode );
			cnode->getArg( ArgsLayout::Implementation::Name, nameNode );
			cnode->getArg( ArgsLayout::Implementation::Bases, bases );
		}
		else {
			OperationNode<Operations::Interface>::Ptr cnode = dynamic_pointer_cast<parser::OperationNode<parser::Operations::Interface> >( node );
			ASSERT( cnode );
			cnode->getArg( ArgsLayout::Interface::Name, nameNode );
			cnode->getArg( ArgsLayout::Interface::Bases, bases );
		}
		ASSERT( nameNode );
		ASSERT( bases );

		NameContext::Ptr ctx = nameNode->getDescriptor().getNameContext();
		ASSERT( ctx );

		Interface::Ptr iface = dynamic_pointer_cast<Interface>( ctx );
		ASSERT( iface );

		size_t count = bases->getSize();

		if ( imp && (!count || (count > 1)) ) {
			ErrorFactory::instance().implementaitionMustBeBasedOnSingleInterface( nameNode->getContext(), iface->getCoreName() );
			return 0;
		}

		if ( count != 0 ) {
			for ( size_t i = 0; i < bases->getSize(); i++ ) {
				NodeInterface::Ptr baseNode = bases->getNode(i);
				ASSERT(baseNode);
				
				if ( !resolve(baseNode) ) {
					ErrorFactory::instance().interfaceExpected( baseNode->getContext() );
					ctx = 0;
					continue;
				}
				
				Interface::Ptr baseInterface = dynamic_pointer_cast<Interface>( baseNode->getDescriptor().getNameContext() );
				if ( !baseInterface ) {
					ErrorFactory::instance().interfaceExpected( baseNode->getContext() );
					ctx = 0;
					continue;
				}
				iface->addBase( baseInterface );
			}
		} else { // we are here if !imp; if no base interfaces, it inherits from StdBase
			NameContext::Ptr std = _root->find( "StdBase", IDL_IF_PFX "StdBase" );
			Interface::Ptr stdBase = dynamic_pointer_cast<Interface>( std );
			if ( !stdBase ) {
				ErrorFactory::instance().nameNotFound( iface->getDeclarationContext(), "StdBase", _contextStack.top() );
				ctx = 0;
			}
			else if ( iface != stdBase ) // StdBase itself surely does not need to be inherited from anything
				iface->addBase(stdBase);
		}
		return ctx;
	}

	template <Operations::Op Op>
	bool ensureNonVoid(typename parser::OperationNode<Op>* node) {
		ASSERT(node);
		
		Type::Ptr type = OpTools::type(node);
		
		BasicType::Ptr t = dynamic_pointer_cast<BasicType>(reduceTypedef(_root, type, false));
		if (t && t->getTypeId() == BasicTypeNode::Void) {
			ErrorFactory::instance().voidNotAllowed(node->getContext());
			return false;
		}
		return true;
	}
		
};


class PropertyResolver : public Visitor {
public:

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Property>* node) {
		Property::Ptr prop = OpTools::context( node );
		ASSERT( prop );
		if ( !prop->isOptional() )
			_announcedProps.insert( prop );
		return false;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::PropertyPlugin>* node) {
		Property::Ptr prop = OpTools::context( node );
		if ( prop )
			_pluginProps.insert( prop );
		return false;
	}

	virtual bool visitBefore(parser::OperationNode<parser::Operations::Implementation>* node) {
		_announcedProps.clear();
		_pluginProps.clear();
		return true;
	}

	virtual bool visitAfter(parser::OperationNode<parser::Operations::Implementation>* node) {

		BOOST_FOREACH( Property::Ptr prop, _announcedProps ) {
			if ( _pluginProps.find(prop) != _pluginProps.end() )
				continue;
			Implementation::Ptr implementation = OpTools::context( node );
			NameContext::Ptr context = implementation->findWithinBases( prop->getCoreName(), prop->getAltName() );
			if ( dynamic_pointer_cast<Property>(context) ) {
				ErrorFactory::instance().requiredPropNotImplemented( node->getContext(), prop->getNamePath(), implementation->getNamePath() );
			}
		}
		return true;
	}

private:
	std::set<Property::Ptr>	_announcedProps;
	std::set<Property::Ptr>	_pluginProps;
};



void resolveNames(RootNameContext::Ptr contexts, parser::NodeInterface::Ptr root) {
	NameResolver resolver(contexts);
	bool done = root->visit( resolver );

	if ( done && resolver.getResult() ) {
		PropertyResolver propResolver;
		root->visit(propResolver);
	}
}

} // namespace idl


