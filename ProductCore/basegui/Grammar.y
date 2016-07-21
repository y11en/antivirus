// afl_grammar.y
//

%{
#define PARSER  (*((cParser *)parser))
#define CHECK_NODE(NODE) if ((NODE) == 0) YYABORT
%}

%pure_parser

%name-prefix="my_"

%start start_expr

%token	TOK_IDENT
%token  TOK_NUMBER
%token	TOK_STRING
%token  TOK_VAR

%token  TOK_IF
%token  TOK_ELSE
%token  TOK_LOGIC_AND TOK_LOGIC_OR
%token	TOK_EQ TOK_NE
%token	TOK_LE TOK_GE

%token	TOK_LSHIFT
%token	TOK_RSHIFT

// if-then-else shift/reduce conflict
// %expect 1

%%

//statement_list
//	: statement                     { PARSER.make_expression(0, $1); }
//	| statement_list statement      { PARSER.make_expression($1, $2); }
//	;

//statement
//	: ';'
//	| top_expr ';'
//	| TOK_IF '(' top_expr ')' statement                    { CHECK_NODE($$ = PARSER.make_ternary_op(ntIF, $1, $3, $5)); }
//	| TOK_IF '(' top_expr ')' statement TOK_ELSE statement { CHECK_NODE($$ = PARSER.make_ternary_op(ntIF, $1, $3, $5)); }
//	| '{' statement_list '}'                               { $$ = $2; }
//	;

start_expr
	: top_expr                      { PARSER.make_expression($1); }
	;

top_expr
	: assign_expr
	| top_expr ',' assign_expr      { CHECK_NODE($$ = PARSER.make_sequence($1, $3)); }
	| top_expr ';' assign_expr      { CHECK_NODE($$ = PARSER.make_sequence($1, $3)); }
	;

assign_expr
	: cond_expr
	| index_expr '=' assign_expr    { CHECK_NODE($$ = PARSER.make_binary_op(ntEQ, $1, $3)); }
	;

cond_expr
	: logic_expr
	| logic_expr '?' assign_expr ':' assign_expr { CHECK_NODE($$ = PARSER.make_ternary_op(ntIF, $1, $3, $5)); }
	;

logic_expr
	: bit_expr
	| bit_expr TOK_LOGIC_AND logic_expr        { CHECK_NODE($$ = PARSER.make_binary_op(ntLogicAnd, $1, $3)); }
	| bit_expr TOK_LOGIC_OR logic_expr         { CHECK_NODE($$ = PARSER.make_binary_op(ntLogicOr, $1, $3)); }
	;
	
bit_expr
	: cmp_expr
	| cmp_expr '&' bit_expr         { CHECK_NODE($$ = PARSER.make_binary_op(ntBitAnd, $1, $3)); }
	| cmp_expr '^' bit_expr         { CHECK_NODE($$ = PARSER.make_binary_op(ntBitXor, $1, $3)); }
	| cmp_expr '|' bit_expr         { CHECK_NODE($$ = PARSER.make_binary_op(ntBitOr, $1, $3)); }
	;

cmp_expr
	: shift_expr
	| shift_expr TOK_EQ shift_expr  { CHECK_NODE($$ = PARSER.make_binary_op(ntCmpEQ, $1, $3)); }
	| shift_expr TOK_NE shift_expr  { CHECK_NODE($$ = PARSER.make_binary_op(ntCmpNE, $1, $3)); }
	| shift_expr '<' shift_expr     { CHECK_NODE($$ = PARSER.make_binary_op(ntCmpLT, $1, $3)); }
	| shift_expr '>' shift_expr     { CHECK_NODE($$ = PARSER.make_binary_op(ntCmpGT, $1, $3)); }
	| shift_expr TOK_LE shift_expr  { CHECK_NODE($$ = PARSER.make_binary_op(ntCmpLE, $1, $3)); }
	| shift_expr TOK_GE shift_expr  { CHECK_NODE($$ = PARSER.make_binary_op(ntCmpGE, $1, $3)); }
	;
	
shift_expr
	: add_expr
	| add_expr TOK_LSHIFT add_expr  { CHECK_NODE($$ = PARSER.make_binary_op(ntShiftLeft, $1, $3)); }
	| add_expr TOK_RSHIFT add_expr  { CHECK_NODE($$ = PARSER.make_binary_op(ntShiftRight, $1, $3)); }
	;

add_expr
	: mul_expr
	| add_expr '+' mul_expr         { CHECK_NODE($$ = PARSER.make_binary_op(ntAdd, $1, $3)); }
	| add_expr '-' mul_expr         { CHECK_NODE($$ = PARSER.make_binary_op(ntSub, $1, $3)); }
	;

mul_expr
	: neg_expr
	| mul_expr '*' neg_expr         { CHECK_NODE($$ = PARSER.make_binary_op(ntMul, $1, $3)); }
	| mul_expr '/' neg_expr         { CHECK_NODE($$ = PARSER.make_binary_op(ntDiv, $1, $3)); }
	| mul_expr '%' neg_expr         { CHECK_NODE($$ = PARSER.make_binary_op(ntRem, $1, $3)); }
	;

neg_expr
	: index_expr
	| '-' index_expr                { CHECK_NODE($$ = PARSER.make_unary_op(ntNeg, $2)); }
	| '!' index_expr                { CHECK_NODE($$ = PARSER.make_unary_op(ntLogicNeg, $2)); }
	| '~' index_expr                { CHECK_NODE($$ = PARSER.make_unary_op(ntBitNeg, $2)); }
	;

index_expr
	: struct_expr
	| struct_expr '[' top_expr ']'  { CHECK_NODE($$ = PARSER.make_index_op($1, $3)); }
	;

struct_expr
	: prim_expr
	| index_expr '.' TOK_IDENT      { CHECK_NODE($$ = PARSER.make_var_op($1, $3)); }
	| index_expr '.' TOK_IDENT '(' ')'                                                                                              { CHECK_NODE($$ = PARSER.make_func_op($3, $1)); }
	| index_expr '.' TOK_IDENT '(' assign_expr ')'                                                                                  { CHECK_NODE($$ = PARSER.make_func_op($3, $1, $5)); }
	| index_expr '.' TOK_IDENT '(' assign_expr ',' assign_expr ')'                                                                  { CHECK_NODE($$ = PARSER.make_func_op($3, $1, $5, $7)); }
	| index_expr '.' TOK_IDENT '(' assign_expr ',' assign_expr ',' assign_expr ')'                                                  { CHECK_NODE($$ = PARSER.make_func_op($3, $1, $5, $7, $9)); }
	| index_expr '.' TOK_IDENT '(' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ')'                                  { CHECK_NODE($$ = PARSER.make_func_op($3, $1, $5, $7, $9, $11)); }
	| index_expr '.' TOK_IDENT '(' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ')'                  { CHECK_NODE($$ = PARSER.make_func_op($3, $1, $5, $7, $9, $11, $13)); }
	| index_expr '.' TOK_IDENT '(' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ')'  { CHECK_NODE($$ = PARSER.make_func_op($3, $1, $5, $7, $9, $11, $13, $15)); }
	;

prim_expr
	: func_expr
	| TOK_IF '(' assign_expr ',' assign_expr ')'                 { CHECK_NODE($$ = PARSER.make_ternary_op(ntIF, $3, $5, 0)); }
	| TOK_IF '(' assign_expr ',' assign_expr ',' assign_expr ')' { CHECK_NODE($$ = PARSER.make_ternary_op(ntIF, $3, $5, $7)); }
	| '(' top_expr ')'              { $$ = $2; }
	| TOK_IDENT                     { CHECK_NODE($$ = PARSER.make_var_op(0, $1)); }
	| TOK_NUMBER
	| TOK_STRING
	;

func_expr
	: TOK_IDENT '(' ')'                                                                                                             { CHECK_NODE($$ = PARSER.make_func_op($1)); }
	| TOK_IDENT '(' assign_expr ')'                                                                                                 { CHECK_NODE($$ = PARSER.make_func_op($1, $3)); }
	| TOK_IDENT '(' assign_expr ',' assign_expr ')'                                                                                 { CHECK_NODE($$ = PARSER.make_func_op($1, $3, $5)); }
	| TOK_IDENT '(' assign_expr ',' assign_expr ',' assign_expr ')'                                                                 { CHECK_NODE($$ = PARSER.make_func_op($1, $3, $5, $7)); }
	| TOK_IDENT '(' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ')'                                                 { CHECK_NODE($$ = PARSER.make_func_op($1, $3, $5, $7, $9)); }
	| TOK_IDENT '(' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ')'                                 { CHECK_NODE($$ = PARSER.make_func_op($1, $3, $5, $7, $9, $11)); }
	| TOK_IDENT '(' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ')'                 { CHECK_NODE($$ = PARSER.make_func_op($1, $3, $5, $7, $9, $11, $13)); }
	| TOK_IDENT '(' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ',' assign_expr ')' { CHECK_NODE($$ = PARSER.make_func_op($1, $3, $5, $7, $9, $11, $13, $15)); }
	;