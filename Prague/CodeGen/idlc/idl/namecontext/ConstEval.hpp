#ifndef namecontext_ConstEval_hpp
#define namecontext_ConstEval_hpp

#include <map>
#include <boost/shared_ptr.hpp>

#include "NodeInterface.hpp"

namespace idl {

class ExpressionStack;

struct Performer {
	typedef scaner::Int Int;
	typedef scaner::Float Float;
	typedef scaner::String String;

	typedef ExpressionStack Stack;
	typedef boost::shared_ptr<Performer> Ptr;
	virtual void perform(Stack& /*stack*/) {}

	enum TypeId {
		TypeInt,
		TypeFloat,
		TypeString,
		TypeFunc,
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
	
	virtual const String& asString() const {
		ASSERT(false);
		static String empty;
		return empty;
	}
};

typedef std::map<NamePath, Performer::Ptr>	EvaledConsts;

extern Performer::Ptr evalConst(parser::NodeInterface::Ptr root, const EvaledConsts& result);

} //namespace idl

#endif //namecontext_ConstEval_hpp
