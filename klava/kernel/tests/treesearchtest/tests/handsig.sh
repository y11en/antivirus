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
    echo "  -c    command {rf | ...}"
    echo "  -d    dir for testing [$DEFAULT_TEST_DIR]"
    echo "  -e    make etalon file"
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
COMMAND="rf"
COMMAND_FLG=0
# Test directory name.
DEFAULT_TEST_DIR="$script_dir"
TEST_DIR=$DEFAULT_TEST_DIR
#
DEFAULT_LOG_FILE="handsig.log"
LOG_FILE=$DEFAULT_LOG_FILE
#
MAKE_ETALON=0
LOG_FILE_APP=0
VERBOSE_FLG=0
VERBOSE_OPT=" -v"
#
#
while getopts :c:d:l:eavh? OPTION
do
    case $OPTION in
           c) COMMAND=$OPTARG
              COMMAND_FLG=1
              ;;
           l) LOG_FILE=$OPTARG;;
           a) LOG_FILE_APP=1;;
           d) TEST_DIR=$OPTARG;;
           e) MAKE_ETALON=1;;
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
#if [ $COMMAND_FLG -eq 0 ]
#then
#    puts "Fatal error: don't specify command !"
#    exit 1
#fi
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
if test $MAKE_ETALON -eq 1
then
    puts "Create etalon tests."
else
    puts "Testing handmade signature. [COMMAND = -$COMMAND] [DIR = $TEST_DIR]"
fi
#
######################################################################
#
HM_FILE_MASK="*.sig"	# cheking file mask
ELOG_FILE_EXT="elog"	# the etalon log file extention
NLOG_FILE_EXT="log"	# the native log file extention
#
#
ffpath=`find_file_path find`
#
for fname in `$ffpath/find $TEST_DIR -depth -name $HM_FILE_MASK -type f`
do
    if [ ! -f $fname ]
    then
        continue
    fi
    #
    prefix_name=$fname
    if test $MAKE_ETALON -eq 1
    then # set option - 'making etalon log file'
        OUT_LOG_FILE="$prefix_name.$ELOG_FILE_EXT"
    else # set option - 'making log file'
        OUT_LOG_FILE="$prefix_name.$NLOG_FILE_EXT"
    fi
    #
    >$OUT_LOG_FILE
    #
    $EXEC_MODULE -tfn yes -$COMMAND "$fname" -s "$fname" >>$OUT_LOG_FILE
    # 
    if test $MAKE_ETALON -eq 1
    then
        vputs "Etalon $fname created."
        continue
    fi
    #
    # compare log with etalon
    ETALON_FILE="$prefix_name.$ELOG_FILE_EXT"
    # file exist?
    if test -f $ETALON_FILE
    then
        cmp $OUT_LOG_FILE $ETALON_FILE
        result=$?
        #
        if test $result -eq 0
        then
            vputs "[$fname] Test passed ok!"
            rm $OUT_LOG_FILE
        elif test $result -eq 1
        then
            puts "[$fname] Files differ. Test error!"
            break;
        elif test $result -eq 2
        then
            puts "Trouble on compare files!"
            break;
        fi
    else
        puts "Etalon file $ETALON_FILE not found!"
        result=1
        break;
    fi
done
#
vputline
#
exit $result
