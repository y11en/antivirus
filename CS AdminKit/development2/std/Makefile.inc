# -*- Makefile -*- Time-stamp: <03/07/11 13:08:45 ptr>
# $Id: Makefile.inc,v 1.20 2004/04/08 09:20:26 ptr Exp $

LIBNAME = klcsstd
MAJOR = 0
MINOR = 1
PATCH = 0
SRC_CC = base/klbase.cpp base/baseobj.cpp\
	 conv/klconv.cpp conv/_conv.cpp conv/_print.cpp\
	../common/bin2hex.cpp\
	fmt/stdfmtstr.cpp\
	 err/error.cpp err/error2string.cpp err/errlocmodule.cpp\
	 gsoap/stubrouter.cpp\
	 init/init_std.cpp\
	 io/klio.cpp io/del_path.cpp\
	 memory/klmem.cpp\
	 par/make_env.cpp par/ops_par.cpp par/paramsimpl.cpp\
		par/parbinser.cpp par/parserialize.cpp par/parstreams.cpp\
		par/s_params.cpp par/ser.cpp\
		par/_env.cpp par/parstrdata.cpp par/valuesimpl.cpp\
	par/paramslogger.cpp \
	 sch/commonsch.cpp sch/everydayscheduleimp.cpp sch/everymonthscheduleimp.cpp\
		sch/everyweekscheduleimp.cpp sch/millisecondsscheduleimp.cpp\
		sch/weeksdaysscheduleimp.cpp\
		sch/private.cpp sch/schedule.cpp sch/schedulerimp.cpp\
		sch/secondsscheduleimp.cpp sch/taskclasseslist.cpp sch/taskimp.cpp\
		sch/tasklist.cpp sch/tasksiteratorimp.cpp\
	 sign/chksgn.cpp\
	 thr/sync.cpp \
	 ../std/thr/thread.cpp \
	 thrstore/threadstore.cpp\
	 tmstg/timeoutstore.cp\
	 tp/threadspool.cpp tp/threadspoolmanager.cpp tp/workingthread.cpp\
	 trc/trace.cpp\
	 time/kltime.cpp\
	 redir/gen_uuid.cpp redir/redir.cpp redir/unpack.cpp redir/macaddr.cpp \
	 rpt/globalreporter.cpp\
	 stress/st_prefix.cpp\
	 hstd/hostdomain.cpp\
	 klcspwd/crypto.cpp klcspwd/klcspwd.cpp klcspwd/klcspwd_inst.cpp klcspwd/strg_unix.cpp \
	 wnf/ComputerInfo.cpp\
	 conv/wcharcrt.cpp\
	 ../gsoap/std/stub/gsoapstubs.cpp ../gsoap/common/namespaces.cpp\
	 	../gsoap/std/soapC.cpp ../gsoap/std/soapClient.cpp\
		 ../gsoap/std/soapServer.cpp ../gsoap/stdsoap2.cpp\
	 ../include/boost/src/thread/src/_thread.cpp\
		../include/boost/src/thread/src/condition.cpp\
		../include/boost/src/thread/src/exceptions.cpp\
		../include/boost/src/thread/src/mutex.cpp\
		../include/boost/src/thread/src/once.cpp\
		../include/boost/src/thread/src/recursive_mutex.cpp\
	 	../include/boost/src/thread/src/threadmon.cpp\
		../include/boost/src/thread/src/xtime.cpp\
	 ../include/boost/src/filesystem/src/convenience.cpp\
	 	../include/boost/src/filesystem/src/exception.cpp\
		../include/boost/src/filesystem/src/operations_posix_windows.cpp\
	 	../include/boost/src/filesystem/src/path_posix_windows.cpp\
	 ../avp/text2bin.c \
	./conf/std_parse_conf.cpp
