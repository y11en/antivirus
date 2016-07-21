/*
 *  i386disasm.cpp,v 1.2 2002/10/17 09:55:02 tim 
 */

#include "typedefs.h"

I386Disasm pDisasm;


I386Disasm::I386Disasm()
{
	fEnableDisassembler = false;
	Cleanup();
}


void I386Disasm::Cleanup()
{
	m_sGetReg8[0] = '\0';
	m_sGetReg16[0] = '\0';
	m_sGetReg32[0] = '\0';
	m_sGetRegLast[0] = '\0';
	m_sGetSegReg[0] = '\0';
	m_bReadByte = 0;
	m_wReadWord = 0;
	m_dwReadDWord = 0;
	m_dwReadLast = 0;
	m_sGetModRmRmX[0] = '\0';
	m_sGetModRmRegX[0] = '\0';
    m_lastSym = LAST_IS_BYTE;
    fEnableReadData = true;
}


void I386Disasm::SetParam (disasm_t param, const char * fmt, ...)
{
	va_list va_alist;
	static char buf[MAX_DISASM_STRING];

	va_start (va_alist, fmt);
	vsnprintf (buf, sizeof(buf), fmt, va_alist);
	va_end (va_alist);

	buf[MAX_DISASM_STRING-1] = '\0';

	switch (param)
	{
	case DISASM_GETREG8:
		strcpy (m_sGetReg8, buf);
		strcpy (m_sGetRegLast, buf);
		break;

	case DISASM_GETREG16:
		strcpy (m_sGetReg16, buf);
		strcpy (m_sGetRegLast, buf);
		break;

	case DISASM_GETREG32:
		strcpy (m_sGetReg32, buf);
		strcpy (m_sGetRegLast, buf);
		break;

	case DISASM_GETSEGREG:
		strcpy (m_sGetSegReg, buf);
		break;

	case DISASM_GETMODRMRMX:
		strcpy (m_sGetModRmRmX, buf);
		break;

	case DISASM_GETMODRMREGX:
		strcpy (m_sGetModRmRegX, buf);
		break;

	default:
		break;
	}
}


const char * I386Disasm::GetCondName (condition_t cond)
{
	const char * name = "???";

	switch (cond)
	{
	case COND_ALWAYS:
		name = "mp";
		break;

	case COND_A_NBE:
		name = "a";
		break;
		
	case COND_AE_NB:
		name = "ae";
		break;

	case COND_B_NAE:
		name = "b";
		break;

	case COND_BE_NA:
		name = "be";
		break;

	case COND_C:
		name = "c";
		break;

	case COND_E_Z:
		name = "z";
		break;

	case COND_G_NLE:
		name = "g";
		break;

	case COND_GE_NL:
		name = "ge";
		break;

	case COND_L_NGE:
		name = "l";
		break;

	case COND_LE_NG:
		name = "le";
		break;

	case COND_NC:
		name = "nc";
		break;

	case COND_NE_NZ:
		name = "nz";
		break;

	case COND_NO:
		name = "no";
		break;

	case COND_NP_PO:
		name = "np";
		break;

	case COND_NS:
		name = "ns";
		break;

	case COND_O:
		name = "o";
		break;

	case COND_P_PE:
		name = "p";
		break;

	case COND_S:
		name = "s";
		break;
	}

	return name;
}


void I386Disasm::SetParam (disasm_t param, DWORD value)
{
	if ( !fEnableReadData )
		return;

	switch (param)
	{
	case DISASM_READIX86BYTE:
		m_bReadByte = (BYTE) value;
        m_dwReadLast = value;
        m_lastSym = LAST_IS_BYTE;
		break;

	case DISASM_READIX86WORD:
		m_wReadWord = (WORD) value;
        m_dwReadLast = value;
        m_lastSym = LAST_IS_WORD;
		break;

	case DISASM_READIX86DWORD:
		m_dwReadDWord = (DWORD) value;
        m_dwReadLast = value;
        m_lastSym = LAST_IS_DWORD;
		break;

	default:
		break;
	}
}


void I386Disasm::Disasm (const char * str, ...)
{
	char specdata[512];
	va_list marker;
	int bufptr = 0, specptr = 0;
	bool fSpecChar = false, fSigned = false;


	va_start (marker, str);
	memset (m_sDisasm, 0, sizeof(m_sDisasm));

	for ( ; *str; str++ )
	{
		if ( fSpecChar )
		{
			// negative sign
			if ( *str == '-' )
			{
				fSigned = true;
				continue;
			}

			specdata[specptr++] = *str;
			specdata[specptr] = '\0';

			// % sign
			if ( specptr == 0 && *str == '%' )
			{
				fSpecChar = false;
				m_sDisasm[bufptr++] = *str;
			}
			else if ( !strcmp(specdata, "imm8") )
			{
				fSpecChar = false;
				sprintf_hex (m_sDisasm, fSigned, LAST_IS_BYTE, LittleEndianToMachine ((int) va_arg (marker, DWORD)));
			}
			else if ( !strcmp(specdata, "imm16") )
			{
				fSpecChar = false;
				sprintf_hex (m_sDisasm, fSigned, LAST_IS_WORD, LittleEndianToMachine (va_arg (marker, DWORD)));
			}
			else if ( !strcmp(specdata, "imm32") )
			{
				fSpecChar = false;
				sprintf_hex (m_sDisasm, fSigned, LAST_IS_DWORD, LittleEndianToMachine (va_arg (marker, DWORD)));
			}
			else if ( !strcmp(specdata, "cond") )
			{
				fSpecChar = false;
				strcat (m_sDisasm, GetCondName((condition_t) va_arg (marker, DWORD)) );
			}
			else if ( !strcmp(specdata, "rep") )
			{
				if ( IS_SET (va_arg (marker, DWORD), PREFIX_REPE) )
					strcat (m_sDisasm, "rep" );

				fSpecChar = false;
			}
			else if ( !strcmp(specdata, "rpe") )
			{
				DWORD flag = va_arg (marker, DWORD);
				if ( IS_SET(flag, PREFIX_REPE) )
					strcat (m_sDisasm, "repe" );
				else if ( IS_SET(flag, PREFIX_REPNE) )
					strcat (m_sDisasm, "repne" );

				fSpecChar = false;
			}
			else if ( !strcmp(specdata, "r8") )
			{
				fSpecChar = false;
				strcat (m_sDisasm, m_sGetReg8);
			}
			else if ( !strcmp(specdata, "r16") )
			{
				fSpecChar = false;
				strcat (m_sDisasm, m_sGetReg16);
			}
			else if ( !strcmp(specdata, "r32") )
			{
				fSpecChar = false;
				strcat (m_sDisasm, m_sGetReg32);
			}
			else if ( !strcmp(specdata, "reg") )
			{
				fSpecChar = false;
				strcat (m_sDisasm, m_sGetRegLast);
			}
			else if ( !strcmp(specdata, "lb") )
			{
				fSpecChar = false;
				sprintf_hex (m_sDisasm, fSigned, LAST_IS_BYTE, m_bReadByte);
			}
			else if ( !strcmp(specdata, "lw") )
			{
				fSpecChar = false;
				sprintf_hex (m_sDisasm, fSigned, LAST_IS_WORD, m_wReadWord);
			}
			else if ( !strcmp(specdata, "ld") )
			{
				fSpecChar = false;
				sprintf_hex (m_sDisasm, fSigned, LAST_IS_DWORD, m_dwReadDWord);
			}
			else if ( !strcmp(specdata, "lr") )
			{
				fSpecChar = false;
				sprintf_hex (m_sDisasm, fSigned, m_lastSym, m_dwReadLast);
			}
			else if ( !strcmp(specdata, "sreg") )
			{
				fSpecChar = false;
				strcat (m_sDisasm, m_sGetSegReg);
			}
			else if ( !strcmp(specdata, "mrm") )
			{
				fSpecChar = false;
				strcat (m_sDisasm, m_sGetModRmRmX);
			}
			else if ( !strcmp(specdata, "mreg") )
			{
				fSpecChar = false;
				strcat (m_sDisasm, m_sGetModRmRegX);
			}

			if ( !fSpecChar )
			{
				fSigned = false;
	            fSpecChar = false;
            	bufptr = strlen(m_sDisasm);
			}

            continue;
		}

        if ( *str == '%' )
		{
			fSpecChar = true;
            specptr = 0;
			continue;
		}

		// Normal character - just store
		m_sDisasm[bufptr++] = *str;
		m_sDisasm[bufptr] = '\0';
	}

	va_end (marker);
}


void I386Disasm::sprintf_hex (char * buf, bool fSigned, lastread_t type, DWORD value)
{
	char tmpbuf[32];

    tmpbuf[0] = '\0';

	switch (type)
	{
	case LAST_IS_BYTE:
		if ( fSigned )
		{
			BYTE val = (BYTE) value;
			sprintf(tmpbuf, "%c0x%02X", val > 0x80 ? '-' : '+', val > 0x80 ? 0 - val : val);
		}
		else
			sprintf(tmpbuf, "0x%02X", value);

        break;

	case LAST_IS_WORD:
		if ( fSigned )
		{
			WORD val = (WORD) value;
			sprintf(tmpbuf, "%c0x%04X", val > 0x8000 ? '-' : '+', val > 0x8000 ? 0 - val : val);
		}
		else
			sprintf(tmpbuf, "0x%04X", value);

		break;

	case LAST_IS_DWORD:
		if ( fSigned )
		{
			DWORD val = (DWORD) value;
			sprintf(tmpbuf, "%c0x%08X", val > 0x80000000 ? '-' : '+', val > 0x80000000 ? 0 - val : val);
		}
		else
			sprintf(tmpbuf, "0x%08X", value);

		break;
	}

	if ( tmpbuf[0] != '\0' )
		strcat (buf, tmpbuf);
}


void I386Disasm::EnableReadData (bool fEnable)
{
	fEnableReadData = fEnable;
}


const char * I386Disasm::GetDisasm()
{
	return m_sDisasm;
}


void I386Disasm::EnableDisasm (bool fEnable)
{
	fEnableDisassembler = fEnable;
}


static const char * LOGFILE = "_disasm.log";

void I386Disasm::Log (const char * fmt, ...)
{
	static bool bWarnLogError = false;
	
	if ( !fEnableDisassembler )
		return;

	va_list va_alist;
	char buf[512];
	FILE * fp;

	va_start (va_alist, fmt);
	vsnprintf (buf, sizeof(buf), fmt, va_alist);
	va_end (va_alist);

	buf[sizeof(buf)-1] = '\0';

	if ( (fp = fopen(LOGFILE, "a")) != 0 )
	{
		fprintf (fp, "%s", buf);
		fclose (fp);
	}
	else
	{
		if ( !bWarnLogError )
		{
			printf ("Couldn't create the log file %s: %s\n", LOGFILE, strerror(errno));
			bWarnLogError = true;
		}
	}
}


void I386Disasm::LogAlways (const char * fmt, ...)
{
	va_list va_alist;
	char buf[512];
	FILE * fp;

	va_start (va_alist, fmt);
	vsnprintf (buf, sizeof(buf), fmt, va_alist);
	va_end (va_alist);

	buf[sizeof(buf)-1] = '\0';

	if ( (fp = fopen(LOGFILE, "a")) != 0 )
	{
		fprintf (fp, "%s", buf);
		fclose (fp);
	}
}
