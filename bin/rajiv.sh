#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/../deps/configs.sh

export TBENCH_INPUT=/home/cc/hurryup-dvfs/bin/xapian.input
export TBENCH_OUTPUT=/home/cc/tmp.txt
export TBENCH_CPID=/home/cc/hurryup-dvfs/xapians/xapian-pure/server.pid
export TBENCH_KEYSERVICE=/home/cc/keyservice.txt
export TBENCH_QPS=5000
export TBENCH_MINSLEEPNS=100000
export TBENCH_TERMS_FILE=/home/cc/inputs/xapian/terms.in
export TBENCH_RANDSEED=3
export TBENCH_MAXREQS=150000
export TBENCH_WARMUPREQS=1000
export TBENCH_CLIENT_THREADS=10

NSERVERS=8

./xapian_integrated -n ${NSERVERS} -d /home/cc/inputs/xapian/wiki -r 100000000
echo $! > server.pid
#sudo ./kill_standalone.sh
#taskset -apc 12 $(cat server.pid)

