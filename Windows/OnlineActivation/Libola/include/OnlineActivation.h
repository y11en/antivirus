#ifndef _AG_ONLINEACTIVATION_H_
#define _AG_ONLINEACTIVATION_H_

#include <boost/shared_ptr.hpp>
#include "ActivationTypes.h"
#include "ActivationErrors.h"
#include "AuthToken.h"
#include "RequestObserver.h"
#include "Content.h"
#include "Response.h"
#include "Request.h"
#include "ActivationFactory.h"

#ifdef LIBOLA_EXPORTS
	#define LIB_OLA_API __declspec (dllexport)
#else
	#define LIB_OLA_API __declspec (dllimport)
#endif

/**
 * Creates ActivationFactory interface.
 * @param[in] config activation process configuration.
 * @exception ActivationError error code specifies exception reason.
 */
LIB_OLA_API boost::shared_ptr<OnlineActivation::ActivationFactory> 
	CreateActivationFactory(const OnlineActivation::ActivationConfig& config);

#endif // _AG_ONLINEACTIVATION_H_
