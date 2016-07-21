
#include <ParentalControl/structs/s_urlflt.h>

#include <Prague/pr_remote.h>
// http::response_message
#include <http/message.hpp>

///
/// Extract response information from the given message
///
/// @param msg - message is about to be analyzed
/// @param info - response information
///
void cUrlFilteringRequest::init( const http::message* const msg )
{
	if ( msg == NULL )
		return;

	// Get content type
	m_ContentType = msg->contenttype();

	const http::response_message* const response_msg = 
		dynamic_cast< const http::response_message* >( msg );

	// It is not a response message, so we can't get more information
	if ( response_msg == NULL )
		return;

	// Get status code
	m_StatusCode = response_msg->status_code();

	// Get content encoding
	m_ContentEncoding = response_msg->contentencoding();
}
