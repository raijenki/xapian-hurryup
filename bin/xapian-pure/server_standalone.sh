#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/../deps/configs.sh

NSERVERS=1
QPS=2000
WARMUPREQS=1000
REQUESTS=10000

TBENCH_MAXREQS=${REQUESTS} TBENCH_WARMUPREQS=${WARMUPREQS} TBENCH_SERVER=129.114.108.163 TBENCH_SERVER_PORT=80 \
    chrt -r 99 ./xapian_networked_server -n ${NSERVERS} -d ${DATA_ROOT}/xapian/wiki \
    -r 1000000000 
echo $! > server.pid
taskset -apc 12 $(cat server.pid)

rm server.pid
