import csv
import time
import subprocess
import numpy as np
import os
import db
import scipy as sp
import sys
import psutil
import aux
import tailbench

sys.path.append('/home/cc/hurryup-dvfs/bin')

# Frequencies for the Skylake Processor
ufreqs = [ '1.0GHz', '1.2GHz', '1.3GHz', '1.5GHz', '1.7GHz', '1.9GHz', '2.1GHz', '2.3GHz', '2.5GHz' ]
# Even CPUs for server-side processing (integrated mode)
cpus = [ 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22 ]
# QPS tests
qps = [ 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000 ]
# Hyperthreading
ht = 'off'

def xapian_sched():
    # Set environment variables
    os.environ["TBENCH_INPUT"] = "/home/cc/hurryup-dvfs/bin/input.test"
    os.environ["TBENCH_OUTPUT"] = "home/cc/hurryup-dvfs/bin/output.txt"
    os.environ["TBENCH_CPID"] = "/home/cc/hurryup-dvfs/xapians/xapian-pure/server.pid"
    os.environ["TBENCH_KEYSERVICE"] = "/home/cc/hurryup-dvfs/bin/keyservice.txt"
    os.environ["TBENCH_QPS"] = "5000"
    os.environ["TBENCH_MINSLEEPNS"] = "100000"
    os.environ["TBENCH_TERMS_FILE"] = "/home/cc/tailbench.inputs/xapian/terms.in"
    os.environ["TBENCH_RANDSEED"] = "3"
    os.environ["TBENCH_MAXREQS"] = "200000"
    os.environ["TBENCH_WARMUPREQS"] = "100"
    os.environ["TBENCH_CLIENT_THREADS"] = "10"
    
    # Set the command line to start Mod-Xapian
    nservers = 12
    cmdLine = "./xapian_integrated -n " + str(int(nservers)) + " -d ${DATA_ROOT}/xapian/wiki -r 1000000000"
    process = subprocess.Popen(cmdLine, shell=True, stdout=subprocess.PIPE)
    # Sleep for 4 seconds while it's greenlighting the server
    time.sleep(4)

    # Scheduler
    while(process): 
        with open(os.getenv("TBENCH_OUTPUT")) as csvfile:
            csvReader = csv.reader(csvfile, delimiter=',')
            lines = len(list(reader))
            print("Number of lines now is: " + lines)
        
        time.sleep(1)


def xapian_simul():
    if(psutil.cpu_count(logical = True) != psutil.cpu_count(logical=False)):
        print('[SYS] Hyperthreading is enabled, disabling...')
        aux.htcontrol(ht)
        print('[SYS] Hyperthreading disabled!')
    for ufreq in ufreqs:
        print('[SYS] Disabling all CPUs for new run, except the first one...')
        aux.disableall()
        freqAdjust = "sudo cpupower frequency-set -f " + ufreq
        os.system(freqAdjust)
        for icpu in cpus:
            if(icpu != 0):
                aux.enable(icpu)
            for iqps in qps:
                print("[SYS] Running Xapian for " + str(icpu) + "cpus, " + str(iqps) + " qps " + " and frequency " + ufreq) 
                consumedEnergy = tailbench.xapian(icpu, iqps)
                db.write(ufreq, icpu, iqps, consumedEnergy)

if __name__ == "__main__":
    if(sys.argv[1] == 'xapian_simul'):
        xapian_simul()
    if(sys.argv[1] == 'xapian_sched'):
        xapian_sched()
