#define rpcSER(name)   [in,out,ptr,size_is(name##_size)] tBYTE* name, tDWORD name##_size
