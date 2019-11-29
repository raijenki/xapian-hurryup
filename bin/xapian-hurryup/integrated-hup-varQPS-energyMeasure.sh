#!/bin/bash
# Test
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/../deps/configs.sh

NSERVERS=12
PROCESSTIME=60
WARMUPTIME=5

for queryPerSecond in {2400..3600..1200}
do
	REQUESTS=$((queryPerSecond * PROCESSTIME))
	WARMUPREQS=$((WARMUPTIME * queryPerSecond))
	echo "Run, qps, Difference" > energy-hup.txt
	for counter in {1..1..1}
	do
	cat /sys/devices/system/cpu/cpu0/cpufreq/stats/time_in_state > /home/cc/hurryup-dvfs/raw-results/ondemand-mod/tis-hup-gov@project-${counter}-${queryPerSecond}-start.txt
	energyStart=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj)
	TBENCH_QPS=${queryPerSecond} TBENCH_CLIENT_THREADS=12 TBENCH_MAXREQS=${REQUESTS} TBENCH_WARMUPREQS=${WARMUPREQS} \
		TBENCH_MINSLEEPNS=100000 TBENCH_TERMS_FILE=${DATA_ROOT}/xapian/terms.in \
       		numactl --cpunodebind=1 ./xapian_integrated -n ${NSERVERS} -d ${DATA_ROOT}/xapian/wiki -r 1000000000
	echo $! > server.pid
	energyEnd=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj)
	cat /sys/devices/system/cpu/cpu0/cpufreq/stats/time_in_state > /home/cc/hurryup-dvfs/raw-results/ondemand-mod/tis-hup-project@ondem-${counter}-${queryPerSecond}-end.txt
	consumption=$((energyEnd - energyStart))
	echo "$counter, $queryPerSecond, $consumption" >> energy-hup.txt
	sudo python parselats.py lats.bin
	mv lats.txt ../../raw-results/ondemand-mod/hup-gov@project-${counter}-${queryPerSecond}.bin
	rm server.pid
	done
	cat energy-hup.txt
	mv energy-hup.txt ../../raw-results/ondemand-mod/energy-hup-gov@project-${queryPerSecond}.txt
done
#taskset -apc 12 $(cat server.pid)

