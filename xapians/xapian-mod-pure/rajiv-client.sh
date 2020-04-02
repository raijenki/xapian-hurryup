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
export TBENCH_MAXREQS=100000
export TBENCH_WARMUPREQS=1000
export TBENCH_CLIENT_THREADS=2

NSERVERS=10

numactl --cpunodebind=1 ./xapian_networked_client -n ${NSERVERS} -d /home/cc/inputs/xapian/wiki -r 4000000 &
echo $! > client.pid
#sudo ./kill_standalone.sh
#taskset -apc 12 $(cat server.pid)

