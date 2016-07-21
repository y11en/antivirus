#include <stack>
#include <vector>
#include <boost/intrusive_ptr.hpp>

#include <boost/noncopyable.hpp>

#include "idl/Context.hpp"
#include "NameResolver.hpp"
#include "BasicType.hpp"
#include "Value.hpp"
#include "idl/ErrorFactory.hpp"
#include "idl/IntNode.hpp"
#include "idl/FloatNode.hpp"
#include "idl/StringNode.hpp"
#include "idl/NameNode.hpp"
#include "idl/OperationNode.hpp"
#include "RootNameContext.hpp"
#include "ConstEval.hpp"

/**
	Вычисление констант производится ДО резолвинга типов, но ПОСЛЕ сбора задекларированных имен.
	Константы могут быть только базовых типов. Это и позволяет выполнить всю работу до основного резолвинга.
	Класс EvaledConsts содержит внутри себя коллекцию вычисленных констант.
	Константу можно найти либо по имени, либо по NodeInterface::Ptr, т.к. не все константы имеют имена.
	
	Кстати, пока существует только два вида констант: именованые и неименованые в качестве аргумента в sequence'е.
	
	В случае с NodeInterface::Ptr используется узел с именем или узел с соответствующим контейнером.
*/


namespace idl {

using namespace parser;

/*class ExpressionStack;

struct Performer {
	typedef ExpressionStack Stack;
	typedef boost::shared_ptr<Performer> Ptr;
	virtual void perform(Stack& stack) {}

	enum TypeId {
		TypeInt,
		TypeFloat,
		TypeFunc
	};
	
	virtual TypeId type() const {
		return TypeFunc;
	}
			
	virtual Int asInt() const {
		ASSERT(false);
		return 0;
	}
	virtual Float asFloat() const {
		ASSERT(false);
		return 0;
	}
};*/

/**

*/

class ExpressionStack {
  public:
  	ExpressionStack() {
  		reset();
  	}
  	
  	void reset() {
  		_stack.clear();
  		_ptr = _stack.begin();
  	}
  
  	Performer::Ptr perform() {
  		ASSERT(!_stack.empty());
  		_ptr = _stack.begin();
  		++_ptr;

  		do {
  			if (top()->type() == Performer::TypeFunc) {
  				pop()->perform(*this);
  			} else {
  				if (_ptr != _stack.end()) {
  					++_ptr;
  				} else {
  					break;
  				}
  			}
  		} while(true);
  		
  		ASSERT(_stack.size() == 1);
  		return top();
  	}
  	
  	void push(Performer::Ptr performer) {
  		_ptr = _stack.insert(_ptr, performer);
  		ASSERT(_ptr != _stack.end());
  		++_ptr;
  	}
  	
  	Performer::Ptr top() const {
  		ASSERT(!_stack.empty());
  		ASSERT(_ptr != _stack.begin());
  		return *(_ptr - 1);
  	}
  	
  	Performer::Ptr pop() {
  		ASSERT(!_stack.empty());
  		ASSERT(_ptr != _stack.begin());
  		Performer::Ptr result = top();
  		_ptr = _stack.erase(_ptr - 1);
  		return result;
  	}
  	
  private:
  	typedef std::vector<Performer::Ptr>	Stack;

  	Stack			_stack;  	
	Stack::iterator	_ptr;		/// points to next element after current stack top
};


template <int id>
struct selector {
	template <class T>
	static void op(T& r, T a, T b) {
	}
};


#define DEF_SELECTOR(code, op)                                                        \
	template <>                                                                         \
	struct selector<parser::Operations::code> { template <class T> T select(T a, T b) { \
		return a op b;                                                                    \
	}};

#define DEF_SELECTOR_1(code, op)                                                      \
	template <>                                                                         \
	struct selector<parser::Operations::code> {                                         \
		template <class T> T select(T a) {                                                \
			return op a;                                                                    \
	}};

DEF_SELECTOR(Sub, -)
DEF_SELECTOR(Add, +)
DEF_SELECTOR(Mul, *)
DEF_SELECTOR(Div, /)
DEF_SELECTOR(BitAnd, &)
DEF_SELECTOR(BitOr, |)
DEF_SELECTOR(BitXor, ^)
DEF_SELECTOR(ShiftLeft, <<)
DEF_SELECTOR(ShiftRight, >>)
DEF_SELECTOR_1(Neg, -)
DEF_SELECTOR_1(BitInv, ~)

struct PerformerIntConst : public Performer {
	PerformerIntConst(Int value) : _value(value) {}
	
	virtual TypeId type() const {
		return TypeInt;
	}
			
	virtual Int asInt() const {
		return _value;
	}
	
	virtual Float asFloat() const {
		return static_cast<Float>(_value);
	}
  private:
  	Int _value;
};

struct PerformerFloatConst : public Performer {
	PerformerFloatConst(Float value) : _value(value) {}
	virtual Int asInt() const {
		//ASSERT(false);
		return static_cast<Int>(_value);
	}
	
	virtual TypeId type() const {
		return TypeFloat;
	}
			
	virtual Float asFloat() const {
		return _value;
	}
  private:
  	Float _value;
};

struct PerformerStringConst : public Performer {
	PerformerStringConst(const String& value) : _value(value) {}
	
	virtual TypeId type() const {
		return TypeString;
	}
			
	virtual const String& asString() const {
		return _value;
	}
	
  private:
  	String _value;
};

template <int id>
struct Performer1Template : public Performer {
	selector<id> select;

	virtual void perform(Stack& stack) {
		Ptr left = stack.pop();
		Ptr result;
		
		if (left->type() == TypeFloat) {
			result = Ptr(new PerformerFloatConst(select.select(left->asFloat())));
		} else {
			ASSERT(left->type() == TypeInt);
			result = Ptr(new PerformerIntConst(select.select(left->asInt())));
		}

		ASSERT(result);
					
		stack.push(result);
	}
};

template <int id>
struct Performer2Template : public Performer {
	selector<id> select;
	
	virtual void perform(Stack& stack) {
		Ptr right = stack.pop();
		Ptr left = stack.pop();
		Ptr result;
		
		if (left->type() == TypeFloat || right->type() == TypeFloat) {
			result = Ptr(new PerformerFloatConst(select.select(left->asFloat(), right->asFloat())));
		} else {
			result = Ptr(new PerformerIntConst(select.select(left->asInt(), right->asInt())));
		}
		ASSERT(result);
		stack.push(result);
	}
};

template <int id>
struct PerformerInt1Template : public Performer {
	selector<id> select;

	virtual void perform(Stack& stack) {
		Ptr left = stack.pop();
		Ptr result;
		
		//ASSERT(left->type() == TypeInt);
		result = Ptr(new PerformerIntConst(select.select(left->asInt())));
		
		ASSERT(result);
		stack.push(result);
	}
};

template <int id>
struct PerformerInt2Template : public Performer {
	selector<id> select;
	
	virtual void perform(Stack& stack) {
		Ptr right = stack.pop();
		Ptr left = stack.pop();
		Ptr result;
		
		//ASSERT(left->type() == TypeInt && right->type() == TypeInt);
		result = Ptr(new PerformerIntConst(select.select(left->asInt(), right->asInt())));
		
		ASSERT(result);
		stack.push(result);
	}
};

class EvalConsts : Visitor, boost::noncopyable {
	//typedef std::map<NamePath, Performer::Ptr>	EvaledConsts;
	EvaledConsts		_unknownConsts;
	const EvaledConsts&	_consts;
	ExpressionStack		_stack;
  public:	
	EvalConsts(const EvaledConsts& consts) : _consts(consts) {
	}

	Performer::Ptr eval(NodeInterface::Ptr node) {
		_stack.reset();
		node->visit(*this);
		return _stack.perform();
	}
		
  private:
  	#define VISIT_OP(kind, op)                                                           \
		virtual bool visitAfter(parser::OperationNode<parser::Operations::op>* ) {       \
			_stack.push(Performer::Ptr(new Performer##kind##Template<parser::Operations::op>)); \
			return true;                                                                 \
		}
  	
	VISIT_OP(2, Add)
	VISIT_OP(2, Sub)
	VISIT_OP(2, Mul)
	VISIT_OP(2, Div)
		
	VISIT_OP(Int2, BitAnd)
	VISIT_OP(Int2, BitOr)
	VISIT_OP(Int2, BitXor)
		
	VISIT_OP(Int2, ShiftLeft)
	VISIT_OP(Int2, ShiftRight)
		
	VISIT_OP(Int1, BitInv)
	VISIT_OP(1, Neg)

	#undef VISIT_OP
	#undef VISIT_OP
			  	
	/*virtual bool visitAfter(parser::OperationNode<parser::Operations::Add>* ) {
		_stack.push(Performer::Ptr(new Performer2Template<parser::Operations::Add>));
		return true;
	}

	virtual bool visitAfter(parser::OperationNode<parser::Operations::Sub>* ) {
		_stack.push(Performer::Ptr(new Performer2Template<parser::Operations::Sub>));
		return true;
	}

	virtual bool visitAfter(parser::OperationNode<parser::Operations::Mul>* ) {
		_stack.push(Performer::Ptr(new Performer2Template<parser::Operations::Mul>));
		return true;
	}

	virtual bool visitAfter(parser::OperationNode<parser::Operations::Div>* ) {
		_stack.push(Performer::Ptr(new Performer2Template<parser::Operations::Div>));
		return true;
	}*/

	virtual bool visitBefore(IntNode* node) {
		_stack.push(Performer::Ptr(new PerformerIntConst(node->getValue())));
		return false;
	}
	
	virtual bool visitBefore(FloatNode* node) {
		_stack.push(Performer::Ptr(new PerformerFloatConst(node->getValue())));
		return false;
	}

	virtual bool visitBefore(StringNode* node) {
		_stack.push(Performer::Ptr(new PerformerStringConst(node->getValue())));
		return false;
	}

	void findName(const Context& context, const NameContext& nameContext) {
		EvaledConsts::const_iterator i = _consts.find(nameContext.getNamePath());

		Performer::Ptr performer;
		
		if (i == _consts.end()) {
			i = _unknownConsts.find(nameContext.getNamePath());
			if (i == _unknownConsts.end()) {
				ErrorFactory::instance().unknownConstName(context, nameContext.getNamePath());
				_unknownConsts[nameContext.getNamePath()] = Performer::Ptr(new PerformerIntConst(0));
			}
			performer = _unknownConsts[nameContext.getNamePath()];
		} else {
			performer = i->second;
		}
		ASSERT(performer);
		_stack.push(performer);
	}
	
	virtual bool visitBefore(parser::OperationNode<parser::Operations::Dot>* node) {
		ASSERT(node->getDescriptor().getNameContext());
		findName(node->getContext(), *(node->getDescriptor().getNameContext()));
		return false; // skip internals
	}
	
	virtual bool visitBefore(NameNode* node) {
		ASSERT(node->getDescriptor().getNameContext());
		findName(node->getContext(), *(node->getDescriptor().getNameContext()));
		return false;
	}
};

Performer::Ptr evalConst(parser::NodeInterface::Ptr node, const EvaledConsts& consts) {
	EvalConsts evalConsts(consts);
	return evalConsts.eval(node);
}

} // namespace idl
