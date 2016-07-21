//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __msoeapi_h__
#define __msoeapi_h__

#ifdef __cplusplus
extern "C"{
#endif




/* Forward Declarations */
#ifndef __IStoreNamespace_FWD_DEFINED__
#define __IStoreNamespace_FWD_DEFINED__
typedef interface IStoreNamespace IStoreNamespace;
#endif 	/* __IStoreNamespace_FWD_DEFINED__ */


#ifndef __IStoreFolder_FWD_DEFINED__
#define __IStoreFolder_FWD_DEFINED__
typedef interface IStoreFolder IStoreFolder;
#endif 	/* __IStoreFolder_FWD_DEFINED__ */



// {E70C92A9-4BFD-11d1-8A95-00C04FB951F3}
DEFINE_GUID(CLSID_StoreNamespace, 0xe70c92a9, 0x4bfd, 0x11d1, 0x8a, 0x95, 0x0, 0xc0, 0x4f, 0xb9, 0x51, 0xf3);

// {E70C92AA-4BFD-11d1-8A95-00C04FB951F3}
DEFINE_GUID(IID_IStoreNamespace, 0xe70c92aa, 0x4bfd, 0x11d1, 0x8a, 0x95, 0x0, 0xc0, 0x4f, 0xb9, 0x51, 0xf3);

// {E70C92AC-4BFD-11d1-8A95-00C04FB951F3}
DEFINE_GUID(IID_IStoreFolder, 0xe70c92ac, 0x4bfd, 0x11d1, 0x8a, 0x95, 0x0, 0xc0, 0x4f, 0xb9, 0x51, 0xf3);

#ifndef __LPSTORENAMESPACE_DEFINED
#define __LPSTORENAMESPACE_DEFINED


typedef struct HENUMSTORE__ *HENUMSTORE;

typedef HENUMSTORE __RPC_FAR *LPHENUMSTORE;

#ifndef __IStoreNamespace_INTERFACE_DEFINED__
#define __IStoreNamespace_INTERFACE_DEFINED__



typedef IStoreNamespace __RPC_FAR *LPSTORENAMESPACE;

typedef DWORD STOREFOLDERID;

typedef STOREFOLDERID __RPC_FAR *LPSTOREFOLDERID;

#define	FOLDERID_ROOT	( 0 )

#define	FOLDERID_INVALID	( 0xffffffff )

typedef DWORD MESSAGEID;

typedef MESSAGEID __RPC_FAR *LPMESSAGEID;

#define	MESSAGEID_INVALID	( 0xffffffff )

#define	MESSAGEID_FIRST	( 0xffffffff )

#define	CCHMAX_FOLDER_NAME	( 256 )


typedef struct tagFOLDERNOTIFYEX __RPC_FAR *LPFOLDERNOTIFYEX;

typedef struct  tagMESSAGEIDLIST
    {
    DWORD cbSize;
    DWORD cMsgs;
    LPMESSAGEID prgdwMsgId;
    }	MESSAGEIDLIST;

typedef struct tagMESSAGEIDLIST __RPC_FAR *LPMESSAGEIDLIST;

typedef
enum tagSPECIALFOLDER
    {	FOLDER_NOTSPECIAL	= -1,
	FOLDER_INBOX	= FOLDER_NOTSPECIAL + 1,
	FOLDER_OUTBOX	= FOLDER_INBOX + 1,
	FOLDER_SENT	= FOLDER_OUTBOX + 1,
	FOLDER_DELETED	= FOLDER_SENT + 1,
	FOLDER_DRAFT	= FOLDER_DELETED + 1,
	FOLDER_MAX	= FOLDER_DRAFT + 1,

    }	SPECIALFOLDER;

typedef struct  tagFOLDERPROPS
    {
    DWORD cbSize;
    STOREFOLDERID dwFolderId;
    INT cSubFolders;
    SPECIALFOLDER sfType;
    DWORD cUnread;
    DWORD cMessage;
    CHAR szName[ 256 ];
    }	FOLDERPROPS;

typedef struct tagFOLDERPROPS __RPC_FAR *LPFOLDERPROPS;

#define CMF_MOVE                     0x0001                  // msgs deleted from src fldr after copy
#define CMF_DELETE                   0x0002                  // same as CMF_MOVE but uses delete string for status

EXTERN_C const IID IID_IStoreNamespace;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("E70C92AA-4BFD-11d1-8A95-00C04FB951F3")
    IStoreNamespace : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize(
            /* [optional][in] */ HWND hwndOwner,
            /* [in] */ DWORD dwReserved) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetDirectory(
            /* [size_is][out][in] */ LPSTR pszPath,
            /* [in] */ DWORD cchMaxPath) = 0;

        virtual HRESULT STDMETHODCALLTYPE OpenSpecialFolder(
            /* [in] */ SPECIALFOLDER sfType,
            /* [in] */ DWORD dwReserved,
            /* [out] */ IStoreFolder __RPC_FAR *__RPC_FAR *ppFolder) = 0;

        virtual HRESULT STDMETHODCALLTYPE OpenFolder(
            /* [in] */ STOREFOLDERID dwFolderId,
            /* [in] */ DWORD dwReserved,
            /* [out] */ IStoreFolder __RPC_FAR *__RPC_FAR *ppFolder) = 0;

        virtual HRESULT STDMETHODCALLTYPE CreateFolder(
            /* [in] */ STOREFOLDERID dwParentId,
            /* [in] */ LPCSTR pszName,
            /* [in] */ DWORD dwReserved,
            /* [out] */ LPSTOREFOLDERID pdwFolderId) = 0;

        virtual HRESULT STDMETHODCALLTYPE RenameFolder(
            /* [in] */ STOREFOLDERID dwFolderId,
            /* [in] */ DWORD dwReserved,
            /* [in] */ LPCSTR pszNewName) = 0;

        virtual HRESULT STDMETHODCALLTYPE MoveFolder(
            /* [in] */ STOREFOLDERID dwFolderId,
            /* [in] */ STOREFOLDERID dwParentId,
            /* [in] */ DWORD dwReserved) = 0;

        virtual HRESULT STDMETHODCALLTYPE DeleteFolder(
            /* [in] */ STOREFOLDERID dwFolderId,
            /* [in] */ DWORD dwReserved) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetFolderProps(
            /* [in] */ STOREFOLDERID dwFolderId,
            /* [in] */ DWORD dwReserved,
            /* [out][in] */ LPFOLDERPROPS pProps) = 0;

        virtual HRESULT STDMETHODCALLTYPE CopyMoveMessages(
            /* [in] */ IStoreFolder __RPC_FAR *pSource,
            /* [in] */ IStoreFolder __RPC_FAR *pDest,
            /* [in] */ LPMESSAGEIDLIST pMsgIdList,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwFlagsRemove,
            /* [in] */ IProgressNotify __RPC_FAR *pProgress) = 0;

        virtual HRESULT STDMETHODCALLTYPE RegisterNotification(
            /* [in] */ DWORD dwReserved,
            /* [in] */ HWND hwnd) = 0;

        virtual HRESULT STDMETHODCALLTYPE UnregisterNotification(
            /* [in] */ DWORD dwReserved,
            /* [in] */ HWND hwnd) = 0;

        virtual HRESULT STDMETHODCALLTYPE CompactAll(
            /* [in] */ DWORD dwReserved) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetFirstSubFolder(
            /* [in] */ STOREFOLDERID dwFolderId,
            /* [out][in] */ LPFOLDERPROPS pProps,
            /* [out] */ LPHENUMSTORE phEnum) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetNextSubFolder(
            /* [in] */ HENUMSTORE hEnum,
            /* [out][in] */ LPFOLDERPROPS pProps) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetSubFolderClose(
            /* [in] */ HENUMSTORE hEnum) = 0;

    };

#else 	/* C style interface */

    typedef struct IStoreNamespaceVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            IStoreNamespace __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            IStoreNamespace __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            IStoreNamespace __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Initialize )(
            IStoreNamespace __RPC_FAR * This,
            /* [optional][in] */ HWND hwndOwner,
            /* [in] */ DWORD dwReserved);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDirectory )(
            IStoreNamespace __RPC_FAR * This,
            /* [size_is][out][in] */ LPSTR pszPath,
            /* [in] */ DWORD cchMaxPath);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OpenSpecialFolder )(
            IStoreNamespace __RPC_FAR * This,
            /* [in] */ SPECIALFOLDER sfType,
            /* [in] */ DWORD dwReserved,
            /* [out] */ IStoreFolder __RPC_FAR *__RPC_FAR *ppFolder);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OpenFolder )(
            IStoreNamespace __RPC_FAR * This,
            /* [in] */ STOREFOLDERID dwFolderId,
            /* [in] */ DWORD dwReserved,
            /* [out] */ IStoreFolder __RPC_FAR *__RPC_FAR *ppFolder);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateFolder )(
            IStoreNamespace __RPC_FAR * This,
            /* [in] */ STOREFOLDERID dwParentId,
            /* [in] */ LPCSTR pszName,
            /* [in] */ DWORD dwReserved,
            /* [out] */ LPSTOREFOLDERID pdwFolderId);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RenameFolder )(
            IStoreNamespace __RPC_FAR * This,
            /* [in] */ STOREFOLDERID dwFolderId,
            /* [in] */ DWORD dwReserved,
            /* [in] */ LPCSTR pszNewName);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveFolder )(
            IStoreNamespace __RPC_FAR * This,
            /* [in] */ STOREFOLDERID dwFolderId,
            /* [in] */ STOREFOLDERID dwParentId,
            /* [in] */ DWORD dwReserved);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteFolder )(
            IStoreNamespace __RPC_FAR * This,
            /* [in] */ STOREFOLDERID dwFolderId,
            /* [in] */ DWORD dwReserved);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFolderProps )(
            IStoreNamespace __RPC_FAR * This,
            /* [in] */ STOREFOLDERID dwFolderId,
            /* [in] */ DWORD dwReserved,
            /* [out][in] */ LPFOLDERPROPS pProps);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CopyMoveMessages )(
            IStoreNamespace __RPC_FAR * This,
            /* [in] */ IStoreFolder __RPC_FAR *pSource,
            /* [in] */ IStoreFolder __RPC_FAR *pDest,
            /* [in] */ LPMESSAGEIDLIST pMsgIdList,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwFlagsRemove,
            /* [in] */ IProgressNotify __RPC_FAR *pProgress);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RegisterNotification )(
            IStoreNamespace __RPC_FAR * This,
            /* [in] */ DWORD dwReserved,
            /* [in] */ HWND hwnd);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnregisterNotification )(
            IStoreNamespace __RPC_FAR * This,
            /* [in] */ DWORD dwReserved,
            /* [in] */ HWND hwnd);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CompactAll )(
            IStoreNamespace __RPC_FAR * This,
            /* [in] */ DWORD dwReserved);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFirstSubFolder )(
            IStoreNamespace __RPC_FAR * This,
            /* [in] */ STOREFOLDERID dwFolderId,
            /* [out][in] */ LPFOLDERPROPS pProps,
            /* [out] */ LPHENUMSTORE phEnum);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetNextSubFolder )(
            IStoreNamespace __RPC_FAR * This,
            /* [in] */ HENUMSTORE hEnum,
            /* [out][in] */ LPFOLDERPROPS pProps);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSubFolderClose )(
            IStoreNamespace __RPC_FAR * This,
            /* [in] */ HENUMSTORE hEnum);

        END_INTERFACE
    } IStoreNamespaceVtbl;

    interface IStoreNamespace
    {
        CONST_VTBL struct IStoreNamespaceVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IStoreNamespace_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IStoreNamespace_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IStoreNamespace_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IStoreNamespace_Initialize(This,hwndOwner,dwReserved)	\
    (This)->lpVtbl -> Initialize(This,hwndOwner,dwReserved)

#define IStoreNamespace_GetDirectory(This,pszPath,cchMaxPath)	\
    (This)->lpVtbl -> GetDirectory(This,pszPath,cchMaxPath)

#define IStoreNamespace_OpenSpecialFolder(This,sfType,dwReserved,ppFolder)	\
    (This)->lpVtbl -> OpenSpecialFolder(This,sfType,dwReserved,ppFolder)

#define IStoreNamespace_OpenFolder(This,dwFolderId,dwReserved,ppFolder)	\
    (This)->lpVtbl -> OpenFolder(This,dwFolderId,dwReserved,ppFolder)

#define IStoreNamespace_CreateFolder(This,dwParentId,pszName,dwReserved,pdwFolderId)	\
    (This)->lpVtbl -> CreateFolder(This,dwParentId,pszName,dwReserved,pdwFolderId)

#define IStoreNamespace_RenameFolder(This,dwFolderId,dwReserved,pszNewName)	\
    (This)->lpVtbl -> RenameFolder(This,dwFolderId,dwReserved,pszNewName)

#define IStoreNamespace_MoveFolder(This,dwFolderId,dwParentId,dwReserved)	\
    (This)->lpVtbl -> MoveFolder(This,dwFolderId,dwParentId,dwReserved)

#define IStoreNamespace_DeleteFolder(This,dwFolderId,dwReserved)	\
    (This)->lpVtbl -> DeleteFolder(This,dwFolderId,dwReserved)

#define IStoreNamespace_GetFolderProps(This,dwFolderId,dwReserved,pProps)	\
    (This)->lpVtbl -> GetFolderProps(This,dwFolderId,dwReserved,pProps)

#define IStoreNamespace_CopyMoveMessages(This,pSource,pDest,pMsgIdList,dwFlags,dwFlagsRemove,pProgress)	\
    (This)->lpVtbl -> CopyMoveMessages(This,pSource,pDest,pMsgIdList,dwFlags,dwFlagsRemove,pProgress)

#define IStoreNamespace_RegisterNotification(This,dwReserved,hwnd)	\
    (This)->lpVtbl -> RegisterNotification(This,dwReserved,hwnd)

#define IStoreNamespace_UnregisterNotification(This,dwReserved,hwnd)	\
    (This)->lpVtbl -> UnregisterNotification(This,dwReserved,hwnd)

#define IStoreNamespace_CompactAll(This,dwReserved)	\
    (This)->lpVtbl -> CompactAll(This,dwReserved)

#define IStoreNamespace_GetFirstSubFolder(This,dwFolderId,pProps,phEnum)	\
    (This)->lpVtbl -> GetFirstSubFolder(This,dwFolderId,pProps,phEnum)

#define IStoreNamespace_GetNextSubFolder(This,hEnum,pProps)	\
    (This)->lpVtbl -> GetNextSubFolder(This,hEnum,pProps)

#define IStoreNamespace_GetSubFolderClose(This,hEnum)	\
    (This)->lpVtbl -> GetSubFolderClose(This,hEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IStoreNamespace_Initialize_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [optional][in] */ HWND hwndOwner,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IStoreNamespace_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreNamespace_GetDirectory_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [size_is][out][in] */ LPSTR pszPath,
    /* [in] */ DWORD cchMaxPath);


void __RPC_STUB IStoreNamespace_GetDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreNamespace_OpenSpecialFolder_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [in] */ SPECIALFOLDER sfType,
    /* [in] */ DWORD dwReserved,
    /* [out] */ IStoreFolder __RPC_FAR *__RPC_FAR *ppFolder);


void __RPC_STUB IStoreNamespace_OpenSpecialFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreNamespace_OpenFolder_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [in] */ STOREFOLDERID dwFolderId,
    /* [in] */ DWORD dwReserved,
    /* [out] */ IStoreFolder __RPC_FAR *__RPC_FAR *ppFolder);


void __RPC_STUB IStoreNamespace_OpenFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreNamespace_CreateFolder_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [in] */ STOREFOLDERID dwParentId,
    /* [in] */ LPCSTR pszName,
    /* [in] */ DWORD dwReserved,
    /* [out] */ LPSTOREFOLDERID pdwFolderId);


void __RPC_STUB IStoreNamespace_CreateFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreNamespace_RenameFolder_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [in] */ STOREFOLDERID dwFolderId,
    /* [in] */ DWORD dwReserved,
    /* [in] */ LPCSTR pszNewName);


void __RPC_STUB IStoreNamespace_RenameFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreNamespace_MoveFolder_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [in] */ STOREFOLDERID dwFolderId,
    /* [in] */ STOREFOLDERID dwParentId,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IStoreNamespace_MoveFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreNamespace_DeleteFolder_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [in] */ STOREFOLDERID dwFolderId,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IStoreNamespace_DeleteFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreNamespace_GetFolderProps_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [in] */ STOREFOLDERID dwFolderId,
    /* [in] */ DWORD dwReserved,
    /* [out][in] */ LPFOLDERPROPS pProps);


void __RPC_STUB IStoreNamespace_GetFolderProps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreNamespace_CopyMoveMessages_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [in] */ IStoreFolder __RPC_FAR *pSource,
    /* [in] */ IStoreFolder __RPC_FAR *pDest,
    /* [in] */ LPMESSAGEIDLIST pMsgIdList,
    /* [in] */ DWORD dwFlags,
    /* [in] */ DWORD dwFlagsRemove,
    /* [in] */ IProgressNotify __RPC_FAR *pProgress);


void __RPC_STUB IStoreNamespace_CopyMoveMessages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreNamespace_RegisterNotification_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [in] */ DWORD dwReserved,
    /* [in] */ HWND hwnd);


void __RPC_STUB IStoreNamespace_RegisterNotification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreNamespace_UnregisterNotification_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [in] */ DWORD dwReserved,
    /* [in] */ HWND hwnd);


void __RPC_STUB IStoreNamespace_UnregisterNotification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreNamespace_CompactAll_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IStoreNamespace_CompactAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreNamespace_GetFirstSubFolder_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [in] */ STOREFOLDERID dwFolderId,
    /* [out][in] */ LPFOLDERPROPS pProps,
    /* [out] */ LPHENUMSTORE phEnum);


void __RPC_STUB IStoreNamespace_GetFirstSubFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreNamespace_GetNextSubFolder_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [in] */ HENUMSTORE hEnum,
    /* [out][in] */ LPFOLDERPROPS pProps);


void __RPC_STUB IStoreNamespace_GetNextSubFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreNamespace_GetSubFolderClose_Proxy(
    IStoreNamespace __RPC_FAR * This,
    /* [in] */ HENUMSTORE hEnum);


void __RPC_STUB IStoreNamespace_GetSubFolderClose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IStoreNamespace_INTERFACE_DEFINED__ */


#endif
#ifndef __LPSTOREFOLDER_DEFINED
#define __LPSTOREFOLDER_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_msoeapi_0161_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_msoeapi_0161_v0_0_s_ifspec;

#ifndef __IStoreFolder_INTERFACE_DEFINED__
#define __IStoreFolder_INTERFACE_DEFINED__


typedef IStoreFolder __RPC_FAR *LPSTOREFOLDER;

#define MSG_DELETED                  0x0001  // The message has been deleted, compaction will remove it
#define MSG_UNREAD                   0x0002  // The message is marked as un-read
#define MSG_SUBMITTED                0x0004  // For messages waiting to be sent: OUTBOX ONLY
typedef enum tagIMSGPRIORITY {
            IMSG_PRI_LOW       = 5,
            IMSG_PRI_NORMAL    = 3,
            IMSG_PRI_HIGH      = 1
        } IMSGPRIORITY;

#define MSG_UNSENT                   0x0008  // For msgs-in-progress saved to a folder
#define MSG_RECEIVED                 0x0010  // For messages that came from a server
#define MSG_NEWSMSG                  0x0020  // For news messages
#define MSG_NOSECUI                  0x0040  // For messages where the user wants to die
#define MSG_VOICEMAIL                0x0080  // The message has the X-Voicemail header set...
#define MSG_LAST                     0x0080  // ** Keep this puppy updated!! **
#define MSG_EXTERNAL_FLAGS           0x00FE
#define MSG_FLAGS                    0x000f
typedef struct  tagMESSAGEPROPS
    {
    DWORD cbSize;
    DWORD dwReserved;
    MESSAGEID dwMessageId;
    DWORD dwLanguage;
    DWORD dwState;
    DWORD cbMessage;
    IMSGPRIORITY priority;
    FILETIME ftReceived;
    FILETIME ftSent;
    LPSTR pszSubject;
    LPSTR pszDisplayTo;
    LPSTR pszDisplayFrom;
    LPSTR pszNormalSubject;
    DWORD dwFlags;
    IStream __RPC_FAR *pStmOffsetTable;
    }	MESSAGEPROPS;

typedef struct tagMESSAGEPROPS __RPC_FAR *LPMESSAGEPROPS;

struct  HBATCHLOCK__
    {
    DWORD unused;
    };
typedef struct HBATCHLOCK__ *HBATCHLOCK;

typedef HBATCHLOCK __RPC_FAR *LPHBATCHLOCK;

#define MSGPROPS_FAST                0x00000001              // See MESSAGEPROPS structure, improved performance
#define COMMITSTREAM_REVERT          0x00000001             // Don't add this stream/message to the folder

EXTERN_C const IID IID_IStoreFolder;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("E70C92AC-4BFD-11d1-8A95-00C04FB951F3")
    IStoreFolder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetFolderProps(
            /* [in] */ DWORD dwReserved,
            /* [out][in] */ LPFOLDERPROPS pProps) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetMessageProps(
            /* [in] */ MESSAGEID dwMessageId,
            /* [in] */ DWORD dwFlags,
            /* [out][in] */ LPMESSAGEPROPS pProps) = 0;

        virtual HRESULT STDMETHODCALLTYPE FreeMessageProps(
            /* [out][in] */ LPMESSAGEPROPS pProps) = 0;

        virtual HRESULT STDMETHODCALLTYPE DeleteMessages(
            /* [in] */ LPMESSAGEIDLIST pMsgIdList,
            /* [in] */ DWORD dwReserved,
            /* [optional][in] */ IProgressNotify __RPC_FAR *pProgress) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetLanguage(
            /* [in] */ DWORD dwLanguage,
            /* [in] */ DWORD dwReserved,
            /* [in] */ LPMESSAGEIDLIST pMsgIdList) = 0;

        virtual HRESULT STDMETHODCALLTYPE MarkMessagesAsRead(
            /* [in] */ BOOL fRead,
            /* [in] */ DWORD dwReserved,
            /* [in] */ LPMESSAGEIDLIST pMsgIdList) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetFlags(
            /* [in] */ LPMESSAGEIDLIST pMsgIdList,
            /* [in] */ DWORD dwState,
            /* [in] */ DWORD dwStatemask,
            /* [out] */ LPDWORD prgdwNewFlags) = 0;

        virtual HRESULT STDMETHODCALLTYPE OpenMessage(
            /* [in] */ MESSAGEID dwMessageId,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPVOID __RPC_FAR *ppvObject) = 0;

        virtual HRESULT STDMETHODCALLTYPE SaveMessage(
            /* [in] */ REFIID riid,
            /* [iid_is][in] */ LPVOID pvObject,
            /* [in] */ DWORD dwMsgFlags,
            /* [out] */ LPMESSAGEID pdwMessageId) = 0;

        virtual HRESULT STDMETHODCALLTYPE BatchLock(
            /* [in] */ DWORD dwReserved,
            /* [out] */ LPHBATCHLOCK phBatchLock) = 0;

        virtual HRESULT STDMETHODCALLTYPE BatchFlush(
            /* [in] */ DWORD dwReserved,
            /* [in] */ HBATCHLOCK hBatchLock) = 0;

        virtual HRESULT STDMETHODCALLTYPE BatchUnlock(
            /* [in] */ DWORD dwReserved,
            /* [in] */ HBATCHLOCK hBatchLock) = 0;

        virtual HRESULT STDMETHODCALLTYPE CreateStream(
            /* [optional][in] */ HBATCHLOCK hBatchLock,
            /* [in] */ DWORD dwReserved,
            /* [out] */ IStream __RPC_FAR *__RPC_FAR *ppStream,
            /* [out] */ LPMESSAGEID pdwMessageId) = 0;

        virtual HRESULT STDMETHODCALLTYPE CommitStream(
            /* [optional][in] */ HBATCHLOCK hBatchLock,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwMsgFlags,
            /* [in] */ IStream __RPC_FAR *pStream,
            /* [in] */ MESSAGEID dwMessageId,
            /* [optional][in] */ IStream __RPC_FAR *pMessage) = 0;

        virtual HRESULT STDMETHODCALLTYPE RegisterNotification(
            /* [in] */ DWORD dwReserved,
            /* [in] */ HWND hwnd) = 0;

        virtual HRESULT STDMETHODCALLTYPE UnregisterNotification(
            /* [in] */ DWORD dwReserved,
            /* [in] */ HWND hwnd) = 0;

        virtual HRESULT STDMETHODCALLTYPE Compact(
            /* [in] */ DWORD dwReserved) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetFirstMessage(
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwMsgFlags,
            /* [in] */ MESSAGEID dwMsgIdFirst,
            /* [out][in] */ LPMESSAGEPROPS pProps,
            /* [out] */ LPHENUMSTORE phEnum) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetNextMessage(
            /* [in] */ HENUMSTORE hEnum,
            /* [in] */ DWORD dwFlags,
            /* [out][in] */ LPMESSAGEPROPS pProps) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetMessageClose(
            /* [in] */ HENUMSTORE hEnum) = 0;

    };

#else 	/* C style interface */

    typedef struct IStoreFolderVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            IStoreFolder __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            IStoreFolder __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFolderProps )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ DWORD dwReserved,
            /* [out][in] */ LPFOLDERPROPS pProps);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMessageProps )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ MESSAGEID dwMessageId,
            /* [in] */ DWORD dwFlags,
            /* [out][in] */ LPMESSAGEPROPS pProps);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FreeMessageProps )(
            IStoreFolder __RPC_FAR * This,
            /* [out][in] */ LPMESSAGEPROPS pProps);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteMessages )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ LPMESSAGEIDLIST pMsgIdList,
            /* [in] */ DWORD dwReserved,
            /* [optional][in] */ IProgressNotify __RPC_FAR *pProgress);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetLanguage )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ DWORD dwLanguage,
            /* [in] */ DWORD dwReserved,
            /* [in] */ LPMESSAGEIDLIST pMsgIdList);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MarkMessagesAsRead )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ BOOL fRead,
            /* [in] */ DWORD dwReserved,
            /* [in] */ LPMESSAGEIDLIST pMsgIdList);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetFlags )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ LPMESSAGEIDLIST pMsgIdList,
            /* [in] */ DWORD dwState,
            /* [in] */ DWORD dwStatemask,
            /* [out] */ LPDWORD prgdwNewFlags);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OpenMessage )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ MESSAGEID dwMessageId,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPVOID __RPC_FAR *ppvObject);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveMessage )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][in] */ LPVOID pvObject,
            /* [in] */ DWORD dwMsgFlags,
            /* [out] */ LPMESSAGEID pdwMessageId);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BatchLock )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ DWORD dwReserved,
            /* [out] */ LPHBATCHLOCK phBatchLock);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BatchFlush )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ DWORD dwReserved,
            /* [in] */ HBATCHLOCK hBatchLock);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BatchUnlock )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ DWORD dwReserved,
            /* [in] */ HBATCHLOCK hBatchLock);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateStream )(
            IStoreFolder __RPC_FAR * This,
            /* [optional][in] */ HBATCHLOCK hBatchLock,
            /* [in] */ DWORD dwReserved,
            /* [out] */ IStream __RPC_FAR *__RPC_FAR *ppStream,
            /* [out] */ LPMESSAGEID pdwMessageId);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CommitStream )(
            IStoreFolder __RPC_FAR * This,
            /* [optional][in] */ HBATCHLOCK hBatchLock,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwMsgFlags,
            /* [in] */ IStream __RPC_FAR *pStream,
            /* [in] */ MESSAGEID dwMessageId,
            /* [optional][in] */ IMimeMessage __RPC_FAR *pMessage);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RegisterNotification )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ DWORD dwReserved,
            /* [in] */ HWND hwnd);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnregisterNotification )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ DWORD dwReserved,
            /* [in] */ HWND hwnd);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Compact )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ DWORD dwReserved);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFirstMessage )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwMsgFlags,
            /* [in] */ MESSAGEID dwMsgIdFirst,
            /* [out][in] */ LPMESSAGEPROPS pProps,
            /* [out] */ LPHENUMSTORE phEnum);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetNextMessage )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ HENUMSTORE hEnum,
            /* [in] */ DWORD dwFlags,
            /* [out][in] */ LPMESSAGEPROPS pProps);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMessageClose )(
            IStoreFolder __RPC_FAR * This,
            /* [in] */ HENUMSTORE hEnum);

        END_INTERFACE
    } IStoreFolderVtbl;

    interface IStoreFolder
    {
        CONST_VTBL struct IStoreFolderVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IStoreFolder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IStoreFolder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IStoreFolder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IStoreFolder_GetFolderProps(This,dwReserved,pProps)	\
    (This)->lpVtbl -> GetFolderProps(This,dwReserved,pProps)

#define IStoreFolder_GetMessageProps(This,dwMessageId,dwFlags,pProps)	\
    (This)->lpVtbl -> GetMessageProps(This,dwMessageId,dwFlags,pProps)

#define IStoreFolder_FreeMessageProps(This,pProps)	\
    (This)->lpVtbl -> FreeMessageProps(This,pProps)

#define IStoreFolder_DeleteMessages(This,pMsgIdList,dwReserved,pProgress)	\
    (This)->lpVtbl -> DeleteMessages(This,pMsgIdList,dwReserved,pProgress)

#define IStoreFolder_SetLanguage(This,dwLanguage,dwReserved,pMsgIdList)	\
    (This)->lpVtbl -> SetLanguage(This,dwLanguage,dwReserved,pMsgIdList)

#define IStoreFolder_MarkMessagesAsRead(This,fRead,dwReserved,pMsgIdList)	\
    (This)->lpVtbl -> MarkMessagesAsRead(This,fRead,dwReserved,pMsgIdList)

#define IStoreFolder_SetFlags(This,pMsgIdList,dwState,dwStatemask,prgdwNewFlags)	\
    (This)->lpVtbl -> SetFlags(This,pMsgIdList,dwState,dwStatemask,prgdwNewFlags)

#define IStoreFolder_OpenMessage(This,dwMessageId,riid,ppvObject)	\
    (This)->lpVtbl -> OpenMessage(This,dwMessageId,riid,ppvObject)

#define IStoreFolder_SaveMessage(This,riid,pvObject,dwMsgFlags,pdwMessageId)	\
    (This)->lpVtbl -> SaveMessage(This,riid,pvObject,dwMsgFlags,pdwMessageId)

#define IStoreFolder_BatchLock(This,dwReserved,phBatchLock)	\
    (This)->lpVtbl -> BatchLock(This,dwReserved,phBatchLock)

#define IStoreFolder_BatchFlush(This,dwReserved,hBatchLock)	\
    (This)->lpVtbl -> BatchFlush(This,dwReserved,hBatchLock)

#define IStoreFolder_BatchUnlock(This,dwReserved,hBatchLock)	\
    (This)->lpVtbl -> BatchUnlock(This,dwReserved,hBatchLock)

#define IStoreFolder_CreateStream(This,hBatchLock,dwReserved,ppStream,pdwMessageId)	\
    (This)->lpVtbl -> CreateStream(This,hBatchLock,dwReserved,ppStream,pdwMessageId)

#define IStoreFolder_CommitStream(This,hBatchLock,dwFlags,dwMsgFlags,pStream,dwMessageId,pMessage)	\
    (This)->lpVtbl -> CommitStream(This,hBatchLock,dwFlags,dwMsgFlags,pStream,dwMessageId,pMessage)

#define IStoreFolder_RegisterNotification(This,dwReserved,hwnd)	\
    (This)->lpVtbl -> RegisterNotification(This,dwReserved,hwnd)

#define IStoreFolder_UnregisterNotification(This,dwReserved,hwnd)	\
    (This)->lpVtbl -> UnregisterNotification(This,dwReserved,hwnd)

#define IStoreFolder_Compact(This,dwReserved)	\
    (This)->lpVtbl -> Compact(This,dwReserved)

#define IStoreFolder_GetFirstMessage(This,dwFlags,dwMsgFlags,dwMsgIdFirst,pProps,phEnum)	\
    (This)->lpVtbl -> GetFirstMessage(This,dwFlags,dwMsgFlags,dwMsgIdFirst,pProps,phEnum)

#define IStoreFolder_GetNextMessage(This,hEnum,dwFlags,pProps)	\
    (This)->lpVtbl -> GetNextMessage(This,hEnum,dwFlags,pProps)

#define IStoreFolder_GetMessageClose(This,hEnum)	\
    (This)->lpVtbl -> GetMessageClose(This,hEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IStoreFolder_GetFolderProps_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ DWORD dwReserved,
    /* [out][in] */ LPFOLDERPROPS pProps);


void __RPC_STUB IStoreFolder_GetFolderProps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_GetMessageProps_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ MESSAGEID dwMessageId,
    /* [in] */ DWORD dwFlags,
    /* [out][in] */ LPMESSAGEPROPS pProps);


void __RPC_STUB IStoreFolder_GetMessageProps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_FreeMessageProps_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [out][in] */ LPMESSAGEPROPS pProps);


void __RPC_STUB IStoreFolder_FreeMessageProps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_DeleteMessages_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ LPMESSAGEIDLIST pMsgIdList,
    /* [in] */ DWORD dwReserved,
    /* [optional][in] */ IProgressNotify __RPC_FAR *pProgress);


void __RPC_STUB IStoreFolder_DeleteMessages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_SetLanguage_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ DWORD dwLanguage,
    /* [in] */ DWORD dwReserved,
    /* [in] */ LPMESSAGEIDLIST pMsgIdList);


void __RPC_STUB IStoreFolder_SetLanguage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_MarkMessagesAsRead_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ BOOL fRead,
    /* [in] */ DWORD dwReserved,
    /* [in] */ LPMESSAGEIDLIST pMsgIdList);


void __RPC_STUB IStoreFolder_MarkMessagesAsRead_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_SetFlags_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ LPMESSAGEIDLIST pMsgIdList,
    /* [in] */ DWORD dwState,
    /* [in] */ DWORD dwStatemask,
    /* [out] */ LPDWORD prgdwNewFlags);


void __RPC_STUB IStoreFolder_SetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_OpenMessage_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ MESSAGEID dwMessageId,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ LPVOID __RPC_FAR *ppvObject);


void __RPC_STUB IStoreFolder_OpenMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_SaveMessage_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ REFIID riid,
    /* [iid_is][in] */ LPVOID pvObject,
    /* [in] */ DWORD dwMsgFlags,
    /* [out] */ LPMESSAGEID pdwMessageId);


void __RPC_STUB IStoreFolder_SaveMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_BatchLock_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ DWORD dwReserved,
    /* [out] */ LPHBATCHLOCK phBatchLock);


void __RPC_STUB IStoreFolder_BatchLock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_BatchFlush_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ DWORD dwReserved,
    /* [in] */ HBATCHLOCK hBatchLock);


void __RPC_STUB IStoreFolder_BatchFlush_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_BatchUnlock_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ DWORD dwReserved,
    /* [in] */ HBATCHLOCK hBatchLock);


void __RPC_STUB IStoreFolder_BatchUnlock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_CreateStream_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [optional][in] */ HBATCHLOCK hBatchLock,
    /* [in] */ DWORD dwReserved,
    /* [out] */ IStream __RPC_FAR *__RPC_FAR *ppStream,
    /* [out] */ LPMESSAGEID pdwMessageId);


void __RPC_STUB IStoreFolder_CreateStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_CommitStream_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [optional][in] */ HBATCHLOCK hBatchLock,
    /* [in] */ DWORD dwFlags,
    /* [in] */ DWORD dwMsgFlags,
    /* [in] */ IStream __RPC_FAR *pStream,
    /* [in] */ MESSAGEID dwMessageId,
    /* [optional][in] */ IStream __RPC_FAR *pMessage);


void __RPC_STUB IStoreFolder_CommitStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_RegisterNotification_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ DWORD dwReserved,
    /* [in] */ HWND hwnd);


void __RPC_STUB IStoreFolder_RegisterNotification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_UnregisterNotification_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ DWORD dwReserved,
    /* [in] */ HWND hwnd);


void __RPC_STUB IStoreFolder_UnregisterNotification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_Compact_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IStoreFolder_Compact_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_GetFirstMessage_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ DWORD dwFlags,
    /* [in] */ DWORD dwMsgFlags,
    /* [in] */ MESSAGEID dwMsgIdFirst,
    /* [out][in] */ LPMESSAGEPROPS pProps,
    /* [out] */ LPHENUMSTORE phEnum);


void __RPC_STUB IStoreFolder_GetFirstMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_GetNextMessage_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ HENUMSTORE hEnum,
    /* [in] */ DWORD dwFlags,
    /* [out][in] */ LPMESSAGEPROPS pProps);


void __RPC_STUB IStoreFolder_GetNextMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStoreFolder_GetMessageClose_Proxy(
    IStoreFolder __RPC_FAR * This,
    /* [in] */ HENUMSTORE hEnum);


void __RPC_STUB IStoreFolder_GetMessageClose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IStoreFolder_INTERFACE_DEFINED__ */


#ifdef __cplusplus
}
#endif


#endif
#endif
