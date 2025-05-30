#!/bin/bash

# Author: Hubertus Franke  (frankeh@nyu.edu)

[[ ${#} -lt 2 ]] && echo "usage: $0 <outputdir> <scheduler+optionalargs>" && exit

INDIR=${INDIR:-.}
OUTDIR=$1
PROG=$2
shift 2
[[ ! -x ${PROG} ]]   && echo "program <$PROG> is not executable" && exit
[[ ! -d ${OUTDIR} ]] && echo "outputdir <$OUTDIR> does not exist"  && exit
[[ ! -d ${INDIR} ]]  && echo "inputdir <$INDIR> does not exist"  && exit

SCHED="${PROG} ${*}"

echo "outdir=<$OUTDIR> sched=<$SCHED>"

INS="`seq 0 10`"
INPRE="input"
OUTPRE="out"

SCHEDS="N S L C F"

########################## TIMER MONITOR ####################

TIMELIMIT=${TIMELIMIT:-0}   # 0: nolimit;  >0 kills process after this many seconds 
SPID=0        # process pid we are monitoring
TPID=0        # sleep process
MPID=$$       # this script
SPID_KILLED=0 # notification that the process got killed due to timeout
EXIT_NOW=0    # notification that ^C what hit

start_redirect() { exec 3>&2 ; exec 2> /dev/null;  }
stop_redirect()  { exec 2>&3 ; exec 3>&- ;         }
check_exit()     { [[ ${EXIT_NOW} == 1 ]] && exit; }

TimerOn()
{
    SPID_KILLED=0
    (sleep ${TIMELIMIT} ; if [[ "$?" == 0 ]]; then kill -SIGUSR1 $MPID; fi) &
    TPID=$! 
}

TimerOff()
{
    if [[ ${TPID} != 0 ]]; then
        SPROCS=$(ps h --ppid $TPID | awk '{print $1}')
        # kill the sleep process which results in TPID to terminate
        kill -9 ${SPROCS} 2>&1   # kills the <sleep> process
        wait ${SPROCS}
        wait ${TPID}
        TPID=0
    fi
}

TimerFires()
{
    ps -p ${SPID} | grep -v "^[ ]*PID" 2>&1 > /dev/null
    if [ $? == 0 ]; then
        start_redirect
        kill -9 ${SPID} > /dev/null 2>&1 
        stop_redirect
        SPID_KILLED=1
    fi
    TPID=0
}

GetMeOutOfHere()
{
    echo "INTERRUPTED -> TERMINATING"
    [[ ${TIMELIMIT} -eq 0 ]] && exit
    EXIT_NOW=1
    TimerFires
}

monitor()
{
    # first argument is where to redirect the output
    # remainder is the program + arguments to run
    OUTFILE=$1
    shift
    echo "$@"
    $@ > ${OUTFILE} &    # execute the program
    SPID=$!

    if [[ ${TIMELIMIT} -gt 0 ]]; then
        start_redirect; TimerOn
    fi

    wait ${SPID} 2>&1 > /dev/null 
    SPID=0

    if [[ ${TIMELIMIT} -gt 0 ]]; then
        TimerOff; stop_redirect
        [[ ${SPID_KILLED} == 1 ]] && echo "      Killed after ${TIMELIMIT} secs"
        check_exit
    fi
}

trap TimerFires     SIGUSR1
trap GetMeOutOfHere SIGINT

##########################  END TIMER MONITOR ####################

for f in ${INS}; do
	for s in ${SCHEDS}; do 
		monitor ${OUTDIR}/${OUTPRE}_${f}_${s} ${SCHED} -s${s} ${INDIR}/${INPRE}${f}
	done
done

