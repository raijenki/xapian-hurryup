/** $lic$
 * Copyright (C) 2016-2017 by Massachusetts Institute of Technology
 *
 * This file is part of TailBench.
 *
 * If you use this software in your research, we request that you reference the
 * TaiBench paper ("TailBench: A Benchmark Suite and Evaluation Methodology for
 * Latency-Critical Applications", Kasture and Sanchez, IISWC-2016) as the
 * source in any publications that use this software, and that you send us a
 * citation of your work.
 *
 * TailBench is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.
 */

#ifndef __CLIENT_H
#define __CLIENT_H

#include "msgs.h"
#include "msgs.h"
#include "dist.h"

#include <pthread.h>
#include <stdint.h>

#include <string>
#include <unordered_map>
#include <vector>

#include<stdlib.h>
#include<inttypes.h>
#include<queue>

enum ClientStatus { INIT, WARMUP, ROI, FINISHED };

class QPScombo {
    private:
        uint64_t duration;
        double QPS;
    public:
        QPScombo(uint64_t dur, double _QPS) {
            duration = dur;
            QPS = _QPS;
        }
        double getQPS() {return QPS;}
        uint64_t getDuration() {return duration;}
};

class DQPSLookup {
    private:
        std::queue<QPScombo*> QPStiming;
        uint64_t startingNs; //Starting time for curretn QPS period
        bool started;
    public:
        DQPSLookup(); //std::string inputFile);
        std::string inputFile;
        double currentQPS();
        void setStartingNs();
        //int counter_value();
        //void reset_counter_value();
        //int counter= 0;
};


class Client {
    protected:
        ClientStatus status;

        int nthreads;
        pthread_mutex_t lock;
        pthread_barrier_t barrier;

        uint64_t minSleepNs;
        uint64_t seed;
        double lambda;
        double current_qps; //nishtala:current qps
        FILE* fileOut=NULL;
	FILE* fileOut_keyservice=NULL;
        int counter;

        ExpDist* dist;

        uint64_t startedReqs;
        std::unordered_map<uint64_t, Request*> inFlightReqs;
        std::vector<uint64_t> KeyLengths;
        std::vector<uint64_t> svcTimes;
        std::vector<uint64_t> queueTimes;
        std::vector<uint64_t> sjrnTimes;
        //std::vector<double> copy_sjrnTimes;

        void _startRoi();
        DQPSLookup dqpsLookup;
        bool dumped;

    public:
        Client(int nthreads);
        Request* startReq();
        //std::string outputFile;
        //extern FILE* fileOut; 
        void finiReq(Response* resp);//, FILE *fp);
        int startRoi();
        void dumpStats();
        //void periodicDump(FILE *fp);
        bool getDumped();
        //int readfile();
        //int pid_num;

};

class NetworkedClient : public Client {
    private:
        pthread_mutex_t sendLock;
        pthread_mutex_t recvLock;

        int serverFd;
        std::string error;

    public:
        NetworkedClient(int nthreads, std::string serverip, int serverport);
        bool send(Request* req);
        bool recv(Response* resp);
        const std::string& errmsg() const { return error; }
};

#endif
