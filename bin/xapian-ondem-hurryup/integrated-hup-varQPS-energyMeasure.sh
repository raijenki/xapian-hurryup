#!/bin/bash
# Test
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/../deps/configs.sh

NSERVERS=12
PROCESSTIME=60
WARMUPTIME=5

for queryPerSecond in {1200..6000..2400}
do
	REQUESTS=$((queryPerSecond * PROCESSTIME))
	WARMUPREQS=$((WARMUPTIME * queryPerSecond))
	echo "Run, qps, Difference" > energy-hup.txt
	for counter in {1..1..1}
	do
	cat /sys/devices/system/cpu/cpu0/cpufreq/stats/time_in_state > /home/cc/hurryup-dvfs/raw-results/time_in_state/tis-hup-gov@varFreq-${counter}-${queryPerSecond}-sleep3000-start.txt
	energyStart=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj)
	TBENCH_QPS=${queryPerSecond} TBENCH_CLIENT_THREADS=12 TBENCH_MAXREQS=${REQUESTS} TBENCH_WARMUPREQS=${WARMUPREQS} \
		TBENCH_MINSLEEPNS=100000 TBENCH_TERMS_FILE=${DATA_ROOT}/xapian/terms.in \
       		numactl --cpunodebind=1 ./xapian_integrated -n ${NSERVERS} -d ${DATA_ROOT}/xapian/wiki -r 1000000000
	echo $! > server.pid
	energyEnd=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj)
	cat /sys/devices/system/cpu/cpu0/cpufreq/stats/time_in_state > /home/cc/hurryup-dvfs/raw-results/time_in_state/tis-hup-gov@varFreq-${counter}-${queryPerSecond}-sleep3000-end.txt
	consumption=$((energyEnd - energyStart))
	echo "$counter, $queryPerSecond, $consumption" >> energy-hup.txt
	python parselats.py lats.bin
	mv lats.txt ../../raw-results/time_in_state/hup-gov@varFreq-${counter}-${queryPerSecond}-sleep2000.txt
	rm server.pid
	done
	mv energy-hup.txt ../../raw-results/time_in_state/energy-hup-gov@varFreq-${queryPerSecond}-sleep2000.txt
done
#taskset -apc 12 $(cat server.pid)

