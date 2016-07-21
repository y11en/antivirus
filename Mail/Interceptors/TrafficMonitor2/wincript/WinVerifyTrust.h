#ifndef _WINVERIFYTRUST_H_
#define _WINVERIFYTRUST_H_

#include <Prague/prague.h>

#pragma warning(disable:4786)
#include <stdio.h>
#include <time.h>
#include <map>
#include <string>
#include <windows.h>
#include <wincrypt.h>

#pragma comment( lib, "crypt32.lib" )

#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)

#ifdef	USE_UNICODE
	#define STRINGTYPE char
#else
	#define STRINGTYPE char
#endif//USE_UNICODE
void MyHandleError(char *s);
bool ViewCertificate (STRINGTYPE* szFileName = NULL);
bool CheckCertificate (STRINGTYPE* szFileName, HCERTSTORE hCollectionStore = NULL);
bool CheckCertificateChain(PCCERT_CONTEXT pCertContext);
bool AddCertificate (STRINGTYPE* szFileName, HCERTSTORE hSystemStoreROOT = NULL);
bool RemoveCertificate (STRINGTYPE* szFileName, HCERTSTORE hCollectionStore = NULL);

#define CHECK_CERT_RESULT(_h, _sgood, _sbad)	if ( _h ) PR_TRACE( (NULL, prtNOTIFY, "wincript =>" _sgood)); else { MyHandleError(_sbad); return false; }
#define CHECK_CET_ERRORSTATUS(_status, _flag, _s) if ( _status & _flag ) PR_TRACE( (NULL, prtNOTIFY,"wincript => \t0x%X\t" _s, _flag));
#define CERT_CLOSE_STORE(_h, _s) if (CertCloseStore( _h, 0)) PR_TRACE( (NULL, prtNOTIFY, "wincript => " _s " closed\n")); else MyHandleError("Could not close the " _s);
#define CERT_CLOSE_CTX(_h, _s) if (_h && CertFreeCertificateContext( _h )) PR_TRACE( (NULL, prtNOTIFY, "wincript => " _s " closed\n")); else PR_TRACE( (NULL, prtNOTIFY,"wincript => Could not close the " _s "\n"));

class CWinVerifyTrust
{
public:
	CWinVerifyTrust():
		m_hCollectionStore(NULL),
		m_hSystemStoreCA(NULL),
		m_hSystemStoreMY(NULL),
		m_hSystemStoreROOT(NULL)
	{
		Prepare();
	}
	virtual ~CWinVerifyTrust()
	{
		Done();
	}
	bool ViewCertificate (char* szFileName = NULL)
	{
		return ::ViewCertificate(szFileName);
	};
	bool CheckCertificate (char* szFileName)
	{
		cert_check_results_t::iterator i = m_cert_check_results.find(szFileName);
		if ( i != m_cert_check_results.end() )
		{
			if ( !((cert_check_result_t)i->second).valid_record() )
				m_cert_check_results.erase(i);
			else
				return ((cert_check_result_t)i->second).valid;
		}
		bool bValid = ::CheckCertificate(szFileName, m_hCollectionStore);
		m_cert_check_results[szFileName] = cert_check_result_t(bValid);
		return bValid;
			
	};
	bool AddCertificate (char* szFileName)
	{
		return ::AddCertificate(szFileName, m_hSystemStoreROOT);
	}
	bool RemoveCertificate(char* szFileName)
	{
		return ::RemoveCertificate(szFileName, m_hCollectionStore);
	}

private:
	HCERTSTORE		m_hCollectionStore;     // The collection store handle
	HCERTSTORE      m_hSystemStoreCA;
	HCERTSTORE      m_hSystemStoreMY;
	HCERTSTORE      m_hSystemStoreROOT;
	bool Prepare()
	{
		CHECK_CERT_RESULT( 
			m_hSystemStoreCA = CertOpenSystemStore( 0, "CA" ), 
			"CertOpenStore succeeded. The CA store is open. \n", 
			"Error opening the CA store." 
			);
		
		CHECK_CERT_RESULT(	
			m_hSystemStoreMY = CertOpenSystemStore( 0, "MY"	),
			"CertOpenStore succeeded. The MY store is open. \n",
			"Error opening the MY store."
			);
		
		CHECK_CERT_RESULT(	
			m_hSystemStoreROOT = CertOpenSystemStore( 0, "ROOT"	),
			"CertOpenStore succeeded. The ROOT store is open. \n",
			"Error opening the ROOT store."
			);

		CHECK_CERT_RESULT(	
			m_hCollectionStore = CertOpenStore( CERT_STORE_PROV_COLLECTION, 0, 0, 0, 0 ), 
			"The collection store was opened. \n",
			"There was an error while opening the collection store!"
			);

		CertAddStoreToCollection(m_hCollectionStore, m_hSystemStoreCA, 0, 0);
		CertAddStoreToCollection(m_hCollectionStore, m_hSystemStoreMY, 0, 2);
		CertAddStoreToCollection(m_hCollectionStore, m_hSystemStoreROOT, 0, 1);
		return true;
	};
	void Done()
	{
		CERT_CLOSE_STORE( m_hSystemStoreCA    , "hSystemStoreCA"    );
		CERT_CLOSE_STORE( m_hSystemStoreMY    , "hSystemStoreMY"    );
		CERT_CLOSE_STORE( m_hSystemStoreROOT  , "hSystemStoreROOT"  );
		CERT_CLOSE_STORE( m_hCollectionStore  , "hCollectionStore"  );
	};

private:
	struct cert_check_result_t
	{
		cert_check_result_t(): check_date(time(0)), valid(false) {};
		cert_check_result_t(bool v): check_date(time(0)), valid(v) {};
		bool   valid_record() { return check_date > (time(0) - 3600); };
		time_t check_date;
		bool   valid;
	};
	typedef std::map<std::string, cert_check_result_t> cert_check_results_t;
	cert_check_results_t m_cert_check_results;
};

#endif//_WINVERIFYTRUST_H_
