#include <sys/types.h>
#include <time.h>

//typedef struct _GUID
//{
//    unsigned long Data1;
//    unsigned short Data2;
//    unsigned short Data3;
//    unsigned char Data4[8];
//} GUID;

//typedef GUID UUID;
struct UUID
{
    unsigned char Data16[16];
};
//typedef unsigned char UUID[16];
#define uuid_t UUID

//typedef unsigned char uuid_t[16];

/* UUID Variant definitions */
#define UUID_VARIANT_NCS 	0
#define UUID_VARIANT_DCE 	1
#define UUID_VARIANT_MICROSOFT	2
#define UUID_VARIANT_OTHER	3

extern "C"
{
    void uuid_initialize();
    void uuid_deinitialize();
    
    //! function uses staitc vars and is not thread-safe !!!
    void uuid_generate(uuid_t& out);

    int  uuid_parse(char *in, uuid_t& uu);
    void uuid_unparse(uuid_t& uu, char *out);

}
