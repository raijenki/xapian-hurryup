import os
import psutil
import sys

# Workaround script to active/deactive hyperthreading and cpus for Intel Skylake

def htcontrol(turn):
    if(turn == 'on'):
        for i in range(psutil.cpu_count(logical=False), (psutil.cpu_count(logical=False)*2)):
            filePath = "/sys/devices/system/cpu/cpu" + str(i) + "/online"    
            with open(filePath, 'w') as cpuFile: 
                cpuFile.write(str(1))
                print("[SYS/cpu] Enabled core " +  str(i))
    if(turn == 'off'):
        for i in range(psutil.cpu_count(logical=False), psutil.cpu_count(logical=True)):
            filePath = "/sys/devices/system/cpu/cpu" + str(i) + "/online"    
            with open(filePath, 'w') as cpuFile: 
                cpuFile.write(str(0))
                print("[SYS/cpu] Disabling core " +  str(i))

            
def energy():
    energyPath = "/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj"
    with open(energyPath, 'r') as energyFile:
        value = int(energyFile.read())
    return value

def disableall():
    for i in range(1, psutil.cpu_count(logical=False)):
        if(i%2 == 0):
            filePath = "/sys/devices/system/cpu/cpu" + str(i) + "/online"    
            with open(filePath, 'w') as cpuFile: 
                cpuFile.write(str(0))
                print("[SYS/cpu] Disabling core " +  str(i))

def enable(cpu):
    filePath = "/sys/devices/system/cpu/cpu" + str(int(cpu)) + "/online"    
    with open(filePath, 'w') as cpuFile: 
        cpuFile.write(str(1))
        print("[SYS/cpu] Enabling core " +  str(int(cpu)))

