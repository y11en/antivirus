#!/bin/bash
#
usage ()
{
    echo "Usage: `basename $0` [options]"
    echo "options:"
    echo "  -l    log file name [${DEFAULT_LOG_FILE}]"
    echo "  -a    append log"
    echo "  -v    explain what is being done"
    echo "  -h|?  help"
}
#
prepare_result ()
{
    local result=$1
    if [ $result -ne 0 ];
    then
        puts "TESTS FAILED"
        cd $cdir
        exit 1
    fi
    remove_file_by_mask "." "*.tmp"
}
#
remove_file ()
{
    local file=$1

    if [ -f "$file" ]; then
        chmod 770 "$file"
        if [ $? -ne 0 ]; then
           puts "changing mode of file $file failed."
           exit 1
        fi

        rm "$file"
        if [ $? -ne 0 ]; then
            puts "error deleting file $file"
            exit 1
        else
            vputs "deleted : $file"
        fi
    fi

    return 0
}
#
remove_file_by_mask ()
{
    local path=$1
    local mask=$2
    local file
    for file in `/usr/bin/find "$path" -maxdepth 1 -iname "$mask" -type f`
    do
	if test "$OSTYPE" = "cygwin"; then
            file=`cygpath -m -a "$file"`	
	fi
        remove_file $file
    done
}
############################################################
#
DEFAULT_LOG_FILE="io_test.log"
LOG_FILE=${DEFAULT_LOG_FILE}
LOG_FILE_APP=0
VERBOSE_FLG=0
#
while getopts :l:avh? OPTION
do
    case $OPTION in
           l) LOG_FILE=$OPTARG;;
           a) LOG_FILE_APP=1;;
           v) VERBOSE_FLG=1;;
        \?|h) usage
              exit 1;;
    esac
done
#
############################################################
# prologue
#
cdir=`pwd`
script_dir=`dirname $0`
#
# include common scripts variables
cd $script_dir
KLAV_ROOT="./../../.."  # klava root
KLAV_ENV=${KLAV_ROOT}/scripts/env.sh
. ${KLAV_ENV}
cd $cdir
#
# initialize log file, verbose
#
set_verbose ${VERBOSE_FLG}
LOG_FILE=`get_fullname ${LOG_FILE}`
set_log ${LOG_FILE}
if test "${LOG_FILE_APP}" -eq 0
then
    create_log
fi
#
#
EXEC_MODULE=${KLAV_BIN_DIR}/klavstl_test.exe
if [ ! -f "${EXEC_MODULE}" ]
then
    puts "Fatal error: Module ${EXEC_MODULE} not found !"
    exit 1
fi
#
cd $script_dir
remove_file_by_mask "." "*.tmp"
#
vputs "--- running test: klavstl_test"
${EXEC_MODULE} >>${LOG_FILE}
prepare_result $?
#
cd $cdir
puts "TESTS PASSED"
exit 0
