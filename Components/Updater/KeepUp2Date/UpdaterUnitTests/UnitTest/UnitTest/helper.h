#ifndef HELPER_H
#define HELPER_H

#pragma warning (disable: 4786)



#include <string>
#include <memory>
#include <vector>
#include <map>

#include "..\..\..\Client\net\netcore.h"
#include "..\..\..\Client\helper\fileInfo.h"
#include "..\..\..\Client\Include\kavupd_strings.h"
#include "..\..\..\Client\helper\sites_info.h"

#include "TestInterface.h"
#include "boost_test.h"

#include <windows.h>
#include <shellapi.h>

using namespace std;
/*
#define TD_BOOST_CHECK_MESSAGE(predicate, message_)\
if		(boost::test_tools::tt_detail::func(                      \
        P,                                                   \
        boost::wrap_stringstream().ref() << check_descr,     \
        BOOST_TEST_L(__FILE__),                              \
        (std::size_t)__LINE__,                               \
        boost::test_tools::tt_detail::TL,                    \
        boost::test_tools::tt_detail::CT)                     \
reason.append (message_);

BOOST_CHECK_MESSAGE
*/

#define TD_BOOST_CHECK_MESSAGE(predicate, message_)\
{\
	 bool res =(predicate);\
	 if (!res) reason.append (message_);\
	 BOOST_CHECK_MESSAGE (res, message_);\
}

namespace helper
{
	bool DeleteDirectory (const char* dir);
	bool MakeDirectory (const char* dir);
	bool IsFoldersEqual (const string& tool, const string& dir1, const string& dir2);
	void StartNetworkAgent ();
	void StopNetworkAgent ();
	bool RecreateDirectories (vector<string>& dirs);
	int CloneFile (LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists);

};

class AutoFile
	{
	public:
		AutoFile (const char* filename, DWORD desireAccess, DWORD dwShareMode, DWORD dwCreationDisposition);
		~AutoFile ();

		int GetFileSize ();
		HANDLE GetHandle();
		void LockFile ();

	private:
		HANDLE hFile;
		int bLocked;
	};

//former class NetAuthInfo_Unit: public NetAuthInfo
class AuthorizationInformation_Unit: public AuthorizationInformation
{
public:

	AuthorizationInformation_Unit (char* user, char* pass, char* request);
	~AuthorizationInformation_Unit ();

	bool operator== (const AuthorizationInformation& nai);
};

//former class NetUrlInfo_Unit: public netUrlInfo
class AddressInformation_Unit: public AddressInformation
{
public:
	AddressInformation_Unit (	Protocol m_protocol,
								std::string m_fullUrlFromUser,
								std::string m_hostName,
								std::string m_IPaddress,
								std::string m_relativeUrlPath,
								unsigned short m_port
							);
	~AddressInformation_Unit ();
	bool operator== (const AddressInformation& nui);
};

class NetFileInfo_Unit: public NetFileInfo
{
public:
	NetFileInfo_Unit (const char* filename, unsigned long size);
	bool operator== (const NetFileInfo& nfi);
};

class NetFileInfoPointer_Unit: public NetFileInfoPointer
{
public:
	NetFileInfoPointer_Unit (	const char *name,
								const char *rpath,
								const char *m_path,
								unsigned long rsize = 0,
								unsigned long lsize = 0,
								FILE *file = 0
							);
	~NetFileInfoPointer_Unit();
	bool operator== (const NetFileInfoPointer& nfip);
};

class FileInfo_Unit: public FileInfo
{
public:
	FileInfo_Unit (	const STRING &filename  = STRING(),
					const STRING &relativeURLPath = STRING("/"),
					const Type & updTyp= FileInfo::base,
					const bool primaryOrSubstitutedIndex = false):
		FileInfo (filename, relativeURLPath, updTyp, primaryOrSubstitutedIndex)  {};

	bool operator== (const FileInfo& right);

};

bool operator== (std::vector<FileInfo_Unit, std::allocator<FileInfo_Unit> >& fiu,
				 const std::vector<FileInfo, std::allocator<FileInfo> >& fi);


class SitesInfo_Unit: public SitesInfo
{
public:
	bool operator== (const SitesInfo& right);
};

bool operator== (std::vector<SitesInfo_Unit, std::allocator<SitesInfo_Unit> >& fiu,
				 const std::vector<SitesInfo, std::allocator<SitesInfo> >& fi);





// NTLM Messages
#pragma pack(push,1)


struct Type1Message
{
    BYTE    protocol[8];     // 'N', 'T', 'L', 'M', 'S', 'S', 'P', '\0'
    BYTE    type;            // 0x01
    BYTE    zero1[3];
    short   flags;           // 0xb203
    BYTE    zero2[2];

    short   dom_len;         // domain string length
    short   dom_len1;         // domain string length
    short   dom_off;         // domain string offset
    BYTE    zero3[2];

    short   host_len;        // host string length
    short   host_len1;        // host string length
    short   host_off;        // host string offset (always 0x20)
    BYTE    zero4[2];

    BYTE    host[256];         // host string (ASCII)
    //BYTE    dom;          // domain string (ASCII)
};


struct Type2Message
{
    BYTE    protocol[8];     // 'N', 'T', 'L', 'M', 'S', 'S', 'P', '\0'
    BYTE    type;            // 0x02
    BYTE    zero1[7];
    short   msg_len;         // 0x28
    BYTE    zero2[2];
    short   flags;           // 0x8201
    BYTE    zero3[2];

    BYTE    nonce[8];        // nonce
    BYTE    zero4[8];
};


struct Type3Message
{
    BYTE    protocol[8];     // 'N', 'T', 'L', 'M', 'S', 'S', 'P', '\0'
    BYTE    type;            // 0x03
    BYTE    zero1[3];

    short   lm_resp_len;     // LanManager response length (always 0x18)
    short   lm_resp_len2;     // LanManager response length (always 0x18)
    short   lm_resp_off;     // LanManager response offset
    BYTE    zero2[2];

    short   nt_resp_len;     // NT response length (always 0x18)
    short   nt_resp_len2;     // NT response length (always 0x18)
    short   nt_resp_off;     // NT response offset
    BYTE    zero3[2];

    short   dom_len;         // domain string length
    short   dom_len2;         // domain string length
    short   dom_off;         // domain string offset (always 0x40)
    BYTE    zero4[2];

    short   user_len;        // username string length
    short   user_len2;        // username string length
    short   user_off;        // username string offset
    BYTE    zero5[2];

    short   host_len;        // host string length
    short   host_len2;        // host string length
    short   host_off;        // host string offset
    BYTE    zero6[6];

    short   msg_len;         // message length
    BYTE    zero7[2];

    short   flags;           // 0x8201
    BYTE    zero8[2];

    BYTE    dom[2048];          // domain string (unicode UTF-16LE)
//    BYTE    user[*];         // username string (unicode UTF-16LE)
//    BYTE    host[*];         // host string (unicode UTF-16LE)
//    BYTE    lm_resp[*];      // LanManager response
//    BYTE    nt_resp[*];      // NT response
};

#pragma pack(pop)

struct Type1Message_Unit
{
	Type1Message_Unit (string hostname, string domenname);
	bool operator== (const Type1Message& t1m);

private:

	string host;
	string domen;
};

struct Type2Message_Unit
{
	bool operator== (const Type2Message& t2m);
};

struct Type3Message_Unit
{
	Type3Message_Unit (string hostname, string domenname, string username);
	bool operator== (const Type3Message& t3m);

private:

	string host;
	string domen;
	string user;
};


struct TEST_START
{
	string testName;
};

struct TEST_STEP
{
	string name;
	string result;
	string comment;
};

struct TEST_END
{
	string result;
	int duration;
};

class Logger
{

public:
	static Logger& GetLogger();
	static void Init(string& filename);

	void LogTestStart (TEST_START* str);
	void LogTestStep (TEST_STEP* str);
	void LogTestEnd (TEST_END* str);

	Logger (string filename);
	Logger::~Logger ();

protected:
	
	FILE* log;

	static shared_ptr<Logger> instance_ptr;
	static bool bInited;

	string os;
	string user;
	string host;
	string fileName;

	string tdtool;
	bool RunImport ();

};

class BaseTest
{
public:
	BaseTest (string testname);

	void Start();
	void End();
	void Step (string name, string reason);
	Logger& logger;

protected:

	string testName;
	bool testResult;
	
	DWORD startTick;
};


#endif