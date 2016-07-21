#include "isprog.h"

UINT FAR IsProgram(BYTE FAR*stHeader, BYTE FAR*stExt)
{
	WORD	Ret_Code,Maybe_DOS_COM, w0;
	UINT	ptr, flags, i;
	UINT	Coinstances;	// to detect random Hybris plugins

	// stHeader - 0x40 bytes of File Header (stack)
	// stExt - ptr for File Extension (anycased, stack)

	Ret_Code=Maybe_DOS_COM=0;

// Get FileName Extension ----------------
	// Printf("%s %08lX",stExt,((DWORD FAR*)stExt)[0] | 0x20202020);
	switch( ((DWORD FAR*)stExt)[0] | 0x20202020 )   {
	case 0x6D6F632e:	  // com  DOS executable extensions
	case 0x6578652e:	  // exe
	case 0x7379732e:	  // sys
	case 0x6772702e:	  // prg
	case 0x6E69622e:	  // bin
	case 0x7461622e:	  // bat
	case 0x646d632e:	  // cmd
	case 0x6c70642e:	  // dpl - Borland's Delphi files
			Maybe_DOS_COM=1;

	case 0x6C6C642e:	  // dll  WIN executable extensions
	case 0x7263732e:	  // scr
	case 0x6c70632e:	  // cpl
	case 0x78636f2e:	  // ocx
	case 0x7073742e:	  // tsp
	case 0x7672642e:	  // drv
	case 0x6478762e:	  // vxd
	//case 0x6E6F662e:	  // fon
	//case 0x3638332e:	  // 386

	case 0x6669702e:	  // pif  WIN semi-executable extensions
	case 0x6b6e6c2e:	  // lnk
	case 0x6765722e:	  // reg

	case 0x696E692e:	  // ini - Script.Ini, etc
	case 0x616c632e:	  // cla - Java Class
	case 0x7362762e:	  // vbs - Visual Basic Script
	case 0x6562762e:	  // vbe - Visual Basic Script Encrypted
	case 0x20736A2e:	  // js  - Java Script
	case 0x65736A2e:	  // jse - Java Script Encrypted
	case 0x6D74682e:	  // htm - HTML pages
	case 0x7474682e:	  // htt - HTTP pages
	case 0x6174682e:	  // hta - HTA  (HTML applications)
	case 0x7073612e:	  // asp - 
	case 0x6D68632e:	  // chm - CHM  (compressed HTML)
	case 0x7468702e:	  // pht - PHTML
	case 0x7068702e:	  // php - PHP
	case 0x6873772e:	  // wsh
	case 0x6673772e:	  // wsf
	case 0x6568742e:	  // the (.theme)

	case 0x706c682e:	  // hlp - HELP pages

	case 0x6c6d652e:	  // eml - Email files
	case 0x73776e2e:	  // nws - Email files
	case 0x67736d2e:	  // msg - Email files
	case 0x676c702e:	  // plg - Email files
	case 0x78626d2e:	  // mbx - Eudora database

	case 0x636F642e:	  // doc - MS Office
	case 0x746F642e:	  // dot
	case 0x6D70662e:	  // fpm
	case 0x6674722e:	  // rtf
	case 0x7368732e:	  // shs
	case 0x6777642e:	  // dwg - Acad2000

	case 0x69736d2e:	  // msi - MS Installer
	case 0x6d746f2e:	  // otm - Outlook macro

	case 0x6664702e:	  // pdf - AcrobatReader
	case 0x6677732e:	  // swf - ShockwaveFlash

			Ret_Code|=1;
	}		// Printf("Extension: RetCode=%d",Ret_Code);

	switch( ((WORD FAR*)(stExt+1))[0] | 0x2020 )	  {
	case 0x766f:		// ov?  DOS executable extensions
			Maybe_DOS_COM=1;
//	case 0x6f64:		// do
	case 0x6c78:		// xl* - MS Office
	case 0x7070:		// pp*
	case 0x646D:		// md*

			Ret_Code|=1;
	}

	if (Maybe_DOS_COM)	  goto Ret;

// Get File Format ----------------

	if ( (( WORD FAR*)(stHeader+2))[0]==0xFFFF )	goto Ret2;	// DOS SYS

	switch( ((WORD FAR*)stHeader)[0] )	{
		case 0x4D5A:
		case 0x5A4D:	goto Ret2;				// DOS EXE

		case 0x5746:	if (stHeader[2]==0x53)	goto Ret2;	// ShockwaveFlash
				break;
	}

	switch ( ((DWORD FAR*) stHeader )[0] )	{
		case 0x464C457F:			// ELF
		case 0x00035F3F:			// HLP
		case 0xBEBAFECA:	goto Ret2;	// Java

		case 0xE011CFD0:			// OLE2
			if (((DWORD FAR*)(stHeader+4))[0]==0xE11AB1A1)	goto Ret2;
			break;
		case 0x0DFC110E:			// OLE2 inverted (OLE beta2)
			if (((DWORD FAR*)(stHeader+4))[0]==0xE011CFD0)	goto Ret2;
			break;

		case 0x00000100:			// Access
			if (((DWORD FAR*)(stHeader+ 4))[0]==0x6E617453 &&
				((DWORD FAR*)(stHeader+ 8))[0]==0x64726164 &&
				((DWORD FAR*)(stHeader+12))[0]==0x74654A20)	goto Ret2;
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
			goto Ret2;

		case 0x0000004C:			// Win lnk
			if (((DWORD FAR*)(stHeader+ 4))[0]==0x00021401 &&
				((DWORD FAR*)(stHeader+ 8))[0]==0x00000000 &&
				((DWORD FAR*)(stHeader+12))[0]==0x000000C0)
				goto Ret2;
			break;

		case 0x72747441:		// extracted VBA source code - "Attribute VB"
			if (((DWORD FAR*)(stHeader+ 4))[0]==0x74756269 &&
				((DWORD FAR*)(stHeader+ 8))[0]==0x42562065)
				goto Ret2;
			break;

		case 0x7263535B:		// [ScriptFile]
		case 0x7263735B:
			if ( (((DWORD FAR*)(stHeader+ 4))[0] | 0x20202020) == 0x66747069 )
				goto Ret2;
			break;

		case 0x0424748B:		// decrypted Hybris plugin
		case 0x04247C8B:
			goto Ret2;
	}

	// is PIF file? -----------------------------------------------------

	if (stHeader[0]!=0x00 && stHeader[0]!=0x20)	goto NotPIFFile; // no idea why these values...
	if (stHeader[1]<0x38 || stHeader[1]>0xE0)	goto NotPIFFile; // no idea why these values...

	switch ( ((WORD FAR*)(stHeader+0x20))[0] )	{
		case 0x0080:
		case 0x0200: case 0x0280:	break;
		default:					goto NotPIFFile;
	}
	switch ( ((WORD FAR*)(stHeader+0x22))[0] )	{
		case 0x0000: case 0x0080:
		case 0x0100: case 0x2020: 	break;
		default:					goto NotPIFFile;
	}

	for (i=2;i<0x20;i++)	{
		if (stHeader[i]<0x20 || stHeader[i]>'~')	goto NotPIFFile;
	}
	for (i=0x24;i<0x40;i++)	{
		if (stHeader[i]==0)		continue;
		if (stHeader[i]<0x20 || stHeader[i]>'~')	goto NotPIFFile;
	}
	// Printf("looks like PIF file");
	goto Ret2;


NotPIFFile:

	// is DOS COM program? ----------------------------------------------

	// check if that is RIFF (Corel) - no need to scan them
	if ( ((DWORD*)(stHeader))[0]==0x46464952 && stHeader[7]==0 )	goto Not_DOS_COM_file;

	#define	d0	((BYTE)w0)
	#define	d1	(((BYTE*)(&w0))[1])
	#define	WAS_PUSH	0x0001

	ptr=flags=0;	goto DOS_Loop;
	Opc4:	ptr++;
	Opc3:	ptr++;
	Opc2:	ptr++;
	Opc1:	ptr++;
DOS_Loop:
	if (ptr>0x30)	goto Ret2;

	w0=((WORD FAR*)(stHeader+ptr))[0];	// Printf("DOS code: %04X: %02X %02X",ptr,d0,d1);

	switch (d0&0xF0)	{
		case 0x40:			goto Opc1;
		case 0x70:
			if (d1<0x80 || d1>0xE0)	goto Opc2;	// JMP short
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
			d1=((BYTE FAR*)(stHeader+ptr+1))[0];
			if (d1<0xA0 || d1>=0xF0)	goto Ret2;	// JMP or CALL
			d1=((BYTE FAR*)(stHeader+ptr+2))[0];
			if (d1<0xA0 || d1>=0xF0)	goto Ret2;	// JMP or CALL
			break;

		case 0xEB:
			if (d1<0x80 || d1>0xE0)		goto Ret2;	// JMP short
			break;

		case 0xC3: case 0xCB:
			if ((flags&WAS_PUSH) && ptr>2)	goto Ret2;	// RET
			break;

		case 0xCD:
			if (d1!=0 && d1<0x40)			goto Ret2;
			break;
		case 0x9A: case 0xEA:
			if (ptr>5)			goto Ret2;	// CALL/JMP FAR
			break;
		case 0xFF:
			if (d1>=0xC0 && ptr>2)	{	// must be init
				switch (d1&0x38)	{
					case 0x10: case 0x20:	goto Ret2;	// CALL/JMP reg
					default: 		break;
				}
			}

			switch (d1)	{
				case 0x16: case 0x1E:			// CALL data
				case 0x26: case 0x2E:	goto Ret2;	// JMP data
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
                        goto Ret2;
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
		switch (stHeader[i])	{
			case 0x27:		// VBS comment?
				if (i==0)	flags+=2;
				if (i>0)	{	// if there was new line
						if (stHeader[i-1]==0x0A)	flags+=2;
				}
				break;

			case 7: case 9: case 0x0D: case 0x0A: break;
			default:
				if (flags>=4 && flags>=ptr)	goto Ret2;	// there are short file with garbage in Header...
				if (stHeader[i]<0x20 || stHeader[i]>0xF8)	ptr++;
				if (ptr>10)	goto NotScriptFile;
		}

		switch ( ((DWORD FAR*)(stHeader+i))[0] | 0x20202020 )	{
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
						if (stHeader[i-1]=='@')	flags+=2;
					}
	   				// Printf("DOS %04X %c%c%c%c Value=%d Errors=%d",(WORD)i,stHeader[i],stHeader[i+1],stHeader[i+2],stHeader[i+3],flags,ptr);
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
						if (stHeader[i-1]=='<')	flags+=2;
					}
	   				// Printf("HTML %04X %c%c%c%c Value=%d Errors=%d",(WORD)i,stHeader[i],stHeader[i+1],stHeader[i+2],stHeader[i+3],flags,ptr);
					break;
		// [script]
			case 0x69726373:	// scri
					if (i>0 && i<0x38)	{
						if ((((WORD FAR*)(stHeader+i+4))[0]|0x2020)==0x7470 &&
							( (stHeader[i-1]=='[' && stHeader[i+6]==']') ||
							  (stHeader[i-1]=='<') ) )
								goto Ret2;
					}
					break;
		// [Levels]
			case 0x6576656C:	// scri
					if (i>0 && i<0x38)	{
						if ((((WORD FAR*)(stHeader+i+4))[0]|0x2020)==0x736C &&
							(stHeader[i-1]=='[' && stHeader[i+6]==']') )
								goto Ret2;
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
						if ( (((DWORD FAR*)(stHeader+4))[0]|0x20202020)==0x726F7272)
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
							if (stHeader[i-1]==0x0A)	flags+=2;
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
	if (flags>=4 && flags>=ptr)	goto Ret2;	// at least one "good" token and not many errors

NotScriptFile:
	// Printf("NOT script file: flags=%d ptr=%d",flags,ptr);


       	// is random Hybris plugin? -------------------------------------------------

       	if (stHeader[0]>0x80 || stHeader[1]>0x80 || stHeader[2]>0x80 || stHeader[3]>0x80) {

	       	Coinstances=0;
		for (ptr=0;ptr<0x3F;ptr++)	{	// calc num of bytes that are equal
			if (Coinstances>=12)	break;
		for (i=ptr+1;i<0x40;i++)	{
			if (stHeader[ptr]==stHeader[i])	{
				Coinstances++;
				break;
			}
		}}

		// Printf("%d", Coinstances);
		if (Coinstances<12)		goto Ret2;	// data seem to be random
	}

        // Printf("NOT rnd file (hybris)");


Ret:	if (Ret_Code==0)	// Printf("It is not program");	else 	Printf("It is a program");
	return Ret_Code;

Ret2:	// Printf("It is a program");
	return Ret_Code|2;
}			   // --- finished


