import parselats
import os

def write(ufreq, cpu, qps, energy):
    p95, p99 = parselats.getLatPct()
    with open("db.txt", 'a') as dbFile:
        usedCpus = int(cpu)/2 + 1
        dbFile.write(str(p95) + ',' + str(p99) + ',' + ufreq + ',' + str(int(usedCpus)) + ',' + str(qps) + ',' + str(energy))
        dbFile.write('\n')
        os.remove('lats.bin')
    dbFile.close()
