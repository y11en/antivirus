// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  18 October 2004,  15:16 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Properties map
// Author      -- Sychev
// File Name   -- registry.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version, )
#define Registry_VERSION ((tVERSION)1)
// AVP Prague stamp end


// AVP Prague stamp begin( Header includes, )
#include <prague.h>
#include <plugin/p_propertiesmap.h>
#include <iface/i_root.h>
#include "keystree.h"
#include <iface/i_io.h>
#include <plugin/p_filemapping.h>
#include <plugin/p_win32_nfio.h>
#include <value_utils.h>
#include <iface/i_buffer.h>
#include <pr_cpp.h>
// AVP Prague stamp end

#define IMPEX_IMPORT_LIB
#include <plugin/p_win32_nfio.h>

// AVP Prague stamp begin(C++ class declaration, )
struct pr_novtable PropertiesMap : public cRegistry /*cObjImpl*/ 
{
private:
	// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectClose();

	// Property function declarations
	tERROR pr_call getRootPoint(tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size);
	tERROR pr_call setRootPoint(tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size);
	tERROR pr_call getRootKey(tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size);
	tERROR pr_call setRootKey(tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size);
	tERROR pr_call getName(tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size);
	tERROR pr_call setName(tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size);

public:
	// External function declarations
	tERROR pr_call OpenKey(tRegKey* result, tRegKey p_key, const tCHAR* p_szSubKeyName, tBOOL p_bCreateIfNotExist);
	tERROR pr_call OpenKeyByIndex(tRegKey* result, tRegKey p_key, tDWORD p_index, tBOOL p_bClosePrev);
	tERROR pr_call OpenNextKey(tRegKey* result, tRegKey p_key, tBOOL p_bClosePrev);
	tERROR pr_call CloseKey(tRegKey p_key);
	tERROR pr_call GetKeyName(tDWORD* result, tRegKey p_key, tCHAR* p_name_buff, tDWORD p_size, tBOOL p_bFullName);
	tERROR pr_call GetKeyNameByIndex(tDWORD* result, tRegKey p_key, tDWORD p_index, tCHAR* p_name_buff, tDWORD p_size, tBOOL p_bFullName);
	tERROR pr_call GetValue(tDWORD* result, tRegKey p_key, const tCHAR* p_szValueName, tTYPE_ID* p_type, tPTR p_pValue, tDWORD p_size);
	tERROR pr_call GetValueByIndex(tDWORD* result, tRegKey p_key, tDWORD p_index, tTYPE_ID* p_type, tPTR p_pValue, tDWORD p_size);
	tERROR pr_call GetValueNameByIndex(tDWORD* result, tRegKey p_key, tDWORD p_index, tCHAR* p_name_buff, tDWORD p_size);
	tERROR pr_call SetValue(tRegKey p_key, const tCHAR* p_szValueName, tTYPE_ID p_type, tPTR p_pValue, tDWORD p_size, tBOOL p_bCreateIfNotExist);
	tERROR pr_call SetValueByIndex(tRegKey p_key, tDWORD p_index, tTYPE_ID p_type, tPTR p_pValue, tDWORD p_size, tBOOL p_bCreateIfNotExist);
	tERROR pr_call GetKeyCount(tDWORD* result, tRegKey p_key);
	tERROR pr_call GetValueCount(tDWORD* result, tRegKey p_key);
	tERROR pr_call DeleteKey(tRegKey p_key, const tCHAR* p_szSubKeyName);
	tERROR pr_call DeleteKeyByInd(tRegKey p_key, tDWORD p_index);
	tERROR pr_call DeleteValue(tRegKey p_key, const tCHAR* p_szValueName);
	tERROR pr_call DeleteValueByInd(tRegKey p_key, tDWORD p_index);
	tERROR pr_call Clean();
	tERROR pr_call CopyKey(tRegKey p_src_key, hREGISTRY p_destination, tRegKey p_dest_key);
	tERROR pr_call CopyKeyByName(const tCHAR* p_src, hREGISTRY p_dst);
	tERROR pr_call SetRootKey(tRegKey p_root, tBOOL p_close_prev);
	tERROR pr_call SetRootStr(const tCHAR* p_root, tBOOL p_close_prev);
	tERROR pr_call Flush(tBOOL force);

private:
	tERROR get(iValue* aValue, tDWORD* result, tTYPE_ID* p_type, tPTR p_pValue, tDWORD p_size);
	tERROR set(iValue* aValue, tTYPE_ID p_type, tPTR p_pValue, tDWORD p_size);

	// Data declaration
	KeysTree m_KeysTree;         // --
	tBOOL    m_ReadOnly;         // --
	tDWORD   m_MaxNameLen;       // --
	tDWORD   m_Clean;            // --
	tBOOL    m_SaveResultsOnClose; // --
	tSTRING  m_FileName;         // --
	tSTRING  m_RootPoint;         // --
	// AVP Prague stamp end



	// AVP Prague stamp begin(C++ class declaration end, )
public:
	mDECLARE_INITIALIZATION(PropertiesMap)
		};
// AVP Prague stamp end



// AVP Prague stamp begin(Global property variable declaration, )
// Interface "Registry". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin ( Internal (kernel called) interface method implementation, ObjectInitDone)
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR PropertiesMap::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter Registry::ObjectInitDone method");

	// Place your code here
	m_KeysTree.setOwner(*this);
	if(m_FileName){
		hBUFFER aBuffer;
		error = this->sysCreateObject(reinterpret_cast<hOBJECT*>(&aBuffer), IID_BUFFER, PID_FILEMAPPING);
		if(PR_SUCC(error))
			error = aBuffer->propSetStr(0, pgOBJECT_FULL_NAME, m_FileName, 0, cCP_ANSI);
		if(PR_SUCC(error))
			error = aBuffer->propSetDWord(pgOBJECT_ACCESS_MODE, fACCESS_READ);
 
		if(PR_SUCC(error))
			error = aBuffer->sysCreateObjectDone();
		tQWORD aMappedSize = 0;    
		if(PR_SUCC(error))
			error = aBuffer->GetSize(&aMappedSize, IO_SIZE_TYPE_EXPLICIT);
		tCHAR* aMappedBlock = 0;
		if(PR_SUCC(error))
			error = aBuffer->Lock(reinterpret_cast<tPTR*>(&aMappedBlock));
		if(PR_SUCC(error)){
			error = m_KeysTree.readFromXML(aMappedBlock, static_cast<unsigned int>(aMappedSize));
			aBuffer->Unlock();
		}
		if(PR_SUCC(error)&& m_RootPoint)
			error = m_KeysTree.setRoot(m_RootPoint, strlen(m_RootPoint));
		if(PR_SUCC(error)&& m_RootPoint){
			heapFree(m_RootPoint);
			m_RootPoint = 0;
		}
		if(aBuffer)
			aBuffer->sysCloseObject();
	}
	PR_TRACE_A1(this, "Leave Registry::ObjectInitDone method, ret %terr", error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(Internal(kernel called)interface method implementation, ObjectClose)
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR PropertiesMap::ObjectClose()
{

	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter Registry::ObjectClose method");

	// Place your code here
	if(m_SaveResultsOnClose){
		if(!m_FileName)
			error = errPARAMETER_INVALID;
		cStringObj aBackup(m_FileName);
		aBackup += ".tmp";
		hIO anIO = 0;
		if(PR_SUCC(error))
			error = this->sysCreateObject(reinterpret_cast<hOBJECT*>(&anIO), IID_IO, PID_WIN32_NFIO, SUBTYPE_ANY);

		if(PR_SUCC(error))
			error = anIO->propSetStr(0, pgOBJECT_FULL_NAME, aBackup.c_str(cCP_ANSI), aBackup.size()+ 1);

		if(PR_SUCC(error))
			error = anIO->set_pgOBJECT_OPEN_MODE(fOMODE_CREATE_ALWAYS);

		if(PR_SUCC(error))
			error = anIO->set_pgOBJECT_ACCESS_MODE(fACCESS_WRITE);

		if(PR_SUCC(error))
			error = anIO->sysCreateObjectDone();
    
		KLAV_Pr_Alloc alloc(*this);
		stringbuf aBuffer(&alloc);
		if(PR_SUCC(error))
			error = m_KeysTree.writeToXML(aBuffer);
    
		if(PR_SUCC(error)){
			const void* tmp =  reinterpret_cast<const void*>(aBuffer.c_str());
			error = anIO->SeekWrite(0, 0, const_cast<void*>(tmp), aBuffer.size());
		}

		if(anIO)
			anIO->sysCloseObject();
		if(PR_SUCC(error))
			error = PrMoveFileEx(cAutoString(aBackup), cAutoString(cStringObj(m_FileName)), fPR_FILE_REPLACE_EXISTING);
	}

	PR_TRACE_A1(this, "Leave Registry::ObjectClose method, ret %terr", error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(Property method implementation, getRootPoint)
// Interface "Registry". Method "Get" for property(s):
//  -- ROOT_POINT
tERROR PropertiesMap::getRootPoint(tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size)
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter *GET* method ObjectClose for property pgROOT_POINT");

	*out_size = 0;
	KeyBranch* aRootKey = reinterpret_cast<KeyBranch*>(m_KeysTree.getRoot());
	tDWORD aLength = 0;
	error = m_KeysTree.getFullKeyFromRoot(aRootKey, buffer, size, aLength);
	if(out_size)
		*out_size = aLength;
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A2(this, "Leave *GET* method ObjectClose for property pgROOT_POINT, ret tDWORD = %u(size), %terr", *out_size, error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(Property method implementation, setRootPoint)
// Interface "Registry". Method "Set" for property(s):
//  -- ROOT_POINT
tERROR PropertiesMap::setRootPoint(tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size)
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter *SET* method ObjectClose for property pgROOT_POINT");
  
	*out_size = 0;
	if(!buffer)
		error = errPARAMETER_INVALID;

	if(PR_SUCC(error))
		if(!m_KeysTree.isEmpty()){
			if(PR_SUCC(error)){
				error = m_KeysTree.setRoot(buffer, size);
				PR_TRACE((this, prtNOT_IMPORTANT,"Setting root to %s: %terr", buffer, error));
			}
		}
		else
			error = setValue(buffer, size, m_RootPoint, *this);
  
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A2(this, "Leave *SET* method ObjectClose for property pgROOT_POINT, ret tDWORD = %u(size), %terr", *out_size, error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(Property method implementation, getRootKey)
// Interface "Registry". Method "Get" for property(s):
//  -- ROOT_KEY
tERROR PropertiesMap::getRootKey(tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size)
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter *GET* method ObjectClose for property pgROOT_KEY");

	*out_size = 0;
	if(!m_KeysTree.isEmpty())
		error = getValue(buffer, size, out_size, m_KeysTree.getRoot());
	else
		error = getValue(buffer, size, out_size, m_RootPoint);
	PR_TRACE((this, prtNOT_IMPORTANT,"%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A2(this, "Leave *GET* method ObjectClose for property pgROOT_KEY, ret tDWORD = %u(size), %terr", *out_size, error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(Property method implementation, setRootKey)
// Interface "Registry". Method "Set" for property(s):
//  -- ROOT_KEY
tERROR PropertiesMap::setRootKey(tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size)
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter *SET* method ObjectClose for property pgROOT_KEY");

	*out_size = 0;
	if(!buffer || size < sizeof(tRegKey))
		error = errPARAMETER_INVALID;

	if(PR_SUCC(error)){
		tRegKey aKey = *reinterpret_cast<tRegKey*>(buffer);
		error  = m_KeysTree.setRoot(aKey);
	}
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A2(this, "Leave *SET* method ObjectClose for property pgROOT_KEY, ret tDWORD = %u(size), %terr", *out_size, error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(Property method implementation, getName)
// Interface "Registry". Method "Get" for property(s):
//  -- OBJECT_NAME
tERROR PropertiesMap::getName(tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size)
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter *GET* method getName for property pgOBJECT_NAME");

	*out_size = 0;
	error = getValue(buffer, size, out_size, m_FileName);
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A2(this, "Leave *GET* method getName for property pgOBJECT_NAME, ret tDWORD = %u(size), %terr", *out_size, error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(Property method implementation, setName)
// Interface "Registry". Method "Set" for property(s):
//  -- OBJECT_NAME
tERROR PropertiesMap::setName(tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size)
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter *SET* method setName for property pgOBJECT_NAME");

	*out_size = 0;
	error = setValue(buffer, size, m_FileName, *this);
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A2(this, "Leave *SET* method setName for property pgOBJECT_NAME, ret tDWORD = %u(size), %terr", *out_size, error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, OpenKey)
// Extended method comment
//    if parametr bCreateIfNotExist is cTRUE and key was really created method must return errKEY_NOT_EXISTS warning
// Parameters are:
tERROR PropertiesMap::OpenKey(tRegKey* result, tRegKey p_key, const tCHAR* p_szSubKeyName, tBOOL p_bCreateIfNotExist)
{
	tERROR  error = errOK;
	tRegKey ret_val = cRegNothing;
	PR_TRACE_A0(this, "Enter Registry::OpenKey method");

	// Place your code here
	KeyBranch* aBranch = m_KeysTree.getKey(p_key);
	if(!aBranch)
		error = errKEY_INDEX_OUT_OF_RANGE;

	KeyBranch* aTargetBranch = 0;
	if( PR_SUCC(error)&& p_szSubKeyName){
		aTargetBranch = aBranch->getKey(p_szSubKeyName);
    
		if(!aTargetBranch){
			error = errKEY_NOT_FOUND;
			if(p_bCreateIfNotExist){
				if(m_ReadOnly)
					error = errOBJECT_READ_ONLY;
				else {
					error = warnKEY_NOT_EXISTS;
					if((aTargetBranch = aBranch->addKey(p_szSubKeyName)) == 0)
						error = errNOT_ENOUGH_MEMORY;
				}
			}
		}
		if(PR_SUCC(error))
			ret_val = reinterpret_cast<tRegKey>(aTargetBranch);
	}
	else
		if(PR_SUCC(error))
			ret_val = reinterpret_cast<tRegKey>(aBranch);
	if(result)
		*result = ret_val;
#if defined(_PRAGUE_TRACE_)
	tCHAR aName [ 1000 ] = {0};
	tDWORD aSize = 0;
	if(aTargetBranch && PR_SUCC(m_KeysTree.getFullKeyFromRoot(aTargetBranch, aName, sizeof(aName), aSize)))
		PR_TRACE((this, prtNOT_IMPORTANT,"prmap\tOpening key %s: %terr",  aName, error));
	else
		PR_TRACE((this, prtNOT_IMPORTANT,"prmap\tOpening key %s\\%s: %terr",   "?", p_szSubKeyName, error));
#endif
	PR_TRACE_A2(this, "Leave Registry::OpenKey method, ret tRegKey = %p, %terr", ret_val, error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, OpenKeyByIndex)
// Extended method comment
//    if parametr bCreateIfNotExist is cTRUE and key was really created method must return errKEY_NOT_EXISTS warning
// Parameters are:
tERROR PropertiesMap::OpenKeyByIndex(tRegKey* result, tRegKey p_key, tDWORD p_index, tBOOL p_bClosePrev)
{
	tERROR  error = errOK;
	tRegKey ret_val = cRegNothing;
	PR_TRACE_A0(this, "Enter Registry::OpenKeyByIndex method");

	// Place your code here
	KeyBranch* aBranch = m_KeysTree.getKey(p_key);
	if(!aBranch)
		error = errKEY_INDEX_OUT_OF_RANGE;


	if(PR_SUCC(error))
		aBranch = aBranch->getKey(p_index);

	if(aBranch)
		ret_val = reinterpret_cast<tRegKey>(aBranch);
	else
		error = errKEY_NOT_FOUND;

	if(result)
		*result = ret_val;
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A2(this, "Leave Registry::OpenKeyByIndex method, ret tRegKey = %p, %terr", ret_val, error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, OpenNextKey)
// Parameters are:
tERROR PropertiesMap::OpenNextKey(tRegKey* result, tRegKey p_key, tBOOL p_bClosePrev)
{
	tERROR  error = errOK;
	tRegKey ret_val = cRegNothing;
	PR_TRACE_A0(this, "Enter Registry::OpenNextKey method");

	// Place your code here
	KeyBranch* aBranch = m_KeysTree.getNextKey(p_key);
	if(!aBranch)
		error = errKEY_INDEX_OUT_OF_RANGE;
	else
		ret_val = reinterpret_cast<tRegKey>(aBranch);

	if(result)
		*result = ret_val;
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A2(this, "Leave Registry::OpenNextKey method, ret tRegKey = %p, %terr", ret_val, error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, CloseKey)
// Parameters are:
tERROR PropertiesMap::CloseKey(tRegKey p_key)
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter Registry::CloseKey method");

	// Place your code here
#if defined(_PRAGUE_TRACE_)
	KeyBranch* aBranch = m_KeysTree.getKey(p_key);
	tCHAR aName [ 1000 ] = {0};
	tDWORD aSize = 0;
	if(aBranch && PR_SUCC(m_KeysTree.getFullKeyFromRoot(aBranch, aName, sizeof(aName), aSize)))
		PR_TRACE((this, prtNOT_IMPORTANT,"prmap\tClosing key %s", aName));
	else
		PR_TRACE((this, prtNOT_IMPORTANT,"prmap\tClosing key %x: %terr", p_key, error));
#endif
	PR_TRACE_A1(this, "Leave Registry::CloseKey method, ret %terr", error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, GetKeyName)
// Parameters are:
//   "name_buff" : if this parametr is NULL method must returns length of the buffer to returns name
tERROR PropertiesMap::GetKeyName(tDWORD* result, tRegKey p_key, tCHAR* p_name_buff, tDWORD p_size, tBOOL p_bFullName)
{
	tERROR error = errOK;
	tDWORD ret_val = 0;
	PR_TRACE_A0(this, "Enter Registry::GetKeyName method");

	// Place your code here
	KeyBranch* aBranch = m_KeysTree.getKey(p_key);
	if(!aBranch)
		error = errKEY_INDEX_OUT_OF_RANGE;
	else{
		if(p_bFullName){
			error =  m_KeysTree.getFullKey(aBranch,
							 p_name_buff,
							 p_size,
							 ret_val);
			if(result)
				*result = ret_val;
		}
		else
			error = getValue(p_name_buff, p_size, result, aBranch->getName());
	}
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\tGetting key name %d: %terr", p_key, error));
	PR_TRACE_A2(this, "Leave Registry::GetKeyName method, ret tDWORD = %u, %terr", ret_val, error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, GetKeyNameByIndex)
// Parameters are:
//   "name_buff" : if this parametr is NULL method must returns length of the buffer to returns name
tERROR PropertiesMap::GetKeyNameByIndex(tDWORD* result, tRegKey p_key, tDWORD p_index, tCHAR* p_name_buff, tDWORD p_size, tBOOL p_bFullName)
{
	tERROR error = errOK;
	tDWORD ret_val = 0;

	PR_TRACE_A0(this, "Enter Registry::GetKeyNameByIndex method");

	// Place your code here
	KeyBranch* aBranch = m_KeysTree.getKey(p_key);
	if(!aBranch)
		error = errKEY_INDEX_OUT_OF_RANGE;

	if(PR_SUCC(error))
		aBranch = aBranch->getKey(p_index);

	if(!aBranch)
		error = errKEY_NOT_FOUND;
	else {
		if(p_bFullName){
			error = m_KeysTree.getFullKey(aBranch,
							p_name_buff,
							p_size,
							ret_val );
			if(result)
				*result = ret_val;
		}
		else
			error = getValue(p_name_buff, p_size, result, aBranch->getName());
	}
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\tGetting key name by index %d: %terr", p_index, error));
	PR_TRACE_A2(this, "Leave Registry::GetKeyNameByIndex method, ret tDWORD = %u, %terr", ret_val, error);
	return error;
}
// AVP Prague stamp end

tERROR PropertiesMap::get(iValue* aValue, tDWORD* result, tTYPE_ID* p_type, tPTR p_pValue, tDWORD p_size)
{
	tERROR error = errOK;
	tDWORD ret_val = 0;

	if(!aValue || !p_type)
		return errPARAMETER_INVALID;

	if(*p_type == pTYPE_NOTHING)
		*p_type = aValue->getType();

	if(*p_type == aValue->getType())
		return aValue->getValue(p_pValue, p_size, result);

	tTYPE_ID type = *p_type;
	
	if(*p_type == tid_STRING_OBJ)
		type = tid_WSTRING;
	
	tDWORD aSize = 0;
	error = aValue->getValueAsString(0, 0, &aSize);
	if(PR_FAIL(error) && (error != errBUFFER_TOO_SMALL))
		return error;

	tCHAR* aBuffer = 0;
	aSize++;
	error = heapAlloc(reinterpret_cast<tVOID**>(&aBuffer), aSize);
	if(PR_FAIL(error))
		return error;

	error = aValue->getValueAsString(aBuffer, aSize, 0);
	if(PR_FAIL(error)){
		heapFree(aBuffer);
		return error;
	}

	iValue* aTmpValue = allocateValue(*this, type);
	if(!aTmpValue){
		heapFree(aBuffer);
		return errNOT_ENOUGH_MEMORY;
	}
	
	// this dirty hack has to be removed when tWSTRING is gone from outer xml files
	if(type == tid_WSTRING){
		StringValue_Base<wchar_t*>* value = static_cast<StringValue_Base<wchar_t*>*>(aTmpValue);
		error = value->setValueAsString2(aBuffer);
	}
	else
		error = aTmpValue->setValueAsString(aBuffer);
	   
	heapFree(aBuffer);

	if(PR_FAIL(error))
		return error;

	if(*p_type != tid_STRING_OBJ){
		aTmpValue->getValue(p_pValue, p_size, result);
		destroyValue(*this, aTmpValue);
		return error;
	}


	error = aTmpValue->getValue(0, 0, &aSize);
	if(PR_FAIL(error) && (error != errBUFFER_TOO_SMALL)){
		destroyValue(*this, aTmpValue);
		return error;
	}
	tWCHAR* aWBuffer = 0;
	aSize += sizeof(tWCHAR);
	error = heapAlloc(reinterpret_cast<tVOID**>(&aWBuffer), aSize);
	if(PR_SUCC(error))
		error = aTmpValue->getValue(aWBuffer, aSize, 0);

	destroyValue(*this, aTmpValue);
	
	if(PR_FAIL(error))
		return error;

	cStringObj* aTemp = reinterpret_cast<cStringObj*>(p_pValue);
	error = aTemp->assign(aWBuffer, cCP_UNICODE, aSize);
	heapFree(aWBuffer);
	if(result)
		*result = ret_val;
	return error;
}

// AVP Prague stamp begin(External(user called)interface method implementation, GetValue)
// Parameters are:
tERROR PropertiesMap::GetValue(tDWORD* result, tRegKey p_key, const tCHAR* p_szValueName, tTYPE_ID* p_type, tPTR p_pValue, tDWORD p_size)
{
	tERROR error = errOK;
	tDWORD ret_val = 0;
	PR_TRACE_A0(this, "Enter Registry::GetValue method");

	// Place your code here
	KeyBranch* aBranch = m_KeysTree.getKey(p_key);
	if(!aBranch)
		error = errKEY_INDEX_OUT_OF_RANGE;

	iValue* aValue = 0;
	if(PR_SUCC(error) && ((aValue = aBranch->getValue(p_szValueName)) == 0))
		error = errKEY_NOT_FOUND;

	if(PR_SUCC(error))
		error = get(aValue, result, p_type, p_pValue, p_size);

#if defined(_PRAGUE_TRACE_)
	tCHAR aName[1000] = {0};
	tDWORD aSize = 0;
	if(aBranch && PR_SUCC(m_KeysTree.getFullKeyFromRoot(aBranch, aName, sizeof(aName), aSize)))
		PR_TRACE((this, prtNOT_IMPORTANT,"prmap\tGetting value %s\\%s: %terr",  aName, p_szValueName, error));
	else
		PR_TRACE((this, prtNOT_IMPORTANT,"prmap\tGetting value %s\\%s: %terr",   "?", p_szValueName, error));
#endif
	PR_TRACE_A2(this, "Leave Registry::GetValue method, ret tDWORD = %u, %terr", ret_val, error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, GetValueByIndex)
// Parameters are:
tERROR PropertiesMap::GetValueByIndex(tDWORD* result, tRegKey p_key, tDWORD p_index, tTYPE_ID* p_type, tPTR p_pValue, tDWORD p_size)
{
	tERROR error = errOK;
	tDWORD ret_val = 0;
	PR_TRACE_A0(this, "Enter Registry::GetValueByIndex method");

	// Place your code here
	KeyBranch* aBranch = m_KeysTree.getKey(p_key);
	if(!aBranch)
		error = errKEY_INDEX_OUT_OF_RANGE;

	iValue* aValue = 0;
	if(PR_SUCC(error) && ((aValue = aBranch->getValue(p_index)) == 0))
		error = errKEY_NOT_FOUND;

	if(PR_SUCC(error))
		error = get(aValue, result, p_type, p_pValue, p_size);

	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\tGetting value by index %d: %terr", p_index, error));
	PR_TRACE_A2(this, "Leave Registry::GetValueByIndex method, ret tDWORD = %u, %terr", ret_val, error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, GetValueNameByIndex)
// Parameters are:
tERROR PropertiesMap::GetValueNameByIndex(tDWORD* result, tRegKey p_key, tDWORD p_index, tCHAR* p_name_buff, tDWORD p_size)
{
	tERROR error = errOK;
	tDWORD ret_val = 0;
	PR_TRACE_A0(this, "Enter Registry::GetValueNameByIndex method");

	// Place your code here
	KeyBranch* aBranch = m_KeysTree.getKey(p_key);
	if(!aBranch)
		error = errKEY_INDEX_OUT_OF_RANGE;

	if(PR_SUCC(error))
		error = getValue(p_name_buff, p_size, &ret_val, aBranch->getValueName(p_index));
	if(result)
		*result = ret_val;
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A2(this, "Leave Registry::GetValueNameByIndex method, ret tDWORD = %u, %terr", ret_val, error);
	return error;
}
// AVP Prague stamp end


tERROR PropertiesMap::set(iValue* aValue, tTYPE_ID p_type, tPTR p_pValue, tDWORD p_size)
{
	if(p_type == aValue->getType())
		return aValue->setValue(p_pValue, p_size);
	if(p_type == tid_STRING_OBJ){
		cStringObj* aTemp = reinterpret_cast<cStringObj*>(p_pValue);
		if(!aTemp)
			return errPARAMETER_INVALID;
		p_type = tid_WSTRING;
		p_pValue = const_cast<wchar_t*>(aTemp->data());
		p_size = aTemp->size() * sizeof(wchar_t);
	}
	
	if(aValue->getType() == tid_WSTRING)
		return aValue->setValue(p_pValue, p_size);

	iValue* aTmpValue = allocateValue(*this, p_type);
	if(!aTmpValue)
		return errNOT_ENOUGH_MEMORY;

	aTmpValue->setValue(p_pValue, p_size);
	
	tDWORD aSize = 0;
	tERROR error = aTmpValue->getValueAsString(0, 0, &aSize);
	if(PR_FAIL(error) && (error != errBUFFER_TOO_SMALL)){
		destroyValue(*this, aTmpValue);
		return error;
	}

	tCHAR* aBuffer = 0;
	aSize++;
	error = heapAlloc(reinterpret_cast<tVOID**>(&aBuffer), aSize);
	if(PR_FAIL(error)){
		destroyValue(*this, aTmpValue);
		return error;
	}

	error = aTmpValue->getValueAsString(aBuffer, aSize, 0);
	if(PR_SUCC(error))
		error = aValue->setValueAsString(aBuffer);
	heapFree(aBuffer);
	destroyValue(*this, aTmpValue);
	return error;
}


// AVP Prague stamp begin(External(user called)interface method implementation, SetValue)
// Parameters are:
tERROR PropertiesMap::SetValue(tRegKey p_key, const tCHAR* p_szValueName, tTYPE_ID p_type, tPTR p_pValue, tDWORD p_size, tBOOL p_bCreateIfNotExist)
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter Registry::SetValue method");

	// Place your code here
	if(m_ReadOnly)
		error = errOBJECT_READ_ONLY;
	else {
		KeyBranch* aBranch = m_KeysTree.getKey(p_key);
		if(!aBranch)
			error = errKEY_INDEX_OUT_OF_RANGE;

		iValue* aValue = 0;
		if(PR_FAIL(error))
			return error;
		aValue = aBranch->getValue(p_szValueName);

		if(!aValue){
			error = errKEY_NOT_FOUND;
			if(p_bCreateIfNotExist)
				error = aBranch->addValue(p_szValueName, p_type, p_pValue, p_size);
		}
		else
			error = set(aValue, p_type, p_pValue, p_size);
	}
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A1(this, "Leave Registry::SetValue method, ret %terr", error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, SetValueByIndex)
// Parameters are:
tERROR PropertiesMap::SetValueByIndex(tRegKey p_key, tDWORD p_index, tTYPE_ID p_type, tPTR p_pValue, tDWORD p_size, tBOOL p_bCreateIfNotExist)
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter Registry::SetValueByIndex method");

	// Place your code here
	if(m_ReadOnly)
		error = errOBJECT_READ_ONLY;
	else {
		KeyBranch* aBranch = m_KeysTree.getKey(p_key);
		if(!aBranch)
			error = errKEY_INDEX_OUT_OF_RANGE;

		iValue* aValue = 0;
		if(PR_SUCC(error)){
			aValue = aBranch->getValue(p_index);

			if(!aValue){
				error = errKEY_NOT_FOUND;
				if(p_bCreateIfNotExist)
					error = aBranch->addValue(p_index, p_type, p_pValue, p_size);
			}
			else
				error = set(aValue, p_type, p_pValue, p_size);
		}
	}
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A1(this, "Leave Registry::SetValueByIndex method, ret %terr", error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, GetKeyCount)
// Parameters are:
tERROR PropertiesMap::GetKeyCount(tDWORD* result, tRegKey p_key)
{
	tERROR error = errOK;
	tDWORD ret_val = 0;
	PR_TRACE_A0(this, "Enter Registry::GetKeyCount method");

	// Place your code here
	KeyBranch* aBranch = m_KeysTree.getKey(p_key);
	if(!aBranch)
		error = errKEY_INDEX_OUT_OF_RANGE;
	else
		ret_val = aBranch->getKeysCount();

	if(result)
		*result = ret_val;
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A2(this, "Leave Registry::GetKeyCount method, ret tDWORD = %u, %terr", ret_val, error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, GetValueCount)
// Parameters are:
tERROR PropertiesMap::GetValueCount(tDWORD* result, tRegKey p_key)
{
	tERROR error = errOK;
	tDWORD ret_val = 0;
	PR_TRACE_A0(this, "Enter Registry::GetValueCount method");

	// Place your code here
	KeyBranch* aBranch = m_KeysTree.getKey(p_key);
	if(!aBranch)
		error = errKEY_INDEX_OUT_OF_RANGE;
	else
		ret_val = aBranch->getValuesCount();

	if(result)
		*result = ret_val;
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A2(this, "Leave Registry::GetValueCount method, ret tDWORD = %u, %terr", ret_val, error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, DeleteKey)
// Parameters are:
tERROR PropertiesMap::DeleteKey(tRegKey p_key, const tCHAR* p_szSubKeyName)
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter Registry::DeleteKey method");

	// Place your code here
	if(m_ReadOnly)
		error = errOBJECT_READ_ONLY;
	else {
		KeyBranch* aBranch = m_KeysTree.getKey(p_key);
		if(!aBranch)
			error = errKEY_INDEX_OUT_OF_RANGE;

		if(PR_SUCC(error))
			error = aBranch->eraseKey(p_szSubKeyName);
	}
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A1(this, "Leave Registry::DeleteKey method, ret %terr", error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, DeleteKeyByInd)
// Parameters are:
tERROR PropertiesMap::DeleteKeyByInd(tRegKey p_key, tDWORD p_index)
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter Registry::DeleteKeyByInd method");

	// Place your code here
	if(m_ReadOnly)
		error = errOBJECT_READ_ONLY;
	else {
		KeyBranch* aBranch = m_KeysTree.getKey(p_key);
		if(!aBranch)
			error = errKEY_INDEX_OUT_OF_RANGE;

		if(PR_SUCC(error))
			error = aBranch->eraseKey(p_index);
	}
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A1(this, "Leave Registry::DeleteKeyByInd method, ret %terr", error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, DeleteValue)
// Parameters are:
tERROR PropertiesMap::DeleteValue(tRegKey p_key, const tCHAR* p_szValueName)
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter Registry::DeleteValue method");

	// Place your code here
	if(m_ReadOnly)
		error = errOBJECT_READ_ONLY;
	else {
		KeyBranch* aBranch = m_KeysTree.getKey(p_key);
		if(!aBranch)
			error = errKEY_INDEX_OUT_OF_RANGE;

		if(PR_SUCC(error))
			error = aBranch->eraseValue(p_szValueName);

	}
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A1(this, "Leave Registry::DeleteValue method, ret %terr", error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, DeleteValueByInd)
// Parameters are:
tERROR PropertiesMap::DeleteValueByInd(tRegKey p_key, tDWORD p_index)
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter Registry::DeleteValueByInd method");

	// Place your code here
	if(m_ReadOnly)
		error = errOBJECT_READ_ONLY;
	else {
		KeyBranch* aBranch = m_KeysTree.getKey(p_key);
		if(!aBranch)
			error = errKEY_INDEX_OUT_OF_RANGE;

		if(PR_SUCC(error))
			error = aBranch->eraseValue(p_index);
	}
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A1(this, "Leave Registry::DeleteValueByInd method, ret %terr", error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, Clean)
// Parameters are:
tERROR PropertiesMap::Clean()
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter Registry::Clean method");

	// Place your code here
	if(m_ReadOnly)
		error = errOBJECT_READ_ONLY;
	else
		m_KeysTree.clear();
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A1(this, "Leave Registry::Clean method, ret %terr", error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, CopyKey)
// Parameters are:
tERROR PropertiesMap::CopyKey(tRegKey p_src_key, hREGISTRY p_destination, tRegKey p_dest_key)
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0(this, "Enter Registry::CopyKey method");

	// Place your code here

	PR_TRACE_A1(this, "Leave Registry::CopyKey method, ret %terr", error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, CopyKeyByName)
// Parameters are:
tERROR PropertiesMap::CopyKeyByName(const tCHAR* p_src, hREGISTRY p_dst)
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0(this, "Enter Registry::CopyKeyByName method");

	// Place your code here

	PR_TRACE_A1(this, "Leave Registry::CopyKeyByName method, ret %terr", error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, SetRootKey)
// Parameters are:
tERROR PropertiesMap::SetRootKey(tRegKey p_root, tBOOL p_close_prev)
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter Registry::SetRootKey method");

	// Place your code here
	error = m_KeysTree.setRoot(p_root);
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A1(this, "Leave Registry::SetRootKey method, ret %terr", error);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin(External(user called)interface method implementation, SetRootStr)
// Parameters are:
tERROR PropertiesMap::SetRootStr(const tCHAR* p_root, tBOOL p_close_prev)
{
	tERROR error = errOK;
	PR_TRACE_A0(this, "Enter Registry::SetRootStr method");

	// Place your code here
	error = m_KeysTree.setRoot(p_root);
	PR_TRACE((this, prtNOT_IMPORTANT,"prmap\t%s : %terr", __FUNCTION__,  error));
	PR_TRACE_A1(this, "Leave Registry::SetRootStr method, ret %terr", error);
	return error;
}
// AVP Prague stamp end

tERROR PropertiesMap::Flush(tBOOL force)
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0(this, "Enter Registry::Flush method");

	// Place your code here

	PR_TRACE_A1(this, "Leave Registry::Flush, ret %terr", error);
	return error;
}


// AVP Prague stamp begin(C++ class regitration, )
// Interface "Registry". Register function
tERROR PropertiesMap::Register(hROOT root)
{
	tERROR error;
	// AVP Prague stamp end



	// AVP Prague stamp begin(Property table, )
	mpPROPERTY_TABLE(Registry_PropTable)
		mSHARED_PROPERTY(pgINTERFACE_VERSION, Registry_VERSION)
		mSHARED_PROPERTY_PTR(pgINTERFACE_COMMENT, "properties map", 15)
		mpLOCAL_PROPERTY_BUF(pgOBJECT_RO, PropertiesMap, m_ReadOnly, cPROP_BUFFER_READ_WRITE)
		mpLOCAL_PROPERTY_BUF(pgMAX_NAME_LEN, PropertiesMap, m_MaxNameLen, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT)
		mpLOCAL_PROPERTY_BUF(pgCLEAN, PropertiesMap, m_Clean, cPROP_BUFFER_READ)
		mpLOCAL_PROPERTY_BUF(pgSAVE_RESULTS_ON_CLOSE, PropertiesMap, m_SaveResultsOnClose, cPROP_BUFFER_READ_WRITE)
		mpLOCAL_PROPERTY_FN(pgROOT_POINT, FN_CUST(getRootPoint), FN_CUST(setRootPoint))
		mpLOCAL_PROPERTY_FN(pgROOT_KEY, FN_CUST(getRootKey), FN_CUST(setRootKey))
		mpLOCAL_PROPERTY_FN(pgOBJECT_NAME, FN_CUST(getName), FN_CUST(setName))
		mpPROPERTY_TABLE_END(Registry_PropTable)
		// AVP Prague stamp end



		// AVP Prague stamp begin(Internal method table, )
		mINTERNAL_TABLE_BEGIN(Registry)
		mINTERNAL_METHOD(ObjectInitDone)
		mINTERNAL_METHOD(ObjectClose)
		mINTERNAL_TABLE_END(Registry)
		// AVP Prague stamp end



		// AVP Prague stamp begin(External method table, )
		mEXTERNAL_TABLE_BEGIN(Registry)
		mEXTERNAL_METHOD(Registry, OpenKey)
		mEXTERNAL_METHOD(Registry, OpenKeyByIndex)
		mEXTERNAL_METHOD(Registry, OpenNextKey)
		mEXTERNAL_METHOD(Registry, CloseKey)
		mEXTERNAL_METHOD(Registry, GetKeyName)
		mEXTERNAL_METHOD(Registry, GetKeyNameByIndex)
		mEXTERNAL_METHOD(Registry, GetValue)
		mEXTERNAL_METHOD(Registry, GetValueByIndex)
		mEXTERNAL_METHOD(Registry, GetValueNameByIndex)
		mEXTERNAL_METHOD(Registry, SetValue)
		mEXTERNAL_METHOD(Registry, SetValueByIndex)
		mEXTERNAL_METHOD(Registry, GetKeyCount)
		mEXTERNAL_METHOD(Registry, GetValueCount)
		mEXTERNAL_METHOD(Registry, DeleteKey)
		mEXTERNAL_METHOD(Registry, DeleteKeyByInd)
		mEXTERNAL_METHOD(Registry, DeleteValue)
		mEXTERNAL_METHOD(Registry, DeleteValueByInd)
		mEXTERNAL_METHOD(Registry, Clean)
		mEXTERNAL_METHOD(Registry, CopyKey)
		mEXTERNAL_METHOD(Registry, CopyKeyByName)
		mEXTERNAL_METHOD(Registry, SetRootKey)
		mEXTERNAL_METHOD(Registry, SetRootStr)
		mEXTERNAL_TABLE_END(Registry)
		// AVP Prague stamp end



		// AVP Prague stamp begin(Registration call, )

		PR_TRACE_A0(root, "Enter Registry::Register method");

	error = CALL_Root_RegisterIFace(
					root,                                   // root object
					IID_REGISTRY,                           // interface identifier
					PID_PROPERTIESMAP,                      // plugin identifier
					0x00000000,                             // subtype identifier
					Registry_VERSION,                       // interface version
					VID_SYCHEV,                             // interface developer
					&i_Registry_vtbl,                       // internal(kernel called)function table
					(sizeof(i_Registry_vtbl)/sizeof(tPTR)), // internal function table size
					&e_Registry_vtbl,                       // external function table
					(sizeof(e_Registry_vtbl)/sizeof(tPTR)), // external function table size
					Registry_PropTable,                     // property table
					mPROPERTY_TABLE_SIZE(Registry_PropTable),// property table size
					sizeof(PropertiesMap)-sizeof(cObjImpl), // memory size
					0                                       // interface flags
					);

#ifdef _PRAGUE_TRACE_
	if(PR_FAIL(error))
		PR_TRACE((root,prtDANGER,"Registry(IID_REGISTRY)registered [%terr]",error));
#endif // _PRAGUE_TRACE_

	PR_TRACE_A1(root, "Leave Registry::Register method, ret %terr", error);
	// AVP Prague stamp end



	// AVP Prague stamp begin(C++ class regitration end, )
	return error;
}

tERROR pr_call Registry_Register(hROOT root){ return PropertiesMap::Register(root); }
// AVP Prague stamp end



