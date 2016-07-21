#ifndef CFG_UPDATER_H_INCLUDED_150B635F_6EBF_4e27_9A38_30790F1A13CB
#define CFG_UPDATER_H_INCLUDED_150B635F_6EBF_4e27_9A38_30790F1A13CB


#define COMPONENT_DELIMITER	L",;"

#include "../helper/map_wrapper.h"
#include "../helper/sourceDescription.h"
#include "cfg_updater2.h"


namespace KLUPD {

typedef std::vector<NoCaseString> ComponentIdentefiers;
// represent component identifier list in human readable string
NoCaseString KAVUPDCORE_API toString(const ComponentIdentefiers &);
ComponentIdentefiers KAVUPDCORE_API fromString(const NoCaseString &);

// the UpdatedSettings represents variable read from index file
struct KAVUPDCORE_API UpdatedSettings
{
    // outputs value in human readable way
    NoCaseString toString()const;

    NoCaseString m_name;
    NoCaseString m_type;
    NoCaseString m_value;
};

// this structure Storage for updater parameters
struct KAVUPDCORE_API UpdaterConfiguration
{
	// the structure groupes settings to filter source files for retranslation-mode
	struct KAVUPDCORE_API SRetranslatedObjects
	{
        SRetranslatedObjects(const bool componentsWhiteListMode = true, const bool applicationsWhiteListMode = true);

        // represents structure in human-readable form
        NoCaseString toString(const char *indent = "\t")const;


		// flag indicates if list of Components specifies White List,
         //  otherwise of Components is a Black List. Default value is true
		bool Components_WhiteListMode;
		// component filter
		ComponentIdentefiers m_componentsToRetranslate;
		
		
		// flag indicates if list of Applications specifies White List,
         //  otherwise of Applications is a Black List. Default value is true
		bool Applications_WhiteListMode;
		
		// specifies filter for Applications
		MapStringVStringWrapper Applications;
	};
	
	//	the structure groupes list of flags which influence on filtering files to download
	struct ListFlags
	{
		// flag indicates whether bases should be updated
		bool update_bases;
		// download elements marked as 'patch' or 'code' in master.xml and marked as 'urgent'
		bool apply_urgent_updates;
		// download all elements marked as 'patch' or 'code' in master.xml
		bool automatically_apply_available_updates;

        // outputs list flags in readable form
        NoCaseString toString()const;

        // returns true if no operation is configured and nothing is to be updated
        bool empty()const;
	};

	// OS information
	struct OS
	{
        // output structure as readable string
        NoCaseString toString()const;

		NoCaseString os;
		NoCaseString version;
	};


    // Warning: you may construct an instance of UpdaterConfiguration with 0 values for identification,
     //  but the members m_applicationIdentifier, m_installationIdentifier, m_buildNumberIdentifier
     // MUST be non-null and non-empty strings before you call Updater::doUpdate()
     //
    // more detalis see for m_applicationIdentifier, m_installationIdentifier, m_buildNumberIdentifier
     //  members documentation
    explicit UpdaterConfiguration(const size_t applicationIdentifier, const size_t installationIdentifier, const NoCaseString &buildNumberIdentifier);

    // returns true in case something is to be retranslated configured
    bool updateRequired()const;
    // returns true in case something is to be retranslated configured
    bool retranslationRequired()const;
	
    // represent updater structure as human readable string
    // indent [in] - appended for each string
    NoCaseString toString(const char *indent = "\t")const;
    NoCaseString composeIdentityInfoString()const;

    // return filer flags according to update or retranslation operation
    const ListFlags &filterFlags(const bool retranslationMode)const;
    // return component filter according to update or retranslation operation
    const ComponentIdentefiers &filterComponents(const bool retranslationMode)const;


	// ** Generic Settings **
	
	// temporary folder to use during update
	Path temporary_dir;
    // array of substitutions to obtain paths. The variables are substituted with given values
	PathSubstitutionMap m_pathSubstitutionMap;


	// the location from which IUpdater is run. This location is used to search source in and use it.
     // If region is "<default>" string, then automatic Region detection mechanism takes place
	NoCaseString self_region;
	// product language for filtering
    std::vector<NoCaseString> product_lang;

    // filter by OS. Used only in update mode
	OS os;


    // update sources
    SourceList m_sourceList;

	
	//  ** Proxy Settings **

	// proxy server address
    Path proxy_url;
    // Bypass Proxy Server for local addresses,
     //  local addresses do not contain periods in them
     //  OR located in network which is directly connected to host
    bool m_bypassProxyServerForLocalAddresses;

	// proxy server port
	unsigned short proxy_port;

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    // Retrieve proxy settings automatically
    //
     // WARNING: Automatic proxy server detection uses JSPorxy library
     //  which in turn calls CoInitialize() in multithreaded appartement.
     //
    //  Requirements:
     //   implemented only on Windows platforms
     //
    // Restrictions:
     //  Algorithm details:
     //   1. In "Bypass proxy server for local addresses" mode (m_bypassProxyServerForLocalAddresses),
     //     first address is checked whether update source address is local.
     //    In case update source address is local then proxy server is not used.
     //   2. WPAD script is downloaded (Web proxy auto discovery script). WPAD script address is determined
     //     by DNS/DHCP requests. Proxy server address and usage need is obtained with WPAD script.
     //   3. In case WPAD script download failed, then proxy server address is read
     //     from Windows Registry for Internet Microsoft Explorer applicaiton
	bool use_ie_proxy;
    /////////////////////////////////////////////////////////

    // credentials for authorization on proxy server
    Credentials m_proxyAuthorizationCredentials;
	
    // a list of proxy authorization methods to try
    AuthorizationTypeList m_httpAuthorizationMethods;
	
	
	// ** parameters for update mode **
	// retranslation-mode flags to specify what to download
	ListFlags UpdaterListFlags;
	
	// component set of given product
    ComponentIdentefiers m_componentsToUpdate;
	
	// ** parameters for retranslation mode **
	// enable retranslation flag. Retranslation may happen only if this flag is true
	bool updates_retranslation;

	// folder to place retranslated files
	Path retranslation_dir;
	// retranslate-mode flags to specify what to download
	ListFlags RetranslationListFlags; 

    // filters for retranslation applications and components
	SRetranslatedObjects RetranslatedObjects;
	// flag indicates that only specified components should be retranslated
	//  Note: the set of components is specified in RetranslatedObjects structure
	bool retranslate_only_listed_apps;


	// ** product identification information **

    // Build number filter in update mode. Filters all files of with build not match.
    // Format:
     // Build is represented by string value or numeric ranges which are into semicolon separated list.
     // Empty string matches to all build numbers.
     // String range: is string build or application identifier representation 
     // Numeric range: is single number or range represented by two number delimited by comma symbol.
     //  First number represents minimal suit value, second number is maximum suit number in range.
	NoCaseString pID;
	// Product version filter for update operatoin. Filters all files which version is less then specified one.
     // Format: the product version is represented by 4 optional digits string
     //  [d1[.d2[.d3[.d4]]]]
     // Examples: "5.0.1.2", "5.0.1", ""
     // Note: comparison is performed by comparison each digit. In case product version
     //   is represented with less digits, then comparision is performed only with leading
     //  product version digits and trailing digits are not involved into comparison
	NoCaseString pVer;

	// application identifier is included into HTTP and FTP requests for statistics
     // Warning: this field is obligatory to be filled
	size_t m_applicationIdentifier;
    // build number identifier is included into HTTP and FTP requests for statistics
     // Warning: this field is obligatory to be filled
    NoCaseString m_buildNumberIdentifier;
	// installation identifier is included into HTTP and FTP requests for statistics
     // Warning: this field is obligatory to be filled
	size_t m_installationIdentifier;
	// serial key is included for statistics into HTTP and FTP requests
      // Example: "0038-00006d-0008e316"
    // Warning: hexademical numbers are in string
    // Warning: this field is optional, and may be empty. BUT Laboratory obtains profit in case
     // we are able to detect stollen keys and put them into black list, thus increasing sales number
	NoCaseString m_serialNumber;


	// ** network settings **

	// use ftp protocol in passive mode
	bool passive_ftp;
    // indicates whether updater should try acrive FTP mode in case passive FTP mode fails
    bool m_tryActiveFtpIfPassiveFails;
	// timeout for network operations
	unsigned connect_tmo;

private:
    // helper functions to compose identity information on update server
    static size_t encodeStringForIdentity(unsigned char *outBuffer, const size_t outSize, const NoCaseString &stringForIdentity);

    void convertSerialToLongs(long &serial1, long &serial2, long &serial3)const;

	// session identifier is included into HTTP and FTP requests for statistics,
     // randomly generated value per one update session
	size_t m_sessionIdentifier;


    static const size_t s_defaultTimeout;
};

}   // namespace KLUPD

#endif  // CFG_UPDATER_H_INCLUDED_150B635F_6EBF_4e27_9A38_30790F1A13CB
