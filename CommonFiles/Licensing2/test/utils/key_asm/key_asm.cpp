// key_asm.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <fstream>
#include "KeyInfoAssembler.h"
#include <property/property.h>
#include <Serialize/KLDTSer.h>
#include <_avpio.h>

using namespace std;

typedef std::basic_string<_TCHAR> tstring;

////////////////////////////////////////////////////////////////////////////
void print_help()
{
	cout << _T("Licence key assemble/disassemble utility.") << endl;
	cout << _T("Usage:") << endl
		 << _T("\t1) key_asm.exe /a[ssemble] <xml input file> <key output file>") << endl
		 << _T("\t2) key_asm.exe /d[isassemble] <key input file> <xml output file>") << endl
		 << _T("\t3) key_asm.exe /generate_xml_template <output file>") << endl
		 << _T("\t4) key_asm.exe /generate_stupid_key <output file>") << endl
		 << _T("\t5) key_asm.exe /help") << endl
		 << endl;	
}

////////////////////////////////////////////////////////////////////////////
void assemble(const _TCHAR* szFileIn, const _TCHAR* szFileOut)
{
	KeyRoot key_root;
	KeyInfoAssembler::ReadFromXmlFile(szFileIn, key_root);
	KeyInfoAssembler::WriteToKeyFile(szFileOut, key_root);
}

////////////////////////////////////////////////////////////////////////////
void disassemble(const _TCHAR* szFileIn, const _TCHAR* szFileOut)
{
	KeyRoot key_root;
	KeyInfoAssembler::ReadFromKeyFile(szFileIn, key_root);
	KeyInfoAssembler::WriteToXmlFile(szFileOut, key_root);
}

////////////////////////////////////////////////////////////////////////////
void generate_xml_template(const _TCHAR* szFileName)
{
	KeyRoot key_root;
	key_root.InitializeTemplate();
	
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMDocumentPtr spXmlDoc;
	hr = spXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument/*FreeThreadedDOMDocument40*/));
	if (FAILED(hr))
		throw std::runtime_error("Can't create MSXML.Document object.");

	spXmlDoc->appendChild(key_root.SerializeToXmlNode(spXmlDoc));
	spXmlDoc->save(_bstr_t(szFileName));
}

////////////////////////////////////////////////////////////////////////////
void generate_stupid_key(const _TCHAR* szFileName)
{
	KeyRoot key_root;
	key_root.InitializeTemplate();
	KeyInfoAssembler::WriteToKeyFile(szFileName, key_root);
}

////////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	int result = 0;

	::CoInitialize(NULL);

    AvpCloseHandle = CloseHandle;
    AvpWriteFile   = WriteFile;
    AvpCreateFile  = CreateFile;
    AvpGetFileSize = GetFileSize;
    AvpReadFile    = ReadFile;

	::DATA_Init_Library(malloc, free);
	::KLDT_Init_Library(malloc, free);
	
	try
	{
		bool bParseOk = false;

		if (argc > 1)	
		{
			tstring sCommand(argv[1]);
			if (sCommand[0] == _T('/') || sCommand[0] == _T('-'))
			{
				tstring sCommandName(sCommand.substr(1));
				if (argc > 3 && (sCommandName == _T("a") || sCommandName == _T("assemble")))
				{
					bParseOk = true;
					assemble(argv[2], argv[3]);
				}
				else if (argc > 3 && (sCommandName == _T("d") || sCommandName == _T("disassemble")))
				{
					bParseOk = true;
					disassemble(argv[2], argv[3]);
				}
				else if (argc > 2 && sCommandName == _T("generate_xml_template"))
				{
					bParseOk = true;
					generate_xml_template(argv[2]);
				}
				else if (argc > 2 && sCommandName == _T("generate_stupid_key"))
				{
					bParseOk = true;
					generate_stupid_key(argv[2]);
				}
				else if (sCommandName == _T("?") || sCommandName == _T("help"))
				{
					bParseOk = true;
					print_help();
				}
			}
		}
		else
		{
			bParseOk = true;
			print_help();
		}
		
		if (!bParseOk)
		{
			cout << _T("Error in parameter.") << endl << endl;
			print_help();
			result = 1;
		}
	}
	catch (_com_error& err)
	{
		std::cout << _T("ERROR: ") << err.ErrorMessage() << std::endl;
		result = 1;
	}
	
	::KLDT_Deinit_Library(FALSE);
	::DATA_Deinit_Library();
	
	::CoUninitialize();

	return result;
}
 