#if !defined(_KEYSTREE_H_)
#define _KEYSTREE_H_

#include <nestednodelist.h>

#include <klava/klavsys.h>
#include <klava/klavstl/string.h>
#include <klava/klavstl/vector.h>

#define LAX_NO_STL
#define LAX_NO_IMPLEMENTATION
#include <LAX.h>
#include "value.h"

typedef klavstl::string <klav_allocator> stringbuf;

typedef LAX::UTF8_UTF8 <LAX::STD_STRING_ADAPTER <char, stringbuf> > XmlTransform;

struct XML_Attr
{
        stringbuf m_name;
        stringbuf m_value;

        XML_Attr(hKLAV_Alloc alloc)
                : m_name(alloc), m_value(alloc){}
};

struct XML_Attrs
{
public:
        XML_Attrs(hKLAV_Alloc alloc)
	: m_allocator(alloc), m_attrs(alloc){}

        size_t size() const { return m_attrs.size(); }

        const XML_Attr& operator [](size_t idx) const { return m_attrs[idx]; }

        void clear(){ m_attrs.clear(); }

        const XML_Attr* find(const char *name) const
        {
                size_t i, cnt = m_attrs.size();
                for(i = 0; i < cnt; ++i){
			const XML_Attr& attr = m_attrs[i];
			if(strcmp(attr.m_name.c_str(), name)== 0)
				return &m_attrs[i];
		}
                return 0;
        }       

/*         const char * find_val(const char *name) const */
/*         { */
/*                 int idx = find(name); */
/*                 if(idx < 0) */
/*                         return 0; */
/*                 return m_attrs [idx].m_value.c_str(); */
/*         } */

	int addAttr(const char *name, size_t nlen, const char *value, size_t vlen)
        {
                const size_t idx = m_attrs.size();
                m_attrs.resize(idx+1, XML_Attr(m_allocator));
                XML_Attr& attr = m_attrs[idx];
                LAX::Xml<XmlTransform>::unescape(name, nlen, attr.m_name);
                LAX::Xml<XmlTransform>::unescape(value, vlen, attr.m_value);
                return LAX::XML_OK;
        }

	private:
        hKLAV_Alloc m_allocator;
        klavstl::trivial_vector <XML_Attr, klav_allocator> m_attrs;
};

typedef LAX::XmlReader <XmlTransform, LAX::STD_ATTR_MAP_ADAPTER<char, XML_Attrs>, LAX::XmlNoExceptions> XmlReader;
typedef XmlReader::Element XmlElement;
typedef LAX::XmlWriter <XmlTransform> XmlWriter; 


class KeyBranch {
public:
	KeyBranch(cObject* anOwner, 
		  const char* aName = 0);

	~KeyBranch();

	const char* getName() const { return theName; }

	tERROR setName(const char* aName);
  
	KeyBranch* addKey(const char* aName);

	KeyBranch* getKey(const char* aName);
	KeyBranch* getKey(unsigned int anIndex);

	KeyBranch* getNextKey(KeyBranch* aBranch);

	tERROR eraseKey(const char* aName);
	tERROR eraseKey(unsigned int anIndex);

	tERROR addValue(const char* aName, tTYPE_ID aType, const void* aBuffer, unsigned int aSize);
	tERROR addValue(unsigned int anIndex, tTYPE_ID aType, const void* aBuffer, unsigned int aSize);

	iValue* getValue(const char* aName);
	iValue* getValue(unsigned int anIndex);

	const char* getValueName(unsigned int anIndex) const;

	tERROR eraseValue(const char* aName);
	tERROR eraseValue(unsigned int anIndex);

	void clear();

	KeyBranch* checkBranch(const KeyBranch* aCheckedBranch);

	tERROR getFullKey(const KeyBranch* aCheckedBranch,
			  char* aBuffer, 
			  unsigned int aSize, 
			  unsigned int& aFullSize, 
			  char aDelimeter = '\\');

	unsigned int getKeysCount();

	unsigned int getValuesCount();
  
	tERROR readFromXML(XmlReader& aXMLReader);
	tERROR writeToXML(XmlWriter& aXMLWriter, unsigned int anIndent);

private:
	KeyBranch(const KeyBranch&){}

	struct ValueNode {
		char* theName;
		iValue* theValue;
		NESTED_NODE_LIST_ENTRY(ValueNode);
	};
  
	ValueNode* createValue(tTYPE_ID aType);
	tERROR setValue(const char* aName, tTYPE_ID aType, const void* aBuffer, unsigned int aSize);
	tERROR setValue(const char* aName, tTYPE_ID aType, const char* aBuffer);
	KeyBranch* getBranch(const char* aName) const;
	KeyBranch* getBranch(unsigned int anIndex) const;
	void destroyBranch(KeyBranch*);
	void destroyValue(ValueNode* aValue);
	void destroyName(char* aName);
	tERROR setName(const char* aName, char*& aTargetName);

	typedef NestedNodeListNS::NestedNodeList<ValueNode> ValuesList;
	typedef NestedNodeListNS::NestedNodeList<KeyBranch> KeysList;

	cObject* theOwner;
	NESTED_NODE_LIST_ENTRY(KeyBranch);
	char* theName;
	KeysList theKeysList;
	ValuesList theValuesList;
};


class KeysTree {
 public:
	KeysTree();
	~KeysTree();

	tERROR setOwner(cObject* anOwner);

	void* getRoot();

	tERROR setRoot(void* aKey);
	tERROR setRoot(const char* aName, unsigned int aSize = 0);

	KeyBranch* getKey(void* aKey);

	KeyBranch* getNextKey(void* aKey);

	void clear();

	tERROR getFullKey(KeyBranch* aBranch, char* aBuffer, unsigned int aSize, unsigned int& aFullSize, char aDelimeter = '\\');
	tERROR getFullKeyFromRoot(KeyBranch* aBranch, char* aBuffer, unsigned int aSize, unsigned int& aFullSize, char aDelimeter = '\\');

	tERROR readFromXML(char* aBuffer, unsigned int aSize);
	tERROR writeToXML(stringbuf& aBuffer);

	bool isEmpty() const { return theRoot == 0;}
 private:
	typedef NestedNodeListNS::NestedNodeList<KeyBranch> KeysList;

	cObject* theOwner;
	KeyBranch* theRoot;
	KeyBranch* theCurrentRoot;
};

iValue* allocateValue(cObject* anOwner, tTYPE_ID aType);
void destroyValue(cObject* anOwner, iValue* aValue);
const char* getStringType(tTYPE_ID aType);

class KLAV_Pr_Alloc : public KLAV_IFACE_IMPL(KLAV_Alloc)
{
public:
	KLAV_Pr_Alloc (hOBJECT h=0)
		: m_hObject (h) {}

	hOBJECT get_object () const
		{ return m_hObject; }

	void set_object (hOBJECT h)
		{ m_hObject = h; }

	virtual uint8_t * KLAV_CALL alloc (size_t size)
	{
		tPTR ptr = 0;
		tERROR err = CALL_SYS_ObjHeapAlloc (m_hObject, &ptr, (tDWORD)size);
		if (PR_FAIL (err)) return 0;
		return (uint8_t *)ptr;
	}

	virtual uint8_t * KLAV_CALL realloc (void *ptr, size_t newsize)
	{
		tPTR nptr = 0;
		tERROR err = CALL_SYS_ObjHeapRealloc (m_hObject, &nptr, ptr, (tDWORD)newsize);
		if (PR_FAIL (err)) return 0;
		return (uint8_t *)nptr;
	}

	virtual void KLAV_CALL free (void *ptr)
	{
		CALL_SYS_ObjHeapFree (m_hObject, ptr);
	}

private:
	hOBJECT m_hObject;
};

#endif // _KEYSTREE_H_
