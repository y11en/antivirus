// stream_packet.h
//
// data structures for stream processing applications
//

#ifndef stream_packet_h_INCLUDED
#define stream_packet_h_INCLUDED

enum
{
	KLAV_STREAM_FORMAT_RAW  = 0,
	KLAV_STREAM_FORMAT_GZIP = 1
};

enum // flags
{
	KLAV_STREAM_PACKET_F_BEGIN = 0x0001,  // flag: stream begin
	KLAV_STREAM_PACKET_F_END   = 0x0002,  // flag: stream end
};

struct KLAV_Stream_Packet
{
	// OFF: 0x00
	uint32_t  m_struct_size; // sizeof (KLAV_Stream_Packet)
	// OFF: 0x04
	uint32_t  m_flags;   // stream packet flags
	// OFF: 0x08
	uint32_t  m_format;  // stream data format
	// OFF: 0x0C
	uint32_t  m_size;    // size of packet data
	// OFF: 0x10
	KLAV_PAD64(uint8_t *, m_data); // ptr to packet data
	// OFF: 0x18

#ifdef __cplusplus
	KLAV_Stream_Packet (
			const void *data,
			uint32_t size,
			uint32_t flags,
			uint32_t format
		) :
			m_struct_size (sizeof (KLAV_Stream_Packet)),
			m_flags (flags),
			m_format (format),
			m_size (size)
		{
			m_data_pad = 0;
			m_data = (uint8_t *) data;
		}
#endif // __cplusplus
};

#endif // stream_packet_h_INCLUDED
