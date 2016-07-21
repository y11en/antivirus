#ifdef __WATCOMC__
#  include <direct.h>
#  define MAXPATH 260
#  define MAXDRIVE 3
#  define MAXDIR  256
#  define MAXFILE 256
#  define MAXEXT  10
#  define FA_DIREC		_A_SUBDIR
#  define FA_RDONLY		_A_RDONLY
#  define FA_ARCH		_A_ARCH
#  define findnext		_dos_findnext
#  define findfirst(p,blk,attr)	_dos_findfirst(p,attr,blk)
#  define findclose(p)		_dos_findclose(p)
#  define ffblk			struct find_t
#  define ff_attrib		attrib
#  define ff_ftime		wr_time
#  define ff_fdate		wr_date
#  define ff_fsize		size
#  define ff_name		name
#else
#  define findclose(p)
#  if !defined( __DIR_H )
#    include <Dir.h>
#  endif  // __DIR_H
#endif
