/* packing macros */

#ifndef KL_PACKED
# error kl_pushpack.h should have been included
#endif

#ifdef _MSC_VER
# pragma pack(pop)
#else
# pragma pack()
#endif

#undef KL_PACKED



