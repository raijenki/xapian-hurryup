#!/usr/bin/python

import sys
import os
import numpy as np
from scipy import stats

class Lat(object):
    def __init__(self, fileName):
        f = open(fileName, 'rb')
        a = np.fromfile(f, dtype=np.uint64)
        self.reqTimes = a.reshape((a.shape[0]/4, 4))
        f.close()

    def parseLengthSize(self):
        return self.reqTimes[:, 0]

    def parseQueueTimes(self):
        return self.reqTimes[:, 1]

    def parseSvcTimes(self):
        return self.reqTimes[:, 2]

    def parseSojournTimes(self):
        return self.reqTimes[:, 3]


def getLatPct(latsFile):
    assert os.path.exists(latsFile)
    n = os.path.splitext(latsFile)[0]
    latsObj = Lat(latsFile)

    lLength = [l for l in latsObj.parseLengthSize()]
    qTimes = [l/1e6 for l in latsObj.parseQueueTimes()]
    svcTimes = [l/1e6 for l in latsObj.parseSvcTimes()]
    sjrnTimes = [l/1e6 for l in latsObj.parseSojournTimes()]
    
    p95 = stats.scoreatpercentile(sjrnTimes, 95)
    p99 = stats.scoreatpercentile(sjrnTimes, 99)
    maxLat = max(sjrnTimes)
    print(p95)
    return p95
        
