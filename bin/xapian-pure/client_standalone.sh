#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/../deps/configs.sh

QPS=800
THREADS=12
SERVER_IP=129.114.108.163
SERVER_PORT=8080

TBENCH_QPS=${QPS} TBENCH_CLIENT_THREADS=${THREADS} TBENCH_MINSLEEPNS=100000 \
    TBENCH_SERVER=${SERVER_IP} TBENCH_SERVER_PORT=${SERVER_PORT} TBENCH_TERMS_FILE=${DATA_ROOT}/xapian/terms.in \
    chrt -r 99 ./xapian_networked_client &
  
echo $! > client.pid
#taskset -apc 13 $(cat client.pid)
wait $(cat client.pid)
# Clean up
./kill_client.sh
