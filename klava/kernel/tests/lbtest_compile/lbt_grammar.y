// kt_grammar.y
//

%{
#define COMPILER (((LbtParser*)parser)->compiler())
%}

%pure_parser

%locations
%name-prefix="lbt_"

%start test

%token	TOK_NUM
%token	TOK_STRING
%token	TOK_IDENT

%token	TOK_TRAP  TOK_HUB   TOK_FRAME  TOK_CALL   TOK_CALL0 TOK_CALL1  TOK_CALL2  TOK_CALL3
        TOK_AND2  TOK_AND3  TOK_AND4   TOK_ANDN   TOK_SEQ2  TOK_SEQ3   TOK_SEQ4   TOK_SEQN
        TOK_PUSH1 TOK_PUSH2 TOK_PUSH3  TOK_PUSHN  TOK_STKOP

        TOK_EQ0   TOK_NE0   TOK_EQ16   TOK_NE16   TOK_CMP32 TOK_CMP64  TOK_CMP
        TOK_LT16  TOK_LE16  TOK_GT16   TOK_GE16   TOK_LTS16 TOK_LES16  TOK_GTS16  TOK_GES16
        TOK_RD8   TOK_RD16  TOK_RD32   TOK_RD64   TOK_RDX8  TOK_RDX16  TOK_RDX32  TOK_RDX64
        TOK_RDS8  TOK_RDS16 TOK_RDS32  TOK_RDSLOT TOK_RDXS8 TOK_RDXS16 TOK_RDXS32 TOK_RDCX

%token  TOK_NOP   TOK_ADD   TOK_ADDN   TOK_ADDS  TOK_ADDS2  TOK_ADDS4  TOK_ADDS8  TOK_SUB
%token            TOK_MUL   TOK_MULN             TOK_DIV    TOK_DIVS   TOK_REM    TOK_REMS
%token  TOK_CMPLT TOK_CMPGT TOK_CMPLE  TOK_CMPGE TOK_CMPLTS TOK_CMPGTS TOK_CMPLES TOK_CMPGES
%token  TOK_CMPEQ TOK_CMPNE TOK_STRCMP TOK_LAND  TOK_LOR    TOK_LNOT              TOK_NEG
%token  TOK_DUP   TOK_DROP  TOK_DROPF  TOK_SWAP  TOK_U64    TOK_S64    TOK_SIGN   TOK_TRUNC
%token  TOK_BAND  TOK_BOR   TOK_BXOR   TOK_BINV  TOK_SHL    TOK_SHR    TOK_STRP   TOK_VARP
%token  TOK_RDP   TOK_RD    TOK_WRP    TOK_WR

%token  TOK_EQ TOK_NE TOK_LT TOK_GT TOK_LE TOK_GE TOK_LTS TOK_GTS TOK_LES TOK_GES

%%

test
	: fragments                                       { COMPILER.setUpLinks(); }
	;

fragments
	:
	| fragment fragments
	;

fragment
	: word_definition
	| var_definition
	| lb_fragment
	;

word_definition
	: word label_or_verdict ';'
	| word lb_fragment_as_reaction
	;

word
	: TOK_STRING                                      { COMPILER.declareWord($1); }
	;

label_or_verdict
	: label
	| verdict
	;

label
	: TOK_IDENT                                       { COMPILER.referLabel($1, 0); }
	| TOK_IDENT '(' TOK_NUM ')'                       { COMPILER.referLabel($1, $3); }
	;

verdict
	: TOK_STRING                                      { COMPILER.referVerdict($1); }
	;

lb_fragment_as_reaction
	: '{'                                             { COMPILER.referLb(); }
	  lb_element_seq '}'                              { COMPILER.checkSequenceFinalization(); }
	;

lb_fragment
	: '{' lb_element_seq '}'                          { COMPILER.checkSequenceFinalization(); }
	;

lb_element_seq
	: labelled_lb_element
	| labelled_lb_element lb_element_seq
	;

labelled_lb_element
	: label_def lb_element
	| lb_element
	;

label_def
	: ':' TOK_IDENT                                   { COMPILER.declareLabel($2); }
	;

cmp_mode
	: TOK_EQ                                          { $$ = LB_ELT_CMP_OP_EQ; }
	| TOK_NE                                          { $$ = LB_ELT_CMP_OP_NE; }
	| TOK_LT                                          { $$ = LB_ELT_CMP_OP_LT; }
	| TOK_GT                                          { $$ = LB_ELT_CMP_OP_GT; }
	| TOK_LE                                          { $$ = LB_ELT_CMP_OP_LE; }
	| TOK_GE                                          { $$ = LB_ELT_CMP_OP_GE; }
	| TOK_LTS                                         { $$ = ~LB_ELT_CMP_OP_LT; }
	| TOK_GTS                                         { $$ = ~LB_ELT_CMP_OP_GT; }
	| TOK_LES                                         { $$ = ~LB_ELT_CMP_OP_LE; }
	| TOK_GES                                         { $$ = ~LB_ELT_CMP_OP_GE; }
	| TOK_NUM                                         { $$ = $1; }
	| '~' TOK_NUM                                     { $$ = ~$2; }
	;

and_header
	: TOK_AND2                                        { $$ = -2; }
	| TOK_AND3                                        { $$ = -3; }
	| TOK_AND4                                        { $$ = -4; }
	;

seq_header
	: TOK_SEQ2                                        { $$ = -2; }
	| TOK_SEQ3                                        { $$ = -3; }
	| TOK_SEQ4                                        { $$ = -4; }
	;

cmp0_header
	: TOK_EQ0                                         { $$ = LB2_ELT_EQ0; }
	| TOK_NE0                                         { $$ = LB2_ELT_NE0; }
	;

cmp16_header
	: TOK_EQ16                                        { $$ = LB2_ELT_EQ16; }
	| TOK_NE16                                        { $$ = LB2_ELT_NE16; }
	| TOK_LT16                                        { $$ = LB2_ELT_LT16; }
	| TOK_GT16                                        { $$ = LB2_ELT_GT16; }
	| TOK_LE16                                        { $$ = LB2_ELT_LE16; }
	| TOK_GE16                                        { $$ = LB2_ELT_GE16; }
	| TOK_LTS16                                       { $$ = LB2_ELT_LTS16; }
	| TOK_GTS16                                       { $$ = LB2_ELT_GTS16; }
	| TOK_LES16                                       { $$ = LB2_ELT_LES16; }
	| TOK_GES16                                       { $$ = LB2_ELT_GES16; }
	;

rd_header
	: TOK_RD8                                         { $$ = LB2_ELT_RD8; }
	| TOK_RD16                                        { $$ = LB2_ELT_RD16; }
	| TOK_RD32                                        { $$ = LB2_ELT_RD32; }
	| TOK_RD64                                        { $$ = LB2_ELT_RD64; }
	| TOK_RDS8                                        { $$ = LB2_ELT_RDS8; }
	| TOK_RDS16                                       { $$ = LB2_ELT_RDS16; }
	| TOK_RDS32                                       { $$ = LB2_ELT_RDS32; }
	;

rdx_header
	: TOK_RDX8                                        { $$ = LB2_ELT_RDX8; }
	| TOK_RDX16                                       { $$ = LB2_ELT_RDX16; }
	| TOK_RDX32                                       { $$ = LB2_ELT_RDX32; }
	| TOK_RDX64                                       { $$ = LB2_ELT_RDX64; }
	| TOK_RDXS8                                       { $$ = LB2_ELT_RDXS8; }
	| TOK_RDXS16                                      { $$ = LB2_ELT_RDXS16; }
	| TOK_RDXS32                                      { $$ = LB2_ELT_RDXS32; }
	;

lb_element
	: TOK_TRAP final_opt                              { COMPILER.emitTrap(); }
	| TOK_HUB final_opt                               { COMPILER.emitHub(); }
	  hub_outputs
	| TOK_FRAME final_opt TOK_NUM                     { COMPILER.emitFrame($3); }
	| TOK_CALL final_opt TOK_IDENT                    { COMPILER.emitCall($3); }
	| TOK_CALL0 final_opt TOK_IDENT                   { COMPILER.emitCallN(0, $3); }
	| TOK_CALL1 final_opt TOK_IDENT                   { COMPILER.emitCallN(1, $3); }
	  val_or_obj
	| TOK_CALL2 final_opt TOK_IDENT                   { COMPILER.emitCallN(2, $3); }
	  val_or_obj
	  val_or_obj
	| TOK_CALL3 final_opt TOK_IDENT                   { COMPILER.emitCallN(3, $3); }
	  val_or_obj
	  val_or_obj
	  val_or_obj
	| and_header non_final                            { COMPILER.emitAnd($1); }
	| and_header final                                { COMPILER.emitAnd($1); }
	  hub_output
	| TOK_ANDN non_final '/' TOK_NUM                  { COMPILER.emitAnd($4); }
	| TOK_ANDN final '/' TOK_NUM                      { COMPILER.emitAnd($4); }
	  hub_output
	| seq_header non_final                            { COMPILER.emitSeq($1); }
	| seq_header final                                { COMPILER.emitSeq($1); }
	  hub_output
	| TOK_SEQN non_final '/' TOK_NUM                  { COMPILER.emitSeq($4); }
	| TOK_SEQN final '/' TOK_NUM                      { COMPILER.emitSeq($4); }
	  hub_output
	| TOK_PUSH1 final_opt TOK_NUM                     { COMPILER.emitPush1($3); }
	| TOK_PUSH2 final_opt TOK_NUM TOK_NUM             { COMPILER.emitPush2($3, $4); }
	| TOK_PUSH3 final_opt TOK_NUM TOK_NUM TOK_NUM     { COMPILER.emitPush3($3, $4, $5); }
	| TOK_PUSHN final_opt TOK_NUM                     { COMPILER.emitPush($3); }
	  pushed_seq
	| TOK_STKOP op                                    { COMPILER.emitStkop($2); }
	| TOK_STKOP op op                                 { COMPILER.emitStkop($2, $3); }
	| TOK_STKOP op op op                              { COMPILER.emitStkop($2, $3, $4); }
	| cmp0_header final_opt m64_opt                   { COMPILER.emitCmp0($1, $3); }
	  hub_outputs
	| cmp16_header final_opt m64_opt TOK_NUM          { COMPILER.emitCmp16($1, $3, $4); }
	  hub_outputs
	| TOK_CMP32 final_opt m64_opt cmp_mode TOK_NUM    { COMPILER.emitCmp32($4, $3, $5); }
	  hub_outputs
	| TOK_CMP64 final_opt cmp_mode TOK_NUM            { COMPILER.emitCmp64($3, $4); }
	  hub_outputs
	| TOK_CMP final_opt m64_opt cmp_mode              { COMPILER.emitCmp($4, $3); }
	  hub_outputs
	| rd_header final_opt TOK_IDENT                   { COMPILER.emitRd($1, $3, 0); }
	| rd_header final_opt TOK_IDENT '+' TOK_NUM       { COMPILER.emitRd($1, $3, $5); }
	| rdx_header final_opt TOK_NUM TOK_NUM            { COMPILER.emitRdx($1, $3, $4); }
	| TOK_RDSLOT final_opt TOK_NUM                    { COMPILER.emitRdslot($3); }
	| TOK_RDCX final_opt TOK_NUM TOK_NUM              { COMPILER.emitRdcx($3, $4); }
	;
	
final_opt
	: final
	| non_final
	;

final
	: '.'                                             { COMPILER.setFinal(1); }
	;                              
	                               
non_final                          
	:                                                 { COMPILER.setFinal(0); }
	;                              
	                               
m64_opt                         
	: mode64                       
	| mode32                       
	;                              

mode64
	:  '^'                                            { $$ = 1; }
	;                              
	                               
mode32                             
	:                                                 { $$ = 0; }
	;                              
	                               
hub_outputs                        
	:                              
	| hub_output hub_outputs       
	;                              

hub_output
	: label_or_verdict                                { COMPILER.addOutput(); }
	;                              
	                               
val_or_obj
	: TOK_NUM                                         { COMPILER.addValue($1); }
	| TOK_IDENT                                       { COMPILER.addVarRef($1, 0); }
	| TOK_IDENT '+' TOK_NUM                           { COMPILER.addVarRef($1, $3); }
	| TOK_STRING                                      { COMPILER.addStrRef($1); }
	;                              
	                               
pushed_seq                         
	:                              
	| pushed pushed_seq
	;

pushed
	: val_or_obj                                      { COMPILER.addPushed(); }
	;
	
op
	: opcode m64_opt arg_opt                          { $$ = $1 | ($2 ? LB_ELT_STKOP_64BIT : 0) | ($3 ? LB_ELT_STKOP_HAS_ARG : 0); }
	| TOK_NUM
	;
	
arg_opt
	: has_arg
	| has_no_arg
	;
	
has_arg
	: '/'                                             { $$ = 1; }
	;
	
has_no_arg
	:                                                 { $$ = 0; }
	;

opcode
    : TOK_NOP                                         { $$ = LB2_STKOP_NOP; }
    | TOK_ADD                                         { $$ = LB2_STKOP_ADD; }
    | TOK_ADDN                                        { $$ = LB2_STKOP_ADDN; }
    | TOK_ADDS                                        { $$ = LB2_STKOP_ADDS; }
    | TOK_ADDS2                                       { $$ = LB2_STKOP_ADDS2; }
    | TOK_ADDS4                                       { $$ = LB2_STKOP_ADDS4; }
    | TOK_ADDS8                                       { $$ = LB2_STKOP_ADDS8; }
    | TOK_SUB                                         { $$ = LB2_STKOP_SUB; }
    | TOK_MUL                                         { $$ = LB2_STKOP_MUL; }
    | TOK_MULN                                        { $$ = LB2_STKOP_MULN; }
    | TOK_DIV                                         { $$ = LB2_STKOP_DIV; }
    | TOK_DIVS                                        { $$ = LB2_STKOP_DIVS; }
    | TOK_REM                                         { $$ = LB2_STKOP_REM; }
    | TOK_REMS                                        { $$ = LB2_STKOP_REMS; }
    | TOK_CMPLT                                       { $$ = LB2_STKOP_CMPLT; }
    | TOK_CMPGT                                       { $$ = LB2_STKOP_CMPGT; }
    | TOK_CMPLE                                       { $$ = LB2_STKOP_CMPLE; }
    | TOK_CMPGE                                       { $$ = LB2_STKOP_CMPGE; }
    | TOK_CMPLTS                                      { $$ = LB2_STKOP_CMPLTS; }
    | TOK_CMPGTS                                      { $$ = LB2_STKOP_CMPGTS; }
    | TOK_CMPLES                                      { $$ = LB2_STKOP_CMPLES; }
    | TOK_CMPGES                                      { $$ = LB2_STKOP_CMPGES; }
    | TOK_CMPEQ                                       { $$ = LB2_STKOP_CMPEQ; }
    | TOK_CMPNE                                       { $$ = LB2_STKOP_CMPNE; }
    | TOK_STRCMP                                      { $$ = LB2_STKOP_STRCMP; }
    | TOK_LAND                                        { $$ = LB2_STKOP_LAND; }
    | TOK_LOR                                         { $$ = LB2_STKOP_LOR; }
    | TOK_LNOT                                        { $$ = LB2_STKOP_LNOT; }
    | TOK_NEG                                         { $$ = LB2_STKOP_NEG; }
    | TOK_DUP                                         { $$ = LB2_STKOP_DUP; }
    | TOK_DROP                                        { $$ = LB2_STKOP_DROP; }
    | TOK_DROPF                                       { $$ = LB2_STKOP_DROPF; }
    | TOK_SWAP                                        { $$ = LB2_STKOP_SWAP; }
    | TOK_U64                                         { $$ = LB2_STKOP_64; }
    | TOK_S64                                         { $$ = LB2_STKOP_64S; }
    | TOK_SIGN                                        { $$ = LB2_STKOP_SIGN; }
    | TOK_TRUNC                                       { $$ = LB2_STKOP_TRUNC; }
    | TOK_BAND                                        { $$ = LB2_STKOP_BAND; }
    | TOK_BOR                                         { $$ = LB2_STKOP_BOR; }
    | TOK_BXOR                                        { $$ = LB2_STKOP_BXOR; }
    | TOK_BINV                                        { $$ = LB2_STKOP_BINV; }
    | TOK_SHL                                         { $$ = LB2_STKOP_SHL; }
    | TOK_SHR                                         { $$ = LB2_STKOP_SHR; }
    | TOK_STRP                                        { $$ = LB2_STKOP_STRP; }
    | TOK_VARP                                        { $$ = LB2_STKOP_VARP; }
    | TOK_RDP                                         { $$ = LB2_STKOP_RDP; }
    | TOK_RD                                          { $$ = LB2_STKOP_RD; }
    | TOK_WRP                                         { $$ = LB2_STKOP_WRP; }
    | TOK_WR                                          { $$ = LB2_STKOP_WR; }
    ;
                                       
var_definition                         
    : TOK_IDENT TOK_NUM ';'                           { COMPILER.declareVar($1, $2, 0); }
    | TOK_IDENT TOK_NUM '@' TOK_NUM ';'               { COMPILER.declareVar($1, $2, $4); }
    ;                                  
