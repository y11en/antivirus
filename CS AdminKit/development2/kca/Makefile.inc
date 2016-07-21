# -*- Makefile -*- Time-stamp: <03/07/11 13:08:45 ptr>
# $Id: Makefile.inc,v 1.20 2004/04/08 09:20:26 ptr Exp $

LIBNAME = klcskca
MAJOR = 0
MINOR = 1
PATCH = 0
SRC_CC = aud/auditsource.cpp\
	prci/cmp_helpers.cpp prci/componentinstanceimpl.cpp prci/componentinstancebaseimpl prci/prci_auth.cpp\
		 prci/componentidimpl.cpp\
		 prci/prci_inprocess.cpp prci/sink.cpp prci/soapapi.cpp\
		 prci/taskdata.cpp\
		 init/init_kca.cpp\
	prcp/componentproxy.cpp prcp/proxybase.cpp\
	pres/eventsstorage.cpp pres/eventsstorageserver.cpp\
		 pres/prespersistenteventlist.cpp\
		 pres/pressubscriptionlist.cpp ../common/policy.cpp\    common/locevents.cpp common/fmtmsg.cpp common/normaddress.cpp common/error2params.cpp \
	prss/distr_pol.cpp prss/helpers.cpp prss/nameconvertor.cpp\
		 prss/ops_pol.cpp prss/paths.cpp prss/profiles.cpp\
		 prss/settingsstorage.cpp prss/ss_bulkmodeimp.cpp\
		 prss/ss_props.cpp prss/ss_sync.cpp prss/store.cpp prss/ts_smart_rw.cpp\
		 prss/cxreplace.cpp\
	prts/taskinfo.cpp prts/tasksstorage.cpp prts/taskstorageserver.cpp\
	prts/TasksStorageLocalProxy.cpp prts/tsk_list.cpp\
	prts/prts_task_ser.cpp\
	prts2/ts_server2.cpp\
	contie/connectiontiedobjectshelperimp.cpp\
	jrnl/common.cpp jrnl/journalimpl.cpp\
		jrnl/journaltextimpl.cpp\
	ss_srv/ss_srv.cpp ss_srv/sssrv_proxy.cpp ss_srv/sssrv_serverimp.cpp\
		ss_srv/sssrv_soapapi.cpp\
	../kca/ft/file.cpp ../kca/ft/filereceiverbridge.cpp ../kca/ft/filesender.cpp ../kca/ft/filetransferimp.cpp\
		 ../kca/ft/filetransfersoapapi.cpp ../kca/ft/foldersync.cpp ../kca/ft/multicastsender.cpp\
	../nagent/naginst/cert_load.cpp\
	srvhrch/srvhrch_common_impl.cpp\
	../avp/text2bin.c
