// --------------------------------------------------------------------------------
#ifndef NET_PARSER_H_INCLUDED
#define NET_PARSER_H_INCLUDED

#include <stdio.h>
#include "../include\klava\klav_avp.h"
#include "../windows/Hook/Plugins/Include/logger_api.h"
#include <NetDetails/structs/s_netdetails.h>

class CNetParser
{
public:
	CNetParser(void);
	~CNetParser(void);

	tERROR GetSnifferFrameSummary(const cSnifferData* pData, cSnifferFrameReport* pFrameReport);

	tERROR GetSnifferFrameDetails(const cSnifferData* pData, cSnifferFrameDetailsItem* pFrameDetails);

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define tTree	cSnifferFrameDetailsItem
#define tString cStringObj

extern void utf8_to_utf16(const char*, size_t , tString&);

class ParserEngine
{
public:
	struct tSummary
	{
		tSummary() : mac_destination_address(0), mac_source_address(0), is_ipv4(true), ether_type(0xFFFF), protocol(0xFF), hi_protocol(0xFF) {}

		tQWORD	mac_destination_address;
		tQWORD	mac_source_address;
		tWORD	ether_type;
		tBYTE	protocol;
		tBYTE	hi_protocol;
		tBOOL	is_ipv4;
		tIPv6	ip_source_address;
		tIPv6	ip_destination_address;
	};

private:
	// id строк в файле "sniffer.loc" [s_FrameDetails]
	enum
	{
		FRAME = 1,
		NODE_ETHERNET,
		NODE_IEEE802_3,
		NODE_IEEE802_1Q,
		NODE_DEST_MAC,
		NODE_SRC_MAC,
		NODE_ETHER_TYPE,
		NODE_IP_V4,
		NODE_IP_V6,
		NODE_ARP,
		NODE_GROUP_ADDR,
		NODE_INDIVIDUAL_ADDR,
		NODE_LOCAL_ADMIN_ADDR,
		NODE_UNIVERSAL_ADDR,
		NODE_VERSION,
		NODE_HEADER_LEN,
		NODE_DSF,
		NODE_TL,
		NODE_ID,
		NODE_FLAGS,
		NODE_OFFSET,
		NODE_TTL,
		NODE_PROTO,
		NODE_CHECKSUM,
		NODE_SRC_IP,
		NODE_DST_IP,
		NODE_TC,
		NODE_FL,
		NODE_PAYL,
		NODE_NEXTH,
		NODE_HOPL,
		NODE_HTYPE,  
		NODE_PTYPE,  
		NODE_HLEN, 
		NODE_PLEN,  
		NODE_OPER,
		NODE_SHA, 
		NODE_SPA,  
		NODE_THA,  
		NODE_TPA,
		NODE_ICMP,
		NODE_TCP,
		NODE_UDP,
		NODE_SRC_PORT,
		NODE_DST_PORT,
		NODE_LEN,
		NODE_CHKSUM,
		NODE_DATA,
		NODE_SEQ_NUM,
		NODE_ACK_NUM,
		NODE_DATA_OFF,
		NODE_WINDOW,
		NODE_URG_PTR,
		NODE_CWR,
		NODE_ECN_ECHO,
		NODE_URG,
		NODE_ACK,
		NODE_PSH,
		NODE_RST,
		NODE_SYN,
		NODE_FIN,
		NODE_TYPE,
		NODE_CODE,
		NODE_ID2,
		NODE_SEQUENCE,
		NODE_DNS,
		NODE_QUERY_ID,
		NODE_QDCOUNT,
		NODE_ANCOUNT,
		NODE_NSCOUNT,
		NODE_ARCOUNT,
		NODE_QR,
		NODE_OPCODE,
		NODE_AA,
		NODE_TRC,
		NODE_RD,
		NODE_RA,
		NODE_Z,
		NODE_RCODE,
		NODE_QUSECT,
		NODE_QUNAME,
		NODE_QUTYPE,
		NODE_QCLASS,
		NODE_ANSECT,
		NODE_AUSECT,
		NODE_ADSECT,
		NODE_DATALEN,
		NODE_DOMN,
		NODE_HOST,
		NODE_IP,
		NODE_AUTH_SERVER,
		NODE_MAIL_DEST,
		NODE_MAIL_FRWD,
		NODE_CNNAME,
		NODE_MAIL_DOMN,
		NODE_MAIL_GRP,
		NODE_MAIL_REN,
		NODE_SOAR,
		NODE_PR_DNS,
		NODE_RESP_MAIL,
		NODE_VER,
		NODE_REFRESH,
		NODE_RETRY,
		NODE_EXP_LIMIT,
		NODE_MIN_TTL,
		NODE_NULL_RD,
		NODE_WKS,
		NODE_PROTOCOL,
		NODE_BIT_MAP,
		NODE_HINFO,
		NODE_CPU,
		NODE_OS,
		NODE_MINFO,
		NODE_RMBOX,
		NODE_EMBOX,
		NODE_MEXCH,
		NODE_PREF,
		NODE_EXCH_MBOX,
		NODE_TEXT,
		NODE_PPPOE,
		NODE_PPP_TYPE,
		NODE_PPP_CODE,
		NODE_PPP_ID,
		NODE_IPX,
		NODE_LLC,
		NODE_DSAP,
		NODE_SSAP,
		NODE_CF,
		NODE_RF,
		NODE_ADDRESS,
		NODE_CFLD,
		NODE_PACKET_LEN,
		NODE_TRANSPORT_CTRL,
		NODE_PACKET_TYPE,
		NODE_DST_NETWORK,
		NODE_DST_NODE,
		NODE_DST_SOCKET,
		NODE_SRC_NETWORK,
		NODE_SRC_NODE,
		NODE_SRC_SOCKET,
		NODE_NBIPX,
		NODE_TRANS_ID,
		NODE_R,
		NODE_OPCODE_2,
		NODE_RESERVED,
		NODE_B,
		NODE_RCODE_2,
		NODE_QUTYPE_2,
		NODE_QCLASS_2,
		NODE_ILLEGAL_NBNAME,
		NODE_DHCP,
		NODE_NBNS,
		NODE_M_TYPE,
		NODE_H_TYPE,
		NODE_AD_LEN,
		NODE_HOPE_COUNT,
		NODE_SECONDS,
		NODE_CIP,
		NODE_YIP,
		NODE_SIP,
		NODE_RAIP,
		NODE_FLAG,
		NODE_C_MAC,
		NODE_H_ADDR,
		NODE_HOST_NAME,
		NODE_BOOT_FILE,
		NODE_MAGIC,
		NODE_DHCP_OPTION,
		NODE_VALUE,
		NODE_BOOL,
		NODE_IP_MASK,
		NODE_DST_ROUTER,
		NODE_F_TYPE,
		NODE_OVERLOADS,
		NODE_M_TYPE2,
		NODE_OPTION,
		NODE_SLPD,
		NODE_SLPA,
		NODE_NWIP,
		NODE_RCODE1,
		NODE_RCODE2,
		NODE_DN
	};

	tBYTE*		m_start;
	tBYTE*		m_end;
	tTree*		m_root;
	tSummary	m_summary;
	

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// методы чтения данных сетевого пакета
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	tBYTE read_uint8(const tBYTE* data, tDWORD pos = 0)
	//-------------------------------------------------
	{
		return (tBYTE) data[pos];
	}
	tWORD read_uint16(const tBYTE* data, tDWORD pos = 0)
	//--------------------------------------------------
	{
		return (tWORD)(data[pos] << 8) + data[pos + 1];
	}
	tDWORD read_uint32(const tBYTE* data, tDWORD pos = 0)
	//---------------------------------------------------
	{
		return ((tDWORD)data[pos] << 24) | ((tDWORD)data[pos + 1] << 16) | ((tDWORD)data[pos + 2] << 8) | (tDWORD) data[pos + 3];
	}
	tQWORD read_uint64(const tBYTE* data, tDWORD pos = 0)
	//---------------------------------------------------
	{
		tDWORD hi = read_uint32(data, pos);
		tDWORD lo = read_uint32(data, pos + 4);
		return ((tQWORD)hi  << 32) | (tQWORD) lo;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// методы заполнения дерева детальной информации о сетевом пакете
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void set_name(tTree* tree, tDWORD id, tString value = "")
	//-------------------------------------------------------
	{
		if (!tree) return;

		tree->m_fmt_item.m_id = id;
		tree->m_fmt_item.m_value = value;
	}
	void set_length(tTree* tree, tDWORD length)
	//-----------------------------------------
	{
		if (!tree) return;

		tree->m_fmt_item.m_lenData = length;
	}
	tDWORD get_offset(tTree* tree)
	//----------------------------
	{
		if (!tree) return (tDWORD) -1;

		return tree->m_fmt_item.m_offData;
	}
	tTree* add_child(tTree* tree, tDWORD offset, tDWORD length, tDWORD id, tString value = "", tString value2 = "", tString value3 = "")
	//----------------------------------------------------------------------------------------------------------------------------------
	{
		if (!tree) return tree;

		tTree item;
		item.m_fmt_item.m_id = id;
		item.m_fmt_item.m_offData = offset;
		item.m_fmt_item.m_lenData = length;
		item.m_fmt_item.m_value = value;
		item.m_fmt_item.m_value2 = value2;
		item.m_fmt_item.m_value3 = value3;
		return &tree->m_vChild.push_back(item);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// строковое представление различных данных
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// для переносимости на *nix используем SNPRINTF (а не SNPRINTF_S) и не обращаем внимание на warning C4996
	#pragma warning(disable : 4996 4309)

	tString format_as_mac_address(const tBYTE* data)
	//----------------------------------------------
	{
		tCHAR buff[18];

		_snprintf(buff, sizeof(buff)/sizeof(tCHAR), "%02x-%02x-%02x-%02x-%02x-%02x", data[0], data[1], data[2], data[3], data[4], data[5]);
		return buff;
	}
	tString format_as_ip_address(const tBYTE* data)
	//----------------------------------------------
	{
		tCHAR buff[16];

		_snprintf(buff, sizeof(buff)/sizeof(tCHAR), "%d.%d.%d.%d", data[0], data[1], data[2], data[3]);
		return buff;
	}
	tString format_as_ipv6_address(const tBYTE* data)
	//-----------------------------------------------
	{
		tCHAR buff[40];

		_snprintf(buff, sizeof(buff)/sizeof(tCHAR), "%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x", 
			data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
			data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
		return buff;
	}
	tString format_as_hex_byte(const tBYTE data)
	//------------------------------------------
	{
		tCHAR buff[7];
		_snprintf(buff, sizeof(buff)/sizeof(tCHAR), "0x%02x", data);
		return buff;
	}
	tString format_as_hex_word(const tWORD data)
	//------------------------------------------
	{
		tCHAR buff[7];
		_snprintf(buff, sizeof(buff)/sizeof(tCHAR), "0x%04x", data);
		return buff;
	}
	tString format_as_hex(const tBYTE* data, tBYTE length)
	//----------------------------------------------------
	{
		tString s = "0x";

		for (int i = 0; i < length; i++)
		{
			tCHAR buff[4];
			_snprintf(buff, sizeof(buff)/sizeof(tCHAR), "%02x", data[i]);
			s += buff;
		}
		return s;
	}	
	tString format_as_num(const tDWORD data)
	//--------------------------------------
	{
		tCHAR buff[20];
		_snprintf(buff, sizeof(buff)/sizeof(tCHAR), "%u", data);
		return buff;
	}
	tString format_as_bit_mask(const tBYTE data, const tBYTE mask)
	//------------------------------------------------------------
	{
		tCHAR buff[9] = {"........"};
		for (int i = 0; i < 8; i++)
		{
			if (mask & ((tBYTE) 1 << i))
				buff[7 - i] = data & ((tBYTE) 1 << i) ? '1' : '0';
		}
		return buff;
	}
	tString format_as_wbit_mask(const tWORD data, const tWORD mask)
	//-------------------------------------------------------------
	{
		tCHAR buff[17] = {"................"};
		for (int i = 0; i < 16; i++)
		{
			if (mask & ((tBYTE) 1 << i))
				buff[15 - i] = data & ((tBYTE) 1 << i) ? '1' : '0';
		}
		return buff;
	}
	tString format_as_z_string(const tBYTE* data)
	//-------------------------------------------
	{
		tCHAR buff[2048];
		tWORD i = 0;
		
		while (data + i < m_end && data[i] != '\0' && i < sizeof(buff)/sizeof(tCHAR))
		{
			buff[i] = data[i];
			i++;
		}
		buff[i] = '\0';

		return buff;
	}
	tString format_as_string(const tBYTE* data, tWORD length)
	//-------------------------------------------------------
	{
		tCHAR buff[2048];
		
		memcpy(buff, data, length);

		buff[length] = '\0';

		return buff;
	}
	tString format_as_dns_string(const tBYTE* data, const tBYTE* start, tWORD* length)
	//--------------------------------------------------------------------------------
	{
		tString out = "";
		tWORD i = 0;
		tBYTE* pos = (tBYTE*) data;
		tBOOL do_increment_len = true;
		*length = 0;
		
		while (pos + i < m_end && pos[i] != '\0' )
		{
			tCHAR buff[256];
			if (pos[i] & 0xC0)
			{
				tWORD offset = pos[i + 1] | ((pos[i] & 0x3F) << 8);
				pos = (tBYTE*) start + offset;
				i = 0;
				if (do_increment_len)
					*length += 2;
				do_increment_len = false;
			}
			else
			{
				memcpy(buff, pos + i + 1, pos[i]);
				buff[pos[i]] = '\0';
				out += buff;
				out += ".";
				i += pos[i] + 1;
				if (do_increment_len)
					*length = i;
			}
		}
		if (out.length() > 0)
			out.erase(out.length() - 1, 1);

		if (do_increment_len)
			(*length)++;

		return out;
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// разбираем прикладной уровень: DNS/...
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// DNS fields offsets
	#define DNS_QUERY_ID	0
	#define DNS_FLAGS		2		
	#define DNS_QDCOUNT		4
	#define DNS_ANCOUNT		6
	#define DNS_NSCOUNT		8
	#define DNS_ARCOUNT		10
	#define DNS_SIZE		12

	bool parse_dns(const tBYTE* data, tTree* tree)
	//--------------------------------------------
	{
		if (data + DNS_SIZE > m_end) return false;

		m_summary.protocol = 0xFF;
		m_summary.hi_protocol = 0x01;

		tWORD off = (tWORD) (data - m_start);
		if (tree)
		{
			add_child(tree, off + DNS_QUERY_ID, 2, NODE_QUERY_ID,	format_as_num(read_uint16(data, DNS_QUERY_ID)));
			tWORD tflags = read_uint16(data, DNS_FLAGS);
			tTree* flags = add_child(tree, off + DNS_FLAGS, 2, NODE_FLAGS, format_as_hex_word(tflags));
			add_child(flags, off + DNS_FLAGS, 2, NODE_QR,		format_as_wbit_mask(tflags, 0x8000), format_as_num((tflags & 0x8000) >> 15));
			add_child(flags, off + DNS_FLAGS, 2, NODE_OPCODE,	format_as_wbit_mask(tflags, 0x7800), format_as_num((tflags & 0x7800) >> 11));
			add_child(flags, off + DNS_FLAGS, 2, NODE_AA,		format_as_wbit_mask(tflags, 0x0400), format_as_num((tflags & 0x0400) >> 10));
			add_child(flags, off + DNS_FLAGS, 2, NODE_TRC,		format_as_wbit_mask(tflags, 0x0200), format_as_num((tflags & 0x0200) >> 9));
			add_child(flags, off + DNS_FLAGS, 2, NODE_RD,		format_as_wbit_mask(tflags, 0x0100), format_as_num((tflags & 0x0100) >> 8));
			add_child(flags, off + DNS_FLAGS, 2, NODE_RA,		format_as_wbit_mask(tflags, 0x0080), format_as_num((tflags & 0x0080) >> 7));
			add_child(flags, off + DNS_FLAGS, 2, NODE_Z,		format_as_wbit_mask(tflags, 0x0070));
			add_child(flags, off + DNS_FLAGS, 2, NODE_RCODE,	format_as_wbit_mask(tflags, 0x000F), format_as_num(tflags & 0x000F));

			tWORD qd = read_uint16(data, DNS_QDCOUNT);
			tWORD an = read_uint16(data, DNS_ANCOUNT);
			tWORD ns = read_uint16(data, DNS_NSCOUNT);
			tWORD ar = read_uint16(data, DNS_ARCOUNT);

			add_child(tree, off + DNS_QDCOUNT, 2, NODE_QDCOUNT,	format_as_num(qd));
			add_child(tree, off + DNS_ANCOUNT, 2, NODE_ANCOUNT,	format_as_num(an));
			add_child(tree, off + DNS_NSCOUNT, 2, NODE_NSCOUNT,	format_as_num(ns));
			add_child(tree, off + DNS_ARCOUNT, 2, NODE_ARCOUNT,	format_as_num(ar));

			tWORD offset = DNS_SIZE;
			for (int i = 0; i < qd; i++)
			{
				if (data + offset > m_end) return false;
				offset += add_query_node(data, offset, tree);
			}
			for (int i = 0; i < an; i++)
			{
				if (data + offset > m_end) return false;
				offset += add_answer_node(NODE_ANSECT, data, offset, tree);
			}
			for (int i = 0; i < ns; i++)
			{
				if (data + offset > m_end) return false;
				offset += add_answer_node(NODE_AUSECT, data, offset, tree);
			}
			for (int i = 0; i < ar; i++)
			{
				if (data + offset > m_end) return false;
				offset += add_answer_node(NODE_ADSECT, data, offset, tree);
			}
			set_length(tree, offset);
		}
		return true;
	}
	tWORD add_query_node(const tBYTE* data, tWORD offset, tTree* tree)
	//----------------------------------------------------------------
	{
		tWORD length;
		tWORD off = (tWORD) (data - m_start) + offset;
		tString name = format_as_dns_string(data + offset, data, &length);
		tTree* node = add_child(tree, off, length + 4, NODE_QUSECT);
		add_child(node, off, length, NODE_QUNAME, name);
		add_child(node, off + length, 2, NODE_QUTYPE, format_as_num(read_uint16(data, offset + length)));
		add_child(node, off + length + 2, 2, NODE_QCLASS, format_as_num(read_uint16(data, offset + length + 2)));
		
		return length + 4;
	}
	tWORD add_answer_node(tDWORD id, const tBYTE* data, tWORD offset, tTree* tree)
	//----------------------------------------------------------------------------
	{
		tWORD length, l2, l3;
		tString name = format_as_dns_string(data + offset, data, &length);
		tWORD off = (tWORD) (data - m_start) + offset;
		tTree* node = add_child(tree, off, 0, id);
		add_child(node, off, length, NODE_QUNAME, name);
		
		off += length;
		tWORD type = read_uint16(data + offset + length);
		add_child(node, off, 2, NODE_QUTYPE,	format_as_num(type));
		add_child(node, off + 2, 2, NODE_QCLASS,	format_as_num(read_uint16(data, offset + length + 2)));
		add_child(node, off + 4, 4, NODE_TTL,		format_as_num(read_uint32(data, offset + length + 4)));
		tWORD data_length = read_uint16(data + offset + length + 8);
		add_child(node, off + 8, 2, NODE_DATALEN,	format_as_num(data_length));

		off += 10;
		set_length(tree, length + 10 + data_length);

		tTree* child;
		tTree* child2;
		switch (type)
		{
		case 1:
			add_child(node, off, data_length, NODE_IP, format_as_ip_address(data + offset + length + 10));
			break;
		case 2:
			child = add_child(node, off, data_length, NODE_AUTH_SERVER, format_as_dns_string(data + offset + length + 10, data, &l2));
			break;
		case 3:
			child = add_child(node, off, data_length, NODE_MAIL_DEST, format_as_dns_string(data + offset + length + 10, data, &l2));
			break;
		case 4:
			child = add_child(node, off, data_length, NODE_MAIL_FRWD, format_as_dns_string(data + offset + length + 10, data, &l2));
			break;
		case 5:
			child = add_child(node, off, data_length, NODE_CNNAME, format_as_dns_string(data + offset + length + 10, data, &l2));
			break;
		case 6:
			child = add_child(node, off, data_length, NODE_SOAR);
			child2 = add_child(child, off, 0, NODE_PR_DNS, format_as_dns_string(data + offset + length + 10, data, &l2));
			set_length(child2, l2);
			child2 = add_child(child, off + l2, 0, NODE_RESP_MAIL, format_as_dns_string(data + offset + length + 10 + l2, data, &l3));
			set_length(child2, l3);
			add_child(child, off + l2 + l3, 4, NODE_VER,		format_as_num(read_uint32(data, offset + length + 10 + l2 + l3)));
			add_child(child, off + l2 + l3 + 4, 4, NODE_REFRESH,	format_as_num(read_uint32(data, offset + length + 14 + l2 + l3)));
			add_child(child, off + l2 + l3 + 8, 4, NODE_RETRY,		format_as_num(read_uint32(data, offset + length + 18 + l2 + l3)));
			add_child(child, off + l2 + l3 + 12, 4, NODE_EXP_LIMIT,	format_as_num(read_uint32(data, offset + length + 22 + l2 + l3)));
			add_child(child, off + l2 + l3 + 16, 4, NODE_MIN_TTL,	format_as_num(read_uint32(data, offset + length + 26 + l2 + l3)));
			break;
		case 7:
			add_child(node, off, data_length, NODE_MAIL_DOMN, format_as_dns_string(data + offset + length + 10, data, &l2));
			break;
		case 8:
			add_child(node, off, data_length, NODE_MAIL_GRP, format_as_dns_string(data + offset + length + 10, data, &l2));
			break;
		case 9:
			add_child(node, off, data_length, NODE_MAIL_REN, format_as_dns_string(data + offset + length + 10, data, &l2));
			break;
		case 12:
			add_child(node, off, data_length, NODE_DOMN, format_as_dns_string(data + offset + length + 10, data, &l2));
			break;
		case  10:
			add_child(node, off, data_length, NODE_NULL_RD);
			break;
		case  11:
			child = add_child(node, off, data_length, NODE_WKS);
			add_child(child, off, 4, NODE_IP, format_as_ip_address(data + offset + length + 10));
			add_child(child, off + 4, 1, NODE_PROTOCOL, format_as_num(read_uint8(data, offset + length + 14)));
			add_child(child, off + 5, data_length - 5, NODE_BIT_MAP);
			break;
		case  13:
			child = add_child(node, off, data_length, NODE_HINFO);
			l2 = read_uint8(data, offset + length + 10);
			add_child(child, off, l2, NODE_CPU, format_as_string(data + offset + length + 11, l2));
			l3 = read_uint8(data, offset + length + 11 + l2);
			add_child(child, off, l3, NODE_OS, format_as_string(data + offset + length + 12 + l2, l3));
			break;
		case  14:
			child = add_child(node, off, data_length, NODE_MINFO);
			child2 = add_child(child, off, 0, NODE_RMBOX, format_as_dns_string(data + offset + length + 10, data, &l2));
			set_length(child2, l2);
			child2 = add_child(child, off + l2, 0, NODE_EMBOX, format_as_dns_string(data + offset + length + 10 + l2, data, &l3));
			set_length(child2, l3);
			break;
		case  15:
			child = add_child(node, off, data_length, NODE_MEXCH);
			add_child(child, off, 2, NODE_PREF, format_as_num(read_uint16(data, offset + length + 10)));
			child2 = add_child(child, off + 2, 0, NODE_EXCH_MBOX, format_as_dns_string(data + offset + length + 12, data, &l2));
			set_length(child2, l2);
			break;
		case  16:
			add_child(node, off, data_length, NODE_TEXT);
			break;
		case 28:
			add_child(node, off, data_length, NODE_IP, format_as_ipv6_address(data + offset + length + 10));
			break;
		}

		return length + 10 + data_length;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// разбираем транспортный уровень: TCP/UDP/ICMP
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// fields offsets
	#define UDP_SRC_PORT	0
	#define UDP_DST_PORT	2
	#define UDP_LEN			4
	#define UDP_CHKSUM		6
	#define UDP_SIZE		8
	//-------------------------
	#define	TCP_SRC_PORT	0
	#define	TCP_DST_PORT	2
	#define	TCP_SEQ_NUM		4
	#define	TCP_ACK_NUM		8
	#define	TCP_DATA_OFF	12
	#define	TCP_FLAGS		13
	#define	TCP_WINDOW		14
	#define	TCP_CHKSUM		16
	#define	TCP_URG_PTR		18
	#define TCP_SIZE		20
	//-------------------------
	#define	ICMP_TYPE		0
	#define	ICMP_CODE		1
	#define	ICMP_CHKSUM		2
	#define	ICMP_ID2		4
	#define	ICMP_SEQUENCE	6
	#define ICMP_SIZE		8
	//-------------------------
	#define NBTNS_ID		0
	#define NBTNS_FLAGS		2		
	#define NBTNS_QDCOUNT	4
	#define NBTNS_ANCOUNT	6
	#define NBTNS_NSCOUNT	8
	#define NBTNS_ARCOUNT	10
	#define NBTNS_SIZE		12
	//-------------------------
	#define DHCP_M_TYPE		0
	#define DHCP_H_TYPE		1
	#define DHCP_AD_LEN		2
	#define DHCP_H_COUNT	3
	#define DHCP_TR_ID		4
	#define DHCP_SECONDS	8
	#define DHCP_FLAG		10
	#define DHCP_CIP		12
	#define DHCP_YIP		16
	#define DHCP_SIP		20
	#define DHCP_RAIP		24
	#define DHSP_H_ADDR		28
	#define DHSP_HOST_NAME	44
	#define DHSP_BOOT_FILE	108
	#define DHSP_MAGIC		236		
	#define DHCP_SIZE		240

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool parse_dhcp(const tBYTE* data, tTree* tree) 
	//---------------------------------------------
	{
		if (data + DHCP_SIZE > m_end) return false;

		m_summary.protocol = 0xFF;
		m_summary.hi_protocol = 0x05;

		tWORD off = (tWORD) (data - m_start);
		if (tree)
		{
			set_length(tree, DHCP_SIZE);
			add_child(tree, off + DHCP_M_TYPE,	1, NODE_M_TYPE,	format_as_hex_byte(read_uint8(data, DHCP_M_TYPE)));
			add_child(tree, off + DHCP_H_TYPE,	1, NODE_H_TYPE,	format_as_hex_byte(read_uint8(data, DHCP_H_TYPE)));
			add_child(tree, off + DHCP_AD_LEN,	1, NODE_AD_LEN,	format_as_hex_byte(read_uint8(data, DHCP_AD_LEN)));
			add_child(tree, off + DHCP_H_COUNT,	1, NODE_HOPE_COUNT,format_as_hex_byte(read_uint8(data, DHCP_H_COUNT)));
			add_child(tree, off + DHCP_TR_ID,	4, NODE_TRANS_ID,	format_as_hex(data + DHCP_TR_ID, 4));
			add_child(tree, off + DHCP_SECONDS, 2, NODE_SECONDS,	format_as_num(read_uint16(data, DHCP_SECONDS)));
			tWORD flag = read_uint16(data, DHCP_FLAG);
			add_child(tree, off + DHCP_FLAG,	2, NODE_FLAG,	format_as_wbit_mask(flag, 0x8000), format_as_num((flag & 0x8000) >> 15));
			add_child(tree, off + DHCP_CIP,		4, NODE_CIP,	format_as_ip_address(data + DHCP_CIP));
			add_child(tree, off + DHCP_YIP,		4, NODE_YIP,	format_as_ip_address(data + DHCP_YIP));
			add_child(tree, off + DHCP_SIP,		4, NODE_SIP,	format_as_ip_address(data + DHCP_SIP));
			add_child(tree, off + DHCP_RAIP,	4, NODE_RAIP,	format_as_ip_address(data + DHCP_RAIP));

			if (read_uint8(data, DHCP_H_TYPE) == 1 && read_uint8(data, DHCP_AD_LEN) == 6)
				add_child(tree, off + DHSP_H_ADDR, 6, NODE_C_MAC, format_as_mac_address(data + DHSP_H_ADDR));
			else
				add_child(tree, off + DHSP_H_ADDR, 16, NODE_H_ADDR, format_as_hex(data + DHSP_H_ADDR, 16));

//>>>>>>>>>>>TODO>>>> что это за имена?
			add_child(tree, off + DHSP_HOST_NAME, 64, NODE_HOST_NAME);
			add_child(tree, off + DHSP_BOOT_FILE, 128, NODE_BOOT_FILE);
//>>>>>>>>>>>			
			add_child(tree, off + DHSP_MAGIC, 4, NODE_MAGIC,	format_as_hex(data + DHSP_MAGIC, 4));

			off += DHCP_SIZE;
			tBYTE off2;
			tBYTE* offset = (tBYTE*) data + DHCP_SIZE;
			tBYTE* offset_old = offset - 2;
			while (offset <= m_end && *offset != 0xFF)
			{
				off += (tWORD) (offset - offset_old) - 2;
				offset_old = offset;

				tBYTE value = *offset;
				tTree* option = add_child(tree, off, *(++offset) + 2, NODE_DHCP_OPTION, format_as_num(value));
				//off++;
				if (value != 0)
				{
					add_child(option, off + 1, 1, NODE_LEN, format_as_num(*offset));
				}
				off += 2;
//>>>>>>>>>>>			
#ifdef _DEBUG
				tString _trace = "DHCP option: ";
				_trace += format_as_num(value);
				_trace += " Length: ";
				_trace += format_as_num(read_uint8(offset));
				_trace += "\n";
				OutputDebugString((LPCTSTR) _trace.c_str(0));
#endif
//>>>>>>>>>>>			
				tBYTE count = 0;
				tString str = "";
				tBYTE val = 0;
				switch (value)
				{
				case 0:
					while (*(offset + count) == 0x00 && offset + count <= m_end)
					{
						count++;
					}
					add_child(option, off - 2, count, NODE_LEN, format_as_num(count));
					offset += count;
					break;
				case 1:
					add_child(option, off, 4, NODE_VALUE, format_as_ip_address(offset));
					offset += 4;
					break;
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 16:
				case 28:
				case 32:
				case 41:
				case 42:
				case 44:
				case 45:
				case 48:
				case 49:
				case 50:
				case 54:
				case 65:
				case 68:
				case 69:
				case 70:
				case 71:
				case 72:
				case 73:
				case 74:
				case 75:
				case 76:
				case 85:
				case 118:
					count = read_uint8(offset++)/4;
					for (tBYTE i = 0; i < count; i++)
					{
						add_child(option, off + 4*i, 4, NODE_IP, format_as_ip_address(offset));
						offset += 4;
					}
					break;
				case 12:
				case 14:
				case 15:
				case 17:
				case 18:
				case 40:
				case 47:
				case 56:
				case 62:
				case 64:
				case 66:
				case 67:
				case 77:
				case 117:
					count = read_uint8(offset);
					add_child(option, off, count, NODE_VALUE, format_as_string(offset + 1, count));
					offset += count + 1;
					break;
				case 13:
					add_child(option, off, 2, NODE_VALUE, format_as_num(read_uint16(offset, 1)));
					offset += read_uint8(offset) + 1;
					break;
				case 19:
				case 20:
				case 27:
				case 29:
				case 30:
				case 31:
				case 34:
				case 36:
				case 39:
					add_child(option, off, 1, NODE_BOOL, format_as_num(*(++offset)));
					offset++;
					break;
				case 24:
				case 35:
				case 38:
				case 51:
				case 58:
				case 59:
					add_child(option, off, 4, NODE_VALUE, format_as_num(read_uint32(offset, 1)));
					offset += 5;
					break;
				case 21:
					count = read_uint8(offset++)/8;
					for (tBYTE i = 0; i < count; i++)
					{
						add_child(option, off + i*8, 8, NODE_IP_MASK, format_as_ip_address(offset), format_as_ip_address(offset + 4));
						offset += 8;
					}
					break;
				case 22:
				case 26:
				case 57:
					add_child(option, off, 2, NODE_VALUE, format_as_num(read_uint16(offset, 1)));
					offset += 3;
					break;
				case 23:
				case 37:
					add_child(option, off, 1, NODE_VALUE, format_as_num(read_uint8(offset, 1)));
					offset += 2;
					break;
				case 25:
					count = read_uint8(offset++)/2;
					for (tBYTE i = 0; i < count; i++)
					{
						str += format_as_num(read_uint16(offset));
						str += " ";
						offset += 2;
					}
					add_child(option, off, 2*count, NODE_VALUE, str);
					break;
				case 33:
					count = read_uint8(offset++)/8;
					for (tBYTE i = 0; i < count; i++)
					{
						add_child(option, off + i*8, 8, NODE_DST_ROUTER, format_as_ip_address(offset), format_as_ip_address(offset + 4));
						offset += 8;
					}
					break;
				case 46:
					add_child(option, off, 1, NODE_F_TYPE, format_as_hex_byte(read_uint8(offset, 1)));
					offset +=2;
					break;
				case 52:
					add_child(option, off, 1, NODE_OVERLOADS, format_as_num(read_uint8(offset, 1)));
					offset += 2;
//>>>>>>>>>>>TODO>>>> пройтись по дополнительным опциям
					break;
				case 53:
					add_child(option, off, 1, NODE_M_TYPE2, format_as_num(read_uint8(offset, 1)));
					offset +=2;
					break;
				case 55:
					count = read_uint8(offset++);
					for (tBYTE i = 0; i < count; i++)
					{
						add_child(option, off + i, 1, NODE_OPTION, format_as_num(*(offset++)));
					}
					break;
				case 61:
				case 97:
					count = read_uint8(offset);
					add_child(option, off, 1, NODE_H_TYPE, format_as_num(*(offset + 1)));
					add_child(option, off + 1, count - 1, NODE_VALUE, format_as_hex(offset + 2, count - 1));
					offset += count + 1;
					break;
				case 78:
					add_child(option, off, 1, NODE_SLPD, format_as_hex_byte(read_uint8(offset, 1)));
					count = (read_uint8(offset) - 1)/4;
					offset += 2;
					for (tBYTE i = 0; i < count; i++)
					{
						add_child(option, off + i*4 + 1, 4, NODE_IP, format_as_ip_address(offset));
						offset += 4;
					}
					break;
				case 79:
					add_child(option, off, 1, NODE_SLPA, format_as_hex_byte(read_uint8(offset, 1)));
					str = "";
					count = read_uint8(offset) - 1;
					val = count;
					if (count > 0)
					{
						utf8_to_utf16((const char*) offset + 2,  count, str);
						add_child(option, off + 1, val, NODE_VALUE, str);
					}
					offset += count + 2;
					break;
				case 63:
					off2 = 1;
					count = read_uint8(offset++);
					while (count > 0 && offset <= m_end)
					{
						switch (*offset)
						{
						case 1:
						case 2:
						case 3:
						case 4:
							add_child(option, off + off2, 1, NODE_NWIP, format_as_num(read_uint8(offset)));
							count -= 2;
							offset += 2;
							off2 += 2;
							break;
						case 5:
						case 8:
						case 9:
						case 10:
							add_child(option, off + off2, 2, NODE_NWIP, format_as_num(read_uint8(offset)), format_as_num(read_uint8(offset, 2)));
							count -= 3;
							offset += 3;
							off2 += 3;
							break;
						case 6:
						case 7:
							str = "";
							val = read_uint8(offset, 2)/4; 
							for (tBYTE i = 0; i < val; i++)
							{
								str += format_as_ip_address(offset + 2 + 4*i);
								if (val != i + 1)
									str += ", ";
							}
							add_child(option, off + off2, read_uint8(offset, 1) + 1, NODE_NWIP, format_as_num(read_uint8(offset)), str);
							count -= (read_uint8(offset, 1) + 2);
							off2 += read_uint8(offset, 1) + 2;
							offset += read_uint8(offset, 1) + 2;
							break;
						case 11:
							add_child(option, off + off2, 5, NODE_NWIP, format_as_num(read_uint8(offset)), format_as_ip_address(offset + 2));
							count -= 6;
							offset += 6;
							off2 += 6;
							break;
						default:
							add_child(option, off + off2, read_uint8(offset, 1) + 1, NODE_NWIP, format_as_num(read_uint8(offset)));
							count -= (read_uint8(offset, 1) + 2);
							off2 += read_uint8(offset, 1) + 2;
							offset += read_uint8(offset, 1) + 2;
							break;
						}
					}
					break;
				case 81:
					count = read_uint8(offset);
					add_child(option, off, 1, NODE_FLAGS, format_as_hex_byte(*(offset + 1)));
					add_child(option, off + 1, 1, NODE_RCODE1, format_as_hex_byte(*(offset + 2)));
					add_child(option, off + 2, 1, NODE_RCODE2, format_as_hex_byte(*(offset + 3)));
					if (count > 3)
						add_child(option, off + 3, count - 3, NODE_DN, format_as_string(offset + 4, count - 3));
					offset += count + 1;
					break;

				case 43:
				case 60:
				default:
					add_child(option, off, read_uint8(offset), NODE_VALUE, format_as_hex(offset + 1, read_uint8(offset)));
					offset += read_uint8(offset) + 1;
					break;
				}
			}
//>>>>>>>>>>>			
#ifdef _DEBUG
				tString _trace = "Last byte: ";
				_trace += format_as_hex_byte(*offset);
				_trace += "\n";
				OutputDebugString((LPCTSTR) _trace.c_str(0));
#endif
//>>>>>>>>>>>			

//>>>>>>>>>>>TODO>>>>
/*
			case  82:DHCPOptionsRelayAgentInfo RelayAgentInfo;
			case  86:DHCPNDSTreeName NDSTreeName;
			case  87:DHCPNDSContext NDSContext;
			case  90:DHCPAuthentication Authentication;
			case  98:DHCPOpenGroupUserAuthentication OpenGroupUserAuthentication;
			case 116:DHCPOptionsAutoConfigure AutoConfigure;
			case 119:DHCPOptionsGeneral DomainSearch;
			case 120:DHCPSIPServers SIPservers;
			case 121:DHCPClasslessStaticRoute ClasslessStaticRoute;
			case 122:DHCPCableLabsClientConfiguration CableLabsClientConfiguration;
			case 123:DHCPLocationConfigurationInformation LocationConfigurationInformation;
			case 124:DHCPVendorIdentifyingVendorClass VendorIdentifyingVendorClass;
			case 125:DHCPVendorIdentifyingVendorSpecific VendorIdentifyingVendorSpecific;
			case 131: DHCPOptionRemotestatisticsserverIPAddress RemotestatisticsserverIPAddress;
			case 132: DHCPOptionVLANID VLANID;
			case 133: DHCPOptionL2Priority L2Priority;
			case 134: DHCPOptionDiffservCodePoint DiffservCodePoint;
			case 135: DHCPOptionHTTPProxyForPhoneSpecApps HTTPProxyForPhoneSpecApps;
			case 175: DHCPOptionEtherboot Etherboot;
			case 176: DHCPOptionIPTelephone IPTelephone;
			case 208: DHCPOptionsPxelinuxMagic PxelinuxMagic;
			case 209: DHCPOptionsPxelinuxConfigfile PxelinuxConfigfile;
			case 210: DHCPOptionsPxelinuxPathprefix PxelinuxPathprefix;
			case 211: DHCPOptionsPxelinuxReboottime PxelinuxReboottime;
			case 220: DHCPOptionsSubnetAllocation SubnetAllocation;
			case 221: DHCPOptionsVirtualSubnetSelection VirtualSubnetSelection;
			case 252: DHCPOptionsWpad WPAD;
*/
//>>>>>>>>>>>			

		}

		return true;
	}
//>>>>>>>>>>>TODO>>>>
	bool parse_dhcpv6_client(const tBYTE* data, tTree* tree) 
	//------------------------------------------------------
	{
		return true;
	}
//>>>>>>>>>>>TODO>>>>
	bool parse_dhcpv6_server(const tBYTE* data, tTree* tree) 
	//------------------------------------------------------
	{
		return true;
	}
	bool parse_nbtns(const tBYTE* data, tTree* tree) 
	//----------------------------------------------
	{
		if (data + NBTNS_SIZE > m_end) return false;

		m_summary.protocol = 0xFF;
		m_summary.hi_protocol = 0x04;

		tWORD off = (tWORD) (data - m_start);
		if (tree)
		{
			add_child(tree, off + NBTNS_ID, 2, NODE_TRANS_ID,	format_as_num(read_uint16(data, NBTNS_ID)));
			tWORD tflags = read_uint16(data, NBTNS_FLAGS);
			tTree* flags = add_child(tree, off + NBTNS_FLAGS, 2, NODE_FLAGS, format_as_hex_word(tflags));
			add_child(flags, off + NBTNS_FLAGS, 2, NODE_R,		format_as_wbit_mask(tflags, 0x8000), format_as_num((tflags & 0x8000) >> 15));
			add_child(flags, off + NBTNS_FLAGS, 2, NODE_OPCODE_2,	format_as_wbit_mask(tflags, 0x7800), format_as_num((tflags & 0x7800) >> 11));
			add_child(flags, off + NBTNS_FLAGS, 2, NODE_AA,		format_as_wbit_mask(tflags, 0x0400), format_as_num((tflags & 0x0400) >> 10));
			add_child(flags, off + NBTNS_FLAGS, 2, NODE_TRC,		format_as_wbit_mask(tflags, 0x0200), format_as_num((tflags & 0x0200) >> 9));
			add_child(flags, off + NBTNS_FLAGS, 2, NODE_RD,		format_as_wbit_mask(tflags, 0x0100), format_as_num((tflags & 0x0100) >> 8));
			add_child(flags, off + NBTNS_FLAGS, 2, NODE_RA,		format_as_wbit_mask(tflags, 0x0080), format_as_num((tflags & 0x0080) >> 7));
			add_child(flags, off + NBTNS_FLAGS, 2, NODE_RESERVED,	format_as_wbit_mask(tflags, 0x0060));
			add_child(flags, off + NBTNS_FLAGS, 2, NODE_B,		format_as_wbit_mask(tflags, 0x0010), format_as_num((tflags & 0x0010) >> 4));
			add_child(flags, off + NBTNS_FLAGS, 2, NODE_RCODE_2,	format_as_wbit_mask(tflags, 0x000F), format_as_num(tflags & 0x000F));

			tWORD qd = read_uint16(data, NBTNS_QDCOUNT);
			tWORD an = read_uint16(data, NBTNS_ANCOUNT);
			tWORD ns = read_uint16(data, NBTNS_NSCOUNT);
			tWORD ar = read_uint16(data, NBTNS_ARCOUNT);

			add_child(tree, off + NBTNS_QDCOUNT, 2, NODE_QDCOUNT,	format_as_num(qd));
			add_child(tree, off + NBTNS_ANCOUNT, 2, NODE_ANCOUNT,	format_as_num(an));
			add_child(tree, off + NBTNS_NSCOUNT, 2, NODE_NSCOUNT,	format_as_num(ns));
			add_child(tree, off + NBTNS_ARCOUNT, 2, NODE_ARCOUNT,	format_as_num(ar));

			tWORD offset = NBTNS_SIZE;
			for (int i = 0; i < qd; i++)
			{
				if (data + offset > m_end) return false;

				tWORD length;
				tString name = format_as_dns_string(data + offset, data, &length);
				tString temp = decode_nbns_name(name);

				tTree* node = add_child(tree, off + offset, 0, NODE_QUSECT);
				if (temp == "")
					add_child(node, off + offset, length, NODE_ILLEGAL_NBNAME);
				else
					add_child(node, off + offset, length, NODE_QUNAME, temp);

				add_child(node, off + offset + length, 2, NODE_QUTYPE_2, format_as_num(read_uint16(data, offset + length)));
				add_child(node, off + offset + length + 2, 2, NODE_QCLASS_2, format_as_num(read_uint16(data, offset + length + 2)));

				set_length(node, length + 4);

				offset += length + 4;
			}
//>>>>>>>>>>>TODO>>>> структура ответов?
			for (int i = 0; i < an; i++)
			{
				tTree* node = add_child(tree, off + offset, 0, NODE_ANSECT);
			}
			for (int i = 0; i < ns; i++)
			{
				tTree* node = add_child(tree, off + offset, 0, NODE_AUSECT);
			}
			for (int i = 0; i < ar; i++)
			{
				tTree* node = add_child(tree, off + offset, 0, NODE_ADSECT);
			}
			set_length(tree, offset);
		}

		return true;
	}
	tString decode_nbns_name(tString name)
	//------------------------------------
	{
		tString out = "";

		tBYTE ln2 = name.length()/2;
		
		if (2*ln2 != name.length()) return "";

		for (int i = 0; i < ln2; i++)
		{
			if (name[2*i] < 'A' || name[2*i] > 'Z' || name[2*i + 1] < 'A' || name[2*i + 1] > 'Z') return "";
			tCHAR ch = ((name[2*i] - 'A') << 4) | (name[2*i + 1] - 'A');
//>>>>>>>>>>>TODO>>>> а может быть nbns-name с пробелами?
			if (ch == ' ')
				break;

			out += ch;
		}

		return out;
	}

//>>>>>>>>>>>TODO>>>>
	bool parse_nbtds(const tBYTE* data, tTree* tree) 
	//----------------------------------------------
	{
		return true;
	}
//>>>>>>>>>>>TODO>>>>
	bool parse_nbtss(const tBYTE* data, tTree* tree) 
	//----------------------------------------------
	{
		return true;
	}
//>>>>>>>>>>>TODO>>>>
	bool parse_icmpv6(const tBYTE* data, tTree* tree) 
	//-----------------------------------------------
	{
		return true;
	}
	bool parse_icmp(const tBYTE* data, tTree* tree) 
	//---------------------------------------------
	{
		if (data + ICMP_SIZE > m_end) return false;

		tWORD off = (tWORD) (data - m_start);
		if (tree)
		{
			set_length(tree, ICMP_SIZE);
			add_child(tree, off + ICMP_TYPE,		1,	NODE_TYPE,		format_as_num(read_uint8(data, ICMP_TYPE)));
			add_child(tree, off + ICMP_CODE,		1,	NODE_CODE,		format_as_num(read_uint8(data, ICMP_CODE)));
			add_child(tree, off + ICMP_CHKSUM,	2,	NODE_CHKSUM,	format_as_hex_word(read_uint16(data, ICMP_CHKSUM)));
			add_child(tree, off + ICMP_ID2,		2,	NODE_ID2,		format_as_num(read_uint16(data, ICMP_ID2)));
			add_child(tree, off + ICMP_SEQUENCE,	2,	NODE_SEQUENCE,	format_as_num(read_uint16(data, ICMP_SEQUENCE)));
		}

		return true;
	}
	bool parse_tcp(const tBYTE* data, tTree* tree) 
	//--------------------------------------------
	{
		if (data + TCP_SIZE > m_end) return false;
		
		tWORD off = (tWORD) (data - m_start);
		if (tree)
		{
			set_length(tree, TCP_SIZE);
			add_child(tree, off + TCP_SRC_PORT,	2, NODE_SRC_PORT,	format_as_num(read_uint16(data, TCP_SRC_PORT)));
			add_child(tree, off + TCP_DST_PORT,	2, NODE_DST_PORT,	format_as_num(read_uint16(data, TCP_DST_PORT)));
			add_child(tree, off + TCP_SEQ_NUM,	4, NODE_SEQ_NUM,	format_as_num(read_uint32(data, TCP_SEQ_NUM)));
			add_child(tree, off + TCP_ACK_NUM,	4, NODE_ACK_NUM,	format_as_num(read_uint32(data, TCP_ACK_NUM)));
			add_child(tree, off + TCP_DATA_OFF,	1, NODE_DATA_OFF,	format_as_num((read_uint8(data, TCP_DATA_OFF)  & 0xF0) >> 2));
			
			tBYTE  tflags = read_uint8(data, TCP_FLAGS);
			tTree* flags = add_child(tree, off + TCP_FLAGS, 1, NODE_FLAGS,	format_as_hex_byte(tflags));
			add_child(flags, off + TCP_FLAGS, 1, NODE_CWR,		format_as_bit_mask(tflags, 0x80));
			add_child(flags, off + TCP_FLAGS, 1, NODE_ECN_ECHO,	format_as_bit_mask(tflags, 0x40));
			add_child(flags, off + TCP_FLAGS, 1, NODE_URG,		format_as_bit_mask(tflags, 0x20));
			add_child(flags, off + TCP_FLAGS, 1, NODE_ACK,		format_as_bit_mask(tflags, 0x10));
			add_child(flags, off + TCP_FLAGS, 1, NODE_PSH,		format_as_bit_mask(tflags, 0x08));
			add_child(flags, off + TCP_FLAGS, 1, NODE_RST,		format_as_bit_mask(tflags, 0x04));
			add_child(flags, off + TCP_FLAGS, 1, NODE_SYN,		format_as_bit_mask(tflags, 0x02));
			add_child(flags, off + TCP_FLAGS, 1, NODE_FIN,		format_as_bit_mask(tflags, 0x01));

			add_child(tree, off + TCP_WINDOW,		2, NODE_WINDOW,	format_as_num(read_uint16(data, TCP_WINDOW)));
			add_child(tree, off + TCP_CHKSUM,		2, NODE_CHKSUM,	format_as_hex_word(read_uint16(data, TCP_CHKSUM)));
			add_child(tree, off + TCP_URG_PTR,	2, NODE_URG_PTR,	format_as_num(read_uint16(data, TCP_URG_PTR)));
		}
		return true;
	}
	bool parse_udp(const tBYTE* data, tTree* tree)
	//--------------------------------------------
	{
		if (data + UDP_SIZE > m_end) return false;
		
		tWORD length = read_uint16(data, UDP_LEN);
		tWORD dst_port = read_uint16(data, UDP_DST_PORT);
		tWORD src_port = read_uint16(data, UDP_SRC_PORT);

		tWORD off = (tWORD) (data - m_start);
		if (tree)
		{
			set_length(tree, UDP_SIZE);
			add_child(tree, off + UDP_SRC_PORT,	2, NODE_SRC_PORT,	format_as_num(src_port));
			add_child(tree, off + UDP_DST_PORT,	2, NODE_DST_PORT,	format_as_num(dst_port));
			add_child(tree, off + UDP_LEN,		2, NODE_LEN,		format_as_num(length));
			add_child(tree, off + UDP_CHKSUM,		2, NODE_CHKSUM,	format_as_num(read_uint16(data, UDP_CHKSUM)));
			add_child(tree, off + UDP_SIZE, length - UDP_SIZE, NODE_DATA,		format_as_num(length - UDP_SIZE));

		}
		
		if (src_port == 53 || dst_port == 53)
			return parse_dns(data + UDP_SIZE, add_child(m_root, off + UDP_SIZE, 0, NODE_DNS));

		if (dst_port == 67 || src_port == 68 || dst_port == 68 || src_port == 67)
			return parse_dhcp(data + UDP_SIZE, add_child(m_root, off + UDP_SIZE, DHCP_SIZE, NODE_DHCP));
		
		if (dst_port == 137)
			return parse_nbtns(data + UDP_SIZE, add_child(m_root, off + UDP_SIZE, NBTNS_SIZE, NODE_NBNS));

//>>>>>>>>>>>TODO>>>> есть еще масса протоколов, использующих UDP как транспорт...
/*

		if (dst_port == 138)
			return parse_nbtds(data + UDP_SIZE, add_child(m_root, 0, 0, NODE_???));

		if (dst_port == 139)
			return parse_nbtss(data + UDP_SIZE, add_child(m_root, 0, 0, NODE_???));

		if (dst_port == 546)
			return parse_dhcpv6_client(data + UDP_SIZE, add_child(m_root, 0, 0, NODE_???));

		if (dst_port == 547)
			return parse_dhcpv6_server(data + UDP_SIZE, add_child(m_root, 0, 0, NODE_???s));
*/
		return true;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// разбираем сетевой уровень: IP/ARP
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// IPv4 fields offsets
	#define IPV4_VER_HL	0	// version and header length
	#define IPV4_DS		1	// differentiated services
	#define IPV4_LENGTH	2	// total length
	#define IPV4_ID		4	// identification
	#define IPV4_FL_OFF	6	// flags and fragment offset
	#define IPV4_TTL	8	// time to live
	#define IPV4_PROTO	9	// next protocol
	#define IPV4_CHK	10	// header check sum
	#define IPV4_SRC	12	// source ip address
	#define IPV4_DST	16	// destination ip address
	#define	IPV4_SIZE	20	// ip structure size (minimum if no options and data sections)

	// IPv6 fields offsets
	#define IPV6_VER	0	// version, traffic class and flow label
	#define IPV6_PL		4	// payload length
	#define IPV6_NH		6	// next header
	#define IPV6_HL		7	// hop limit
	#define IPV6_SRC	8	// source ip address
	#define IPV6_DST	24	// destination ip address
	#define	IPV6_SIZE	40	// ip structure size (minimum if no options and data sections)


	bool parse_ipv4(const tBYTE* data, tTree* tree) 
	//---------------------------------------------
	{
		if (data + IPV4_SIZE > m_end) return false;

		m_summary.is_ipv4 = true;
		m_summary.ip_source_address.m_Zone = read_uint32(data, IPV4_SRC);
		m_summary.ip_destination_address.m_Zone = read_uint32(data, IPV4_DST);
		m_summary.protocol = read_uint8(data, IPV4_PROTO);

		tWORD off = (tWORD) (data - m_start);
		if (tree)
		{
			tBYTE ver_hl = read_uint8(data, IPV4_VER_HL);

			add_child(tree, off + IPV4_VER_HL, 1,	NODE_VERSION,		format_as_num((ver_hl & 0xF0) >> 4));
			add_child(tree, off + IPV4_VER_HL, 1,	NODE_HEADER_LEN,	format_as_num((ver_hl & 0x0F) << 2));
			add_child(tree, off + IPV4_DS, 1,		NODE_DSF,			format_as_bit_mask(read_uint8(data, IPV4_DS), 0xFF));
			add_child(tree, off + IPV4_LENGTH, 2,	NODE_TL,			format_as_num(read_uint16(data, IPV4_LENGTH)));
			add_child(tree, off + IPV4_ID, 2,		NODE_ID,			format_as_num(read_uint16(data, IPV4_ID)));
			add_child(tree, off + IPV4_FL_OFF, 2,	NODE_FLAGS,			format_as_bit_mask(read_uint8(data, IPV4_FL_OFF), 0xE0));
			add_child(tree, off + IPV4_FL_OFF, 2,	NODE_OFFSET,		format_as_num(read_uint16(data, IPV4_FL_OFF) & 0x1FFF));
			add_child(tree, off + IPV4_TTL, 1,		NODE_TTL,			format_as_num(read_uint8(data, IPV4_TTL)));
			add_child(tree, off + IPV4_PROTO, 1,	NODE_PROTO,			format_as_num(m_summary.protocol));
			add_child(tree, off + IPV4_CHK, 2,		NODE_CHECKSUM,		format_as_num(read_uint16(data, IPV4_CHK)));
			add_child(tree, off + IPV4_SRC, 4,		NODE_SRC_IP,		format_as_ip_address(data + IPV4_SRC));
			add_child(tree, off + IPV4_DST, 4,		NODE_DST_IP,		format_as_ip_address(data + IPV4_DST));
			
		}

//>>>>>>>>>>>TODO>>>> !!! - будет сбиваться, если в конце есть поля OPTIONS и DATA
		tWORD offset = IPV4_SIZE;
		switch (m_summary.protocol)
		{
		case 0x01:
			return parse_icmp(data + offset, add_child(m_root, offset + off, 0, NODE_ICMP));
		case 0x06:
			return parse_tcp(data + offset, add_child(m_root, offset + off, 0, NODE_TCP));
		case 0x11:
			return parse_udp(data + offset, add_child(m_root, offset + off, 0, NODE_UDP));
		}

		return true;
	}
	bool parse_ipv6(const tBYTE* data, tTree* tree) 
	//---------------------------------------------
	{
		if (data + IPV6_SIZE > m_end) return false;

		m_summary.is_ipv4 = false;
		m_summary.ip_source_address.m_Hi = read_uint64(data, IPV6_SRC);
		m_summary.ip_source_address.m_Lo = read_uint64(data, IPV6_SRC + 8);
		m_summary.ip_destination_address.m_Hi = read_uint64(data, IPV6_DST);
		m_summary.ip_destination_address.m_Lo = read_uint64(data, IPV6_DST + 8);
		m_summary.protocol = read_uint8(data, IPV6_NH);

		tWORD off = (tWORD) (data - m_start);
		if (tree)
		{
			tBYTE ver = read_uint8(data, IPV6_VER);
			tWORD tc = read_uint16(data, IPV6_VER);
			add_child(tree, off + IPV6_VER, 1, NODE_VERSION,	format_as_num((ver & 0xF0) >> 4));
			add_child(tree, off + IPV6_VER, 2, NODE_TC,		format_as_bit_mask((tc & 0x0FF0) >> 4, 0xFF));
			
			tString flow_label = format_as_bit_mask(tc & 0x000F, 0x0F);
			flow_label.erase(0, 4);
			flow_label.append(format_as_bit_mask(read_uint8(data, IPV6_VER + 2), 0xFF));
			flow_label.append(format_as_bit_mask(read_uint8(data, IPV6_VER + 3), 0xFF));
			add_child(tree, off + IPV6_VER + 1, 3,	NODE_FL,		flow_label);
			add_child(tree, off + IPV6_PL, 2,		NODE_PAYL,		format_as_num(read_uint16(data, IPV6_PL)));
			add_child(tree, off + IPV6_NH, 1,		NODE_NEXTH,		format_as_num(m_summary.protocol));
			add_child(tree, off + IPV6_HL, 1,		NODE_HOPL,		format_as_num(read_uint8(data, IPV6_HL)));
			add_child(tree, off + IPV6_SRC, 16,		NODE_SRC_IP,	format_as_ipv6_address(data + IPV6_SRC));
			add_child(tree, off + IPV6_DST, 16,		NODE_DST_IP,	format_as_ipv6_address(data + IPV6_DST));

		}

		tWORD offset = IPV6_SIZE;
		switch (m_summary.protocol)
		{
		case 0x3A:
			return parse_icmpv6(data + offset, add_child(m_root, offset + off, 0, NODE_ICMP));
		case 0x06:
			return parse_tcp(data + offset, add_child(m_root, offset + off, 0, NODE_TCP));
		case 0x11:
			return parse_udp(data + offset, add_child(m_root, offset + off, 0, NODE_UDP));
		}

		return true;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ARP-parser
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// ARP packet fields offsets
	#define	ARP_HT		0	// hardware type
	#define	ARP_PT		2	// protocol type
	#define	ARP_HL		4	// hardware address length
	#define	ARP_PL		5	// protocol address length
	#define	ARP_OP		6	// operation	
	#define	ARP_SHA		8	// sender hardware address

	bool parse_arp(const tBYTE* data, tTree* tree)
	//--------------------------------------------
	{
		if (data + ARP_OP > m_end) return false;

		tBYTE hardware_length = read_uint8(data, ARP_HL);
		tBYTE protocol_length = read_uint8(data, ARP_PL);

		m_summary.protocol = 0xFF;
		m_summary.hi_protocol = 0x00;

		if (data + ARP_SHA + 2*hardware_length + 2*protocol_length > m_end) return false;

		tWORD off = (tWORD) (data - m_start);
		if (tree)
		{
			set_length(tree, ARP_SHA + 2*hardware_length + 2*protocol_length);
			add_child(tree, off + ARP_HT, 2, NODE_HTYPE,	format_as_hex_word(read_uint16(data, ARP_HT)));
			add_child(tree, off + ARP_PT, 2, NODE_PTYPE,	format_as_hex_word(read_uint16(data, ARP_PT)));
			add_child(tree, off + ARP_HL, 1, NODE_HLEN,	format_as_num(read_uint8(data, ARP_HL)));
			add_child(tree, off + ARP_PL, 1, NODE_PLEN,	format_as_num(read_uint8(data, ARP_PL)));
			add_child(tree, off + ARP_OP, 2, NODE_OPER,	format_as_hex_word(read_uint16(data, ARP_OP)));
			add_child(tree, off + ARP_SHA,6, NODE_SHA,	format_as_mac_address(data + ARP_SHA));
		}
		// ipv4
		if (protocol_length == 4)
		{
			m_summary.is_ipv4 = true;
			m_summary.ip_source_address.m_Zone = read_uint32(data, ARP_SHA + hardware_length);
			m_summary.ip_destination_address.m_Zone = read_uint32(data, ARP_SHA + protocol_length + 2*hardware_length);
			
			if (tree)
			{
				add_child(tree, off + ARP_SHA + hardware_length, 4, NODE_SPA,	format_as_ip_address(data + ARP_SHA + hardware_length));
				add_child(tree, off + ARP_SHA + hardware_length + protocol_length, 6, NODE_THA,	format_as_mac_address(data + ARP_SHA + hardware_length + protocol_length));
				add_child(tree, off + ARP_SHA + 2*hardware_length + protocol_length, 4, NODE_TPA,	format_as_ip_address(data + ARP_SHA + 2*hardware_length + protocol_length));
			}
		}
		// ipv6
		if (protocol_length == 16)
		{
			m_summary.is_ipv4 = false;
			m_summary.ip_source_address.m_Hi = read_uint64(data, ARP_SHA + hardware_length);
			m_summary.ip_source_address.m_Lo = read_uint64(data, ARP_SHA + hardware_length + protocol_length/2);
			m_summary.ip_destination_address.m_Hi = read_uint64(data, ARP_SHA + protocol_length + 2*hardware_length);
			m_summary.ip_destination_address.m_Lo = read_uint64(data, ARP_SHA + 3*protocol_length/2 + 2*hardware_length);
			
			if (tree)
			{
				add_child(tree, off + ARP_SHA + hardware_length, 16, NODE_SPA,	format_as_ipv6_address(data + ARP_SHA + hardware_length));
				add_child(tree, off + ARP_SHA + hardware_length + protocol_length, 6, NODE_THA,	format_as_mac_address(data + ARP_SHA + hardware_length + protocol_length));
				add_child(tree, off + ARP_SHA + 2*hardware_length + protocol_length, 16, NODE_TPA,	format_as_ipv6_address(data + ARP_SHA + 2*hardware_length + protocol_length));
			}
		}

		return true;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PPPoE-parser
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// PPPoE header fields offsets
	#define PPP_VER_TYPE	0
	#define PPP_CODE		1
	#define PPP_S_ID		2
	#define PPP_LEN			4
	#define PPP_SIZE		6

	bool parse_ppoe(const tBYTE* data, tTree* tree)
	//--------------------------------------------
	{
		if (data + PPP_SIZE > m_end) return false;
		
		tWORD off = (tWORD) (data - m_start);
		if (tree)
		{
			add_child(tree, off + PPP_VER_TYPE, 1, NODE_VERSION, format_as_num((read_uint8(data, PPP_VER_TYPE) & 0xF0) >> 4));
			add_child(tree, off + PPP_VER_TYPE, 1, NODE_TYPE, format_as_num(read_uint8(data, PPP_VER_TYPE) & 0x0F));
			add_child(tree, off + PPP_CODE, 1, NODE_CODE, format_as_num(read_uint8(data, PPP_CODE)));
			add_child(tree, off + PPP_S_ID, 1, NODE_PPP_ID, format_as_num(read_uint16(data, PPP_S_ID)));
			tWORD data_length = read_uint16(data, PPP_LEN);
			add_child(tree, off + PPP_LEN, 2, NODE_LEN, format_as_num(data_length));
		}

		switch (m_summary.ether_type)
		{
		// Active Discovery
		case 0x8863:
//>>>>>>>>>>>TODO>>>> разобраться что за хрень
			break;
		// Session
		case 0x8864:
			if (data + PPP_SIZE + 2 > m_end) return false;
			tWORD protocol = read_uint16(data, PPP_SIZE);
			set_length(tree, PPP_SIZE + 2);
			
			if (protocol == 0x0021) 
				return parse_ipv4(data + PPP_SIZE + 2, add_child(m_root, get_offset(tree) + PPP_SIZE + 2, IPV4_SIZE, NODE_IP_V4));
			
			if (protocol == 0x0057)
				return parse_ipv6(data + PPP_SIZE + 2, add_child(m_root, get_offset(tree) + PPP_SIZE + 2, IPV6_SIZE, NODE_IP_V6));
			break;
		}

		return true;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// IPX, LLC
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	#define LLC_DSAP	0
	#define LLC_SSAP	1
	#define LLC_CF		2
	#define LLC_SIZE	3

	#define IPX_CHECKSUM		0
	#define IPX_PACKET_LEN		2
	#define IPX_TRANSPORT_CTRL	4
	#define IPX_TYPE			5
	#define IPX_DST_NETWORK		6
	#define IPX_DST_NODE		10
	#define IPX_DST_SOCKET		16
	#define IPX_SRC_NETWORK		18
	#define IPX_SRC_NODE		22
	#define IPX_SRC_SOCKET		28
	#define IPX_SIZE			30

	bool parse_as_nbipx(const tBYTE* data, tTree* tree)
	//-------------------------------------------------
	{
		return true;
	}

	bool parse_as_ipx(const tBYTE* data, tTree* tree)
	//-----------------------------------------------
	{
		m_summary.protocol = 0xFF;
		m_summary.hi_protocol = 0x02;

		if (data + IPX_SIZE > m_end) return false;

		tBYTE type = read_uint8(data, IPX_TYPE);
		tWORD off = (tWORD) (data - m_start);
		if (tree)
		{
			add_child(tree, off + IPX_CHECKSUM,			2, NODE_CHKSUM, format_as_hex_word(read_uint16(data, IPX_CHECKSUM)));
			add_child(tree, off + IPX_PACKET_LEN,		2, NODE_PACKET_LEN, format_as_hex_word(read_uint16(data, IPX_PACKET_LEN)));
			add_child(tree, off + IPX_TRANSPORT_CTRL,	1, NODE_TRANSPORT_CTRL, format_as_hex_byte(read_uint8(data, IPX_TRANSPORT_CTRL)));
			add_child(tree, off + IPX_TYPE,				1, NODE_PACKET_TYPE,	format_as_hex_byte(type), format_as_num(type));
			
			tWORD d_socket = read_uint16(data, IPX_DST_SOCKET);
			add_child(tree, off + IPX_DST_NETWORK,	4, NODE_DST_NETWORK,	format_as_hex(data + IPX_DST_NETWORK, 4));
			add_child(tree, off + IPX_DST_NODE,		6, NODE_DST_NODE,		format_as_mac_address(data + IPX_DST_NODE));
			add_child(tree, off + IPX_DST_SOCKET,	2, NODE_DST_SOCKET,	format_as_hex_word(d_socket), format_as_num(d_socket));
			
			tWORD s_socket = read_uint16(data, IPX_SRC_SOCKET);
			add_child(tree, off + IPX_SRC_NETWORK,	4, NODE_SRC_NETWORK,	format_as_hex(data + IPX_SRC_NETWORK, 4));
			add_child(tree, off + IPX_SRC_NODE,		6, NODE_SRC_NODE,		format_as_mac_address(data + IPX_SRC_NODE));
			add_child(tree, off + IPX_SRC_SOCKET,	2, NODE_SRC_SOCKET,	format_as_hex_word(s_socket), format_as_num(s_socket));
		}
		// нет нормального алгоритма определения что за данные идут следом: NBIPX, RIP, SAP???
//>>>>>>>>>>>TODO>>>> разобраться что за хрень

		return true;
	}
	bool parse_as_llc(const tBYTE* data, tTree* tree)
	//-----------------------------------------------
	{
		m_summary.protocol = 0xFF;
		m_summary.hi_protocol = 0x03;

		if (data + LLC_SIZE > m_end) return false;

		tBYTE dsap = read_uint8(data, LLC_DSAP);
		tBYTE control_field = read_uint8(data, LLC_CF);
		tBYTE shift = (((control_field & 0x01) == 0x00) || ((control_field & 0x03) == 0x01)) ? 1 : 0;
		
		tWORD off = (tWORD) (data - m_start);
		if (tree)
		{
			tTree* t_dsap = add_child(tree, off + LLC_DSAP, 1, NODE_DSAP);
			add_child(t_dsap, off + LLC_DSAP, 1, NODE_ADDRESS, format_as_bit_mask(dsap, 0xFE), format_as_num(dsap & 0xFE));
			add_child(t_dsap, off + LLC_DSAP, 1, (dsap & 0x01) ? NODE_GROUP_ADDR : NODE_INDIVIDUAL_ADDR, format_as_bit_mask(dsap, 0x01));

			tTree* t_ssap = add_child(tree, off + LLC_SSAP, 1, NODE_SSAP);
			tBYTE ssap = read_uint8(data, LLC_SSAP);
			add_child(t_ssap, off + LLC_SSAP, 1, NODE_ADDRESS, format_as_bit_mask(ssap, 0xFE), format_as_num(ssap  & 0xFE));
			add_child(t_ssap, off + LLC_SSAP, 1, (ssap & 0x01) ? NODE_RF : NODE_CF, format_as_bit_mask(ssap, 0x01));

			if ((control_field & 0x01) == 0x00)
			{
				if (data + LLC_SIZE + 1 > m_end) return false;
				add_child(tree, off + LLC_CF, 1, NODE_CFLD, format_as_hex_word(read_uint16(data, LLC_CF)), "1");
			}
			else if ((control_field & 0x03) == 0x01)
			{
				if (data + LLC_SIZE + 1 > m_end) return false;
				add_child(tree, off + LLC_CF, 1, NODE_CFLD, format_as_hex_word(read_uint16(data, LLC_CF)), "2");
			}
			else
			{
				add_child(tree, off + LLC_CF, 1, NODE_CFLD, format_as_hex_byte(control_field), "0");
			}
		}
		
		switch (dsap)
		{
//>>>>>>>>>>>TODO>>>> разобраться что за хрень
			case 0x04:
			case 0x05:
			case 0x08:
			case 0x0C:
				//SNA
				break;
			case 0x06:
				//IPv4
				break;
			case 0x80:
				//XNS
				break;
			case 0xAA:
			case 0xAB:
				//SNAP  Snap;
				break;
//>>>>>>>>>>>TODO>>>> 
			case 0xE0:
				//IPX
				return parse_as_ipx(data + LLC_SIZE + shift, add_child(m_root, off + LLC_SIZE, IPX_SIZE, NODE_IPX));
			case 0xF0:
				//NETBIOS
				break;
		}

		return true;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// разбираем ethernet-header
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// ETHERNET MAC header fields offsets
	#define	MAC_DST		0	// destination MAC address
	#define	MAC_SRC		6	// source MAC address
	#define	MAC_ET		12	// ether type
	#define	MAC_SIZE	14	// header size

	bool parse_mac_header(const tBYTE* data, tTree* tree)
	//---------------------------------------------------
	{
		if (data + MAC_SIZE > m_end) return false;

		m_summary.mac_destination_address = ((tQWORD) read_uint16(data, MAC_DST) << 32) | read_uint32(data, MAC_DST + 2);
		m_summary.mac_source_address = ((tQWORD) read_uint16(data, MAC_SRC) << 32) | read_uint32(data, MAC_SRC + 2);
		m_summary.ether_type = read_uint16(data, MAC_ET);

		if (tree)
		{
			tTree* dst = add_child(tree, MAC_DST, 6, NODE_DEST_MAC, format_as_mac_address(data));
			add_child(dst, MAC_DST, 1, data[MAC_DST] & 0x80 ? NODE_GROUP_ADDR : NODE_INDIVIDUAL_ADDR, format_as_bit_mask(data[MAC_DST], 0x80));
			add_child(dst, MAC_DST, 1, data[MAC_DST] & 0x40 ? NODE_LOCAL_ADMIN_ADDR : NODE_UNIVERSAL_ADDR, format_as_bit_mask(data[MAC_DST], 0x40));

			tTree* src = add_child(tree, MAC_SRC, 6, NODE_SRC_MAC, format_as_mac_address(data + MAC_SRC));
			add_child(src, MAC_SRC, 1, data[MAC_SRC] & 0x80 ? NODE_GROUP_ADDR : NODE_INDIVIDUAL_ADDR, format_as_bit_mask(data[MAC_SRC], 0x80));
			add_child(src, MAC_SRC, 1, data[MAC_SRC] & 0x40 ? NODE_LOCAL_ADMIN_ADDR : NODE_UNIVERSAL_ADDR, format_as_bit_mask(data[MAC_SRC], 0x40));

			add_child(tree, MAC_ET, 2, (m_summary.ether_type <= 0x05DC) ? NODE_DATALEN : NODE_ETHER_TYPE, format_as_hex_word(m_summary.ether_type));
		}

		return true;
	}
	bool parse_as_old_ethernet(const tBYTE* data, tTree* tree)
	//--------------------------------------------------------
	{
		if (data + 2 > m_end) return false;
		
		tWORD off = (tWORD) (data - m_start);
		tWORD t = read_uint16(data);
		if (t == 0xFFFF) // raw ipx
			return parse_as_ipx(data, add_child(tree, off, IPX_SIZE, NODE_IPX));
		else
			return parse_as_llc(data, add_child(tree, off, LLC_SIZE, NODE_LLC));

		return true;
	}
	bool parse_as_ethernet(const tBYTE* data, tTree* tree)
	//----------------------------------------------------
	{
		tTree* ether = add_child(tree, 0, MAC_SIZE, NODE_ETHERNET);

		if (parse_mac_header(data, ether)) 
		{
			// IEEE 802.3 Length Field (old)
			if (m_summary.ether_type <= 0x05DC)
			{
				set_name(ether, NODE_IEEE802_3);
				return parse_as_old_ethernet(data + MAC_SIZE, tree);
			}
			else
			{
				switch (m_summary.ether_type)
				{
				// IEEE 802.1Q
				case 0x8100:
					set_name(ether, NODE_IEEE802_1Q);
					return true;
				
				// Ethernet II
				// IPv4
				case 0x0800:
						return parse_ipv4(data + MAC_SIZE, add_child(tree, MAC_SIZE, IPV4_SIZE, NODE_IP_V4));
				// ARP
				case 0x0806:
						return parse_arp(data + MAC_SIZE, add_child(tree, MAC_SIZE, 0, NODE_ARP));
				// IPv6
				case 0x86DD:
						return parse_ipv6(data + MAC_SIZE, add_child(tree, MAC_SIZE, IPV6_SIZE, NODE_IP_V6));

				// PPPoE
				case 0x8863:
				case 0x8864:
						return parse_ppoe(data + MAC_SIZE, add_child(tree, MAC_SIZE, PPP_SIZE, NODE_PPPOE));

				default:
					return true;
				}
			}
		}
		return false;
	}

	bool parse(const tBYTE* data, tTree* tree)
	//----------------------------------------
	{
		m_root = tree;
		return parse_as_ethernet(data, tree);
	}
public:

	void init(const tBYTE* data, tDWORD size)
	//---------------------------------------
	{
		m_start = (tBYTE*) data;
		m_end = (tBYTE*) (data + size);
	}

	bool parse_tree(const tBYTE* data, tTree* tree)
	//---------------------------------------------
	{
		m_root = tree;

//>>>>>>>>>>>			
/*
#ifdef _DEBUG
		FILE* temp;
		int error = fopen_s(&temp, "c:\\_frame.bin", "wb");
		if (error == 0)
		{
			fwrite(data, 1, m_end - data, temp);
			fclose(temp);
		}
#endif
*/
//>>>>>>>>>>>			

		tDWORD packet_length = (tDWORD)(m_end - data);
		set_name(tree, FRAME, format_as_num(packet_length));
		set_length(tree, packet_length);
		if (parse(data, tree))
			return true;

		return false;
	}
	bool surface_parse(const tBYTE* data, tSummary* summary)
	//------------------------------------------------------
	{
		if (parse(data, NULL))
		{
			*summary = m_summary;
			return true;
		}

		return false;
	}

};

#endif // NET_PARSER_H_INCLUDED