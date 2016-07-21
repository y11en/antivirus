class xsd__anyType {
public:
	virtual int getType();
};

class xsd__anySimpleType: public xsd__anyType {
public:
	virtual int getType();
};

typedef char *xsd__anyURI;
class xsd__anyURI_: public xsd__anySimpleType {
public:
	xsd__anyURI __item;
	virtual int getType();
};

typedef bool xsd__boolean;
class xsd__boolean_: public xsd__anySimpleType {
public:
	xsd__boolean __item;
	virtual int getType();
};

typedef char *xsd__date;
class xsd__date_: public xsd__anySimpleType {
public:
	xsd__date __item;
	virtual int getType();
};

typedef char* xsd__dateTime;
class xsd__dateTime_: public xsd__anySimpleType {
public:
	xsd__dateTime __item;
	virtual int getType();
};

typedef double xsd__double;
class xsd__double_: public xsd__anySimpleType{
public:
	xsd__double __item;
	virtual int getType();
};

typedef char *xsd__duration;
class xsd__duration_: public xsd__anySimpleType {
public:
	xsd__duration __item;
	virtual int getType();
};

typedef float xsd__float;
class xsd__float_: public xsd__anySimpleType {
public:
	xsd__float __item;
	virtual int getType();
};

typedef char *xsd__time;
class xsd__time_: public xsd__anySimpleType {
public:
	xsd__time __item;
	virtual int getType();
};

typedef char *xsd__decimal;
class xsd__decimal_: public xsd__anySimpleType {
public:
	xsd__decimal __item;
	virtual int getType();
};

typedef char *xsd__integer;
class xsd__integer_: public xsd__decimal_ {
public:
	xsd__integer __item;
	virtual int getType();
};

typedef LONG64 xsd__long;
class xsd__long_: public xsd__integer_ {
public:
	xsd__long __item;
	virtual int getType();
};

typedef long xsd__int;
class xsd__int_: public xsd__long_ {
public:
	xsd__int __item;
	virtual int getType();
};

typedef short xsd__short;
class xsd__short_: public xsd__int_ {
public:
	xsd__short __item;
	virtual int getType();
};

typedef char xsd__byte;
class xsd__byte_: public xsd__short_{
public:
	xsd__byte __item;
	virtual int getType();
};

typedef char *xsd__nonPositiveInteger;
class xsd__nonPositiveInteger_: public xsd__integer_ {
public:
	xsd__nonPositiveInteger __item;
	virtual int getType();
};

typedef char *xsd__negativeInteger;
class xsd__negativeInteger_: public xsd__nonPositiveInteger_ {
public:
	xsd__negativeInteger __item;
	virtual int getType();
};

typedef char *xsd__nonNegativeInteger;
class xsd__nonNegativeInteger_: public xsd__integer_ {
public:
	xsd__nonNegativeInteger __item;
	virtual int getType();
};

typedef char *xsd__positiveInteger;
class xsd__positiveInteger_: public xsd__nonNegativeInteger_ {
public:
	xsd__positiveInteger __item;
	virtual int getType();
};

typedef ULONG64 xsd__unsignedLong;
class xsd__unsignedLong_: public xsd__nonNegativeInteger_{
public:
	xsd__unsignedLong __item;
	virtual int getType();
};

typedef unsigned long xsd__unsignedInt;
class xsd__unsignedInt_: public xsd__unsignedLong_ {
public:
	xsd__unsignedInt __item;
	virtual int getType();
};

typedef unsigned short xsd__unsignedShort;
class xsd__unsignedShort_: public xsd__unsignedInt_ {
public:
	xsd__unsignedShort __item;
	virtual int getType();
};

typedef unsigned char xsd__unsignedByte;
class xsd__unsignedByte_: public xsd__unsignedShort_ {
public:
	xsd__unsignedByte __item;
	virtual int getType();
};

typedef wchar_t *xsd__wstring;
class xsd__wstring_: public xsd__anySimpleType {
public:
	xsd__wstring __item;
	virtual int getType();
};

typedef char *xsd__string;
class xsd__string_: public xsd__anySimpleType {
public:
	xsd__string __item;
	virtual int getType();
};

typedef char *xsd__normalizedString;
class xsd__normalizedString_: public xsd__string_ {
public:
	xsd__normalizedString __item;
	virtual int getType();
};

typedef char *xsd__token;
class xsd__token_: public xsd__normalizedString_ {
public:
	xsd__token __item;
	virtual int getType();
};

class xsd__base64Binary: public xsd__anySimpleType{
public:
	unsigned char *__ptr;
	int __size;
	virtual int getType();
};

class xsd__hexBinary: public xsd__anySimpleType {
public:
	unsigned char *__ptr;
	int __size;
	virtual int getType();
};

class param_error_loc_args
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};

class param_error_loc
{
public:
    xsd__int                format_id;
    xsd__wstring            format;
    xsd__wstring            locmodule;
    param_error_loc_args    args;
};

//struct param_error;

struct param_error{
	xsd__int            code;
	xsd__wstring        module;
	xsd__string         file;
	xsd__int            line;
	xsd__wstring        message;
    param_error_loc*    locdata;
    //param_error*        prev;
};

typedef xsd__boolean param__null;


//Value
class param__value{
public:
	xsd__anyType* data;
};

class param__entry{
public:
	xsd__wstring	name;
	param__value	value;
};

//Params
class param__node{
public:
	param__entry*	    __ptr;
	int				    __size;
};

//ParamsValue
class param__params : public xsd__anyType{
public:
	param__node *node;
	virtual int getType();
    xsd__int            checksum;
    xsd__base64Binary   binfmt;
};

//ArrayValue
class param__arrayvalue: public xsd__anyType{
public:
	param__value*	__ptr;
	int				__size;
	virtual int getType();
};






class aklwngt__anyCmd{
public:
	// virtual int getType();
};

class aklwngt__CmdError : public aklwngt__anyCmd {
public:
    struct param_error  oError;
	// virtual int getType();
};

class aklwngt__CmdHash : public aklwngt__anyCmd {
public:
	xsd__base64Binary       hash;
	// virtual int getType();
};

class aklwngt__CmdPar : public aklwngt__anyCmd {
public:
	param__params           parameters;
	// virtual int getType();
};

class aklwngt__CmdParAndHash : public aklwngt__anyCmd {
public:
	xsd__base64Binary       hash;
	param__params           parameters;
	// virtual int getType();
};

struct aklwngt__string_array
{
	xsd__string*	__ptr;
	int				__size;
};

class aklwngt__CmdSubscriptions : public aklwngt__anyCmd {
public:
	struct aklwngt__string_array    typesInfo;
	struct aklwngt__string_array    typesWarning;
	struct aklwngt__string_array    typesError;
	struct aklwngt__string_array    typesCritical;
	// virtual int getType();
};

struct aklwngt__DevInfo
{
    xsd__wstring    presumedName;
    xsd__wstring    presumedGroup;
    xsd__int        os_type;
    xsd__int        os_ver_major;
    xsd__int        os_ver_minor;

    xsd__wstring        os_manufacturer;
    xsd__wstring        os_name;
    xsd__wstring        os_ver;
    xsd__unsignedLong   memory;
    xsd__wstring        processor_family;
    xsd__wstring        system_name;
    xsd__wstring        system_type;
};

struct aklwngt__ProdInfo
{
    xsd__string     product_name;
    xsd__string     product_version;
    xsd__dateTime   installTime;
    xsd__wstring    dn;
    xsd__string     prodVersion;
};

struct aklwngt__LicKeyInfo
{
    xsd__int        keyType;
    xsd__int        keyProdSuiteID;
    xsd__int        keyAppID;
    xsd__dateTime   keyInstallDate;
    xsd__dateTime   keyExpirationDate;
    xsd__int        keyLicPeriod;
    xsd__int        keyLicCount;
    xsd__string     keySerial;
    xsd__string     keyMajVer;
    xsd__wstring    keyProdName;
};

struct aklwngt__LicenseInfo
{
    struct aklwngt__LicKeyInfo* keyCurrent;
    struct aklwngt__LicKeyInfo* keyNext;
};

class aklwngt__CmdAppInfo : public aklwngt__anyCmd {
public:
	xsd__base64Binary               hash;
	struct aklwngt__ProdInfo        prodInfo;
	struct aklwngt__LicenseInfo     licenseInfo;
    struct aklwngt__DevInfo         devInfo;
	// virtual int getType();
};

class aklwngt__CmdAppState : public aklwngt__anyCmd {
public:
	xsd__base64Binary       hash;
    xsd__int                stateApp;
    xsd__int                stateRTP;
    xsd__dateTime           lastFullScan;
    xsd__dateTime           lastUpdateTime;
    xsd__dateTime           basesDate;
    xsd__dateTime           basesInstallDate;
    xsd__int                basesRecords;

	// virtual int getType();
};

struct aklwngt__command{
    xsd__int            code;
    aklwngt__anyCmd*    data;
};

struct aklwngt__sync_info{
    xsd__base64Binary    hash_stgs;
    xsd__base64Binary    hash_pol;
    xsd__base64Binary    hash_sbsc;
    xsd__base64Binary    hash_info;
    xsd__base64Binary    hash_state;
};

struct aklwngt__event_body
{
    xsd__int                    severity;
    xsd__string                 product_name;
    xsd__string                 product_version;
    xsd__string                 event_type;
    xsd__wstring                event_type_display_name;
    xsd__wstring                descr;
    xsd__int                    locid;
    xsd__dateTime               rise_time;
    xsd__int                    rise_time_ms;
    xsd__wstring                par1;
    xsd__wstring                par2;
    xsd__wstring                par3;
    xsd__wstring                par4;
    xsd__wstring                par5;
    xsd__wstring                par6;
    xsd__wstring                par7;
    xsd__wstring                par8;
    xsd__wstring                par9;
};

struct aklwngt__event_info
{
    xsd__string                 eventId;
    struct aklwngt__event_body  body;
};

struct aklwngt__event_info_array
{
	struct aklwngt__event_info* __ptr;
	int                         __size;
};

struct aklwngt__klft_file_chunk_buff
{
	xsd__unsignedByte*	__ptr;
	int					__size;
};

struct aklwngt__klft_file_chunk_t
{
	xsd__wstring		                    fileName;
	xsd__unsignedLong	                    CRC;
	xsd__unsignedLong	                    startPosInFile;
	xsd__unsignedLong	                    fullFileSize;
	struct aklwngt__klft_file_chunk_buff    buff;
};

struct aklwngt__klft_file_info_t
{
	xsd__wstring		fileName;
	xsd__boolean		isDir;
	xsd__boolean		readPermission;
	xsd__dateTime       createTime;
	xsd__unsignedLong	fullFileSize;
};


int aklwngt__SessionBegin(
						xsd__string     szDeviceId,
                        xsd__string     szPassword,
                        int             nProtocolVersion,
                        struct aklwngt__SessionBeginResponse
                        {
                            xsd__string         szSessionId;
                            int                 nProtocolVersion;
                            struct param_error* oError;
                        }&r);

int aklwngt__SessionEnd(
                        xsd__string         szSessionId,
                        struct aklwngt__SessionEndResponse
                        {
                            struct param_error*         oError;
                        }&r);

int aklwngt__Heartbeat(
                        xsd__string     szSessionId,
                        struct aklwngt__sync_info* oMaySynchronize,
                        struct aklwngt__HeartbeatResponse{
                            struct aklwngt__command     oCommand;
                            struct param_error*         oError;
                        }&r);

int aklwngt__PutResult(
                        xsd__string     szSessionId,
                        struct aklwngt__command*    result,
                        struct aklwngt__PutResultResponse{
                            struct aklwngt__command     oNextCommand;
                            struct param_error*         oError;
                        }&r);

int aklwngt__PutEvents(
                        xsd__string     szSessionId,
                        struct aklwngt__event_info_array* oEventsInfoArray,
                        struct aklwngt__PutEventsResponse{
                            struct param_error*         oError;
                        }&r);

int aklwngt__klft_GetFileInfo(
                        xsd__string     szSessionId,
						xsd__wstring	fileName,
						struct aklwngt__klft_GetFileInfoResponse
						{
							struct aklwngt__klft_file_info_t	    info;
							struct param_error*         oError;
						}&r);

int aklwngt__klft_GetNextFileChunk(
                        xsd__string     szSessionId,
						xsd__wstring	fileName,
						xsd__unsignedLong	startFilePos,
						xsd__unsignedLong	neededSize,
						struct aklwngt__klft_GetNextFileChunkResponse
						{
							struct aklwngt__klft_file_chunk_t	chunk;
							struct param_error*                 oError;
						}&r);
