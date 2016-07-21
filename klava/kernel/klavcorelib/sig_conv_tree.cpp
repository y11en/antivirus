// klavcorelib/sig_conv_tree.cpp
//
//

#include <klava/klav_sig.h>
#include <klava/klavtree.h>

static void put_tree_op (klavstl::buffer <klav_allocator>& buf, uint8_t code, uint8_t arg1, uint8_t arg2)
{
	buf.push_back (KLAV_TREE_ESCAPE_BYTE);
	buf.push_back (code);
	buf.push_back (arg1);
	buf.push_back (arg2);
}

klav_bool_t KLAV_CALL KLAV_Sig_Conv_Tree (
			const void * sig_data,
			size_t sig_data_len,
			klavstl::buffer <klav_allocator>& tree_sig,
			const char **errmsg
		)
{
	tree_sig.clear ();
	if (errmsg != 0)
		*errmsg = 0;

	const uint8_t *p = (const uint8_t *)sig_data;
	const uint8_t *pe = p + sig_data_len;

	KLAV_Sig_Element elt;

	while (p < pe)
	{
		const uint8_t *elt_data = p;

		unsigned int elt_type = KLAV_SIG_ELT_INVALID;

		size_t sz = elt.scan (p, pe - p);
		if (sz != 0)
		{
			p += sz;
			elt_type = elt.type ();
		}

		switch (elt_type)
		{
		case KLAV_SIG_ELT_BYTES:
			{
			  elt_data += elt.get_data_offset ();
			  unsigned int cnt = elt.get_data_size ();
			  for (unsigned int i = 0; i < cnt; ++i)
			  {
			    uint8_t ch = elt_data [i];
			    if (ch == KLAV_TREE_ESCAPE_BYTE)
				  tree_sig.push_back (KLAV_TREE_ESCAPE_BYTE);
				tree_sig.push_back (ch);
			  }
			}
			break;

		case KLAV_SIG_ELT_IBYTES:
			{
			  elt_data += elt.get_data_offset ();
			  unsigned int cnt = elt.get_data_size ();
			  for (unsigned int i = 0; i < cnt; ++i)
			  {
			    uint8_t ch = elt_data [i];
				if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
				{
				  put_tree_op (tree_sig, KLAV_TREE_CODE_AND, ~0x20, ch & ~0x20);
				}
				else
				{
				  if (ch == KLAV_TREE_ESCAPE_BYTE)
				    tree_sig.push_back (KLAV_TREE_ESCAPE_BYTE);
				  tree_sig.push_back (ch);
				}
			  }
			}
			break;

		case KLAV_SIG_ELT_ANYBYTES:
			{
			  unsigned int cnt = elt.get_anybyte_count ();
			  for (unsigned int i = 0; i < cnt; ++i)
			  {
			    put_tree_op (tree_sig, KLAV_TREE_CODE_AND, 0, 0);
			  }
			}
			break;

		case KLAV_SIG_ELT_BITMASK:
			put_tree_op (tree_sig, KLAV_TREE_CODE_AND, elt.get_bitmask_mask (), elt.get_bitmask_value ());
			break;

		case KLAV_SIG_ELT_RANGE:
			put_tree_op (tree_sig, KLAV_TREE_CODE_RANGE, elt.get_range_lbound (), elt.get_range_hbound ());
			break;

		case KLAV_SIG_ELT_STAR:
			{
			  unsigned int cnt = elt.get_anybyte_count ();
			  uint8_t lo = (uint8_t)(cnt & 0xFF);
			  uint8_t hi = (uint8_t)((cnt >> 8) & 0xFF);
			  put_tree_op (tree_sig, KLAV_TREE_CODE_ASTER, lo, hi);
			}
			break;

		default:
			if (errmsg != 0)
				*errmsg = "signature contains invalid elements";
			tree_sig.clear ();
			return false;
		}
	}

	if (tree_sig.size () == 0)
	{
		if (errmsg != 0)
			*errmsg = "signature is empty";
		return false;
	}

	return true;
}
