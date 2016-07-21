#include "precompiled.h"
#include "expression.h"

namespace KasperskyLicensing {
namespace Implementation {


//-----------------------------------------------------------------------------

namespace{

double negate(double a)				
	{ return !a; }
double mult(double a, double b)		
	{ return a * b; }
double divide(double a, double b)	
	{ 
		if (b == 0.0)
			throw std::overflow_error("division by 0 detected");
		return a / b; 
	}
double summ(double a, double b)		
	{ return a + b; }
double subtr(double a, double b)	
	{ return a - b; }
double less(double a, double b)		
	{ return a < b; }
double less_or_equal(double a, double b)
	{ return a <= b; }
double greater(double a, double b)	
	{ return a > b; }
double greater_or_equal(double a, double b)	
	{ return a >= b; }
double equal(double a, double b)	
	{ return a == b; }
double not_equal(double a, double b)	
	{ return a != b; }
double and(double a, double b)	
	{ return a && b; }
double or(double a, double b)	
	{ return a || b; }

}

//-----------------------------------------------------------------------------

unsigned operation::precedence() const
{
	unsigned prec[] = 
	{
		0,	// op_unknown
		1,	// op_opening_bracket
		1,	// op_closing_bracket
		8,	// op_negation
		7,	// op_multiplication
		7,	// op_division
		6,	// op_addition
		6,	// op_subtraction
		5,	// op_less
		5,	// op_less_or_equal
		5,	// op_greater
		5,	// op_greater_or_equal
		4,	// op_equal
		4,	// op_not_equal
		3,	// op_and
		2	// op_or
	};

	return prec[op_type];
}

//-----------------------------------------------------------------------------

void number::accept(evaluator& eval) const
{
	eval.process(*this); 
}


//-----------------------------------------------------------------------------

void variable::accept(evaluator& eval) const
{
	eval.process(*this); 
}

//-----------------------------------------------------------------------------

void unary_operator::accept(evaluator& eval) const
{ 
	eval.process(*this); 
}

//-----------------------------------------------------------------------------

void binary_operator::accept(evaluator& eval) const
{ 
	eval.process(*this); 
}

//-----------------------------------------------------------------------------

element* make_operator(const operation& op)
{
	switch (op.type())
	{
	case operation::op_negation:
		return new unary_operator(negate);
		break;
	case operation::op_multiplication:
		return new binary_operator(mult);
		break;
	case operation::op_division:
		return new binary_operator(divide);
		break;
	case operation::op_addition:
		return new binary_operator(summ);
		break;
	case operation::op_subtraction:
		return new binary_operator(subtr);
		break;
	case operation::op_less:
		return new binary_operator(less);
		break;
	case operation::op_less_or_equal:
		return new binary_operator(less_or_equal);
		break;
	case operation::op_greater:
		return new binary_operator(greater);
		break;
	case operation::op_greater_or_equal:
		return new binary_operator(greater_or_equal);
		break;
	case operation::op_equal:
		return new binary_operator(equal);
		break;
	case operation::op_not_equal:
		return new binary_operator(not_equal);
		break;
	case operation::op_and:
		return new binary_operator(and);
		break;
	case operation::op_or:
		return new binary_operator(or);
		break;
	}

	throw std::runtime_error("unknown operation");
}

//-----------------------------------------------------------------------------

evaluator::evaluator(const calculation_context& c) : context(c)
{
}

//-----------------------------------------------------------------------------

double evaluator::get_value()
{
	return opstack.top();
}

//-----------------------------------------------------------------------------

void evaluator::process(const unary_operator& op)
{
	double a = opstack.top(); opstack.pop();

	opstack.push(op(a));
}

//-----------------------------------------------------------------------------

void evaluator::process(const binary_operator& op)
{
	double b = opstack.top(); opstack.pop();
	double a = opstack.top(); opstack.pop();

	opstack.push(op(a, b));
}

//-----------------------------------------------------------------------------

void evaluator::process(const number& num)
{
	opstack.push(num.get_value());
}

//-----------------------------------------------------------------------------

void evaluator::process(const variable& v)
{
	opstack.push(resolve_variable(v.type()));
}

//-----------------------------------------------------------------------------

double evaluator::resolve_variable(variable::variable_type vt) const
{
	double res = 0;

	switch(vt)
	{
	case variable::vt_unknown:
		break;
	case variable::vt_current_value:
		res = context.cur_value;
		break;
	case variable::vt_nominal_value:
		res = context.nom_value;
		break;
	case variable::vt_invalid_avdb:
		res = context.is_invalid_avdb;
		break;
	case variable::vt_bad_blacklist:
		res = context.is_invalid_blst;
		break;
	case variable::vt_sn_blacklisted:
		res = context.is_sn_blisted;
		break;
	case variable::vt_expiration_date:
		res = context.exp_date;
		break;
	case variable::vt_current_date:
		res = context.cur_date;
		break;
	case variable::vt_avdb_update_date:
		res = context.update_date;
		break;
	case variable::vt_true:
		res = 1;
		break;
	case variable::vt_false:
		res = 0;
		break;
	}
	return res;
}

//-----------------------------------------------------------------------------

void expression::parse(const std::string& data)
{
	std::string::const_iterator it(data.begin()), end(data.end());

	while (it != end && *it)
	{
		// skip whitespaces
		if (*it == ' ')
		{
			skipws(it, end);
		}
		// if current token is operation
		else if(isoperation(*it))
		{
			process_operation(get_operation(it, end));
		}
		// start of number
		else if(isdigit(*it))
		{
			process_number(get_number(it, end));
		}
		// start of variable
		else if (*it == '%')
		{
			process_variable(get_variable(it, end));
		}
		// unknown token
		else
			throw std::runtime_error("syntax error");
	}

	for (; !opstack.empty(); opstack.pop())
	{
		// unbalanced bracket case
		if (opstack.top().type() == operation::op_opening_bracket)
			throw std::runtime_error("unbalanced bracket found");
		// copy the rest operations to rpn
		rpn.push_back(make_operator(opstack.top()));
	}
}

//-----------------------------------------------------------------------------

void expression::skipws(std::string::const_iterator& it, const std::string::const_iterator& end)
{
	for(; it != end && *it == ' '; ++it);
}

//-----------------------------------------------------------------------------

operation::operation_type expression::get_operation(char token)
{
	struct entry
	{
		char	symbol;
		operation::operation_type oper;
	};

	entry trans[] = 
	{
		{'(', operation::op_opening_bracket},
		{')', operation::op_closing_bracket},
		{'!', operation::op_negation},
		{'*', operation::op_multiplication},
		{'/', operation::op_division},
		{'+', operation::op_addition},
		{'-', operation::op_subtraction},
		{'<', operation::op_less},
		{'>', operation::op_greater},
		{'=', operation::op_equal},
		{'&', operation::op_and},
		{'|', operation::op_or}
	};

	unsigned it(0), end(sizeof(trans)/sizeof(trans[0]));

	for(; it != end; ++it)
	{
		if (trans[it].symbol == token)
			return trans[it].oper;
	}

	return operation::op_unknown;
}

//-----------------------------------------------------------------------------

bool expression::isoperation(char token)
{
	return get_operation(token) != operation::op_unknown;
}

//-----------------------------------------------------------------------------

operation::operation_type expression::get_operation(
	std::string::const_iterator& it, 
	const std::string::const_iterator& end
)
{
	operation::operation_type op = get_operation(*it);
	// check for two-char operations
	if (++it != end && get_operation(*it) == operation::op_equal)
	{
		switch(op) 
		{
		case operation::op_negation:
			op = operation::op_not_equal; ++it;
			break;
		case operation::op_less:
			op = operation::op_less_or_equal; ++it;
			break;
		case operation::op_greater:
			op = operation::op_greater_or_equal; ++it;
			break;
		}
	}
	return op;
}

//-----------------------------------------------------------------------------

double expression::get_number(std::string::const_iterator& it, const std::string::const_iterator& end)
{
	double num = 0, exp = 1;
	std::vector<char> part1, part2;
	enum { st_part1, st_part2 } state = st_part1;
	
	while(it != end)
	{
		if (isdigit(*it))
		{
			switch(state) 
			{
			case st_part1:
				part1.push_back(*it);
				break;
			case st_part2:
				part2.push_back(*it);
				exp *= 10;
				break;
			}
		}
		else if(*it == '.')
		{
			if (state == st_part2)
				break;
			state = st_part2;
		}
		else
			break;
		++it;
	}
	// convert first part
	if (!part1.empty())
	{
		part1.push_back(0);
		num = atoi(&part1[0]);
	}
	// convert second part
	if (!part2.empty())
	{
		part2.push_back(0);
		num += (double)(atoi(&part2[0])) / exp;
	}

	return num;
}

//-----------------------------------------------------------------------------

variable::variable_type expression::get_variable(
	std::string::const_iterator& it, 
	const std::string::const_iterator& end
)
{
	std::string::const_iterator begin(it);
	while(++it != end && *it != '%');

	if (it == end)
		throw std::runtime_error("syntax error");

	std::string var(begin, ++it);
	// convert to uppercase
	std::transform(var.begin(), var.end(), var.begin(), std::ptr_fun(toupper));
	variable::variable_type vt = variable::vt_unknown;

	// translate variable type
	if (var == "%BAD_BLST%")
		vt = variable::vt_bad_blacklist;
	else if (var == "%INVALID_AV_BASES%")
		vt = variable::vt_invalid_avdb;
	else if (var == "%SN_BLSTED%")
		vt = variable::vt_sn_blacklisted;
	else if (var == "%LICENSE_EXPIRATION_DATE%")
		vt = variable::vt_expiration_date;
	else if (var == "%CURRENT_DATE%")
		vt = variable::vt_current_date;
	else if (var == "%AV_BASES_UPDATE_DATE%")
		vt = variable::vt_avdb_update_date;
	else if (var == "%CURRENT_VALUE%")
		vt = variable::vt_current_value;
	else if (var == "%VALUE%")
		vt = variable::vt_nominal_value;
	else if (var == "%TRUE%")
		vt = variable::vt_true;
	else if (var == "%FALSE%")
		vt = variable::vt_false;
	else
		throw std::runtime_error("syntax error");

	return vt;
}

//-----------------------------------------------------------------------------

void expression::process_operation(operation::operation_type op_type)
{
	operation op(op_type);

	// closing bracket must match opening bracket
	if (op.type() == operation::op_closing_bracket)
	{
		// find corresponding opening bracket
		while (!(opstack.empty() || opstack.top().type() == operation::op_opening_bracket))
		{
			// push operations to rpn
			rpn.push_back(make_operator(opstack.top()));
			opstack.pop();
		}
		// stack is empty but no opening bracket found
		if (opstack.empty())
			throw std::runtime_error("unbalanced bracket found");
		// opening bracket is cancelled
		opstack.pop();
	}
	// if stack is empty then operation is pushed into stack
	// opening brackets are always pushed into stack
	else if (opstack.empty() || op.type() == operation::op_opening_bracket)
	{
		opstack.push(op);
	}
	// operations with higher precedence are pushed into stack
	else if(opstack.top().precedence() < op.precedence())
	{
		opstack.push(op);
	}
	// operation with the same or lower precedence
	else
	{
		// same or lower precedence loop
		while(!opstack.empty() && opstack.top().precedence() >= op.precedence())
		{
			// operations from stack are pushed into rpn
			rpn.push_back(make_operator(opstack.top()));
			opstack.pop();
		}
		// current operation is pushed into rpn
		opstack.push(op);
	}
}

//-----------------------------------------------------------------------------

void expression::process_number(double num)
{
	rpn.push_back(new number(num));
}

//-----------------------------------------------------------------------------

void expression::process_variable(variable::variable_type vt)
{
	rpn.push_back(new variable(vt));
}

//-----------------------------------------------------------------------------

double expression::evaluate(const calculation_context& context) const
{
	rpn_t::const_iterator it(rpn.begin()), end(rpn.end());

	evaluator v(context);

	for (; it != end; ++it)
		(*it)->accept(v);

	return v.get_value();
}

//-----------------------------------------------------------------------------

void expression::clear()
{
	// clear rpn
	rpn.clear();
	// clear operations stack
	for (; !opstack.empty(); opstack.pop());
}

//-----------------------------------------------------------------------------

} // namespace Implementation
} // namespace KasperskyLicensing
