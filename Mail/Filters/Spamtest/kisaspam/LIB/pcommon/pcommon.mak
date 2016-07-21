#
# $Source: /projects/lib/pcommon/pcommon.mak,v $
# $Revision: 1.4 $
#
.include "../../@host.mak"

_TARGET	= _STATIC
_STLP_NO_IOSTREAMS=1

.include "$(ProjectRoot_Directory)/_include/common.ext.mak"

DST	= ../libpcommon$(_A)
SRC	= koi8_ctype.c strptime.c \
          convert.cpp Dos2Uni.cpp Iso2Uni.cpp Koi2Uni.cpp Mac2Uni.cpp Win2Uni.cpp \
          Lat12Uni.cpp Lat12Uni_cyr.cpp

MakefileTemplate += pcommon.mak
.include "$(ProjectRoot_Directory)/_include/common.lib.mak"
