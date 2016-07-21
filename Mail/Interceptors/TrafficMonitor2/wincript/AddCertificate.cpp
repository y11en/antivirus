#include "winverifytrust.h"

bool AddCertificate(char* szFileName, HCERTSTORE hSystemStoreROOT)
{
	bool bResult = false;

	//--------------------------------------------------------------------
	// Declare and initialize variables.
	
	PCCERT_CONTEXT  pTargetCert = NULL;
	HCERTSTORE      hFileStore = NULL;
	DWORD			dwVerificationFlags = 
		CERT_STORE_SIGNATURE_FLAG | 
		CERT_STORE_TIME_VALIDITY_FLAG 
		;
	
	bool bSystemStoreROOT = false;
	if ( !hSystemStoreROOT ) 	
	{
		CHECK_CERT_RESULT(	
			hSystemStoreROOT = CertOpenSystemStore( 0, "ROOT"	),
			"CertOpenStore succeeded. The ROOT store is open. \n",
			"Error opening the ROOT store."
			);
		bSystemStoreROOT = true;
	}

	CHECK_CERT_RESULT(	
		hFileStore = CertOpenStore(
			CERT_STORE_PROV_FILENAME_A,
			MY_ENCODING_TYPE,
			NULL,
			0,
			szFileName ),
		"The file store is open. Continue.\n", 
		"The file store did not open."
		);
	
	CHECK_CERT_RESULT(	
		pTargetCert = CertEnumCertificatesInStore( hFileStore, pTargetCert),
		"A certificate is available. Continue.\n",
		"No certificate available. The store may be empty."
		);
	

	bResult = CertAddCertificateContextToStore(
		hSystemStoreROOT,
		pTargetCert, 
		CERT_STORE_ADD_REPLACE_EXISTING, NULL
		) != FALSE;
	if ( !bResult )
		PR_TRACE( (NULL, prtNOTIFY, "wincrypt => CertAddCertificateContextToStore failed: 0x%X\n", GetLastError() ));

	//--------------------------------------------------------------------
	// Clean up memory and quit.
	
	CERT_CLOSE_CTX(   pTargetCert       , "pTargetCert"       );
	CERT_CLOSE_STORE( hFileStore        , "hFileStore"        );
	if ( bSystemStoreROOT )
		CERT_CLOSE_STORE( hSystemStoreROOT  , "hSystemStoreROOT"  );
	return bResult;
}