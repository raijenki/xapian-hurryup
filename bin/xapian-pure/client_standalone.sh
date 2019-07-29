#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/../deps/configs.sh

NSERVERS=1
QPS=1000
WARMUPREQS=1000
REQUESTS=10000

TBENCH_QPS=${QPS} TBENCH_MINSLEEPNS=100000 TBENCH_CLIENT_THREADS=2 \
    TBENCH_SERVER=129.114.108.163 TBENCH_SERVER_PORT=8080 TBENCH_TERMS_FILE=${DATA_ROOT}/xapian/terms.in \
    chrt -r 99 ./xapian_networked_client &

echo $! > client.pid



wait $(cat client.pid)

# Clean up
./kill_standalone.sh
rm client.pid
