#!/bin/sh 
#
############################################################
#
# KERNEL TEST
#
############################################################
# print usage
usage()
{
    echo "Usage: `basename $0` [options]"
    echo "options:"
    echo "  -l    log file name [$DEFAULT_LOG_FILE]"
    echo "  -a    append log"
    echo "  -v    explain what is being done"
    echo "  -h|?  help"
}
#
############################################################
#
cdir=`pwd`	           # save current directory
script_dir=`dirname $0`    # change current directory on script dirsctory
#
# include common scripts variables
cd $script_dir
KLAV_ROOT="./../.."        # klava
KLAV_ENV="$KLAV_ROOT/scripts/env.sh"
. "$KLAV_ENV"
cd $cdir
#
############################################################
# global test number
TEST_NUMBER=3
#
DEFAULT_LOG_FILE=$script_dir/"kernel.log"
LOG_FILE=$DEFAULT_LOG_FILE
#
#
LOG_FILE_APP=0
VERBOSE_FLG=0
VERBOSE_OPT=""
#
#
while getopts :l:avh? OPTION
do
    case $OPTION in
           l) LOG_FILE=$OPTARG
              ;;
           a) LOG_FILE_APP=1
              ;;
           v) VERBOSE_FLG=1
              VERBOSE_OPT=" -v"
              ;;
        \?|h) usage
              exit 1
              ;;
    esac
done
#
#
# initialize log file, verbose
#
set_verbose $VERBOSE_FLG
LOG_FILE=`get_fullname $LOG_FILE`
set_log $LOG_FILE
if test $LOG_FILE_APP -eq 0
then
    create_log
fi
#
vputline
puts "---------------------------------------------"
puts "[$TEST_NUMBER] Kernel test."
puts "---------------------------------------------"
#
###########################################################
# 1.) Tree search tests.
###########################################################
#
$script_dir/treesearchtest/tests/runtest.sh -a -l $LOG_FILE $VERBOSE_OPT
result=$?
#
if test $result -ne 0
then
    puts "[$TEST_NUMBER.1] Test error!"
    exit $result
else
    puts "[$TEST_NUMBER.1] Test passed."
fi
#
############################################################
# 2.) Region manager tests.
############################################################
#
vputline
puts "[$TEST_NUMBER.2] Test of the region manager."
#
$script_dir/rm_test/test/runtest.sh -a -l $LOG_FILE $VERBOSE_OPT
result=$?
if test $result -ne 0
then
    puts "[$TEST_NUMBER.2] Test error!"
    exit $result
else
    puts "[$TEST_NUMBER.2] Test passed."
fi
#
############################################################
# 3.) KLAV_IO test.
############################################################
#
vputline
puts "[$TEST_NUMBER.3] Test of the KLAV IO."
#
$script_dir/klaviotest/tests/runtest.sh -a -l $LOG_FILE $VERBOSE_OPT
result=$?
if test $result -ne 0
then
    puts "[$TEST_NUMBER.3] Test error!"
    exit $result
else
    puts "[$TEST_NUMBER.3] Test passed."
fi

#
############################################################
# 3.) klavstl test.
############################################################
#
vputline
puts "[$TEST_NUMBER.4] Test of the klavstl."
#
$script_dir/klavstl_test/runtest.sh -a -l $LOG_FILE $VERBOSE_OPT
result=$?
if test $result -ne 0
then
    puts "[$TEST_NUMBER.3] Test error!"
    exit $result
else
    puts "[$TEST_NUMBER.3] Test passed."
fi

############################################################
# The end test.
exit $result
#