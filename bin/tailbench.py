import subprocess
import aux

def xapian(icpu, qps):
    nservers = icpu/2 + 1
    cmdLine = "TBENCH_QPS=" + str(qps) + " TBENCH_CLIENT_THREADS=12" + " TBENCH_MAXREQS=" + str(qps*60) + " TBENCH_WARMUPREQS=" + str(qps*0.2) + " TBENCH_MINSLEEPNS=100000 TBENCH_TERMS_FILE=/home/cc/tailbench.inputs/xapian/terms.in " + "numactl --cpunodebind=1 ./xapian_integrated -n " + str(nservers) + " -d /home/cc/tailbench.inputs/xapian/wiki -r 1000000000"
    print(cmdLine)
    energyStart = aux.energy()
    process = subprocess.Popen(cmdLine, shell=True, stdout=subprocess.PIPE)
    process.wait()
    energyEnd = aux.energy()
    consumed = energyStart - energyEnd
    return consumed
