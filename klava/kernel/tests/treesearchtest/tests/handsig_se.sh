#!/bin/sh 
#
############################################################
#
# TREE SEARCH LIBRARY TEST.
# HANDMADE SIGNATURES.
#
############################################################
#
# print usage
usage()
{
    echo "Usage: `basename $0` [options]"
    echo "options:"
    echo "  -d    root dir for testing [$DEFAULT_TEST_DIR]"
    echo "  -l    log file name [$DEFAULT_LOG_FILE]"
    echo "  -a    append log"
    echo "  -v    explain what is being done"
    echo "  -h|?  help"
}
######################################################################
#
cdir=`pwd`		# save current directory
script_dir=`dirname $0`	# get script directory
#
#
# Perform command
COMMAND="se"
#
# Test directory name.
DEFAULT_TEST_DIR="$script_dir"
TEST_DIR=$DEFAULT_TEST_DIR
#
DEFAULT_LOG_FILE="handsig_se.log"
LOG_FILE=$DEFAULT_LOG_FILE
#
LOG_FILE_APP=0
VERBOSE_FLG=0
VERBOSE_OPT=" -v"
#
#
while getopts :d:l:avh? OPTION
do
    case $OPTION in
           d) TEST_DIR=$OPTARG;;
           l) LOG_FILE=$OPTARG;;
           a) LOG_FILE_APP=1;;
           v) VERBOSE_FLG=1;;
        \?|h) usage
              exit 1
              ;;
    esac
done
#
#
# include common scripts variables and functions
cd $script_dir               # change current directory on script directory
KLAV_ROOT="./../../../.."    # klava
KLAV_ENV="$KLAV_ROOT/scripts/env.sh"
. "$KLAV_ENV"
cd $cdir                     # restore to current directory
#
EXEC_MODULE="$KLAV_BIN_DIR/treesearchtest"
#
if [ ! -f $EXEC_MODULE ]
then
    puts "Fatal error: Module $EXEC_MODULE not found !"
    exit 1
fi
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
# Write banner in log file.
puts "Testing exhaustive file scanning."
#
######################################################################
#
TEST_FILE_MASK="*.sig"	# testing file mask
#
ffpath=`find_file_path find`
#
for fname in `$ffpath/find $TEST_DIR -depth -name $TEST_FILE_MASK -type f`
do
    if [ ! -f $fname ]
    then
        continue
    fi
    #
    $EXEC_MODULE -$COMMAND "$fname" "$fname" >>$LOG_FILE
    # 
    result=$?
    if test $result -eq 2
    then
        puts "[$fname] Test error!"
        break
    else
        result=0
        vputs "[$fname] Test passed ok!"
    fi
done
#
vputline
#
exit $result
