#if !defined(_MAPI_WRAPPER_H_)
#define _MAPI_WRAPPER_H_

#include <mapi.h>
#include "../KLUtil/inl/PoliStorage.h"
#include "../KLUtil/UnicodeConv.h"
#include <objbase.h>
#include <tchar.h>
#include "..\StuffIO\Ioutil.h"

#define MAPI_SERIALIZE_FILE_MAGIC_NUMBER	0xE92C2903
#define MAPI_SEND							0x0F0F0F0F
#define MAPI_DONTPROCESS					0x00000000

inline bool GetAutoDelTempFileName(std::string& szFullPath)
{
	TCHAR tmp_path[MAX_PATH];
	DWORD dwRet = ::GetTempPath(MAX_PATH, tmp_path);
	if ( (dwRet > MAX_PATH) || (dwRet == 0) )
		return false;
	
	TCHAR f_name[MAX_PATH];
	
	GUID guid;
	
	if (CoCreateGuid (&guid) != S_OK)
		return false;
	
	wchar_t wszGuid[64];
	if (StringFromGUID2 (guid, wszGuid, 64) == 0)
		return false;
	
	_tmakepath ( f_name, NULL, tmp_path, __LPCTSTR(wszGuid), NULL );
	
	IOSMoveFileOnReboot( f_name, NULL );
	
	szFullPath = f_name;
	
	return true;
}

struct MapiFileDescSer : protected MapiFileDesc
{
    std::string lpszPathNameSer;          /* Full path name of attachment file       */
    std::string lpszFileNameSer;          /* Original file name (optional)           */
	bool m_bCopied;

	MapiFileDescSer() : m_bCopied(false)
	{
		ZeroMemory( (MapiFileDesc*)this, sizeof (MapiFileDesc) );
	}
	
	
	MapiFileDescSer(const MapiFileDesc& mfd) : m_bCopied(false)
	{
		Copy (mfd);
	}
	
	operator MapiFileDesc* ()
	{
		lpszPathName = lpszPathName?const_cast<LPSTR>(lpszPathNameSer.c_str()):NULL;
		lpszFileName = lpszFileName?const_cast<LPSTR>(lpszFileNameSer.c_str  ()):NULL;
		return (MapiFileDesc*)this;
	}

	PoliType& MarshalImpl (PoliType& pt)
	{
		MARSHAL_AS_LONG(pt, LPSTR, lpszPathName)
		MARSHAL_AS_LONG(pt, LPSTR, lpszFileName)
			
		return pt<<ulReserved<<flFlags<<nPosition<<lpszPathNameSer<<lpszFileNameSer<<m_bCopied;
	}
	

protected:
	
	MapiFileDescSer& operator= (const MapiFileDesc& mfd)
	{
		return *this;
	}
	
	void Copy(const MapiFileDesc& mfd)
	{
		ulReserved = mfd.ulReserved;
		flFlags = mfd.flFlags;
		nPosition = mfd.nPosition;
		
		lpszPathName = mfd.lpszPathName;
		if (mfd.lpszPathName)
			lpszPathNameSer = mfd.lpszPathName;

		lpszFileName = mfd.lpszFileName;
		if (mfd.lpszFileName)
			lpszFileNameSer = mfd.lpszFileName;

		lpFileType = NULL;
	}

};

inline PoliType& operator<<(PoliType& pt, MapiFileDescSer& mfd) 
{	
	return mfd.MarshalImpl  (pt);
}

struct MapiRecipDescSer : protected MapiRecipDesc
{
	MapiRecipDescSer()
	{
		ZeroMemory( (MapiRecipDesc*)this, sizeof (MapiRecipDesc) );
	}

	MapiRecipDescSer(const MapiRecipDesc& mrd)
	{
		Copy (mrd);
	}
		
	
	MapiRecipDescSer& operator= (const MapiRecipDesc& mrd)
	{
		Copy(mrd);
		return *this;
	}
	
	operator MapiRecipDesc* ()
	{
		lpszName = lpszName?const_cast<LPSTR>(lpszNameSer.c_str()):NULL;
		lpszAddress = lpszAddress?const_cast<LPSTR>(lpszAddressSer.c_str  ()):NULL;

		if (lpEntryIDSer.empty  ())
		{
			lpEntryID = NULL;
			ulEIDSize = 0;
		}
		else
		{
			lpEntryID = (LPVOID)&lpEntryIDSer.front();
			ulEIDSize = lpEntryIDSer.size();
		}
		
		return (MapiRecipDesc*)this;
	}
	
	PoliType& MarshalImpl (PoliType& pt)
	{
		MARSHAL_AS_LONG(pt, LPSTR, lpszName)
		MARSHAL_AS_LONG(pt, LPSTR, lpszAddress)
			
		return pt<<ulReserved<<ulRecipClass<<lpszNameSer<<lpszAddressSer<<lpEntryIDSer;
	}
	
protected:
	void Copy (const MapiRecipDesc& mrd)
	{
		ulReserved = mrd.ulReserved;
		ulRecipClass = mrd.ulRecipClass;
		
		lpszName = mrd.lpszName;
		if (mrd.lpszName)
			lpszNameSer = mrd.lpszName;
		
		lpszAddress = mrd.lpszAddress;
		if (mrd.lpszAddress)
			lpszAddressSer = mrd.lpszAddress;
		
		ulEIDSize = mrd.ulEIDSize;
		lpEntryID = mrd.lpEntryID;
		
		if ( mrd.lpEntryID != NULL && mrd.ulEIDSize > 0 )
		{
			lpEntryIDSer.assign((const BYTE *)mrd.lpEntryID, (const BYTE *)mrd.lpEntryID + mrd.ulEIDSize);
		}
	}
    std::string lpszNameSer;             /* Recipient name                           */
    std::string lpszAddressSer;          /* Recipient address (optional)             */
	
	std::vector<BYTE> lpEntryIDSer;

};

inline PoliType& operator<<(PoliType& pt, MapiRecipDescSer& mrd) 
{	
	return mrd.MarshalImpl(pt);
}

struct MapiMessageSer : protected MapiMessage
{
	
	MapiMessageSer ()
	{
		ZeroMemory( (MapiMessage*)this, sizeof (MapiMessage) );
	}
	
	
	MapiMessageSer (const MapiMessage& mm)
	{
		Copy(mm);
	}
	
	MapiMessageSer& operator= (const MapiMessage& mm)
	{
		Copy  (mm);
		return *this;
	}
	
	MapiMessage* ConvertToMapiMessage ()
	{
		lpszSubject = lpszSubject?const_cast<LPSTR>(lpszSubjectSer.c_str()):NULL;
		lpszNoteText = lpszNoteText?const_cast<LPSTR>(lpszNoteTextSer.c_str  ()):NULL;
		lpszMessageType = lpszMessageType?const_cast<LPSTR>(lpszMessageTypeSer.c_str  ()):NULL;
		lpszDateReceived = lpszDateReceived?const_cast<LPSTR>(lpszDateReceivedSer.c_str  ()):NULL;
		lpszConversationID = lpszConversationID?const_cast<LPSTR>(lpszConversationIDSer.c_str  ()):NULL;
		lpOriginator = lpOriginator?(MapiRecipDesc*)OriginatorSer:NULL;
		
		m_Recips.clear  ();
		m_Files.clear  ();
		
		if (lpRecips)
		{
			for (std::vector<MapiRecipDescSer>::iterator i = RecipsSer.begin  (); i!=RecipsSer.end (); ++i)
				m_Recips.push_back(*(MapiRecipDesc*)*i);
			
			_ASSERT(nRecipCount == m_Recips.size());
			
			lpRecips = &*m_Recips.begin  ();
		}
		
		
		if (lpFiles)
		{
			for (std::vector<MapiFileDescSer>::iterator i = FilesSer.begin  (); i!=FilesSer.end (); ++i)
				m_Files.push_back(*(MapiFileDesc*)*i);
			
			_ASSERT(nFileCount == m_Files.size  ());
			
			lpFiles = &*m_Files.begin  ();
		}
		
		return (MapiMessage*)this;
	}
	
	bool CopyAttaches( std::string & szErr )
	{
		for ( std::vector<MapiFileDescSer>::iterator i = FilesSer.begin  (); i != FilesSer.end  (); ++i )
		{
			std::string szOldPath = (*i).lpszPathNameSer;
			
			if ( GetAutoDelTempFileName  ( (*i).lpszPathNameSer ) )
			{
				if ( ::CopyFileA( szOldPath.c_str(), (*i).lpszPathNameSer.c_str  (), FALSE ) )
				{
					(*i).m_bCopied = true;
				}
				else
				{
					szErr = szOldPath;
					DeleteFile((*i).lpszPathNameSer.c_str  ());
					DeleteIfCopied ();
					return false;
				}
				
			}
		}

		return true;
	}

	void DeleteIfCopied ()
	{
		for ( std::vector<MapiFileDescSer>::iterator i = FilesSer.begin  (); i != FilesSer.end  (); ++i )
		{
			if ( (*i).m_bCopied == true )
				DeleteFile( (*i).lpszPathNameSer.c_str  () );
		}
	}
	
	PoliType& MarshalImpl (PoliType& pt)
	{
		MARSHAL_AS_LONG(pt, LPSTR, lpszSubject)
		MARSHAL_AS_LONG(pt, LPSTR, lpszNoteText)
		MARSHAL_AS_LONG(pt, LPSTR, lpszMessageType)
		MARSHAL_AS_LONG(pt, LPSTR, lpszDateReceived)
		MARSHAL_AS_LONG(pt, LPSTR, lpszConversationID)

		MARSHAL_AS_LONG(pt, lpMapiRecipDesc, lpOriginator)
		MARSHAL_AS_LONG(pt, lpMapiRecipDesc, lpRecips)
		MARSHAL_AS_LONG(pt, lpMapiFileDesc, lpFiles)
			
		return pt<<ulReserved<<lpszSubjectSer<<lpszNoteTextSer<<lpszMessageTypeSer<<
			lpszDateReceivedSer<<lpszConversationIDSer<<flFlags<<OriginatorSer<<nRecipCount<<RecipsSer<<nFileCount<<FilesSer;
	}
	
protected:
	void Copy(const MapiMessage& mm)
	{
		ulReserved = mm.ulReserved;
		
		lpszSubject = mm.lpszSubject;
		if (mm.lpszSubject)
			lpszSubjectSer = mm.lpszSubject;


		lpszNoteText = mm.lpszNoteText;
		if (mm.lpszNoteText)
			lpszNoteTextSer = mm.lpszNoteText;

		lpszMessageType = mm.lpszMessageType;
		if (mm.lpszMessageType)
			lpszMessageTypeSer = mm.lpszMessageType;

		lpszDateReceived = mm.lpszDateReceived;
		if (mm.lpszDateReceived)
			lpszDateReceivedSer = mm.lpszDateReceived;

		lpszConversationID = mm.lpszConversationID;
		if (mm.lpszConversationID)
			lpszConversationIDSer = mm.lpszConversationID;
		
		flFlags = mm.flFlags;

		lpOriginator = mm.lpOriginator;
		if ( mm.lpOriginator )
			OriginatorSer = *mm.lpOriginator;
		
		RecipsSer.clear  ();
		FilesSer.clear  ();
		m_Recips.clear  ();
		m_Files.clear  ();
		
		nRecipCount = mm.nRecipCount;
		ULONG i = 0;
		for (; i<nRecipCount; ++i)
			RecipsSer.push_back  (mm.lpRecips[i]);

		lpRecips = mm.lpRecips;
		
		nFileCount = mm.nFileCount;
		for (i = 0; i<nFileCount; ++i)
			FilesSer.push_back  (mm.lpFiles[i]);

		lpFiles = mm.lpFiles;
	}
	
protected:
	std::string lpszSubjectSer;            /* Message Subject                        */
    std::string lpszNoteTextSer;           /* Message Text                           */
    std::string lpszMessageTypeSer;        /* Message Class                          */
    std::string lpszDateReceivedSer;       /* in YYYY/MM/DD HH:MM format             */
    std::string lpszConversationIDSer;     /* conversation thread ID                 */
	
	MapiRecipDescSer OriginatorSer; /* Originator descriptor                  */
    std::vector<MapiRecipDescSer> RecipsSer;     /* Recipient descriptors                  */
	std::vector<MapiFileDescSer> FilesSer;       /* Attachment descriptors                 */
	
	std::vector<MapiRecipDesc> m_Recips;
	std::vector<MapiFileDesc> m_Files;
	
};

inline PoliType& operator<<(PoliType& pt, MapiMessageSer& mm) 
{	
	return mm.MarshalImpl(pt);
}

#endif //_MAPI_WRAPPER_H_