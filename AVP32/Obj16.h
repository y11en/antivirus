
#define		THEADR	0x80	// Source name
#define		COMENT	0x88	// Comment
#define		MODEND	0x8A	// End of Object
#define		EXTDEF	0x8C	// Externals
#define		PUBDEF	0x90	// Internals
#define		LINNUM	0x94	// Debug information
#define		LNAMES	0x96	// Names
#define		SEGDEF	0x98	// Segments definition
#define		GRPDEF	0x9A	// DGroup definitions
#define		FIXUPP	0x9C	// Fixup table
#define		LEDATA	0xA0	// Code or Data
#define		TYPDEF	0x8E	// Code or Data

#pragma pack(push,pack_obj16)
#pragma pack(1)

typedef struct{
	BYTE type;
	WORD length;
}ObjRecHead;

#pragma pack(pop,pack_obj16)
