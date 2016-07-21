# -*- Makefile -*- Time-stamp: <02/12/26 23:03:35 ptr>
# $Id: mwccnlm.mak,v 1.1.1.1 2003/07/23 13:15:29 ptr Exp $

SRCROOT := ../../../explore
COMPILER_NAME = mwccnlm
STLPORT_DIR := ../../../Novell-STLP/STLport
CoMT_DIR := ../../../explore
BOOST_DIR := ../../../extern/boost
GSOAP_DIR := ../gsoap
CRYPTO_DIR:= ../openssl/crypto
include MakefileNW.inc
include ${SRCROOT}/Makefiles/top.mak

#INCLUDES += -I$(STLPORT_INCLUDE_DIR) -I../../CommonFiles -I.. -I../../../explore/include
INCLUDES += -I$(STLPORT_INCLUDE_DIR) -I../../../CommonFiles\
            -I${BOOST_INCLUDE_DIR}\
            -I$(CoMT_INCLUDE_DIR)\
            -I.. -I../include -I../include/std/conv -I$(GSOAP_DIR) -I$(GSOAP_DIR)/std\
            -I../openssl/include\
            -I$(CRYPTO_DIR)
	    	
