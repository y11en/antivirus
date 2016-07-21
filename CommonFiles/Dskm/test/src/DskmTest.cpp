// DskmTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <dskm/cpp_wrap/dskmwrap.h>
#include "RunTest.h"
#include "Test_GenerateKeyPair.h"
#include "Test_CheckObjects.h"
#include "Test_DeleteKey.h"

//////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	using namespace std;
	using namespace KL::DskmLibWrapper::Test;
	
	TestRunner tr;
	
	tr.RunTest("GenerateKeyPair::SomeCase", Test_GenerateKeyPair_SomeCase);
	tr.RunTest("GenerateKeyPair::SignRegFile", Test_GenerateKeyPair_SignRegFile);
	tr.RunTest("CheckObjects::Main", Test_CheckObjects_Main);
	tr.RunTest("DeleteKey::Main", Test_DeleteKey_Main);
	
	cout << endl;
	
	tr.PrintStatistics(cout);
	
	return (tr.Failed() > 0 ? 1 : 0);
}

