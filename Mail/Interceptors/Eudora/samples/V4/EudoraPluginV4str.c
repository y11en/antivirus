/*____________________________________________________________________________
	Copyright (C) 2005 PGP Corporation
	All rights reserved.

	$Id: EudoraPluginV4str.c 31157 2005-01-02 18:17:37Z wjb $
____________________________________________________________________________*/
#include "pgpPFLConfig.h"       /* or pgpConfig.h in the SDK */
#include "pgprc.h"
#include "resource.h"

PGPLocStringsContextRef g_CSStringContext=NULL;

PGPStrLocWChar *ModuleDependentCS( const char *string, const PGPChar *wide, const char *description )
{
	return MacroLookup(
		&g_CSStringContext,
		PGPTXT_MACHINE("PGPeudra"), 
		string, 
		wide,
		description );
}

const PGPStrLocWChar *GetPGPCSString(DWORD dwID)
{
	const PGPStrLocWChar *shared;

	shared=GetPGPCSSharedString(dwID);

	if(shared!=NULL)
		return shared;

	switch(dwID)
	{
		case IDS_WARN_NORECIPIENTS:
			return PGPCS("In order to Encrypt you must choose\nat least one recipient.","IDS_WARN_NORECIPIENTS");
		case IDS_WARN_NORECIPIENTSTITLE:
			return PGPCS("No Recpients Chosen","IDS_WARN_NORECIPIENTSTITLE");
		case IDS_TITLE_RECIPIENTDIALOG:
			return PGPCS("PGP Eudora - Encrypt Message To...","IDS_TITLE_RECIPIENTDIALOG");
		case IDS_TITLE_PGPERROR:
			return PGPCS("PGP Error","IDS_TITLE_PGPERROR");
		case IDS_SUCCESS_ADDKEY:
			return PGPCS("PGP successfully added the key(s) to your keyring.","IDS_SUCCESS_ADDKEY");
		case IDS_TITLE_ADDKEY:
			return PGPCS("PGP Eudora - Extract Key","IDS_TITLE_ADDKEY");
		case IDS_STRING8:
			return PGPCS("Reserved","IDS_STRING8");
		case IDS_STRING9:
			return PGPCS("Reserved","IDS_STRING9");
		case IDS_TOOLTIP1:
			return PGPCS("Decrypt PGP Encrypted Email Message","IDS_TOOLTIP1");
		case IDS_TOOLTIP2:
			return PGPCS("Extract PGP Key(s) from Email Message","IDS_TOOLTIP2");
		case IDS_TOOLTIP3:
			return PGPCS("Launch PGP Desktop Application","IDS_TOOLTIP3");
		case IDS_TOOLTIP4:
			return PGPCS("Just Do It!","IDS_TOOLTIP4");
		case IDS_TOOLTIP5:
			return PGPCS("PGP Encrypt Email Message","IDS_TOOLTIP5");
		case IDS_TOOLTIP6:
			return PGPCS("PGP Sign Email Message","IDS_TOOLTIP6");
		case IDS_TOOLTIP7:
			return PGPCS("PGP Encrypt and Sign Email Message","IDS_TOOLTIP7");
		case IDS_TOOLTIP8:
			return PGPCS("Use PGP/MIME When Sending Email Message","IDS_TOOLTIP8");
		case IDS_E_EXPIRED:
			return PGPCS("The evaluation period for PGP has passed.\nThe Eudora Plugin will no longer function.","IDS_E_EXPIRED");
		case IDS_E_EXPIREDTITLE:
			return PGPCS("PGP Plugin Expired","IDS_E_EXPIREDTITLE");
		case IDS_E_LAUNCHPGPKEYS:
			return PGPCS("Unable to locate the PGP Desktop application","IDS_E_LAUNCHPGPKEYS");
		case IDS_EXE:
			return PGPCS("Eudora","IDS_EXE");
		case IDS_DLL:
			return PGPCS("PGPeudora.dll","IDS_DLL");
		case IDS_Q_NOKEYRINGS:
			return PGPCS("You must specify a keyring file and generate a pair of keys before you can fully use PGP.\r\nDo you wish launch PGP Desktop and do this now?","IDS_Q_NOKEYRINGS");
		case IDS_E_PGPMIME:
			return PGPCS("This message contains a PGP/MIME object, which is\r\nrepresented by an envelope with a lock on it placed in\r\nthe message itself. You must click on this icon to\r\ndecrypt/verify the message.","IDS_E_PGPMIME");
		case IDS_W_PGPMIME:
			return PGPCS("Because this message contains formatted text and/or attachments, the use of PGP/MIME is required in order to perform the requested PGP operation. The recipient(s) may not have a PGP/MIME compatible mailer/plugin combination to read your message.\r\n\r\nDo you wish to continue?","IDS_W_PGPMIME");
		case IDS_MUSTRELOAD:
			return PGPCS("Due to the way Eudora reads plugin settings at load time,\r\nyou must quit Eudora and restart it in order to use the new settings.","IDS_MUSTRELOAD");
		case IDS_Q_HASATTACHMENT:
			return PGPCS("This email contains an attachment that was not encrypted or\r\nsigned by PGP. If you continue decryption, you will not be\r\nable to open the attachment unless you close and re-open\r\nthis message. Do you want to continue decryption?","IDS_Q_HASATTACHMENT");
	}
	return NULL;
}
