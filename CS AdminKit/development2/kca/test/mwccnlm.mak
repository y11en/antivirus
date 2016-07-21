# -*- Makefile -*- Time-stamp: <02/12/26 23:03:35 ptr>
# $Id: mwccnlm.mak,v 1.1.1.1 2003/07/23 13:15:29 ptr Exp $

SRCROOT := ../../../../explore
SRCROOT_STD := ../../
COMPILER_NAME = mwccnlm
STLPORT_DIR := ../../../../Novell-STLP/STLport
CoMT_DIR := ../../../../explore
BOOST_DIR := ../../../../extern/boost
GSOAP_DIR := ../../gsoap
STDLIB_DIR:= ../../std/obj/netware-mwccnlm/shared 
STRLIB_DIR:= ../../transport/obj/netware-mwccnlm/shared 
KCALIB_DIR:= ../obj/netware-mwccnlm/shared 
BOOSTLIB_DIR:= ../../../custom/boost/libs/filesystem/obj/netware-mwccnlm/shared

include Makefile.inc
include ${SRCROOT}/Makefiles/top.mak

INCLUDES += -I$(SRCROOT_STD)include -I$(STLPORT_INCLUDE_DIR)\
	    -I$(GSOAP_DIR)\
	    -I$(GSOAP_DIR)/std\
	    -I$(BOOST_DIR)\
	    -I$(SRCROOT)/../../CommonFiles\
	    -I$(SRCROOT_STD)/tst/kltester\
	    -I../ -I../../ -I../../std
release-shared:	LDSEARCH = -L${SRCROOT}/build/lib\
		-L${STDLIB_DIR} -L${STRLIB_DIR} -L${KCALIB_DIR} -L${STLPORT_DIR}/build/lib/obj/netware-mwccnlm/shared -L$(BOOSTLIB_DIR)

dbg-shared:	LDSEARCH = -L${SRCROOT}/build/lib\
		-L${SRCROOT}/std/obj/gcc/shared-g -L${STLPORT_DIR}/include/stlport/lib

release-shared : LDLIBS = -lklcsstd -lklcstr -lklcskca -lstlport -lboost
dbg-shared     : LDLIBS = -lklcsstdg -lstlport -lssl -lcrypto

#LDFLAGS += -commandfile novelltest.def "$(MWCW_NOVELL)/Libraries/Runtime/mwcrtl.lib"  "$(MWCW_NOVELL)/Libraries/Runtime/Output/CLib/mwcrtl.lib"
LDFLAGS +=  -commandfile novelltest.def \
            "$(MWCW_NOVELL)/Libraries/Runtime/mwcrtl.lib"\
            "$(MWCW_NOVELL)/Libraries/Runtime/Output/CLib/mwcrtl.lib"\
            "$(NWSDK_DIR)/imports/locnlm32.imp"\
            "$(NWSDK_DIR)/imports/socklib.imp"
