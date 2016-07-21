#!/bin/sh 
#
############################################################
#
# TREE SEARCH LIBRARY TEST.
# RANDOM GENERATION SIGNATURES.
#
############################################################
# print usage
usage()
{
    echo "Usage: `basename $0` [options]"
    echo "options:"
    echo "  -d    $ROOT_DIR_COMMENT [$DEFAULT_ROOT_DIR]"
    echo "  -c    $SIG_COUNT_COMMENT [$DEFAULT_SIG_COUNT]"
    echo "  -n    $MIN_SIG_SZ_COMMENT [$DEFAULT_MIN_SIG_SZ]"
    echo "  -x    $MAX_SIG_SZ_COMMENT [$DEFAULT_MAX_SIG_SZ]"
    echo "  -o    $REPEATER_COMMENT [$DEFAULT_REPEATER]"
    echo "  -r    $RANDOM_START_COMMENT"
    echo "  -l    $LOG_FILE_COMMENT [$DEFAULT_LOG_FILE]"
    echo "  -a    append log"
    echo "  -v    explain what is being done"
    echo "  -h|?  help"
}
######################################################################
chk_param()
{
    if it_is_number $SIG_COUNT
    then :
    else
        echo "You entered a non-numeric SIG_COUNT."
        return 1
    fi
    #
    if it_is_number $MIN_SIG_SZ
    then :
    else
        echo "You entered a non-numeric MIN_SIG_SZ."
        return 1
    fi
    #
    if it_is_number $MAX_SIG_SZ
    then :
    else
        echo "You entered a non-numeric MAX_SIG_SZ."
        return 1
    fi
    #
    if it_is_number $REPEATER
    then :
    else
        echo "You entered a non-numeric REPEATER."
        return 1
    fi
    #
    if [ "$SIG_COUNT" -lt "0" ]
    then
        echo "Wrong number of signature!"
        return 1
    fi
    return 0
}
######################################################################
#
cdir=`pwd`                # save current directory
script_dir=`dirname $0`   # get script directory
#
# include common scripts variables and functions
cd $script_dir            # change current directory on script directory
KLAV_ROOT="./../../../.." # klava
KLAV_ENV="$KLAV_ROOT/scripts/env.sh"
. "$KLAV_ENV"
cd $cdir                  # restore to current directory
#
EXEC_MODULE="$KLAV_BIN_DIR/treesearchtest"
#
if test ! -f $EXEC_MODULE
then
    puts "Fatal error: Module $EXEC_MODULE not found!"
    exit 1
fi  
#
# Test directory name.
DEFAULT_ROOT_DIR=$script_dir"/randsig.data"
ROOT_DIR=$DEFAULT_ROOT_DIR
ROOT_DIR_COMMENT="root dir for testing"
# Log file name.
DEFAULT_LOG_FILE="randsig.log"
LOG_FILE=$DEFAULT_LOG_FILE
LOG_FILE_COMMENT="log file name"
# Signature count generating.
DEFAULT_SIG_COUNT=100
SIG_COUNT=$DEFAULT_SIG_COUNT
SIG_COUNT_COMMENT="number of signature"
# Minimum signature size
DEFAULT_MIN_SIG_SZ=4
MIN_SIG_SZ=$DEFAULT_MIN_SIG_SZ
MIN_SIG_SZ_COMMENT="minimum signature size"
# Maximum signature size
DEFAULT_MAX_SIG_SZ=512
MAX_SIG_SZ=$DEFAULT_MAX_SIG_SZ
MAX_SIG_SZ_COMMENT="maximum signature size"
# How mach generate signature from same offset.
DEFAULT_REPEATER=1
REPEATER=$DEFAULT_REPEATER
REPEATER_COMMENT="number of signatures generate from same offset"
# Set a random starting point. 0-no, 1-yes
DEFAULT_RANDOM_START=0
RANDOM_START=$DEFAULT_RANDOM_START
RANDOM_START_COMMENT="set a random starting point"
#
VERBOSE_FLG=0
LOG_FILE_APP=0
#
#
while getopts :d:l:c:n:x:o:e:varh? OPTION
do
    case $OPTION in
           d) ROOT_DIR=$OPTARG;;
           l) LOG_FILE=$OPTARG;;
           c) SIG_COUNT=$OPTARG;;
           n) MIN_SIG_SZ=$OPTARG;;
           x) MAX_SIG_SZ=$OPTARG;;
           o) REPEATER=$OPTARG;;
           r) RANDOM_START=1;;
           v) VERBOSE_FLG=1
              VERBOSE_OPT=" -v"
              ;;
           a) LOG_FILE_APP=1
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
error_msg=`chk_param`
result=$?
if test $result -eq 1
then
   puts $error_msg
   exit $result
fi
#
# Write banner in log file.
puts "Random generation signature test."
#
#
TMP_LOG_FILE="$ROOT_DIR/$DEFAULT_LOG_FILE"
FILE_MASK="*.data"    # scaning file mask
#
#
ffpath=`find_file_path find`
#
for fname in `$ffpath/find $ROOT_DIR -depth -name "$FILE_MASK" -type f`
do
    >$TMP_LOG_FILE
    #
    $EXEC_MODULE -g "$fname" $SIG_COUNT $MIN_SIG_SZ $MAX_SIG_SZ $REPEATER $RANDOM_START -s "$fname" >>$TMP_LOG_FILE
    result=$?
    if test $result -eq 2
    then
        puts "[$fname] Test error! See log $TMP_LOG_FILE for detail."
        break
    else
        result=0
        vputs "[$fname] Test passed ok!"
    fi
    #
    if test -f $TMP_LOG_FILE
    then
        rm $TMP_LOG_FILE
    fi
done
#
vputline
#
exit $result
