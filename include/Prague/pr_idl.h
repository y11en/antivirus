#ifndef pr_idl_h
#define pr_idl_h

#include <Prague/pr_int.h>

#define CAST_FROM_MEMBER_FN(fn)	((tDATA)cIFnCust((TFn)&fn))

#define DATA_STRUCT_BEGIN(impl)		\
	struct tag_t##impl##_Data {		\
		DATA_STRUCT_##impl##_FIELDS

#define DATA_STRUCT_END(Impl)	};

#define PROPERTY_MAKE(id, offaddr, size, mode, getter, setter) (tDATA)(id), (tDATA)(offaddr), (tDATA)(size), (tDATA)(mode), (tDATA)(getter), (tDATA)(setter), 

#define CPP_DEFINE_IMPLEMENTATION(scope) \
		const size_t scope::Base::ActualObjectSize = sizeof(scope);						\
		tERROR pr_call scope::Base::ObjectNew( hOBJECT object, tBOOL construct ) {      \
			if (construct)                                                              \
				new ((void*)object) scope;                                              \
			else                                                                        \
				((scope*)object)->scope::~scope();                                      \
			return errOK;                                                               \
		}


#undef pgNATIVE_ERR                  

#undef pgOBJECT_NAME                 
#undef pgOBJECT_NAME_CP              
#undef pgOBJECT_FULL_NAME            
#undef pgOBJECT_FULL_NAME_CP         
#undef pgOBJECT_COMPLETE_NAME        
#undef pgOBJECT_COMPLETE_NAME_CP     
#undef pgOBJECT_ALT_NAME             
#undef pgOBJECT_ALT_NAME_CP          
#undef pgOBJECT_PATH                 
#undef pgOBJECT_PATH_CP              
#undef pgOBJECT_SIZE                 
#undef pgOBJECT_SIZE_Q               
#undef pgOBJECT_SIGNATURE            
#undef pgOBJECT_SUPPORTED            
#undef pgOBJECT_ORIGIN               
#undef pgOBJECT_OS_TYPE              
#undef pgOBJECT_OPEN_MODE            
#undef pgOBJECT_NEW                  
#undef pgOBJECT_CODEPAGE             
#undef pgOBJECT_LCID                 
#undef pgOBJECT_ACCESS_MODE          
#undef pgOBJECT_USAGE_COUNT          
#undef pgOBJECT_COMPRESSED_SIZE      
#undef pgOBJECT_HEAP                 
#undef pgOBJECT_AVAILABILITY         
#undef pgOBJECT_BASED_ON             
#undef pgOBJECT_ATTRIBUTES           
#undef pgOBJECT_HASH                 
#undef pgOBJECT_REOPEN_DATA            // applicable for OS, ObjPtr & IO objects: unique name and/or state of the object used for reopen it at the same state
#undef pgOBJECT_INHERITABLE_HEAP     
#undef pgOBJECT_NATIVE_IO            
#undef pgOBJECT_PROP_SYNCHRONIZED    
#undef pgOBJECT_MEM_SYNCHRONIZED     
#undef pgOBJECT_NATIVE_PATH          

#undef pgINTERFACE_ID                
#undef pgINTERFACE_SUBTYPE           
#undef pgINTERFACE_VERSION           
#undef pgINTERFACE_REVISION          
#undef pgINTERFACE_COMMENT           
#undef pgINTERFACE_COMMENT_CP        
#undef pgINTERFACE_SUPPORTED         
#undef pgINTERFACE_FLAGS             
#undef pgINTERFACE_COMPATIBILITY     
#undef pgINTERFACE_CODEPAGE          
#undef psINTERFACE_COMPATIBILITY_BASE

#undef pgVENDOR_ID                   
#undef pgVENDOR_NAME                 
#undef pgVENDOR_NAME_CP              
#undef pgVENDOR_COMMENT              
#undef pgVENDOR_COMMENT_CP           

#undef pgPLUGIN_ID                   
#undef pgPLUGIN_VERSION              
#undef pgPLUGIN_NAME                 
#undef pgPLUGIN_NAME_CP              
#undef pgPLUGIN_COMMENT              
#undef pgPLUGIN_COMMENT_CP           
#undef pgPLUGIN_HANDLE               
#undef pgPLUGIN_CODEPAGE             
#undef pgPLUGIN_EXPORT_COUNT         

#undef pgTRACE_LEVEL                 
#undef pgTRACE_LEVEL_MIN             
#undef pgTRACE_OBJ                   

#endif //pr_idl_h
