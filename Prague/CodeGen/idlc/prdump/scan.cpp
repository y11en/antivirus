#include "stdafx.h"
#include <property/property.h>
#include "scantree.h"
#include "datatreeutils/DTUtils.h"
#include <Stuff/CPointer.h>
#include <Stuff/SArray.h>
#include <atlcomcli.h>
#include "../../AVPPveID.h"
#include "Prague/prague.h"
#include <serialize/kldtser.h>

// !data! -- local data structure
//cItem* g_local_data = 0;

static bool WriteNodeAttrib        ( cItem& node, HDATA data, tMode mode, tNodeDsc* dsc, bool verbose, bool type_details );
static bool WriteNodeHeader        ( cItem& node, HDATA data, tMode mode, tNodeDsc* dsc, bool ret_convert );
static bool WriteNodeContent       ( cItem& node, cItem* parent, tMode mode, HDATA data, tNodeDsc* dsc, bool ret_convert, bool verbose );
static bool AdditionalPropAttribs  ( cItem& node, cItem* parent, HDATA data, tNodeDsc* dsc, tMode mode );
static bool MakeDefinitionBySymbDsc( cItem& node, cOutput& buff, HDATA data, tNodeDsc* dsc, tMode mode, uint name_id, bool ret_convert, bool shared_type, bool value );
static bool MakeDefinitionByData   ( cItem& node, cOutput& buff, HDATA data, tNodeDsc* dsc, tMode mode );
       bool ScanTree               ( cItem& node, cItem* parent, tMode mode, HDATA data, tNodeDsc* dsc, bool ret_convert, bool verbose );


// ---
static struct tTypeName {
	uint        m_id;
	const char* m_name;
} gTypes[] = {
  { avpt_nothing,   "nth"   },
	{ avpt_char,      "chr"   },
	{ avpt_wchar,     "wchr"  },
	{ avpt_short,     "shrt"  },
	{ avpt_long,      "long"  },
	{ avpt_longlong,  "llong" },
	{ avpt_byte,      "byte"  },
	{ avpt_group,     "grp"   },
	{ avpt_word,      "word"  },
	{ avpt_dword,     "dw"    },
	{ avpt_qword,     "qw"    },
	{ avpt_int,       "int"   },
	{ avpt_uint,      "uint"  },
	{ avpt_bool,      "bool"  },
	{ avpt_date,      "date"  },
	{ avpt_time,      "tm"    },
	{ avpt_datetime,  "dt"    },
	{ avpt_str,       "str"   },
	{ avpt_wstr,      "wstr"  },
	{ avpt_bin,       "bin"   },
};



// ---
const char* _type_name( uint type ) {
	static char buff[30];
	for( int i=0; i<(sizeof(gTypes)/sizeof(gTypes[0])); ++i ) {
		if ( type == gTypes[i].m_id )
			return gTypes[i].m_name;
	}
	sprintf( buff, "unk(%u)", type );
	return buff;
}




// ---
static const char* global_prop_names[] = {
	"NATIVE_ERR",
	"OBJECT_NAME",
	"OBJECT_NAME_CP",
	"OBJECT_FULL_NAME",
	"OBJECT_FULL_NAME_CP",
	"OBJECT_COMPLETE_NAME",
	"OBJECT_COMPLETE_NAME_CP",
	"OBJECT_ALT_NAME",
	"OBJECT_ALT_NAME_CP",
	"OBJECT_PATH",
	"OBJECT_PATH_CP",
	"OBJECT_SIZE",
	"OBJECT_SIZE_Q",
	"OBJECT_SIGNATURE",
	"OBJECT_SUPPORTED",
	"OBJECT_ORIGIN",
	"OBJECT_OS_TYPE",
	"OBJECT_OPEN_MODE",
	"OBJECT_NEW",
	"OBJECT_CODEPAGE",
	"OBJECT_LCID",
	"OBJECT_ACCESS_MODE",
	"OBJECT_USAGE_COUNT",
	"OBJECT_COMPRESSED_SIZE",
	"OBJECT_HEAP",
	"OBJECT_AVAILABILITY",
	"OBJECT_BASED_ON",
	"OBJECT_ATTRIBUTES",
	"OBJECT_HASH",
	"OBJECT_REOPEN_DATA",
	"OBJECT_INHERITABLE_HEAP",
	"OBJECT_NATIVE_IO",
	"OBJECT_PROP_SYNCHRONIZED",
	"OBJECT_MEM_SYNCHRONIZED",
	"OBJECT_NATIVE_PATH",
	"INTERFACE_ID",
	"INTERFACE_SUBTYPE",
	"INTERFACE_VERSION",
	"INTERFACE_REVISION",
	"INTERFACE_COMMENT",
	"INTERFACE_COMMENT_CP",
	"INTERFACE_SUPPORTED",
	"INTERFACE_FLAGS",
	"INTERFACE_COMPATIBILITY",
	"INTERFACE_CODEPAGE",
	"INTERFACE_COMPATIBILITY_BASE",
	"VENDOR_ID",
	"VENDOR_NAME",
	"VENDOR_NAME_CP",
	"VENDOR_COMMENT",
	"VENDOR_COMMENT_CP",
	"PLUGIN_ID",
	"PLUGIN_VERSION",
	"PLUGIN_NAME",
	"PLUGIN_NAME_CP",
	"PLUGIN_COMMENT",
	"PLUGIN_COMMENT_CP",
	"PLUGIN_HANDLE",
	"PLUGIN_CODEPAGE",
	"PLUGIN_EXPORT_COUNT",
	"TRACE_LEVEL",
	"TRACE_LEVEL_MIN",
	"TRACE_OBJ",
	"PROP_LAST_CALL",
};



// ---
static bool _is_global_prop_name( const char* name ) {
	for( uint i=0; i<countof(global_prop_names); ++i ) {
		if ( !strcmp(name,global_prop_names[i]) )
			return true;
	}
	return false;
}




// ---
static bool WritePropDetalils( cOutput& out, HPROP prop ) {
	AVP_dword nID		  = ::PROP_Get_Id( prop );
	AVP_dword nIDId   = GET_AVP_PID_ID(nID);
	AVP_dword nType   = GET_AVP_PID_TYPE(nID);

	return 
		out.write( "(" ) &&
		out.write( nIDId ) &&
		out.write( "," ) &&
		out.write( _type_name(nType) ) &&
		out.write( ")" );
}



// ---
bool WritePropContent( cOutput& buff, tNodeDsc* dsc, HPROP hProp, tMode mode, const char* prefix, bool open, bool verbose, bool type_details, bool quotation ) {

	AVP_dword nID    = ::PROP_Get_Id( hProp );
	AVP_dword nIDId  = GET_AVP_PID_ID(nID);
	AVP_dword nType  = GET_AVP_PID_TYPE(nID);
	AVP_dword bArray = GET_AVP_PID_ARR(nID);

	if ( g_no_comment ) {
		switch ( nID ) {
			case VE_PID_SHORTCOMMENT:
			case VE_PID_LARGECOMMENT:
			case VE_PID_BEHAVIORCOMMENT:
			case VE_PID_VALUECOMMENT:
			case VE_PID_IMPLCOMMENT: 
			case HP_PID_SHORTCOMMENT:
			case HP_PID_LARGECOMMENT:
			case HP_PID_BEHAVIORCOMMENT:
			//case SP_PID_SHORTCOMMENT:
			//case SP_PID_LARGECOMMENT:
			//case SP_PID_BEHAVIORCOMMENT:
				return true;
		}
	}

	AVP_bool    bool_val = false;
	tEnumDsc*   en = 0;
	const char* name = "v";
	tPropDsc*   pdsc = dsc ? _find_pdsc(nID,dsc->m_props) : 0;

	uint size = ::PROP_Get_Val( hProp, 0, 0 );
	if ( ((nType == avpt_str) && (size <= 1)) || ((nType == avpt_wstr) && (size <= 2)) || !size ) {
		if ( pdsc && !pdsc->m_show_if_empty )
			return true;
	}
	else if ( nType == avpt_bool ) {
		::PROP_Get_Val( hProp, &bool_val, sizeof(bool_val) );
		if ( !bool_val && pdsc && !pdsc->m_show_if_empty )
			return true;
	}

	if ( pdsc ) {
		if ( !verbose && (pdsc->m_ignore || (pdsc->m_id && (dsc->m_name_prop_id == pdsc->m_id))) )
			return true;
		name = pdsc->m_name;
		en = pdsc->m_enum;
		if ( quotation )
			quotation = pdsc->m_quotation;
	}

	if ( prefix )
		buff << prefix;

	if ( type_details ) {
		buff << name;
		if ( !pdsc )
			::WritePropDetalils( buff, hProp );
	}

	if ( (type_details || open) /*&& !bArray*/ )
		buff << "(";

	if ( nType == avpt_nothing )
		buff << "nothing";
	else {
		AVP_dword size = 0;
		AVP_dword val = 0;
		cAutoChar	pcStr;
		char cbuf[100];
		const char* str = 0;

		if ( bArray ) {
			int saved = 0;
			AVP_dword nCount = ::PROP_Arr_Count( hProp );
			for ( AVP_dword i=0; i<nCount; i++ ) {
				cOutput tmp;
				if ( en && (nType == avpt_dword) ) {
					if ( ::PROP_Arr_Get_Items(hProp,i,&val,sizeof(val)) )
						str = _get_enum_name( en, val );
					if ( !str ) {
						sprintf( cbuf, "%u", val );
						str = cbuf;
					}
					tmp << str;
				}
				else {
					str = ::DTUT_GetPropArrayValueAsString( hProp, i, NULL );
					if ( !str )
						return false;// error
					pcStr = (char*)str;

					if ( nID == VE_PID_IF_INCLUDENAMES ) {
						int len = strlen( str );
						if ( !g_no_lower_fn )
							_strlwr( (char*)str );
						if ( !strstr(buff.str(),str) ) {
							if ( (len > 2) && (str[0] == '\"') && (str[len-1]=='\"') )
								tmp << str;
							if ( (len > 2) && (str[0] == '<') && (str[len-1]=='>') )
								tmp << str;
							else if ( !strncmp(str,"i_",2) ) {
								tmp << "<iface/" << str;
								if ( !((str[len-2] == '.') && ((str[len-1] != 'h') || (str[len-1] != 'h'))) )
									tmp << ".h";
								tmp << ">";
							}
							else
								tmp << "<" << str << ">";
						}
					}
					else
						tmp << str;
				}
				if ( tmp.avail() ) {
					if ( saved )
						buff << ", ";
					else
						buff << " ";
					buff << tmp.str();
					++saved;
				}
			}
			if ( saved )
				buff << " ";
		}
		else {
			if ( en && (nType == avpt_dword) ) {
				if ( ::PROP_Get_Val(hProp,&val,sizeof(val)) )
					str = _get_enum_name( en, val );
				if ( !str ) {
					if ( !type_details && name && *name ) {
						int len = strlen(name);
						while( *(name+len-1) == ' ' )
							--len;
						memcpy( cbuf, name, len );
						*(cbuf+len++) = '(';
						len += sprintf( cbuf+len, "0x%08x", val );
						*(cbuf+len++) = ')';
						*(cbuf+len) = 0;
					}
					else
						sprintf( cbuf, "0x%08x", val );
					str = cbuf;
				}
			}
			else if ( (nType == avpt_bool) && (0 != (size=::PROP_Get_Val(hProp,&bool_val,sizeof(bool_val)))) ) {
				if ( bool_val )
					str = "true";
				else
					str = "false";
			}
			else {
				str = ::DTUT_GetPropValueAsString( hProp, NULL );
				pcStr = (char*)str;
				int len = strlen(str);
				while( len && (('\n' == *(str+len-1)) || ('\r' == *(str+len-1))) ) {
					*(((char*)str)+len---1) = 0;
				}
			}
			if ( quotation /*&& (nType == avpt_str) &&*/ )
				buff << "\"";

			if ( nID == VE_PID_IFPLS_MEMBER ) {
				const char* dot = strchr( str, '.' );
				if ( dot )
					str = dot+1;
			}
			char* np = _check_quot( str );
			if ( np )
				str = np;

			buff << str;

			if ( np )
				delete [] np;

			if ( quotation /*&& (nType == avpt_str) &&*/ )
				buff << "\"";
		}
	}

	if ( type_details || open )
		buff << ")";
	return true;
}



// ---
static bool MakeDefinitionBySymbDsc( cItem& node, cOutput& buff, HDATA data, tNodeDsc* dsc, tMode mode, uint name_id, bool ret_convert, bool shared_type, bool value ) {
	
	if ( !dsc )
		return false;

	tSymbDsc* sdsc = dsc->m_symb_dsc;
	if ( !sdsc )
		return false;

	bool local_prop = false;
	char type_name[0x100] = {0};

	if ( shared_type )
		buff << "shared ";
	else if ( (mode == em_imp) && (node.nt() == VE_NT_PROPERTY) && !_is_native(data,VE_PID_IFP_NATIVE) ) {
		local_prop = true;
		buff << "add " n_PROP " ";
	}
	else if ( (mode == em_imp) && (node.nt() == VE_NT_ERRORCODE) && !_is_native(data,VE_PID_IFE_NATIVE) )
		buff << "add " n_ERR " ";
	else if ( (mode == em_imp) && (node.nt() == VE_NT_METHOD) ) {
		HDATA dad = ::DATA_Get_Dad( data, 0 );
		if ( dad )
			dad = ::DATA_Get_Dad( dad, 0 );
		if ( dad ) {
			uint nt = _get_uint( dad, VE_PID_NODETYPE );
			bool unnamed = !::DATA_Find_Prop( dad, 0, VE_PID_IF_INTERFACENAME ); // "plugin" iface
			if ( (nt == VE_NT_INTERFACE) && !unnamed && _get_bool(data,VE_PID_IFM_EXTENDED) )
				buff << "add ";
		}
		buff << n_METH " ";
	}
	else if ( (mode == em_imp) && (node.nt() == VE_NT_MESSAGECLASS) )
		buff << "extend " n_MSG_CL " ";
	else if ( sdsc->m_node_type )
		buff << sdsc->m_node_type << " ";

	if ( _get_bool(data,sdsc->m_const) )
		buff << "const ";

	cAutoChar prx;
	bool pointer = false;
	uint val_type = 0;
	cAutoChar type = _get_type_by_id_prop( data, dsc, val_type, sdsc->m_type_id );
	if ( !type ) {
		HPROP tprop = ::DATA_Find_Prop( data, 0, sdsc->m_type_name );
		if ( tprop && ::PROP_Get_Val(tprop,(void*)type_name,sizeof(type_name)) )
			type = _strdup(type_name);
	}

	if ( ret_convert && (node.nt() == VE_NT_METHOD) && (val_type != tid_ERROR) ) {
		cOutput ret;
		ret << type << "* result";
		node.Add( new cItem(ret.reset(),"result",type,VE_NT_METHODPARAM,",","",true) );
		type = _strdup("tERROR");
	}

	if ( type ) {
		char node_name[0x100] = {0};
		if ( !name_id )
			name_id = dsc->m_name_prop_id;
		HPROP name_prop = ::DATA_Find_Prop( data, 0, name_id );
		if ( name_prop )
			::PROP_Get_Val( name_prop, (void*)node_name, sizeof(node_name) );

		if ( (node.nt() != VE_NT_SYSTEMMETHOD) && !((node.nt()==VE_NT_PROPERTY) && _known_prop(node_name)) ) {
			buff << type;
			if ( _get_bool(data,sdsc->m_pointer) ) {
				buff << "*";
				pointer = true;
				if ( _get_bool(data,sdsc->m_dbl_pointer) ) {
					pointer = false;
					buff << "*";
				}
			}
			buff << " ";
		}
		node.type( type );

		if ( node_name[0] ) {
			if ( !shared_type && (node.nt() == VE_NT_PROPERTY) ) {
				if ( local_prop && !_is_global_prop_name(node_name) ) {
					if ( (node_name[0] != 'p') && (node_name[1] != 'l') )
						buff << "pl";
				}
				else if ( (node_name[0] != 'p') && (node_name[1] != 'g') )
					buff << "pg";
			}

			if ( _check_keyword(node_name) ) {
				switch( node.nt() ) {
					case VE_NT_PLUGIN              : buff << "p_"; break;
					case VE_NT_INTERFACE           : buff << "i_"; break;
					case VE_NT_PROPERTY            : buff << "pr"; break;
					case VE_NT_METHOD              : buff << "mt_"; break;
					case VE_NT_METHODPARAM         : buff << "p_"; break;
					case VE_NT_DATASTRUCTURE       : buff << "s_"; break;
					case VE_NT_DATASTRUCTUREMEMBER : buff << "m_"; break;
					case VE_NT_TYPE                : buff << "t_"; break;
					case VE_NT_CONSTANT            : buff << "c_"; break;
					case VE_NT_SYSTEMMETHOD        : break;
					case VE_NT_SYSTEMMETHODPARAM   : break;
					case VE_NT_ERRORCODE           : buff << "e_"; break;
					case VE_NT_MESSAGECLASS        : buff << "mc_"; break;
					case VE_NT_MESSAGE             : buff << ""; break;
				}
			}

			buff << node_name;
			node.name( node_name );

			if ( (mode == em_prt) && (node.nt() == VE_NT_INTERFACE) )
				_check_base_iface( node, buff, data, dsc );

			bool is_array = false;
			if ( _get_bool(data,sdsc->m_array) ) {
				cAutoChar pcStr = _get_data_as_str( data, sdsc->m_array_bound );
				if ( pcStr && *pcStr ) {
					is_array = true;
					buff << "[" << pcStr << "]";
				}
			}

			if ( value && sdsc->m_value ) {
				HPROP prop = ::DATA_Find_Prop( data, 0, sdsc->m_value );
				if ( prop ) {
					char* value = ::DTUT_GetPropValueAsString( prop, 0 );
					cAutoChar pcStr = (char*)value;
					char* np = _check_quot( value );
					if ( np )
						value = np;
					if ( value && *value ) {
						buff << " = ";
						if ( is_array )
							buff << "{";
						else if ( (*value != '\"') && ((val_type == tid_STRING) || (val_type == tid_WSTRING) || (pointer && ((val_type==tid_CHAR) || (val_type == tid_WCHAR)))) )
							buff << "\"";
						buff << value;
						bool quot = *(value+strlen(value)-1) == '\"';
						if ( is_array )
							buff << "}";
						else if ( !quot && ((val_type == tid_STRING) || (val_type == tid_WSTRING) || (pointer && ((val_type==tid_CHAR) || (val_type == tid_WCHAR)))) )
							buff << "\"";
					}
					if ( np )
						delete [] np;
				}
			}
		}
	}
	return !!type;
}



// ---
static bool MakeDefinitionByData( cItem& node, cOutput& buff, HDATA data, tNodeDsc* dsc, tMode mode ) {

	bool data_struct = (mode == em_imp) && ::_is_datastr_hdl( data );

	// !data! -- make data definition
	if ( data_struct )
		return false;

	HPROP   type_prop = 0;
	bool    sub_data  = data_struct && ::_is_parent_datastr( data );

	cOutput name;
	cOutput type_name;

	bool    has_name = false;
	bool    has_type = false;
	buff.reset();

	// node type
	if ( data_struct ) {
		if ( sub_data )
			type_name << n_STRUCT;
		else
			type_name << "data";
	}
	else if ( dsc && dsc->m_type_prop_id && (0!=(type_prop=::DATA_Find_Prop(data,0,dsc->m_type_prop_id))) ) {
		if ( !::WritePropContent(type_name,dsc,type_prop,mode,0,false,true,false,false) )
			return false;
	}
	else if ( dsc && dsc->m_static_type_name )
		type_name << dsc->m_static_type_name;

	has_type = !!type_name.avail();

	// node name
	HPROP descript = 0;
	if ( dsc && dsc->m_name_prop_id && (0 != (descript=::DATA_Find_Prop(data,0,dsc->m_name_prop_id))) ) {
		if ( (mode == em_imp) && (node.nt() == VE_NT_INTERFACE) ) { // implementation OS_Data : OS
			_data_struct_name( data, 0, name );
			//::WritePropContent( buff, dsc, descript, mode, 0, false, true, false, false );
			node.name( name.str() );
		}
		else if ( data_struct ) {
			_data_struct_name( data, 0, name );
			node.name( name.str() );
		}
		else if ( ::WritePropContent(name,dsc,descript,mode,0,false,true,false,false) )
			node.name( name.str() );
		else
			return false;
		has_name = true;
	}

	if ( has_type ) {
		if ( mode == em_imp ) {
			if ( node.nt() == VE_NT_MESSAGECLASS ) {
				bool native = false;
				CFaceInfo* proto = _get_proto( data );
				if ( proto ) {
					const char* cl_name = name.str();
					CMsgClassInfo msgcl( *proto );
					while( !native && msgcl ) {
						const char* name = msgcl.Name();
						native = !strcmp( name, cl_name );
					}
				}
				else 
					native = _is_native( data, VE_PID_NODEEXPANDED ); // VE_PID_IFMC_NATIVE

				if ( native )
					buff.write( "extend " );
				else
					buff.write( "add " );
			}
			//else if ( (node.nt() == VE_NT_MESSAGE) && !_is_native(data,VE_PID_IFM_NATIVE) )
			//	buff.write( "add " );
			else if ( (node.nt() == VE_NT_METHOD) && _get_bool(data,VE_PID_IFM_EXTENDED) )
				buff.write( "add " );
		}
		buff.write( type_name.reset() );
		node.type( type_name.str() );
	}

	if ( descript ) {
		if ( has_type )
			buff << " ";
		if ( (mode == em_imp) && (node.nt() == VE_NT_INTERFACE) ) { // implementation OS_Data : OS
			buff << name << " : ";
			::WritePropContent( buff, dsc, descript, mode, 0, false, true, false, false );
		}
		else if ( data_struct ) {
			if ( sub_data )
				buff << "tag_" << name;
		}
		else {
			name.reset();
			if ( ::WritePropContent(name,dsc,descript,mode,0,false,true,false,false) )
				buff << name;
		}
	}
	else {
		uint id = GET_AVP_PID_ID( ::DATA_Get_Id(data,0) );
		if ( !buff.write(id) )
			return false;
	}

	if ( (mode == em_prt) && (node.nt() == VE_NT_INTERFACE) ) // interface Buffer : IO
		_check_base_iface( node, buff, data, dsc );

// node value
	bool has_hdr = (has_type || has_name);
	if ( /*level && (root_val ||*/ !has_hdr /*)*/ ) {
		HPROP main_prop = ::DATA_Find_Prop( data, NULL, 0 );
		if ( !main_prop ) {
			buff.write( "bad node" );
			node.hdr( buff.reset() );
			return false;
		}

		const char* prefix;
		if ( has_hdr )
			prefix = ", ";
		else
			prefix = 0;
		if ( !::WritePropContent(buff,dsc,main_prop,mode,prefix,false,true,!type_prop,false) )
			return false;
	}
	return true;
}



// ---
static bool AddTypeRef( cItem& datastr, const char* member, const char* type ) {
	if ( !member ||!*member )
		return false;
	const char* dot = strchr( member, '.' );
	if ( dot ) {
		char name_sect[100];
		int len = dot - member;
		if ( !*(member+len+1) )
			return false;
		memcpy( name_sect, member, len );
		name_sect[len] = 0;
		cItem* substr = _find_memb( datastr, name_sect );
		if ( !substr )
			return false;
		return AddTypeRef( *substr, member+len+1, type );
	}

	cItem* memb = _find_memb( datastr, member );
	if ( !memb )
		return false;

	const char* var_type = memb->type();
	if ( strcmp(type,var_type) ) {
		cOutput buff;
		buff << "typeref(" << type << ")";
		const char* attr_ref = buff.reset();
		for( uint i=0,c=memb->attr_count(); i<c; ++i ) {
			if ( !strcmp(memb->get_attr(i),attr_ref) )
				return true;
		}
		memb->attr( attr_ref );
	}
	return true;
}



// ---
static bool AdditionalPropAttribs( cItem& node, cItem* parent, HDATA data, tNodeDsc* dsc, tMode mode ) {
	if ( (mode != em_imp) || !dsc || dsc->m_name_prop_id != VE_PID_IFP_PROPNAME || node.nt() != VE_NT_PROPERTY )
		return true;

	uint scope = _get_uint( data, VE_PID_IFP_SCOPE );

	cOutput buff;
	CPropInfo pi( data );

	if ( scope == VE_IFP_LOCAL ) {
		CPropInfo::Mode fmode   = pi.FuncMode(); //_get_uint( data, VE_PID_IFPLS_MODE );
		CPropInfo::Mode mmode   = pi.MemberMode(); //_get_uint( data, VE_PID_IFPLS_MEMBER_MODE );

		cAutoChar readf   = _get_data_as_str( data, VE_PID_IFPLS_READFUNC );
		cAutoChar writef  = _get_data_as_str( data, VE_PID_IFPLS_WRITEFUNC );
		cAutoChar memb    = _get_data_as_str( data, VE_PID_IFPLS_MEMBER );

		bool      a_read  = (fmode == CPropInfo::read)  || (fmode == CPropInfo::read_write) || (mmode == CPropInfo::read)  || (mmode == CPropInfo::read_write);
		bool      a_write = (fmode == CPropInfo::write) || (fmode == CPropInfo::read_write) || (mmode == CPropInfo::write) || (mmode == CPropInfo::read_write);
		bool      a_wroni = _get_bool( data, VE_PID_IFPLS_WRITABLEDURINGINIT );
		
		buff << "access(";
		if ( a_read )
			buff << "read";
		if ( a_wroni ) {
			if ( a_read )
				buff << ",";
			buff << "write_on_init";
		}
		else if ( a_write ) {
			if ( a_read )
				buff << ",";
			buff << "write";
		}
		buff << ")";
		node.attr( buff.reset() );

		if ( readf && *readf ) {
			buff.write( "getter(" );
			buff.write( readf );
			buff.write( ")" );
			node.attr( buff.reset() );
		}
		if ( writef && *writef ) {
			buff.write( "setter(" );
			buff.write( writef );
			buff.write( ")" );
			node.attr( buff.reset() );
		}
		if ( memb && *memb ) {
			const char* m = strchr( memb, '.' );
			if ( m )
				++m;
			else
				m = memb;
			buff.write( "member_ref(" );
			buff.write( m );
			buff.write( ")" );
			node.attr( buff.reset() );

			// !data! -- add type reference for data field
			//if ( g_local_data ) {
			//	char type[100];
			//	pi.TypeName( 0, type, sizeof(type) );
			//	AddTypeRef( *g_local_data, memb, type );
			//}
		}
	}
	else {
		uint name_id = VE_PID_IFPSS_VARIABLENAME;
		cAutoChar memb = _get_data_as_str( data, name_id );
		//if ( !memb ||!*memb ) {
		//	name_id = dsc->m_name_prop_id;
		//	memb = _get_data_as_str( data, name_id );
		//}

		node.attr( "shared(true)" );

		if ( memb && *memb ) {
			node.attr( "access(read)" );
			buff.write( "member_ref(" );
			buff.write( memb );
			buff.write( ")" );
			node.attr( buff.reset() );

			// !data! -- add static description
			//if ( parent && ::MakeDefinitionBySymbDsc(node,buff,data,dsc,mode,name_id,false,true,true) ) {
			//	uint i, c = parent->Count();
			//	bool not_found = true;
			//	const char* str = buff.reset();
			//	for( i=0; not_found && (i<c); ++i ) {
			//		cItem& it = *(*parent)[i];
			//		if ( it.hdr() && *it.hdr() )
			//			not_found = !!strcmp( str, it.hdr() );
			//	}
			//	if ( not_found )
			//		parent->Add( new cItem(str,node.name(),node.type(),VE_NT_DATASTRUCTUREMEMBER,";","",false) );
			//	AddTypeRef( *parent, node.name(), node.type() );
			//}
		}
		//else
		//	node.attr( "access(unknown)" );
	}
	return true;
}



// ---
static bool WriteNodeAttrib( cItem& node, HDATA data, tMode mode, tNodeDsc* dsc, bool verbose, bool type_details ) {

	HPROP prop = ::DATA_Find_Prop( data, NULL, 0 );
	if ( !prop ) {
		node.comment( "[bad node]" );
		return true;
	}

	for( prop = ::PROP_Get_Next(prop); prop; prop=::PROP_Get_Next(prop) ) {
		AVP_dword id = ::PROP_Get_Id( prop );
		if ( id == AVP_PID_END )
			break;
		if ( dsc ) {
			tPropDsc* pdsc = _find_pdsc( ::PROP_Get_Id(prop), dsc->m_props );
			if ( pdsc && pdsc->m_ignore )
				continue;
			if ( !g_no_comment && (pdsc->m_id == VE_PID_SHORTCOMMENT) )
				continue;
			switch ( node.nt() ) {
				case VE_NT_SYSTEMMETHOD:
					if ( pdsc && (pdsc->m_id == VE_PID_LARGECOMMENT) )
						continue;
					break;
				case VE_NT_PROPERTY:
					if ( (mode == em_imp) && pdsc && (pdsc->m_id == VE_PID_IFP_DIGITALID) && !_is_native(data,VE_PID_IFP_NATIVE) )
						continue;
					{
						char node_name[0x100];
						HPROP name_prop = ::DATA_Find_Prop( data, 0, dsc->m_name_prop_id );
						if ( name_prop && ::PROP_Get_Val(name_prop,node_name,sizeof(node_name)) && _known_prop(node_name) )
							continue;
					}
					break;
				case VE_NT_METHODPARAM: 
					continue; 
					break;
				case VE_NT_MESSAGECLASS:
					if ( (mode == em_imp) && pdsc && (pdsc->m_id == VE_PID_IFM_MESSAGECLASSID) && _is_native(data,VE_PID_IFMC_NATIVE) )
						continue;
					break;
			}
		}

		cOutput buff;	
		if ( !::WritePropContent(buff,dsc,prop,mode,0,false,verbose,type_details,true) )
			return false;
		if ( buff )
			node.attr( buff.reset() );
	}
	return true;
}




// ---
static bool WriteNodeHeader( cItem& node, HDATA data, tMode mode, tNodeDsc* dsc, bool ret_convert ) {

	if ( !dsc )
		return false;
	if ( node.hdr() && *node.hdr() )
		return true;

	cOutput buff;
	bool header = false;
	if ( dsc->m_symb_dsc ) {
		bool value = (
			(node.nt() == VE_NT_CONSTANT)  || 
			(node.nt() == VE_NT_ERRORCODE) || 
			((node.nt() == VE_NT_PROPERTY) && (VE_IFP_SHARED == _get_uint(data,VE_PID_IFP_SCOPE))));
		header = ::MakeDefinitionBySymbDsc( node, buff, data, dsc, mode, dsc->m_name_prop_id, ret_convert, false, value );
	}
	if ( !header )
		header = ::MakeDefinitionByData( node, buff, data, dsc, mode );

	if ( header ) 
		node.hdr( buff.reset() );
	return header;
}



// ---
static bool WriteNodeContent( cItem& node, cItem* parent, tMode mode, HDATA data, tNodeDsc* dsc, bool ret_convert, bool verbose ) {
	struct cNode {
		tNodeDsc* dsc;
		HDATA     data;
	};

	CSArray<cNode> nodes;

	tNodeDsc* sub = 0;
	if ( dsc ) {
		sub = dsc->m_nodes;
		if ( sub && !dsc->m_array ) {
			MAKE_ADDR1( addr, 0 );
			tNodeDsc* s = sub;
			for( ; (s->m_id != DSC_END_POINT); ++s ) {
				addr[0] = s->m_id;
				HDATA n = ::DATA_Find( data, addr );
				if ( n ) {
					cNode* node = nodes.Add();
					node->dsc = s;
					node->data = n;
				}
			}
		}
	}

	uint i, c;
	HDATA hCurrData = ::DATA_Get_First( data, 0 );
	while ( hCurrData ) {
		for( i=0,c=nodes.Count(); i<c; ++i ) {
			cNode& nod = nodes[i];
			if ( nod.data == hCurrData )
				break;
		}
		if ( !c || (i >= c) ) {
			cNode* nod = nodes.Add();
			if ( sub && !dsc->m_array ) {
				AVP_dword id = DATA_Get_Id( hCurrData, 0 );
				tNodeDsc* s = sub;
				nod->dsc = 0;
				for( ; s && (s->m_id != DSC_END_POINT); ++s ) {
					if ( s->m_id == id ) {
						nod->dsc = s;
						break;
					}
				}
			}
			else
				nod->dsc = sub;
			nod->data = hCurrData;
		}
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}

	if ( !parent )
		parent = &node;

	cItem* curr = &node;
	for( i=0,c=nodes.Count(); i<c; ++i ) {

		cNode& nod = nodes[i];
		bool skip_unnamed_impl_level = false;
		if ( (mode == em_imp) && (nod.dsc->m_name_prop_id == VE_PID_IF_INTERFACENAME) ) {
			uint nodetype = _get_uint( nod.data, VE_PID_NODETYPE );
			if ( nodetype == VE_NT_INTERFACE ) {
				HPROP nprop = ::DATA_Find_Prop( nod.data, 0, nod.dsc->m_name_prop_id );
				if ( !nprop )
					skip_unnamed_impl_level = true;
				else {
					cAutoChar name = ::DTUT_GetPropValueAsString( nprop, 0 );
					skip_unnamed_impl_level = !name || !*name;
				}
			}
		}
		
		cItem* item;
		if ( skip_unnamed_impl_level ) { // это интерфейс плагина (где только экспортные методы)
			ret_convert = false;
			item = parent;
		}
		else
			item = new cItem;

		if ( ::ScanTree(*item,parent,mode,nod.data,nod.dsc,ret_convert,verbose) ) {
			uint cnt = curr->Count();
			if ( cnt ) {
				cItem* pitem = (*curr)[cnt-1];
				pitem->no_open( false );
			}
			bool data_str = (mode == em_imp) && ::_is_datastr_hdl(nod.data);
			bool mescl    = (mode == em_imp) && (item->nt() == VE_NT_MESSAGECLASS);
			bool check_content = item->section() || data_str || mescl;
			if ( check_content && !item->content() ) {
				if ( item != parent )
					delete item;
				item = 0;
			}
			if ( item && (item != parent) ) {
				if ( *item )
					curr->Add( item );
				else {
					delete item;
					item = 0;
				}
			}
		}
		else {
			if ( item != parent )
				delete item;
			item = 0;
		}

		if ( item && (i==(c-1)) && nod.dsc && nod.dsc->m_last_no_open )
			item->no_open( true );
	}

	return true;
}



// ---
bool ScanTree( cItem& node, cItem* parent, tMode mode, HDATA data, tNodeDsc* dsc, bool ret_convert, bool verbose ) {
	if ( !dsc || !dsc->m_hidden ) {

		node.nt( _get_uint(data,VE_PID_NODETYPE) );

		if ( parent ) {
			if ( (node.nt() == VE_NT_PROPERTY) && _check_version_comment_props(*parent,mode,data,dsc) && !verbose )
				return true;
			if ( _check_entity_defined(parent->base(),data) )
				return true;
		}

		if ( mode == em_prt ) {
			if ( node.nt() == VE_NT_INTERFACE ) {
				g_root_item->curr_iface( data );
				_make_iface_imp_def( *g_root_item, mode, data );
				_make_typedef( *g_root_item, data );
			}
		}
		else if ( mode == em_imp ) {
			bool native = false;
			switch( node.nt() ) {
				case VE_NT_PLUGIN              : _make_plg_imp_def( *g_root_item, data ); break;
				case VE_NT_INTERFACE           : g_root_item->curr_iface( data ); /*_make_iface_imp_def( *g_root_item, mode, data );*/ break;
				case VE_NT_PROPERTY            : break;
				case VE_NT_METHOD              : native = /*ret_convert ? (*/_is_native(data,0)/* || !_get_bool(data,VE_PID_IFM_EXTENDED)) : false*/; break;
				case VE_NT_METHODPARAM         : break;
				case VE_NT_DATASTRUCTURE       : break;
				case VE_NT_DATASTRUCTUREMEMBER : break;
				case VE_NT_TYPE                : native = _is_native(data,VE_PID_IFT_NATIVE); break;
				case VE_NT_CONSTANT            : native = _is_native(data,VE_PID_IFC_NATIVE); break;
				case VE_NT_SYSTEMMETHOD        : break;
				case VE_NT_SYSTEMMETHODPARAM   : native = true;
				case VE_NT_ERRORCODE           : native = _is_native(data,VE_PID_IFE_NATIVE); break;
				case VE_NT_MESSAGECLASS        : break;
				case VE_NT_MESSAGE             : native = _is_native(data,VE_PID_IFM_NATIVE); break;
			}
			if ( native )
				return false;
		}

		if ( dsc && dsc->m_symb_dsc ) {
			tSymbDsc* sdsc = dsc->m_symb_dsc;
			if ( (mode != em_imp) || (node.nt() != VE_NT_METHOD) || ret_convert ) {
				if ( sdsc->m_show && (sdsc->m_show != DSC_END_POINT) && !_get_bool(data,sdsc->m_show) )
					return false;
				if ( sdsc->m_not_show && (sdsc->m_not_show != DSC_END_POINT) && _get_bool(data,sdsc->m_not_show) )
					return false;
			}
		}

		cOutput buff;
		if ( ::DATA_Find_Prop(data,0,VE_PID_SEPARATOR) ) {
			::WritePropContent( buff, dsc, ::DATA_Find_Prop(data,0,VE_PID_SHORTCOMMENT), mode, "// ", false, true, false, false );
			node.section( true );
			if ( buff )
				node.comment( buff.reset() );
		}
		else {
			node.cplace( !dsc ? ecp_b : dsc->m_comment_placement );
			if ( dsc ) {
				if ( dsc->m_open )
					node.open( dsc->m_open );
				if ( dsc->m_fin )
					node.close( dsc->m_fin );
			}

			if ( (mode != em_imp) || (node.nt() != VE_NT_SYSTEMMETHOD) ) {
				HPROP comment = ::DATA_Find_Prop( data, 0, VE_PID_SHORTCOMMENT );
				if ( comment ) {
					::WritePropContent( buff, dsc, comment, mode, 0, false, true, false, false );
					if ( buff )
						node.comment( buff.reset() );
				}
			}

			if ( !::WriteNodeAttrib(node,data,mode,dsc,verbose,true) )
				return false;

			if ( !::WriteNodeHeader(node,data,mode,dsc,ret_convert) )
				return false;

			if ( !::WriteNodeContent(node,0,mode,data,dsc,ret_convert,verbose) )
				return false;

			if ( dsc && (mode == em_imp) && ::_is_datastr_hdl(data) )
				node.print_name( ::_is_parent_datastr(data) );

			if ( (mode == em_imp) && dsc && (node.nt() == VE_NT_PROPERTY) )
				::AdditionalPropAttribs( node, parent, data, dsc, mode );
			if ( node.nt() == VE_NT_PROPERTY ) {
				int i = 0;
			}
		}
		return true;
	}

	if ( dsc && dsc->m_hidden && dsc->m_array && dsc->m_static_type_name && *dsc->m_static_type_name ) {
		cOutput buff;	
		node.section( true );
		node.comment( dsc->m_static_type_name );
		if ( !::WriteNodeContent(node,parent,mode,data,dsc,ret_convert,verbose) )
			return false;
		return true;
	}

	return false;
}



