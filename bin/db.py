import parselats
import os

def write(ufreq, cpu, qps, energy):
    p95 = parselats.getLatPct(lats.bin)
    with open("db.txt", 'a') as dbFile:
        usedCpus = int(cpu)/2 + 1
        dbFile.write(str(p95) + ',' + ufreq + ',' str(cpu) + ',' str(qps) + ',' + str(energy))
        dbFile.write('\n')
        os.sys('rm lats.bin')
    dbFile.close()
