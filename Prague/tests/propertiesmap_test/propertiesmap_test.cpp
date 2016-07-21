#define PR_IMPEX_DEF

#include <iostream>
#include <iomanip>

#include <prague.h> 
#include <pr_loadr.h>
#include <iface/i_root.h>
#include <iface/i_reg.h>
#include <plugin/p_propertiesmap.h>
#include <pr_cpp.h>

using namespace std;

hROOT g_root = 0;

PR_MAKE_TRACE_FUNC;

void DbgTrace(tSTRING string)
{
        OUTPUT_DEBUG_STRING(string);
        OUTPUT_DEBUG_STRING("\n");
}

int main()
{
	setlocale(LC_ALL, "");
	CPrague aPrague(0);

	if(!g_root){
		cerr << "Can't load Prague" << endl;
		return -1;
	}

	CALL_SYS_PropertySetPtr(g_root, pgOUTPUT_FUNC,(void*)DbgTrace);
	CALL_SYS_TraceLevelSet(g_root, tlsALL_OBJECTS, prtNOTIFY, prtMIN_TRACE_LEVEL);

	tERROR anError = errOK; 
  
	hREGISTRY aReg = 0;
  
	anError = g_root->sysCreateObjectQuick(reinterpret_cast<hOBJECT*>(&aReg), IID_REGISTRY, PID_PROPERTIESMAP, SUBTYPE_ANY);

	tRegKey aKey1, aKey2, aKey3, aKey4, aKey5, aKey6;
	if(PR_SUCC(anError))
		anError = aReg->OpenKey(&aKey1, 0, "Key", cTRUE);

	if(PR_SUCC(anError))
		anError = aReg->OpenKey(&aKey2, aKey1, "SubKey", cTRUE);
  
	if(PR_SUCC(anError))
		anError = aReg->OpenKey(&aKey3, aKey2, "SubSubKey", cTRUE);

	if(PR_SUCC(anError))
		anError = aReg->OpenKey(&aKey4, aKey3, "SubSubSubKey", cTRUE);

	if(PR_SUCC(anError))
		anError = aReg->OpenKey(&aKey5, aKey3, "SubSubSubKey1", cTRUE);

	if(PR_SUCC(anError))
		anError = aReg->OpenKey(&aKey6, aKey3, "SubSubSubKey2", cTRUE);


	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetKeyName(0, aKey4, aBuffer, sizeof(aBuffer), cTRUE);

		if(PR_FAIL(anError)|| strcmp(aBuffer, "Key\\SubKey\\SubSubKey\\SubSubSubKey")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}

	if(PR_SUCC(anError))
		anError = aReg->SetRootKey(aKey3, cTRUE);    

	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetKeyName(0, aKey4, aBuffer, sizeof(aBuffer), cTRUE);

		if(PR_FAIL(anError)|| strcmp(aBuffer, "SubSubKey\\SubSubSubKey")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}


	if(PR_SUCC(anError))
		anError = aReg->SetRootKey(0, cTRUE);    

	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetKeyName(0, aKey4, aBuffer, sizeof(aBuffer), cTRUE);

		if(PR_FAIL(anError)|| strcmp(aBuffer, "Key\\SubKey\\SubSubKey\\SubSubSubKey")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}

	if(PR_SUCC(anError)){
		anError = aReg->SetRootStr("1Key\\SubKey\\", cTRUE);    
    
		if(PR_FAIL(anError))
			anError = errOK;
		else {
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}

	if(PR_SUCC(anError))
		anError = aReg->SetRootStr("Key\\SubKey\\", cTRUE);    
  
	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetKeyName(0, aKey4, aBuffer, sizeof(aBuffer), cTRUE);

		if(PR_FAIL(anError)|| strcmp(aBuffer, "SubKey\\SubSubKey\\SubSubSubKey")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}

	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->get_pgROOT_POINT(aBuffer, sizeof(aBuffer));
    
		if(PR_FAIL(anError)|| strcmp(aBuffer, "Key\\SubKey")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}

	tPTR g_rootKey = 0;
	if(PR_SUCC(anError)){
		g_rootKey = aReg->get_pgROOT_KEY();
		aReg->set_pgROOT_KEY(g_rootKey);
	}


	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetKeyName(0, aKey6, aBuffer, sizeof(aBuffer), cTRUE);

		if(PR_FAIL(anError)|| strcmp(aBuffer, "SubKey\\SubSubKey\\SubSubSubKey2")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}
  
	char g_rootStr [] = "";

	if(PR_SUCC(anError))
		aReg->set_pgROOT_POINT(g_rootStr,  sizeof(g_rootStr));

	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetKeyName(0, aKey6, aBuffer, sizeof(aBuffer), cTRUE);

		if(PR_FAIL(anError)|| strcmp(aBuffer, "Key\\SubKey\\SubSubKey\\SubSubSubKey2")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}

	if(PR_SUCC(anError))
		anError = aReg->OpenKey(&aKey6, aKey3, "SubSubSubKey1", cFALSE);

	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetKeyName(0, aKey6, aBuffer, sizeof(aBuffer), cTRUE);
    
		if(PR_FAIL(anError)|| strcmp(aBuffer, "Key\\SubKey\\SubSubKey\\SubSubSubKey1")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}

	if(PR_SUCC(anError))
		anError = aReg->OpenKeyByIndex(&aKey6, aKey3, 2, cTRUE);

	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetKeyName(0, aKey6, aBuffer, sizeof(aBuffer), cTRUE);
    
		if(PR_FAIL(anError)|| strcmp(aBuffer, "Key\\SubKey\\SubSubKey\\SubSubSubKey2")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}


	if(PR_SUCC(anError))
		anError = aReg->OpenKeyByIndex(&aKey6, aKey3, 1, cTRUE);

	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetKeyName(0, aKey6, aBuffer, sizeof(aBuffer), cFALSE);
    
		if(PR_FAIL(anError)|| strcmp(aBuffer, "SubSubSubKey1")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}

	if(PR_SUCC(anError))
		anError = aReg->OpenNextKey(&aKey6, aKey6, cTRUE);
  
	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetKeyName(0, aKey6, aBuffer, sizeof(aBuffer), cFALSE);
    
		if(PR_FAIL(anError)|| strcmp(aBuffer, "SubSubSubKey2")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}


	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetKeyNameByIndex(0, aKey3, 1, aBuffer, sizeof(aBuffer), cFALSE);
    
		if(PR_FAIL(anError)|| strcmp(aBuffer, "SubSubSubKey1")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}


	if(PR_SUCC(anError))
		anError = aReg->OpenKey(&aKey6, aKey3, "SubSubSubKey2", cFALSE);

	if(PR_SUCC(anError)){
		tTYPE_ID aType =(tTYPE_ID)pTYPE_NOTHING;
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetValue(0, aKey3, "SubSubSubKey1", &aType, aBuffer, sizeof(aBuffer));

		if(PR_FAIL(anError))
			anError = errOK;
		else {
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}

	if(PR_SUCC(anError)){
		tTYPE_ID aType =(tTYPE_ID)pTYPE_NOTHING;
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetValueByIndex(0, aKey3, 1, &aType, aBuffer, sizeof(aBuffer));

		if(PR_FAIL(anError))
			anError = errOK;
		else {
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}

	}

	tCHAR aBuffer [] = "SubSubSubKey1";

	if(PR_SUCC(anError))
		anError = aReg->SetValue(aKey6, "Value1", tid_STRING, aBuffer, sizeof(aBuffer), cTRUE);

	if(PR_SUCC(anError))
		anError = aReg->SetValue(aKey6, "Value2", tid_STRING, aBuffer, sizeof(aBuffer), cTRUE);

	if(PR_SUCC(anError))
		anError = aReg->SetValueByIndex(aKey6, 2, tid_STRING, aBuffer, sizeof(aBuffer), cTRUE);

	if(PR_SUCC(anError)){
		tDWORD aKeyCount = 0;
		anError = aReg->GetKeyCount(&aKeyCount, 0);
		if(PR_FAIL(anError)||(aKeyCount != 6)){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}   
	}

	if(PR_SUCC(anError)){
		tDWORD aValueCount = 0;
		anError = aReg->GetValueCount(&aValueCount, aKey6);
		if(PR_FAIL(anError)||(aValueCount != 3)){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}   
	}
    

	if(PR_SUCC(anError)){
		tTYPE_ID aType =(tTYPE_ID)pTYPE_NOTHING;
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetValue(0, aKey6, "Value1", &aType, aBuffer, sizeof(aBuffer));
    
		if(PR_FAIL(anError)|| strcmp(aBuffer, "SubSubSubKey1")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
    
	}

	if(PR_SUCC(anError)){
		tTYPE_ID aType =(tTYPE_ID)pTYPE_NOTHING;
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetValueByIndex(0, aKey6, 1, &aType, aBuffer, sizeof(aBuffer));
    
		if(PR_FAIL(anError)|| strcmp(aBuffer, "SubSubSubKey1")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}

	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetKeyNameByIndex(0, aKey3, 1, aBuffer, sizeof(aBuffer), cTRUE);
    
		if(PR_FAIL(anError)|| strcmp(aBuffer, "Key\\SubKey\\SubSubKey\\SubSubSubKey1")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
  
	}

	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetValueNameByIndex(0, aKey6, 2, aBuffer, sizeof(aBuffer));

		if(PR_FAIL(anError)|| strcmp(aBuffer, "noname2")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}

	if(PR_SUCC(anError)){
		anError = aReg->DeleteValueByInd(aKey6, 1 );
    
		if(PR_FAIL(anError)){
			cout << "Error has occured. Line " << __LINE__ << endl;
		} 
	}

	if(PR_SUCC(anError)){
		tTYPE_ID aType =(tTYPE_ID)pTYPE_NOTHING;
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetValueByIndex(0, aKey6, 2, &aType, aBuffer, sizeof(aBuffer));

		if(PR_FAIL(anError))
			anError = errOK;
		else {
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}

	if(PR_SUCC(anError)){
		anError = aReg->DeleteValue(aKey6, "Value1");
    
		if(PR_FAIL(anError)){
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}

	if(PR_SUCC(anError)){
		tTYPE_ID aType =(tTYPE_ID)pTYPE_NOTHING;
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetValue(0, aKey6, "Value1", &aType, aBuffer, sizeof(aBuffer));

		if(PR_FAIL(anError))
			anError = errOK;
		else {
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}

	}

	if(PR_SUCC(anError)){
		anError = aReg->DeleteKeyByInd(aKey3, 0);
    
		if(PR_FAIL(anError))
			cout << "Error has occured. Line " << __LINE__ << endl;
    
	}
  
	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetKeyNameByIndex(0, aKey3, 0, aBuffer, sizeof(aBuffer), cTRUE);
    
		if(PR_FAIL(anError)|| strcmp(aBuffer, "Key\\SubKey\\SubSubKey\\SubSubSubKey1")){
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}

	if(PR_SUCC(anError)){
		anError = aReg->DeleteKey(aKey3, "SubSubSubKey2");
    
		if(PR_FAIL(anError))
			cout << "Error has occured. Line " << __LINE__ << endl;
	}

	if(PR_SUCC(anError)){
		tCHAR aBuffer [1024] = {0};
		anError = aReg->GetKeyNameByIndex(0, aKey3, 1, aBuffer, sizeof(aBuffer), cTRUE);

		if(PR_FAIL(anError))
			anError = errOK;
		else {
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}


	if(PR_SUCC(anError))
		anError = aReg->Clean();
  
	if(PR_SUCC(anError)){
		anError = aReg->OpenKey(&aKey1, 0, "Key", cFALSE);
    
		if(PR_FAIL(anError))
			anError = errOK; 
		else {
			anError = errNOT_OK;
			cout << "Error has occured. Line " << __LINE__ << endl;
		}
	}

	aReg->sysCloseObject();
	aReg = 0;

	anError = g_root->sysCreateObjectQuick(reinterpret_cast<hOBJECT*>(&aReg), IID_REGISTRY, PID_PROPERTIESMAP, SUBTYPE_ANY);

	if(PR_SUCC(anError))
		anError = aReg->set_pgSAVE_RESULTS_ON_CLOSE(cTRUE);
  
	if(PR_SUCC(anError)){
		tCHAR aFileName [] = "propertiesmap_test.xml";
		anError = aReg->propSetStr(0 , pgOBJECT_NAME, aFileName, 0, cCP_ANSI);
	}


	if(PR_SUCC(anError))
		anError = aReg->OpenKey(&aKey1, 0, "RootKey", cTRUE);

	if(PR_SUCC(anError))
		anError = aReg->OpenKey(&aKey2, aKey1, "SignedValues", cTRUE);
  
	if(PR_SUCC(anError))
		anError = aReg->OpenKey(&aKey3, aKey1, "UnsignedValues", cTRUE);

	if(PR_SUCC(anError))
		anError = aReg->OpenKey(&aKey4, aKey1, "OtherValues", cTRUE);

	if(PR_SUCC(anError))
		anError = aReg->OpenKey(&aKey5, aKey1, "Convertation", cTRUE);


	if(PR_SUCC(anError)){
		tCHAR aBuffer = 'A';
		anError = aReg->SetValue(aKey2, "tCHAR", tid_CHAR, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tSBYTE aBuffer = -100;
		anError = aReg->SetValue(aKey2, "tSBYTE", tid_SBYTE, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tSHORT aBuffer = -30125;
		anError = aReg->SetValue(aKey2, "tSHORT", tid_SHORT, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tINT aBuffer = -345500;
		anError = aReg->SetValue(aKey2, "tINT", tid_INT, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tLONG aBuffer = -34555500;
		anError = aReg->SetValue(aKey2, "tLONG", tid_LONG, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		long long int aBuffer = LONG_LONG_CONST(-744674407370955161);
		anError = aReg->SetValue(aKey2, "tLONGLONG", tid_LONGLONG, &aBuffer, sizeof(aBuffer), cTRUE);
	}
  
	if(PR_SUCC(anError)){
		tBYTE aBuffer = 10;
		anError = aReg->SetValue(aKey3, "tBYTE", tid_BYTE, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tWORD aBuffer = 500;
		anError = aReg->SetValue(aKey3, "tWORD", tid_WORD, &aBuffer, sizeof(aBuffer), cTRUE);
	}


	if(PR_SUCC(anError)){
		tDWORD aBuffer = 12500;
		anError = aReg->SetValue(aKey3, "tDWORD", tid_DWORD, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tUINT aBuffer = 345500;
		anError = aReg->SetValue(aKey3, "tUINT", tid_UINT, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		unsigned long long int aBuffer = LONG_LONG_CONST(1844674407370955161);
		anError = aReg->SetValue(aKey3, "tQWORD", tid_QWORD, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tBOOL aBuffer = cFALSE;
		anError = aReg->SetValue(aKey4, "tBOOL", tid_BOOL, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tBOOL aBuffer = cTRUE;
		anError = aReg->SetValue(aKey4, "tBOOL2", tid_BOOL, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tCHAR aBuffer [] = "utf8 1 строка";
		anError = aReg->SetValue(aKey4, "tSTRING", tid_STRING, aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		cStringObj strobj (L"utf8 строка");
		anError = aReg->SetValue(aKey4, "tSTRING", tid_STRING_OBJ, &strobj, sizeof(strobj), cFALSE);
	}

	if(PR_SUCC(anError)){
		tCHAR aBuffer [sizeof("utf8 строка")] = {0} ;
		tTYPE_ID tid = tid_STRING;
		anError = aReg->GetValue(0, aKey4, "tSTRING", &tid, aBuffer, sizeof(aBuffer));
		if(PR_SUCC(anError) && strcmp(aBuffer, "utf8 строка"))
			anError = errNOT_OK;
	}

	if(PR_SUCC(anError)){
		tWCHAR aBuffer [sizeof(L"utf8 строка")] = {0} ;
		tTYPE_ID tid = tid_WSTRING;
		anError = aReg->GetValue(0, aKey4, "tSTRING", &tid, aBuffer, sizeof(aBuffer));
		if(PR_SUCC(anError) && wcscmp(aBuffer, L"utf8 строка"))
			anError = errNOT_OK;
	}

	if(PR_SUCC(anError)){
		cStringObj strobj;
		tTYPE_ID tid = tid_STRING_OBJ;
		anError = aReg->GetValue(0, aKey4, "tSTRING", &tid, &strobj, sizeof(aBuffer));
		if(PR_SUCC(anError) && wcscmp(strobj.data(), L"utf8 строка"))
			anError = errNOT_OK;
	}

	if(PR_SUCC(anError)){
		tWCHAR aBuffer [] = L"юникодная 1 строка";
		anError = aReg->SetValue(aKey4, "tWSTRING", tid_WSTRING, aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		cStringObj strobj (L"юникодная строка");
		anError = aReg->SetValue(aKey4, "tWSTRING", tid_STRING_OBJ, &strobj, sizeof(strobj), cFALSE);
	}

	if(PR_SUCC(anError)){
		tWCHAR aBuffer [sizeof(L"юникодная строка")] = {0} ;
		tTYPE_ID tid = tid_WSTRING;
		anError = aReg->GetValue(0, aKey4, "tWSTRING", &tid, aBuffer, sizeof(aBuffer));
		if(PR_SUCC(anError) && wcscmp(aBuffer, L"юникодная строка"))
			anError = errNOT_OK;
	}

	if(PR_SUCC(anError)){
		tCHAR aBuffer [sizeof("юникодная строка")] = {0} ;
		tTYPE_ID tid = tid_STRING;
		anError = aReg->GetValue(0, aKey4, "tWSTRING", &tid, aBuffer, sizeof(aBuffer));
		if(PR_SUCC(anError) && strcmp(aBuffer, "юникодная строка"))
			anError = errNOT_OK;
	}

	if(PR_SUCC(anError)){
		cStringObj strobj;
		tTYPE_ID tid = tid_STRING_OBJ;
		anError = aReg->GetValue(0, aKey4, "tWSTRING", &tid, &strobj, sizeof(aBuffer));
		if(PR_SUCC(anError) && wcscmp(strobj.data(), L"юникодная строка"))
			anError = errNOT_OK;
	}

	if(PR_SUCC(anError)){
		tDOUBLE aBuffer = -344.5655550;
		anError = aReg->SetValue(aKey4, "tDOUBLE", tid_DOUBLE, &aBuffer, sizeof(aBuffer), cTRUE);
	}


	if(PR_SUCC(anError)){
		tCHAR aBuffer [] = {0x10, 0x11, 0x12, 0x13, 0xa, 0xb, 0xff} ;
		anError = aReg->SetValue(aKey4, "tBINARY", tid_BINARY, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tBOOL aBuffer = cFALSE;
		anError = aReg->SetValue(aKey5, "tBOOL<-tDWORD = 1", tid_BOOL, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tDWORD aBuffer = 10;
		anError = aReg->SetValue(aKey5, "tBOOL<-tDWORD = 1", tid_DWORD, &aBuffer, sizeof(aBuffer), cTRUE);
	}


	if(PR_SUCC(anError)){
		tDWORD aBuffer = 10000;
		anError = aReg->SetValue(aKey5, "tDWORD<-tBOOL = 0", tid_DWORD, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tBOOL aBuffer = cFALSE;
		anError = aReg->SetValue(aKey5, "tDWORD<-tBOOL = 0", tid_BOOL, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tCHAR aBuffer [] = "wrong value";
		anError = aReg->SetValue(aKey5, "tSTRING<-tDWORD = 1234567", tid_STRING, &aBuffer, sizeof(aBuffer), cTRUE);

	}

	if(PR_SUCC(anError)){
		tDWORD aBuffer = 1234567;
		anError = aReg->SetValue(aKey5, "tSTRING<-tDWORD = 1234567", tid_DWORD, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tCHAR aBuffer [] = "wrong value";
		anError = aReg->SetValue(aKey5, "tSTRING<-tQWORD = 1234567890", tid_STRING, &aBuffer, sizeof(aBuffer), cTRUE);

	}

	if(PR_SUCC(anError)){
		tQWORD aBuffer = 1234567890;
		anError = aReg->SetValue(aKey5, "tSTRING<-tQWORD = 1234567890", tid_QWORD, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tDWORD aBuffer = 1234567;
		anError = aReg->SetValue(aKey5, "tDWORD<-tSTRING = 1234567", tid_DWORD, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tCHAR aBuffer [] = "wrong value";
		anError = aReg->SetValue(aKey5, "tDWORD<-tSTRING = 1234567", tid_STRING, &aBuffer, sizeof(aBuffer), cTRUE);
		if(PR_SUCC(anError))
			cout << "Error has occured. Line " << __LINE__ << endl;
		anError = errOK;
	}

	if(PR_SUCC(anError)){
		tQWORD aBuffer = 123456789;
		anError = aReg->SetValue(aKey5, "tQWORD<-tSTRING = 987654321", tid_QWORD, &aBuffer, sizeof(aBuffer), cTRUE);
	}

	if(PR_SUCC(anError)){
		tCHAR aBuffer [] = "987654321";
		anError = aReg->SetValue(aKey5, "tQWORD<-tSTRING = 987654321", tid_STRING, &aBuffer, sizeof(aBuffer), cTRUE);

	}

	if(aReg)
		aReg->sysCloseObject();
	aReg = 0;  

	if(PR_SUCC(anError))
		anError = g_root->sysCreateObject(reinterpret_cast<hOBJECT*>(&aReg), IID_REGISTRY, PID_PROPERTIESMAP, SUBTYPE_ANY);

	if(PR_SUCC(anError))
		anError = aReg->set_pgSAVE_RESULTS_ON_CLOSE(cTRUE);
  
	if(PR_SUCC(anError)){
		tCHAR aFileName [] = "propertiesmap_test.xml";
		anError = aReg->propSetStr(0 , pgOBJECT_NAME, aFileName, 0, cCP_ANSI);
	}
  
	if(PR_SUCC(anError))
		anError = aReg->sysCreateObjectDone();
  
  
	if(PR_SUCC(anError)){
		tCHAR aFileName [] = "propertiesmap_test2.xml";
		anError = aReg->propSetStr(0 , pgOBJECT_NAME, aFileName, 0, cCP_ANSI);
	}
  
	
	if(aReg)
		aReg->sysCloseObject();
	aReg = 0;  
  
	cout << "************************************************" << endl;
	if(PR_SUCC(anError)){
		cout << "********** PropertiesMap tests passed **********" << endl;
		cout << "************************************************" << endl;
		cout << endl;
		cout << "Please, show differences between resulting files propertiesmap_test.xml and propertiesmap_test2.xml. They must be the same." << endl;
	}
	else {
		cout << "********** PropertiesMap tests failed **********" << endl;
		cout << "************************************************" << endl;
	}


 
	return 0;
}


