#include "licensing.h"

#include "stdinc.h"
#include "indexFileXMLParser.h"

#include "updaterStaff.h"

#include <Dskm/dskm.h>


#ifdef WIN32
    typedef wchar_t DSKM_WCHAR_T;
#else
    typedef unsigned short DSKM_WCHAR_T;
#endif

// DSKM requires 2-bytes UNICODE
void convertStringToDskmMultibyte(const std::string &input, std::vector<DSKM_WCHAR_T> &multibyteResult);


#if defined(WIN32) && defined(WSTRING_SUPPORTED)
    void convertStringToDskmMultibyte(const std::string &input, std::vector<DSKM_WCHAR_T> &multibyteResult)
    {
        const std::wstring inputWString = KLUPD::asciiToWideChar(input);
        multibyteResult.assign(inputWString.begin(), inputWString.end());
        multibyteResult.push_back(0);
    }
#else
    void convertStringToDskmMultibyte(const std::string &input, std::vector<DSKM_WCHAR_T> &multibyteResult)
    {
        multibyteResult.clear();

        // test if big-endian or little-endian platform
        size_t endianIndex = 1;
        for(size_t j = 0; j <= sizeof(DSKM_WCHAR_T) - 1; ++j)
        {
            const DSKM_WCHAR_T test = 0xFF;
            if(((char*)(&test))[j] == (char)0xFF)
                endianIndex = j;
        }

        char mb[8]="\0\0\0\0\0\0\0";
        for(size_t i = 0; i < input.size(); ++i)
        {
            mb[endianIndex] = input[i];
            multibyteResult.push_back(*reinterpret_cast<const DSKM_WCHAR_T *>(&mb));
        }
        multibyteResult.push_back(0);
    }

#endif


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/// signature 5

#ifdef KLUPD_ENABLE_SIGNATURE_5
    #include <sign/sign.h>

    #ifdef WIN32
        #include <io.h>

    #else   // WIN32
        #if defined(UNICODE) || defined(_UNICODE)
            #define _topen _wopen
        #else
            #define _topen _open
        #endif

        #define _lseek lseek
        #define _read read
        #define _close close
    #endif  // WIN32

    #include <fcntl.h>
    #include <stdio.h>


    #define DEF_SIGN_BUFFER 58

    namespace {

        // check if file signature is correct
        bool getSignature5FromFile(const KLUPD::Path &fullFileName, KLUPD::NoCaseString &signBuffer, KLUPD::Log *pLog)
        {
            KLUPD::LocalFile signedFile(fullFileName, pLog);
            std::vector<unsigned char> signedFileBuffer;
            const KLUPD::CoreError fileReadResult = signedFile.read(signedFileBuffer);
                
            if(!KLUPD::isSuccess(fileReadResult))
            {
                TRACE_MESSAGE3("Failed to get signature from file '%S', result '%S'",
                    fullFileName.toWideChar(), KLUPD::toString(fileReadResult).toWideChar());
                return false;
            }

            if(signedFileBuffer.size() < DEF_SIGN_BUFFER + 2)
            {
                TRACE_MESSAGE3("Failed get signature from file '%S', file does not contain signature, file size %d",
                    fullFileName.toWideChar(), signedFileBuffer.size());
                return false;
            }

            const std::string asciiSignature(reinterpret_cast<const char *>(&signedFileBuffer[0] + (signedFileBuffer.size() - (DEF_SIGN_BUFFER + 2))),
                DEF_SIGN_BUFFER);
            signBuffer = KLUPD::asciiToWideChar(asciiSignature);
            return true;
        }
    }   // namespace

#endif  // KLUPD_ENABLE_SIGNATURE_5


bool KLUPD::checkInsideSignature5(const Path &fullFileName)
{
    #ifdef KLUPD_ENABLE_SIGNATURE_5
        #if (defined(UNICODE) || defined(_UNICODE))
            #ifdef WSTRING_SUPPORTED
                return sign_check_file(fullFileName.toWideChar(), 1, 0, 0, 0) == SIGN_OK;
            #else
                #error signature 5 support can only be enabled if
                #error    1. both UNICODE _UNICODE macros are defined to gether with WSTRING_SUPPORTED macro defined
                #error    2. neither UNICODE, nor _UNICODE macros defined
                    stop here
            #endif
        #else
            return sign_check_file(fullFileName.toAscii().c_str(), 1, 0, 0, 0) == SIGN_OK;
        #endif
    #else
        return false;
    #endif
}

bool KLUPD::checkBufferSignature5(const std::vector<unsigned char> &buffer)
{
    #ifdef KLUPD_ENABLE_SIGNATURE_5
        return sign_check_buffer(buffer.empty() ? 0 : reinterpret_cast<void *>(const_cast<unsigned char *>(&buffer[0])),
            buffer.size(), 0, 1, 0, 0, 0) == SIGN_OK;
    #else
        return false;
    #endif
}


bool KLUPD::checkInsideSignature5AgainstEtalon(const Path &fullFileName, const NoCaseString &sign, Log *pLog)
{
    #ifdef KLUPD_ENABLE_SIGNATURE_5
        // unable to check signature 5 against etalon, because etalon signature is not specified
        if(sign.empty())
            return false;

        NoCaseString signBuffer;
        if(!getSignature5FromFile(fullFileName, signBuffer, pLog))
            return false;

        // compare signature 5 against etalon
        if(sign != signBuffer)
        {
            TRACE_MESSAGE4("Wrong signature 5 for file '%S', actually '%S' expected '%S'",
                fullFileName.toWideChar(), signBuffer.toWideChar(), sign.toWideChar());
            return false;
        }

        if(!checkInsideSignature5(fullFileName))
        {
            TRACE_MESSAGE4("Inside signature 5 check against etalon failed for file '%S', actually '%S' expected '%S'",
                fullFileName.toWideChar(), signBuffer.toWideChar(), sign.toWideChar());
            return false;
        }

        return true;

    #else
        return false;
    #endif  // KLUPD_ENABLE_SIGNATURE_5
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/// signature 6
class KLUPD::Signature6Checker::Implementation
{
public:
    typedef std::vector<unsigned char> Buffer;
    enum HashState
    {
        // hash only exists in memory, but was not added to Dskm library
        used,
        // hash has already been added to Dskm library
        notUsed,
    };

    // a wrapper to simplify HDSKMLIST memory management
    class DskmListWrapper
    {
        typedef std::map<int, std::string> MapKeyParams;
        typedef std::map<std::string, MapKeyParams> MapObjectKeyParams;
        MapObjectKeyParams m_mapObjectKeyParams;

    public:
        DskmListWrapper(Log *);
        ~DskmListWrapper();

        void clearList(HDSKMLIST &)const;
        // this funciton introduced to detalize trace. Ussually error on list it is not critical,
          // it may happen if damaged object added to list. But in some cases it may be critical
         // for update that is why output to trace helps
        void outputNonCriticalListErrorToTrace(HDSKMLIST &list, const std::string &additionalInformation)const;

        // prepare list is only performed in case registry changes,
         // because performance penalties due to crypto operations
        bool prepareRegistryList(NoCaseString &errorMessage);

        bool _CreateFileList(const std::vector<unsigned char>& buffer, const char* szFilename, const MapKeyParams& mapKeyParams, NoCaseString& strErrMsg);

        bool checkSignature(const std::vector<unsigned char> &buffer, const Path &fileNameForHash, bool &insideSignature, NoCaseString &errorMessage);

        bool m_registrySetModified;
        // map is used to avoid buffer re-allocation, because Dskm library stores a pointer to buffers,
         // bool specifies whether registry buffer has already been used, to avoid add of already added buffers
        std::map<Buffer, HashState> m_registrySet;

        void SetObjectType(const char* szObjectName, const char* szObjectType);

        // in same cases signature check traces should be disabled
        Log *pLog;

    private:
        // disable copy operations
        DskmListWrapper &operator=(const DskmListWrapper &);
        DskmListWrapper(const DskmListWrapper &);

        HDSKM m_dskmLibraryHandle;
        size_t m_uniqueFileIdentifier;
        // list for operations with registry set
        HDSKMLIST m_registryList;
        // list for operations with file
        HDSKMLIST m_fileList;
    };


    Implementation(Log *);

    // list for signature checks
    DskmListWrapper m_dskmList;
    Log *pLog;

private:
    // disable copy operations
    Implementation &operator=(const Implementation &);
    Implementation(const Implementation &);
};

//////////////////////////////////////////////////////////////////////////
/// Signature6Checker::Implementation::DskmListWrapper

KLUPD::Signature6Checker::Implementation::DskmListWrapper::DskmListWrapper(Log *log)
 : m_registrySetModified(false),
   pLog(log),
   m_dskmLibraryHandle(DSKM_InitLibraryEx(malloc, free,
        0,  // no file input output operations, thus 0 - file IO interface
        true)),
   m_uniqueFileIdentifier(0),
   m_registryList(0),
   m_fileList(0)
{
    if(m_dskmLibraryHandle)
    {
        DSKM_ParList_Create(&m_registryList);
        if(!m_registryList)
            TRACE_MESSAGE("Failed to create DSKM parameters list for signature check");
    }
    else
        TRACE_MESSAGE("Failed to initialize DSKM library for signature check");
}

KLUPD::Signature6Checker::Implementation::DskmListWrapper::~DskmListWrapper()
{
    clearList(m_registryList);
    clearList(m_fileList);

    if(m_dskmLibraryHandle)
    {
        const AVP_dword deinitializationLibraryRestult = DSKM_DeInitLibrary(m_dskmLibraryHandle, false);
        if(deinitializationLibraryRestult != DSKM_ERR_OK)
        {
            TRACE_MESSAGE2("Warning: failed to de-initialize DSKM library, result '%S'",
                toStringDskmCode(deinitializationLibraryRestult).toWideChar());
        }
    }
}

bool KLUPD::Signature6Checker::Implementation::DskmListWrapper::prepareRegistryList(NoCaseString &errorMessage)
{
    if(!m_registryList)
    {
        TRACE_MESSAGE("Failed to create DSKM parameters list for signature check");
        return false;
    }

    for(std::map<Buffer, HashState>::iterator registryIter = m_registrySet.begin(); registryIter != m_registrySet.end(); ++registryIter)
    {
        // skip already added hash
        if(registryIter->second == used)
            continue;

        if(registryIter->first.empty())
            continue;

        HDSKMLISTOBJ hReg =
            DSKM_ParList_AddBufferedReg( m_registryList, 0,
                                         reinterpret_cast<void *>(const_cast<unsigned char *>(&(registryIter->first)[0])),
                                         registryIter->first.size(), 0, 0 );
        if(!hReg)
        {
            errorMessage += NoCaseString(L"Signature 6 check error: failed to add registry to set, result '")
                + toStringDskmCode(DSKM_ParList_GetLastError(m_registryList)) + L"'";
            return false;
        }
        // hash is used already
        registryIter->second = used;
    }

    // objectsType parameter is ignored by DSKM library
    const int objectsType = 0;
    // provide new registry set to Dskm library
    const AVP_dword prepareRegistrySetResult = DSKM_PrepareRegsSet(m_dskmLibraryHandle, m_registryList, objectsType);
    if(prepareRegistrySetResult != DSKM_ERR_OK)
    {
        errorMessage += NoCaseString(L"Signature 6 check error: failed to prepare registry set, result '")
            + toStringDskmCode(prepareRegistrySetResult) + L"'";
        return false;
    }

    outputNonCriticalListErrorToTrace(m_registryList, "preparing");

    return true;
}
void KLUPD::Signature6Checker::Implementation::DskmListWrapper::clearList(HDSKMLIST &list)const
{
    if(list)
    {
        const AVP_dword deleteDskmListRestult = DSKM_ParList_Delete(list);
        if(deleteDskmListRestult != DSKM_ERR_OK)
        {
            TRACE_MESSAGE2("Warning: delete DSKM list error, result '%S'",
                toStringDskmCode(deleteDskmListRestult).toWideChar());
        }
        list = 0;
    }
}

void KLUPD::Signature6Checker::Implementation::DskmListWrapper::outputNonCriticalListErrorToTrace(HDSKMLIST &list, const std::string &additionalInformation)const
{
    AVP_dword result = DSKM_ERR_OK;
    for(AVP_dword objectIdentifier = DSKM_ParList_GetFirstObjectError(list, &result);
        objectIdentifier;
        objectIdentifier = DSKM_ParList_GetNextObjectError(list, objectIdentifier, &result))
    {
        if((result != DSKM_ERR_OK)
            && (result != DSKM_ERR_LIB_CODE_HACKED))
        {
            TRACE_MESSAGE4("Warning: DSKM list is not correct (%s), object identifier %d, result '%S'",
                additionalInformation.c_str(), objectIdentifier, toStringDskmCode(result).toWideChar());
        }
    }
}

bool KLUPD::Signature6Checker::Implementation::DskmListWrapper::_CreateFileList(const std::vector<unsigned char>& buffer, const char* szFilename, const MapKeyParams& mapKeyParams, NoCaseString& strErrMsg)
{
    clearList(m_fileList);
    DSKM_ParList_Create(&m_fileList);
    if(!m_fileList)
    {
        strErrMsg += NoCaseString(L" Signature 6 check error: unable to create DSKM list, result '")
            + toStringDskmCode(DSKM_ParList_GetLastError(m_fileList));
        return false;
    }

    for(MapKeyParams::const_iterator iter = mapKeyParams.begin(); iter != mapKeyParams.end(); ++iter)
        DSKM_ParList_AddStrParam(m_fileList, iter->first, iter->second.c_str());

    // adding file buffer
    HDSKMLISTOBJ bufferedObject = DSKM_ParList_AddBufferedObject(m_fileList, ++m_uniqueFileIdentifier,
        buffer.empty() ? 0 : const_cast<unsigned char *>(&buffer[0]), buffer.size(), 0, 0);

    if(!bufferedObject)
    {
        strErrMsg += NoCaseString(L" Signature 6 check error: failed to add buffered file to DSKM library, result '")
            + toStringDskmCode(DSKM_ParList_GetLastError(m_fileList));
        return false;
    }
    outputNonCriticalListErrorToTrace(m_fileList, "adding object");


    std::vector<DSKM_WCHAR_T> multibyteBuffer;
    convertStringToDskmMultibyte(szFilename, multibyteBuffer);

    AVP_dword addFileNameHashingPropertyResult = DSKM_ParList_SetObjectHashingProp(
        m_fileList,
        bufferedObject,
        reinterpret_cast<void *>(&multibyteBuffer[0]),
        multibyteBuffer.size() * sizeof(DSKM_WCHAR_T));
    if(addFileNameHashingPropertyResult != DSKM_ERR_OK)
    {
        strErrMsg += NoCaseString(L" Signature 6 check error: failed to set hashing property file name, result '")
            + toStringDskmCode(addFileNameHashingPropertyResult);
        return false;
    }

    return true;
}

bool KLUPD::Signature6Checker::Implementation::DskmListWrapper::checkSignature(const std::vector<unsigned char> &buffer,
    const Path &fileNameForHash, bool &insideSignature, NoCaseString &errorMessage)
{
    if(!m_dskmLibraryHandle)
    {
        TRACE_MESSAGE("Signature 6 check error: library initialization error");
        return false;
    }

    if(m_registrySet.empty())
    {
        errorMessage += L" Signature 6 check error: registry set is empty";
        return false;
    }

    if(m_registrySetModified)
    {
        if(!prepareRegistryList(errorMessage))
            return false;

        m_registrySetModified = false;
    }

    // adding lowercase UNICODE file name to library
    std::string lowercaseFileName = fileNameForHash.getFileNameFromPath().toAscii();
    std::transform(lowercaseFileName.begin(), lowercaseFileName.end(), lowercaseFileName.begin(), tolower);

    MapObjectKeyParams::const_iterator iter = m_mapObjectKeyParams.find(lowercaseFileName);
    const MapKeyParams& mapKeyParams = iter != m_mapObjectKeyParams.end() ? iter->second : MapKeyParams();

    if( !_CreateFileList(buffer, lowercaseFileName.c_str(), mapKeyParams, errorMessage) )
        return false;

    const int objectsType = 0;  // objectsType parameter is ignored by DSKM library
    AVP_dword checkResult = DSKM_CheckObjectsUsingRegsSet(m_dskmLibraryHandle, m_fileList, objectsType);

    if(checkResult != DSKM_ERR_OK && !mapKeyParams.empty())
        // if signature check with key params failed, try once again without
        if(_CreateFileList(buffer, lowercaseFileName.c_str(), MapKeyParams(), errorMessage))
            checkResult = DSKM_CheckObjectsUsingRegsSet(m_dskmLibraryHandle, m_fileList, objectsType);

    if(checkResult == DSKM_ERR_OK)
    {
        // to reduce CPU load
        usleep(2000);

        // determine whether inside-signature 6 check was used
        HDSKMLISTOBJ object = DSKM_ParList_GetObject(m_fileList, m_uniqueFileIdentifier, 0, 0);
        if(!object)
        {
            TRACE_MESSAGE3("Signature 6 check error: failed to get object with identifier %d from list, error '%S'",
                m_uniqueFileIdentifier, toStringDskmCode(DSKM_ParList_GetLastError(m_fileList)).toWideChar());
            return false;
        }

        outputNonCriticalListErrorToTrace(m_fileList, "checking if inside signature");


        AVP_dword signedBy = -1;
        AVP_dword propertySize = sizeof(signedBy);
        const AVP_dword getSignedByPropertyResult = DSKM_ParList_GetObjectProp(m_fileList, object, DSKM_OBJ_SIGNED_BY, &signedBy, &propertySize);
        if(getSignedByPropertyResult != DSKM_ERR_OK)
        {
            TRACE_MESSAGE2("Signature 6 check error: failed to determine if file is signed inside or not, unexpected code %S",
                toStringDskmCode(getSignedByPropertyResult).toWideChar());
            return false;
        }
        if(propertySize != sizeof(signedBy))
        {
            TRACE_MESSAGE3("Signature 6 check error: unexpected signature type property size %d, expected %d", propertySize, sizeof(signedBy));
            return false;
        }

        if(signedBy == DSKM_OBJ_SIGN_INOBJ)
            insideSignature = true;
        else if(signedBy == DSKM_OBJ_SIGN_INREG)
            insideSignature = false;
        else
        {
            TRACE_MESSAGE2("Signature 6 check error: unexpected signature type %d", signedBy);
            return false;
        }

        return true;
    }


    // expected code for signature failure is DSKM_ERR_NOT_ALL_OBJECTS_FOUND
    errorMessage += L" Signature 6 check failed";
    if(checkResult != DSKM_ERR_NOT_ALL_OBJECTS_FOUND)
        errorMessage += NoCaseString(L", generic result '") + toStringDskmCode(checkResult) + L"'";


    AVP_dword firstErrorObject = -1;
    // expected error is DSKM_ERR_INVALID_SIGN
    const AVP_dword error = DSKM_ParList_GetFirstObjectError(m_fileList, &firstErrorObject);
    if(firstErrorObject != DSKM_ERR_INVALID_SIGN)
    {
        errorMessage += NoCaseString(L", result '") + toStringDskmCode(firstErrorObject)
            + L"', get first object call result " + toStringDskmCode(error);
    }
    
    return false;
}

void KLUPD::Signature6Checker::Implementation::DskmListWrapper::SetObjectType(const char* szObjectName, const char* szObjectType)
{
    MapKeyParams& params = m_mapObjectKeyParams[szObjectName];
    params.clear();

    const char* p = szObjectType;
    if(*p != '\0')
        while(true)
        {
            const char* p0 = p;
            while(*p != '\0' && *p != '=' && *p != ';') ++p;
            if(*p != '=')
                continue;
            
            std::string strId(p0, p);

            p0 = ++p;
            while(*p != '\0' && *p != ';') ++p;

            std::string strParam(p0, p);

            if( !strId.empty() && !strParam.empty() )
                params[ atoi(strId.c_str()) ] = strParam;

            if(*p == '\0') break;
            ++p;
        }
}

//////////////////////////////////////////////////////////////////////////
/// Signature6Checker::Implementation
KLUPD::Signature6Checker::Implementation::Implementation(Log *log)
 : m_dskmList(log),
   pLog(log)
{
}


//////////////////////////////////////////////////////////////////////////
/// Signature6Checker
KLUPD::Signature6Checker::Signature6Checker(Log *log)
 : pLog(log),
   m_implementation(new Implementation(log))
{
}
KLUPD::Signature6Checker::~Signature6Checker()
{
    delete m_implementation;
}

void KLUPD::Signature6Checker::addDataForSignature6Check(const std::vector<unsigned char> &data)
{
    // skip empty data
    if(data.empty())
        return;

    // filter duplicates
    std::map<Implementation::Buffer, Implementation::HashState>::const_iterator registryIter = m_implementation->m_dskmList.m_registrySet.find(data);
    if(registryIter != m_implementation->m_dskmList.m_registrySet.end())
        return;

    // duplicate not found, adding registry
    m_implementation->m_dskmList.m_registrySetModified = true;
    m_implementation->m_dskmList.m_registrySet[data] = Implementation::notUsed;
}

bool KLUPD::Signature6Checker::checkSignature(const Path &fullFileName, bool &insideSignature, NoCaseString &errorMessage)const
{
    // in some cases signature checks traces should be suppressed
    Log *saveSignatureCheckerLog = m_implementation->m_dskmList.pLog;
    saveSignatureCheckerLog = pLog;

    bool result = false;

    // adding file buffer
    std::vector<unsigned char> buffer;
    const CoreError readFileResult = (LocalFile(fullFileName, pLog)).read(buffer);
    if(isSuccess(readFileResult))
    {
        result = m_implementation->m_dskmList.checkSignature(buffer, fullFileName, insideSignature, errorMessage);
        if(!result)
            errorMessage += NoCaseString(L". Current file '") + fullFileName.toWideChar() + L"'";
    }
    else
    {
        errorMessage += NoCaseString(L"Signature 6 check error: unable to read content of file '")
            + fullFileName.toWideChar() + L"', result '" + toString(readFileResult) + L"'";
    }


    // restore log
    m_implementation->m_dskmList.pLog = saveSignatureCheckerLog;

    return result;
}

bool KLUPD::Signature6Checker::checkSignature(const std::vector<unsigned char> &buffer, const Path &fileNameForHash, bool &insideSignature, NoCaseString &errorMessage)const
{
    // in some cases signature checks traces should be suppressed
    Log *saveSignatureCheckerLog = m_implementation->m_dskmList.pLog;
    saveSignatureCheckerLog = pLog;

    const bool result = m_implementation->m_dskmList.checkSignature(buffer, fileNameForHash, insideSignature, errorMessage);

    // restore log
    m_implementation->m_dskmList.pLog = saveSignatureCheckerLog;

    return result;
}

void KLUPD::Signature6Checker::loadKlAndOperableKeysFromPrimaryIndex( const NoCaseString& strFilename,
                                                                      const std::vector<unsigned char> &fileBuffer,
                                                                      CallbackInterface &callbacks )
{
    if(fileBuffer.empty())
    {
        TRACE_MESSAGE("Failed to read signature 6 first and second level keys, buffer is corrupted");
        return;
    }

    // a file list obtained from XML
    FileVector fileList;
    const bool retranslationModeNoMatter = false;
    IndexFileXMLParser xmler( strFilename, fileList, *this, Path(),
                             retranslationModeNoMatter, callbacks, pLog );

    XmlReader xmlReader(reinterpret_cast<const char *>(&fileBuffer[0]), fileBuffer.size());
    if(!xmler.parseXMLRecursively(xmlReader, 0))
    {
        TRACE_MESSAGE("Failed to read signature 6 first and second level keys, XML parse error");
        return;
    }
    m_implementation->m_dskmList.m_registrySetModified = true;
}

KLUPD::NoCaseString KLUPD::Signature6Checker::toStringDskmCode(const unsigned long code)
{
    switch(code)
    {
    case DSKM_ERR_OK: 
        return L"success";
    case DSKM_ERR_NOT_INITIALIZED: 
        return L"not initialized";
    case DSKM_ERR_CANNOT_CREATE_REG_FILE:
        return L"Failed to create registry file";
    case DSKM_ERR_CANNOT_WRITE_REG_FILE:
        return L"Failed to write registry file";
    case DSKM_ERR_CANNOT_WRITE_REG_TOMEM:
        return L"Failed to write registry to memory";
    case DSKM_ERR_CANNOT_OPEN_REG_FILE:
        return L"Failed to open registry file";
    case DSKM_ERR_CANNOT_READ_REG_FILE:
        return L"Can not read registry file";
    case DSKM_ERR_INVALID_REG_FILE:
        return L"Invalid registry file";
    case DSKM_ERR_INVALID_REG_FILE_VERSION:
        return L"Invalid registry file version";
    case DSKM_ERR_INVALID_REG_FILE_OBJTYPE:
        return L"Invalid registry file object type";
    case DSKM_ERR_KEY_NOT_FOUND:
        return L"Key is not found";
    case DSKM_ERR_REG_NOT_FOUND:
        return L"Registry is not found";
    case DSKM_ERR_CANNOT_CALC_OBJ_HASH:
        return L"Failed to calculate object hash";
    case DSKM_ERR_NOT_ALL_OBJECTS_FOUND:
        return L"Error while processing object list";
    case DSKM_ERR_INVALID_SIGN:
        return L"Signature is not valid";
    case DSKM_ERR_INVALID_BUFFER:
        return L"Invalid buffer";
    case DSKM_ERR_INVALID_PARAMETER:
        return L"Invalid parameter";
    case DSKM_ERR_INVALID_OBJTYPE:
        return L"Invalid object type";
    case DSKM_ERR_DATA_PROCESSING:
        return L"Data processing error";
    case DSKM_ERR_HASH_NOT_FOUND:
        return L"Hash is not found";
    case DSKM_ERR_ANOTHER_BASE_REG_FOUND:
        return L"Another base registry found";
    case DSKM_ERR_LIB_CODE_HACKED:
        return L"Object has not been checked yet";   // warning, not error
    case DSKM_ERR_CANNOT_CREATE_SIGN:
        return L"Failed to create signature";
    case DSKM_ERR_CRIPTO_LIB:
        return L"Crypto library error";
    case DSKM_ERR_OBJECT_NOT_FOUND:
        return L"Object is not found";
    case DSKM_ERR_CALLBACK_READ_ERROR:
        return L"Callback read error";
    case DSKM_ERR_CANNOT_OPEN_FILE:
        return L"Failed to open file";
    case DSKM_ERR_IO_NOT_INITIALIZED:
        return L"Object is not initialized";
    case DSKM_ERR_REGS_NOT_INITIALIZED:
        return L"Registry set is not initialized";
    case DSKM_ERR_MORE_THAN_ONE_KEY:
        return L"More then one key";
    case DSKM_ERR_SIGN_NOT_FOUND:
        return L"Signature is not found";
    case DSKM_ERR_ANOTHER_SIGN_FOUND:
        return L"Another signature is found";
    case DSKM_ERR_INVALID_SIZE:
        return L"Size is invalid";
    case DSKM_ERR_PROP_NOT_FOUND:
        return L"Property is not found";
    case DSKM_ERR_OBJECTS_NOT_EQUAL:
        return L"Object is not equal";
    case DSKM_UNKNOWN_ERROR:
        return L"Unknown error";

    default:
        std::ostringstream stream;
        stream.imbue(std::locale::classic());
        stream << "Unknown Dskm code " << code;
        return asciiToWideChar(stream.str());
    }
}

void KLUPD::Signature6Checker::SetObjectType(const char* szObjectName, const char* szObjectType)
{
    if(m_implementation)
        m_implementation->m_dskmList.SetObjectType(szObjectName, szObjectType);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool KLUPD::checkSignature5And6AndMd5(const Path &fullFileName, const FileInfo &etalon,
    Signature6Checker &signature6Checker, NoCaseString &signature6ErrorMessage)
{
    // check md5
    if(!etalon.m_md5.empty())
    {
        std::vector<unsigned char> md5;
        if(calcMD5Hash(fullFileName, md5) && (md5 == etalon.m_md5))
            return true;
    }

    // check 5 and 6 signature
    bool insideSignature = false;
    return checkInsideSignature5(fullFileName)
        || signature6Checker.checkSignature(fullFileName, insideSignature, signature6ErrorMessage);
}

bool KLUPD::checkSignature5And6AndMd5AgainstEtalon(const Path &fullFileName, const FileInfo &etalon,
    Signature6Checker &signature6Checker, NoCaseString &signature6ErrorMessage,
    const InsideSignatureCheckBehaviour &insideSignatureCheckBehaviour,
    CallbackInterface &callbacks, Log *pLog)
{
    if(etalon.isInlined())
    {
        signature6ErrorMessage += L" inlined file's signature checked on index file signature check";
        return true;
    }

    // check against etalon signature 5
    if(checkInsideSignature5AgainstEtalon(fullFileName, etalon.m_signature5, pLog))
        return true;

    // check md5
    if(!etalon.m_md5.empty())
    {
        std::vector<unsigned char> md5;
        if(calcMD5Hash(fullFileName, md5) && md5 == etalon.m_md5)
            return true;
    }

    // file is expected to be consistent if signature or MD5 field is not empty
    if(!etalon.m_signature5.empty() || !etalon.m_md5.empty())
    {
        signature6ErrorMessage += L" signature 5 or md5 etalon specified, but check failed";
        return false;
    }


    // read file into buffer
    std::vector<unsigned char> fileBuffer;
    const KLUPD::CoreError fileReadResult = LocalFile(fullFileName, pLog).read(fileBuffer);
    if(!KLUPD::isSuccess(fileReadResult))
    {
        signature6ErrorMessage += NoCaseString(L" signature check error, unable to read file '")
            + fullFileName.toWideChar() + L"', result " + KLUPD::toString(fileReadResult);
        return false;
    }
    // signature check failed
    bool insideSignature = false;
    if(!signature6Checker.checkSignature(fileBuffer, fullFileName, insideSignature, signature6ErrorMessage))
    {
        if(insideSignatureCheckBehaviour == disableInsideSignature)
        {
            signature6ErrorMessage += L" signature is valid, bug inside signature is disabled";
            return false;
        }

        if(insideSignatureCheckBehaviour == enableInsideSignatureWithLoadKey)
        {
            signature6Checker.loadKlAndOperableKeysFromPrimaryIndex(fullFileName.getFileNameFromPath().m_value, fileBuffer, callbacks);
            if(signature6Checker.checkSignature(fileBuffer, fullFileName, insideSignature, signature6ErrorMessage))
                return true;
        }

        // try inside signature 5 which is only allowed for index files
        if((etalon.m_type == FileInfo::index) && checkInsideSignature5(fullFileName))
            return true;
        signature6ErrorMessage += L" non-index or inside signature 5 failed";
        return false;
    }

    // inside signature 6 is does not suit, because can not determine there is
     // no possibility to determine whether file belongs to some exact update set
    if(!insideSignature)
        return true;

    // inside signature for non-index files is error
    if(etalon.m_type != FileInfo::index)
    {
        signature6ErrorMessage += L" inside signature for non-index files is error";
        return false;
    }

    if(insideSignatureCheckBehaviour == disableInsideSignature)
    {
        signature6ErrorMessage += L" inside signature is not allowed for current operation";
        return false;
    }

    return true;
}

bool KLUPD::checkFileMd5OrPrimaryIndexSignature5And6(const Path &fullFileName, const FileInfo &etalon,
    Signature6Checker &signature6Checker, NoCaseString &signature6ErrorMessage, Log *pLog)
{
    if(etalon.isInlined())
    {
        signature6ErrorMessage += L" inlined file's signature checked on index file signature check";
        return true;
    }

    // check against etalon signature 5
    if(checkInsideSignature5(fullFileName))
        return true;

    // check md5
    if(!etalon.m_md5.empty())
    {
        std::vector<unsigned char> md5;
        if(calcMD5Hash(fullFileName, md5) && md5 == etalon.m_md5)
            return true;
    }


    // primary index contains inside signature 6
    bool insideSignature = false;
    return signature6Checker.checkSignature(fullFileName, insideSignature, signature6ErrorMessage);
}

bool KLUPD::checkBlackListPresence(const FileVector &files, unsigned long &blackListDateLimit, Log *pLog)
{
    NoCaseString dateForTrace;
    unsigned long solvePeriodForTrace = 0;

    // search the *latest* update threshold, for the most actual black list file
    blackListDateLimit = 0;
    for(FileVector::const_iterator fileIter = files.begin(); fileIter != files.end(); ++fileIter)
    {
        if(fileIter->m_type == FileInfo::blackList)
        {
            // get threshold from black list item
            const unsigned long currentBlackListDateLimit
                = addDays(updateDateStringToSequenceNumber(fileIter->m_dateFromIndex), fileIter->m_solvePeriodDays);

            // first black list in set
            if(!blackListDateLimit)
            {
                blackListDateLimit = currentBlackListDateLimit;
                dateForTrace = fileIter->m_dateFromIndex;
                solvePeriodForTrace = fileIter->m_solvePeriodDays;
            }

            if(blackListDateLimit < currentBlackListDateLimit)
            {
                blackListDateLimit = currentBlackListDateLimit;
                dateForTrace = fileIter->m_dateFromIndex;
                solvePeriodForTrace = fileIter->m_solvePeriodDays;
            }
        }
    }
    if(!blackListDateLimit)
    {
        TRACE_MESSAGE("Check black list date failed, black list with date must present in update set");
        return false;
    }

    TRACE_MESSAGE3("Most recent black list date '%S', solve period %d days",
        dateForTrace.toWideChar(), solvePeriodForTrace);
    return true;
}
