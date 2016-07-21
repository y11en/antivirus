#include "ssl_util.h"
#include <stdio.h>
#include "wincript/winverifytrust.h"

#include <Prague/pr_wrappers.h>

bool g_bSSLLoaded = FALSE;
static PRLocker   g_hSSLCertCache;
static CWinVerifyTrust* g_pWFT = NULL;

#define CERTNAME_MAXLEN	256
#define strcasecmp _stricmp

bool SaveToCryptoStorage(IN const char* file);
bool ExtractFromCryptoStorage(IN const char* file);


//////////////////////////////////////////////////////////////////////////
//
//	Part of crypto/threads/mttest.c
//	Thread-safe enabling
//
#include <openssl/applink.c>
static HANDLE *lock_cs = NULL;
void win32_locking_callback(int mode, int type, const char *file, int line)
{
	if (mode & CRYPTO_LOCK)
		WaitForSingleObject(lock_cs[type],INFINITE);
	else
		ReleaseMutex(lock_cs[type]);
}
void thread_setup()
{
	int i = 0;

	lock_cs = (HANDLE*)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(HANDLE));
	for (i=0; i<CRYPTO_num_locks(); i++)
		lock_cs[i]=CreateMutex(NULL,FALSE,NULL);

	CRYPTO_set_locking_callback((void (*)(int,int,const char *,int))win32_locking_callback);
}
void thread_cleanup()
{
	if ( !lock_cs )
		return;
	
	int i = 0;

	CRYPTO_set_locking_callback(NULL);
	for (i=0; i<CRYPTO_num_locks(); i++)
		CloseHandle(lock_cs[i]);
	OPENSSL_free(lock_cs);
}
//
//	Part of crypto/threads/mttest.c
//	Thread-safe enabling
//
//////////////////////////////////////////////////////////////////////////



bool ipcmp(char* name, unsigned long ip)
{
	unsigned long addr = inet_addr(name);
	if (addr == INADDR_NONE)
	{
		struct hostent *hp = gethostbyname(name);	
        if (hp == NULL)
			return false;
		addr = *((unsigned long*)hp->h_addr);
	}
	return (addr == ip);
	
}

/* RSA key length */
#define SSL_RSA_KEYLENGTH   (1024)
static RSA *rsa_tmp = NULL;      /* temporary RSA key    */
static time_t rsa_timeout = (time_t) 0; /* Timeout for this key */

/* ssl_make_rsakey() *****************************************************
 *
 * Set up RSAkey
 ************************************************************************/

static void ssl_make_rsakey()
{
    time_t now = time(NULL);
    if (rsa_tmp)
        RSA_free(rsa_tmp);

    if (!
        (rsa_tmp =
         RSA_generate_key(SSL_RSA_KEYLENGTH, RSA_F4, NULL, NULL)))

    rsa_timeout = now + (30*60);   /* 30 minutes */
}

/* ssl_init_rsakey() *****************************************************
 *
 * Initialise RSAkey stuff 
 ************************************************************************/

static void ssl_init_rsakey()
{
    ssl_make_rsakey();
}


/* ssl_check_rsakey() *****************************************************
 *
 * Generate fresh RSAkey if existing key has expired.
 *************************************************************************/

void ssl_check_rsakey()
{
    time_t now = time(NULL);

    if (!rsa_tmp || (rsa_timeout != (time_t) 0L) || (rsa_timeout < now))
        ssl_make_rsakey();
}

/* ====================================================================== */

/* A pair of OpenSSL callbacks */

static RSA *rsa_callback(SSL * s, int _export, int keylen)
{
    ssl_check_rsakey();

    return rsa_tmp;
}

EVP_PKEY *_crypto_pk_env_get_evp_pkey(RSA* rsa, bool bPrivate)
{
  RSA *key = NULL;
  EVP_PKEY *pkey = NULL;
  if (bPrivate) {
    if (!(key = RSAPrivateKey_dup(rsa)))
      goto error;
  } else {
    if (!(key = RSAPublicKey_dup(rsa)))
      goto error;
  }
  if (!(pkey = EVP_PKEY_new()))
    goto error;
  if (!(EVP_PKEY_assign_RSA(pkey, key)))
    goto error;
  return pkey;
 error:
  if (pkey)
    EVP_PKEY_free(pkey);
  if (key)
    RSA_free(key);
  return NULL;
}

/** Generate and sign an X509 certificate with the public key <b>rsa</b>,
 * signed by the private key <b>rsa_sign</b>.  The commonName of the
 * certificate will be <b>cname</b>; the commonName of the issuer will be
 * <b>cname_sign</b>. The cert will be valid for <b>cert_lifetime</b> seconds
 * starting from now.  Return a certificate on success, NULL on
 * failure.
 */
static X509 *_create_certificate(RSA *rsa,
	RSA *rsa_sign,
	const char *cname,
	const char *oname,
	ASN1_TIME* start_time,
	ASN1_TIME* end_time
	)
{
  EVP_PKEY *sign_pkey = NULL, *pkey=NULL;
  X509 *x509 = NULL;
  X509_NAME *name = NULL, *name_issuer=NULL;
  int nid;

  if (!(sign_pkey = _crypto_pk_env_get_evp_pkey(rsa_sign,true)))
    goto error;
  if (!(pkey = _crypto_pk_env_get_evp_pkey(rsa,false)))
    goto error;
  if (!(x509 = X509_new()))
    goto error;
  if (!(X509_set_version(x509, 2)))
    goto error;
  if (!(ASN1_INTEGER_set(X509_get_serialNumber(x509), (long)time(NULL))))
    goto error;

  if (!(name = X509_NAME_new()))
    goto error;
  if ((nid = OBJ_txt2nid("organizationName")) == NID_undef) 
	  goto error;
  if (oname && strlen(oname) && !(X509_NAME_add_entry_by_NID(name, nid, MBSTRING_ASC, (unsigned char*)oname, -1, -1, 0))) 
	  goto error;
  if ((nid = OBJ_txt2nid("commonName")) == NID_undef) 
	  goto error;
  if (cname && strlen(cname) && !(X509_NAME_add_entry_by_NID(name, nid, MBSTRING_ASC, (unsigned char*)cname, -1, -1, 0))) 
	  goto error;
  if (!(X509_set_subject_name(x509, name)))
    goto error;

  if (!(name_issuer = X509_NAME_new()))
    goto error;
  if ((nid = OBJ_txt2nid("organizationName")) == NID_undef) 
	  goto error;
  if (oname && strlen(oname) && !(X509_NAME_add_entry_by_NID(name_issuer, nid, MBSTRING_ASC, (unsigned char*)oname, -1, -1, 0))) 
	  goto error;
  if ((nid = OBJ_txt2nid("commonName")) == NID_undef) 
	  goto error;
  if (cname && strlen(cname) && !(X509_NAME_add_entry_by_NID(name_issuer, nid, MBSTRING_ASC, (unsigned char*)cname, -1, -1, 0))) 
	  goto error;
  if (!(X509_set_issuer_name(x509, name_issuer)))
    goto error;

  if (!X509_set_notBefore(x509, start_time))
    goto error;
  if (!X509_set_notAfter(x509, end_time))
    goto error;
  if (!X509_set_pubkey(x509, pkey))
    goto error;
  if (!X509_sign(x509, sign_pkey, EVP_md5()))
    goto error;

  goto done;

error:
  if (x509) {
    X509_free(x509);
    x509 = NULL;
  }

done:
  if (sign_pkey)
    EVP_PKEY_free(sign_pkey);
  if (pkey)
    EVP_PKEY_free(pkey);
  if (name)
    X509_NAME_free(name);
  if (name_issuer)
    X509_NAME_free(name_issuer);
  return x509;
}

X509* create_certificate(
	RSA *rsa,
	const char *cname,
	const char *oname,
	ASN1_TIME* start_time,
	ASN1_TIME* end_time,
	const char* dir = NULL
	)
{
	X509* pkey = load_certificate(dir, cname);
	if ( !pkey )
		pkey = _create_certificate(rsa, rsa, cname, oname, start_time, end_time);
	return pkey;
}

int get_certificate_name(X509 *cert, char* buf, int buf_size)
{
	X509_NAME *subj = X509_get_subject_name(cert);
	if ( !subj )
		return 0;
	int i = X509_NAME_get_text_by_NID(subj, NID_commonName, buf, buf_size);
	if ( i ) buf[buf_size-1] = 0;
	return i;
}

int get_certificate_org_name(X509 *cert, char* buf, int buf_size)
{
	X509_NAME *subj = X509_get_subject_name(cert);
	if ( !subj )
		return 0;
	int i = X509_NAME_get_text_by_NID(subj, NID_organizationName, buf, buf_size);
	if ( i ) buf[buf_size-1] = 0;
	return i;
}

#define REPLACE_BAD_SYMBOLS(_str)	\
{	\
	for ( size_t i = 0; i < _str.size(); i++ )	\
	{	\
		switch ( _str.at(i) )	\
		{	\
		case '\\':	\
		case '/':	\
		case ':':	\
		case '*':	\
		case '?':	\
		case '"':	\
		case '<':	\
		case '>':	\
		case '|':	\
			_str.at(i) = '_';	\
			break;	\
		};	\
	}	\
}

X509* load_certificate(const char* dir, const char* p_name)
{
	std::string file;
	std::string name = p_name; REPLACE_BAD_SYMBOLS(name);
	FILE *fp = NULL;
	X509* pkey = NULL;
	

	file = dir;
	file += "\\[fake]";
	file += name;
	file += ".pem";


	PRAutoLocker _cs_(g_hSSLCertCache); // Синхронизируем доступ к кешу сертификатов
	fp = fopen(file.c_str(), "rb");
	if (fp == NULL) 
		return false;
	d2i_X509_fp(fp, &pkey);
	fclose(fp);
	return pkey;
}

RSA* load_privatekey(const char* dir)
{
	std::string file;
	FILE *fp = NULL;
	RSA* pkey = NULL;
	
	file = dir;
	file += "\\KLSSL_privkey.pem";

	PRAutoLocker _cs_(g_hSSLCertCache); // Синхронизируем доступ к кешу сертификатов
	fp = fopen(file.c_str(), "rb");
	if (fp) 
	{
		d2i_RSAPrivateKey_fp(fp, &pkey);
		fclose(fp);
		// Прячем старый ключ в криптохранилище
		if ( pkey && SaveToCryptoStorage(file.c_str()) )
			DeleteFileA(file.c_str());
	}
	else
	{
		// Достаем ключ из криптохранилища
		if ( !ExtractFromCryptoStorage(file.c_str()) )
			return false;
		fp = fopen(file.c_str(), "rb");
		if (fp == NULL) 
			return false;
		d2i_RSAPrivateKey_fp(fp, &pkey);
		fclose(fp);
		if ( pkey ) 
			DeleteFileA(file.c_str());
		return pkey;
	}
	return pkey;
}

bool save_privatekey(RSA* pkey, const char* dir)
{
	std::string file;
	FILE *fp = NULL;

	file = dir;
	file += "\\KLSSL_privkey.pem";

	PRAutoLocker _cs_(g_hSSLCertCache); // Синхронизируем доступ к кешу сертификатов
	fp = fopen(file.c_str(), "wb");
	if (fp == NULL) 
		return false;
	int i = i2d_RSAPrivateKey_fp(fp, pkey);
	fflush(fp);
	fclose(fp);

	// Прячем ключ в криптохранилище
	if ( (i = 1) && SaveToCryptoStorage(file.c_str()) )
	{
		DeleteFileA(file.c_str());
		return true;
	}

	return true;
}

bool save_publickey(RSA* pkey, const char* dir)
{
	std::string file;
	FILE *fp = NULL;

	file = dir;
	file += "\\KLSSL_pubkey.pem";

	PRAutoLocker _cs_(g_hSSLCertCache); // Синхронизируем доступ к кешу сертификатов
	fp = fopen(file.c_str(), "wb");
	if (fp == NULL) 
		return false;
	i2d_RSAPublicKey_fp(fp, pkey);
	fclose(fp);
	return true;
}

bool save_certificate(SSL *ssl, X509 *cert, const char* dir, std::string& file)
{
	if ( !cert ) 
		cert = SSL_get_peer_certificate(ssl);
	if ( !cert ) 
		return false;
	char data[CERTNAME_MAXLEN];
	FILE *fp = NULL;

	if ( file.empty() )
	{
		file = dir;
		file += "\\";
		if ( ssl )
			file += "[orig]";
		else
			file += "[fake]";
		
		if ( get_certificate_name(cert, data, CERTNAME_MAXLEN) )
		{
			std::string name = data; REPLACE_BAD_SYMBOLS(name);
			file += name;
			file += ".pem";
		}
		else
			file += "unknown.pem";
	}
	
	PRAutoLocker _cs_(g_hSSLCertCache); // Синхронизируем доступ к кешу сертификатов
	fp = fopen(file.c_str(), "rb");
	if (fp != NULL) 
	{
		//Такой файл уже есть 
		fclose(fp);
		return false;
	}

	fp = fopen(file.c_str(), "wb");
	if (fp == NULL) 
		return false;
	i2d_X509_fp(fp, cert);
	fclose(fp);
	X509_free(cert);
	return true;
}

bool check_certificate(const char* file)
{
	PRAutoLocker _cs_(g_hSSLCertCache); // Синхронизируем доступ к кешу сертификатов
	if ( g_pWFT )
		return g_pWFT->CheckCertificate((char*)file);
	return true;
}

long post_connection_check(SSL *ssl, const char *host, unsigned long ip_addr, const char* file)
{
	// Проверяем только в g_pWFT->CheckCertificate
	return check_certificate(file) ? X509_V_OK : X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT;

//	X509 *cert;
//	X509_NAME *subj;
//	char data[CERTNAME_MAXLEN];
//	int extcount;
//	int ok = 0;
//	long lCertCheck = X509_V_OK;
//	if (
//		!(cert = SSL_get_peer_certificate(ssl)) || 
//		(!host && !ip_addr)
//		)
//		goto err_occured;
//	if ((extcount = X509_get_ext_count(cert)) > 0)
//	{
//		int i;
//		for (i = 0; i < extcount; i++)
//		{
//			char *extstr;
//			X509_EXTENSION *ext;
//			ext = X509_get_ext(cert, i);
//			extstr = (char*) OBJ_nid2sn(OBJ_obj2nid(X509_EXTENSION_get_object(ext)));
//			if (!strcmp(extstr, "subjectAltName"))
//			{
//				int j;
//				unsigned char *data;
//				STACK_OF(CONF_VALUE) *val;
//				CONF_VALUE *nval;
//				X509V3_EXT_METHOD *meth;
//				void *ext_str = NULL;
//				if (!(meth = X509V3_EXT_get(ext)))
//					break;
//				data = ext->value->data;
//#if (OPENSSL_VERSION_NUMBER > 0x00907000L)
//				if (meth->it)
//					ext_str = ASN1_item_d2i(NULL, (const unsigned char **)&data, ext->value->length,
//					ASN1_ITEM_ptr(meth->it));
//				else
//					ext_str = meth->d2i(NULL, (const unsigned char **)&data, ext->value->length);
//#else
//				ext_str = meth->d2i(NULL, (const unsigned char **)&data, ext->value->length);
//#endif
//				val = meth->i2v(meth, ext_str, NULL);
//				for (j = 0; j < sk_CONF_VALUE_num(val); j++)
//				{
//					nval = sk_CONF_VALUE_value(val, j);
//					printf("value = %s\n host = %s\n", nval->value, host);
//					if (
//						!strcmp(nval->name, "DNS") && 
//						!strcmp(nval->value, host) || ipcmp(nval->value, ip_addr)
//						)
//					{
//						ok = 1;
//						break;
//					}
//				}
//			}
//			if (ok)
//				break;
//		}
//	}
//	if (!ok && (subj = X509_get_subject_name(cert)) &&
//		X509_NAME_get_text_by_NID(subj, NID_commonName, data, CERTNAME_MAXLEN) > 0)
//	{
//		data[CERTNAME_MAXLEN-1] = 0;
//		if (
//			(host && strcasecmp(data, host) != 0) ||
//			!ipcmp(data, ip_addr)
//			)
//			goto err_occured;
//	}
//	X509_free(cert);
//	lCertCheck = SSL_get_verify_result(ssl);
//	if ( X509_V_OK != lCertCheck )
//		goto err_occured;
//	return check_certificate(file) ? X509_V_OK : X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT;
//err_occured:
//	if (cert)
//		X509_free(cert);
//	return X509_V_ERR_APPLICATION_VERIFICATION;
}


static PRLocker g_cs;
static RSA* g_pRSATemp = NULL;
RSA* get_RSA(const char* dir)
{
	PRAutoLocker _cs_(g_cs);
	if ( !g_pRSATemp )
		g_pRSATemp = load_privatekey(dir);
	static bool bIs9x = (GetVersion() & 0x80000000) != 0;
	if(g_pRSATemp && bIs9x && RSA_size(g_pRSATemp) != 64)
	{
		// меняем длину private ключа для 9x (не работаем с IE c 40-битной шифрацией)
		g_pRSATemp = NULL;
	}
	if ( !g_pRSATemp )
	{
		g_pRSATemp = RSA_generate_key(bIs9x ? 512 : 1024, RSA_F4, NULL, NULL);
		if ( g_pRSATemp )
		{
			save_privatekey(g_pRSATemp, dir);
			//save_publickey(g_pRSATemp, dir);
		}
	}
	return g_pRSATemp;
}
void  free_RSA()
{
	PRAutoLocker _cs_(g_cs);
	if ( g_pRSATemp )
		RSA_free(g_pRSATemp);
	g_pRSATemp = NULL;
	if ( g_pWFT )
		delete g_pWFT;
	g_pWFT = NULL;
}

class c
{
public:
	~c()
	{
		thread_cleanup();
		EVP_cleanup();
		free_RSA();
	}
};
static c p;

void* ssl_malloc  (size_t n);
void* ssl_realloc (void* p, size_t n);
void  ssl_free    (void* p);

bool LoadSSLLibrary()
{
	PRAutoLocker _cs_(g_cs);
	if ( !g_bSSLLoaded )
	{
		try
		{
			CRYPTO_set_mem_functions( ssl_malloc, ssl_realloc, ssl_free); //CRYPTO_malloc_init();
			SSL_library_init();
			OpenSSL_add_all_algorithms();
			thread_setup();
			if ( !g_pWFT )
				g_pWFT = new CWinVerifyTrust();
			g_bSSLLoaded=TRUE;
			return true;
		}
		catch (...) 
		{
		}
	}
	else
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////
//
//	Создадим собственный хип для OpenSSL
//
HANDLE g_hOpenSSLHeap = NULL;
void* ssl_malloc( size_t n )
{
	if ( !g_hOpenSSLHeap )
	{
		PRAutoLocker _cs_(g_cs);
		if ( !g_hOpenSSLHeap )
			g_hOpenSSLHeap = ::HeapCreate( 0, 0, 0 );
	}
	if ( g_hOpenSSLHeap ) 
		return ::HeapAlloc( g_hOpenSSLHeap, 0, n );
	return malloc(n);
}

void* ssl_realloc( void* p, size_t n )
{
	if ( g_hOpenSSLHeap )
		return ::HeapReAlloc(g_hOpenSSLHeap, 0, p, n);
	return realloc(p, n);
}

void ssl_free( void* p )
{
	if ( g_hOpenSSLHeap )
	{
		::HeapFree( g_hOpenSSLHeap, 0, p );
		return;
	}
	free(p);
}
//
//	Создадим собственный хип для OpenSSL
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Будем хранить приватный ключ в SecurityStorage
//
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
extern hROOT g_root;
#include <ProductCore/plugin/p_cryptohelper.h>
#include <PPP/structs/s_licensing.h>

struct privkey_t
{
	privkey_t(): size(0) { memset(data,0,1024); };
	int size;
	char data[1024];
};

bool SaveToCryptoStorage(IN const char* file)
{
	FILE* fp = fopen(file, "rb");
	if ( !fp || !g_root ) 
		return false;
	
	privkey_t privkey;
	privkey.size = fread( privkey.data, sizeof(char), 1024, fp );
	if ( privkey.size && feof(fp) )
	{
		if (fp) fclose(fp); fp = NULL;
		bool success = false;
		cCryptoHelper* pCryptoHelper = NULL;
		success = 
			PR_SUCC( g_root->sysCreateObjectQuick((hOBJECT*)&pCryptoHelper, IID_CRYPTOHELPER, PID_CRYPTOHELPER)) &&
			PR_SUCC( pCryptoHelper->StoreData(esiSSLPrivkey, &privkey, sizeof(privkey)));
		if ( pCryptoHelper ) pCryptoHelper->sysCloseObject(); pCryptoHelper = NULL;
			return success;
	}
	if (fp) fclose(fp); fp = NULL;
	return false;
}

bool ExtractFromCryptoStorage(IN const char* file)
{
	FILE* fp = fopen(file, "wb");
	if ( !fp || !g_root ) 
		return false;
	
	privkey_t privkey;
	tDWORD size = sizeof(privkey);
	bool success = false;
	int written = 0;
	cCryptoHelper* pCryptoHelper = NULL;
	success = 
		PR_SUCC( g_root->sysCreateObjectQuick((hOBJECT*)&pCryptoHelper, IID_CRYPTOHELPER, PID_CRYPTOHELPER)) &&
		PR_SUCC( pCryptoHelper->GetData(esiSSLPrivkey, &privkey, &size));
	if ( pCryptoHelper ) pCryptoHelper->sysCloseObject(); pCryptoHelper = NULL;
	if ( success )
		written = fwrite(privkey.data, sizeof(char), privkey.size, fp);
	success &= (written == privkey.size);

	if (fp) fflush(fp);
	if (fp) fclose(fp); fp = NULL;
	return success;
}
//
// Будем хранить приватный ключ в SecurityStorage
//
//////////////////////////////////////////////////////////////////////////
