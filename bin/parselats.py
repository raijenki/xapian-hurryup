#!/usr/bin/python

import sys
import os
import numpy as np
from scipy import stats

def getLatPct():
    latsFile = '/home/cc/hurryup-dvfs/bin/lats.bin'
    assert os.path.exists(latsFile)
    f = open(latsFile, 'rb')
    a = np.fromfile(f, dtype=np.uint64)
    f.close()
    reqTimes = a.reshape((a.shape[0]//4, 4))
    p95 = stats.scoreatpercentile(reqTimes[:, 3]/1e6, 95)
    p99 = stats.scoreatpercentile(reqTimes[:, 3]/1e6, 99)
    return p95, p99
        
