/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file common.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Общие типы
 */

class xsd__anyType {
public:
	virtual int getType();
}; 

class xsd__anySimpleType: public xsd__anyType {
public:
	virtual int getType();
}; 

typedef char *xsd__anyURI; 
class xsd__anyURI_: public xsd__anySimpleType {
public:	
	xsd__anyURI __item;
	virtual int getType();
}; 

typedef bool xsd__boolean; 
class xsd__boolean_: public xsd__anySimpleType {
public:
	xsd__boolean __item;
	virtual int getType();
}; 

typedef char *xsd__date; 
class xsd__date_: public xsd__anySimpleType {
public:
	xsd__date __item;
	virtual int getType();
}; 

typedef char* xsd__dateTime; 
class xsd__dateTime_: public xsd__anySimpleType {
public:	
	xsd__dateTime __item;
	virtual int getType();
}; 

typedef double xsd__double; 
class xsd__double_: public xsd__anySimpleType{
public:
	xsd__double __item;
	virtual int getType();
}; 

typedef char *xsd__duration; 
class xsd__duration_: public xsd__anySimpleType {
public:
	xsd__duration __item;
	virtual int getType();
}; 

typedef float xsd__float; 
class xsd__float_: public xsd__anySimpleType {
public:
	xsd__float __item;
	virtual int getType();
}; 

typedef char *xsd__time; 
class xsd__time_: public xsd__anySimpleType {
public:
	xsd__time __item;
	virtual int getType();
}; 

typedef char *xsd__decimal; 
class xsd__decimal_: public xsd__anySimpleType {
public:
	xsd__decimal __item;
	virtual int getType();
}; 

typedef char *xsd__integer; 
class xsd__integer_: public xsd__decimal_ {
public:
	xsd__integer __item;
	virtual int getType();
}; 

typedef LONG64 xsd__long; 
class xsd__long_: public xsd__integer_ {
public:
	xsd__long __item;
	virtual int getType();
}; 

typedef long xsd__int; 
class xsd__int_: public xsd__long_ {
public:
	xsd__int __item;
	virtual int getType();
}; 

typedef short xsd__short; 
class xsd__short_: public xsd__int_ {
public:
	xsd__short __item;
	virtual int getType();
}; 

typedef char xsd__byte; 
class xsd__byte_: public xsd__short_{
public:
	xsd__byte __item;
	virtual int getType();
}; 

typedef char *xsd__nonPositiveInteger; 
class xsd__nonPositiveInteger_: public xsd__integer_ {
public:
	xsd__nonPositiveInteger __item;
	virtual int getType();
}; 

typedef char *xsd__negativeInteger; 
class xsd__negativeInteger_: public xsd__nonPositiveInteger_ {
public:
	xsd__negativeInteger __item;
	virtual int getType();
}; 

typedef char *xsd__nonNegativeInteger; 
class xsd__nonNegativeInteger_: public xsd__integer_ {
public:
	xsd__nonNegativeInteger __item;
	virtual int getType();
}; 

typedef char *xsd__positiveInteger; 
class xsd__positiveInteger_: public xsd__nonNegativeInteger_ {
public:
	xsd__positiveInteger __item;
	virtual int getType();
}; 

typedef ULONG64 xsd__unsignedLong; 
class xsd__unsignedLong_: public xsd__nonNegativeInteger_{
public:
	xsd__unsignedLong __item;
	virtual int getType();
}; 

typedef unsigned long xsd__unsignedInt; 
class xsd__unsignedInt_: public xsd__unsignedLong_ {
public:
	xsd__unsignedInt __item;
	virtual int getType();
}; 

typedef unsigned short xsd__unsignedShort; 
class xsd__unsignedShort_: public xsd__unsignedInt_ {
public:
	xsd__unsignedShort __item;
	virtual int getType();
}; 

typedef unsigned char xsd__unsignedByte; 
class xsd__unsignedByte_: public xsd__unsignedShort_ {
public:
	xsd__unsignedByte __item;
	virtual int getType();
}; 

typedef wchar_t *xsd__wstring; 
class xsd__wstring_: public xsd__anySimpleType {
public:
	xsd__wstring __item;
	virtual int getType();
}; 

typedef char *xsd__string; 
class xsd__string_: public xsd__anySimpleType {
public:
	xsd__string __item;
	virtual int getType();
}; 

typedef char *xsd__normalizedString; 
class xsd__normalizedString_: public xsd__string_ {
public:
	xsd__normalizedString __item;
	virtual int getType();
}; 

typedef char *xsd__token; 
class xsd__token_: public xsd__normalizedString_ {
public:
	xsd__token __item;
	virtual int getType();
}; 

class xsd__base64Binary: public xsd__anySimpleType{
public:
	unsigned char *__ptr;
	int __size;
	virtual int getType();
}; 

class xsd__hexBinary: public xsd__anySimpleType {
public:
	unsigned char *__ptr;
	int __size;
	virtual int getType();
}; 

class SOAPComponentId {
public:
	SOAPComponentId();
	xsd__wstring productName;
	xsd__wstring version;
	xsd__wstring componentName;
	xsd__wstring instanceId;
};

struct SOAPComponentIdsList
{
	SOAPComponentId* 		__ptr;
	int						__size;
};
