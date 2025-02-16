#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/../deps/configs.sh

NSERVERS=10
<<<<<<< HEAD
QPS=4000
=======
QPS=1600
>>>>>>> 916f2c8b8c75cc1047828d3364991b2d67f0edc0
WARMUPREQS=1000
REQUESTS=40000

TBENCH_MAXREQS=${REQUESTS} TBENCH_WARMUPREQS=${WARMUPREQS} \
    chrt -r 99 ./xapian_networked_server -n ${NSERVERS} -d ${DATA_ROOT}/xapian/wiki \
    -r 1000000000 &
echo $! > server.pid
sleep 5 # Wait for server to come up
#taskset -apc 0,2,4,6,8,10,12,14,16,18,20,22 $(cat server.pid)

<<<<<<< HEAD
TBENCH_QPS=${QPS} TBENCH_CLIENT_THREADS=4 TBENCH_MINSLEEPNS=100000 \
=======
TBENCH_QPS=${QPS} TBENCH_CLIENT_THREADS=4 TBENCH_MINSLEEPNS=50000 \
>>>>>>> 916f2c8b8c75cc1047828d3364991b2d67f0edc0
    TBENCH_TERMS_FILE=${DATA_ROOT}/xapian/terms.in \
    chrt -r 99 ./xapian_networked_client &
  
echo $! > client.pid
taskset -apc 1,3,5,7 $(cat client.pid)

wait $(cat client.pid)

# Clean up
./kill_networked.sh
rm server.pid client.pid
