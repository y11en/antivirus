#ifndef AttributeNode_hpp
#define AttributeNode_hpp

#include "grammar.hpp"
#include "NodeInterface.hpp"
#include "Tuple.hpp"

namespace idl { namespace parser {

class AttributeNode : public NodeBase<AttributeNode> {
public:
	enum TypeId {
		TypeString,
		TypeInt,
		TypeTuple,
	};
	
	enum AttributeId {
		NotAnAttr          = -1,
		Id                 = lx_InterfaceId	,
		Version            = lx_Version		,
		Mnemonic           = lx_Mnemonic		,
		C_Type_Name        = lx_C_Type_Name	,
		Cpp_Type_Name      = lx_Cpp_Type_Name	,
		Shared             = lx_Shared		,
		Warning            = lx_Warning	,
		Access             = lx_Access		,
		Getter             = lx_Getter		,
		Setter             = lx_Setter		,
		Member_Ref         = lx_Member_Ref	,
		Comment            = lx_Comment	,
		Comment_Ex         = lx_Comment_Ex	,
		Autostart          = lx_Autostart	,
		Output_Header_File = lx_Output_Header_File	,
		Output_Source_File = lx_Output_Source_File	,
		Include_Files      = lx_Include_Files		,
		Codepage           = lx_Codepage	,
		UnknownPropGetter  = lx_UnknownPropGetter	,
		UnknownPropSetter  = lx_UnknownPropSetter	,
	};

	AttributeNode(const Context& context, AttributeId id, const idl::scaner::String& value);
	AttributeNode(const Context& context, AttributeId id, const idl::scaner::Int& value);
	AttributeNode(const Context& context, AttributeId id, Tuple::Ptr value);

	AttributeId                 getId() const;
	const idl::scaner::String&  getString() const;
	const idl::scaner::Int&     getInt() const;
	Tuple::Ptr                  getTuple() const;

private:
	void ensureType( TypeId typeId );

private:
	AttributeId			    _id;

	idl::scaner::String _stringValue;
	idl::scaner::Int	  _intValue;
	Tuple::Ptr			    _tupleValue;
};
}

struct AccessTools {
	static bool has(parser::AttributeNode::Ptr attr, int right);
	static bool hasRead(parser::AttributeNode::Ptr attr);
	static bool hasWrite(parser::AttributeNode::Ptr attr);
	static bool hasWriteOnInit(parser::AttributeNode::Ptr attr);
	static bool isReadOnly(parser::AttributeNode::Ptr attr);
	static bool isWriteOnly(parser::AttributeNode::Ptr attr);
	static bool isReadWrite(parser::AttributeNode::Ptr attr);
	static bool isEmpty(parser::AttributeNode::Ptr attr);
};

} //namespace idl::parser

#endif //AttributeNode_hpp
