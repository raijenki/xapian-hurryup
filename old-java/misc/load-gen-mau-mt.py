# RUN Scheduler

# load generator for Elasticsearch
# Copyright 2017 - Vinicius Petrucci @ UFBA

import commands
import sys
from random import randint
import numpy as np
import csv
import time
from multiprocessing import Process, Manager
import requests
import signal


def handler_stop_signals(signum, frame):
    global running
    running = False

signal.signal(signal.SIGINT, handler_stop_signals)
signal.signal(signal.SIGTERM, handler_stop_signals)

def read_file(filename):
    l = []
    with open(filename, "r") as myFile:
        l = myFile.readlines()
    return l

# Gets the query and return the time it took to process
def took_time(search):
  i = search.find("took")
  search = search[i+6:]
  j = search.find(",")
  return int(search[:j])


# Creates the query of words based on the list file
def getQuery(l):
  randomCard = randint(0, 100)
  termsSize = len(l)
  query = l[randint(0, termsSize-1)].rstrip('\n')
  num_items_query = 1
  if randomCard > 23:
    query = query + '+' + l[randint(0, termsSize-1)].rstrip('\n')
    num_items_query = num_items_query + 1
    if randomCard > 47:
      query = query + '+' + l[randint(0, termsSize-1)].rstrip('\n')
      num_items_query = num_items_query + 1
      if randomCard > 69:
        query = query + '+' + l[randint(0, termsSize-1)].rstrip('\n')
        num_items_query = num_items_query + 1
        if randomCard > 83:
          query = query + '+' + l[randint(0, termsSize-1)].rstrip('\n')
          num_items_query = num_items_query + 1
          if randomCard > 88:
            query = query + '+' + l[randint(0, termsSize-1)].rstrip('\n')
            num_items_query = num_items_query + 1
            if randomCard > 92:
              query = query + '+' + l[randint(0, termsSize-1)].rstrip('\n')
              num_items_query = num_items_query + 1
              if randomCard > 95:
                rnd = randint(1, 10)
                num_items_query = num_items_query + rnd
                for i in range (rnd):
                  query = query + '+' + l[randint(0, termsSize-1)].rstrip('\n')

  #query = "curl -XGET 'http://192.168.1.2:9200/wiki/_search?q=" + query + "'"
  payload = {'q': query}
  r = requests.get('http://'+IP+':9200/wiki/_search', params=payload)
  return r.text.encode('utf-8').strip(), num_items_query


list_of_words    = read_file("terms_en.out")

def process_query(d):
    #for j in range(number_of_queries):
    while running:

        query, num_items_query = getQuery(list_of_words)
        #time the query was processed
        took_value = took_time(query)

        #print "%d,%d,%d" % (time.time(),num_items_query,took_value)
        t = int(time.time())

        if not t in d: d[t] = [(num_items_query,took_value)]
        else: d[t] += [(num_items_query,took_value)]

#        if not t in d: d[t] = [took_value]
#        else: d[t] += [took_value]

        #t2 = time.time()

        #time.sleep(1-(t2-t1))
#    print 'exiting...'

if len(sys.argv) == 3:
    num_threads = int(sys.argv[1])
    IP = str(sys.argv[2])
else:
    num_threads = 1
    IP = '127.0.0.1'

running = True
proc_list = []

manager = Manager()
req_tab = manager.dict()

for i in range(num_threads):
  p = Process(target=process_query, args=(req_tab,))
  proc_list += [p]

for p in proc_list:
  p.start()

for p in proc_list:
  p.join()


print 'timestamp,kw_lengh,took_ms'

for k,v in req_tab.items():
  for j in v:
    print str(k)+','+str(j[0])+','+str(j[1])
