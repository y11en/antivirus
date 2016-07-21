# -*- Makefile -*- Time-stamp: <03/07/11 13:08:45 ptr>
# $Id: Makefile.inc,v 1.20 2004/04/08 09:20:26 ptr Exp $

LIBNAME = klcsstd
MAJOR = 0
MINOR = 1
PATCH = 0
SRC_CC = base/klbase.cpp base\baseobj.cpp\
 	 ../common/bin2hex.cpp\
 	 conv/klconv.cpp conv/_conv.cpp conv/_print.cpp conv/wcharcrt.cpp\
	 fmt/stdfmtstr.cpp\
	 err/error.cpp err/error2string.cpp err/errlocmodule.cpp\
	 gsoap/stubrouter.cpp\
	 init/init_std.cpp\
	 io/klio.cpp  io/del_path.cpp\
	 memory/klmem.cpp\
	 par/make_env.cpp par/ops_par.cpp par/paramsimpl.cpp\
		par/parbinser.cpp par/parserialize.cpp par/parstreams.cpp\
		par/s_params.cpp par/ser.cpp par/paramslogger.cpp\
		par/_env.cpp par/parstrdata.cpp par/valuesimpl.cpp\
	 sch/commonsch.cpp sch/everydayscheduleimp.cpp sch/everymonthscheduleimp.cpp\
		sch/everyweekscheduleimp.cpp sch/millisecondsscheduleimp.cpp\
		sch/private.cpp sch/schedule.cpp sch/schedulerimp.cpp\
		sch/secondsscheduleimp.cpp sch/taskclasseslist.cpp sch/taskimp.cpp\
		sch/tasklist.cpp sch/tasksiteratorimp.cpp\
	 sign/chksgn.cpp\
	 thr/sync.cpp thr/threadnw.cpp\
	 thrstore/threadstore.cpp\
	 tmstg/timeoutstore.cp\
	 tp/threadspool.cpp tp/threadspoolmanager.cpp tp/workingthread.cpp\
	 trc/trace.cpp\
	 time/kltime.cpp\
	 redir/gen_uuid.cpp redir/redir.cpp redir/unpack.cpp\
	 stress/st_prefix.cpp\
	 hstd/hostdomain.cpp\
	 klcspwd/klcspwd_fake.cpp\
	 klcspwd/klcspwd.cpp\
	 ../gsoap/std/stub/gsoapstubs.cpp ../gsoap/common/namespaces.cpp\
	 	../gsoap/std/soapC.cpp ../gsoap/std/soapClient.cpp\
		 ../gsoap/std/soapServer.cpp ../gsoap/stdsoap2.cpp\
	 ../../../CommonFiles/Stuff/text2bin.c\
	 rpt/globalreporter.cpp\
	 wnf/ComputerInfo.cpp
SRC_C = md5/md5nw.c

