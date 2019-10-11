#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/../deps/configs.sh

NSERVERS=10
<<<<<<< HEAD
QPS=1000
WARMUPREQS=10
REQUESTS=10000
=======
QPS=2500
WARMUPREQS=1000
REQUESTS=30000
>>>>>>> 19abdaf68cf2b8ebe9048823865311370d7cd7f9

TBENCH_MAXREQS=${REQUESTS} TBENCH_WARMUPREQS=${WARMUPREQS} \
    chrt -r 99 ./xapian_networked_server -n ${NSERVERS} -d ${DATA_ROOT}/xapian/wiki \
    -r 1000000000 &
echo $! > server.pid
energyStart0=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj)
energyStart1=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:1/energy_uj)
energyStart=$((energyStart1+energyStart0))
sleep 5 # Wait for server to come up
#taskset -apc 0,2,4,6,8,10,12,14,16,18,20,22 $(cat server.pid)
sudo wrmsr -a 0x199 0x1200

<<<<<<< HEAD
TBENCH_QPS=${QPS} TBENCH_CLIENT_THREADS=4 TBENCH_MINSLEEPNS=200000 \
=======
TBENCH_QPS=${QPS} TBENCH_CLIENT_THREADS=4 TBENCH_MINSLEEPNS=50000 \
>>>>>>> 19abdaf68cf2b8ebe9048823865311370d7cd7f9
    TBENCH_TERMS_FILE=${DATA_ROOT}/xapian/terms.in \
    chrt -r 99 ./xapian_networked_client &
  
echo $! > client.pid
#taskset -apc 1,3,5,7 $(cat client.pid)

wait $(cat client.pid)
energyEnd0=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj)
energyEnd1=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj)
energyEnd=$((energyEnd1 + energyEnd0))
consumption=$((energyEnd - energyStart))

echo "Consumption for Hurryup was $consumption"

# Clean up
./kill_networked.sh
rm server.pid client.pid
