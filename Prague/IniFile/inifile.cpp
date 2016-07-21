// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  15 February 2005,  17:12 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Pavlushchik
// File Name   -- inifile.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define IniFile_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/plugin/p_inifile.h>
// AVP Prague stamp end



#include <string.h>
#include "../IniLib/IniLib.h"


// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable IniFile : public cIniFile /*cObjImpl*/ 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();

// Property function declarations
	tERROR pr_call IOPropGet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call IOPropSet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call DecodeIOInterfaceSet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call Deserialize( cSerializable* p_pSerializable );
	tERROR pr_call GetSectionsCount( tDWORD* p_pdwSectionsCount );
	tERROR pr_call FindSection( tSTRING p_sSectionName, tDWORD* p_pdwSectionIndex );
	tERROR pr_call EnumSections( tDWORD p_dwSectionIndex, tSTRING* p_psSectionName, tDWORD* p_pdwValuesCount );
	tERROR pr_call GetValuesCount( tDWORD p_dwSectionIndex, tDWORD* p_pdwValuesCount );
	tERROR pr_call EnumValues( tDWORD p_dwSectionIndex, tDWORD p_dwValueIndex, tSTRING* p_psValueName, tSTRING* p_psValueData );
	tERROR pr_call GetValue( tSTRING p_sSectionName, tSTRING p_sValueName, tSTRING* p_psValueData );
	tERROR pr_call SetValue( tSTRING p_sSectionName, tSTRING p_sValueName, tSTRING p_sValueData );
	tERROR pr_call DelValue( tSTRING p_sSectionName, tSTRING p_sValueName );
	tERROR pr_call DelSection( tSTRING p_sSectionName );
	tERROR pr_call Commit();
	tERROR pr_call SetValueByIndex( tDWORD p_dwSectionIndex, tDWORD p_dwValueIndex, tSTRING p_sValueData );
	tERROR pr_call DelValueByIndex( tDWORD p_dwSectionIndex, tDWORD p_dwValueIndex );

// Data declaration
// AVP Prague stamp end

	hIO				m_hIO;
	sIni			m_ini;
	tBOOL			b_InsideIO;

private:
	sIniSection* GetSectionByIndex(sIniSection* pHomeSection,tDWORD dIndex);
	sIniLine* GetLineByIndex(sIniSection* head,tDWORD dIndex,sIniLine** prev);

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(IniFile)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IniFile". Static(shared) property table variables
const tCODEPAGE PlugInCP = cCP_UNICODE; // must be READ_ONLY at runtime
// AVP Prague stamp end


// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR IniFile::ObjectInit()
{
//	__asm int 3;

	tERROR error;
	PR_TRACE_FUNC_FRAME( "IniFile::ObjectInit method" );

	error = sysCreateObject((hOBJECT*)&m_hIO, IID_IO, PID_NATIVE_FIO);
	if (PR_FAIL(error))
		PR_TRACE((this, prtERROR, "ini\tCannot create IO object, %terr", error));

	memset(&m_ini, 0, sizeof(m_ini));
	b_InsideIO=1;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR IniFile::ObjectInitDone()
{
	tERROR error;

	PR_TRACE_FUNC_FRAME( "IniFile::ObjectInitDone method" );

	if (m_hIO&&b_InsideIO)
	{
		error = m_hIO->sysCreateObjectDone();
		if (PR_FAIL(error))
		{
			PR_TRACE((this, prtERROR, "ini\tCannot done IO object, %terr", error));
			return error;
		}
	}

//	__asm int 3;

	error = IniFileLoad(&m_ini, m_hIO);
	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtERROR, "ini\tCannot load ini file, %terr", error));
		return error;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR IniFile::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IniFile::ObjectPreClose method" );

	error = IniFileFree(&m_ini);
	if (m_hIO&&b_InsideIO) m_hIO->sysCloseObject();

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, IOPropGet )
// Interface "IniFile". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_FULL_NAME
//  -- OBJECT_PATH
//! tERROR IniFile::IOPropGet( tDWORD* out_size, tPROPID prop, tWCHAR* buffer, tDWORD size )
//! tERROR IniFile::IOPropGet( tDWORD* out_size, tPROPID prop, tWCHAR* buffer, tDWORD size )
tERROR IniFile::IOPropGet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* multyproperty method IOPropGet" );

	switch ( prop ) {
		case pgOBJECT_NAME:
		case pgOBJECT_FULL_NAME:
		case pgOBJECT_PATH:
			error = CALL_SYS_PropertyGetStr(m_hIO,out_size, prop, buffer, size, cCP_UNICODE);
//			error = m_hIO->propGet(out_size, prop, buffer, size);
			break;

		default:
			error = errPARAMETER_INVALID;
			*out_size = 0;
			break;
	}

	PR_TRACE_A2( this, "Leave *GET* multyproperty method IOPropGet, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, IOPropSet )
// Interface "IniFile". Method "Set" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_FULL_NAME
//  -- OBJECT_PATH
//! tERROR IniFile::IOPropSet( tDWORD* out_size, tPROPID prop, tWCHAR* buffer, tDWORD size )
//! tERROR IniFile::IOPropSet( tDWORD* out_size, tPROPID prop, tWCHAR* buffer, tDWORD size )
tERROR IniFile::IOPropSet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* multyproperty method IOPropSet" );

	switch ( prop ) {

		case pgOBJECT_NAME:
		case pgOBJECT_FULL_NAME:
		case pgOBJECT_PATH:
			error = CALL_SYS_PropertySetStr(m_hIO,out_size, prop, buffer, size, cCP_UNICODE);
//			error = m_hIO->propSet(out_size, prop, buffer, size);
			break;

		default:
			error = errPARAMETER_INVALID;
			*out_size = 0;
			break;
	}

	PR_TRACE_A2( this, "Leave *SET* multyproperty method IOPropSet, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, DecodeIOInterfaceSet )
// Interface "IniFile". Method "Set" for property(s):
//  -- m_hIO
tERROR IniFile::DecodeIOInterfaceSet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method DecodeIOInterfaceSet for property pgm_hIO" );

//__asm int 3;

	this->sysCheckObject(*((cObject**)buffer),IID_IO,PID_ANY,SUBTYPE_ANY,cFALSE);
	if (PR_SUCC(error))
	{
		if (m_hIO&&b_InsideIO) m_hIO->sysCloseObject();
		m_hIO=*((hIO*)buffer);
		*out_size = 0;
		b_InsideIO=0;
	}
	else
		PR_TRACE((this, prtERROR, "ini\tCannot set outsider IO object, %terr", error));

	PR_TRACE_A2( this, "Leave *SET* method DecodeIOInterfaceSet for property pgm_hIO, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Deserialize )
// Parameters are:
tERROR IniFile::Deserialize( cSerializable* p_pSerializable )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "IniFile::Deserialize method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSectionsCount )
// Parameters are:
tERROR IniFile::GetSectionsCount( tDWORD* p_pdwSectionsCount )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IniFile::GetSectionsCount method" );

	// Place your code here

	if (!p_pdwSectionsCount) return errPARAMETER_INVALID;
	*p_pdwSectionsCount=m_ini.section_count;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, FindSection )
// Parameters are:
tERROR IniFile::FindSection( tSTRING p_sSectionName, tDWORD* p_pdwSectionIndex )
{
	tERROR error = errOK;
	tDWORD Count = 0;
	PR_TRACE_FUNC_FRAME( "IniFile::FindSection method" );

//	__asm int 3;
	// Place your code here

	if (!p_pdwSectionIndex) return errPARAMETER_INVALID;

	sIniSection* head=m_ini.head;
	while(head)
	{
		if(!_stricmp(p_sSectionName,head->name))
		{
			*p_pdwSectionIndex=Count;
			return errOK;
		}
		Count++;
		head=head->next;
	}

	return errNOT_MATCHED;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, EnumSections )
// Parameters are:
tERROR IniFile::EnumSections( tDWORD p_dwSectionIndex, tSTRING* p_psSectionName, tDWORD* p_pdwValuesCount )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IniFile::EnumSections method" );

	// Place your code here

	if (!p_pdwValuesCount|!p_psSectionName) return errPARAMETER_INVALID;
	if (p_dwSectionIndex>=m_ini.section_count) return errEND_OF_THE_LIST;

	sIniSection* head=GetSectionByIndex(m_ini.head,p_dwSectionIndex);
	*p_pdwValuesCount=head->line_count;
	*p_psSectionName=head->name;

	return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetValuesCount )
// Parameters are:
tERROR IniFile::GetValuesCount( tDWORD p_dwSectionIndex, tDWORD* p_pdwValuesCount )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IniFile::GetValuesCount method" );

	// Place your code here

	if (!p_pdwValuesCount) return errPARAMETER_INVALID;
	if (p_dwSectionIndex>=m_ini.section_count) return errEND_OF_THE_LIST;

	sIniSection* head=GetSectionByIndex(m_ini.head,p_dwSectionIndex);
	*p_pdwValuesCount=head->line_count;

	return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, EnumValues )
// Parameters are:
tERROR IniFile::EnumValues( tDWORD p_dwSectionIndex, tDWORD p_dwValueIndex, tSTRING* p_psValueName, tSTRING* p_psValueData )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IniFile::EnumValues method" );

	// Place your code here

	if (!p_psValueData|!p_psValueName) return errPARAMETER_INVALID;

	if (p_dwSectionIndex>=m_ini.section_count) return errEND_OF_THE_LIST;
	sIniSection* head=GetSectionByIndex(m_ini.head,p_dwSectionIndex);

	if (head->line_count<=p_dwValueIndex) return errEND_OF_THE_LIST;
	sIniLine* line=GetLineByIndex(head,p_dwValueIndex,NULL);

	*p_psValueData=line->value;
	*p_psValueName=line->name;

	return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetValue )
// Parameters are:
tERROR IniFile::GetValue( tSTRING p_sSectionName, tSTRING p_sValueName, tSTRING* p_psValueData )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IniFile::GetValue method" );

	// Place your code here

	if (!p_psValueData) return errPARAMETER_INVALID;

	*p_psValueData=IniGetValue(m_ini.head,p_sSectionName,p_sValueName);
	return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetValue )
// Parameters are:
tERROR IniFile::SetValue( tSTRING p_sSectionName, tSTRING p_sValueName, tSTRING p_sValueData )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IniFile::SetValue method" );
	
	// Place your code here
	
	//__asm int 3;
	IniSetValue(IniCreateLine(IniCreateSection(&m_ini, p_sSectionName), p_sValueName), p_sValueData);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, DelValue )
// Parameters are:
tERROR IniFile::DelValue( tSTRING p_sSectionName, tSTRING p_sValueName )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IniFile::DelValue method" );
	
	// Place your code here
	
//__asm int 3;
	IniDelLine(IniGetSection(m_ini.head, p_sSectionName), p_sValueName);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, DelSection )
// Parameters are:
tERROR IniFile::DelSection( tSTRING p_sSectionName )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IniFile::DelSection method" );
	
	// Place your code here
	
//__asm int 3;
	return IniDelSection(&m_ini, p_sSectionName);
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Commit )
// Parameters are:
tERROR IniFile::Commit()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IniFile::Commit method" );
	
	// Place your code here
	
	tCHAR*	buffer=NULL;
	tDWORD	size=0;
	tDWORD	w_size=0;
	
	//__asm int 3;

	tDWORD old_accessmode=m_hIO->get_pgOBJECT_ACCESS_MODE();
	m_hIO->set_pgOBJECT_ACCESS_MODE(fACCESS_WRITE|fACCESS_NO_BUFFERING_WRITE);
	error = IniFileSave(&m_ini, m_hIO, NULL);
	m_hIO->set_pgOBJECT_ACCESS_MODE(old_accessmode);

	if (PR_FAIL(error))
		PR_TRACE((this, prtERROR, "ini\tCannot save new ini file, %terr", error));
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetValueByIndex )
// Parameters are:
tERROR IniFile::SetValueByIndex( tDWORD p_dwSectionIndex, tDWORD p_dwValueIndex, tSTRING p_sValueData )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "IniFile::SetValueByIndex method" );

	// Place your code here

	sIniLine* line=0;
	error=errNOT_FOUND;
	if (p_dwSectionIndex>=m_ini.section_count) return errEND_OF_THE_LIST;
	sIniSection* sec=GetSectionByIndex(m_ini.head,p_dwSectionIndex);
	if (sec)
	{
//		line=IniGetLine(sec,p_sValueName);
		if (sec->line_count<p_dwValueIndex) return errEND_OF_THE_LIST;
		line=GetLineByIndex(sec,p_dwValueIndex,NULL);
		if (line)
			error = IniSetValue(line, p_sValueData);
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, DelValueByIndex )
// Parameters are:
tERROR IniFile::DelValueByIndex( tDWORD p_dwSectionIndex, tDWORD p_dwValueIndex )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "IniFile::DelValueByIndex method" );

	// Place your code here

	if (p_dwSectionIndex>=m_ini.section_count) return errEND_OF_THE_LIST;
	sIniSection* sec=GetSectionByIndex(m_ini.head,p_dwSectionIndex);
	if (sec)
	{
		if (sec->line_count<p_dwValueIndex) return errEND_OF_THE_LIST;
		sIniLine* prv_line=0;
		sIniLine* line=GetLineByIndex(sec,p_dwValueIndex,&prv_line);
		if (line)
			error = IniDelLineEx(sec, line, prv_line);
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "IniFile". Register function
tERROR IniFile::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(IniFile_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, IniFile_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "", 1 )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_NAME, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_FULL_NAME, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_PATH, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgm_hIO, NULL, FN_CUST(DecodeIOInterfaceSet) )
	mSHARED_PROPERTY_PTR( pgPLUGIN_CODEPAGE, PlugInCP, sizeof(PlugInCP) )
mpPROPERTY_TABLE_END(IniFile_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(IniFile)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(IniFile)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(IniFile)
	mEXTERNAL_METHOD(IniFile, Deserialize)
	mEXTERNAL_METHOD(IniFile, GetSectionsCount)
	mEXTERNAL_METHOD(IniFile, FindSection)
	mEXTERNAL_METHOD(IniFile, EnumSections)
	mEXTERNAL_METHOD(IniFile, GetValuesCount)
	mEXTERNAL_METHOD(IniFile, EnumValues)
	mEXTERNAL_METHOD(IniFile, GetValue)
	mEXTERNAL_METHOD(IniFile, SetValue)
	mEXTERNAL_METHOD(IniFile, DelValue)
	mEXTERNAL_METHOD(IniFile, DelSection)
	mEXTERNAL_METHOD(IniFile, Commit)
	mEXTERNAL_METHOD(IniFile, SetValueByIndex)
	mEXTERNAL_METHOD(IniFile, DelValueByIndex)
mEXTERNAL_TABLE_END(IniFile)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "IniFile::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_INIFILE,                            // interface identifier
		PID_INIFILE,                            // plugin identifier
		0x00000000,                             // subtype identifier
		IniFile_VERSION,                        // interface version
		VID_MIKE,                               // interface developer
		&i_IniFile_vtbl,                        // internal(kernel called) function table
		(sizeof(i_IniFile_vtbl)/sizeof(tPTR)),  // internal function table size
		&e_IniFile_vtbl,                        // external function table
		(sizeof(e_IniFile_vtbl)/sizeof(tPTR)),  // external function table size
		IniFile_PropTable,                      // property table
		mPROPERTY_TABLE_SIZE(IniFile_PropTable),// property table size
		sizeof(IniFile)-sizeof(cObjImpl),       // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"IniFile(IID_INIFILE) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call IniFile_Register( hROOT root ) { return IniFile::Register(root); }
// AVP Prague stamp end



sIniSection* IniFile::GetSectionByIndex(sIniSection* pHomeSection,tDWORD dIndex)
{
	tDWORD i;
	sIniSection* head=pHomeSection;
	for (i=0;i<dIndex;i++) head=head->next;
	return head;
}

sIniLine* IniFile::GetLineByIndex(sIniSection* head,tDWORD dIndex,sIniLine** prev)
{
	tDWORD i;
	sIniLine* line=head->line_head;
	if (prev) *prev=0;
	for (i=0;i<dIndex;i++) 
	{
		if (prev) *prev=line;
		line=line->next;
	}
	return line;
}
