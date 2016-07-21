#include "precompiled.h"
#include "strconv.hpp"
#include "datatree.h"

namespace KasperskyLicensing{
namespace Implementation{

//-----------------------------------------------------------------------------

data_tree::kldt_library::kldt_library()
{
	KLDT_Init_Library(malloc, free);
}

//-----------------------------------------------------------------------------

data_tree::kldt_library::~kldt_library()
{
	KLDT_Deinit_Library(false);
}

//-----------------------------------------------------------------------------

data_tree::data_tree(const void* data, size_t size) : root(0)
{
	if(!KLDT_DeserializeFromMemoryUsingSWM((char*)data, size, &root))
		throw bad_data_format();
}

//-----------------------------------------------------------------------------

data_tree::node data_tree::get_root()
{
	return node(root);
}

//-----------------------------------------------------------------------------

data_tree::~data_tree()
{
	DATA_Remove(root, 0);
}

//-----------------------------------------------------------------------------

data_tree::kldt_library data_tree::libkldt;

//-----------------------------------------------------------------------------

class child_enumerator_imp : public data_tree::node::child_enumerator::Implement
{
public:
	explicit child_enumerator_imp(HDATA handle);
	virtual data_tree::node::child_enumerator::Implement* Clone() const;
	virtual data_tree::node::child_enumerator::ValueType Item() const;
	virtual bool IsDone() const;
	virtual void Next();
private:
	HDATA hdata;
};

//-----------------------------------------------------------------------------

child_enumerator_imp::child_enumerator_imp(HDATA handle) : hdata(handle)
{
}

//-----------------------------------------------------------------------------

data_tree::node::child_enumerator::Implement* child_enumerator_imp::Clone() const
{
	return new child_enumerator_imp(hdata);
}

//-----------------------------------------------------------------------------

data_tree::node::child_enumerator::ValueType child_enumerator_imp::Item() const
{
	return data_tree::node(hdata);
}

//-----------------------------------------------------------------------------

bool child_enumerator_imp::IsDone() const
{
	return (!hdata);
}

//-----------------------------------------------------------------------------

void child_enumerator_imp::Next()
{
	hdata = DATA_Get_Next(hdata, 0);
}

//-----------------------------------------------------------------------------

data_tree::node::node(HDATA handle) : hdata(handle)
{
}

//-----------------------------------------------------------------------------

data_tree::node data_tree::node::get_child(unsigned id) const
{
	MAKE_ADDR1(addr, id);
	HDATA child = DATA_Find(hdata, addr);
	if (!child)
		throw node_not_found();
	return data_tree::node(child);
}

//-----------------------------------------------------------------------------

unsigned data_tree::node::id() const
{
	return DATA_Get_Id(hdata, 0);
}

//-----------------------------------------------------------------------------

data_tree::value data_tree::node::get_value() const
{
	return value(hdata);
}

//-----------------------------------------------------------------------------

data_tree::value::value(HDATA handle) : hdata(handle)
{
}

//-----------------------------------------------------------------------------

AVP_TYPE data_tree::value::type() const
{
	return DATA_Get_Type(hdata, 0, 0);
}

//-----------------------------------------------------------------------------

unsigned data_tree::value::as_uint() const
{
	if (type() != avpt_dword)
		throw bad_value_cast();

	AVP_dword i = 0;

	if (!DATA_Get_Val(hdata, 0, 0, &i, sizeof(i)))
		throw bad_data_size();

	return i;
}

//-----------------------------------------------------------------------------

time_t data_tree::value::as_date() const
{
	if (type() != avpt_date)
		throw bad_value_cast();

	union
	{
		AVP_date date;
		struct
		{
			unsigned short year;
			unsigned short month;
			unsigned short wday;
			unsigned short mday;
		} parts;
	} d = { 0 };

	if (!DATA_Get_Val(hdata, 0, 0, &d.date, sizeof(d.date)))
		throw bad_data_size();

	tm date = { 0 };
	date.tm_year	= d.parts.year - 1900;
	date.tm_mon		= d.parts.month - 1;
	date.tm_mday	= d.parts.mday;
	date.tm_wday	= d.parts.wday;
	date.tm_isdst	= -1;

	return mktime(&date);
}

//-----------------------------------------------------------------------------

void data_tree::value::as_string(std::basic_string<char_t>& str) const
{
	if (type() != avpt_str)
		throw bad_value_cast();

	AVP_dword size = DATA_Get_Val(hdata, 0, 0, 0, 0);

	if (!size)
		throw bad_data_size();

	std::vector<char> buf(size, 0);

	if (!DATA_Get_Val(hdata, 0, 0, &buf[0], buf.size()))
		throw bad_data_size();

	str.assign(conv::string_conv<char_t>(buf.begin(), buf.end()));
}

//-----------------------------------------------------------------------------

size_t data_tree::value::as_binary(void* data, size_t size) const
{
	// data size is requested
	if (!data)
		return DATA_Get_Val(hdata, 0, 0, 0, 0);

	// query value
	return DATA_Get_Val(hdata, 0, 0, data, size);
}

//-----------------------------------------------------------------------------

data_tree::node::child_enumerator data_tree::node::get_children() const
{
	return new child_enumerator_imp(DATA_Get_First(hdata, 0));
}

//-----------------------------------------------------------------------------


} // namespace Implementation
} // namespace KasperskyLicensing
