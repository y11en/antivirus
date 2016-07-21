#ifndef __KLOLA_H__
#define __KLOLA_H__

#include <common/kllicinfo.h>

namespace KLOLA
{
	typedef enum
	{
		KLOLA_DIRECT,       //! direct inet connection
		KLOLA_PRECONFIG,    //! configuration retrieved from the registry
		KLOLA_PROXY         //! use specified proxy server
	}
    AccessType;


    //! URLs list
    const wchar_t c_szwOLA_Urls[] = L"KLOLA_URLS"; //ARRAY_T|STRING_T

    //! inet access settings, contains c_szwOLA_ICf_* parameters
    const wchar_t c_szwOLA_InetConfig[] = L"KLOLA_INET_CONFIG"; //PARAMS_T

        //! one of AccessType
        const wchar_t c_szwOLA_ICf_AccessType[] = L"KLOLA_ICFG_ACCESS_TYPE"; //INT_T

        //! Proxy server address
        const wchar_t c_szwOLA_ICf_ProxyServer[] = L"KLOLA_ICFG_PROXY"; //STRING_T

        //! Proxy bypass list
        const wchar_t c_szwOLA_ICf_ProxyByPass[] = L"KLOLA_ICFG_PROXY"; //ARRAY_T|STRING_T

	    //! Bypass proxy for local addresses
	    const wchar_t c_szwOLA_BypassLocal[] = L"KLOLA_ICFG_BYPASS_LOCAL"; //BOOL_T

        //! Proxy login
        const wchar_t c_szwOLA_ProxyLogin[] = L"KLOLA_ICFG_PROXY_LOGIN"; //STRING_T

        //! Proxy password
        const wchar_t c_szwOLA_ProxyPassword[] = L"KLOLA_ICFG_PROXY_PASSWORD"; //STRING_T

    //! application id
    const wchar_t c_szwOLA_AppId[] = L"KLOLA_APP_ID"; //INT_T

    //! CustomerId id
    const wchar_t c_szwOLA_CustomerId[] = L"KLOLA_CUSTOMER_ID"; //INT_T

    //! CustomerId password
    const wchar_t c_szwOLA_CustomerPassword[] = L"KLOLA_CUSTOMER_PASSW"; //INT_T

    //! Activation code in XXXXX-XXXXX-XXXXX-XXXX format
    const wchar_t c_szwOLA_ActivationCode[] = L"KLOLA_ACTIVATION_CODE"; //STRING_T
    
    //! key file
    const wchar_t c_szwOLAKey_KeyFile[] = KLLIC_LICFILE; //BINARY_T

    //! customer info file
    const wchar_t c_szwOLAKey_CustomerInfo[] = KLLIC_CUSTOMER_INFO; //BINARY_T


    //! key data returned by activation server
    class KLSTD_NOVTABLE KeyData
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	returns key data

          \retval ppData    key data
        */
        virtual void GetData(KLPAR::Params** ppData) = 0;
    };
    
    //! html form to fill returned by activation server
    class KLSTD_NOVTABLE HtmlForm
        :   public KLSTD::KLBaseQI
    {
    public:

        /*!
          \brief	Returns HTML form code. Put into browser control and 
                    process navigate event (see DWebBrowserEvents2::BeforeNavigate2).

          \retval   pHtml   html code
          \retval	nHtml   size of html code
            */
        virtual void GetHtml(const char* & pHtml, size_t& nHtml) = 0;
        

        /*!
          \brief	Submit

          \param	szHeaders       contains additional HTTP headers to send to the server 
          \param	nHeadersLength  length of szHeaders
          \param	pData           contains the data to send to the server if the HTTP POST transaction is being used
          \param	nDataSize       size of data pData
          \param    pbCancel        cancelling flag
          \param	ppResult        either KeyData or HtmlForm pointer
        */
        virtual void Submit(
                const char*         szHeaders, 
                size_t              nHeadersLength, 
                const void*         pData,
                size_t              nDataSize,
                bool*               pbCancelled,
                KLSTD::KLBaseQI**   ppResult) = 0;
    };
};


/*!
  \brief	Starts online activation. 
  \param	pData       input parameters (c_szwOLA_*)
  \param    pbCancel    cancelling flag
  \param	ppResult    either KeyData or HtmlForm pointer
*/
void KLOLA_ActivateOnline(
                KLPAR::ParamsPtr    pData,
                bool*               pbCancelled,
                KLSTD::KLBaseQI**   ppResult);


#endif //__KLOLA_H__
