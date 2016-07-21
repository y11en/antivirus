#define _USE_VTBL

#include <iostream>

#include <prague.h>
#include <pr_loadr.h>

#include "sample.h"
#include "sampleimpl.h"
#include "sampleplugin.h"

hROOT g_root = NULL;

int main() {
	CPrague prague;
	
	tERROR err = prague.Init(NULL, PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH|PR_LOADER_FLAG_FORCE_PLUGIN_LOAD_ALL);
	
	if (!PR_SUCC(err)) {
		std::cerr << "can't init prague" << std::endl;
		return 1;
	}

	hSampleI sample = NULL;

	err = CALL_SYS_ObjectCreateQuick(g_root, (hOBJECT*)&sample, IID_SampleI, PLUGINID_SamplePlugin, IMPLID_SampleImpl);
			
	if (!PR_SUCC(err)) {
		std::cerr << "can't create sample interface: " << std::hex << err << std::endl;
		return 1;
	}

	if (PR_FAIL(CALL_SampleI_baseMethod(sample,0))) {
		std::cerr << "unexpected return value" << std::endl;
	}
	
	err = CALL_SYS_PropertySet(sample, NULL, SampleI_IProp, "My Value", sizeof("My Value"));

	if (PR_FAIL(err)) {
		std::cerr << "can't set prop SampleI_IProp, err: " << std::hex << err << std::endl;
	}

	char buffer[1024] = "no value";
	
	err = CALL_SYS_PropertyGet(sample, NULL, SampleI_IProp, buffer, sizeof(buffer));

	if (PR_FAIL(err)) {
		std::cerr << "can't get prop SampleI_IProp, err: " << std::hex << err << std::endl;
		return 1;
	} else {
		std::cout << "value: " << buffer << std::endl;
	}

	tDATA value = 1234;
	err = CALL_SYS_PropertySet(sample, NULL, Base_BaseProp, &value, sizeof(value));

	if (PR_FAIL(err)) {
		std::cerr << "can't set prop Base_BaseProp, err: " << std::hex << err << std::endl;
	}


	value = 0;	
	err = CALL_SYS_PropertyGet(sample, NULL, Base_BaseProp, &value, sizeof(value));

	if (PR_FAIL(err)) {
		std::cerr << "can't get prop Base_BaseProp, err: " << std::hex << err << std::endl;
		return 1;
	} else {
		std::cout << "value: " << value << std::endl;
	}

	if (!PR_FAIL(CALL_SampleI_baseMethod(sample,0))) {
		std::cerr << "unexpected return value" << std::endl;
	}

	return 0;
}
