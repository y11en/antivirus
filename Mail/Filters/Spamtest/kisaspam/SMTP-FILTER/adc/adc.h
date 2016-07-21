#ifndef _HAVE_ADC_HEADER_
#define _HAVE_ADC_HEADER_

#include <sys/types.h>
#ifndef _WIN32
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/nameser.h>
#else // _WIN32
#include <time.h>
#include <winsock.h>
#include <_include/nix_types.h>
#include <gnu/libnet/nameser.h>
#endif // _WIN32


#ifdef __cplusplus
extern "C"
{
#endif

#define ADC_DNS_PORT 53

#define ADC_MAX_NAME_SERVERS 4
#define ADC_MAX_DNS_NAME_LEN 255
#define ADC_MAX_TXT_LEN 255
#define ADC_MAX_SEARCH_LIST_LEN 4
#define ADC_MAX_ALIASES_ENTRY 100
#define ADC_ALIASES_POOL_SIZE 4096

enum {
  ADC_SUCCESS,
  ADC_ERESOLVCONF,
  ADC_EINTERNAL,
  ADC_ESOCK,
  ADC_ECLOSE,
  ADC_ENOMEM,

  ADC_MAXERRNUM
};

typedef union {
  uint16_t id;
  u_char buf[NS_PACKETSZ];
} adc_dns_packet;

struct adc_packet {
  size_t size;
  adc_dns_packet dns_packet;
};

struct adc_dns_request {
  struct adc_packet packet;
  size_t adc_request_index;
  size_t search_list_index;
};

struct adc_search_list {
  char domain[ADC_MAX_SEARCH_LIST_LEN][ADC_MAX_DNS_NAME_LEN + 1];
  int dnum;
};

struct adc_host_aliases_data {
  struct pair {
    char *name, *fqdn;
  } table[100];
  size_t table_size;
  char pool[ADC_ALIASES_POOL_SIZE];
};

struct adc_request {
  char *name;
  struct in_addr ip;       /* if name = 0 */
  int type;
};

struct adc_response {
  int rq_index;
  int type;
  char fqdn[ADC_MAX_DNS_NAME_LEN + 1];
  char txt[ADC_MAX_TXT_LEN + 1];
  struct in_addr ip;
  u_char *data;
  int len;
  int mx_weight;
};

typedef struct {
  void *area;
  size_t rec_size;
  size_t size;
  size_t capacity;
} adc_generic_vector;

typedef struct {
  struct adc_packet *area;
  size_t rec_size;
  size_t size;
  size_t capacity;
} adc_packet_vec;

typedef struct {
  struct adc_response *area;
  size_t rec_size;
  size_t size;
  size_t capacity;
} adc_response_vec;

typedef struct {
  struct adc_dns_request *area;
  size_t rec_size;
  size_t size;
  size_t capacity;
} adc_dns_request_vec;

struct adc_object {
  /* configuration */
  int sock[ADC_MAX_NAME_SERVERS];
  size_t cur_req[ADC_MAX_NAME_SERVERS];
  int maxfd;
  int socknum;
  struct adc_search_list search_list;
  struct adc_host_aliases_data host_aliases;
  struct timeval rej_time;

  /* other */
  adc_dns_request_vec dns_request;
  adc_packet_vec dns_response;

  adc_response_vec response;
};

int adc_create(struct adc_object *adc);
int adc_destroy(struct adc_object *adc);
int adc_resolve(struct adc_object *adc,
                const struct adc_request *request, size_t req_num,
                const struct timeval *interval);
void adc_reject_time(struct adc_object *adc, const struct timeval *interval);
const char *adc_strerror(int code);

#define ADC_RESPONSE_NUM(adc) ((adc).response.size)
#define ADC_GET_RESPONSE(adc,n) ((adc).response.area[(n)])

size_t adc_response_num(struct adc_object *adc);
struct adc_response *adc_get_response(struct adc_object *adc, size_t n);

#ifdef __cplusplus
}
#endif


#endif
