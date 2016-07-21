#if (defined (KL_MULTIPLATFORM_LINKS))
    #pragma once
    #include <stuff/cpointer.h>
    #include <Bases/Format/Records.h>

    #pragma warning (disable : 4068 4081)
    #include <serparam/include/serparam/av_extra.h>
    #pragma warning (default : 4068 4081)
#endif

class CRecordEdit :public CObject
{
public:
	SetRecordLinkIdx(int Idx);
	CString	Name;

	CString	Comment;

#if (defined (KL_MULTIPLATFORM_LINKS))
private:
    CAPointer<char> SerializedExtraInfo;
public:
    TExtraRecordInfo ExtraInfo;
#endif

	BYTE	SubType;

	BYTE*	Link16Buffer;
	BYTE*	Link32Buffer;

	BOOL	ModifyFlag;

	BYTE*	Record;
	BOOL Unlink();

#pragma pack(push,pack_recordedit)
#pragma pack(1)

	WORD	Type;		
	DWORD	NameIdx;	//        		Idx
	DWORD	CommentIdx;	//        		Idx
	DWORD	RecordIdx;		//	   		Idx
	DWORD	Link16Idx;		//	   		Idx
	DWORD	Link32Idx;		//	   		Idx

#pragma pack(pop,pack_recordedit)


	CRecordEdit(WORD type);

#if (!defined (KL_MULTIPLATFORM_LINKS))
	CRecordEdit(void* ptr);
#else
	CRecordEdit(R_Edit * ptr, const char * extraInfoString = NULL);
#endif

	CRecordEdit(CRecordEdit* re);
	~CRecordEdit();

	BOOL	IsModified(){ return ModifyFlag; };
	void	SetModifyFlag(BOOL state=TRUE){ ModifyFlag = state; };

	void		SetName(const char* sz){ Name = sz; };
	void		SetComment(const char* sz){ Comment = sz; };
	const char*	GetName();
	const char*	GetComment();

	const char*	GetTypeString();
	const char*	GetSubTypeString();
	const char*	GetMethodString();
	const char*	GetLink16String();
	const char*	GetLink32String();

	SetRecord(void* ptr);
	SetLink16(void* ptr);
	SetLink32(void* ptr);

	MakeNameIdx(CPtrArray& array, BOOL pack, BOOL init);
	MakeCommentIdx(CPtrArray& array);
	MakeRecordIdx(CPtrArray& array);
	MakeLinkIdx(CPtrArray& link32array, CPtrArray& link16array);

	int	EditRecord();
	BYTE AddLink(const char* filrname, BOOL nodlg_=0);
};

