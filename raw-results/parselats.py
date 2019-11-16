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

if __name__ == '__main__':
    def getLatPct(latsFile):
        assert os.path.exists(latsFile)
        n = os.path.splitext(latsFile)[0]
        latsObj = Lat(latsFile)
        latsName = n + '.txt'

        lLength = [l for l in latsObj.parseLengthSize()]
        qTimes = [l/1e6 for l in latsObj.parseQueueTimes()]
        svcTimes = [l/1e6 for l in latsObj.parseSvcTimes()]
        sjrnTimes = [l/1e6 for l in latsObj.parseSojournTimes()]
        f = open(latsName,'w')

        f.write('%12s | %12s | %12s | %12s\n\n' \
                % ('KeyLength', 'QueueTimes', 'ServiceTimes', 'SojournTimes'))

        for (le, q, svc, sjrn) in zip(lLength, qTimes, svcTimes, sjrnTimes):
            f.write("%12s | %12s | %12s | %12s\n" \
                    % ('%.3f' % le, '%.3f' % q, '%.3f' % svc, '%.3f' % sjrn))
        f.close()
        p95 = stats.scoreatpercentile(sjrnTimes, 95)
        p99 = stats.scoreatpercentile(sjrnTimes, 99)
        maxLat = max(sjrnTimes)
        print "(Sjrn) 95th percentile latency %.3f ms | 99th percentile latency %.3f ms | max latency %.3f ms" \
                % (p95, p99, maxLat)
        p95 = stats.scoreatpercentile(svcTimes, 95)
        p99 = stats.scoreatpercentile(svcTimes, 99)
        maxLat = max(sjrnTimes)
        print "(Svc) 95th percentile latency %.3f ms | 99th percentile latency %.3f ms | max latency %.3f ms" \
                % (p95, p99, maxLat)


    latsFile = sys.argv[1]
    getLatPct(latsFile)
        
