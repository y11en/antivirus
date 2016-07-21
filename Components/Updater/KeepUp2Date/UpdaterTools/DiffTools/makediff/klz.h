#ifndef _KLZ_FORMAT_
#define _KLZ_FORMAT_

// signature
static const tBYTE bKLZSign[4] = "KLZF";

#pragma pack(push, 1)
// header
typedef struct tag_KLZ_HDR {
	tBYTE  bSign[4];
	tDWORD DSize;
	tDWORD PSize;
	tDWORD dwCrc;
	tBYTE  nHdrSize;
} KLZ_HDR;
#pragma pack(pop)

int GenKLZ(char* src_path, char* src_fn, char* res_path, char* res_fn);

#endif // _KLZ_FORMAT_
