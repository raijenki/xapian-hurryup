#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/../deps/configs.sh

NSERVERS=12
WARMUPREQS=5000
PROCESSTIME=60

<<<<<<< HEAD
#for queryPerSecond in {120..1440..120}
for queryPerSecond in {1560..3000..120}
=======
for queryPerSecond in {120..1420..120}
>>>>>>> 916f2c8b8c75cc1047828d3364991b2d67f0edc0
do
	REQUESTS=$((queryPerSecond * PROCESSTIME))
	echo "Run , Difference" > energy-pure.txt
		for counter in {1..5..1}; 
		do
			# SERVER SIDE
			TBENCH_MAXREQS=${REQUESTS} TBENCH_WARMUPREQS=${WARMUPREQS} ./xapian_networked_server -n ${NSERVERS} -d ${DATA_ROOT}/xapian/wiki -r 1000000000 &
			echo $! > server.pid
			taskset -apc 0,2,4,6,8,10,12,14,16,18,20,22 $(cat server.pid)
			energyStart=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj)
			sleep 5 # Wait for server to come up
			
			# CLIENT SIDE
			TBENCH_QPS=${queryPerSecond} TBENCH_CLIENT_THREADS=12 TBENCH_MINSLEEPNS=50000 TBENCH_TERMS_FILE=${DATA_ROOT}/xapian/terms.in ./xapian_networked_client &
			echo $! > client.pid
			taskset -apc 1,3,5,7,9,11,13,15,17,19,21,23 $(cat client.pid)
			wait $(cat client.pid)
			energyEnd=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj)
			consumption=$((energyEnd - energyStart))
			echo "$counter , $queryPerSecond, $consumption" >> energy-pure.txt

			# Clean up
			./kill_networked.sh
			rm server.pid client.pid
<<<<<<< HEAD
			mv lats.bin ../../raw-results/xapian/final/sjrnTime-on-varQPS/pure-governor@perf-12servers-${counter}-${queryPerSecond}.bin
		done
	mv energy-pure.txt ../../raw-results/xapian/final/sjrnTime-on-varQPS/energy-pure-governor@perf-12servers-${queryPerSecond}.txt
=======
			mv lats.bin ../../raw-results/xapian/final/sjrnTime-on-varQPS/pure-governor@userspace-12servers-${counter}-${queryPerSecond}.bin
		done
	mv energy-pure.txt ../../raw-results/xapian/final/sjrnTime-on-varQPS/energy-pure-governor@userspace-12servers-${queryPerSecond}.txt
>>>>>>> 916f2c8b8c75cc1047828d3364991b2d67f0edc0
done
