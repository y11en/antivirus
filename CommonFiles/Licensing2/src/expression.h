#ifndef EXPRESSION_H
#define EXPRESSION_H

namespace KasperskyLicensing {
namespace Implementation {


/**
 * Represents calculation context.
 */
struct calculation_context
{
	/**
	 * AV database is invalid.
	 */
	bool is_invalid_avdb;

	/**
	 * Black list is invalid.
	 */
	bool is_invalid_blst;

	/**
	 * Serial number is in the black list.
	 */
	bool is_sn_blisted;

	/**
	 * Current value of license restriction.
	 */
	unsigned cur_value;

	/**
	 * Nominal value of license restriction.
	 */
	unsigned nom_value;

	/**
	 * AV database update date.
	 */
	time_t	update_date;

	/**
	 * Current date.
	 */
	time_t	cur_date;

	/**
	 * License expiration date.
	 */
	time_t	exp_date;

};

/**
 * Represents operation of expression.
 */
class operation
{
public:

	/**
	 * Type of operation.
	 */
	enum operation_type
	{
		op_unknown,
		op_opening_bracket,
		op_closing_bracket,
		op_negation,
		op_multiplication,
		op_division,
		op_addition,
		op_subtraction,
		op_less,
		op_less_or_equal,
		op_greater,
		op_greater_or_equal,
		op_equal,
		op_not_equal,
		op_and,
		op_or
	};
	
	/**
	 * Constructs object.
	 */
	operation(operation_type ot) 
		: op_type(ot) {}

	/**
	 * Returns operation precedence index.
	 */
	unsigned precedence() const;

	/**
	 * Returns type of operation.
	 */
	operation_type type() const
		{ return op_type; }

private:

	// operation type
	operation_type op_type;

};

class evaluator;

/**
 * Represents element of expression.
 */
class element
{
public:

	/**
	 * Accepts evaluator (visitor pattern).
	 */
	virtual void accept(evaluator&) const = 0;

	/**
	 * Destroys object.
	 */
	virtual ~element() {}
};

/**
 * Represents numeric constant.
 */
class number : public element
{
public:

	/**
	 * Constructs object.
	 */
	number(double n) 
		: num(n) {}

	/**
	 * Accepts evaluator (visitor pattern).
	 */
	virtual void accept(evaluator& eval) const;

	/**
	 * Returns value of numeric constant.
	 */
	double get_value() const
		{ return num; }

private:

	// stores value
	double num;
	
};

/**
 * Represents environment variable.
 */
class variable : public element
{
public:
	/**
	 * Type of environment variable.
	 */
	enum variable_type
	{
		vt_unknown,				
		vt_current_value,		// %CURRENT_VALUE%
		vt_nominal_value,		// %VALUE%
		vt_invalid_avdb,		// %INVALID_AV_BASES%
		vt_bad_blacklist,		// %BAD_BLST%
		vt_sn_blacklisted,		// %SN_BLSTED%
		vt_expiration_date,		// %LICENSE_EXPIRATION_DATE%
		vt_current_date,		// %CURRENT_DATE%
		vt_avdb_update_date,	// %AV_BASES_UPDATE_DATE%
		vt_true,				// %TRUE%
		vt_false				// %FALSE%
	};

	/**
	 * Constructs object.
	 */
	variable(variable_type vt) 
		: var_type(vt) {}

	/**
	 * Accepts evaluator (visitor pattern).
	 */
	virtual void accept(evaluator& eval) const;

	/**
	 * Returns type of the variable.
	 */
	variable_type type() const
		{ return var_type; }

private:

	// type of the variable
	variable_type var_type;

};

/**
 * Represents unary operator.
 */
class unary_operator : public element
{
public:

	/**
	 * Unary function type.
	 */
	typedef double (*fun_t)(double);

	/**
	 * Constructs object.
	 */
	unary_operator(fun_t f) 
		: fun(f) {}

	/**
	 * Accepts evaluator (visitor pattern).
	 */
	virtual void accept(evaluator& eval) const;

	/**
	 * Preforms operation by calling stored function.
	 */
	double operator() (double a) const
		{ return fun(a); }
	
private:

	// operator function
	fun_t fun;

};

/**
 * Represents binary operator.
 */
class binary_operator : public element
{
public:

	/**
	 * Binary function type.
	 */
	typedef double (*fun_t)(double, double);

	/**
	 * Constructs object.
	 */
	binary_operator(fun_t f) 
		: fun(f) {}

	/**
	 * Accepts evaluator (visitor pattern).
	 */
	virtual void accept(evaluator& eval) const;

	/**
	 * Preforms operation by calling stored function.
	 */
	double operator() (double a, double b) const
		{ return fun(a, b); }
	
private:

	// operator function
	fun_t fun;

};

/**
 * Factory function makes operator by given operation.
 */
element* make_operator(const operation& op);

/**
 * Calculates value visiting rpn nodes.
 */
class evaluator
{
public:

	/**
	 * Constructs object.
	 */
	evaluator(const calculation_context& c);

	/**
	 * Returns value for top operations stack.
	 */
	double get_value();

	/**
	 * Processes unary operator element.
	 */
	void process(const unary_operator&);

	/**
	 * Processes binary operator element.
	 */
	void process(const binary_operator&);

	/**
	 * Processes variable element.
	 */
	void process(const variable&);

	/**
	 * Processes numeric constant element.
	 */
	void process(const number&);

private:

	// stores calculation context
	const calculation_context& context;

	// resolves value of environment variable
	double resolve_variable(variable::variable_type vt) const;

	// operations stack
	std::stack<double> opstack;
};

/**
 * Represents expression abstraction.
 */
class expression
{
public:

	/**
	 * Parces expression string.
	 */
	void parse(const std::string& s);

	/**
	 * Evaluates expression.
	 */
	double evaluate(const calculation_context& context) const;

	/**
	 * Clears current expression.
	 */
	void clear();

private:

	// processes operation
	void process_operation(operation::operation_type op_type);

	// processes number
	void process_number(double num);

	// processes variable
	void process_variable(variable::variable_type vt);

	// returns true if token is an operation
	static bool isoperation(char token);

	// extracts operation
	static operation::operation_type get_operation(
		std::string::const_iterator& it, 
		const std::string::const_iterator& end
	);

	// extracts part of the operation
	static operation::operation_type get_operation(char token);

	// extracts number
	static double get_number(std::string::const_iterator& it, const std::string::const_iterator& end);

	// extracts variable
	static variable::variable_type get_variable(
		std::string::const_iterator& it, 
		const std::string::const_iterator& end
	);

	// skips whitespaces
	static void skipws(std::string::const_iterator& it, const std::string::const_iterator& end);

	// stack of operations
	std::stack<operation> opstack;

	// reversed polish notation type
	typedef pointainer<std::vector<element*> > rpn_t;

	// reversed polish notation
	rpn_t rpn;
};

} // namespace Implementation
} // namespace KasperskyLicensing


#endif // EXPRESSION_H