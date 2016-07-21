/* packing macros */

#ifdef KL_PACKED
# error kl_pushpack.h already has been included
#endif

#ifdef __GNUC__
# define KL_PACKED __attribute__ ((packed))
#else
# define KL_PACKED
#endif

#ifdef _MSC_VER
# pragma warning(disable:4103)
# pragma pack(push,1)
#else
# pragma pack(1)
#endif



