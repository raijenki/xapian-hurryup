#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/../deps/configs.sh

NSERVERS=12
WARMUPREQS=1000
REQUESTS=100000

TBENCH_MAXREQS=${REQUESTS} TBENCH_WARMUPREQS=${WARMUPREQS} \
    chrt -r 99 ./xapian_networked_server -n ${NSERVERS} -d ${DATA_ROOT}/xapian/wiki \
    -r 1000000000 &
echo $! > server.pid
wrmsr -a 0x199 0x1000 #put all the frequencies to 1.6Ghz
sleep 5 # Wait for server to come up


# Clean up
./kill_server.sh
rm server.pid 
