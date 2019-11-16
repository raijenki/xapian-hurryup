#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/../deps/configs.sh

NSERVERS=2
QPS=4000
WARMUPREQS=1000
REQUESTS=10000

TBENCH_MAXREQS=${REQUESTS} TBENCH_WARMUPREQS=${WARMUPREQS} \
    chrt -r 99 ./xapian_networked_server -n ${NSERVERS} -d ${DATA_ROOT}/xapian/wiki \
    -r 1000000000 &
echo $! > server.pid
sleep 5 # Wait for server to come up
taskset -apc 12 $(cat server.pid)

TBENCH_QPS=${QPS} TBENCH_CLIENT_THREADS=2 TBENCH_MINSLEEPNS=100000 \
    TBENCH_TERMS_FILE=${DATA_ROOT}/xapian/terms.in \
    chrt -r 99 ./xapian_networked_client &
  
echo $! > client.pid
taskset -apc 13 $(cat client.pid)

wait $(cat client.pid)

# Clean up
./kill_networked.sh
rm server.pid client.pid
