#include "winverifytrust.h"

bool CheckCertificateChain(PCCERT_CONTEXT pCertContext)
{
	HCERTCHAINENGINE hChainEngine = NULL;
	CERT_CHAIN_ENGINE_CONFIG ChainConfig;
	PCCERT_CHAIN_CONTEXT pChainContext = NULL;
	CERT_ENHKEY_USAGE EnhkeyUsage;
	CERT_USAGE_MATCH CertUsage;
	CERT_CHAIN_PARA ChainPara;
	DWORD dwFlags = CERT_CHAIN_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT; //CERT_CHAIN_REVOCATION_CHECK_CHAIN;
	
	
	
	//---------------------------------------------------------
	// Initialize data structures.
	
	EnhkeyUsage.cUsageIdentifier = 0;
	EnhkeyUsage.rgpszUsageIdentifier = NULL;
	CertUsage.dwType = USAGE_MATCH_TYPE_AND;
	CertUsage.Usage = EnhkeyUsage;
	ChainPara.cbSize = sizeof(CERT_CHAIN_PARA);
	ChainPara.RequestedUsage = CertUsage;
	
	ChainConfig.cbSize = sizeof(CERT_CHAIN_ENGINE_CONFIG);
	ChainConfig.hRestrictedRoot= NULL;
	ChainConfig.hRestrictedTrust= NULL;
	ChainConfig.hRestrictedOther= NULL;
	ChainConfig.cAdditionalStore = 0;
	ChainConfig.rghAdditionalStore = NULL;
	ChainConfig.dwFlags = CERT_CHAIN_CACHE_END_CERT;
	
	ChainConfig.dwUrlRetrievalTimeout= 0;
	ChainConfig.MaximumCachedCertificates = 0;
	ChainConfig.CycleDetectionModulus = 0;
	
	
	
	CHECK_CERT_RESULT(	
		CertCreateCertificateChainEngine( &ChainConfig, &hChainEngine),
		"", 
		"\tCertCreateCertificateChainEngine failed."
		);
	
	bool bChainContext = hChainEngine && CertGetCertificateChain(
		hChainEngine,
		pCertContext,
		NULL,
		NULL,
		&ChainPara,
		dwFlags,
		NULL,
		&pChainContext);
	if ( bChainContext )
	{
		bChainContext = false;
		if ( pChainContext->TrustStatus.dwErrorStatus == CERT_TRUST_NO_ERROR )
		{
			bChainContext = true;
			PR_TRACE( (NULL, prtNOTIFY,"wincript => \tNo error found for this certificate or chain.\n"));
		}
		else
		{
			PR_TRACE( (NULL, prtNOTIFY,"wincript => \tError status for the chain: \n"));
			DWORD dwErr = pChainContext->TrustStatus.dwErrorStatus;
			CHECK_CET_ERRORSTATUS(dwErr, CERT_TRUST_IS_NOT_TIME_VALID,			"This certificate or one of the certificates in the certificate chain is not time-valid.\n");
			CHECK_CET_ERRORSTATUS(dwErr, CERT_TRUST_IS_NOT_TIME_NESTED,			"Certificates in the chain are not properly time-nested.\n");
			CHECK_CET_ERRORSTATUS(dwErr, CERT_TRUST_IS_REVOKED,					"Trust for this certificate or one of the certificates in the certificate chain has been revoked.\n");
			CHECK_CET_ERRORSTATUS(dwErr, CERT_TRUST_IS_NOT_SIGNATURE_VALID,		"The certificate or one of the certificates in the certificate chain does not have a valid signature.\n");
			CHECK_CET_ERRORSTATUS(dwErr, CERT_TRUST_IS_NOT_VALID_FOR_USAGE,		"The certificate or certificate chain is not valid in its proposed usage.\n");
			CHECK_CET_ERRORSTATUS(dwErr, CERT_TRUST_IS_UNTRUSTED_ROOT,			"The certificate or certificate chain is based on an untrusted root.\n");
			CHECK_CET_ERRORSTATUS(dwErr, CERT_TRUST_REVOCATION_STATUS_UNKNOWN,	"The revocation status of the certificate or one of the certificates in the certificate chain is unknown.\n");
			CHECK_CET_ERRORSTATUS(dwErr, CERT_TRUST_IS_CYCLIC,					"One of the certificates in the chain was issued by a certification authority that the original certificate had certified.\n");
			CHECK_CET_ERRORSTATUS(dwErr, CERT_TRUST_IS_PARTIAL_CHAIN,			"The certificate chain is not complete.\n");
			CHECK_CET_ERRORSTATUS(dwErr, CERT_TRUST_CTL_IS_NOT_TIME_VALID,		"A CTL used to create this chain was not time-valid.\n");
			CHECK_CET_ERRORSTATUS(dwErr, CERT_TRUST_CTL_IS_NOT_SIGNATURE_VALID, "A CTL used to create this chain did not have a valid signature.\n");
			CHECK_CET_ERRORSTATUS(dwErr, CERT_TRUST_CTL_IS_NOT_VALID_FOR_USAGE, "A CTL used to create this chain is not valid for this usage.\n");
		}

		PR_TRACE( (NULL, prtNOTIFY,"wincript => \tInfo status for the chain: "));
		switch(pChainContext->TrustStatus.dwInfoStatus)
		{
		case 0:
			 PR_TRACE( (NULL, prtNOTIFY,"wincript => No information status reported."));
			 break;
		case CERT_TRUST_HAS_EXACT_MATCH_ISSUER :
			 PR_TRACE( (NULL, prtNOTIFY,"wincript => An exact match issuer certificate has been found for this certificate."));
			 break;
		case CERT_TRUST_HAS_KEY_MATCH_ISSUER: 
			 PR_TRACE( (NULL, prtNOTIFY,"wincript => A key match issuer certificate has been found for this certificate."));
			 break;
		case CERT_TRUST_HAS_NAME_MATCH_ISSUER: 
			 PR_TRACE( (NULL, prtNOTIFY,"wincript => A name match issuer certificate has been found for this certificate."));
			 break;
		case CERT_TRUST_IS_SELF_SIGNED:
			 PR_TRACE( (NULL, prtNOTIFY,"wincript => This certificate is self-signed."));
			 break;
		case CERT_TRUST_IS_COMPLEX_CHAIN:
			 PR_TRACE( (NULL, prtNOTIFY,"wincript => The certificate chain created is a complex chain.\n"));
			 break;
		case CERT_TRUST_HAS_PREFERRED_ISSUER:
			 PR_TRACE( (NULL, prtNOTIFY,"wincript => The certificate or chain has a preferred issuer. Applies to certificates and chains."));
			 break;
		case CERT_TRUST_HAS_ISSUANCE_CHAIN_POLICY:
			PR_TRACE( (NULL, prtNOTIFY,"wincript => An issuance chain policy exists. Applies to certificates and chains."));
			break;
		case CERT_TRUST_HAS_VALID_NAME_CONSTRAINTS:
			PR_TRACE( (NULL, prtNOTIFY,"wincript => A valid name constraints for all name spaces, including UPN. Applies to certificates and chains."));
			break;
		default:
			PR_TRACE( (NULL, prtNOTIFY,"wincript => none..."));
		} 
		PR_TRACE( (NULL, prtNOTIFY,"wincript => \n"));
	}
	if (pChainContext ) 
		CertFreeCertificateChain(pChainContext);
	if ( hChainEngine )
		CertFreeCertificateChainEngine(hChainEngine);
	return bChainContext;
}