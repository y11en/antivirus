#include <new>
#include <klava/klavstl/buffer.h>
#include <LAX.h>
#include "keystree.h"

//
// class KeyBranch
//

const char RootKey[] = "";
const char UnnamedValue[] = "noname";

const char XML_TAG_PROPERTIESMAP_FILE[] = "propertiesmap";
const char XML_TAG_KEY[] = "key";

const char XML_ATTR_TYPE[] = "type";
const char XML_ATTR_NAME[] = "name";

const char* TypeIDTable[] = {
	0,
	"tVOID",
	"tBYTE",
	"tWORD",
	"tDWORD",
	"tQWORD",
	"tBOOL",
	"tCHAR",
	"tWCHAR",
	"tSTRING",
	"tWSTRING",
	"tERROR",
	"tPTR",
	"tINT",
	"tUINT",
	"tSBYTE",
	0,
	"tSHORT",
	"tLONG",
	"tLONGLONG",
	"tIID",
	"tPID",
	"tORIG_ID",
	"tOS_ID",
	"tVID",
	"tPROPID",
	"tVERSION",
	"tCODEPAGE",
	"tLCID",
	"tDATA",
	"tDATETIME",
	"tFUNC_PTR",
	"tDOUBLE",
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	"tBINARY",
	"tIFACEPTR",
	"tOBJECT",
	"tEXPORT",
	"tIMPORT",
	"tTRACE_LEVEL",
	"tTYPE_ID",
	"tVECTOR",
	"tSTRING_OBJ", 
	"tSERIALIZABLE",
	"tBUFFER", 
	"tSTR_DSC",
	"tSIZE_T",
	"tIMPL_ID",
	"tMSG_ID",
	"tMSGCLS_ID",
};

KeyBranch::KeyBranch(cObject* anOwner,  
		     const char* aName)
	: theOwner(anOwner),
	  theName(0)
{
	setName(aName, theName);
}

KeyBranch::~KeyBranch()
{
	clear();
	destroyName(theName);
}

tERROR KeyBranch::setName(const char* aName )
{
	if(!aName)
		return errPARAMETER_INVALID;
	return setName (aName, theName);
}

KeyBranch* KeyBranch::addKey(const char* aName)
{
	if(!theOwner)
		return 0;
	void* aPointer = 0;
	if(PR_FAIL(theOwner->heapAlloc(&aPointer, sizeof(KeyBranch))))
		return 0;
	KeyBranch* aKey = new(aPointer)KeyBranch(theOwner, aName);
	if(!aKey)
		return 0;
	theKeysList.push_back(*aKey);
	return aKey;
}

KeyBranch* KeyBranch::getKey(const char* aName)
{
	if(!*aName)
		return this;
	const char* aPosition = strchr(aName, '/');
	if(!aPosition)
		aPosition = strchr(aName, '\\');
	unsigned int aSize = 0;
	if(aPosition)
		aSize = aPosition - aName;
	else
		aSize = strlen(aName);
	if(aPosition && !aSize)
		return this;
	for(KeysList::iterator anIterator = theKeysList.begin();
	    anIterator != theKeysList.end();
	    ++anIterator)
		if(!_strnicmp(anIterator->getName(), aName, aSize )&& 
		   (strlen(anIterator->getName()) == aSize))
			return anIterator->getKey(aName + aSize + ((aPosition)? 1 : 0 ));
	return 0;
}

KeyBranch* KeyBranch::getKey(unsigned int anIndex)
{
	for(KeysList::iterator anIterator = theKeysList.begin();
	    anIterator != theKeysList.end();
	    ++anIterator)
		if(!anIndex--)
			return &(*anIterator);
	return 0;
}

KeyBranch* KeyBranch::getNextKey(KeyBranch* aBranch)
{
	for(KeysList::iterator anIterator = theKeysList.begin();
	    anIterator != theKeysList.end();
	    ++anIterator)
		if(&(*anIterator)== aBranch)
			return &(*++anIterator);
	return 0;
}

tERROR KeyBranch::eraseKey(const char* aName)
{
	KeyBranch* aBranch = getKey(aName);
	if(!aBranch)
		return errPARAMETER_INVALID;
	destroyBranch(aBranch);
	return errOK;
}

tERROR KeyBranch::eraseKey(unsigned int anIndex)
{
	KeyBranch* aBranch = getKey(anIndex);
	if(!aBranch)
		return errPARAMETER_INVALID;
	destroyBranch(aBranch);
	return errOK;
}

tERROR KeyBranch::addValue(const char* aName, tTYPE_ID aType, const void* aBuffer, unsigned int aSize)
{
	if(!aName)
		return errPARAMETER_INVALID;

	if(getValue(aName))
		return errOBJECT_ALREADY_EXISTS;

	return setValue(aName, aType, aBuffer, aSize);
}

tERROR KeyBranch::addValue(unsigned int anIndex, tTYPE_ID aType, const void* aBuffer, unsigned int aSize)
{
	unsigned int aListSize = theValuesList.size();
	if(aListSize < anIndex)
		return errPARAMETER_INVALID;

	if(aListSize > anIndex)
		return errOBJECT_ALREADY_EXISTS;

	char aPattern [ 100 ] = {0};

	sprintf(aPattern, "%s%d", UnnamedValue, anIndex);

	return  setValue(aPattern, aType, aBuffer, aSize);

}

iValue* KeyBranch::getValue(const char* aName)
{
	for(ValuesList::iterator anIterator = theValuesList.begin();
	    anIterator != theValuesList.end();
	    ++anIterator)
		if(!_stricmp(anIterator->theName, aName))
			return anIterator->theValue;
	return 0;
}

iValue* KeyBranch::getValue(unsigned int anIndex)
{
	for(ValuesList::iterator anIterator = theValuesList.begin();
	    anIterator != theValuesList.end();
	    ++anIterator)
		if(!anIndex--)
			return anIterator->theValue;
	return 0;
}

const char* KeyBranch::getValueName(unsigned int anIndex)const
{
	for(ValuesList::const_iterator anIterator = theValuesList.begin();
	    anIterator != theValuesList.end();
	    ++anIterator)
		if(!anIndex--)
			return anIterator->theName;
	return 0;
}


tERROR KeyBranch::eraseValue(const char* aName)
{
	for(ValuesList::iterator anIterator = theValuesList.begin();
	    anIterator != theValuesList.end();
	    ++anIterator)
		if(!_stricmp(anIterator->theName, aName)){
			destroyValue(&(*anIterator));
			return errOK;
		}
	return errPARAMETER_INVALID;
}

tERROR KeyBranch::eraseValue(unsigned int anIndex)
{
	for(ValuesList::iterator anIterator = theValuesList.begin();
	    anIterator != theValuesList.end();
	    ++anIterator)
		if(!anIndex--){
			destroyValue(&(*anIterator));
			return errOK;
		}
	return errPARAMETER_INVALID;
}

void KeyBranch::clear()
{
	ValuesList::iterator anValuesIterator = theValuesList.begin();
	while(anValuesIterator != theValuesList.end())
		destroyValue(&(*anValuesIterator++));
	KeysList::iterator anKeysIterator = theKeysList.begin();
	while(anKeysIterator != theKeysList.end())
		destroyBranch(&(*anKeysIterator++));
}

KeyBranch* KeyBranch::checkBranch(const KeyBranch* aCheckedBranch)
{
	if(!aCheckedBranch)
		return 0;
	if(aCheckedBranch == this)
		return this;
	for(KeysList::iterator anIterator = theKeysList.begin();
	    anIterator != theKeysList.end();
	    ++anIterator){
		if(&(*anIterator)== aCheckedBranch)
			return this;
		if(KeyBranch* aBranch = anIterator->checkBranch(aCheckedBranch))
			return aBranch;
	}
	return 0;
}

tERROR KeyBranch::getFullKey(const KeyBranch* aCheckedBranch,
			     char* aBuffer, 
			     unsigned int aSize, 
			     unsigned int& aFullSize, 
			     char aDelimeter)
{
	if(!aCheckedBranch)
		return errPARAMETER_INVALID;
	unsigned int aCurrentSize = aFullSize;
	aFullSize += strlen(theName);
	if(aCurrentSize)
		aFullSize += sizeof(aDelimeter);
	bool aContained =(this == aCheckedBranch);
	if(!aContained)
		for(KeysList::iterator anIterator = theKeysList.begin();
		    anIterator != theKeysList.end();
		    ++anIterator)
			if(PR_SUCC(anIterator->getFullKey(aCheckedBranch, aBuffer, aSize, aFullSize, aDelimeter)))
				aContained = true;
	if(aContained){
		if(aFullSize > aSize)
			return errBUFFER_TOO_SMALL;
		if(aBuffer){
			if(aCurrentSize){
				aBuffer += aCurrentSize;
				*aBuffer = aDelimeter;
				aBuffer += sizeof(aDelimeter);
			}
			memcpy(aBuffer, 
			       theName,
			       strlen(theName) + ((this == aCheckedBranch) ? 1 : 0));
		}
    
		return errOK;
	}
	aFullSize = aCurrentSize;
	return errNOT_OK;
}

unsigned int KeyBranch::getKeysCount()
{
	unsigned int aCount = theKeysList.size();
  
	for(KeysList::iterator anIterator = theKeysList.begin(); anIterator != theKeysList.end(); ++anIterator)
		aCount += anIterator->getKeysCount();
	return aCount;
}

unsigned int KeyBranch::getValuesCount()
{
	return theValuesList.size();
}

tERROR KeyBranch::readFromXML(XmlReader& aXMLReader)
{
	KLAV_Pr_Alloc alloc(theOwner);
	while(true){
		XmlElement anElement;
		int aResult = aXMLReader.readElement(anElement, 0, 0);
		if(aResult == LAX::XML_END)
			return errOK;
		if(aResult < 0)
			return errUNEXPECTED;

		if(anElement.compareXmlTag(XML_TAG_KEY)){ 
			XmlReader::AttrMap anAttributesMap(&alloc);
			aResult = anElement.readAttrs(anAttributesMap);
			if(aResult < 0)
				return errUNEXPECTED;
			const XML_Attr* anAttribute = anAttributesMap.find(XML_ATTR_NAME);
			if(!anAttribute)
				return errPARAMETER_INVALID;
			KeyBranch* aBranch = addKey(anAttribute->m_value.c_str());
			if(!aBranch)
				return errNOT_ENOUGH_MEMORY;
			if(anElement.hasBody()){
				tERROR anError = aBranch->readFromXML(aXMLReader);
				if(PR_FAIL(anError))
					return anError;
			}
		}
		else {
			unsigned int anID = 0;
			for(; anID < countof(TypeIDTable); ++anID)
				if(TypeIDTable [ anID ] && anElement.compareXmlTag(TypeIDTable [ anID ]))
					break;
			if(anID >= countof(TypeIDTable))
				return errPARAMETER_INVALID;
			XmlReader::AttrMap anAttributesMap(&alloc);
			aResult = anElement.readAttrs(anAttributesMap);
			if(aResult < 0)
				return errUNEXPECTED;
			const XML_Attr* anAttribute = anAttributesMap.find(XML_ATTR_NAME);
			if(!anAttribute)
				return errPARAMETER_INVALID;
			if(!anElement.hasBody())
				return errUNEXPECTED;
			stringbuf aValue(&alloc);
			aXMLReader.readText(aValue);
			tERROR anError = setValue(anAttribute->m_value.c_str(), static_cast<tTYPE_ID>(anID), aValue.c_str());
			if(PR_FAIL(anError))
				return anError;
		}
		aResult = aXMLReader.endElement(anElement);      
		if(aResult < 0)
			return errUNEXPECTED; 
	}
	return errOK;
}

tERROR KeyBranch::writeToXML(XmlWriter& aXMLWriter, unsigned int anIndent)
{
	for(KeysList::iterator anKeysIterator = theKeysList.begin();
	    anKeysIterator != theKeysList.end();
	    ++anKeysIterator){
		aXMLWriter.indent(anIndent);
		aXMLWriter.beginElement(XML_TAG_KEY);
		aXMLWriter.writeAttribute(XML_ATTR_NAME, anKeysIterator->getName());
		aXMLWriter.beginElementBody();
		aXMLWriter.endLine();
		anKeysIterator->writeToXML(aXMLWriter, anIndent + 4);
		aXMLWriter.indent(anIndent);
		aXMLWriter.endElement(XML_TAG_KEY);
		aXMLWriter.endLine();
	}
	KLAV_Pr_Alloc alloc(theOwner);
	klavstl::buffer<klav_allocator> aBuffer(&alloc);
	for(ValuesList::iterator anValuesIterator = theValuesList.begin();
	    anValuesIterator != theValuesList.end();
	    ++anValuesIterator){
		tDWORD size = 0;
		tERROR anError = anValuesIterator->theValue->getValueAsString(0, 0, &size);
		if(PR_FAIL(anError) && (anError != errBUFFER_TOO_SMALL))
			return anError;
		aBuffer.reserve(size);
		memset(aBuffer.data(), 0, aBuffer.capacity());
		anError = anValuesIterator->theValue->getValueAsString(reinterpret_cast<char*>(aBuffer.data()), size, 0);
		aXMLWriter.indent(anIndent);
 		if(anValuesIterator->theValue->getType() == tid_WSTRING)
 			aXMLWriter.beginElement(TypeIDTable[tid_STRING]);
 		else
			aXMLWriter.beginElement(TypeIDTable[anValuesIterator->theValue->getType()]);
		aXMLWriter.writeAttribute(XML_ATTR_NAME, anValuesIterator->theName);
		aXMLWriter.beginElementBody();
		aXMLWriter.writeText(reinterpret_cast<char*>(aBuffer.data()), strlen(reinterpret_cast<char*>(aBuffer.data())));
 		if(anValuesIterator->theValue->getType() == tid_WSTRING)
 			aXMLWriter.endElement(TypeIDTable[tid_STRING]);
 		else
			aXMLWriter.endElement(TypeIDTable[anValuesIterator->theValue->getType()]);
		aXMLWriter.endLine();
	}
	return errOK;
}


KeyBranch::ValueNode* KeyBranch::createValue(tTYPE_ID aType)
{
	void* aPointer = 0;
	if(PR_FAIL(theOwner->heapAlloc(&aPointer, sizeof(ValueNode))))
		return 0;
	ValueNode* aValueNode = new(aPointer)ValueNode;
	if(!aValueNode)
		return 0;
	aValueNode->theValue = allocateValue(theOwner, aType);
	if(!aValueNode->theValue){
		destroyValue(aValueNode);
		return 0;
	}
	return aValueNode;
}

tERROR KeyBranch::setValue(const char* aName, tTYPE_ID aType, const void* aBuffer, unsigned int aSize)
{
	if(!theOwner)
		return errOBJECT_NOT_INITIALIZED;

	ValueNode* aValueNode = createValue(aType);
	if(!aValueNode)
		return errNOT_ENOUGH_MEMORY;

	tERROR anError = setName(aName, aValueNode->theName);
	if(PR_FAIL(anError)){
		destroyValue(aValueNode);
		return anError;
	}

	anError = aValueNode->theValue->setValue(aBuffer, aSize);
	if(PR_FAIL(anError)){
		destroyValue(aValueNode);
		return anError;
	}

	theValuesList.push_back(*aValueNode);
	return errOK;
}

tERROR KeyBranch::setValue(const char* aName, tTYPE_ID aType, const char* aBuffer)
{
	if(!theOwner)
		return errOBJECT_NOT_INITIALIZED;

	ValueNode* aValueNode = createValue(aType);
	if(!aValueNode)
		return errNOT_ENOUGH_MEMORY;

	tERROR anError = setName(aName, aValueNode->theName);
	if(PR_FAIL(anError)){
		destroyValue(aValueNode);
		return anError;
	}

	anError = aValueNode->theValue->setValueAsString(aBuffer);
	if(PR_FAIL(anError)){
		destroyValue(aValueNode);
		return anError;
	}

	theValuesList.push_back(*aValueNode);
	return errOK;
}

void KeyBranch::destroyBranch(KeyBranch* aBranch)
{
	if(!aBranch)
		return;
	if(!theOwner)
		return;
	aBranch->KeyBranch::~KeyBranch();
	theOwner->heapFree(aBranch);
}

void KeyBranch::destroyValue(ValueNode* aValue)
{
	if(!aValue)
		return;
	if(!theOwner)
		return;
	::destroyValue(theOwner, aValue->theValue);
	destroyName(aValue->theName);
	aValue->ValueNode::~ValueNode();
	theOwner->heapFree(aValue);
}

void KeyBranch::destroyName(char* aName)
{
	if(!aName)
		return;
	if(!theOwner)
		return;
	theOwner->heapFree(aName);
}

tERROR KeyBranch::setName(const char* aName, char*& aTargetName )
{
	if(!aName)
		return errPARAMETER_INVALID;
	if(!theOwner)
		return errOBJECT_NOT_INITIALIZED;
	char* aTmp = 0;
	unsigned int aLen = strlen(aName)+ 1;
	if(PR_FAIL(theOwner->heapAlloc(reinterpret_cast<tPTR*>(&aTmp), aLen)))
		return errNOT_ENOUGH_MEMORY;
	strncpy(aTmp, aName, aLen);
	if(aTargetName)
		destroyName(aTargetName);
	aTargetName = aTmp;
	return errOK;
}


// 
//  class KeysTree
//

KeysTree::KeysTree()
	: theOwner(0),
	  theRoot(0),
	  theCurrentRoot(0)
{}

KeysTree::~KeysTree()
{
	if(!theOwner)
		return;
	theRoot->KeyBranch::~KeyBranch();
	theOwner->heapFree(theRoot);
} 

tERROR KeysTree::setOwner(cObject* anOwner)
{
	if(theOwner)
		return errOBJECT_NOT_INITIALIZED;
	theOwner = anOwner;
	void* aPointer = 0;
	if(PR_FAIL(theOwner->heapAlloc(&aPointer, sizeof(KeyBranch))))
		return errNOT_ENOUGH_MEMORY;
	theRoot = new(aPointer)KeyBranch(theOwner, RootKey);
	theCurrentRoot = theRoot;
	return errOK;
}

void* KeysTree::getRoot()
{
	return theCurrentRoot;
}

tERROR KeysTree::setRoot(void* aKey)
{
	if(!theRoot)
		return errOBJECT_NOT_INITIALIZED;

	if(!aKey){
		theCurrentRoot = theRoot;
		return errOK;
	}
	KeyBranch* aBranch = reinterpret_cast<KeyBranch*>(aKey);
	if(!theRoot->checkBranch(aBranch))
		return errPARAMETER_INVALID;
	theCurrentRoot = aBranch;
	return errOK;
}

tERROR KeysTree::setRoot(const char* aName, unsigned int aSize)
{
	if(!theRoot)
		return errOBJECT_NOT_INITIALIZED;
	if(!aName)
		return errPARAMETER_INVALID;
	if(!*aName){
		theCurrentRoot = theRoot;
		return errOK;
	}
	KeyBranch* aBranch = theRoot->getKey(aName);
	if(aBranch){
		theCurrentRoot = aBranch;
		return errOK;
	}
	return errKEY_NOT_FOUND;
}

KeyBranch* KeysTree::getKey(void* aKey)
{
	if(!theRoot)
		return 0;
	if(!aKey)
		return theCurrentRoot;
	KeyBranch* aBranch = reinterpret_cast <KeyBranch*>(aKey);
	if(theCurrentRoot->checkBranch(aBranch))
		return aBranch;
	return 0;
}

KeyBranch* KeysTree::getNextKey(void* aKey)
{
	if(!theRoot)
		return 0;
	if(!aKey)
		return 0;
	KeyBranch* aBranch = reinterpret_cast <KeyBranch*>(aKey);
	KeyBranch* aPrevBranch = theCurrentRoot->checkBranch(aBranch);
	return(aPrevBranch)? aPrevBranch->getNextKey(aBranch): 0;
}


void KeysTree::clear()
{
	theCurrentRoot->clear();
}


tERROR KeysTree::getFullKey(KeyBranch* aBranch, 
			    char* aBuffer, 
			    unsigned int aSize, 
			    unsigned int& aFullSize , 
			    char aDelimeter)
{
	if(!theRoot)
		return errOBJECT_NOT_INITIALIZED;
	return theCurrentRoot->getFullKey(aBranch, aBuffer, aSize, aFullSize, aDelimeter);
}

tERROR KeysTree::getFullKeyFromRoot(KeyBranch* aBranch, 
				    char* aBuffer, 
				    unsigned int aSize, 
				    unsigned int& aFullSize,  
				    char aDelimeter)
{
	if(!theRoot)
		return errOBJECT_NOT_INITIALIZED;
	return theRoot->getFullKey(aBranch, aBuffer, aSize, aFullSize, aDelimeter);
}

tERROR KeysTree::readFromXML(char* aBuffer, unsigned int aSize)
{
	if(!theRoot)
		return errOBJECT_NOT_INITIALIZED;


	XmlReader aXMLReader(aBuffer, aSize);
  
	XmlElement aRootElement;
  
	int aResult = aXMLReader.readElement(aRootElement, LAX::XML_F_ELT_REQUIRED | LAX::XML_F_CHECK_NAME, XML_TAG_PROPERTIESMAP_FILE);
	if(aResult < 0)
		return errUNEXPECTED;
  
	if(!aRootElement.hasBody())
		return errPARAMETER_INVALID;

	tERROR anError = theRoot->readFromXML(aXMLReader);
	if(PR_FAIL(anError))
		return anError;
  
	aResult = aXMLReader.endElement(aRootElement);
	if(aResult < 0)
		return errUNEXPECTED; 
  
	return errOK;
}

tERROR KeysTree::writeToXML(stringbuf& aBuffer)
{
	if(!theRoot)
		return errOBJECT_NOT_INITIALIZED;
  
	XmlWriter aXMLWriter(aBuffer);
 
	aXMLWriter.beginElement(XML_TAG_PROPERTIESMAP_FILE);
	aXMLWriter.beginElementBody();
	aXMLWriter.endLine();
  
	tERROR anError = theRoot->writeToXML(aXMLWriter, 4);

	aXMLWriter.endElement(XML_TAG_PROPERTIESMAP_FILE);
	aXMLWriter.endLine();

	return anError;
}

#define TYPEID_CASE(type,type_id)\
	case type_id:{\
    		typedef Value<type,type_id> type##_Value;\
	      	void* aPointer = 0;\
	      	if(PR_FAIL(anOwner->heapAlloc(&aPointer, sizeof(type##_Value)))){\
			return 0;\
		}\
	     	return new(aPointer)type##_Value;\
    	}


#define TYPEID_CASE_WITH_OWNER(type,type_id)\
	case type_id:{\
		typedef Value<type,type_id> type##_Value;\
		void* aPointer = 0;\
		if(PR_FAIL(anOwner->heapAlloc(&aPointer, sizeof(type##_Value)))){\
			return 0;\
		}\
		return new(aPointer)type##_Value(anOwner);\
	}

iValue* allocateValue(cObject* anOwner,  tTYPE_ID aType)
{
	switch(aType){
		TYPEID_CASE(tVOID,tid_VOID);
		TYPEID_CASE(tBYTE,tid_BYTE);
		TYPEID_CASE(tWORD,tid_WORD);
		TYPEID_CASE(tDWORD,tid_DWORD);
		TYPEID_CASE(tQWORD,tid_QWORD);
		TYPEID_CASE(bool,tid_BOOL);
		TYPEID_CASE(tCHAR,tid_CHAR);
		TYPEID_CASE(tWCHAR,tid_WCHAR);
		TYPEID_CASE_WITH_OWNER(tSTRING,tid_STRING);
		TYPEID_CASE_WITH_OWNER(tWSTRING,tid_WSTRING);
		TYPEID_CASE(tERROR,tid_ERROR);
		TYPEID_CASE_WITH_OWNER(tPTR,tid_PTR);
		TYPEID_CASE(tINT,tid_INT);
		TYPEID_CASE(tUINT,tid_UINT);
		TYPEID_CASE(tSBYTE,tid_SBYTE);
		TYPEID_CASE(tSHORT,tid_SHORT);
		TYPEID_CASE(tLONG,tid_LONG);
		TYPEID_CASE(tLONGLONG,tid_LONGLONG);
		TYPEID_CASE(tIID,tid_IID);
		TYPEID_CASE(tPID,tid_PID);
		TYPEID_CASE(tORIG_ID,tid_ORIG_ID);
		TYPEID_CASE(tOS_ID,tid_OS_ID);
		TYPEID_CASE(tVID,tid_VID);
		TYPEID_CASE(tPROPID,tid_PROPID);
		TYPEID_CASE(tVERSION,tid_VERSION);
		TYPEID_CASE(tCODEPAGE,tid_CODEPAGE);
		TYPEID_CASE(tLCID,tid_LCID);
		TYPEID_CASE(tDATA,tid_DATA);
		TYPEID_CASE(tDATETIME,tid_DATETIME);
		TYPEID_CASE(tFUNC_PTR,tid_FUNC_PTR);
		TYPEID_CASE(tDOUBLE,tid_DOUBLE);
		TYPEID_CASE_WITH_OWNER(tPTR,tid_BINARY);
		TYPEID_CASE(tDWORD,tid_TYPE_ID);
	default:
		return 0;
	}
}

void destroyValue(cObject* anOwner,  iValue* aValue)
{
	if(!aValue)	
		return;
	aValue->iValue::~iValue();
	anOwner->heapFree(aValue);
}

const char* getStringType(tTYPE_ID aType)
{
	return TypeIDTable[aType];
}
