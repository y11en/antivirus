#ifndef CFG_UPDATER2H_INCLUDED_DC1D058A_6987_4360_984F_6D926501FFD4
#define CFG_UPDATER2H_INCLUDED_DC1D058A_6987_4360_984F_6D926501FFD4

#include "../helper/comdefs.h"
#include "../helper/path.h"

namespace KLUPD {


enum AuthorizationType
{
    noAuthorization,
    basicAuthorization,
    ntlmAuthorization,
    ntlmAuthorizationWithCredentials,
};
extern NoCaseString toString(const AuthorizationType &);
extern AuthorizationType fromStringAuthorization(const NoCaseString &);
extern bool credentialsRequired(const AuthorizationType &);

typedef std::vector<AuthorizationType> AuthorizationTypeList;
extern NoCaseString toString(const AuthorizationTypeList &);

// Transport protocols enumeration
enum Protocol
{
    ftpTransport,
    httpTransport,
    administrationKitTransport,
    fileTransport,
};
extern NoCaseString toString(const Protocol &);
extern Path toProtocolPrefix(const Protocol &);

enum FtpMode
{
    activeFtp,
    passiveFtp,
    tryActiveFtpIfPassiveFails,
};


enum ProxyServerMode
{
    // connection to source via proxy should be done
    withProxy,
    // direct connection to destination address should be used
    directConnection,
};

struct KAVUPDCORE_API Credentials
{
    Credentials(const NoCaseString &userName = NoCaseString(), const NoCaseString &password = NoCaseString());

    NoCaseString toString()const;
    void clear();
    bool empty()const;

	///
	/// Mark credentials as canceled
	///
	void cancel( bool canceled );

	///
	/// Check if credentials are marked as canceled
	///
	bool cancelled() const;

    void userName(const NoCaseString &);
    NoCaseString userName()const;
    void password(const NoCaseString &);
    NoCaseString password()const;

private:
    NoCaseString m_userName;
    NoCaseString m_password;
	bool m_cancelled;
};

// TODO: merge with Source
// the Address structure represents address or server
struct KAVUPDCORE_API Address
{
    Address(const Path &urlToParse = Path());
    bool parse(const Path &urlToParse);

    NoCaseString toString()const;
    void clear();

	bool operator==(const Address &)const;
	bool operator!=(const Address &)const;
	// checks if proxy address is empty
	bool empty()const;

    NoCaseString& BuildURI(NoCaseString& str) const;
    NoCaseString BuildURI() const { NoCaseString tmpString; return BuildURI( tmpString ); }

    Protocol m_protocol;
    Credentials m_credentials;
    Path m_hostname;
    Path m_service;     // service name or port number
    Path m_path;
    Path m_fileName;

private:

    bool setProtocol(const NoCaseString &strProto);
    // host group includes host name, service name and login credentials
    bool parseHostGroup(const NoCaseString &strHost);
    // path includes local path with file name
    bool parsePath(const NoCaseString &strPath);
};


}   // namespace KLUPD

#endif  // #ifndef CFG_UPDATER2H_INCLUDED_DC1D058A_6987_4360_984F_6D926501FFD4
