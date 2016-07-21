// stream_scanner.cpp
//
//

#include "scanner_engine_impl.h"
#include <klava/klav_io.h>

////////////////////////////////////////////////////////////////

static void action_error (KLAV_CONTEXT_TYPE ctx, const char *action_name, KLAV_ERR err)
{
	KLAV_CONTEXT_TRACE (ctx, (KLAV_TRACE_LEVEL_ERROR, "error handling action: %s, error 0x%X", action_name, err));
}

KLAV_ERR KLAV_Scanner_Engine_Impl::scan_stream_data (
			KLAV_CONTEXT_TYPE ctx,
			KLAV_IO_Object *io,
			uint8_t *buf,
			size_t bufsz,
			uint32_t stream_fmt
		)
{
	KLAV_ERR err = KLAV_OK;

	if (m_engine == 0)
		return KLAV_ENOINIT;

//	err = m_engine->process_action (KLAV_CONTEXT, "BEGIN_STREAM", 0, 0);
//	if (KLAV_FAILED (err))
//	{
//		action_error (KLAV_CONTEXT, "BEGIN_STREAM", err);
//		return err;
//	}

	klav_filepos_t remain = 0;
	err = io->get_size (& remain);
	if (KLAV_FAILED (err))
	{
		KLAV_CONTEXT_TRACE (ctx, (KLAV_TRACE_LEVEL_ERROR, "io::get_size error", err));
		return err;
	}

	klav_filepos_t off = 0;

	uint32_t pkt_flags = KLAV_STREAM_PACKET_F_BEGIN;

	while (remain != 0)
	{
		uint32_t portion = (uint32_t) bufsz;
		if ((klav_filepos_t) portion > remain)
			portion = (uint32_t) remain;

		err = io->seek_read (off, buf, portion, 0);
		if (KLAV_FAILED (err))
		{
			KLAV_CONTEXT_TRACE (ctx, (KLAV_TRACE_LEVEL_ERROR, "io::get_size error", err));
			return err;
		}

		off += portion;
		remain -= portion;

		if (remain == 0)
			pkt_flags |= KLAV_STREAM_PACKET_F_END;

		KLAV_Stream_Packet pkt (buf, portion, pkt_flags, stream_fmt);
		KLAV_ERR err = m_engine->process_action (ctx, "PROCESS_PACKET", &pkt, 0);
		if (KLAV_FAILED (err))
		{
			action_error (ctx, "PROCESS_PACKET", err);
			return err;
		}
		
		pkt_flags &= ~KLAV_STREAM_PACKET_F_BEGIN;
	}

//	err = m_engine->process_action (KLAV_CONTEXT, "END_STREAM", 0, 0);
//	if (KLAV_FAILED (err))
//	{
//		action_error (KLAV_CONTEXT, "END_STREAM", err);
//		return err;
//	}

	return KLAV_OK;
}

KLAV_ERR KLAV_Scanner_Engine_Impl::scan_object_as_stream (
				KLAV_CONTEXT_TYPE ctx,
				KLAV_IO_Object *object
			)
{
	KLAV_ERR err = KLAV_OK;

	uint32_t stream_pkt_size = m_stream_pkt_size;
	if (stream_pkt_size == 0)
		stream_pkt_size = KLAV_STREAM_DEFAULT_PKT_SIZE;

	uint32_t stream_fmt = m_stream_fmt;

	if (object == 0)
		KLAV_RETURN_ERR (KLAV_EINVAL);

	uint8_t * stream_buf = m_allocator->alloc (stream_pkt_size);
	if (stream_buf == 0)
		KLAV_RETURN_ERR (KLAV_ENOMEM);

	err = scan_stream_data (ctx, object, stream_buf, stream_pkt_size, stream_fmt);

	if (stream_buf != 0)
	{
		m_allocator->free (stream_buf);
	}

	KLAV_RETURN_ERR (err);
}

KLAV_ERR KLAV_Scanner_Engine_Impl::do_process_stream (
			KLAV_IO_Object * object,
			KLAV_Properties *props
		)
{
	KLAV_ERR err = KLAV_OK;

	KLAV_CONTEXT_TYPE ctx;
	err = m_engine->create_context (props, &ctx);
	if (KLAV_FAILED (err))
		KLAV_RETURN_ERR (err);

	err = m_engine->activate_context (ctx, 0, props);
	if (KLAV_FAILED (err))
	{
		m_engine->destroy_context (ctx);
		KLAV_RETURN_ERR (err);
	}

	err = scan_object_as_stream (ctx, object);

	m_engine->deactivate_context (ctx);
	m_engine->destroy_context (ctx);

	KLAV_RETURN_ERR (err);
}

////////////////////////////////////////////////////////////////
// Katran action: scan object as stream
/*
KLAV_PROC_INTERNAL (klav_bool_t, KLAV_Scan_Object_As_Stream)(KLAV_CONTEXT_ARG)
{
	KLAV_ERR err = KLAV_OK;
	
	KLAV_Scanner_Engine_Impl * engine = (KLAV_Scanner_Engine_Impl *) KlavGetEngine ();
	KLAV_IO_Object * object = (KLAV_IO_Object *) KlavGetObject ();

	err = engine->scan_object_as_stream (KLAV_CONTEXT, object);
	if (KLAV_FAILED (err))
	{
		KLAV_SET_ERROR (err);
		return 0;
	}
	
	return 1;
}
*/