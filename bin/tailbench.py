import subprocess
import helper
import time 
import os 
import signal

def xapian(icpu, qps):
    nservers = 10
    cmdLine_srvr = "TBENCH_MAXREQS=" + str(int(qps*30)) + " TBENCH_WARMUPREQS=200 numactl --cpunodebind=0 ./xapian_db_server -n " + str(int(nservers)) + " -d /home/cc/inputs/xapian/wiki -r 1000000000"
    print(cmdLine_srvr)
    process = subprocess.Popen(cmdLine_srvr, shell=True, stdout=subprocess.PIPE)
    time.sleep(5)
    cmdLine_clnt = "TBENCH_QPS=" + str(int(qps)) + " TBENCH_CLIENT_THREADS=8 TBENCH_MINSLEEPNS=100000 TBENCH_TERMS_FILE=/home/cc/inputs/xapian/terms.in numactl --cpunodebind=1 ./xapian_db_client"
    client = subprocess.Popen(cmdLine_clnt, shell=True, stdout=subprocess.PIPE)
    energyStart = aux.energy()
    process.wait()
    energyEnd = aux.energy()
    #time.sleep(5)
    consumed = energyEnd - energyStart
    return consumed
