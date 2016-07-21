#ifndef _SSL_UTIL_H_
#define _SSL_UTIL_H_

#include <string>
#include <windows.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
//#include <openssl/ossl_typ.h>

//--------------------------------------------------------------------------------------------------
//typedef int (*PFN_SSL_int_void)(void);
//typedef PVOID (*PFN_SSL_pvoid_void)(void);
//typedef PVOID (*PFN_SSL_pvoid_int)(int);
//typedef PVOID (*PFN_SSL_pvoid_pvoid)(PVOID);
//typedef void (*PFN_SSL_void_pvoid)(PVOID);
//typedef int (*PFN_SSL_int_pvoid_int)(PVOID, int);
//typedef int (*PFN_SSL_int_pvoid)(PVOID);
//typedef int (*PFN_SSL_int_pvoid_pvoid_int)(PVOID, PVOID, int);
//--------------------------------------------------------------------------------------------------
//extern PFN_SSL_int_void			fSSL_library_init;			// int SSL_library_init()
//extern PFN_SSL_pvoid_void		fSSLv23_method;				// SSL_METHOD *SSLv23_method()
//extern PFN_SSL_pvoid_void		fSSLv23_server_method;		// SSL_METHOD *SSLv23_server_method()
//extern PFN_SSL_pvoid_void		fSSLv23_client_method;		// SSL_METHOD *SSLv23_client_method()
//extern PFN_SSL_pvoid_pvoid		fSSL_CTX_new;				// SSL_CTX *SSL_CTX_new(SSL_METHOD *method)
//extern PFN_SSL_void_pvoid		fSSL_CTX_free;				// void SSL_CTX_free(SSL_CTX *ctx);
//extern PFN_SSL_pvoid_pvoid		fSSL_get_peer_certificate;	// X509 *SSL_get_peer_certificate(SSL* ssl)
//extern PFN_SSL_pvoid_pvoid		fSSL_new;					// SSL *SSL_new(SSL_CTX *ctx)
//extern PFN_SSL_void_pvoid		fSSL_free;					// void SSL_free(SSL *ssl);
//extern PFN_SSL_int_pvoid_int	fSSL_set_fd;				// int SSL_set_fd(SSL *ssl, int fd);
//extern PFN_SSL_int_pvoid_int	fSSL_set_read_ahead;		// int SSL_set_read_ahead(SSL *ssl, int yes);
//extern PFN_SSL_int_pvoid		fSSL_connect;				// int SSL_connect(SSL *ssl);
//extern PFN_SSL_int_pvoid		fSSL_accept;				// int SSL_accept(SSL *ssl);
//extern PFN_SSL_int_pvoid		fSSL_pending;				// int SSL_pending(SSL *ssl);
//extern PFN_SSL_int_pvoid		fSSL_get_read_ahead;		// int SSL_get_read_ahead(SSL *ssl);
//extern PFN_SSL_int_pvoid		fSSL_get_verify_result;		// long SSL_get_verify_result(SSL *ssl);
//extern PFN_SSL_int_pvoid_pvoid_int	fSSL_read;				// int SSL_read(SSL *ssl, void *buffer, int bufsize)
//extern PFN_SSL_int_pvoid_pvoid_int	fSSL_write;				// int SSL_write(SSL *ssl, void *buffer, int bufsize)
//extern PFN_SSL_int_pvoid_pvoid_int	fSSL_CTX_use_PrivateKey_file;// int SSL_CTX_use_PrivateKey_file(SSL *ssl, void *buffer, int bufsize)
//extern PFN_SSL_int_pvoid_pvoid_int	fSSL_CTX_use_certificate_file;// int SSL_CTX_use_certificate_file(SSL *ssl, void *buffer, int bufsize)
//extern PFN_SSL_int_pvoid_int	fSSL_get_error;				// int SSL_get_error(SSL *ssl, int ret)
//extern PFN_SSL_int_pvoid		fX509_get_ext_count;		// int X509_get_ext_count(X509 *x);
//extern PFN_SSL_pvoid_int		fOBJ_nid2sn;
//extern PFN_SSL_int_pvoid		fOBJ_obj2nid;				
//--------------------------------------------------------------------------------------------------
extern bool g_bSSLLoaded;
extern HINSTANCE g_hSSLLibrary;
//--------------------------------------------------------------------------------------------------

#define DebugLog(x,y)
RSA* get_RSA(const char* dir);
void  free_RSA();
X509* load_certificate(const char* dir, const char* name);
bool save_certificate(SSL *ssl, X509 *cert, const char* dir, std::string& file);
long post_connection_check(SSL *ssl, const char *host, unsigned long ip_addr, const char* file);
int get_certificate_name(X509 *cert, char* buf, int buf_size);
int get_certificate_org_name(X509 *cert, char* buf, int buf_size);
X509* create_certificate(
	RSA *rsa,
	const char *cname,
	const char *oname,
	ASN1_TIME* start_time,
	ASN1_TIME* end_time,
	const char* dir
	);
bool LoadSSLLibrary();

#endif//_SSL_UTIL_H_