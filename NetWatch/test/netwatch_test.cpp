// netwatch_test.cpp
//

#include <Prague/prague.h>
#include <Prague/pr_loadr.h>
//#include <NetWatch/i_NetWatch.h>
#include "../p_NetWatch.h"
#include <ProductCore/idl/i_task.h>

#include <stdio.h>

cRoot* g_root;

int main( int argc, char* argv[] )
{
	cPrague pr;

	//tERROR err = pr.Init ((HMODULE)0);
	tERROR err = pr.InitByFolder( "o:\\netw" );
	if (PR_FAIL (err))
	{
		printf ("error initializing Prague, error code 0x%X\n", err);
		return 1;
	}

	cAutoObj<IDL_NAME(cNetWatch)> watcher;
	err = pr->sysCreateObject( watcher, NetWatch::IID, NetWatchImpl_PLUGINID, 0 );
	if ( PR_FAIL(err) )
	{
		printf ("error creating object, error code 0x%X\n", err);
		return 1;
	}

	watcher->SetSettings( 0 );

	return 0;
}

