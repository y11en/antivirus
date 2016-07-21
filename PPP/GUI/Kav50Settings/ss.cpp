#include <memory.h>
#if defined (_WIN32)
#include <minmax.h>
#elif defined (__unix__)
#include <unix/compatutils.h>
#endif
#include "ss.h"

#define GET_INTEGRAL_TYPE(data, size, type) (size < sizeof(type) ? 0 : (data+=sizeof(type), size-=sizeof(type), ((type*)data)[-1]) )

static const unsigned long c_dwNullParamsSize=0xFFFFFFFF;

static char* g_sProductPath = NULL;

int (__cdecl *_pfKLCSPWD_UnprotectData)(const void* pProtectedData, size_t nProtectedData, void*& pData, size_t& nData) = NULL;
//int _load_nodes(unsigned char*& data, unsigned int& size, pxnode parent, pxnode* proot, bool array);

bool UnprotectData(void const *p, unsigned int s, void *&p1, unsigned int& s1)
{
	void *new_data = NULL;
	void *new_data_copy = NULL;
	size_t new_size = 0;
	if (!_pfKLCSPWD_UnprotectData)
		return false;
    if (0 != _pfKLCSPWD_UnprotectData(p, s, new_data, new_size))
		return false;
	new_data_copy = xtree_alloc(new_size);
	if (!new_data_copy)
		return false;
	if (p1)
		xtree_free(p1);
	p1 = new_data_copy;
	memcpy(p1, new_data, new_size);
	s1 = new_size;
	return true;
}

#define B2T_LENGTH(scount) ( (scount/3)*4 + ( ((scount % 3)==2) ? 3 : (((scount % 3)==1) ? 2 : 0) ) )
unsigned int BinToText(const void *source, unsigned int scount, void *destination, unsigned int dcount)
/*
 * BinToText procedure
 *
 * convert binary data to printable ASCII text
 *
 * parameters
 *      source - pointer to source buffer contaning data to be converted.
 *      scount - number of bytes to convert to text.
 *      destination - pointer to destination buffer for storing resulting text
 *      dcount - size of destination buffer in bytes. Buffer should be at
 *               least ((scount+2)/3)*4 bytes
 *
 * return values
 *      > 0 - number of bytes successfully written to destination buffer
 *
 *      0 - error, no data was written. Probably causes:
 *        - source or destination points to NULL
 *        - dcount smaller than ((scount+2)/3)*4 bytes
 */
{
    unsigned int rcode = 0;        /* return value of procedure */

    static const unsigned char B2Ttable[64] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', /* 0x00 */
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', /* 0x08 */
        'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', /* 0x10 */
        'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', /* 0x18 */
        'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', /* 0x20 */
        'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', /* 0x28 */
        'm', 'n', 'o', 'p', 'q', 'r', 's', 't', /* 0x30 */
        'u', 'v', 'w', 'x', 'y', 'z', '+', '/', /* 0x38 */
    };

    /* check source and destination pointers */
    if ( source != NULL && destination != NULL && source != destination )
    {
        if ( dcount >= B2T_LENGTH(scount) )
        {
            unsigned int value = 0;         /* temporary buffer for storing bit stream */
            int bits = 0;               /* bit count in temporary buffer */
            int sym;                    /* temporary byte */

            rcode = 0;
			unsigned char* uc_destination = (unsigned char*)destination;
			unsigned char* uc_source = (unsigned char*)source;

            while ( scount || bits > 0 )
            {
                /* get 6 bits from source stream */
                if ( bits < 6 )
                {
                    /* extract next byte from input stream */
                    if ( scount )
                    {
                        sym = *uc_source++;
                        sym <<= bits;
                        value |= sym;
                        bits += 8;
                        scount--;
                    }
                }
                *uc_destination++ = B2Ttable[value & 0x3F];
                rcode++;
                value >>= 6;
                bits -= 6;
            }
        }
    }

    return rcode;
}

pxnode xtree_alloc_node()
{
	pxnode node = (pxnode)xtree_alloc(sizeof(_xnode));
	if (!node)
		return NULL;
	memset(node, 0, sizeof(_xnode));
	node->pdata = &node->data;
	return node;
}

void xtree_free_node(pxnode pnode)
{
	if (!pnode)
		return;
 	if (pnode->first_child)
		xtree_free_node(pnode->first_child);
	if (pnode->next)
		xtree_free_node(pnode->next);
	if (pnode->name)
		xtree_free(pnode->name);
	if (pnode->pdata != &pnode->data)
		xtree_free(pnode->pdata);
}

pxnode xtree_create_node(wchar_t* name, serbinval_t type)
{
	int len;
	pxnode node;
	if (!name)
		return NULL;
	node = xtree_alloc_node();
	if (!node)
		return NULL;
	len = (wcslen(name)+1)*sizeof(wchar_t);
	node->name = (wchar_t*)xtree_alloc(len);
	if (!node->name)
	{
		xtree_free_node(node);
		return NULL;
	}
	memcpy(node->name, name, len);
	node->pdata = &node->data;
	node->type = type;
	return node;
}

bool xtree_deserialize_data(void* pvalue, size_t value_size, void* pdata, size_t data_size, size_t& offset)
{
	if (data_size - offset < value_size)
		return false;
	memcpy(pvalue, ((char*)pdata)+offset, value_size);
	offset += value_size;
	return true;
}

bool xtree_deserialize_node(pxnode parent, pxnode* pnode, void* pdata, size_t data_size, size_t& offset)
{
	int len = 0;
	char _bool;
	pxnode node;

	*pnode = node = xtree_alloc_node();
	if (!node)
		return false;
	node->parent = parent;
	if (!xtree_deserialize_data(&len, 2, pdata, data_size, offset))
		return false;
	node->name = (wchar_t*)xtree_alloc(len);
	if (!node->name)
		return false;
	if (!xtree_deserialize_data(node->name, len, pdata, data_size, offset))
		return false;
	if (!xtree_deserialize_data(&node->type, 1, pdata, data_size, offset))
		return false;
	_bool = 0;
	if (!xtree_deserialize_data(&_bool, 1, pdata, data_size, offset))
		return false;
	if (!xtree_deserialize_data(&node->data_size, sizeof(node->data_size), pdata, data_size, offset))
		return false;
	if (!_bool)
	{
		node->pdata = xtree_alloc(node->data_size);
		if (!node->pdata)
			return false;
	}
	if (!xtree_deserialize_data(node->pdata, node->data_size, pdata, data_size, offset))
		return false;
	_bool = 0;
	if (!xtree_deserialize_data(&_bool, 1, pdata, data_size, offset))
		return false;
	if (_bool)
	{
		if (!xtree_deserialize_node(node, &node->first_child, pdata, data_size, offset))
			return false;
	}
	_bool = 0;
	if (!xtree_deserialize_data(&_bool, 1, pdata, data_size, offset))
		return false;
	if (_bool)
	{
		if (!xtree_deserialize_node(parent, &node->next, pdata, data_size, offset))
			return false;
	}
	return true;
}

bool xtree_deserialize(pxnode* proot, void* pdata, size_t size)
{
	size_t data_size = size;
	size_t offset = 0;
	bool res;
	if (!proot || !pdata || !size)
		return false;
	res = xtree_deserialize_node(0, proot, pdata, data_size, offset);
	if (!res)
		xtree_free_node(*proot);
	return res;
}

bool xtree_serialize_data(void* pvalue, size_t value_size, void*& pdata, size_t& allocated, size_t& data_size)
{
	if (allocated - data_size < value_size)
	{
		allocated+=max((size_t)4*1024, value_size);
		pdata = xtree_realloc(pdata, allocated);
		if (!pdata)
		{
			allocated = 0;
			data_size = 0;
			return false;
		}
	}
	memcpy(((char*)pdata)+data_size, pvalue, value_size);
	data_size += value_size;
	return true;
}

bool xtree_serialize_node(pxnode node, void*& pdata, size_t& allocated, size_t& data_size)
{
	int len = (wcslen(node->name) + 1) * sizeof(wchar_t);
	char _bool;
	if (!xtree_serialize_data(&len, 2, pdata, allocated, data_size))
		return false;
	if (!xtree_serialize_data(node->name, len, pdata, allocated, data_size))
		return false;
	if (!xtree_serialize_data(&node->type, 1, pdata, allocated, data_size))
		return false;
	_bool = (node->pdata == &node->data);
	if (!xtree_serialize_data(&_bool, 1, pdata, allocated, data_size))
		return false;
	if (!xtree_serialize_data(&node->data_size, sizeof(node->data_size), pdata, allocated, data_size))
		return false;
	if (!xtree_serialize_data(node->pdata, node->data_size, pdata, allocated, data_size))
		return false;
	_bool = (node->first_child != 0);
	if (!xtree_serialize_data(&_bool, 1, pdata, allocated, data_size))
		return false;
	if (node->first_child)
	{
		if (!xtree_serialize_node(node->first_child, pdata, allocated, data_size))
			return false;
	}
	_bool = (node->next != 0);
	if (!xtree_serialize_data(&_bool, 1, pdata, allocated, data_size))
		return false;
	if (node->next)
	{
		if (!xtree_serialize_node(node->next, pdata, allocated, data_size))
			return false;
	}
	return true;
}

void xtree_free_serialized(void* pdata)
{
	if (pdata)
		xtree_free(pdata);
}

bool xtree_serialize(pxnode root, void** ppdata, size_t* psize)
{
	void* pdata;
	size_t allocated = 4*1024;
	size_t data_size = 0;
	bool res;
	if (!root || !ppdata || !psize)
		return false;
	*psize = 0;
	*ppdata = pdata = xtree_alloc(allocated);
	if (!pdata)
		return false;
	res = xtree_serialize_node(root, pdata, allocated, data_size);
	if (!res)
	{
		xtree_free_serialized(pdata);
		pdata = 0;
		data_size = 0;
	}
	*ppdata = pdata;
	*psize = data_size;
	return res;
}

void print_node(t_output_func output_func, void* output, pxnode node, int level, bool end)
{
	if (!node)
		return;
	if (node->parent)
		print_node(output_func, output, node->parent, level-1, false);
	output_func(output, "%S", node->name);
	switch(node->type)
	{
	case SBVT_NULL:
		output_func(output, "=<NULL>");
		break;
	case SBVT_EMPTY:
		output_func(output, "=<EMPTY>");
		break;
	case SBVT_STRING:
		output_func(output, "=S:%S", node->pdata);
		break;
	case SBVT_BOOL:
		output_func(output, "=s:%s", node->data._int ? "TRUE" : "FALSE");
		break;
	case SBVT_INT:
		output_func(output, "=d:%d", node->data._int);
		break;
	case SBVT_LONG:
	case SBVT_DATE_TIME:
		output_func(output, "=%016I64X", node->data._int64d);
		break;
	case SBVT_DATE:
		output_func(output, "=date:%s", node->pdata);
		break;
	case SBVT_BINARY:
		output_func(output, "=<BINARY, size='%d'>", node->data_size);
		break;
	case SBVT_FLOAT:
		output_func(output, "=f:%f", node->data._float);
		break;
	case SBVT_DOUBLE:
		output_func(output, "=f:%f", node->data._double);
		break;
	case SBVT_ARRAY:
		output_func(output, "[]\\");
		break;
	case SBVT_PARAMS:
		output_func(output, "\\");
		break;
	default:
		output_func(output, "=???");
		break;
	}
	if (end) 
		output_func(output, "\n");
}

void xtree_dump(t_output_func output_func, void* output, pxnode root, int level)
{
	pxnode node = root;
	
	while (node)
	{
		print_node(output_func, output, node, level, true);
		if (node->first_child)
			xtree_dump(output_func, output, node->first_child, level+1);
		node = node->next;
	}
}

int get_nodes_count(pxnode node)
{
	int c = 0;
	while (node)
	{
		c++;
		node = node->next;
	};
	return c;
}

void output_xml_bin_data(t_output_func output_func, void* output, void* data, size_t size)
{
	size_t tsize = ((size+2)/3)*4;
	char* ptdata = (char*)xtree_alloc(tsize+1);
	if (!ptdata)
		return;
	tsize = BinToText(data, size, ptdata, tsize);
	ptdata[tsize] = 0;
	output_func(output, "%s", ptdata);
	xtree_free(ptdata);
	return;
}

void output_xml_str(t_output_func output_func, void* output, wchar_t* str)
{
	do
	{
		size_t i, substr_len = wcscspn(str, L"<>&\"\'");
		for (i=0; i<substr_len; i++)
			output_func(output, "%C", str[i]);
		switch(str[substr_len])
		{
		case '<':	output_func(output, "&lt;"); break;
		case '>':	output_func(output, "&gt;"); break;
		case '&':	output_func(output, "&amp;"); break;
		case '\"':	output_func(output, "&quot;"); break;
		case '\'':	output_func(output, "&apos;"); break;
		default:
			return;
		}
		str += substr_len+1;
	} while (true);
	return;
}

void print_node_xml_pre(t_output_func output_func, void* output, pxnode node, int level, bool end)
{
	int i;
	if (!node)
		return;
//	if (node->parent)
//		print_node(output, node->parent, level-1, false);
//	output_func(output, "%S", node->name);
	for (i=0; i<level; i++)
		output_func(output, "\t");

	switch(node->type)
	{
	case SBVT_NULL:
		output_func(output, "<item name='");
		output_xml_str(output_func, output, node->name);
		output_func(output, "' type='null'></item>");
		break;
	case SBVT_EMPTY:
		output_func(output, "<item name='");
		output_xml_str(output_func, output, node->name);
		output_func(output, "' type='empty'></item>");
		break;
	case SBVT_STRING:
		output_func(output, "<item name='");
		output_xml_str(output_func, output, node->name);
		output_func(output, "' type='string'>");
		output_xml_str(output_func, output, (wchar_t*)node->pdata);
		output_func(output, "</item>");
		break;
	case SBVT_BOOL:
		output_func(output, "<item name='");
		output_xml_str(output_func, output, node->name);
		output_func(output, "' type='bool'>%d</item>", node->data._int);
		break;
	case SBVT_INT:
		output_func(output, "<item name='");
		output_xml_str(output_func, output, node->name);
		output_func(output, "' type='int'>%d</item>", node->data._int);
		break;
	case SBVT_LONG:
		output_func(output, "<item name='");
		output_xml_str(output_func, output, node->name);
		output_func(output, "' type='int64'>%I64d</item>", node->data._int64d);
		break;
	case SBVT_DATE_TIME:
		output_func(output, "<item name='");
		output_xml_str(output_func, output, node->name);
		output_func(output, "' type='time'>%I64d</item>", node->data._int64d);
		break;
	case SBVT_DATE:
		output_func(output, "<item name='");
		output_xml_str(output_func, output, node->name);
		output_func(output, "' type='date'>%s</item>", node->pdata);
		break;
	case SBVT_BINARY:
		output_func(output, "<item name='");
		output_xml_str(output_func, output, node->name);
		output_func(output, "' type='binary'>");
		output_xml_bin_data(output_func, output, node->pdata, node->data_size);
		output_func(output, "</item>", node->pdata);
		break;
	case SBVT_FLOAT:
		output_func(output, "<item name='");
		output_xml_str(output_func, output, node->name);
		output_func(output, "' type='float'>%f</item>", node->data._float);
		break;
	case SBVT_DOUBLE:
		output_func(output, "<item name='");
		output_xml_str(output_func, output, node->name);
		output_func(output, "' type='double'>%f</item>", node->data._double);
		break;
	case SBVT_ARRAY:
		output_func(output, "<array name='");
		output_xml_str(output_func, output, node->name);
		if (node->first_child)
			output_func(output, "' size='%d'>\n", get_nodes_count(node->first_child));
		else
			output_func(output, "' size='0'/>");
		break;
	case SBVT_PARAMS:
		output_func(output, "<group name='");
		output_xml_str(output_func, output, node->name);
		output_func(output, "'>\n");
		break;
	default:
		output_func(output, "=???");
		break;
	}
}

void print_node_xml_post(t_output_func output_func, void* output, pxnode node, int level, bool end)
{
	int i;
	if (!node)
		return;
//	if (node->type == SBVT_PARAMS || node->type == SBVT_ARRAY)
//		output_func(output, "        ");
	if ((node->type == SBVT_ARRAY && node->first_child)
		|| node->type == SBVT_PARAMS)
	{
		for (i=0; i<level; i++)
			output_func(output, "\t");
	}
	switch(node->type)
	{
	case SBVT_ARRAY:
		if (node->first_child)
			output_func(output, "</array>");
		break;
	case SBVT_PARAMS:
		output_func(output, "</group>");
		break;
	default:
		break;
	}
	output_func(output, "\n");
}

void xtree_dump_xml(t_output_func output_func, void* output, pxnode root, int level)
{
	pxnode node = root;
	if (level == 0)
	{
		output_func(output, "<?xml version='1.0' encoding='utf-8'?>\n");
	}

	while (node)
	{	
		if (wcscmp(node->name, L"chg") && wcscmp(node->name, L"hash"))
		{
			bool b_print_node = (wcscmp(node->name, L"chg") && wcscmp(node->name, L"dat") && wcscmp(node->name, L"hash"));
			if (b_print_node)
				print_node_xml_pre(output_func, output, node, level, true);
			if (node->first_child)
				xtree_dump_xml(output_func, output, node->first_child, b_print_node ? level+1 : level);
			if (b_print_node)
				print_node_xml_post(output_func, output, node, level, true);
		}
		node = node->next;
	}
}

static bool _load_nodes(unsigned char*& data, unsigned int& size, pxnode parent, pxnode* pnode, bool array)
{
	unsigned long i, count;
	count = GET_INTEGRAL_TYPE(data, size, unsigned long);
	if (count == c_dwNullParamsSize)
	{
		return true;
	}
	for (i=0; i<count; i++)
	{
		unsigned long len;
		pxnode node = xtree_alloc_node();
		if (!node)
			return false;
		node->parent = parent;
		if (array)
			len = 10;
		else
		{
			len = GET_INTEGRAL_TYPE(data, size, unsigned long);
			if (!len)
			{
#if defined (_WIN32)
				__asm int 3;
#endif
				return false;
			}
		}
		node->name = (wchar_t*)xtree_alloc((len+1) * sizeof(wchar_t));
		if (!node->name)
			return false;

		if (array)
		{
			_itow(i, node->name, 10);
		}
		else
		{
			memcpy(node->name, data, len*sizeof(wchar_t));
			node->name[len]=0;
			data += len*sizeof(wchar_t); size -= len*sizeof(wchar_t);
		}
		node->type = (serbinval_t)GET_INTEGRAL_TYPE(data, size, unsigned char);

		switch(node->type)
		{
			case SBVT_NULL:
				node->data_size = GET_INTEGRAL_TYPE(data, size, int);
				break;
			case SBVT_EMPTY:
#if defined (_WIN32)
				__asm int 3;
#endif
				break;
			case SBVT_STRING:
				len = GET_INTEGRAL_TYPE(data, size, unsigned long);
				node->pdata = xtree_alloc((len+1) * sizeof(wchar_t));
				if (!node->pdata)
					return false;
				memcpy(node->pdata, data, len*sizeof(wchar_t));
				((wchar_t*)(node->pdata))[len]=0;
				node->data_size = (len+1) * sizeof(wchar_t);
				data += len*sizeof(wchar_t); size -= len*sizeof(wchar_t);
				break;
			case SBVT_BOOL:
				node->data._int = GET_INTEGRAL_TYPE(data, size, unsigned char);
				node->data_size = 1;
				break;
			case SBVT_INT:
				node->data._int = GET_INTEGRAL_TYPE(data, size, int);
				node->data_size = sizeof(int);
				break;
			case SBVT_LONG:
			case SBVT_DATE_TIME:
				node->data._int64d = GET_INTEGRAL_TYPE(data, size, __int64);
				node->data_size = sizeof(__int64);
				break;
			case SBVT_DATE:
				len = GET_INTEGRAL_TYPE(data, size, unsigned long);
				node->pdata = xtree_alloc((len+1));
				if (!node->pdata)
					return false;
				memcpy(node->pdata, data, len);
				((char*)(node->pdata))[len]=0;
				node->data_size = (len+1);
				data += len; size -= len;
				break;
			case SBVT_BINARY:
				node->data_size = GET_INTEGRAL_TYPE(data, size, unsigned int);
				node->pdata = xtree_alloc(node->data_size+2);
				if (!node->pdata)
					return false;
				memcpy(node->pdata, data, node->data_size);
				((wchar_t*)node->pdata)[node->data_size/2] = 0;
				data += node->data_size; size -= node->data_size;
				if (node->type == SBVT_BINARY)
				{
					if (0 == wcscmp(node->name, L"proxy_pwd")
						|| 0 == wcscmp(node->name, L"klprts-TaskAccountPassword"))
					{
						if (UnprotectData(node->pdata, node->data_size, node->pdata, node->data_size))
							node->type = SBVT_STRING;
					}
				}
				break;
			case SBVT_FLOAT:
				node->data._float = GET_INTEGRAL_TYPE(data, size, float);
				node->data_size = sizeof(float);
				break;
			case SBVT_DOUBLE:
				node->data._double = GET_INTEGRAL_TYPE(data, size, double);
				node->data_size = sizeof(double);
				break;
			case SBVT_ARRAY:
				if (!_load_nodes(data, size, node, &node->first_child, true))
					return false;
				break;
			case SBVT_PARAMS:
				if (!_load_nodes(data, size, node, &node->first_child, false))
					return false;
				break;
			default:
#if defined (_WIN32)
				__asm int 3;
#endif
				break;
		}
		*pnode = node;
//		print_node(node, 0, 0);
//		output_func(output, "\n");
		pnode = &((*pnode)->next);
	}
	return true;
}

bool xtree_load(unsigned char* data, unsigned int size, pxnode* proot)
{
	xmlbin_hdr* phdr = (xmlbin_hdr*)data;
	unsigned int oldsize = size;
	bool res;
	if (!data || !proot)
		return false;
	if (size < sizeof(xmlbin_hdr))
		return false;
	if (0!=memcmp(phdr->c_szSignBIN, c_szSignBIN, sizeof(c_szSignBIN)))
		return false;
	data += phdr->body_offset;
	size -= phdr->body_offset;
	res = _load_nodes(data, size, 0, proot, false);
	if (!res)
		xtree_free_node(*proot);
	return res;
}

/*
bool _xtree_load_file(void* ss, pxnode* proot)
{
	unsigned int size;
	unsigned char* buffer = 0;
	bool res = false;
	size = GetFileSize(ss, NULL);
	buffer = (unsigned char*)xtree_alloc(size);
	if (!buffer)
		return false;
	if (ReadFile(ss, buffer, size, (DWORD*)&size, 0))
		res = xtree_load(buffer, size, proot);
	xtree_free(buffer);
	return res;
}

bool xtree_load_fileW(wchar_t* filename, pxnode* proot)
{
	bool res;
	wchar_t* ext;
	wchar_t* lockname = (wchar_t*)xtree_alloc((wcslen(filename)+10)*sizeof(wchar_t));

	if (!lockname)
		return false;
	wcscpy(lockname, filename);
	ext = wcsrchr(lockname, '.');
	if (!ext)
		ext = lockname + wcslen(filename);
	wcscpy(ext, L".lck");
	void* hLock = CreateFileW(lockname, GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE, 0);
	xtree_free(lockname);
	if (hLock == INVALID_HANDLE_VALUE)
		return false;
	void* ss = CreateFileW(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
	if (ss == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hLock);
		return false;
	}
	res = _xtree_load_file(ss, proot);
	CloseHandle(ss);
	CloseHandle(hLock);
	return res;
}

bool xtree_load_fileA(char* filename, pxnode* proot)
{
	bool res;
	char* ext;
	char* lockname = (char*)xtree_alloc(strlen(filename)+10);

	if (!lockname)
		return false;
	strcpy(lockname, filename);
	ext = strrchr(lockname, '.');
	if (!ext)
		ext = lockname + strlen(filename);
	strcpy(ext, ".lck");
	void* hLock = CreateFileA(lockname, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE, 0);
	xtree_free(lockname);
	if (hLock == INVALID_HANDLE_VALUE)
		return false;
	void* ss = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
	if (ss == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hLock);
		return false;
	}
	res = _xtree_load_file(ss, proot);
	CloseHandle(ss);
	CloseHandle(hLock);
	return res;
}
*/

pxnode xtree_get_node(pxnode root, wchar_t* path)
{
	wchar_t* name = path;
	wchar_t* next;
	wchar_t wc;
	pxnode node;
	size_t len;
	if (!path)
		return root;
	if (!root)
		return NULL;
	node = root->first_child;
	if (!node)
		return NULL;
	while (wc = *name)
	{
		if (wc == '\\' || wc == '/' || wc == '[' || wc == ']')
			name++;
		else 
			break;
	}
	if (*name == 0)
		return root;
	next = name;
	while (wc = *next)
	{
		if (wc == '\\' || wc == '/' || wc == '[')
			break;
		next++;
	}
	len = next - name;
	while (wc = *next)
	{
		if (wc == '\\' || wc == '/' || wc == '[' || wc == ']')
			next++;
		else 
			break;
	}
	while (node)
	{
		if (0 == wcsncmp(node->name, name, len) && wcslen(node->name) == len)
		{
			if (*next)
				return xtree_get_node(node, next);
			return node;
		}
		node = node->next;
	}
	return NULL;
}

void * xtree_query_value_p(pxnode root, wchar_t* path, serbinval_t* ptype, size_t* pdata_size)
{
	pxnode node = xtree_get_node(root, path);
	if( !node )
	{
		if( pdata_size )
			*pdata_size = 0;
		return NULL;
	}
	
	if( pdata_size )
		*pdata_size = node->data_size;
	if( ptype )
		*ptype = node->type;
	return node->pdata;
}

bool xtree_query_value(pxnode root, wchar_t* path, serbinval_t* ptype, void* buffer, size_t buff_size, size_t* pdata_size)
{
	pxnode node = xtree_get_node(root, path);
	if (!node)
	{
		if (pdata_size)
			*pdata_size = 0;
		return false;
	}
	if (pdata_size)
		*pdata_size = node->data_size;
	if (ptype)
		*ptype = node->type;
	if (buffer)
// RM: buff_size is size_t, node->data_size is unsigned int, so its an error
//		memcpy(buffer, node->pdata, min(buff_size, node->data_size));
		memcpy(buffer, node->pdata, min(buff_size, (size_t)(node->data_size)));
	if (buff_size < node->data_size)
		return false;
	return true;
}

pxnode xtree_find_node(pxnode root, wchar_t* name, bool recursively)
{
	pxnode node, cnode;
	for (node=root; node != NULL; node = node->next)
	{
		if (!wcscmp(node->name, name))
			return node;
		if (recursively && node->first_child)
		{
			cnode = xtree_find_node(node->first_child, name, recursively);
			if (cnode)
				return cnode;
		}
	}
	return NULL;
}
pxnode xtree_find_str_node(pxnode root, wchar_t* name, wchar_t* value, bool recursively)
{
	pxnode node, cnode;
	for (node=root; node != NULL; node = node->next)
	{
		if (node->type == SBVT_STRING)
		{
			if (!wcscmp(node->name, name) && !wcscmp((wchar_t*)node->pdata, value))
				return node;
		}
		if (recursively && node->first_child)
		{
			cnode = xtree_find_str_node(node->first_child, name, value, recursively);
			if (cnode)
				return cnode;
		}
	}
	return NULL;
}

void xtree_xor_buffer(void* buffer, size_t size)
{
	unsigned long i, key = 0x78F25E9C;
	unsigned long* data = (unsigned long*)buffer;
	size /= sizeof(unsigned long);
	for (i=0; i<size; i++)
	{
		data[i] ^= key;
		key += 0x7A3F93DE;
	}
	return;
}

#if !defined(_SS_NO_PRAGUE)
bool xtree_query_val(pxnode root, wchar_t* path, cStrObj& val)
{
	pxnode node = xtree_get_node(root, path);
	if( node && node->type == SBVT_STRING && node->pdata )
		return PR_SUCC(val.assign(node->pdata, cCP_UNICODE, node->data_size));
	return false;
}

bool xtree_query_val(pxnode root, wchar_t* path, tBYTE& val)
{
	tQWORD qw = 0;
	if( !xtree_query_val(root, path, qw) )
		return false;
	return val = (tBYTE)qw, true;
}

bool xtree_query_val(pxnode root, wchar_t* path, tWORD& val)
{
	tQWORD qw = 0;
	if( !xtree_query_val(root, path, qw) )
		return false;
	return val = (tWORD)qw, true;
}

bool xtree_query_val(pxnode root, wchar_t* path, tDWORD& val)
{
	tQWORD qw = 0;
	if( !xtree_query_val(root, path, qw) )
		return false;
	return val = (tDWORD)qw, true;
}

bool xtree_query_val(pxnode root, wchar_t* path, tQWORD& val)
{
	pxnode node = xtree_get_node(root, path);
	if( !node || !node->pdata )
		return false;

	if( node->type == SBVT_LONG && node->data_size == sizeof(unsigned long) )
		val = *(unsigned long *)node->pdata;
	else if( node->type == SBVT_LONG && node->data_size == sizeof(__int64) )
		val = *(__int64 *)node->pdata;
	else if( node->type == SBVT_INT && node->data_size == sizeof(unsigned int) )
		val = *(unsigned int *)node->pdata;
	else if( node->type == SBVT_BOOL )
		val = *(unsigned char *)node->pdata;
	else
		return false;
		
	return true;
}
#endif // _SS_NO_PRAGUE
