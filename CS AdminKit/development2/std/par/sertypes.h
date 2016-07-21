/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file sertypes.h
 * \author Андрей Казачков
 * \date 2002
 * \brief 
 */


class param_error_loc_args
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};

class param_error_loc
{
public:
    xsd__int                format_id;
    xsd__wstring            format;
    xsd__wstring            locmodule;
    param_error_loc_args    args;
};

//struct param_error;

struct param_error{
	xsd__int            code;
	xsd__wstring        module;
	xsd__string         file;
	xsd__int            line;
	xsd__wstring        message;
    param_error_loc*    locdata;
    //param_error*        prev;
};

typedef xsd__boolean param__null;


//Value
class param__value{
public:
	xsd__anyType* data;
};

class param__entry{
public:
	xsd__wstring	name;
	param__value	value;
};

//Params
class param__node{
public:
	param__entry*	    __ptr;
	int				    __size;
};

//ParamsValue
class param__params : public xsd__anyType{
public:
	param__node *node;
	virtual int getType();
    xsd__int            checksum;
    xsd__base64Binary   binfmt;
};

//ArrayValue
class param__arrayvalue: public xsd__anyType{
public:
	param__value*	__ptr;
	int				__size;
	virtual int getType();
};
