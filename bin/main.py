
import numpy as np
import os
import db
import scipy as sp
import sys
import psutil
import aux
import tailbench

sys.path.append('/home/cc/hurryup-dvfs/bin')

ufreqs = [ '1.0GHz', '1.2GHz', '1.3GHz', '1.5GHz', '1.7GHz', '1.9GHz', '2.1GHz', '2.3GHz', '2.5GHz' ]
cpus = [ 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22 ]
qps = [ 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000 ]
ht = 'off'

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
