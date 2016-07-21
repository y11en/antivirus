#!/bin/bash
#
usage()
{
    echo "Usage: `basename $0` [options]"
    echo "options:"
    echo "  -e    make etalon file"
    echo "  -l    log file name [$DEFAULT_LOG_FILE]"
    echo "  -a    append log"
    echo "  -v    explain what is being done"
    echo "  -h|?  help"
}
######################################################################
#
ELOG_FILE_EXT="elog"	# the etalon log file extention
NLOG_FILE_EXT="log"	# the native log file extention
DEFAULT_LOG_FILE="rm_test.$NLOG_FILE_EXT"
LOG_FILE=$DEFAULT_LOG_FILE
#
MAKE_ETALON=0
LOG_FILE_APP=0
VERBOSE_FLG=0
#
while getopts :l:eavh? OPTION
do
    case $OPTION in
           l) LOG_FILE=$OPTARG;;
           a) LOG_FILE_APP=1;;
           e) MAKE_ETALON=1;;
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
KLAV_ROOT="./../../../.."  # klava root
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
EXEC_MODULE=${KLAV_BIN_DIR}/rm_test.exe
if [ ! -f "${EXEC_MODULE}" ]
then
    puts "Fatal error: Module ${EXEC_MODULE} not found !"
    exit 1
fi
#
# !!!!!!!!!!!!!!!!
cd $script_dir
# !!!!!!!!!!!!!!!!
#
# change test data file access permissions
chmod u+w data1
#
result=0
TEST_LIST="tests.lst"
#
while read test_name test_parms
do
    if [ "${test_name}" = "" -o "${test_name}" = "//" ]
    then
        continue
    fi

    if test "${MAKE_ETALON}" -eq 1
    then
        OUT_LOG_FILE=${test_name}.${ELOG_FILE_EXT}
    else
        OUT_LOG_FILE=${test_name}.${NLOG_FILE_EXT}
    fi
    #
    >${OUT_LOG_FILE}
    #
    ${EXEC_MODULE} ${test_parms} >>${OUT_LOG_FILE}
    #
    result=$?
    if [ $result -ne 0 ];
    then
        puts "Error processing test: $test_name"
        break
    fi

    if test ${MAKE_ETALON} -eq 1
    then
        vputs "Etalon ${test_name} created."
        continue
    fi
    #
    # compare log with etalon
    ETALON_FILE=${test_name}.${ELOG_FILE_EXT}
    # file exist?
    if test -f "${ETALON_FILE}"
    then
        cmp ${OUT_LOG_FILE} ${ETALON_FILE}
        result=$?
        #
        if test $result -eq 0
        then
            vputs "[${test_name}] Test passed!"
            rm ${OUT_LOG_FILE}
        elif test $result -eq 1
        then
            puts "[${test_name}] Files differ. Test error!"
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
done <${TEST_LIST}
#
# restore test data file access permissions
chmod u-w data1
#
# !!!!!!!!!!!!!!!!
cd $cdir
# !!!!!!!!!!!!!!!!
#
if [ $result -eq 0 ]
then
    puts "TESTS PASSED"
    exit 0
fi
#
puts "TESTS FAILED"
exit 1
