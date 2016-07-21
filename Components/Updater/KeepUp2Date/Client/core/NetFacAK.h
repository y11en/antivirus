#ifndef NETFACAK_H_CEE037E9_460C_48e5_924F_756C736BD176
#define NETFACAK_H_CEE037E9_460C_48e5_924F_756C736BD176


#include "../helper/comdefs.h"

// forward declaration
namespace KLFT
{
    class FileTransferBridge;
}

namespace KLUPD {

class KAVUPDCORE_API AdministrationKitProtocol
{
public:
    AdministrationKitProtocol(DownloadProgress &, Log *);
    ~AdministrationKitProtocol();

	// provide ability to force close connection thus reduce load on server
    void closeSession();

    CoreError getFile(const Path &fileName, const Path &localPath, const Path &relativeUrlPath,
        const bool useMasterAdministrationServer);

private:
    // disable copy operations
    AdministrationKitProtocol &operator=(const AdministrationKitProtocol &);
    AdministrationKitProtocol(const AdministrationKitProtocol &);

    CoreError setupLowLevelConnectionIfNeed(const bool useMasterAdministrationServer);

    // AdminKit transport
    KLFT::FileTransferBridge *m_adminKitTransprot;

    bool m_connected;
    DownloadProgress &m_downloadProgress;
    Log *pLog;
};

}   // namespace KLUPD

#endif  // NETFACAK_H_CEE037E9_460C_48e5_924F_756C736BD176
