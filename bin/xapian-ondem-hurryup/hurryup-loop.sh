#!/bin/bash
<<<<<<< HEAD
 
=======

>>>>>>> 19abdaf68cf2b8ebe9048823865311370d7cd7f9
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/../deps/configs.sh

NSERVERS=10
<<<<<<< HEAD
QPS=1400
WARMUPREQS=1000
REQUESTS=30000
echo "Run , Difference" > energy-hurryup.txt

for counter in {21..30..1}; 
=======
QPS=2500
WARMUPREQS=1000
REQUESTS=30000
echo "Run , Difference" > energy.txt

for counter in {1..30..1}; 
>>>>>>> 19abdaf68cf2b8ebe9048823865311370d7cd7f9
	do
		TBENCH_MAXREQS=${REQUESTS} TBENCH_WARMUPREQS=${WARMUPREQS} chrt -r 99 ./xapian_networked_server -n ${NSERVERS} -d ${DATA_ROOT}/xapian/wiki -r 1000000000 &
		echo $! > server.pid
		energyStart=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj)
		sleep 5 # Wait for server to come up
#taskset -apc 0,2,4,6,8,10,12,14,16,18,20,22 $(cat server.pid)
<<<<<<< HEAD
		sudo wrmsr -a 0x199 0x1000
=======
		sudo wrmsr -a 0x199 0x1200
>>>>>>> 19abdaf68cf2b8ebe9048823865311370d7cd7f9

		TBENCH_QPS=${QPS} TBENCH_CLIENT_THREADS=4 TBENCH_MINSLEEPNS=50000 TBENCH_TERMS_FILE=${DATA_ROOT}/xapian/terms.in chrt -r 99 ./xapian_networked_client &
  
		echo $! > client.pid
#taskset -apc 1,3,5,7 $(cat client.pid)

		wait $(cat client.pid)
		energyEnd=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj)
		consumption=$((energyEnd - energyStart))

		echo "$counter , $consumption" >> energy.txt

# Clean up
		./kill_networked.sh
		rm server.pid client.pid
<<<<<<< HEAD
		mv lats.bin ../../raw-results/07/hurryup-start@1.6ghz-10servers-${counter}.bin
	done
mv energy-hurryup.txt ../../raw-results/07/energy-hurryup-start@1.6ghz-10servers-3.txt
=======
		mv lats.bin ../../raw-results/06/hurryup-start@1.8ghz-10servers-${counter}.bin
	done
mv energy-hurryup.txt ../../raw-results/06/energy-hurryup-start@1.8ghz-10servers.txt
>>>>>>> 19abdaf68cf2b8ebe9048823865311370d7cd7f9
