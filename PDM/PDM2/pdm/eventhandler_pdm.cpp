#include "eventhandler_pdm.h"

cEventHandlerPDM::cEventHandlerPDM()
{
}

cEventHandlerPDM::~cEventHandlerPDM()
{
}


// === Detect ==========================================================================================

void cEventHandlerPDM::OnSelfCopyMultiple(cEvent& event, cFile& copy)
{
	return;
}

void cEventHandlerPDM::OnSelfCopyToNetwork(cEvent& event, cFile& copy)
{
	return;
}

void cEventHandlerPDM::OnSelfCopyToPeerToPeerFolder(cEvent& event, cFile& copy)
{
	return;
}

void cEventHandlerPDM::OnRegAvpImageFileExecOptions(cEvent& event, const cRegKey& sKey, tcstring sValueName, tcstring sValueData)
{
	return;
}

void cEventHandlerPDM::OnSelfCopyAutoRun(cEvent& event, const cRegKey& sKey, tcstring sValueName, tcstring sValueData, tcstring sValueDataPart, tcstring sFilename)
{
	return;
}

void cEventHandlerPDM::OnSelfCopyToStartup(cEvent& event, tcstring sFilename)
{
	return;
}