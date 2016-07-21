GUID CLSID_IMimeMessage = {0x0FD853CE3, 0x7F86, 0x11d0, 0x82, 0x52, 0x00, 0x0C0, 0x4F, 0x0D8, 0x5A, 0x0B4};
GUID IID_IMimeMessage	= {0x0FD853CD5, 0x7F86, 0x11D0, 0x82, 0x52, 0x00, 0x0C0, 0x4F, 0x0D8, 0x5A, 0x0B4};
GUID IID_IMimeMessageW	= {0x0F90ADFEF, 0xD01F, 0x11D2, 0x0A0, 0x04, 0x00, 0x0A0, 0x0C9,  0x0C, 0x9B, 0x0B6};

struct HBODY__ {
	WORD	wID;
	WORD	wUnk;
};

struct tagBODYOFFSETS {
	DWORD	dwMsgOffset;
	DWORD	dwBodyOffset;
	DWORD	dwContentOffset;
	DWORD	dwEndOffset;
};

enum tagENCODINGTYPE {
	ENCODING_ANSI		= 6,
	ENCODING_UNICODE	= 7
};

enum tagIMSGBODYTYPE {
	MSGBODY_TEXT		= 3,
	MSGBODY_ATTACH		= 1,
};

interface IMimeMessageW {
	public: virtual long __stdcall QueryInterface(struct _GUID const &,void * *);
	public: virtual unsigned long __stdcall AddRef(void);
	public: virtual unsigned long __stdcall Release(void);
	public: virtual long __stdcall GetClassID(struct _GUID *);
	public: virtual long __stdcall IsDirty(void);
	public: virtual long __stdcall Load(struct IStream *);
	public: virtual long __stdcall Save(struct IStream *,int);
	public: virtual long __stdcall GetSizeMax(union _ULARGE_INTEGER *);
	public: virtual long __stdcall InitNew(void);
	public: virtual long __stdcall GetMessageSource(struct IStream * *,unsigned long);
	public: virtual long __stdcall GetMessageSize(unsigned long *,unsigned long);
	public: virtual long __stdcall LoadOffsetTable(struct IStream *);
	public: virtual long __stdcall SaveOffsetTable(struct IStream *,unsigned long);
	public: virtual long __stdcall GetFlags(unsigned long *);
	public: virtual long __stdcall Commit(unsigned long);
	public: virtual long __stdcall HandsOffStorage(void);
	public: virtual long __stdcall BindToObject(struct HBODY__ * const,struct _GUID const &,void * *);
	public: virtual long __stdcall SaveBody(struct HBODY__ *,unsigned long,struct IStream *);
	public: virtual long __stdcall InsertBody(enum  tagBODYLOCATION,struct HBODY__ *,struct HBODY__ * *);
	public: virtual long __stdcall GetBody(enum  tagBODYLOCATION,struct HBODY__ *,struct HBODY__ * *);
	public: virtual long __stdcall DeleteBody(struct HBODY__ *,unsigned long);
	public: virtual long __stdcall MoveBody(struct HBODY__ *,enum  tagBODYLOCATION);
	public: virtual long __stdcall CountBodies(struct HBODY__ *,unsigned char,unsigned long *);
	public: virtual long __stdcall FindFirst(struct tagFINDBODY *,struct HBODY__ * *);
	public: virtual long __stdcall FindNext(struct tagFINDBODY *,struct HBODY__ * *);
	public: virtual long __stdcall ResolveURL(struct HBODY__ *,char const *,char const *,unsigned long,struct HBODY__ * *);
	public: virtual long __stdcall ToMultipart(struct HBODY__ *,char const *,struct HBODY__ * *);
	public: virtual long __stdcall GetBodyOffsets(struct HBODY__ *,struct tagBODYOFFSETS *);
	public: virtual long __stdcall GetCharset(struct HCHARSET__ * *);
	public: virtual long __stdcall SetCharset(struct HCHARSET__ *,enum  tagCSETAPPLYTYPE);
	public: virtual long __stdcall IsBodyType(struct HBODY__ *,enum  tagIMSGBODYTYPE);
	public: virtual long __stdcall IsContentType(struct HBODY__ *,char const *,char const *);
	public: virtual long __stdcall QueryBodyProp(struct HBODY__ *,char const *,char const *,unsigned char,unsigned char);
	public: virtual long __stdcall GetBodyProp(struct HBODY__ *,char const *,unsigned long,struct tagPROPVARIANT *);
	public: virtual long __stdcall SetBodyProp(struct HBODY__ *,char const *,unsigned long,struct tagPROPVARIANT const *);
	public: virtual long __stdcall DeleteBodyProp(struct HBODY__ *,char const *);
	public: virtual long __stdcall SetOption(unsigned long,struct tagPROPVARIANT const *);
	public: virtual long __stdcall GetOption(unsigned long,struct tagPROPVARIANT *);
	public: virtual long __stdcall CreateWebPage(struct IStream *,struct tagWEBPAGEOPTIONS *,struct IMimeMessageCallback *,struct IMoniker * *);
	public: virtual long __stdcall GetPropA(char const *,unsigned long,struct tagPROPVARIANT *);
	public: virtual long __stdcall SetPropA(char const *,unsigned long,struct tagPROPVARIANT const *);
	public: virtual long __stdcall DeleteProp(char const *);
	public: virtual long __stdcall QueryProp(char const *,char const *,unsigned char,unsigned char);
	public: virtual long __stdcall GetTextBody(unsigned long,enum  tagENCODINGTYPE,struct IStream * *,struct HBODY__ * *);
	public: virtual long __stdcall SetTextBody(unsigned long,enum  tagENCODINGTYPE,struct HBODY__ *,struct IStream *,struct HBODY__ * *);
	public: virtual long __stdcall AttachObject(struct _GUID const &,void *,struct HBODY__ * *);
	public: virtual long __stdcall AttachFile(char const *,struct IStream *,struct HBODY__ * *);
	public: virtual long __stdcall AttachURL(char const *,char const *,unsigned long,struct IStream *,char * *,struct HBODY__ * *);
	public: virtual long __stdcall GetAttachments(unsigned long *,struct HBODY__ * * *);
	public: virtual long __stdcall GetAddressTable(struct IMimeAddressTable * *);
	public: virtual long __stdcall GetSender(struct tagADDRESSPROPS *);
	public: virtual long __stdcall GetAddressTypes(unsigned long,unsigned long,struct tagADDRESSLIST *);
	public: virtual long __stdcall GetAddressFormat(unsigned long,enum  tagADDRESSFORMAT,char * *);
	public: virtual long __stdcall EnumAddressTypes(unsigned long,unsigned long,struct IMimeEnumAddressTypes * *);
	public: virtual long __stdcall SplitMessage(unsigned long,struct IMimeMessageParts * *);
	public: virtual long __stdcall GetRootMoniker(struct IMoniker * *);
	public: virtual long __stdcall GetMoniker(unsigned long,unsigned long,struct IMoniker * *);
	public: virtual long __stdcall GetMoniker2(unsigned long,unsigned long,struct IMoniker * *);
	public: virtual long __stdcall EnumVerbs(struct IEnumOLEVERB * *);
	public: virtual long __stdcall SetRectComplex(struct tagRECT *,struct tagRECT *,struct tagRECT *,struct tagRECT *);
	public: virtual long __stdcall AttachFileW(unsigned short const *,struct IStream *,struct HBODY__ * *);
	public: virtual long __stdcall NavigateHack(unsigned long,struct IBindCtx *,struct IBindStatusCallback *,unsigned short const *,struct IBindStatusCallback *,struct IBindStatusCallback *);
	public: virtual long __stdcall GetAddressFormatW(unsigned long,enum  tagADDRESSFORMAT,unsigned short * *);
	public: virtual long __stdcall SetDataW(enum  tagENCODINGTYPE,unsigned short const *,unsigned short const *,struct _GUID const &,void *);
};

#define TYPEDID_MASK                     ((ULONG)0x0000FFFF)
#define TYPEDID_TYPE(_typedid)           (VARTYPE)(((ULONG)(_typedid)) & TYPEDID_MASK)
#define TYPEDID_ID(_typedid)             (((ULONG)(_typedid))>>16)
#define TYPEDID(_vartype,_id)            ((((_id))<<16)|((ULONG)(_vartype)))

// --------------------------------------------------------------------------------
// Options Ids
// --------------------------------------------------------------------------------
#define OID_ALLOW_8BIT_HEADER            TYPEDID(VT_BOOL,    0x0001) // TRUE or FALSE
#define OID_CBMAX_HEADER_LINE            TYPEDID(VT_UI4,     0x0002) // Bytes
#define OID_SAVE_FORMAT                  TYPEDID(VT_UI4,     0x0003) // SAVE_RFC822 or SAVE_RFC1521 (mime)
#define OID_WRAP_BODY_TEXT               TYPEDID(VT_BOOL,    0x0004) // TRUE or FALSE
#define OID_CBMAX_BODY_LINE              TYPEDID(VT_UI4,     0x0005) // Bytes
#define OID_TRANSMIT_BODY_ENCODING       TYPEDID(VT_UI4,     0x0006) // ENCODINGTYPE
#define OID_TRANSMIT_TEXT_ENCODING       TYPEDID(VT_UI4,     0x0007) // ENCODINGTYPE, IMimeMessageTree
#define OID_GENERATE_MESSAGE_ID          TYPEDID(VT_BOOL,    0x0008) // TRUE or FALSE
#define OID_HIDE_TNEF_ATTACHMENTS        TYPEDID(VT_BOOL,    0X000E) // TRUE or FALSE
#define OID_CLEANUP_TREE_ON_SAVE         TYPEDID(VT_BOOL,    0X000F) // TRUE or FALSE
#define OID_BODY_REMOVE_NBSP             TYPEDID(VT_BOOL,    0x0014) // TRUE or FALSE
#define OID_DEFAULT_BODY_CHARSET         TYPEDID(VT_UI4,     0x0015) // HCHARSET
#define OID_DEFAULT_HEADER_CHARSET       TYPEDID(VT_UI4,     0x0016) // HCHARSET
#define OID_DBCS_ESCAPE_IS_8BIT          TYPEDID(VT_BOOL,    0x0017) // TRUE or FALSE
#define OID_SECURITY_TYPE                TYPEDID(VT_UI4,     0x0018)
#define OID_SECURITY_ALG_HASH            TYPEDID(VT_BLOB,    0x0019)
#define OID_SECURITY_ALG_BULK            TYPEDID(VT_BLOB,    0x0020)
#define OID_SECURITY_CERT_SIGNING        TYPEDID(VT_UI4,     0x0021)
#define OID_SECURITY_CERT_DECRYPTION     TYPEDID(VT_UI4,     0x0022)
#define OID_SECURITY_RG_CERT_ENCRYPT     TYPEDID(VT_VECTOR | VT_UI4, 0x0023)
#define OID_SECURITY_HCERTSTORE          TYPEDID(VT_UI4,     0x0024)
#define OID_SECURITY_RG_CERT_BAG         TYPEDID(VT_VECTOR | VT_UI4, 0x0025)
#define OID_SECURITY_CRL                 TYPEDID(VT_BLOB,    0x0026)
#define OID_SECURITY_SEARCHSTORES        TYPEDID(VT_VECTOR | VT_UI4, 0x0027)
#define OID_SECURITY_RG_IASN             TYPEDID(VT_VECTOR | VT_UI4, 0x0028)
#define OID_SECURITY_SYMCAPS             TYPEDID(VT_BLOB,    0x0029)
#define OID_SECURITY_AUTHATTR            TYPEDID(VT_BLOB,    0x002A)
#define OID_SECURITY_UNAUTHATTR          TYPEDID(VT_BLOB,    0x002B)
#define OID_SECURITY_SIGNTIME            TYPEDID(VT_FILETIME,0x002C)
#define OID_SECURITY_USER_VALIDITY       TYPEDID(VT_UI4,     0x002D)
#define OID_SECURITY_RO_MSG_VALIDITY     TYPEDID(VT_UI4,     0x002E) // read-only
#define OID_SECURITY_HCRYPTPROV          TYPEDID(VT_UI4,     0x002F)
#define OID_SECURITY_ENCODE_FLAGS        TYPEDID(VT_UI4,     0x0030)
#define OID_SECURITY_CERT_INCLUDED       TYPEDID(VT_BOOL,    0x0031) // read-only
#define OID_SECURITY_HWND_OWNER          TYPEDID(VT_UI4,     0x0032)
#define OID_SECURITY_REQUESTED_CTE       TYPEDID(VT_I4,      0x0033)
#define OID_NO_DEFAULT_CNTTYPE           TYPEDID(VT_BOOL,    0x0034)
#define OID_XMIT_PLAIN_TEXT_ENCODING     TYPEDID(VT_UI4,     0x0035) // ENCODINGTYPE, IMimeMessageTree
#define OID_XMIT_HTML_TEXT_ENCODING      TYPEDID(VT_UI4,     0x0036) // ENCODINGTYPE, IMimeMessageTree
#define OID_HEADER_RELOAD_TYPE           TYPEDID(VT_UI4,     0x0037) // RELOADTYPE IMimeMessageTree or IMimeBody or IMimePropertySet
#define OID_CAN_INLINE_TEXT_BODIES       TYPEDID(VT_BOOL,    0x0038) // TRUE or FALSE
#define OID_SHOW_MACBINARY               TYPEDID(VT_BOOL,    0x0039) // TRUE or FALSE
