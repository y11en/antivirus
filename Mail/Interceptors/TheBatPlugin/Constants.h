// Constants.h: constant definitions
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSTANTS_H__455A4401_9FBF_42D2_9EB7_7C8AA69B6029__INCLUDED_)
#define AFX_CONSTANTS_H__455A4401_9FBF_42D2_9EB7_7C8AA69B6029__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TheBat
{
//---------------------------------------------------------------------------
// Definitions of all constants which can be used as recognizable values
// of all integer constants defined in CP-API
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// current message properties
//---------------------------------------------------------------------------

enum midx
{
	midxSubject				= 1,
	midxDate				= 2,
	midxComment				= 3,
	midxInReplyTo			= 4,
	midxMessageID			= 5,
	midxNewsgroups			= 6,
	midxMailer				= 7,
	midxContentType			= 8,
	midxContentSubType		= 9,
	midxExpireDate			= 10,
	midxOrganization		= 11,
	midxContentID			= 12,
	midxContentMD5			= 13,
	midxPriority			= 14,
	midxImportance			= 15,
	midxContentLocation		= 16,
	midxEncoding			= 17,
	midxCharset				= 18,
	midxBoundary			= 19,
	midxMsgEncoding			= 20,
	midxC_Name				= 21,
	midxCD_Name				= 22,
	midxReportType			= 23,
	midxReferences			= 24,
	midxC_Description		= 25,
	midxContentDisposition	= 26,
	midxContentLanguage		= 27,
	midxC_ID				= 29, // warning! "28" is not defined
	midxC_AccessType		= 30,
	midxC_Expiration		= 31,
	midxC_Size				= 32,
	midxC_Permission		= 33,
	midxC_Site				= 34,
	midxC_Directory			= 35,
	midxC_Mode				= 36,
	midxC_Server			= 37,
	midxC_Subject			= 38,
	midx_Files				= 39,
	midx_Msgs				= 40,
	midxReturnPath			= 41,
	midxFromName			= 42,
	midxFromAddr			= 43,
	midxReplyName			= 44,
	midxReplyAddr			= 45,
	midxToName				= 46,
	midxToAddr				= 47,
	midxServerID			= 48,
	midxRRC					= 49,
	midxRRQ					= 50,
	midxFileSubst			= 51,
	midxC_Number			= 52,
	midxC_Total				= 53,
	midxXCFile				= 54,
	midxMDN_To				= 55,
	midxMDN_Options			= 56,
	midxRefList				= 57,
	midxC_MICAlg			= 58, // "midxC_..." used for content_type parameters
	midxC_SMIMEType			= 59,
	midxC_Protocol			= 60,
	midxC_ProtocolType		= 61,
	midxC_ProtocolSubType	= 62,
	midxMatter				= 63,
	midxListHelp			= 64,
	midxListUnsub			= 65,
	midxListSub				= 66,
	midxListPost			= 67,
	midxListOwner			= 68,
	midxListArchive			= 69,
	midxDecodedFrom			= 70,
	midxDecodedTo			= 71,
	midxDecodedSubj			= 72,
	midxDecodedToEtc		= 73,
	midxFrom				= 74,
	midxTo					= 75,
	midxCC					= 76,
	midxBCC					= 77,
	midxReplyTo				= 78,
	midxSender				= 79,
	midxXSender				= 80,

	// Global parts of the message
	//////////////////////////////
	mpidMessageHeader		= 100000,	// RFC 822 message header
	mpidMessageBody			= 100001,	// Decoded message text converted into 
										// Windows local code page
	mpidMessageAttachments	= 100002,	// List of attachments separated by NULL character. The end
										// of the list is determined by double NULL character.
	mpidMessageSender		= 100003,	// Decoded list of senders converted into
										// Windows local code page
	mpidMessageSubject		= 100004,	// Decoded message subject converted into
										// Windows local code page
	mpidRawMessage			= 100005	// The entire message source without any conversion made.
										// This may be useful if the Plug-in parses message by itself.
};


// Indexes of possibly properties for Template Processor object
//-------------------------------------------------------------

enum TPXindex
{
	tpxQuotePrefix		= 200, // String
	tpxWrapJustify		= 201, // Boolean
	tpxClear			= 202, // Boolean
	tpxIsSignature		= 203, // Boolean
	tpxSignComplete		= 204, // Integer
	tpxEncryptComplete	= 205, // Integer
	tpxUseSMIME			= 206, // Integer
	tpxUsePGP			= 207, // Integer
	tpxRCR				= 208, // Integer
	tpxRRQ				= 209, // Integer
	tpxSplit			= 210, // Integer
	tpxCharset			= 211, // String
	tpxPriority			= 212, // Integer
	tpxAccount			= 214, // String
	tpxFrom				= 215, // String
	tpxReplyTo			= 216, // String
	tpxReturnPath		= 217, // String
	tpxTo				= 218, // String
	tpxCC				= 219, // String
	tpxBCC				= 220, // String
	tpxOrg				= 221, // String
	tpxSubject			= 222, // String
	tpxFullSubject		= 223, // String
	tpxComment			= 224, // String
	tpxOldTo			= 225, // String
	tpxOldFrom			= 226, // String
	tpxOldReplyTo		= 227, // String
	tpxOldCC			= 228, // String
	tpxOldBCC			= 229, // String
	tpxOldSubject		= 230, // String
	tpxOldComment		= 231, // String
	tpxMatter			= 232, // String
	tpxOldMatter		= 233, // String
	tpxMsgID			= 234, // String
	tpxOldMsgID			= 235, // String
	tpxOldDate			= 236, // String
	tpxOldRcvDate		= 237, // String
	tpxOldReturn		= 238, // String
	tpxOldOrg			= 239, // String
	tpxOldText			= 240, // String
	tpxText				= 241, // String
	tpxHeaders			= 243, // String
	tpxAttachments		= 244, // String
	tpxOldAttachments	= 245, // String
	tpxOldCharset		= 247, // String
	tpxTracking			= 248, // String
	tpxQuoteStyle		= 249, // String
	tpxRegExpPattern	= 251, // String
	tpxRegExpText		= 252, // String
	tpxFullText			= 254, // String
	tpxLastAddress		= 257, // String
	tpxFullTextDifferent= 259, // Integer
	tpxCursorBody		= 260, // Integer
	tpxCursorHeader		= 261, // String
	tpxTotalPages		= 262, // Integer
	tpxCurrentPage		= 263  // Integer
};

 
// IDType constants - for ITBPDataProvider interface
//---------------------------------------------------
enum IDtype
{
	dtcChar		= 0,
	dtcInt		= 1,
	dtcInt64	= 2,
	dtcWChar	= 3,
	dtcBool		= 5, // warning! "4" is not defined!
	dtcBinary	= 6,
	dtcFloat	= 7,
};

// return values for AV plugin functions
//---------------------------------------------------
enum AVC_RetCodes
{
	AVC_OK			=  0,	// the object is clean
	AVC_VirusFound	=  1,   // the object is infected
	AVC_Suspicious	=  2,	// the object is found as suspicious
	AVC_NotSupported = -1,	// function is not supported 
	AVC_CantBeCured	= -2,	// {Cure} the object is infected but cannot be cleaned from virus(es)
	AVC_Error		= -3,	// an error occurred while performing this function
};

}	// namespace TheBat

#endif // !defined(AFX_CONSTANTS_H__455A4401_9FBF_42D2_9EB7_7C8AA69B6029__INCLUDED_)
