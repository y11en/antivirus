#!/bin/sh 
#
############################################################
#
# TREE SEARCH LIBRARY TEST.
#
############################################################
#
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
############################################################
#
# global test number
TEST_NUMBER=3
#
DEFAULT_LOG_FILE="treesearchtest.log"
LOG_FILE=$DEFAULT_LOG_FILE
#
LOG_FILE_APP=0
VERBOSE_FLG=0
VERBOSE_OPT=""
#
#
while getopts :l:avh? OPTION
do
    case $OPTION in
           l) LOG_FILE=$OPTARG;;
           a) LOG_FILE_APP=1;;
           v) VERBOSE_FLG=1
              VERBOSE_OPT=" -v"
              ;;
        \?|h) usage
              exit 1
              ;;
    esac
done
#
cdir=`pwd`                # save current directory
script_dir=`dirname $0`   # get script directory
#
# include common scripts variables
cd $script_dir            # change current directory on script directory
KLAV_ROOT="./../../../.." # klava root
KLAV_ENV="$KLAV_ROOT/scripts/env.sh"
. "$KLAV_ENV"
cd $cdir
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
puts "[$TEST_NUMBER.1] Test of the tree search library."
#
############################################################
# Handmade signatures test.
############################################################
#
$script_dir/handsig.sh -c rf -d $script_dir/handsig.data -a -l $LOG_FILE $VERBOSE_OPT
result=$?
if test $result -ne 0
then
    exit $result
fi
#
############################################################
# Exhaustive scaning test files.
############################################################
#
$script_dir/handsig_se.sh -d $script_dir/handsig_se.data -a -l $LOG_FILE $VERBOSE_OPT
result=$?
if test $result -ne 0
then
    exit $result
fi
#
############################################################
# change the first 4 bytes of signatures.
############################################################
#
$script_dir/handsig_st.sh -d $script_dir/handsig_st.data -a -l $LOG_FILE $VERBOSE_OPT
result=$?
if test $result -ne 0
then
    exit $result
fi
############################################################
# Random generation signatures test.
############################################################
#
$script_dir/randsig.sh -a -l $LOG_FILE $VERBOSE_OPT
result=$?
#
#
exit $result
