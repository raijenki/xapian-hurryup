# RUN Scheduler

# thread level scheduler for elasticsearch
# Copyright 2017 - Vinicius Petrucci @ UFBA

import os
import sys
import subprocess
import time
import commands
import random
import numa

if len(sys.argv) == 5:
  sched_enable = int(sys.argv[1])
  sched_sleep = int(sys.argv[2])
  sched_thres = int(sys.argv[3])
  es_path = sys.argv[4]
else:
  print 'need 4 parameters'
  sys.exit(1)

JAVA_PATH = "/mnt/jdk1.8.0_73/bin/"

IP='192.168.0.189'

MAX_CORES = int(commands.getoutput("grep -c processor /proc/cpuinfo"))
#MAX_CORES = 2

proc = subprocess.Popen(es_path + ' 1>/dev/null', shell=True, stderr=subprocess.PIPE)

time.sleep(45)

os.system("curl -XGET 'http://"+IP+":9200/wiki/_search?q=test+car+house+truck' 2>/dev/null 1>/dev/null")
os.system("curl -XGET 'http://"+IP+":9200/wiki/_search?q=test+car+house+truck' 2>/dev/null 1>/dev/null")
os.system("curl -XGET 'http://"+IP+":9200/wiki/_search?q=test+car+house+truck' 2>/dev/null 1>/dev/null")

time.sleep(2)

ret, cmd_out = commands.getstatusoutput(JAVA_PATH+"jps | grep Elasticsearch | cut -f1 -d' '")
ret, cmd_out = commands.getstatusoutput(JAVA_PATH+"jstack " + cmd_out.strip() + " |  egrep '[[]search[]]'")

print 'cmd_out', cmd_out

thread_pids = {}
thread_cores = {}
core_threads = {}

tid_list = []

req_tab = {}

big_cores = [0,1,2,3]
small_cores = [4,5,6,7]

def core_type(core):
  if core >= big_cores[0] and core <= big_cores[-1]: return "big"
  elif core >= small_cores[0] and core <= small_cores[-1]: return "LITTLE"

def thread_map(tid, cid):
  pid = thread_pids[tid]
  numa.set_affinity(pid, [cid])
  thread_cores[tid] = cid
  core_threads[cid] = tid
#  print 'map', tid,'to',cid

cur_ms_time = lambda: int(round(time.time() * 1000))

c = 0
for line in cmd_out.split('\n'):
  nid = line.split('nid=')[1].split(' ')[0]
  pid = int(eval(nid))
  tid = int(line.split(' daemon')[0].split(' ')[-1][1:])
  #print tid, pid
  thread_pids[tid] = pid
  numa.set_affinity(pid, [c])
  thread_cores[tid] = c
  core_threads[c] = tid
#  os.system('taskset -cp ' + str(c) + ' ' + str(pid))
  tid_list += [tid]
  c = (c + 1) % MAX_CORES

print 'thread_cores', thread_cores

ts1 = cur_ms_time()

if sched_enable == 0:
  proc.wait()

if sched_enable > 0:
  output = file('/mnt/ram/es.out','w')
  output.write(str(thread_cores)+"\n")

while True:

    line = proc.stderr.readline()

    if sched_enable > 0:
      output.write(line)
      output.flush()

    if sched_enable == 2:
      continue

 #   print 'line', line

    cols = line.split(';')
#    print 'tid', cols[0], 'rid', cols[1], 'ts', cols[2]
    rid = cols[1]
    if rid in req_tab:
      cur_ts = int(cols[2])
  #    print 'rid', rid, 'took', cur_ts - req_tab[rid][1]
#      print 'deleting', rid
      del req_tab[rid]
    else:
      req_tab[rid] = (int(cols[0]), int(cols[2]))
 #     print 'adding', rid, 'value', req_tab[rid]

    if True:
#    if (cur_ms_time() - ts1) >= sched_sleep:
      #most_pending_time = 0
      tids_on_little = []
      for k,v in req_tab.items():
        pending_time = cur_ms_time() - v[1]
        if pending_time > sched_thres:
          #print 'tid:', v[0], 'pending:', pending_time,'running on core type', core_type(thread_cores[v[0]])
          # is on little?
          if thread_cores[v[0]] >= small_cores[0]:
            tids_on_little.append((pending_time,v[0]))

      tids_on_little.sort(reverse=True)
  #    print 'tids_on_little', tids_on_little
      #b = random.choice([0,1,2,3])
      #for t in tids_on_little:
      for i in range(len(big_cores)):
         if i >= len(tids_on_little): break
         b = big_cores[i]
         t = tids_on_little[i][1]
         old_core = thread_cores[t]
         t2 = core_threads[b]
         thread_map(t, b)
         thread_map(t2, old_core)
#         print 'map', t,'to',b
 #        print 'map',t2,'to',old_core
    #     b = (b+1) % 3

      if len(tids_on_little) > 0:
        output.write(str(thread_cores)+"\n")

      ts1 = cur_ms_time()

  #time.sleep(0.1)
