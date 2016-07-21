#include "isexecbuff.h"

#include <AV/plugin/p_avlib.h>

#include <string.h>

#define EXT_XOR_VALUE 0x55555555 // used to hide extension list in module listing
#define _EXT(ext)           (((tDWORD)ext>>24) | (((tDWORD)ext>>8) & 0xFF00) | (((tDWORD)ext<<8) & 0xFF0000) | ((tDWORD)ext<<24))
#define CHECK_EXT(ext)      case (_EXT(ext) ^ EXT_XOR_VALUE): 
#define MAKE_DWORD_BE(_1, _2, _3, _4) (((_1) << 24) | ((_2) << 16) | ((_3) << 8) | (_4))

static __inline tBOOL CheckHXS (const tBYTE * Page, size_t Len);

tERROR pr_call AVLIB_IsExecutableExtensionBuff(tBYTE* pExt, tDWORD dwSize, tDWORD* pdwResult)
{
	tDWORD dwExt = 0;
	tDWORD dwFormat = 0;

	if (pdwResult==NULL || pExt==NULL)
		return errPARAMETER_INVALID;
	*pdwResult = 0;
	if (dwSize < 3)
		//return errBUFFER_TOO_SMALL;
		return errOK;
	if (*pExt != '.')
		return errPARAMETER_INVALID;
	pExt++;
	dwSize--;

	if (dwSize >= sizeof(tDWORD))
		dwExt = *(tDWORD*)pExt;
	else
		memcpy(&dwExt, pExt, min(dwSize, sizeof(tDWORD)));
	dwExt |= 0x20202020; // lowercase

	PR_TRACE((NULL, prtNOTIFY, "IsExecutableExtension: %.*s (%.*s)", 4, &dwExt, dwSize, pExt));

	switch(dwExt ^ EXT_XOR_VALUE)   
	{
	CHECK_EXT( 0x33383620 /* '.386' */  ) // Protected Mode Driver
	CHECK_EXT( 0x61636d20 /* '.acm' */  ) // Audio Codec DLL
	CHECK_EXT( 0x61646520 /* '.ade' */  ) // Microsoft Access project extension 
	CHECK_EXT( 0x61647020 /* '.adp' */  ) // Microsoft Access project 
	CHECK_EXT( 0x61737020 /* '.asp' */  ) // Active Server Page
	CHECK_EXT( 0x61737820 /* '.asx' */  ) // Windows Media Audio / Video
	CHECK_EXT( 0x61782020 /* '.ax ' */  ) // Audio Decoder DLL
	CHECK_EXT( 0x62617320 /* '.bas' */  ) // Microsoft Visual Basic class module 
	CHECK_EXT( 0x62617420 /* '.bat' */  ) // Microsoft MS-DOS Command script (Batch file)
	CHECK_EXT( 0x62696e20 /* '.bin' */  ) // Binary file (data or code)
	CHECK_EXT( 0x63686d20 /* '.chm' */  ) // Compiled HTML Help file 
	CHECK_EXT( 0x636c6120 /* '.cla' */  ) // Java Class
	CHECK_EXT( 0x636c6173 /* '.clas*' */) // Java Class
	CHECK_EXT( 0x636d6420 /* '.cmd' */  ) // Microsoft Windows NT Command script 
	CHECK_EXT( 0x636f6d20 /* '.com' */  ) // Microsoft MS-DOS program 
	CHECK_EXT( 0x63706c20 /* '.cpl' */  ) // Control Panel extension 
	CHECK_EXT( 0x63727420 /* '.crt' */  ) // Security certificate 
	CHECK_EXT( 0x646c6c20 /* '.dll' */  ) // Dynamic Link Library
	CHECK_EXT( 0x64706c20 /* '.dpl' */  ) // Borland's Delphi files
	CHECK_EXT( 0x64727620 /* '.drv' */  ) // Driver
	CHECK_EXT( 0x64766220 /* '.dvb' */  ) // Autodesk AutoCAD VB Project
	CHECK_EXT( 0x64776720 /* '.dwg' */  ) // Autodesk AutoCAD Drawing
	CHECK_EXT( 0x65666920 /* '.efi' */  ) // MZ files found in Windows Vista
	CHECK_EXT( 0x656d6620 /* '.emf' */  ) // Enhanced Windows Metafile
	CHECK_EXT( 0x656d6c20 /* '.eml' */  ) // Email message (RFC-822)
	CHECK_EXT( 0x65786520 /* '.exe' */  ) // Program 
	CHECK_EXT( 0x666f6e20 /* '.fon' */  ) // Font
	CHECK_EXT( 0x66706d20 /* '.fpm' */  ) // Microsoft Frontpage Macros
	CHECK_EXT( 0x686c7020 /* '.hlp' */  ) // Help file 
	CHECK_EXT( 0x68746120 /* '.hta' */  ) // HTML Application
	CHECK_EXT( 0x68746d20 /* '.htm' */  ) // HTML file
	CHECK_EXT( 0x68746d6c /* '.html*' */) // HTML file
	CHECK_EXT( 0x68747420 /* '.htt' */  ) // HTML temlate
	CHECK_EXT( 0x69636f20 /* '.ico' */  ) // Windows Icon
	CHECK_EXT( 0x696e6620 /* '.inf' */  ) // Setup Information 
	CHECK_EXT( 0x696e6920 /* '.ini' */  ) // INI-file (MIRC, PIRCH)
	CHECK_EXT( 0x696e7320 /* '.ins' */  ) // Internet Naming Service 
	CHECK_EXT( 0x69737020 /* '.isp' */  ) // Internet Communication settings 
	CHECK_EXT( 0x6a706720 /* '.jpg' */  ) // JPEG image file format
	CHECK_EXT( 0x6a706567 /* '.jpeg' */ ) // JPEG image file format
	CHECK_EXT( 0x6a732020 /* '.js ' */  ) // JScript file 
	CHECK_EXT( 0x6a736520 /* '.jse' */  ) // Jscript Encoded Script file 
	CHECK_EXT( 0x6c6e6b20 /* '.lnk' */  ) // Shortcut 
	CHECK_EXT( 0x6d627820 /* '.mbx' */  ) // Mailbox (Eudora)
	CHECK_EXT( 0x6d736320 /* '.msc' */  ) // Microsoft Common Console document 
	CHECK_EXT( 0x6d736720 /* '.msg' */  ) // Microsoft Outlook message (OLE2 storage)
	CHECK_EXT( 0x6d736920 /* '.msi' */  ) // Microsoft Windows Installer package 
	CHECK_EXT( 0x6d737020 /* '.msp' */  ) // Microsoft Windows Installer patch 
	CHECK_EXT( 0x6d737420 /* '.mst' */  ) // Microsoft Visual Test source files 
	CHECK_EXT( 0x6e767320 /* '.nvs' */  ) // 
	CHECK_EXT( 0x6e777320 /* '.nws' */  ) // Email files
	CHECK_EXT( 0x6f637820 /* '.ocx' */  ) // ActiveX control
	CHECK_EXT( 0x6f667420 /* '.oft' */  ) // Microsoft Outlook form template
	CHECK_EXT( 0x6f746d20 /* '.otm' */  ) // Microsoft Outlook macro
	CHECK_EXT( 0x70636420 /* '.pcd' */  ) // Photo CD image, Microsoft Visual compiled script 
	CHECK_EXT( 0x70646620 /* '.pdf' */  ) // Adobe AcrobatReader
	CHECK_EXT( 0x70687020 /* '.php' */  ) // PHP script
	CHECK_EXT( 0x70687420 /* '.pht' */  ) // PHTML 
	CHECK_EXT( 0x7068746d /* '.phtm*' */) // PHTML 
	CHECK_EXT( 0x70696620 /* '.pif' */  ) // Shortcut to MS-DOS program 
	CHECK_EXT( 0x706c6720 /* '.plg' */  ) // Email files
	CHECK_EXT( 0x706e6720 /* '.png' */  ) // PNG files
	CHECK_EXT( 0x706f7420 /* '.pot' */  ) // 
	CHECK_EXT( 0x70726620 /* '.prf' */  ) // Microsoft Outlook profile settings
	CHECK_EXT( 0x70726720 /* '.prg' */  ) // 
	CHECK_EXT( 0x72656720 /* '.reg' */  ) // Registration entries 
	CHECK_EXT( 0x72736320 /* '.rsc' */  ) // 
	CHECK_EXT( 0x72746620 /* '.rtf' */  ) // Rich Text Format file
	CHECK_EXT( 0x73636620 /* '.scf' */  ) // Windows Explorer command
	CHECK_EXT( 0x73637220 /* '.scr' */  ) // Screen saver 
	CHECK_EXT( 0x73637420 /* '.sct' */  ) // Windows Script Component 
	CHECK_EXT( 0x73686220 /* '.shb' */  ) // Shell Scrap object
	CHECK_EXT( 0x73687320 /* '.shs' */  ) // Shell Scrap object 
	CHECK_EXT( 0x73687420 /* '.sht' */  ) // Secure HTML (shtm, shtml)
	CHECK_EXT( 0x7368746d /* '.shtm*' */) // Secure HTML (shtm, shtml)
	CHECK_EXT( 0x73776620 /* '.swf' */  ) // Shockwave Flash
	CHECK_EXT( 0x73797320 /* '.sys' */  ) // Driver
	CHECK_EXT( 0x74686520 /* '.the' */  ) // Microsft Plus! Theme
	CHECK_EXT( 0x7468656c /* '.them*' */) // Microsft Plus! Theme
	CHECK_EXT( 0x74737020 /* '.tsp' */  ) // 
	CHECK_EXT( 0x75726c20 /* '.url' */  ) // Internet shortcut 
	CHECK_EXT( 0x76622020 /* '.vb ' */  ) // VBScript file 
	CHECK_EXT( 0x76626520 /* '.vbe' */  ) // VBScript Encoded script file 
	CHECK_EXT( 0x76627320 /* '.vbs' */  ) // VBScript file 
	CHECK_EXT( 0x76786420 /* '.vxd' */  ) // Virtual Device Driver
	CHECK_EXT( 0x776d6120 /* '.wma' */  ) // Windows Media Audio
	CHECK_EXT( 0x776d6620 /* '.wmf' */  ) // Windows Metafile
	CHECK_EXT( 0x776d7620 /* '.wmv' */  ) // Windows Media Video
	CHECK_EXT( 0x77736320 /* '.wsc' */  ) // Windows Script Component 
	CHECK_EXT( 0x77736620 /* '.wsf' */  ) // Windows Script file 
	CHECK_EXT( 0x77736820 /* '.wsh' */  ) // Windows Script Host Settings file  
		dwFormat|=cAVLIB_EXECUTABLE_EXTENSION;
	}


	switch( ((dwExt & 0xFFFF00FF) | 0x3F00) ^ EXT_XOR_VALUE )	  // '.xx?'
	{
	CHECK_EXT( 0x646f3f20 /* '.do?' */  ) // Microsoft Word Files (doc,dot,...)
	CHECK_EXT( 0x6d643f20 /* '.md?' */  ) // Microsoft Access files (mda, mdb, mde, mdz)
	CHECK_EXT( 0x6d703f20 /* '.mp?' */  ) // Microsoft Project files
	CHECK_EXT( 0x6f763f20 /* '.ov?' */  ) // MS-DOS program overlay
	CHECK_EXT( 0x70703f20 /* '.pp?' */  ) // Microsoft PowerPoint files (pps, ppt)
	CHECK_EXT( 0x76733f20 /* '.vs?' */  ) // 
	CHECK_EXT( 0x786c3f20 /* '.xl?' */  ) // Mirosoft Excel files (xla, xlb, xlm, xls, xlt, xlw, ...)
		dwFormat|=cAVLIB_EXECUTABLE_EXTENSION;
	}

	// Office 12
	switch(dwExt ^ EXT_XOR_VALUE)   
	{
	CHECK_EXT( MAKE_DWORD_BE( 'd', 'o', 'c', 'm' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'd', 'o', 'c', 'x' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'd', 'o', 't', 'm' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'd', 'o', 't', 'x' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'p', 'o', 't', 'm' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'p', 'o', 't', 'x' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'p', 'p', 'a', 'm' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'p', 'p', 's', 'm' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'p', 'p', 's', 'x' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'p', 'p', 't', 'm' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'p', 'p', 't', 'x' ) )
	CHECK_EXT( MAKE_DWORD_BE( 's', 'l', 'd', 'm' ) )
	CHECK_EXT( MAKE_DWORD_BE( 's', 'l', 'd', 'x' ) )
	CHECK_EXT( MAKE_DWORD_BE( 't', 'h', 'm', 'x' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'x', 'l', 'a', 'm' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'x', 'l', 's', 'b' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'x', 'l', 's', 'm' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'x', 'l', 's', 'x' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'x', 'l', 't', 'm' ) )
	CHECK_EXT( MAKE_DWORD_BE( 'x', 'l', 't', 'x' ) )
		dwFormat|=cAVLIB_EXECUTABLE_EXTENSION;
	}

	*pdwResult = dwFormat;

	return errOK;
}

tERROR pr_call AVLIB_IsExecutableFormatBuff(tBYTE* pInData, tDWORD dwSize, tDWORD* pdwResult)
{
	tBYTE   bLocalData[cFORMAT_DATA_SIZE+1];
	tBYTE*  pData;
	tWORD	w0;
	tUINT	ptr, flags, i;
	tUINT	Coinstances;	// to detect random Hybris plugins

#define DB(offs) (*(tBYTE*)(pData+offs))
#define DW(offs) (*(tWORD*)(pData+offs))
#define DD(offs) (*(tDWORD*)(pData+offs))

	if (pdwResult==NULL || pInData==NULL || dwSize==0)
		return errPARAMETER_INVALID;

	if (dwSize >= cFORMAT_DATA_SIZE)
		pData = pInData;
	else
	{
		pData = bLocalData;
		memcpy(pData, pInData, dwSize);
		memset(pData + dwSize, 0x0F, cFORMAT_DATA_SIZE - dwSize);
	}

    if (    PR_SUCC(AVLIB_IsExecutableFormatBuffLite (pData, dwSize, pdwResult))
         && (*pdwResult & cAVLIB_EXECUTABLE_FORMAT))
    {
        goto Ret_Executable;
    }

	// is PIF file? -----------------------------------------------------

	if (DB(0)!=0x00 && DB(0)!=0x20)	goto NotPIFFile; // no idea why these values...
	if (DB(1)<0x38 || DB(1)>0xE0)	goto NotPIFFile; // no idea why these values...

	switch ( DW(0x20) )	{
		case 0x0080:
		case 0x0200: case 0x0280:	break;
		default:					goto NotPIFFile;
	}
	switch ( DW(0x22) )	{
		case 0x0000: case 0x0080:
		case 0x0100: case 0x2020: 	break;
		default:					goto NotPIFFile;
	}

	for (i=2;i<0x20;i++)	{
		if (DB(i)<0x20 || DB(i)>'~')	goto NotPIFFile;
	}
	for (i=0x24;i<0x40;i++)	{
		if (DB(i)==0)		continue;
		if (DB(i)<0x20 || DB(i)>'~')	goto NotPIFFile;
	}
	// Printf("looks like PIF file");
	goto Ret_Executable;


NotPIFFile:

	// is DOS COM program? ----------------------------------------------

	// check if that is RIFF (Corel) - no need to scan them
	if ( DD(0)==0x46464952 && DB(7)==0 )	goto Not_DOS_COM_file;

	#define	d0	((tBYTE)w0)
	#define	d1	(((tBYTE*)(&w0))[1])
	#define	WAS_PUSH	0x0001

	ptr=flags=0;	goto DOS_Loop;
	Opc4:	ptr++;
	Opc3:	ptr++;
	Opc2:	ptr++;
	Opc1:	ptr++;
DOS_Loop:
	if (ptr>0x30)	goto Ret_Executable;

	w0=DW(ptr);	// Printf("DOS code: %04X: %02X %02X",ptr,d0,d1);

	switch (d0&0xF0)	{
		case 0x40:			goto Opc1;
		case 0x70:
			if (d1<0x80 || d1>0xE0)	goto Opc2;	// JMP tSHORT
			break;
	}

	if (d0==0xBC)	{
		flags|=WAS_PUSH;
		goto Opc3;
	}

	switch (d0&0xF8)	{
		case 0x50: 	flags|=WAS_PUSH;
		case 0x58:	goto Opc1;
		case 0xB8:	ptr++;
		case 0xB0:	ptr++;
		case 0x90:	goto Opc1;
	}

	switch (d0)	  {
		case 0xE9: case 0xE8:
			if ( ((tWORD*)(pData+ptr+1))[0] == 0 )	goto Opc3;
			d1=((tBYTE*)(pData+ptr+1))[0];
			if (d1<0xA0 || d1>=0xF0)	goto Ret_Executable;	// JMP or CALL
			d1=((tBYTE*)(pData+ptr+2))[0];
			if (d1<0xA0 || d1>=0xF0)	goto Ret_Executable;	// JMP or CALL
			break;

		case 0xEB:
			if (d1==0)			goto Opc2;
			if (d1<0x80 || d1>0xE0)		goto Ret_Executable;	// JMP tSHORT
			break;

		case 0xC3: case 0xCB:
			if ((flags&WAS_PUSH) && ptr>2)	goto Ret_Executable;	// RET
			break;

		case 0xCD:
			if (d1!=0 && d1<0x40)			goto Ret_Executable;
			break;
		case 0x9A: case 0xEA:
			if (ptr>5)			goto Ret_Executable;	// CALL/JMP
			break;
		case 0xFF:
			if (d1>=0xC0 && ptr>2)	{	// must be init
				switch (d1&0x38)	{
					case 0x10: case 0x20:	goto Ret_Executable;	// CALL/JMP reg
					default: 		break;
				}
			}

			switch (d1)	{
				case 0x16: case 0x1E:			// CALL data
				case 0x26: case 0x2E:	goto Ret_Executable;	// JMP data
				case 0x36:		flags|=WAS_PUSH;
							goto Opc4;
			}
			break;

		case 0x06: case 0x0E: case 0x16: case 0x1E:
		case 0x07:            case 0x17: case 0x1F:
		case 0x26: case 0x2E: case 0x36: case 0x3E:
		case 0x27:
		case 0x60: case 0x61:
		case 0x99: case 0x9C: case 0x9D:
		case 0xA4: case 0xA5: case 0xA6: case 0xAA: case 0xAB: case 0xAC: case 0xAD:
		case 0xCC:
		case 0xF3:
		case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD:
							goto Opc1;

		case 0x0D:				if (d1==0x0A)	break;
			
                case 0x05: case 0x25: case 0x2D: case 0x35: case 0x3D:
		case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA9:
							ptr++;
		case 0x04: case 0x24: case 0x2C: case 0x34: case 0x3C:
							goto Opc2;

		case 0x68:				ptr++;
		case 0x6A:	flags|=WAS_PUSH;	goto Opc2;

		case 0x81: case 0xC7:			ptr++;
		case 0x80: case 0x83: case 0xC0: case 0xC1: case 0xC6:
							ptr++;
		case 0x01: case 0x03: case 0x0A: case 0x23: case 0x2A: case 0x2B:
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x39: case 0x3B:
		case 0x86: case 0x87: case 0x88: case 0x89: case 0x8A:
		case 0x8B: case 0x8C: case 0x8D: case 0x8E:
		case 0xC4:
		case 0xD1: case 0xD3:
		case 0xF6: case 0xF7: case 0xFE:
			switch (d1&0xC0)	{
				case 0x00:		if ((d1&7)==6)	ptr+=2;
				case 0xC0:		goto Opc2;
				case 0x80:		goto Opc4;
				case 0x40:		goto Opc3;
			}
			break;
		case 0xE0:
		case 0xE2:
			if (ptr<3)		break;
			if (d1<0xF8-ptr)	break;
                        goto Ret_Executable;
		case 0xE4:
			if (d1==0x40)	goto Opc2;
			break;
	}

	// Printf("Bad code %02X %02X at %04X",d0,d1,ptr);
	#undef	WAS_PUSH
	#undef	d0
	#undef	d1

Not_DOS_COM_file:
	// is Script program? -------------------------------------------------

	flags=ptr=0;	// ptr is "error", flags is Token counter

	for (i=0;i<0x3C;i++)	{
		switch (DB(i))	{
			case 0x27:		// VBS comment?
				if (i==0)	flags+=2;
				if (i>0)	{	// if there was new line
						if (DB(i-1)==0x0A)	flags+=2;
				}
				break;

			case 7: case 9: case 0x0D: case 0x0A: break;
			default:
				if (flags>=4 && flags>=ptr)	goto Ret_Executable;	// there are tSHORT file with garbage in Header...
				if (DB(i)<0x20 || DB(i)>0xF8)	ptr++;
				if (ptr>10)	goto NotScriptFile;
		}

		switch ( DD(i) | 0x20202020 )	{
		// DOS Batch
			case 0x206D6572:		// rem
			case 0x7461622E:		// .bat
			case 0x20302520:		// %0
			case 0x20666960:		// @if
					flags+=2;			// plus 4
			case 0x79747463:		// ctty
			case 0x6F686365:		// echo
			case 0x6F746F67:		// goto
			case 0x206E6572:		// ren
					flags++;			// plus 2
			case 0x6C6C6163:		// call
			case 0x79706F63:		// copy
			case 0x646E6966:		// find
			case 0x20726F66:		// for
			case 0x20746573:		// set
					flags++;			// plus 1
					if (i>0)	{	// if there is '@' prefix
						if (DB(i-1)=='@')	flags+=2;
					}
	   				// Printf("DOS %04X %c%c%c%c Value=%d Errors=%d",(tWORD)i,DB(i),DB(i+1),DB(i+2),DB(i+3),flags,ptr);
					break;
		// HTML
			case 0x6C6D7468:		// html
					flags++;
			case 0x6174656D:		// meta
			case 0x70747468:		// http
			case 0x64616568:		// head
			case 0x6C746974:		// title
					flags++;
					if (i>0)	{	// if there is '<' prefix
						if (DB(i-1)=='<')	flags+=2;
					}
	   				// Printf("HTML %04X %c%c%c%c Value=%d Errors=%d",(tWORD)i,DB(i),DB(i+1),DB(i+2),DB(i+3),flags,ptr);
					break;
		// [script]
			case 0x69726373:	// scri
					if (i>0 && i<0x38)	{
						if ((DW(i+4)|0x2020)==0x7470 &&
							( (DB(i-1)=='[' && DB(i+6)==']') ||
							  (DB(i-1)=='<') ) )
								goto Ret_Executable;
					}
					break;
		// [Levels]
			case 0x6576656C:	// scri
					if (i>0 && i<0x38)	{
						if ((DW(i+4)|0x2020)==0x736C &&
							(DB(i-1)=='[' && DB(i+6)==']') )
								goto Ret_Executable;
					}
					break;

		// mIRC script
			case 0x31206E6f:	// "on 1"
					flags++;
			case 0x72686324:	// "$chr"
			case 0x6E696F6A:	// "join"
			case 0x6B63696E:	// "nick"
					flags++;
					break;

                // VBS,JS
			case 0x65206E6F:	// "on error"
					flags++;
					if (i<0x38)	{
						if ( (DD(4)|0x20202020)==0x726F7272)
							flags+=3;
					}
					break;
			case 0x206D6964:	// dim
			case 0x2C6F7366:	// fso, (standard virus name)
			case 0x206F7366:	// fso  (standard virus name)
			case 0x20726176:	// var
			case 0x6267736D:	// MsgB (msgbox)
			case 0x72637377:	// wscr (wscript)
			case 0x28726F66:	// for(
						flags++;
			case 0x2077656E:  	// new
			case 0x7478656E:	// next
						flags++;
						if (i>0)	{	// if there was new line
							if (DB(i-1)==0x0A)	flags+=2;
						}
						break;
		// HYBRIS messages from alt.comp.virus
               		case 0x68746170:	flags+=4;
               					break;

                }
        // Printf("%04X: flags=%d ptr=%d",i,flags,ptr);
	}

	// NOT DONE:
	// Printf("End of Script, flags=%d, ptr=%d",flags,ptr);
	if (flags>=4 && flags>=ptr)	
		goto Ret_Executable;	// at least one "good" token and not many errors

	// check for JavaScript comment
	{
		ptr = 0;
		while (ptr < 0x40 
			&& (DB(ptr) == ' ' 
				|| DB(ptr) == '\t' 
				|| DB(ptr) == '\r' 
				|| DB(ptr) == '\n'))
		{
			++ptr;
		}

		if (ptr < 0x3C 
			&& (DB(ptr) == '/' && DB(ptr+1) == '/'))
		{
			goto Ret_Executable;
		}
	}

NotScriptFile:
	// Printf("NOT script file: flags=%d ptr=%d",flags,ptr);


	// is random Hybris plugin? -------------------------------------------------
	
	//if (DB(0)>0x80 || DB(1)>0x80 || DB(2)>0x80 || DB(3)>0x80) { // by Mike
	if (DD(0) & 0x80808080) {
		Coinstances=0;
		for (ptr=0;ptr<0x3F;ptr++)	{	// calc num of bytes that are equal
			if (Coinstances>=12)	
				break;
			for (i=ptr+1;i<0x40;i++) {
				if (DB(ptr)==DB(i)) {
					Coinstances++;
					break;
				}
			}
		}
		
		// Printf("%d", Coinstances);
		if (Coinstances<12)		goto Ret_Executable;	// data seem to be random
	}
    // Printf("NOT rnd file (hybris)");

	*pdwResult = 0;
	return errOK;

Ret_Executable:	// Printf("It is a program");
	*pdwResult = cAVLIB_EXECUTABLE_FORMAT;
	return errOK;

#undef DD
#undef DW
#undef DB
}

tERROR pr_call AVLIB_IsExecutableFormatBuffLite(tBYTE* pInData, tDWORD dwSize, tDWORD* pdwResult)
{
	tBYTE   bLocalData[cFORMAT_DATA_SIZE+1];
	tBYTE*  pData;
	tUINT	i;

#define DB(offs) (*(tBYTE*)(pData+offs))
#define DW(offs) (*(tWORD*)(pData+offs))
#define DD(offs) (*(tDWORD*)(pData+offs))

	if (pdwResult==NULL || pInData==NULL || dwSize==0)
		return errPARAMETER_INVALID;

	if (dwSize >= cFORMAT_DATA_SIZE)
		pData = pInData;
	else
	{
		pData = bLocalData;
		memcpy(pData, pInData, dwSize);
		memset(pData + dwSize, 0x0F, cFORMAT_DATA_SIZE - dwSize);
	}

	
	// Get File Format ----------------

	if ( DW(2)==0xFFFF )	goto Ret_Executable;	// DOS SYS

	switch( DW(0) )	{
		case 0x4D5A:
		case 0x5A4D:	if (!CheckHXS(pInData, dwSize)) goto Ret_Executable;				// DOS EXE

		case 0x5746:	if (DB(2)==0x53)	goto Ret_Executable;	// ShockwaveFlash
        case 0xD8FF:    if (DB(2)==0xFF)    goto Ret_Executable;    // JPEG
				break;
	}

	switch ( DD(0) )	{
		case 0x464C457F:			// ELF
		case 0x00035F3F:			// HLP
		case 0xBEBAFECA:	goto Ret_Executable;	// Java

		case 0xE011CFD0:			// OLE2
			if (DD(4)==0xE11AB1A1)	goto Ret_Executable;
			break;
		case 0x0DFC110E:			// OLE2 inverted (OLE beta2)
			if (DD(4)==0xE011CFD0)	goto Ret_Executable;
			break;

		case 0x00000100:			// Access
			if (DD( 4)==0x6E617453 &&
				DD( 8)==0x64726164 &&
				DD(12)==0x74654A20)	goto Ret_Executable;
			break;
		case 0x622F2123:			// sh
		case 0x752F2123:			// Perl
		case 0x73752F23:			// Perl - #/usr/bin/perl
		case 0x6D783F3C:			// XML
		case 0x46535449:			// ITSF - CHM (compressed HTML)
		case 0x45474552:			// REG (REGEDIT)
                case 0x46445025:	  		// PDF - AcrobatReader
		case 0x626F6A3C:			// WSF "<job"
		case 0x454D494D:			// MIME ("Nimda" worm)
		case 0x30304B50:			// 'PK00' ZIP files, are missed by scanner.
			goto Ret_Executable;

		case 0x0000004C:			// Win lnk
			if (DD( 4)==0x00021401 &&
				DD( 8)==0x00000000 &&
				DD(12)==0x000000C0)
				goto Ret_Executable;
			break;

		case 0x72747441:		// extracted VBA source code - "Attribute VB"
			if (DD( 4)==0x74756269 &&
				DD( 8)==0x42562065)
				goto Ret_Executable;
			break;

        case 0x75B22630:        // Microsoft Media Audio/Video File (Advanced Streaming Format)
        						// WMA, WMF, ASF
            if (DD( 4)==0x11CF668E &&
                DD( 8)==0xAA00D9A6 &&
                DD(12)==0x6CCE6200)
				goto Ret_Executable;
            break;

		case 0x7263535B:		// [ScriptFile]
		case 0x7263735B:
			if ( (DD( 4) | 0x20202020) == 0x66747069 )
				goto Ret_Executable;
			break;

		case 0x214F3558:		// EICAR.COM
		case 0x8B665566:		// NTDETECT.COM
			
		case 0x0424748B:		// decrypted Hybris plugin
		case 0x04247C8B:
			goto Ret_Executable;

		case 0x474E5089:        // PNG
			if ( (DD( 4)) == 0x0A1A0A0D )
				goto Ret_Executable;
			break;
    }

    /* WMF */
    {
        if (DD(0) == 0x9AC6CDD7)
            goto Ret_Executable;

        {
            const int Offsets[] = { 0, 8, 16 };

            for (i = 0; i < countof (Offsets); ++i)
            {
                if (    (DD(Offsets[i]) == 0x00090001 || DD(Offsets[i]) == 0x00090002)
                    &&  (DW(Offsets[i] + 4) == 0x0300 || DW(Offsets[i] + 4) == 0x0100))
                {
                    goto Ret_Executable;
                }
            }
        }
    }

	if (DD(0x28) == 0x464D4520)	// EMF
		goto Ret_Executable;

	if (DB(0) == '!')
	{   
		switch (DD(0) | 0x20202000)
		{
			case 0x70706121:        // !app - MapBasic Application 
			case 0x62617421:        // !table - MapInfo table
			case 0x726F7721:        // !workspace - MapInfo workspace
				goto Ret_Executable;
		}
	}

    if (DB(0) == 0xEB && DB(1) < 0x80)
        goto Ret_Executable;	// JMP SHORT

	*pdwResult = 0;
	return errOK;

Ret_Executable:	// Printf("It is a program");
	*pdwResult = cAVLIB_EXECUTABLE_FORMAT;
	return errOK;

#undef DD
#undef DW
#undef DB
}

static __inline
unsigned long CalcSum( const tBYTE* mass,unsigned long len)
{
    register unsigned long i;
    register unsigned long s;
    register const tBYTE* m;
    s=0;
    m=mass;
    if(len<4){
        for (i=0; i<len ; i++,m++){
            s^=s<<8;
            s^=*m;
        }
        return s;
    }
    
    for (i=0; i<4 ; i++,m++){
        s^=s<<8;
        s^=*m;
    }
    for ( ;i<len; i++,m++){
        s^=s<<1;
        s^=*m;
    }
    return s;
}

static __inline
tBOOL CheckHXS (const tBYTE * Page, size_t Len)
{
    if (Len < 0x200)
        return cFALSE;
    
    if (memcmp (Page, "MZ\0\0", 4) != 0 || CalcSum (Page, 0x40) != 0x0498F8A7)
        return cFALSE;
    
    Page += 0x40;
    
    if (memcmp (Page, "PE\0\0", 4) != 0 || CalcSum (Page, 0xF8) != 0xA35A55F7)
        return cFALSE;
    
    Page += 0xF8;
    
    if (memcmp (Page, ".rsrc", 5) != 0 || CalcSum (Page, 0x50) != 0x98165E81)
        return cFALSE;
    
    return cTRUE;
}
