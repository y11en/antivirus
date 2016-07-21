#ifndef _AG_ACTIVATIONFACTORYIMP_H_
#define _AG_ACTIVATIONFACTORYIMP_H_

namespace OnlineActivation
{

/**
 * Implementation of activation factory interface.
 */
class ActivationFactoryImp : public ActivationFactory
{
public:
	/**
	 * Constructs an object by given configuration.
	 */
	ActivationFactoryImp(const ActivationConfig& config);
	/**
	 * Composes activation request (new user).
	 */
	virtual boost::shared_ptr<Request> ComposeRequest
	(
		const ApplicationId& appId,
		const ActivationCode& actCode
	);
	/**
	 * Composes activation request (registered user).
	 */
	virtual boost::shared_ptr<Request> ComposeRequest
	(
		const ApplicationId& appId,
		const ActivationCode& actCode,
		const CustomerId& custId
	);

private:
	// deny copying
	ActivationFactoryImp(const ActivationFactoryImp&);
	// deny assignment
	ActivationFactoryImp& operator= (const ActivationFactoryImp&);

private:
	/**
	 * Activation URL list.
	 */
	std::vector<std::string>  m_urls;

	/**
	 * Internet session object.
	 */
	boost::shared_ptr<InternetSession>	m_pSession;
};

} // namespace OnlineActivation

#endif // _AG_ACTIVATIONFACTORYIMP_H_
