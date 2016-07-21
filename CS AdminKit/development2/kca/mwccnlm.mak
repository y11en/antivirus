# -*- Makefile -*- Time-stamp: <02/12/26 23:03:35 ptr>
# $Id: mwccnlm.mak,v 1.1.1.1 2003/07/23 13:15:29 ptr Exp $

SRCROOT := ../../../explore
COMPILER_NAME = mwccnlm
STLPORT_DIR := ../../../Novell-STLP/STLport
CoMT_DIR := ../../../explore
BOOST_DIR := ../../../extern/boost
GSOAP_DIR := ../gsoap
include Makefile.inc
include ${SRCROOT}/Makefiles/top.mak



#DEFS += -DUNICODE -DUSE_BOOST_THREADS
#INCLUDES += -I$(SRCROOT)/include -I$(STLPORT_INCLUDE_DIR) -I$(SRCROOT)/gsoap\
#	 -I$(SRCROOT)/gsoap/std -I$(SRCROOT)/../../CommonFiles


#INCLUDES += -I$(STLPORT_DIR) \
#            -I$(GSOAP_DIR)\
#            -I$(GSOAP_DIR)/std\
#            -I../include \
#	    -I../../CommonFiles  

DEFS += -DUNICODE 

INCLUDES += -I$(STLPORT_INCLUDE_DIR)\
            -I${BOOST_INCLUDE_DIR}\
            -I$(CoMT_INCLUDE_DIR)\
            -I.. -I../std -I../include -I$(GSOAP_DIR) -I$(GSOAP_DIR)/std\
            -I../openssl/include\
	    -I../../../CommonFiles
	    	    	
