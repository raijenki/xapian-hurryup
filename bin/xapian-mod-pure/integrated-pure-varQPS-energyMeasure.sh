#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/../deps/configs.sh

NSERVERS=7
PROCESSTIME=60
WARMUPTIME=5

for cpus in cpu14 cpu16 cpu18 cpu20 cpu22 
do
	sudo echo 0 | sudo tee /sys/devices/system/cpu/${cpus}/online
done

for cpus in cpu12 cpu14 cpu16 cpu18 cpu20 cpu22
do
	sudo echo 1 | sudo tee /sys/devices/system/cpu/${cpus}/online

	for frequencyStep in 1.0GHz 1.2GHz 1.3GHz 1.5GHz 1.7GHz 1.9GHz 2.1GHz 2.3GHz 2.5GHz
	do
		sudo cpupower frequency-set -f ${frequencyStep}

		for queryPerSecond in {1200..6000..2400}
		do
		REQUESTS=$((queryPerSecond * PROCESSTIME))
		WARMUPREQS=$((WARMUPTIME * queryPerSecond))
		echo "Run, qps, Difference" > energy-pure-${cpus}.txt
			for counter in {1..3..1}
			do
			cat /sys/devices/system/cpu/cpu0/cpufreq/stats/time_in_state > /home/cc/hurryup-dvfs/results/core-augmentation/tis-pure-governor@userspace-${frequencyStep}-12servers-${counter}-${queryPerSecond}-start-${cpus}.txt
			energyStart=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj)
	TBENCH_QPS=${queryPerSecond} TBENCH_CLIENT_THREADS=12 TBENCH_MAXREQS=${REQUESTS} TBENCH_WARMUPREQS=${WARMUPREQS} \
		TBENCH_MINSLEEPNS=100000 TBENCH_TERMS_FILE=${DATA_ROOT}/xapian/terms.in \
       		numactl --cpunodebind=1 ./xapian_integrated -n ${NSERVERS} -d ${DATA_ROOT}/xapian/wiki -r 1000000000
			echo $! > server.pid
			energyEnd=$(cat /sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj)
			cat /sys/devices/system/cpu/cpu0/cpufreq/stats/time_in_state > /home/cc/hurryup-dvfs/results/core-augmentation/tis-pure-governor@userspace-${frequencyStep}-12servers-${counter}-${queryPerSecond}-end-${cpus}.txt
			consumption=$((energyEnd - energyStart))
			echo "$counter, $queryPerSecond, $consumption" >> energy-pure-${cpus}.txt
			python parselats.py lats.bin
			mv lats.txt ../../results/core-augmentation/pure-governor@userspace-${frequencyStep}-12servers-${counter}-${queryPerSecond}-${cpus}.txt
			rm server.pid
			done
		mv energy-pure-${cpus}.txt ../../results/core-augmentation/energy-pure-governor@userspace-${frequencyStep}-12servers-${queryPerSecond}-${cpus}.txt
		done
	done
	NSERVERS=$((NSERVERS+1))
done
#taskset -apc 12 $(cat server.pid)

