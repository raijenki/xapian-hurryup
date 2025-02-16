#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/../deps/configs.sh

NSERVERS=1
QPS=400
WARMUPREQS=1000
REQUESTS=100000

TBENCH_MAXREQS=${REQUESTS} TBENCH_WARMUPREQS=${WARMUPREQS} \
    chrt -r 99 ./xapian_networked_server -n ${NSERVERS} -d ${DATA_ROOT}/xapian/wiki \
    -r 1000000000 &
echo $! > server.pid
taskset -apc 12 $(cat server.pid)
sudo wrmsr -a 0x199 0x1a00 #1.2GHz
sleep 5 # Wait for server to come up

TBENCH_QPS=${QPS} TBENCH_MINSLEEPNS=10000 TBENCH_CLIENT_THREADS=2 \
    TBENCH_TERMS_FILE=${DATA_ROOT}/xapian/terms.in \
    chrt -r 99 ./xapian_networked_client &
echo $! > client.pid
taskset -apc 13,15 $(cat client.pid)



wait $(cat client.pid)

# Clean up
./kill_networked.sh
rm server.pid client.pid
