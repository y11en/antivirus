// kdc_mft.cpp
//
// KDC manifest (XML format) reader
//

#include <klava/kcdf_utils.h>
#include <klava/klavstl/buffer.h>
#include <klava/klavstl/vector.h>
#include <klava/klavstl/string.h>
#include <klava/klavsys.h>
#include <klava/klavdb.h>

#define LAX_NO_STL
#define LAX_NO_IMPLEMENTATION
#include <LAX.h>

typedef klavstl::buffer <klav_allocator> klav_buffer;

static KLAV_ERR read_db_file (hKLAV_DB_Manager db_mgr, const char *name, klav_buffer& databuf)
{
	KLAV_ERR err = KLAV_OK;
	databuf.clear ();

	if (db_mgr == 0)
		return KLAV_EINVAL;

	hKLAV_IO_Object hio = 0;
	err = db_mgr->open_db_file (name, KLAV_IO_READONLY, KLAV_FILE_OPEN_EXISTING, 0, & hio);
	if (KLAV_FAIL (err))
		return err;

	KLAV_IO_Object_Holder io_holder (hio);

	klav_filepos_t fsz = 0;
	err = hio->get_size (& fsz);
	if (KLAV_FAIL (err))
		return err;

	if (! databuf.resize ((size_t) fsz))
		return KLAV_ENOMEM;

	err = hio->seek_read (0, databuf.data (), (uint32_t) fsz, 0);
	if (KLAV_FAIL (err))
		return err;

	io_holder.clear ();
	return KLAV_OK;
}

klav_bool_t KDB_Is_XML_Manifest (
			hKLAV_Alloc alloc,
			const void *mft_data,
			size_t mft_size
		)
{
	if (mft_size < 10)
		return false;

	const uint8_t * mft = (const uint8_t *) mft_data;

	if (mft [0] == '<'
	 && mft [1] == 'k'
	 && mft [2] == 'd'
	 && mft [3] == 'b'
	 && (mft [4] == '>' || mft [4] == ' '))
		return true;

	return false;
}

////////////////////////////////////////////////////////////////

static uint32_t parse_dec (const char *s)
{
	uint32_t val = 0;
	while (*s == ' ') ++s;
	while (*s >= '0' && *s <= '9')
	{
		val = val * 10 + (*s - '0');
		++s;
	}
	return val;
}

static uint32_t parse_hex (const char *s)
{
	uint32_t val = 0;
	while (*s == ' ') ++s;
	for (;;++s)
	{
		if (*s >= '0' && *s <= '9') val = val * 16 + (*s - '0');
		else if (*s >= 'A' && *s <= 'F') val = val * 16 + (*s - 'A') + 10;
		else if (*s >= 'a' && *s <= 'f') val = val * 16 + (*s - 'a') + 10;
		else break;
	}
	return val;
}

static uint32_t parse_num (const char *s)
{
	while (*s == ' ') ++s;
	if (s [0] == '0' && (s [1] == 'x' || s [1] == 'X'))
		return parse_hex (s + 2);
	return parse_dec (s);
}

typedef klavstl::string <klav_allocator> klav_stringbuf;

typedef LAX::UTF8_UTF8 <LAX::STD_STRING_ADAPTER <char, klav_stringbuf> > XmlTransform;

struct XML_Attr
{
	klav_stringbuf m_name;
	klav_stringbuf m_value;

	XML_Attr (hKLAV_Alloc alloc)
		: m_name (alloc), m_value (alloc) {}
};

struct XML_Attrs
{
public:
	XML_Attrs (hKLAV_Alloc alloc)
		: m_allocator (alloc), m_attrs (alloc) {}

	size_t size () const
		{ return m_attrs.size (); }

	const XML_Attr& operator [] (size_t idx) const
		{ return m_attrs [idx]; }

	void clear ()
		{ m_attrs.clear (); }

	int find (const char *name) const
	{
		size_t i, cnt = m_attrs.size ();
		for (i = 0; i < cnt; ++i)
		{
			const XML_Attr& attr = m_attrs [i];
			if (strcmp (attr.m_name.c_str (), name) == 0)
				return (int) i;
		}
		return -1;
	}	

	const char * find_val (const char *name) const
	{
		int idx = find (name);
		if (idx < 0)
			return 0;
		return m_attrs [idx].m_value.c_str ();
	}

    int addAttr (const char *name, size_t nlen, const char *value, size_t vlen)
	{ 
		const size_t idx = m_attrs.size();
		m_attrs.resize (idx+1, XML_Attr (m_allocator));
		XML_Attr& attr = m_attrs [idx];
		LAX::Xml<XmlTransform>::unescape (name, nlen, attr.m_name);
		LAX::Xml<XmlTransform>::unescape (value, vlen, attr.m_value);
		return LAX::XML_OK;
	}

private:
	hKLAV_Alloc m_allocator;
	klavstl::trivial_vector <XML_Attr, klav_allocator> m_attrs;
};

typedef LAX::XmlReader <XmlTransform, LAX::STD_ATTR_MAP_ADAPTER<char, XML_Attrs>, LAX::XmlNoExceptions> XmlReader;
typedef XmlReader::Element XmlElement;

class XML_Mft_Reader
{
public:
	XML_Mft_Reader (
			hKLAV_Alloc alloc,
			hKLAV_DB_Manager db_mgr,
			KCDFTagHandler* tag_handler,     // may be 0
			KCDFClusterHandler* kdc_handler  // may be 0
		) :
			m_allocator (alloc),
			m_db_manager (db_mgr),
			m_tag_handler (tag_handler),
			m_kdc_handler (kdc_handler)
		{
		}

	KLAV_ERR parse_set (const void *mft_data, size_t mft_size);
	KLAV_ERR parse_setfile (const char *fname, klav_bool_t opt);

private:
	hKLAV_Alloc      m_allocator;
	hKLAV_DB_Manager m_db_manager;
	KCDFTagHandler * m_tag_handler;
	KCDFClusterHandler * m_kdc_handler;
};

KLAV_ERR XML_Mft_Reader::parse_setfile (const char *set_name, klav_bool_t opt)
{
	KLAV_ERR err = KLAV_OK;

	if (set_name == 0 || set_name [0] == 0)
		return KLAV_EINVAL;

	if (set_name == 0 || set_name [0] == 0)
		return KLAV_EINVAL;

	klav_buffer mft_buf (m_allocator);
	err = read_db_file (m_db_manager, set_name, mft_buf);
	if (KLAV_FAIL (err))
	{
		if (err == KLAV_ENOTFOUND && opt != 0)
			return KLAV_OK;

		return err;
	}

	err = parse_set (mft_buf.data (), mft_buf.size ());

	return err;
}

static KLAV_ERR ret_error (KLAV_ERR err)
{
	return err;
}

#define RETURN_ERROR(ERR) return ret_error (err)
#define CHECK_RETURN_ERROR(ERR) if (KLAV_FAIL (err)) return ret_error (err)

KLAV_ERR XML_Mft_Reader::parse_set (const void *mft_data, size_t mft_size)
{
	KLAV_ERR err = KLAV_OK;
	int xml_err = LAX::XML_OK;

	klav_stringbuf strbuf (m_allocator);

	XmlReader xml_reader ((const char *) mft_data, mft_size);

	XmlElement kdb_elt;
	xml_err = xml_reader.readElement (kdb_elt, LAX::XML_F_ELT_REQUIRED|LAX::XML_F_CHECK_NAME, "kdb");
	if (xml_err != LAX::XML_OK)
		RETURN_ERROR (KLAV_ECORRUPT);

	XML_Attrs elt_attrs (m_allocator);
	if (kdb_elt.readAttrs (elt_attrs) != LAX::XML_OK)
		RETURN_ERROR (KLAV_ECORRUPT);

	const char * strval = elt_attrs.find_val ("rn");
	if (strval != 0 && m_tag_handler != 0)
	{
		uint32_t rn = parse_num (strval);
		err = m_tag_handler->addTag (KLAV_KERNEL_PARM_DB_TIMESTAMP, rn);
		CHECK_RETURN_ERROR (err);
	}

	if (kdb_elt.hasBody ())
	{
		XmlElement grp_elt;
	
		for (;;)
		{
			xml_err = xml_reader.readElement (grp_elt, 0, 0);
			if (xml_err == LAX::XML_END)
				break;
			if (xml_err != LAX::XML_OK)
				RETURN_ERROR (KLAV_ECORRUPT);

			if (grp_elt.compareXmlTag ("parms"))
			{
				if (grp_elt.hasBody ())
				{
					XmlElement elt;
					for (;;)
					{
						xml_err = xml_reader.readElement (elt, 0, 0);
						if (xml_err == LAX::XML_END)
							break;
						if (xml_err != LAX::XML_OK)
							RETURN_ERROR (KLAV_ECORRUPT);

						if (elt.readAttrs (elt_attrs) != LAX::XML_OK)
							RETURN_ERROR (KLAV_ECORRUPT);

						const char * tagname = elt_attrs.find_val ("id");
						uint32_t tag = 0;
						if (! KDBID_PARSE_STRING_QUAD (tagname, & tag))
							RETURN_ERROR (KLAV_ECORRUPT);

						strbuf.clear ();
						if (elt.hasBody ())
							xml_reader.readText (strbuf);

						uint32_t tagval = (strbuf.empty ()) ? 0 : parse_num (strbuf.c_str ());

						if (xml_reader.endElement (elt) != LAX::XML_OK)
							RETURN_ERROR (KLAV_ECORRUPT);

						if (m_tag_handler != 0)
						{
							err = m_tag_handler->addTag (tag, tagval);
							CHECK_RETURN_ERROR (err);
						}
					}
				}
			}
			else if (grp_elt.compareXmlTag ("set") && m_kdc_handler != 0)
			{
				if (grp_elt.readAttrs (elt_attrs) != LAX::XML_OK)
					RETURN_ERROR (KLAV_ECORRUPT);

				int opt_val = 0;
				const char *opt_attr = elt_attrs.find_val ("opt");
				if (opt_attr != 0)
					opt_val = parse_num (opt_attr);

				const char *ref = elt_attrs.find_val ("ref");
				if (ref != 0 && ref [0] != 0)
				{
					// load external setfile
					err = parse_setfile (ref, opt_val);
					CHECK_RETURN_ERROR (err);
				}
				else // inline set
				{
					strval = elt_attrs.find_val ("id");
					uint32_t set_id = (strval == 0) ? 0 : parse_num (strval);

					if (m_kdc_handler != 0)
					{
						err = m_kdc_handler->startSet (set_id);
						CHECK_RETURN_ERROR (err);
					}

					if (grp_elt.hasBody ())
					{
						XmlElement elt;
						for (;;)
						{
							xml_err = xml_reader.readElement (elt, 0, 0);
							if (xml_err == LAX::XML_END)
								break;
							if (xml_err != LAX::XML_OK)
								RETURN_ERROR (KLAV_ECORRUPT);

							if (elt.compareXmlTag ("kdc"))
							{
								strbuf.clear ();
								if (elt.hasBody ())
									xml_reader.readText (strbuf);
								if (strbuf.empty ())
									RETURN_ERROR (KLAV_ECORRUPT);
								if (m_kdc_handler != 0)
								{
									err = m_kdc_handler->startCluster ();
									CHECK_RETURN_ERROR (err);

									err = m_kdc_handler->addProperty (KCDF_TAG_CLUSTER_FILE, strbuf.c_str (), (uint32_t) strbuf.size ());
									CHECK_RETURN_ERROR (err);

									err = m_kdc_handler->endCluster ();
									CHECK_RETURN_ERROR (err);
								}
							}

							if (xml_reader.endElement (elt) != LAX::XML_OK)
								RETURN_ERROR (KLAV_ECORRUPT);
						}
					}
				}
			}

			if (xml_reader.endElement (grp_elt) != LAX::XML_OK)
				RETURN_ERROR (KLAV_ECORRUPT);
		}
	}

	if (xml_reader.endElement (kdb_elt) != LAX::XML_OK)
		RETURN_ERROR (KLAV_ECORRUPT);

	return KLAV_OK;
}

////////////////////////////////////////////////////////////////

KLAV_ERR KDB_Read_XML_Manifest_Data (
			hKLAV_Alloc alloc,
			hKLAV_DB_Manager db_mgr,
			const void *mft_data,
			size_t mft_size,
			KCDFTagHandler* tag_handler,     // may be 0
			KCDFClusterHandler* kdc_handler  // may be 0
		)
{
	if (mft_data == 0 || mft_size == 0)
		return KLAV_EINVAL;

	XML_Mft_Reader reader (alloc, db_mgr, tag_handler, kdc_handler);

	KLAV_ERR err = reader.parse_set (mft_data, mft_size);

	return err;
}

KLAV_ERR KDB_Read_XML_Manifest (
			hKLAV_Alloc alloc,
			hKLAV_DB_Manager db_mgr,
			const char *set_name,
			KCDFTagHandler* tag_handler,     // may be 0
			KCDFClusterHandler* kdc_handler  // may be 0
		)
{
	XML_Mft_Reader reader (alloc, db_mgr, tag_handler, kdc_handler);

	KLAV_ERR err = reader.parse_setfile (set_name, 0);

	return err;
}

