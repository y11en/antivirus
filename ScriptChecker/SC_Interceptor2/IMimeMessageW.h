GUID CLSID_IMimeMessage = {0x0FD853CE3, 0x7F86, 0x11d0, 0x82, 0x52, 0x00, 0x0C0, 0x4F, 0x0D8, 0x5A, 0x0B4};
GUID IID_IMimeMessage	= {0x0FD853CD5, 0x7F86, 0x11D0, 0x82, 0x52, 0x00, 0x0C0, 0x4F, 0x0D8, 0x5A, 0x0B4};
GUID IID_IMimeMessageW	= {0x0F90ADFEF, 0xD01F, 0x11D2, 0x0A0, 0x04, 0x00, 0x0A0, 0x0C9,  0x0C, 0x9B, 0x0B6};

struct HBODY__ {
	WORD	wID;
	WORD	wUnk;
};

enum tagENCODINGTYPE {
	ENCODING_ANSI		= 6,
	ENCODING_UNICODE	= 7
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
