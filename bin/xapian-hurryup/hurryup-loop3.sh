#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/../deps/configs.sh

NSERVERS=10
QPS=1400
WARMUPREQS=1000
REQUESTS=30000
echo "Run , Difference" > energy.txt

for counter in {1..5..1}; 
	do
		cat /sys/devices/system/cpu/cpu10/cpufreq/stats/time_in_state >> USERSPACE-hurryup-timeState-start-${counter}.txt	
		TBENCH_MAXREQS=${REQUESTS} TBENCH_WARMUPREQS=${WARMUPREQS} chrt -r 99 ./xapian_networked_server -n ${NSERVERS} -d ${DATA_ROOT}/xapian/wiki -r 1000000000 &
		echo $! > server.pid
		energyStart=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj)	
		sleep 5 # Wait for server to come up
#taskset -apc 0,2,4,6,8,10,12,14,16,18,20,22 $(cat server.pid)
		#sudo wrmsr -a 0x199 0xe00

		TBENCH_QPS=${QPS} TBENCH_CLIENT_THREADS=4 TBENCH_MINSLEEPNS=50000 TBENCH_TERMS_FILE=${DATA_ROOT}/xapian/terms.in chrt -r 99 ./xapian_networked_client &
  
		echo $! > client.pid
#taskset -apc 1,3,5,7 $(cat client.pid)

		wait $(cat client.pid)
		energyEnd=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj)
		consumption=$((energyEnd - energyStart))
		cat /sys/devices/system/cpu/cpu10/cpufreq/stats/time_in_state >> USERSPACE-hurryup-timeState-end-${counter}.txt
		echo "$counter , $consumption" >> energy.txt

# Clean up
		./kill_networked.sh
		rm server.pid client.pid
		mv lats.bin ../../raw-results/09/USERSPACE-hurryup-start@1.8ghz-10servers-${counter}.bin
	done
mv USERSPACE-hurryup-timeState-* ../../raw-results/09/
mv energy.txt ../../raw-results/09/USERSPACE-energy-hurryup-start@1.8ghz-10servers.txt
