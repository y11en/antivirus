

#include <string.h>
#include <stdio.h>
#include <prague.h>
#include <pr_remote.h>
#include "../../pr_manager.h"
#include <iface/i_buffer.h>

//PRRemoteManager* g_manager=NULL;


int main(int argc, char** argv)
{
	tCHAR strModule [] = "pxstub.ppl";
	tERROR error;
	if(PR_FAIL(error = PRInitialize(PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH, 0))){
		printf("PRInitialize failed: %08X\n", error);
		return -1;
	}
	if(argc == 1){
		hROOT aRoot;
		if(PR_FAIL(error = PRGetRoot(&aRoot))){
			printf("PRGetRoot failed: %08X\n", error);
			PRDeinitialize();
			return -1;
		}
		hSTRING hString;
		if(PR_FAIL(error = CALL_SYS_ObjectCreateQuick((hOBJECT)aRoot,(hOBJECT*)&hString, IID_STRING, PID_ANY, 0))){
			printf("CALL_SYS_ObjectCreate failed: %08X\n", error);
			PRDeinitialize();
			return -1;      
		}
		if(PR_FAIL(error = PRRegisterObject("String",(hOBJECT)hString))){
			printf("PRRegisterObject failed: %08X\n", error);
			PRDeinitialize();
			return -1;
		}
		char TestMessage [] = "Test message URAAAAAAAAAAAAA!";
		if(PR_FAIL(error = CALL_String_ImportFromBuff(hString, 0, TestMessage, sizeof(TestMessage), cCP_ANSI, cSTRING_Z))){
			printf("CALL_String_ImportFromBuff failed: %08X\n", error);
			return -1;
		}

		getchar();
		PRUnregisterObject((hOBJECT)hString);
	}
	else {
		hROOT aRoot;
		if(PR_FAIL(error = PRGetRoot(&aRoot))){
			printf("PRGetRoot failed: %08X	\n", error);
			PRDeinitialize();
			return -1;
		}
		
		
		if(PR_FAIL(error = CALL_Root_LoadModule(aRoot, NULL, strModule, strlen(strModule), cCP_ANSI)))
			printf("Root_LoadModule failed: %08X\n", error);
		else {
			hSTRING hString;
			
			if(PR_FAIL(error = PRGetObjectProxy(PR_PROCESS_ANY, "String",(hOBJECT*)&hString)))
				printf("PRGetObjectProxy failed: %08X\n", error);
			else {
				char buf[100] = {0};    
				if(PR_FAIL(error = CALL_String_ExportToBuff(hString, 0, STR_RANGE(0, cSTRING_WHOLE_LENGTH), buf, sizeof(buf), cCP_ANSI, cSTRING_Z)))
					printf("CALL_String_ExportToBuff failed: %08X\n", error);
				else 
					printf("Buf read: %s\n", buf);
			}
			PRReleaseObjectProxy((hOBJECT)hString);
		}	
	}
	PRDeinitialize();
	return 0;
}
