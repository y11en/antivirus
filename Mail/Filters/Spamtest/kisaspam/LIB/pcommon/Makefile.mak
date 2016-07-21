#
# $Source: /projects/lib/pcommon/Makefile.mak,v $
# $Revision: 1.2 $
#
.include "../../@host.mak"

CONFIGURATIONS += +
CONFIGURATIONS += _PIC=1

.if !defined(_PROFILE) && !defined(_NO_MT)
CONFIGURATIONS += _MT=1
CONFIGURATIONS += _MT=1+_PIC=1
.endif

CUSTOMMAKE = pcommon.mak

.include "$(ProjectRoot_Directory)/_include/common.multi.mak"
