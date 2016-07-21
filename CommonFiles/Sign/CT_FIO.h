#define F_READ          0
#define F_RDWR          1
#define F_CREATE        2

#define F_SEEK_SET      0
#define F_SEEK_CUR      1
#define F_SEEK_END      2

int     CTAPI AGetTrueHandle( CT_INT );
CT_INT  CTAPI Aopen( CT_PTR, CT_INT );
CT_INT  CTAPI Aread( CT_INT, CT_PTR, CT_INT );
CT_INT  CTAPI Awrite( CT_INT, CT_PTR, CT_INT );
CT_LONG CTAPI Aseek( CT_INT, CT_LONG, CT_INT );
CT_LONG CTAPI Atell( CT_INT );
void    CTAPI Aclose( CT_INT );

