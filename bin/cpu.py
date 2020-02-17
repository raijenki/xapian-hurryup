import os
import psutil
import sys

# Workaround script to active/deactive hyperthreading and cpus for Intel Skylake

def ht_control(status):
    if(status == 'on'):
        for i in range(psutil.cpu_count(logical=False), (psutil.cpu_count(logical=False)*2)):
            filePath = "/sys/devices/system/cpu/cpu" + str(i) + "/online"    
            with open(filePath, 'w') as cpuFile: 
                cpuFile.write(str(1))
                print("[SYS/cpu] Enabled core " +  str(i))
            cpuFile.close()
    if(status == 'off'):
        for i in range(psutil.cpu_count(logical=False), psutil.cpu_count(logical=True)):
            filePath = "/sys/devices/system/cpu/cpu" + str(i) + "/online"    
            with open(filePath, 'w') as cpuFile: 
                cpuFile.write(str(0))
                print("[SYS/cpu] Disabling core " +  str(i))
            cpuFile.close()
def energy():
    energyPath = "/sys/class/powercap/intel-rapl/intel-rapl\:0/energy_uj"
    with open(energyPath, 'r') as energyFile:
        value = int(energyFile.read())
    energyFile.close()
    return value

def disableall():
    for i in range(1, psutil.cpu_count(logical=False)):
        if(i%2 == 0):
            filePath = "/sys/devices/system/cpu/cpu" + str(i) + "/online"    
            with open(filePath, 'w') as cpuFile: 
                cpuFile.write(str(0))
                print("[SYS/cpu] Disabling core " +  str(i))
            cpuFile.close()

def enable(cpu):
    filePath = "/sys/devices/system/cpu/cpu" + str(cpu) + "/online"    
    with open(filePath, 'w') as cpuFile: 
        cpuFile.write(str(0))
        print("[SYS/cpu] Enabling core " +  str(i))
    cpuFile.close()



if __name__ == "__main__":
    ht_control(sys.argv[1])

