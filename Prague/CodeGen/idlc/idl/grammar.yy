%{

#ifndef IDL_HIDE_INTERNALS

#include "ValueFactory.hpp"

#include "Builder.hpp"
#include "NodeInterface.hpp"
#include "Operations.hpp"
#include "BasicTypeNode.hpp"
#include "namecontext/NameContext.hpp"

//#ifndef YYSTYPE
struct SType {
	idl::parser::Value*	_value;
};

template <class T, class U>
boost::intrusive_ptr<T> checked_cast(boost::intrusive_ptr<U> u) {
	return boost::dynamic_pointer_cast<T>(u);
}

#define ID_VALUE(v)      (v)._value->_lexeme->getId()
#define NAME_VALUE(v)    (v)._value->_lexeme->getString()
#define INT_VALUE(v)     (v)._value->_lexeme->getInt()
#define FLOAT_VALUE(v)   (v)._value->_lexeme->getFloat()
#define NODE(v)          (v)._value->_node
#define TUPLE(v)         checked_cast<idl::parser::Tuple>(NODE(v))
//#define TUPLE(v)       boost::shared_polymorphic_downcast<idl::parser::Tuple>(NODE(v))

#define CREATE_OPERATION(c, op, v1)               builder.create<idl::parser::Operations::op>(*c._context, TUPLE(v1))
#define CREATE_OPERATION_0(c, op)                 builder.create<idl::parser::Operations::op>(*c._context, make_tuple(builder))
#define CREATE_OPERATION_1(c, op, v1)             builder.create<idl::parser::Operations::op>(*c._context, make_tuple(builder,NODE(v1)))
#define CREATE_OPERATION_2(c, op, v1, v2)         builder.create<idl::parser::Operations::op>(*c._context, make_tuple(builder,NODE(v1), NODE(v2)))
#define CREATE_OPERATION_3(c, op, v1, v2, v3)     builder.create<idl::parser::Operations::op>(*c._context, make_tuple(builder,NODE(v1), NODE(v2), NODE(v3)))
#define CREATE_OPERATION_4(c, op, v1, v2, v3, v4) builder.create<idl::parser::Operations::op>(*c._context, make_tuple(builder,NODE(v1), NODE(v2), NODE(v3), NODE(v4)))

#define CREATE_ATTRIBUTE(c, id, v)                builder.createAttribute(*c._context, id, v)
#define CREATE_OPTIONAL_VALUE_EMPTY(c)            builder.createOptionalValue(*(c)._context)
#define CREATE_OPTIONAL_VALUE(c, v1)              builder.createOptionalValue(*(c)._context, NODE(v1))


#define CREATE_VALUE(v) do (v)._value = idl::parser::ValueFactory::instance().createValue(); while(0)


#define CREATE_LEXEME_INT(v, c, num)																									\
	(v)._value = idl::parser::ValueFactory::instance().createValue();                                                                   \
	(v)._value->_context = *c._context;                                                                                                 \
	(v)._value->_lexeme = boost::shared_ptr<idl::scaner::LexemeValue>(new idl::scaner::LexemeValue(*(c)._context, idl::scaner::Lexeme::Ids::NumberInt, (idl::scaner::Int)(num)));


#define CREATE_TUPLE(v)						CREATE_VALUE(v); NODE(v) = builder.createTuple();
#define CREATE_NAME(c, v)					builder.createName(*((c)._context), NAME_VALUE(v))
//#define ADD_NAME( c, v )          builder.addName( *((c)._context), NAME_VALUE(v) )
#define CREATE_INT(c, v)					builder.createInt(*((c)._context), v)

#define AUTO_VALUE(node)					auto_value(node)

using namespace idl::parser;

static SType auto_value(NodeInterface::Ptr node) {
	SType result; CREATE_VALUE(result);
	NODE(result) = node;
	return result;
}

static Tuple::Ptr make_tuple(Builder& builder) {
	Tuple::Ptr tuple = builder.createTuple();
	return tuple;
}

static Tuple::Ptr make_tuple(Builder& builder, NodeInterface::Ptr v1) {
	Tuple::Ptr tuple = builder.createTuple();
	tuple->prepend(v1);
	return tuple;
}

static Tuple::Ptr make_tuple(Builder& builder, NodeInterface::Ptr v1, NodeInterface::Ptr v2) {
	Tuple::Ptr tuple = builder.createTuple();
	tuple->prepend(v1);
	tuple->prepend(v2);
	return tuple;
}

static Tuple::Ptr make_tuple(Builder& builder, NodeInterface::Ptr v1, NodeInterface::Ptr v2, NodeInterface::Ptr v3) {
	Tuple::Ptr tuple = builder.createTuple();
	tuple->prepend(v1);
	tuple->prepend(v2);
	tuple->prepend(v3);
	return tuple;
}

Tuple::Ptr make_tuple(Builder& builder, NodeInterface::Ptr v1, NodeInterface::Ptr v2, NodeInterface::Ptr v3, NodeInterface::Ptr v4);
extern "C" SType checkIfaceName( SType nameNode );

#define YYSTYPE	SType
//#endif

struct LType {
	idl::parser::Context* _context;
};

#define YYLTYPE	LType

#include "Scaner.hpp"
#include "Parser.hpp"

#include <iostream>

#include "LexemeValue.hpp"

static int yylex(YYSTYPE* value, YYLTYPE* llocp, idl::scaner::Scaner& scaner);
static void yyerror(YYLTYPE* llocp, idl::parser::Parser& parser, idl::parser::Builder& builder, idl::scaner::Scaner& , char const *msg);

template <class T>
void join(YYLTYPE& result, T Rhs, int N);

static YYLTYPE createInitialContext();

#define YYLLOC_DEFAULT(Current, Rhs, N)  \
	do                                     \
		join((Current), Rhs, N);             \
	while (0)

#endif

%}

%initial-action
{
	@$ = createInitialContext();
	$$._value = idl::parser::ValueFactory::instance().createValue();
};

//%expect 11
	// (not any more) if vs if+else, 
	// (not any more) type<const> vs expr < const 
	// name vs name . name
	// triple occurence: forward decl v.s. interface definition

//%expect-rr 2

%glr-parser

%locations
%pure-parser

%lex-param   {idl::scaner::Scaner& scaner}
%parse-param {idl::parser::Parser& parser}
%parse-param {idl::parser::Builder& builder}
%parse-param {idl::scaner::Scaner& scaner}

%token lx_Eof 0
%token lx_ScanerError
%token lx_And
%token lx_As
%token lx_BitAnd
%token lx_BitInv
%token lx_BitOr
%token lx_BitXor
%token lx_Character
%token lx_Comma
%token lx_Component
%token lx_Const
%token lx_Div
%token lx_Dot
%token lx_Enum
%token lx_Eq
%token lx_Float
//%token lx_For
%token lx_Ge
%token lx_Gt
//%token lx_If
%token lx_In
%token lx_Interface
%token lx_InterfaceId
%token lx_ImportEnd
%token lx_ImportStart
%token lx_Le
%token lx_lBr
%token lx_lPt
%token lx_Let
%token lx_Lt
%token lx_Method
%token lx_Minus
%token lx_Mul
%token lx_Name
%token lx_Namespace
%token lx_Ne
%token lx_Neg
%token lx_Not
%token lx_NumberFloat
%token lx_NumberInt
%token lx_Or
%token lx_Out
%token lx_Plus
%token lx_Requires
%token lx_rBr
%token lx_rPt
%token lx_S16
%token lx_S32
%token lx_S64
%token lx_S8
%token lx_Semicolon
//%token lx_Sequence
%token lx_ShiftLeft
%token lx_ShiftRight
%token lx_String
%token lx_Struct
%token lx_TripleDot
%token lx_Typedef
%token lx_Forward
%token lx_U16
%token lx_U32
%token lx_U64
%token lx_U8
%token lx_Vendor
%token lx_Version
//%token lx_While
//%token lx_Else

%token lx_C_Type_Name
%token lx_Cpp_Type_Name
%token lx_Access
%token lx_Getter
%token lx_Setter
%token lx_Member_Ref
%token lx_lsBr
%token lx_rsBr
%token lx_Void

%token lx_Read
%token lx_Write
%token lx_Write_On_Init

%token lx_Add
%token lx_Prop
%token lx_Message
%token lx_Message_Cls

%token lx_Error
%token lx_Implementation
%token lx_SysMethod
%token lx_Colon
%token lx_Shared

%token lx_Data

%token lx_Extend

%token lx_Comment
%token lx_Comment_Ex
%token lx_Warning
%token lx_Mnemonic
%token lx_Autostart
%token lx_Output_Header_File
%token lx_Output_Source_File
%token lx_Include_Files
%token lx_Codepage

%token lx_QuotedString
%token lx_True
%token lx_False

%token lx_Import
%token lx_From

%token lx_UnknownPropGetter
%token lx_UnknownPropSetter
%token lx_Optional
%token lx_BackSlash

%right    lx_Let
%nonassoc lx_lPt
%left     lx_ShiftLeft lx_ShiftRight
%left     lx_Or
%left     lx_And
%left     lx_Eq lx_Ne lx_Gt lx_Lt lx_Ge lx_Le
%left     lx_BitOr
%left     lx_BitAnd
%right    lx_Not
%right    lx_BitInv
%left     lx_BitXor
%left     lx_Minus lx_Plus
%left     lx_Mul lx_Div
%right    lx_Neg
%nonassoc lx_Star
%left     lx_As
%left     lx_Dot
//%left     lx_Dot_Compound_Single
//%left     lx_Dot_Compound


%start start

%%
start
		: idl                                { builder.setRootNode(*(@$._context), NODE($1)); }
		;
		
idl
		:                                    { CREATE_TUPLE($$); }
		| idl_toplevel_entry idl             { $$ = $2; if (NODE($1)) TUPLE($$)->prepend(NODE($1)); }
		;

idl_toplevel_entry
		: idl_toplevel_entry_without_error   { $$ = $1; }
		| error	error_border                 { CREATE_VALUE($$); NODE($$) = builder.createError(*(@1._context), "syntax error"); }
		;

idl_toplevel_entry_without_error
		: interface_definition               { $$ = $1; }
		| namespace_definition               { $$ = $1; }
		| type_definition                    { $$ = $1; }
		| component_definition               { $$ = $1; }
		| import_start_definition            { $$ = $1; }
		| import_end_definition              { $$ = $1; }
		| name_import_definition             { $$ = $1; }
		| lx_Semicolon                       { CREATE_VALUE($$); NODE($$) = idl::parser::NodeInterface::Ptr(); }
		;

error_border
		: lx_Semicolon
		| lx_Vendor
		| lx_Component
		| lx_Interface
		| lx_Typedef
		| lx_Enum
		| lx_Struct
		| lx_Const
		| lx_Namespace
		| lx_rBr
		| lx_Eof
		;


name_import_definition
		: lx_Import name lx_From quoted_string lx_Semicolon             { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_2(@$, ImportName, $2, $4); }
		| lx_Import name lx_As type lx_From quoted_string lx_Semicolon  { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_3(@$, ImportNameAsType, $2, $4, $6); }
		;
		
attributes
		:                                                               { CREATE_TUPLE($$); }
		| lx_lsBr attributes_body lx_rsBr                               { $$ = $2; }
		;

attributes_body
		:                                                               { CREATE_TUPLE($$); }
		| attribute attributes_body                                     { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		;

forward_decl
		: lx_Forward lx_Interface name lx_Semicolon                     { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_1(@$, ForwardIFace, checkIfaceName($3) ); }
		| lx_Forward lx_Struct name lx_Semicolon                        { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_1(@$, ForwardStruct, $3); }
		;

error_definition
		: attributes lx_Error name optional_value lx_Semicolon          { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_3(@$, ErrorDef, $1, $3, $4); }
		;

property_interface
		: attributes lx_Prop type name lx_Semicolon                     { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_4(@$, Property, $1, $3, $4, AUTO_VALUE(builder.createInt(*((@2)._context), 0))); }
		| attributes lx_Optional lx_Prop type name lx_Semicolon         { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_4(@$, Property, $1, $4, $5, AUTO_VALUE(builder.createInt(*((@2)._context), 1))); }
		;

property_declaration
		: attributes lx_Prop name lx_Semicolon                          { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_2(@$, PropertyDecl, $1, $3); }
		;

property_plugin
		: attributes lx_Prop type name optional_value lx_Semicolon      { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_4(@$, PropertyPlugin, $1, $3, $4, $5); }
		;
		
add_property_plugin
		: attributes lx_Add lx_Prop type name optional_value lx_Semicolon  { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_4(@$, PropertyPluginAdd, $1, $4, $5, $6); }
		;

message_class
		: attributes lx_Message_Cls name lx_lBr messages lx_rBr            { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_3(@$, MessageClass, $1, $3, $5); }
		;

message_class_extend
		: attributes lx_Extend lx_Message_Cls name lx_lBr messages lx_rBr  { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_3(@$, MessageClassExtend, $1, $4, $6); }
		;

add_message_class
		: attributes lx_Add lx_Message_Cls name lx_lBr messages lx_rBr	   { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_3(@$, MessageClass, $1, $4, $6); }
		;

sys_method
		: attributes lx_SysMethod name lx_Semicolon   { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_2(@$, SysMethod, $1, $3); }
		;

messages
		:                                             { CREATE_TUPLE($$); }
		| message messages                            { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		;

message
		: attributes lx_Message name lx_Semicolon     { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_2(@$, Message, $1, $3); }
		;

attribute
		: lx_InterfaceId        lx_lPt  lx_NumberInt     lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), INT_VALUE($3)); }
		| lx_Version            lx_lPt  lx_NumberInt     lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), INT_VALUE($3)); }
		| lx_C_Type_Name        lx_lPt  lx_Name          lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), NAME_VALUE($3)); }
		| lx_Cpp_Type_Name      lx_lPt  lx_Name          lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), NAME_VALUE($3)); }
		| lx_Access             lx_lPt  access_rights    lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), TUPLE($3)); }
		| lx_Getter             lx_lPt  lx_Name          lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), NAME_VALUE($3)); }
		| lx_Setter             lx_lPt  lx_Name          lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), NAME_VALUE($3)); }
		| lx_UnknownPropGetter  lx_lPt  lx_Name          lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), NAME_VALUE($3)); }
		| lx_UnknownPropSetter  lx_lPt  lx_Name          lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), NAME_VALUE($3)); }
		| lx_Member_Ref         lx_lPt  lx_Name          lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), NAME_VALUE($3)); }
		| lx_Autostart          lx_lPt  bool_value       lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), INT_VALUE($3)); }
		| lx_Codepage           lx_lPt  lx_Name          lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), NAME_VALUE($3)); }
		| lx_Output_Header_File lx_lPt  lx_Name          lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), NAME_VALUE($3)); }
		| lx_Output_Source_File lx_lPt  lx_Name          lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), NAME_VALUE($3)); }
		| lx_Include_Files      lx_lPt  inc_name_list    lx_rPt	 { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), TUPLE($3)); }
		| lx_Shared             lx_lPt  bool_value       lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), INT_VALUE($3)); }
		| lx_Warning            lx_lPt  bool_value       lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), INT_VALUE($3)); }
		| lx_Comment            lx_lPt  lx_QuotedString  lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), NAME_VALUE($3)); }
		| lx_Comment_Ex         lx_lPt  lx_QuotedString  lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), NAME_VALUE($3)); }
		| lx_Mnemonic           lx_lPt  lx_Name          lx_rPt  { NODE($$) = CREATE_ATTRIBUTE(@$, ID_VALUE($1), NAME_VALUE($3)); }
		;

bool_value
		: lx_False            { CREATE_LEXEME_INT($$, @1, 0); }
		| lx_True             { CREATE_LEXEME_INT($$, @1, 1); }
		;

name 
		: lx_Name             { CREATE_VALUE($$); NODE($$) = CREATE_NAME(@1, $1); }
		;

access_right
		: lx_Read             { CREATE_VALUE($$); NODE($$) = builder.createInt(*((@1)._context), ID_VALUE($1)); }
		| lx_Write            { CREATE_VALUE($$); NODE($$) = builder.createInt(*((@1)._context), ID_VALUE($1)); }
		| lx_Write_On_Init    { CREATE_VALUE($$); NODE($$) = builder.createInt(*((@1)._context), ID_VALUE($1)); }
		;

access_rights
		: access_right                          { CREATE_TUPLE($$); TUPLE($$)->prepend(NODE($1)); }
		| access_right lx_Comma access_rights   { $$ = $3; TUPLE($$)->prepend(NODE($1)); }
		;

compound_name //_list
		: name                      /*%prec lx_Dot_Compound_Single %dprec 1*/    { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_1(@$, Dot, $1); }
		| compound_name lx_Dot name /*_list*/ /*%prec lx_Dot_Compound %dprec 2*/ { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_2(@$, Dot, $1, $3); }
		;

file_name
		: lx_Name                           { $$ = $1; }
		| file_name lx_Dot lx_Name          { $$ = $1; }
		;
		
file_divider
		: lx_Div                            { $$ = $1; }
		| lx_BackSlash                      { $$ = $1; }
		;
		
file_path
		: file_name                         { $$ = $1; }
		| file_divider file_path            { $$ = $1; }
		| file_name file_divider file_path  { $$ = $1; }
		| lx_Name lx_Colon file_path        { $$ = $1; }
		;

inc_name 	
		: lx_Name                           { CREATE_VALUE($$); NODE($$) = CREATE_NAME(@1, $1); }
		| lx_QuotedString                   { CREATE_VALUE($$); NODE($$) = CREATE_NAME(@1, $1); }
		| lx_Lt file_path lx_Gt             { CREATE_VALUE($$); NODE($$) = CREATE_NAME(@1, $2); }
		;

inc_name_list
		: inc_name                          { CREATE_TUPLE($$); TUPLE($$)->prepend(NODE($1)); }
		| inc_name_list lx_Comma inc_name   { $$ = $1; TUPLE($$)->prepend(NODE($3)); }
		;

import_start_definition
		: lx_ImportStart                    { CREATE_VALUE($$); NODE($$) = builder.createImportStart(*(@1._context), NAME_VALUE($1)); }
		;

import_end_definition
		: lx_ImportEnd                      { CREATE_VALUE($$); NODE($$) = builder.createImportEnd(*(@1._context), NAME_VALUE($1)); }
		;

namespace_definition
		: lx_Namespace name lx_lBr idl lx_rBr { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_2(@$, Namespace, $2, $4); }
		;

basic_type 
		: lx_S8           { CREATE_VALUE($$); NODE($$) = builder.createBasicType(*(@1._context), BasicTypeNode::SInt8); }
		| lx_S16          { CREATE_VALUE($$); NODE($$) = builder.createBasicType(*(@1._context), BasicTypeNode::SInt16); }
		| lx_S32          { CREATE_VALUE($$); NODE($$) = builder.createBasicType(*(@1._context), BasicTypeNode::SInt32); }
		| lx_S64          { CREATE_VALUE($$); NODE($$) = builder.createBasicType(*(@1._context), BasicTypeNode::SInt64); }
		| lx_U8           { CREATE_VALUE($$); NODE($$) = builder.createBasicType(*(@1._context), BasicTypeNode::UInt8); }
		| lx_U16          { CREATE_VALUE($$); NODE($$) = builder.createBasicType(*(@1._context), BasicTypeNode::UInt16); }
		| lx_U32          { CREATE_VALUE($$); NODE($$) = builder.createBasicType(*(@1._context), BasicTypeNode::UInt32); }
		| lx_U64          { CREATE_VALUE($$); NODE($$) = builder.createBasicType(*(@1._context), BasicTypeNode::UInt64); }
		| lx_Character    { CREATE_VALUE($$); NODE($$) = builder.createBasicType(*(@1._context), BasicTypeNode::Character); }
		| lx_Float        { CREATE_VALUE($$); NODE($$) = builder.createBasicType(*(@1._context), BasicTypeNode::Float); }
		| lx_Void         { CREATE_VALUE($$); NODE($$) = builder.createBasicType(*(@1._context), BasicTypeNode::Void); }
		;

//basic_container 
//		: lx_String lx_Lt expr lx_Gt                  /*%prec lx_TypeParam*/  { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_1(@$, String, $3); }
//    | lx_Sequence lx_Lt type lx_Comma expr lx_Gt  /*%prec lx_TypeParam*/  { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_2(@$, Sequence, $3, $5); }
//		;

enum_entries
		: enum_entry optional_comma         { CREATE_TUPLE($$); TUPLE($$)->prepend(NODE($1)); }
		| enum_entry lx_Comma enum_entries  { $$ = $3; TUPLE($$)->prepend(NODE($1)); }
		;

enum_entry
		: name                              { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_1(@$, EnumEntry, $1); }
		| name lx_Let expr                  { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_2(@$, Let, $1, $3); }
		;

optional_comma
		:
		| lx_Comma
		;

struct_definition
		: attributes lx_Struct name lx_lBr struct_entries lx_rBr { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_3(@$, Struct, $1, $3, $5); }
		;

struct_entries
		: struct_entry                      { CREATE_TUPLE($$); TUPLE($$)->prepend(NODE($1)); }
		| struct_entry struct_entries       { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		;

struct_entry
		: type name lx_Semicolon            { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_2(@$, StructEntry, $1, $2); }
		;
		
data_struct_definition
		: lx_Data lx_lBr struct_entries lx_rBr { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_1(@$, DataStruct, $3); }
		;

typedef_definition
		: attributes lx_Typedef type name lx_Semicolon		{ CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_3(@$, Typedef, $1, $3, $4); }
		;

non_const_type
		: basic_type                          { $$ = $1; }
//		| basic_container                     { $$ = $1; }
		| compound_name                       { $$ = $1; }
		| non_const_type lx_Mul %prec lx_Star { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_1(@$, RefType, $1); }
		;

type
		: non_const_type                      { $$ = $1; }
		| lx_Const non_const_type             { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_1(@$, ConstType, $2); }
		;

type_definition
		: enum_definition                     { $$ = $1; }
		| struct_definition                   { $$ = $1; }
		| typedef_definition                  { $$ = $1; }
		| const_definition                    { $$ = $1; }
		| forward_decl                        { $$ = $1; }
		;

enum_definition
		: attributes lx_Enum name lx_lBr enum_entries lx_rBr lx_Semicolon  { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_3(@$, Enum, $1, $3, $5); }
		;

const_definition
		: attributes lx_Const non_const_type name lx_Let expr lx_Semicolon { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_4(@$, Const, $1, $3, $4, $6); }
		;

shared_definition
		: lx_Shared type name optional_value lx_Semicolon                  { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_3(@$, Shared, $2, $3, $5); }
		;

optional_value
		:               { CREATE_VALUE($$); NODE($$) = CREATE_OPTIONAL_VALUE_EMPTY(@$); }
		| lx_Let expr   { CREATE_VALUE($$); NODE($$) = CREATE_OPTIONAL_VALUE(@$, $2); }
		;
		
interface_definition
		: attributes lx_Interface name optional_bases lx_lBr interface_body lx_rBr			{ CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_4(@$, Interface, $1, checkIfaceName($3), $4, $6); }
		;

optional_bases
		:                                         { CREATE_TUPLE($$); }
		| lx_Colon name name_list_tail            { $$ = $3; TUPLE($$)->prepend(NODE(checkIfaceName($2))); }
		;
		
name_list_tail
		:                                         { CREATE_TUPLE($$); }
		| lx_Comma name name_list_tail            { $$ = $3; TUPLE($$)->prepend(NODE(checkIfaceName($2))); }
		;

interface_body
		:                                         { CREATE_TUPLE($$); }
		| type_definition        interface_body   { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| method_definition      interface_body   { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| property_interface     interface_body   { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| property_declaration   interface_body   { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| message_class          interface_body   { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| message_class_extend   interface_body   { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| error_definition       interface_body   { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| lx_Semicolon           interface_body   { $$ = $2; }
		;

method_definition
		: attributes lx_Method type name lx_lPt method_arguments_definition lx_rPt lx_Semicolon { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_4(@$, Method, $1, $3, $4, $6); }
		;

extern_method_definition
		: attributes lx_Method type name lx_lPt method_arguments_definition lx_rPt lx_Semicolon { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_4(@$, ExternMethod, $1, $3, $4, $6); }
		;

add_method_definition
		: attributes lx_Add lx_Method type name lx_lPt method_arguments_definition lx_rPt lx_Semicolon { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_4(@$, Method, $1, $4, $5, $7); }
		;

method_argument_definition
		: type name                    { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_2(@$, MethodArg, $1, $2); }
		;

last_method_argument_definition
		: method_argument_definition   { $$ = $1; }
		| lx_TripleDot                 { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_0(@$, VarArg); }
		;

method_arguments_definition
		:                                                                 { CREATE_TUPLE($$); }
		| last_method_argument_definition                                 { CREATE_TUPLE($$); TUPLE($$)->prepend(NODE($1)); }
		| method_argument_definition lx_Comma method_arguments_definition { $$ = $3; TUPLE($$)->prepend(NODE($1)); }
		;

component_definition
		: attributes lx_Component name lx_lBr component_body lx_rBr { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_3(@$, Component, $1, $3, $5); }
		;

component_body
		:                                          { CREATE_TUPLE($$); }
		| extern_method_definition component_body  { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| plugin_implementation    component_body  { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| type_definition          component_body  { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| lx_Semicolon             component_body  { $$ = $2; }
		;

plugin_implementation
		: attributes lx_Implementation name optional_bases lx_lBr plugin_implementation_body lx_rBr	{ CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_4(@$, Implementation, $1, $3, $4, $6); }
		;

plugin_implementation_body		
		:                                                      { CREATE_TUPLE($$); }
		| data_struct_definition plugin_implementation_body    { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| type_definition        plugin_implementation_body    { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| shared_definition      plugin_implementation_body    { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| sys_method             plugin_implementation_body    { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| add_method_definition  plugin_implementation_body    { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| property_plugin        plugin_implementation_body    { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| message_class_extend   plugin_implementation_body    { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| add_property_plugin    plugin_implementation_body    { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| add_message_class      plugin_implementation_body    { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| error_definition       plugin_implementation_body    { $$ = $2; TUPLE($$)->prepend(NODE($1)); }
		| lx_Semicolon           plugin_implementation_body    { $$ = $2; }
		;

number_int
		: lx_NumberInt                 { CREATE_VALUE($$); NODE($$) = builder.createInt(*((@1)._context), INT_VALUE($1)); }
		;

quoted_string
		: lx_QuotedString              { CREATE_VALUE($$); NODE($$) = builder.createString(*((@1)._context), NAME_VALUE($1)); }
		;

un_expr
		: lx_BitInv  expr              { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_1(@$, BitInv, $2); }
		| lx_Not     expr              { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_1(@$, Not, $2); }
		| lx_Minus   expr %prec lx_Neg { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_1(@$, Neg, $2); }
		;

bin_expr
		: expr lx_Mul        expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, Mul, $1, $3); }
		| expr lx_Div        expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, Div, $1, $3); }
		| expr lx_Plus       expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, Add, $1, $3); }
		| expr lx_Minus      expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, Sub, $1, $3); }
		| expr lx_Eq         expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, Eq , $1, $3); }
		| expr lx_Ne         expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, Ne , $1, $3); }
		| expr lx_Gt         expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, Gt , $1, $3); }
		| expr lx_Ge         expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, Ge , $1, $3); }
		| expr lx_Lt         expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, Lt , $1, $3); }
		| expr lx_Le         expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, Le , $1, $3); }
		| expr lx_And        expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, And, $1, $3); }
		| expr lx_Or         expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, Or , $1, $3); }
		| expr lx_ShiftRight expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, ShiftRight, $1, $3); }
		| expr lx_ShiftLeft  expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, ShiftLeft, $1, $3); }
		| expr lx_BitAnd     expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, BitAnd, $1, $3);}
		| expr lx_BitOr      expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, BitOr, $1, $3); }
		| expr lx_BitXor     expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, BitXor, $1, $3); }
		| expr lx_Let        expr      { CREATE_VALUE($$);  NODE($$) = CREATE_OPERATION_2(@$, Let, $1, $3); }
		;

expr
		: number_int                   { $$ = $1; }
		| name                         { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_1(@$, Dot, $1); }
		| expr lx_Dot name             { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_2(@$, Dot, $1, $3); }
		//| compound_name                { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_1(@$, Dot, $1); }
		| un_expr                      { $$ = $1; }
		| bin_expr                     { $$ = $1; }
		| lx_QuotedString              { CREATE_VALUE($$); NODE($$) = builder.createString(*((@1)._context), NAME_VALUE($1)); }
		| lx_lPt expr lx_rPt           { $$ = $2; }
		//| lx_lPt type lx_rPt expr      { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_3(@$, Typecast, $2, $2, $4); }
		| func_call                    { $$ = $1; }
		;

func_call
		: expr lx_lPt func_parameters lx_rPt { CREATE_VALUE($$); NODE($$) = CREATE_OPERATION_2(@$, FunCall, $1, $3); }
		;

func_parameters
		:                              { CREATE_TUPLE($$); }
		| func_par_list                { $$ = $1}
		;
				
func_par_list
		: expr                         { CREATE_TUPLE($$); TUPLE($$)->prepend(NODE($1)); }
		| expr lx_Comma func_par_list  { $$ = $3; TUPLE($$)->prepend(NODE($1)); }
		;

%%

#include "ValueFactory.hpp"
#include "LexemeValue.hpp"


static int yylex( YYSTYPE* value, YYLTYPE* llocp, idl::scaner::Scaner& scaner ) {
	int id = scaner.yylex();

	llocp->_context = idl::parser::ValueFactory::instance().createContext();

	const idl::scaner::Lexeme& lex = scaner.getLexeme();
	const idl::scaner::Context& ctx = lex.getContext();
	
	*(llocp->_context) = ctx;
	
	/*
	llocp->first_line 	= ctx.getFirstLine();
	llocp->first_column	= ctx.getFirstColon();
	llocp->last_line 	= ctx.getLastLine();
	llocp->last_column	= ctx.getLastColon();*/
	
	value->_value = idl::parser::ValueFactory::instance().createValue();
	value->_value->_context = ctx;
	value->_value->_lexeme = boost::shared_ptr<idl::scaner::LexemeValue>(new idl::scaner::LexemeValue(lex.getValue()));
	assert(value->_value->_lexeme->getId() == id);
	return id;
}

static void yyerror(YYLTYPE* llocp, idl::parser::Parser& parser, idl::parser::Builder& builder, idl::scaner::Scaner& , char const *msg) {
	builder.registerSyntaxError(*(llocp->_context), msg);
	parser.error(*(llocp->_context), msg);
}

#define EMPTY_CONTEXT_SOURCE	"<initial>"

static YYLTYPE createInitialContext() {
	using idl::parser::Context;

	YYLTYPE result = { idl::parser::ValueFactory::instance().createContext() };
	*(result._context) = Context(EMPTY_CONTEXT_SOURCE, 1, 1, 1, 1, "");
	return result;
}


template <class T>
void join(YYLTYPE& result, T Rhs, int N) {
	using idl::parser::Context;

	result._context = idl::parser::ValueFactory::instance().createContext();
	
	if (N == 0) {
		*(result._context) = Context(EMPTY_CONTEXT_SOURCE, 0, 0, 0, 0, "");
		return;
	}

	//ASSERT(N > 0);
	
	// scan for non-empty context
	int firstContext = 0;
	for (int cxt = 1; cxt <= N; ++cxt) {
		if (YYRHSLOC(Rhs, cxt)._context->getSource() != EMPTY_CONTEXT_SOURCE) {
			firstContext = cxt;
			break;
		}
	}

	const Context*	first 	= YYRHSLOC(Rhs, firstContext)._context;
	//const Context*	last	= YYRHSLOC(Rhs, (N == 0) ? 0 : N)._context;

	std::string text	= first->getText();
	std::string source	= first->getSource();

	
	int	firstLine 		= first->getFirstLine();
	int	firstColon 		= first->getFirstColon();
	int	lastLine 		= first->getLastLine();
	int	lastColon 		= first->getLastColon();
	
	//if (N > 0) {
		for (int i = firstContext + 1; i <= N; ++i) {
			idl::parser::Context* c = YYRHSLOC(Rhs, i)._context;

			// skip empty contexts
			if (c->getSource() == EMPTY_CONTEXT_SOURCE) {
				continue;
			}

			// if context intersects in-file boundaries then skip it till the end of the first source
			if (source != c->getSource()) {
				break;
			}

			// in case of error, multiple joining of the same context is possible, so it makes sense to check interval inclusion before joining texts
			if (
					c->getFirstLine() >= firstLine && c->getFirstColon() >= firstColon &&
					c->getLastLine() <= lastLine && c->getLastColon() <= lastColon ) {
				continue;
			}

			lastLine 		= c->getLastLine();
			lastColon 		= c->getLastColon();
			
			if (!c->getText().empty()) {
				text += ' ';
				text += c->getText();
			}
		}
	//}

	*(result._context) = Context(source, firstLine, firstColon, lastLine, lastColon, text);
}

