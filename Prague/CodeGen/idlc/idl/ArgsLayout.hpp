#ifndef idl_ArgsLayout_hpp
#define idl_ArgsLayout_hpp

namespace idl {

#define NS_ENUM(op) struct op { enum {
#define NS_ENUM_END };};

struct ArgsLayout {
	NS_ENUM(UnaryOp)
		Arg		,
	NS_ENUM_END
				
	NS_ENUM(BinaryOp)
		First	,
		Second	,
	NS_ENUM_END
				
	NS_ENUM(Dot)
		Left	,
		Right	,
	NS_ENUM_END
				
	NS_ENUM(DotUnary)
		Right	,
	NS_ENUM_END
				
	NS_ENUM(ErrorDef)
		Attrs	,
		Name	,
		Value	,
	NS_ENUM_END
				
	NS_ENUM(Message)
		Attrs	,
		Name	,
	NS_ENUM_END

	NS_ENUM(MessageAdd)
		Attrs	,
		Name	,
	NS_ENUM_END

	NS_ENUM(MessageClass)
		Attrs	,
		Name	,
		Body	,
	NS_ENUM_END

	NS_ENUM(MessageClassExtend)
		Attrs	,
		Name	,
		Body	,
	NS_ENUM_END

	NS_ENUM(Namespace)
		Name	,
	NS_ENUM_END
				
	NS_ENUM(Interface)
		Attrs	,
		Name	,
		Bases	,
		Body	,
	NS_ENUM_END
				
	NS_ENUM(Implementation)
		Attrs	,
		Name	,
		Bases	,
		Body	,
	NS_ENUM_END
				
	NS_ENUM(ImportName)
		Name	,
		File	,
	NS_ENUM_END
				
	NS_ENUM(ImportNameAsType)
		Name	,
		Type	,
		File	,
	NS_ENUM_END
				
	NS_ENUM(InterfaceId)
		Id	,
	NS_ENUM_END
				
	NS_ENUM(Component)
		Attrs	,
		Name	,
		Body	,
	NS_ENUM_END
				
	NS_ENUM(ComponentId)
		Id	,
	NS_ENUM_END
				
	NS_ENUM(ComponentVersion)
		Version	,
	NS_ENUM_END
				
	NS_ENUM(ComponentVendor)
		Name	,
	NS_ENUM_END
	
	//NS_ENUM(ComponentFacet)
	//	Name	,
	//	Body	,
	//NS_ENUM_END
	//			
	//NS_ENUM(ComponentFacetInterface)
	//	Name	,
	//NS_ENUM_END
	//			
	//NS_ENUM(ComponentFacetVersion)
	//	Version	,
	//NS_ENUM_END
				
	NS_ENUM(Const)
		Attrs	,
		Type	,
		Name	,
		Value	,
	NS_ENUM_END

	NS_ENUM(ConstType)
		Type	,
	NS_ENUM_END

	NS_ENUM(Enum)
		Attrs	,
		Name	,
		Body	,
	NS_ENUM_END
	
	NS_ENUM(EnumEntry)
		Name	,
	NS_ENUM_END

	NS_ENUM(RefType)
		Type	,
	NS_ENUM_END

	NS_ENUM(Struct)
		Attrs	,
		Name	,
		Body	,
	NS_ENUM_END
	
	NS_ENUM(StructEntry)
		Type	,
		Name	,
	NS_ENUM_END

	NS_ENUM(SysMethod)
		Attrs	,
		Name	,
	NS_ENUM_END

	NS_ENUM(Typedef)
		Attrs	,
		Type	,
		Name	,
	NS_ENUM_END

	NS_ENUM(Method)
		Attrs	,
		Type	,
		Name	,
		Args	,
	NS_ENUM_END
	
	NS_ENUM(ExternMethod)
		Attrs	= Method::Attrs,
		Type	= Method::Type,
		Name	= Method::Name,
		Args	= Method::Args,
	NS_ENUM_END
	
	NS_ENUM(MethodArg)
		Type	,
		Name	,
	NS_ENUM_END
	
	NS_ENUM(Property)
		Attrs,
		Type,
		Name,
		Optional,
	NS_ENUM_END

	NS_ENUM(PropertyPlugin)
		Attrs,
		Type,
		Name,
		Value,
	NS_ENUM_END

	NS_ENUM(PropertyPluginAdd)
		Attrs,
		Type,
		Name,
		Value,
	NS_ENUM_END

	//NS_ENUM(Sequence)
	//	Type,
	//	Size,
	//NS_ENUM_END

	NS_ENUM(Vendor)
		Name,
		Body,
	NS_ENUM_END
				
	NS_ENUM(VendorId)
		Id,
	NS_ENUM_END
				
	NS_ENUM(ForwardIFace)
		Name,
	NS_ENUM_END

	NS_ENUM(ForwardStruct)
		Name,
	NS_ENUM_END

	NS_ENUM(PropertyDecl)
		Attrs,
		Name,
	NS_ENUM_END

	NS_ENUM(Typecast)
		Name,
		Type,
		Expr,
	NS_ENUM_END

};

}

#endif //idl_ArgsLayout_hpp
